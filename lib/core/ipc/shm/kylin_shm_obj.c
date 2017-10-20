#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_error.h>
#include <kylin/include/arch/x86/kylin_spinlock.h>

#include <kylin/lib/core/ipc/shm/kylin_shm_obj.h>
#include <kylin/lib/core/ipc/shm/kylin_shm_open.h>

#define KYLIN_SHM_OBJ_MAX             512
#define KYLIN_SHM_OBJ_OVERVIEW_FILE   "overview"

typedef struct {
    kspinlock_t lock;
    uint16_t    count;
    kshm_obj_t  obj[KYLIN_SHM_OBJ_MAX];
} kshm_overview_t;

static kshm_overview_t *shm_ov = NULL;

kerr_t object_insert(kshm_obj_t *obj)
{
    kylin_spinlock_lock(&shm_ov->lock);
    if(shm_ov->count > (KYLIN_SHM_OBJ_MAX - 1)) {
        kylin_spinlock_unlock(&shm_ov->lock);
        return KYLIN_ERROR_NOSPC;
    }

    memcpy(&shm_ov->obj[shm_ov->count++], obj, sizeof(kshm_obj_t));
    kylin_spinlock_unlock(&shm_ov->lock);

    return KYLIN_ERROR_OK;
}

kerr_t object_delete(kshm_obj_t *obj)
{
    kylin_spinlock_lock(&shm_ov->lock);
    for(int i = 0; i < shm_ov->count; i++) {
        if(memcmp(shm_ov->obj[i].name, obj->name, KYLIN_NAME_LENGTH) == 0) {
            memmove(&shm_ov->obj[i], &shm_ov->obj[i + 1], 
                    (shm_ov->count - i - 1) * sizeof(kshm_obj_t));
            shm_ov->count--;
            kylin_spinlock_unlock(&shm_ov->lock);
            return KYLIN_ERROR_OK;
        }
    }
    kylin_spinlock_unlock(&shm_ov->lock);

    return KYLIN_ERROR_NOENT;
}

kerr_t object_delete_by_index(uint32_t index)
{
    kylin_spinlock_lock(&shm_ov->lock);
    memmove(&shm_ov->obj[index], &shm_ov->obj[index + 1], 
            (shm_ov->count - index - 1) * sizeof(kshm_obj_t));
    shm_ov->count--;
    kylin_spinlock_unlock(&shm_ov->lock);

    return KYLIN_ERROR_OK;
}

kshm_obj_t *object_find(const char *name)
{
    kylin_spinlock_lock(&shm_ov->lock);
    for(int i = 0; i < shm_ov->count; i++) {
        if(memcmp(shm_ov->obj[i].name, name, KYLIN_NAME_LENGTH) == 0) {
            kylin_spinlock_unlock(&shm_ov->lock);
            return shm_ov->obj + i;
        }
    }
    kylin_spinlock_unlock(&shm_ov->lock);

    return NULL;
}

kshm_obj_t *object_find_by_index(uint32_t index)
{
    return shm_ov->obj + index;
}

kerr_t object_init(void)
{
    kfd_t fd = -1;
    kbool_t is_create = KYLIN_FALSE;

    if((fd = kshm_open(KYLIN_SHM_OBJ_OVERVIEW_FILE, O_RDWR, 00777)) == -1) {
        is_create = KYLIN_TRUE;

        if((fd = kshm_open(KYLIN_SHM_OBJ_OVERVIEW_FILE, O_RDWR | O_CREAT, 00777)) == -1)
            return errno;

        if(ftruncate(fd, sizeof(kshm_overview_t)) == -1) {
            kshm_unlink(KYLIN_SHM_OBJ_OVERVIEW_FILE);
            return errno;
        }
    }

    shm_ov = mmap(NULL, sizeof(kshm_overview_t), PROT_WRITE | PROT_READ ,
            MAP_SHARED, fd, SEEK_SET);
    if(shm_ov == MAP_FAILED) {
        kshm_unlink(KYLIN_SHM_OBJ_OVERVIEW_FILE);
        return errno;
    }

    if(is_create) {
        memset(shm_ov, 0, sizeof(kshm_overview_t));
        kylin_spinlock_init(&shm_ov->lock);
    }

    return KYLIN_ERROR_OK;
}

void object_fini(void)
{
    if(shm_ov) {
        kshm_unlink(KYLIN_SHM_OBJ_OVERVIEW_FILE);
        munmap(shm_ov, sizeof(kshm_overview_t));
    }

    return;
}

