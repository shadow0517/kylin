#include <kylin/include/kylin.h>
#include <kylin/include/arch/x86/kylin_spinlock.h>
#include <kylin/include/utils/kylin_limit.h>

#include <kylin/include/ipc/kylin_shm.h>
#include <kylin/lib/core/ipc/shm/kylin_shm.h>
#include <kylin/lib/core/ipc/shm/kylin_shm_open.h>


struct kylin_shm {
    char         name[KYLIN_NAME_LENGTH];
    uint32_t     index; /*overview中关于该module的索引*/
    size_t       cap;
    void        *addr;
};

kshm_t *kylin_shm_create(const char *name, size_t cap)
{
    kfd_t fd    = -1;
    void *base  = NULL;
    kshm_t *shm = NULL;

    if(strlen(name) > KYLIN_NAME_LENGTH)
        goto exit;

    fd = kylin_shm_open(name, O_RDWR | O_CREAT, 00777);
    if(fd == -1)
        goto exit;

    if(ftruncate(fd, cap) == -1)
        goto exit;

    base = mmap(NULL, cap, PROT_WRITE | PROT_READ, MAP_SHARED, fd, SEEK_SET);
    if(base == NULL)
        goto exit;

    memset(base, 0, cap);

    shm = malloc(sizeof(kshm_t));
    if(shm == NULL)
        goto exit;

    memcpy(shm->name, name, KYLIN_MIN(strlen(name), KYLIN_NAME_LENGTH));
    shm->name[KYLIN_NAME_LENGTH - 1] = '\0';
    kylin_spinlock_init(&shm->lock);
    shm->cap = cap;
    shm->fd = fd;
    shm->addr = base;

exit:
    if(!shm && fd)
        kylin_shm_unlink(name);
    if(!shm && base)
        munmap(base, cap);

    return shm;
}

void kylin_shm_destroy(kshm_t *shm)
{

}

kshm_t *kylin_shm_open(const char *name)
{

}

void kylin_shm_close(kshm_t *shm)
{

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
    return kylin_spinlock_trylock(&shm->loca);
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

}

void kylin_shm_fini(void)
{

}

