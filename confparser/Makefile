#
# confparser Makefile
#

# Compiler settings
CC      = gcc
CFLAGS  = -g -fPIC -Wall -ansi -pedantic -I../include 

# Ar settings to build the library
AR	    = ar
ARFLAGS = rcv

SHLD = ${CC} ${CFLAGS}
LDSHFLAGS = -shared -Wl,-Bsymbolic  -Wl,-rpath -Wl,/usr/lib -Wl,-rpath,/usr/lib
LDFLAGS = -Wl,-rpath -Wl,/usr/lib -Wl,-rpath,/usr/lib

# Set RANLIB to ranlib on systems that require it (Sun OS < 4, Mac OSX)
RANLIB  = ranlib
#RANLIB = true

RM      = rm -f


# Implicit rules

SUFFIXES = .o .c .h .a .so .sl

COMPILE.c=$(CC) $(CFLAGS) -c
.c.o:
	@(echo "compiling $< ...")
	@($(COMPILE.c) -o $@ $<)


SRCS = iniparser.c \
	dictionary.c \
	confparser.c	

OBJS = $(SRCS:.c=.o)


default:	libconfparser.a libconfparser.so

libconfparser.a:	$(OBJS)
	@($(AR) $(ARFLAGS) $@ $(OBJS))
	@($(RANLIB) $@)

libconfparser.so:	$(OBJS)
	@$(SHLD) $(LDSHFLAGS) -o $@.0 $(OBJS) $(LDFLAGS) \
		-Wl,-soname=`basename $@`.0
	ln -s $@.0 $@
	#cp libconfparser.so.0 /usr/local/lib/
	#cp libconfparser.so /usr/local/lib/

clean:
	$(RM) $(OBJS)
	$(RM) *.so *.a *.so.0

veryclean:
	$(RM) $(OBJS) libconfparser.a libconfparser.so*
	rm -rf ./html ; mkdir html
	cd test ; $(MAKE) veryclean

docs:
	@(cd doc ; $(MAKE))
	
check:
	@(cd test ; $(MAKE))
