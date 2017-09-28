#include <kylin/include/kylin.h>
#include <kylin/lib/core/ipc/shm/kylin_shm_opne.h>

static size_t local_len = strlen(KYLIN_SHM_LOCAL); 

kfd_t kylin_shm_open(const char *name, int flag, mode_t mode)
{
    size_t name_len;
    char *file_name;

    name_len  = strlen(name);
    file_name = alloca(name_len + local_len + 1);

    memcpy(file_name, KYLIN_SHM_LOCAL, local_len);
    memcpy(file_name + local_len, name, name_len + 1);

    return open(file_name, flag | O_NOFOLLOW, mode);
}

int kylin_shm_unlink(const char *name)
{
    size_t name_len;
    char *file_name;

    name_len  = strlen(name);
    file_name = alloca(name_len + local_len + 1);

    memcpy(file_name, KYLIN_SHM_LOCAL, local_len);
    memcpy(file_name + local_len, name, name_len + 1);

    return unlink(file_name);
}

