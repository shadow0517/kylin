#include <kylin/include/kylin.h>
#include <kylin/include/kylin_core.h>
#include <kylin/include/math/kylin_list.h>
#include <kylin/include/utils/kylin_error.h>

typedef struct kylin_module_internal
{
    uint16_t            refcnt;
    kmod_t              module;
} kmod_internal_t;

struct kylin_module_session
{
    klist_t  *module_list;
};

kmod_session_t *kylin_module_session_create(const char *config_path)
{

}

void kylin_module_session_destroy(kmod_session_t *session)
{

}

kerr_t kylin_module_load(kmod_session_t *session, const char *mod_name)
{

}

kerr_t kylin_module_load_all(kmod_session_t *session)
{

}

kerr_t kylin_module_unload(kmod_session_t *session, const char *mod_name)
{

}

kerr_t kylin_module_unload_all(kmod_session_t *session)
{

}

size_t kylin_module_count(kmod_session_t *session)
{

}

void kylin_module_dump(kmod_session_t *session, kylin_print print)
{

}

kerr_t module_init(void)
{
    return KYLIN_ERROR_OK;
}

void module_fini(void)
{
    return;
}

