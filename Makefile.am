bin_PROGRAMS = libserver 
pkginclude_HEADERS = protocol_binary.h
noinst_PROGRAMS = libserver-debug sizes testapp timedrun

BUILT_SOURCES=

testapp_SOURCES = testapp.c util.c util.h

timedrun_SOURCES = timedrun.c

libserver_SOURCES = memcached.c memcached.h \
                    hash.c hash.h \
                    slabs.c slabs.h \
                    items.c items.h \
                    assoc.c assoc.h \
                    thread.c daemon.c \
                    stats.c stats.h \
                    util.c util.h \
                    trace.h cache.h sasl_defs.h

libserver_SOURCES += modules/mod.c modules/mod.h
libserver_SOURCES += confparser/confparser.c confparser/confparser.h \
					confparser/dictionary.c confparser/dictionary.h \
					confparser/iniparser.c confparser/iniparser.h \
					confparser/queue.h

if BUILD_CACHE
libserver_SOURCES += cache.c
testapp_SOURCES += cache.c
endif

if BUILD_SOLARIS_PRIVS
libserver_SOURCES += solaris_priv.c
endif

if ENABLE_SASL
libserver_SOURCES += sasl_defs.c
endif

libserver_debug_SOURCES = $(libserver_SOURCES)
libserver_CPPFLAGS = -DNDEBUG
libserver_debug_LDADD = @PROFILER_LDFLAGS@
libserver_debug_CFLAGS = @PROFILER_FLAGS@

libserver_LDADD =
libserver_DEPENDENCIES =
libserver_debug_DEPENDENCIES =
CLEANFILES=

if BUILD_DTRACE
BUILT_SOURCES += memcached_dtrace.h
CLEANFILES += memcached_dtrace.h
endif

if DTRACE_INSTRUMENT_OBJ
libserver_LDADD += memcached_dtrace.o
libserver_DEPENDENCIES += memcached_dtrace.o
libserver_debug_LDADD += memcached_debug_dtrace.o
libserver_debug_DEPENDENCIES += memcached_debug_dtrace.o
CLEANFILES += memcached_dtrace.o memcached_debug_dtrace.o
endif

libserver_dtrace.h: memcached_dtrace.d
	${DTRACE} -h -s memcached_dtrace.d
	sed -e 's,void \*,const void \*,g' memcached_dtrace.h | \
            sed -e 's,char \*,const char \*,g' | tr '\t' ' ' > mmc_dtrace.tmp
	mv mmc_dtrace.tmp memcached_dtrace.h

libserver_dtrace.o: $(libserver_OBJECTS)
	$(DTRACE) $(DTRACEFLAGS) -G -o memcached_dtrace.o -s ${srcdir}/memcached_dtrace.d $(libserver_OBJECTS)

libserver_debug_dtrace.o: $(libserver_debug_OBJECTS)
	$(DTRACE) $(DTRACEFLAGS) -G -o memcached_debug_dtrace.o -s ${srcdir}/memcached_dtrace.d $(libserver_debug_OBJECTS)


SUBDIRS = doc
DIST_DIRS = scripts
EXTRA_DIST = doc scripts t memcached.spec memcached_dtrace.d version.m4

MOSTLYCLEANFILES = *.gcov *.gcno *.gcda *.tcov

test:	memcached-debug sizes testapp
	$(srcdir)/sizes
	$(srcdir)/testapp
	prove $(srcdir)/t
	@if test `basename $(PROFILER)` = "gcov"; then \
	  for file in memcached_debug-*.gc??; do \
	    mv -f $$file `echo $$file | sed 's/memcached_debug-//'`; \
	  done && \
	  for file in *.gcda; do \
	    srcfile=`echo $$file | sed 's/.gcda/.c/'`; \
	    if test -n "`echo $(libserver_debug_SOURCES) | grep $$srcfile`"; then \
	      echo `$(PROFILER) $$srcfile` | sed 's/'$$srcfile':.*//'; \
	    fi \
	  done \
	elif test `basename $(PROFILER)` = "tcov"; then \
	  files=`grep SRCFILE memcached-debug.profile/tcovd | sed 's/SRCFILE://' | sort | uniq` && \
	  $(PROFILER) -x memcached-debug.profile $$files 2>&1; \
	  for file in *.tcov; do \
	    srcfile=`echo $$file | sed 's/.tcov//'`; \
	    if test -n "`echo $(libserver_debug_SOURCES) | grep $$srcfile`"; then \
	      echo $$srcfile : `grep 'Percent of the file executed' $$file`; \
	    fi \
	  done \
	else :; fi

docs:
	(cat Doxyfile ; echo "PROJECT_NUMBER=`cat version.num`") | doxygen -

dist-hook:
	rm -f $(distdir)/*/*~ $(distdir)/t/lib/*~ $(distdir)/*~
