#include <kylin/include/kylin.h>
#include <kylin/lib/core/ipc/shm/kylin_shm_open.h>

kfd_t kshm_open(const char *name, int flag, mode_t mode)
{
    size_t name_len  = strlen(name);
    size_t local_len = strlen(KYLIN_SHM_LOCAL);
    char *file_name;

    file_name = alloca(name_len + local_len + 1);

    memcpy(file_name, KYLIN_SHM_LOCAL, local_len);
    memcpy(file_name + local_len, name, name_len + 1);

    return open(file_name, flag | O_NOFOLLOW, mode);
}

int kshm_unlink(const char *name)
{
    size_t name_len  = strlen(name);
    size_t local_len = strlen(KYLIN_SHM_LOCAL);
    char *file_name;

    file_name = alloca(name_len + local_len + 1);

    memcpy(file_name, KYLIN_SHM_LOCAL, local_len);
    memcpy(file_name + local_len, name, name_len + 1);

    return unlink(file_name);
}

