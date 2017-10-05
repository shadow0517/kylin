#ifndef _KYLIN_SYS_LINUX_H_
#define _KYLIN_SYS_LINUX_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <alloca.h>
#include <endian.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>

#define KYLIN_PAGE_SIZE getpagesize()

#define KYLIN_PID    getpid()
#define KYLIN_PNAME  program_invocation_short_name 

typedef	cpu_set_t kcpuset_t;

#endif /*_KYLIN_SYS_LINUX_H_*/
