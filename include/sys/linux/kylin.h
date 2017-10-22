#ifndef _KYLIN_SYS_LINUX_H_
#define _KYLIN_SYS_LINUX_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <alloca.h>
#include <elf.h>
#include <endian.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <sched.h>
#include <sys/epoll.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

/* libbsd-dev */
#include <bsd/sys/endian.h>
//#include <bsd/stdlib.h>
//#include <bsd/md5.h>

#define KYLIN_PAGE_SIZE getpagesize()

#define KYLIN_PID    getpid()
#define KYLIN_PNAME  program_invocation_short_name 

extern __thread uint8_t kylin_cpu_id;
#define KYLIN_CPU_ID sched_getcpu()

typedef	cpu_set_t kcpuset_t;

#endif /*_KYLIN_SYS_LINUX_H_*/
