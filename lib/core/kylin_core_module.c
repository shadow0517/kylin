#include <kylin/include/kylin.h>
#include <kylin/include/kylin_core.h>
#include <kylin/include/math/kylin_list.h>
#include <kylin/include/utils/kylin_error.h>
#include <kylin/include/utils/kylin_limit.h>

typedef struct kmod_t *(*module_get_func)(void);

typedef struct kylin_module_internal {
    uint32_t  id;
    uint32_t  refcnt;
    char      name[KYLIN_NAME_LENGTH];
    void     *dl_handle;   /*dlopen*/
    kmod_t   *module;
    klist_t  *depend_list; /*list of kmod_internal_t, must be ahead of it*/
} kmod_internal_t;

struct kylin_module_session {
    char      path[PATH_MAX];
    klist_t  *module_list; /*list of kmod_internal_t*/
};

typedef struct kylin_module_elf_params {
    kmod_session_t *session;
    char           *mod_name;
} kmod_elf_params_t;

static int __module_match(const void *val, const void *key);
static int __depend_match(const void *val, const void *key);

static int __module_load(kmod_session_t *session, kmod_internal_t *module_internal)
{
    int ret;
    void *handle = NULL;
    char path[PATH_MAX] = {0};
    char func_name[64];
    module_get_func func;

    snprintf(path, PATH_MAX, "%s/%s.so", session->path, module_internal->name);
    handle = dlopen(path, RTLD_NOW | RTLD_GLOBAL);
    if(!handle) 
        return KYLIN_ERROR_INVAL;

    snprintf(func_name, 64, "%s_module", module_internal->name);
    func = (module_get_func)dlsym(handle, func_name);
    if (func == NULL) {
        dlclose(handle);
        return KYLIN_ERROR_INVAL;
    }

    module_internal->handle = handle;
    module_internal->module = (*func)();
    if(!module_internal->module) {
        dlclose(handle);
        return KYLIN_ERROR_INVAL;
    }

    return module_internal->module->handle(KMOD_ACTION_LOAD);
}

static kerr_t __module_unload(kmod_session_t *session, kmod_internal_t *module_internal)
{
    kerr_t ret = KYLIN_ERROR_OK;

    ret = module_internal->module->handle(KMOD_ACTION_UNLOAD);
    if(ret != KYLIN_ERROR_OK)
        return ret;

    dlclose(module_internal->handle);

    return ret;
}

static kbool_t __module_exist(kmod_session_t *session, char *name)
{
    kmod_internal_t internal;

    memset(internal, 0, sizeof(kmod_internal_t));
    memcpy(internal.name, name, KYLIN_MIN(strlen(name), (KYLIN_NAME_LENGTH - 1))); 

    if(kylin_list_find(session->module_list, &internal))
        return KYLIN_TRUE;

    return KYLIN_FALSE;
}

kmod_session_t *kylin_module_session_create(const char *path)
{
    kmod_session_t *session = NULL;
    klist_opts_t module_opts;

    session = malloc(sizeof(kmod_session_t));
    if(!session)
        return NULL;
    memset(session, 0, sizeof(kmod_session_t));

    module_opts = KLIST_OPTS_VAL_OTHERS_WITH_ALLOCATOR_NULL("session module", 
            sizeof(kmod_internal_t), __module_match);
    session->module_list = kylin_list_create(&module_opts);
    if(!session->module_list) {
        free(session);
        return NULL;
    }

    memcpy(session->path, path, KYLIN_MIN(strlen(path), (PATH_MAX - 1)));

    return session;
}

void kylin_module_session_destroy(kmod_session_t *session)
{
    if(session->module_list)
        kylin_list_destroy(session->module_list);

    return;
}

static kerr_t __module_depend_process(char *name, uint64_t value, void *data)
{
    char               base[PATH_MAX]            = {0};
    char               depend[KYLIN_NAME_LENGTH] = {0};
    kmod_elf_params_t *param                     = NULL;

    param = (kmod_elf_params_t *)data;

    snprintf(base, PATH_MAX, "%s_depend_on_", param->mod_name);

    if(strlen(name) > strlen(base) &&
            strncmp(base, name, strlen(base)) == 0) {
        memcpy(depend, name + strlen(base), strlen(name) - strlen(base));
    }

}

kerr_t kylin_module_load(kmod_session_t *session, const char *mod_name)
{
    kerr_t             ret            = KYLIN_ERROR_OK;
    char               path[PATH_MAX] = {0};
    ked_t             *ed             = NULL;
    kmod_internal_t   *internal       = NULL;
    klist_opts_t       depend_opts;
    kmod_elf_params_t  param;

    if(__module_exist(session, mod_name)) 
        return KYLIN_ERROR_EXIST;
    else {
        internal = malloc(sizeof(kmod_internal_t));
        if(!internal)
            return KYLIN_ERROR_NOMEM;

        depend_opts = KLIST_OPTS_VAL_OTHERS_WITH_ALLOCATOR_NULL("session depend module", 
                sizeof(kmod_internal_t), __depend_match);
        internal->depend_list = kylin_list_create(&depend_opts);
        if(internal->depend_list) {
            free(internal);
            return KYLIN_ERROR_NOMEM;
        }

        memcpy(internal->name, mod_name, KYLIN_MIN(strlen(name), (KYLIN_NAME_LENGTH - 1)));

        if(!kylin_list_insert_tail(session->module_list, internal)) {
            kylin_list_destroy(internal->depend_list);
            free(internal);
            return KYLIN_ERROR_NOENT;
        }
    }

    snprintf(path, PATH_MAX, "%s/%s.so", session->path, mod_name);
    ed = kylin_elf_create(path);
    if(!ed) 
        return KYLIN_ERROR_NOENT;

    param.session  = session;
    param.mod_name = mod_name;

    ret = kylin_elf_symbol(ed, __module_depend_process, &param);

 
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

static int __module_match(const void *val, const void *key)
{
    const kmod_internal_t *m1 = val;
    const kmod_internal_t *m2 = key;

    return memcmp(m1->name, m2->name, KYLIN_NAME_LENGTH);
}

static int __depend_match(const void *val, const void *key)
{
    const kmod_internal_t *m1 = val;
    const kmod_internal_t *m2 = key;

    return memcmp(m1->name, m2->name, KYLIN_NAME_LENGTH);
}

kerr_t module_init(void)
{
    return KYLIN_ERROR_OK;
}

void module_fini(void)
{
    return;
}

