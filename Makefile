CC =	gcc
CFLAGS =	-g -std=gnu99 -DHAVE_CONFIG_H #-O2 -fprofile-arcs -ftest-coverage -pthread -Wall -Werror -pedantic -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls 
INCS =	-I/usr/local/libevent1.4.6/include -I./confparser/ 
LBS =	-L/usr/local/libevent1.4.6/lib -levent -lpthread -ldl -lconfparser
CPP =	gcc -E
LINK =	$(CC) $(CFLAGS) #$(INCS) $(LBS)

#PLUGS_SRC =	./modules/ldap/module_ldap.c 
#PLUGS_INCS =	-I/usr/include/db4
#PLUGS_LBS =	-I/usr/include/db4 -ldb-4.2 -DUSE_THREADS -DHAS_FLOCK_LOCK -DHAS_FCNTL_LOCK


OBJ =	libserver
SLD =	libserver.so


$(OBJ): assoc.o cache.o daemon.o hash.o items.o memcached.o slabs.o stats.o thread.o util.o ./modules/mod.o
	$(LINK) $(INCS) $(LBS) -o $(OBJ) assoc.o cache.o daemon.o hash.o items.o memcached.o slabs.o stats.o thread.o util.o ./modules/mod.o #sasl_defs.c solaris_priv.c
	#$(LINK) $(INCS) $(LBS) -std=gnu99 -DHAVE_CONFIG_H -O -fpic -shared -o $(SLD) assoc.o cache.o daemon.o hash.o items.o memcached.o slabs.o stats.o thread.o util.o


	cd confparser && $(MAKE) -w
	cd modules && $(MAKE) -w


memcached.o: memcached.c
	$(LINK) -c -o memcached.o memcached.c 

assoc.o: assoc.c
	$(LINK) -c -o assoc.o assoc.c

cache.o: cache.c
	$(LINK) -c -o cache.o cache.c

daemon.o: daemon.c
	$(LINK) -c -o daemon.o daemon.c

hash.o: hash.c
	$(LINK) -c -o hash.o hash.c

items.o: items.c
	$(LINK) -c -o items.o items.c

slabs.o: slabs.c
	$(LINK) -c -o slabs.o slabs.c

stats.o: stats.c
	$(LINK) -c -o stats.o stats.c

thread.o: thread.c
	$(LINK) -c -o thread.o thread.c

util.o: util.c
	$(LINK) -c -o util.o util.c



clean:
	rm -f *.o
	rm -f $(OBJ)
	#rm -f $(SLD)

	cd confparser && $(MAKE) clean -w
	cd modules && $(MAKE) clean -w  

