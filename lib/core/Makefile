SHELL=/bin/sh
# Where you want it installed when you do 'make install'
PREFIX=/usr/local/

CFLAGS = ${KYLIN_CFLAGS} -I/usr/local/include -I${KYLIN_LIB_SRC}
LDFLAGS += -L/usr/local/lib -ldl

all: libkylin_module

LIB_OBJS = kylin_module.o

libkylin_module: $(LIB_OBJS)
	${CC} ${CFLAGS} ${LIB_OBJS} -shared -o libkylin_module.so ${LDFLAGS}

distclean: clean
clean:
	rm -f *.o libkylin_module.so
