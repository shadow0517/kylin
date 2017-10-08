SHELL=/bin/sh
# Where you want it installed when you do 'make install'
PREFIX=/usr/local/

CFLAGS = ${PHOENIX_CFLAGS} -I/usr/local/include -I${PHOENIX_LIB_SRC}

all: module library

module:
	make -f Makefile.module clean
	make -f Makefile.module

library:
	make -f Makefile.library clean
	make -f Makefile.library

distclean: clean
clean:
	rm -f *.o plugin/*.o *.so 

