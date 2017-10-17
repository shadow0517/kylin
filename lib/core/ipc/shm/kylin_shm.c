#include <kylin/include/kylin.h>
#include <kylin/include/arch/x86/kylin_spinlock.h>
#include <kylin/include/utils/kylin_limit.h>

#include <kylin/include/ipc/kylin_shm.h>

#include <kylin/lib/core/ipc/shm/kylin_shm.h>
#include <kylin/lib/core/ipc/shm/kylin_shm_obj.h>
#include <kylin/lib/core/ipc/shm/kylin_shm_open.h>

struct kylin_shm {
    char         name[KYLIN_NAME_LENGTH];
    kfd_t        fd;
    kspinlock_t  lock;
    size_t       cap;
    void        *addr;
};

kerr_t kylin_shm_create(const char *name, size_t cap)
{
    kshm_obj_t obj;

    if(strlen(name) > KYLIN_NAME_LENGTH )
        return KYLIN_ERROR_INVAL;
    if(object_find(name))
        return KYLIN_ERROR_EXIST;

    memset(&obj, 0, sizeof(kshm_obj_t));
    memcpy(obj.name, name, strlen(name));
    obj.cap    = cap;
    obj.refcnt = 0;

    return object_insert(&obj);
}

void kylin_shm_destroy(const char *name)
{
    kshm_obj_t *obj = NULL;

    obj = object_find(name);
    if(obj && --obj->refcnt == 0) 
        object_delete(obj);

    return;
}

kshm_t *kylin_shm_open(const char *name)
{
    kshm_t     *shm = NULL;
    kshm_obj_t *obj = NULL;

    if(strlen(name) > KYLIN_NAME_LENGTH ||
            !(obj = object_find(name)))
        return NULL;

    shm = malloc(sizeof(kshm_t));
    if(!shm)
        return NULL;
    memset(shm, 0, sizeof(kshm_t));

    shm->fd = kshm_open(name, O_RDWR | O_CREAT, 00777);
    if(shm->fd == -1 ||
            ftruncate(shm->fd, obj->cap) == -1) {
        kshm_unlink(name);
        free(shm);
        return NULL;
    }

    shm->addr = mmap(NULL, obj->cap, PROT_WRITE | PROT_READ, MAP_SHARED, shm->fd, SEEK_SET);
    if(!shm->addr) {
        kshm_unlink(name);
        free(shm);
        return NULL;
    }

    memset(shm->addr, 0, obj->cap);
    memcpy(shm->name, name, strlen(name));
    kylin_spinlock_init(&shm->lock);
    shm->cap = obj->cap;

    obj->refcnt++;

    return shm;
}

void kylin_shm_close(kshm_t *shm)
{
    kshm_obj_t *obj = NULL;

    obj = object_find(shm->name);
    if(obj) {
        obj->refcnt--;
        if(shm->fd)
            kshm_unlink(shm->name);
        if(shm->addr)
            munmap(shm->addr, shm->cap);
        free(shm);
    }

    return;
}

void kylin_shm_lock(kshm_t *shm)
{
    kylin_spinlock_lock(&shm->lock);
}

void kylin_shm_unlock(kshm_t *shm)
{
    kylin_spinlock_unlock(&shm->lock);
}

int kylin_shm_trylock(kshm_t *shm)
{
    return kylin_spinlock_trylock(&shm->lock);
}

void *kylin_shm_addr(kshm_t *shm)
{
    return shm->addr;
}

size_t kylin_shm_size(kshm_t *shm)
{
    return shm->cap;
}

kerr_t kylin_shm_init(void)
{
    return object_init();
}

void kylin_shm_fini(void)
{
    object_fini();
}

