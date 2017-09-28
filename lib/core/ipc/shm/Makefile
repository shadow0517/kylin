SHELL=/bin/sh
# Where you want it installed when you do 'make install'
PREFIX=/usr/local/

CFLAGS+= ${KYLIN_CFLAGS} -DKYLIN_MODULE -I/usr/local/include -I${KYLIN_LIB_SRC}
LDFLAGS+=-g -L/usr/local/lib -lkylin_module -lrt 

all: kylin_shm

LIB_OBJS = kylin_shm_version.o kylin_shm.o 

kylin_shm: $(LIB_OBJS)
	${CC} ${CFLAGS} ${LIB_OBJS} -shared -o kylin_shm.so ${LDFLAGS}

kylin_shm_version.c:
	@sh ../utils/module_version.sh shm

kylin_shm_version.o: kylin_shm_version.c
	${CC} ${CFLAGS} -c kylin_shm_version.c

distclean: clean
clean:
	@rm kylin_shm_version.h kylin_shm_version.c
	@rm -f *.o kylin_shm.so


