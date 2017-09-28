#ifndef _KYLIN_IPC_SHM_H_
#define _KYLIN_IPC_SHM_H_

#include <kylin/include/kylin.h>

struct kylin_shm;
typedef struct kylin_shm kshm_t;

extern kshm_t *kylin_shm_create(const char *name, size_t);
extern void kylin_shm_destroy(kshm_t *);

extern kshm_t *kylin_shm_open(const char *name);
extern void kylin_shm_close(kshm_t *);

extern void kylin_shm_lock(kshm_t *);
extern void kylin_shm_unlock(kshm_t *);
extern int kylin_shm_trylock(kshm_t *);

extern void *kylin_shm_addr(kshm_t *);
extern size_t kylin_shm_size(kshm_t *);

#endif /*_KYLIN_IPC_SHM_H_*/
