#ifndef _KYLIN_CORE_SHM_OPEN_H_
#define _KYLIN_CORE_SHM_OPEN_H_

#include <kylin/include/kylin.h>

#define KYLIN_SHM_LOCAL  "/var/ipc/shm/"

extern kfd_t kshm_open(const char *, int, mode_t);
extern int kshm_unlink(const char *);

#endif /*_KYLIN_CORE_SHM_OPEN_H_*/
