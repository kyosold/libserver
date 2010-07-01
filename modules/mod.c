#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include "../confparser/confparser.h"
#include "mod.h"

struct http_t
{
	char get[1024];
	char host[25];
	char connection[20];
	char agent[1024];
	char cache[100];
	char accept[1024];
	char encode[100];
	char language[100];
	char charset[100];
};

int create_response(conn *c, int code, char *str, char *pout);

int TYPE;
char USER[1024];
char PASS[1024];

static struct conf_int_config conf_int_array[] = {
	{"type", &TYPE},
	{0, 0}
};

static struct conf_str_config conf_str_array[] = {
	{"user", USER},
	{"pass", PASS},
	{0, 0}
};

static int init_config(void)
{
	dictionary *conf;
	conf = open_conf_file(NULL);
	if (conf==NULL) {
		fprintf(stderr, "cannot parse file\n");
		return (-1);
	}

	// parse 'server' section
	parse_conf_file(conf, "server", conf_int_array, conf_str_array);

	close_conf_file(conf);

	return (0);
}

int mod_init(const char * conf)
{
	set_conf_file(conf);  
	if (init_config() != 0)
		return (-1);
	
	return (0);
}

int mod_conn_init(conn *c)
{
	c->req_buf = (struct http_t *)calloc(1, sizeof(struct http_t));
	if (!c->req_buf) {
		return 1;
	}
	c->res_buf = (char *)calloc(1, 1024*1024);
	if (!c->res_buf) {
		return 1;
	}
	return 0;
}

void mod_conn_clear(conn *c)
{
	if (c->req_buf) {
		free(c->req_buf);
		c->req_buf = NULL;
	}
	if (c->res_buf) {
		free(c->res_buf);
		c->res_buf = NULL;
	}
}


void mod_recv(conn *c, char *buf, size_t buf_len)
{
	//fprintf(stderr, "config:\ntype:%d\nuser:%s\npass:%s\n", TYPE, USER, PASS);

	struct http_t *REQ = (struct http_t *)c->req_buf;
	if (buf_len > 2) {
		//fprintf(stderr, "[%d:%d]%s\n", buf_len, strlen(buf), buf);
		char *key = memchr(buf, ' ', strlen(buf));
		if (key) {
			if (*(key-1) == ':') {
				if (strncasecmp(buf, "host", (key - buf) - 2) == 0) {
					memcpy(REQ->host, key+1, strlen(key+1));
					REQ->host[strlen(key+1)] = '\0';
				} else if (strncasecmp(buf, "connection", (key - buf) - 2) == 0) {
					memcpy(REQ->connection, key+1, strlen(key+1));
					REQ->connection[strlen(key+1)] = '\0';
				} else if (strncasecmp(buf, "user-agent", (key - buf) - 2) == 0) {
					memcpy(REQ->agent, key+1, strlen(key+1));
					REQ->agent[strlen(key+1)] = '\0';
				} else if (strncasecmp(buf, "cache-control", (key - buf) - 2) == 0) {
					memcpy(REQ->cache, key+1, strlen(key+1));
					REQ->cache[strlen(key+1)] = '\0';
				} else if (strncasecmp(buf, "accept", (key - buf) - 2) == 0) {
					memcpy(REQ->accept, key+1, strlen(key+1));
					REQ->accept[strlen(key+1)] = '\0';
				} else if (strncasecmp(buf, "accept-encoding", (key - buf) - 2) == 0) {
					memcpy(REQ->encode, key+1, strlen(key+1));
					REQ->encode[strlen(key+1)] = '\0';
				} else if (strncasecmp(buf, "accept-language", (key - buf) - 2) == 0) {
					memcpy(REQ->language, key+1, strlen(key+1));
					REQ->language[strlen(key+1)] = '\0';
				} else if (strncasecmp(buf, "accept-charset", (key - buf) - 2) == 0) {
					memcpy(REQ->charset, key+1, strlen(key+1));
					REQ->charset[strlen(key+1)] = '\0';
				}
			} else {
				if (buf[0] == 'G' && buf[1] == 'E' && buf[2] == 'T') {
					memcpy(REQ->get, key+1, strlen(key+1));
					REQ->get[strlen(key+1)] = '\0';
				}
			}
		}
		return;
	}

	char *pout = c->res_buf;	
	char *line = REQ->get;
	char *tok = memchr(line, ' ', strlen(line));
	if (tok) {
		*tok = '\0';
	}

	size_t nline = strlen(line);

	if (line[0] == '/')
		line++;

	char *data = memchr(line, '?', nline);
	if (!data) {
		create_response(c, 200, "faid", pout);
	}

	if (strncasecmp(line, "get", data - line) == 0) {
		// get
		data++;
		char key[100];
		char *tok = strtok(data, "&");
		while (tok) {
			char *v = memchr(tok, '=', strlen(tok));
			if (v) {
				if (strncasecmp(tok, "key", (v-tok)-1) == 0) {
					memcpy(key, v+1, strlen(v+1));
					key[strlen(v+1)] = '\0';
				}
			}
			tok = strtok(NULL, "&");
		}
		fprintf(stderr, "get:%s\n", key);

		char *result = get_cache(c, key, strlen(key));
		if (result) {
			char res[strlen(result)];
			memcpy(res, result, strlen(result));
			res[strlen(result)] = '\0';

			free_cache(result);

			create_response(c, 200, res, pout);
		} else {
			create_response(c, 200, "NULL", pout);
		}
	} else if (strncasecmp(line, "set", data - line) == 0) {
		// set
		data++;
		char key[100];
		char val[1024];
		char *tok = strtok(data, "&");
		while (tok) {
			char *v = memchr(tok, '=', strlen(tok));
			if (v) {
				if (strncasecmp(tok, "key", (v-tok)-1) == 0) {
					memcpy(key, v+1, strlen(v+1));
					key[strlen(v+1)] = '\0';
				} else if (strncasecmp(tok, "val", (v-tok)-1) == 0) {
					memcpy(val, v+1, strlen(v+1));
					val[strlen(v+1)] = '\0';
				}
			}
			tok = strtok(NULL, "&");
		}
		if (set_cache(c, key, strlen(key), 0, 0, val, strlen(val)) == STORED) {
			create_response(c, 200, "set succ", pout);
			fprintf(stderr, "succ set:[%s]%s\n", key, val);
		} else {
			create_response(c, 200, "set fail", pout);
			fprintf(stderr, "fail set:[%s]%s\n", key, val);
		}
	}

	fprintf(stderr, "out:\n%s\n", pout);
	if (out_client(c, pout) == TRANSMIT_COMPLETE) {
		fprintf(stderr, "send ok, close\n");
		conn_set_state(c, conn_closing);
	}

	//conn_set_state(c, conn_closing);

	return;
}

int create_response(conn *c, int code, char *str, char *pout)
{
	struct http_t *REQ = (struct http_t *)c->req_buf;

	size_t len = 0;
	size_t nlen = 0;
	char out[1024*9];
	size_t olen = 0;
	olen = sprintf(out, "request:<br>host:%s<br>get:%s<br><br>type:%d<br>user:%s<br>pass:%s<br><br>%s", REQ->host, REQ->get, TYPE, USER, PASS, str);

	len = sprintf(pout, "HTTP/1.1 %d OK\r\n", code);
	pout += len;
	nlen += len;
	len = sprintf(pout, "Date: Fri, 11 Jun 2010 06:30:24 GMT\r\n");
	pout += len;
	nlen += len;
	len = sprintf(pout, "Content-Length: %d\r\n", olen);
	pout += len;
	nlen += len;
	len = sprintf(pout, "Content-Type: text/html;charset=gb2312\r\n");
	pout += len;
	nlen += len;
	len = sprintf(pout, "\r\n");
	pout += len;
	nlen += len;
	len = sprintf(pout, "%s", out);
	pout += len;
	nlen += len;
	len = sprintf(pout, "\r\n");
	pout += len;
	nlen += len;

	return nlen;
}

