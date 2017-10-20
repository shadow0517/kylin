#include <kylin/include/kylin.h>
#include <kylin/include/kylin_core.h>
#include <kylin/include/math/kylin_list.h>
#include <kylin/include/utils/kylin_error.h>
#include <kylin/include/utils/kylin_limit.h>

typedef struct kmod_t *(*module_get_func)(void);

typedef struct kylin_module_internal {
    char      name[KYLIN_NAME_LENGTH];
    uint32_t  refcnt;
    void     *dl_handle;   /*dlopen*/
    kmod_t   *module;
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
    kerr_t             ret = KYLIN_ERROR_OK;
    char               base[PATH_MAX]            = {0};
    char               path[PATH_MAX]            = {0};
    char               depend[KYLIN_NAME_LENGTH] = {0};
    kmod_elf_params_t  depend_param;
    kmod_elf_params_t *param                     = NULL;
    ked_t             *ed                        = NULL;
    klist_node_t      *mod_node                  = NULL;
    kmod_internal_t   *mod_depend                = NULL;
    kmod_internal_t    cmp;

    param = (kmod_elf_params_t *)data;

    snprintf(base, PATH_MAX, "%s_depend_on_", param->mod_name);

    if(strlen(name) > strlen(base) &&
            strncmp(base, name, strlen(base)) == 0) {
        memcpy(depend, name + strlen(base), strlen(name) - strlen(base));

        if(__module_exist(param->session, depend)) {
            memcpy(cmp.name, depend, KYLIN_NAME_LENGTH);

            mod_depend = kylin_list_find(param->session->module_list, &cmp);
            if(!mod_depend)
                return KYLIN_ERROR_NOENT;

            mod_depend->refcnt++;
        }
        else {
            memcpy(cmp.name, param->mod_name, KYLIN_NAME_LENGTH);

            mod_node = kylin_list_find_raw(param->session->module_list, &cmp);
            if(!mod_node)
                return KYLIN_ERROR_INVAL;

            mod_depend = malloc(sizeof(kmod_internal_t));
            if(!mod_depend)
                return KYLIN_ERROR_NOMEM;

            memset(mod_depend, 0, sizeof(kmod_internal_t));
            memcpy(mod_depend->name, depend, KYLIN_NAME_LENGTH);
            mod_depend->refcnt = 1;

            if(!kylin_list_insert_before(param->session->module_list, mod_node, mod_depend)){
                free(mod_depend);
                return KYLIN_ERROR_NOENT;
            }
        }

        snprintf(path, PATH_MAX, "%s/%s.so", param->session->path, depend);
        ed = kylin_elf_create(path);
        if(!ed) 
            return KYLIN_ERROR_NOENT;

        depend_param.session  = param->session;
        depend_param.mod_name = depend;

        if(kylin_elf_symbol(ed, __module_depend_process, &depend_param) != KYLIN_ERROR_OK)
            exit(1);

        if(ed)
            kylin_elf_destroy(ed);

        if(mod_depend->refcnt == 1)
            return __module_load(param->session, mod_depend);
    }

    return KYLIN_ERROR_OK;
}

kerr_t kylin_module_load(kmod_session_t *session, const char *mod_name)
{
    kerr_t             ret            = KYLIN_ERROR_OK;
    char               path[PATH_MAX] = {0};
    ked_t             *ed             = NULL;
    kmod_internal_t   *mod_load       = NULL;
    kmod_elf_params_t  param;

    if(__module_exist(session, mod_name)) 
        return KYLIN_ERROR_EXIST;

    mod_load = malloc(sizeof(kmod_internal_t));
    if(!mod_load)
        return KYLIN_ERROR_NOMEM;

    memset(mod_load, 0, sizeof(kmod_internal_t));
    memcpy(mod_load->name, mod_name, KYLIN_MIN(strlen(name), (KYLIN_NAME_LENGTH - 1)));
    mod_load->refcnt = 1;

    if(!kylin_list_insert_tail(session->module_list, mod_load)) {
        free(mod_load);
        return KYLIN_ERROR_NOENT;
    }

    snprintf(path, PATH_MAX, "%s/%s.so", session->path, mod_name);
    ed = kylin_elf_create(path);
    if(!ed) 
        return KYLIN_ERROR_INVAL;

    param.session  = session;
    param.mod_name = mod_name;

    if(kylin_elf_symbol(ed, __module_depend_process, &param) != KYLIN_ERROR_OK)
        exit(1);

    if(ed)
        kylin_elf_destroy(ed);

    if(mod_load->refcnt == 1)
        return __module_load(session, mod_load);

    return KYLIN_ERROR_OK;
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

kerr_t module_init(void)
{
    return KYLIN_ERROR_OK;
}

void module_fini(void)
{
    return;
}

