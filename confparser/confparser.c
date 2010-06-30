#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h> 

#include "iniparser.h"
#include "confparser.h"
#ifndef MAX_PATH
#define MAX_PATH 1024
#endif

char conf_file[MAX_PATH];

void set_conf_file(const char *conf_name)
{
	if (conf_name != NULL)
		strncpy(conf_file, conf_name, MAX_PATH - 1);
}

dictionary *
open_conf_file(const char *conf_name)
{
	char *cn = NULL;

	if (conf_name != NULL)
		cn = (char *)conf_name;
	else
		cn = conf_file;

    return iniparser_load((char *)cn);
}

int 
parse_conf_file(dictionary *conf,
				const char *mod_name,
                CONF_INT_CONFIG cic[], 
                CONF_STR_CONFIG csc[])
{
    /* Some temporary variables to hold query results */
    int i;
    char *s;
	char buf[CONF_ITEM_LEN + 1];
    const CONF_INT_CONFIG* cicp;
	const CONF_STR_CONFIG* cscp;

	if (conf == NULL || mod_name == NULL)
		return (-1);

	/* Get the int configure */
    for (cicp = cic;
        (cicp != NULL) && (cicp->config_name != 0);
        cicp++) {
		snprintf(buf, CONF_ITEM_LEN, "%s:%s", mod_name, cicp->config_name);
        i = iniparser_getint(conf, (char *)(buf), -1);
         
        if (i != -1) {
            *(cicp->var) = i;
        }
    }

	/* Get the string configure */
    for (cscp = csc;
        (cscp != NULL) && (cscp->config_name != 0);
        cscp++) {
		snprintf(buf, CONF_ITEM_LEN, "%s:%s", mod_name, cscp->config_name);
        s = iniparser_getstr(conf, (char *)(buf));
        if (s) {
            strncpy(cscp->var, s, CONF_ITEM_LEN);
        }
    }
    return (0);
}

void
close_conf_file(dictionary *conf)
{
    iniparser_freedict(conf);
}

int
load_conf(char *conf_name, const char *mod_name, CONF_INT_CONFIG cic[],
                CONF_STR_CONFIG csc[])
{
	dictionary	*conf;

	conf = open_conf_file(conf_name);
	if (conf == NULL) {
		return (-1);
	}
	
	parse_conf_file(conf, mod_name, cic, csc);
	close_conf_file(conf);

	return (0);
}

