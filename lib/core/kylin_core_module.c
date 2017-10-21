#include <kylin/include/kylin.h>
#include <kylin/include/kylin_core.h>
#include <kylin/include/math/kylin_list.h>
#include <kylin/include/utils/kylin_error.h>
#include <kylin/include/utils/kylin_limit.h>

#include <kylin/lib/core/kylin_core_module.h>
#include <kylin/lib/core/kylin_core_module_elf.h>

#define KYLIN_MODULE_CONFIG_FILE  "modules.conf"

typedef kmod_t *(*module_get_func)(void);

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

static int __module_load(kmod_session_t *session, kmod_internal_t *module_load)
{
    void *handle = NULL;
    char path[PATH_MAX] = {0};
    char func_name[64];
    module_get_func func;

    snprintf(path, PATH_MAX, "%s/%s.so", session->path, module_load->name);
    handle = dlopen(path, RTLD_NOW | RTLD_GLOBAL);
    if(!handle) 
        return KYLIN_ERROR_INVAL;

    snprintf(func_name, 64, "%s_module", module_load->name);
    func = (module_get_func)dlsym(handle, func_name);
    if (func == NULL) {
        dlclose(handle);
        return KYLIN_ERROR_INVAL;
    }

    module_load->dl_handle = handle;
    module_load->module    = func();
    if(!module_load->module) {
        dlclose(handle);
        return KYLIN_ERROR_INVAL;
    }

    return module_load->module->handle(KMOD_ACTION_LOAD);
}

static kerr_t __module_unload(kmod_session_t *session, kmod_internal_t *module_unload)
{
    kerr_t ret = KYLIN_ERROR_OK;

    ret = module_unload->module->handle(KMOD_ACTION_UNLOAD);
    if(ret != KYLIN_ERROR_OK)
        return ret;

    dlclose(module_unload->dl_handle);

    return ret;
}

static kbool_t __module_exist(kmod_session_t *session, const char *name)
{
    kmod_internal_t cmp;

    memset(&cmp, 0, sizeof(kmod_internal_t));
    memcpy(cmp.name, name, KYLIN_MIN(strlen(name), (KYLIN_NAME_LENGTH - 1))); 

    if(kylin_list_find(session->module_list, &cmp))
        return KYLIN_TRUE;

    return KYLIN_FALSE;
}

kmod_session_t *kylin_module_session_create(const char *path)
{
    kmod_session_t *session = NULL;

    session = kylin_malloc(sizeof(kmod_session_t));
    if(!session)
        return NULL;

    klist_opts_t module_opts = {
        .name      = "session module",
        .val_type  = KOBJ_OTHERS,
        .val_size  = sizeof(kmod_internal_t),
        .match     = __module_match,
        .allocator = KLIST_OPTS_ALLOCATOR_NULL 
        
    };
    session->module_list = kylin_list_create(&module_opts);
    if(!session->module_list) {
        kylin_free(session);
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

            mod_depend = kylin_malloc(sizeof(kmod_internal_t));
            if(!mod_depend)
                return KYLIN_ERROR_NOMEM;

            memcpy(mod_depend->name, depend, KYLIN_NAME_LENGTH);
            mod_depend->refcnt = 1;

            if(!kylin_list_insert_before(param->session->module_list, mod_node, mod_depend)){
                kylin_free(mod_depend);
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
    char               path[PATH_MAX] = {0};
    ked_t             *ed             = NULL;
    kmod_internal_t   *mod_load       = NULL;
    kmod_elf_params_t  param;

    if(__module_exist(session, mod_name)) 
        return KYLIN_ERROR_EXIST;

    mod_load = kylin_malloc(sizeof(kmod_internal_t));
    if(!mod_load)
        return KYLIN_ERROR_NOMEM;

    memcpy(mod_load->name, mod_name, KYLIN_MIN(strlen(mod_name), (KYLIN_NAME_LENGTH - 1)));
    mod_load->refcnt = 1;

    if(!kylin_list_insert_tail(session->module_list, mod_load)) {
        kylin_free(mod_load);
        return KYLIN_ERROR_NOENT;
    }

    snprintf(path, PATH_MAX, "%s/%s.so", session->path, mod_name);
    ed = kylin_elf_create(path);
    if(!ed) 
        return KYLIN_ERROR_INVAL;

    param.session  = session;
    memcpy(param.mod_name, mod_name, KYLIN_MIN(strlen(mod_name), (KYLIN_NAME_LENGTH - 1)));

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
    kerr_t       ret = KYLIN_ERROR_OK;
    int          fd;
    struct stat  sb;
    char        *config_string                  = NULL;
    char        *begin                          = NULL;
    char        *end                            = NULL;
    char         config_filename[PATH_MAX]      = {0};
    char         module_name[KYLIN_NAME_LENGTH] = {0};

    snprintf(config_filename, PATH_MAX, "%s/%s", session->path, KYLIN_MODULE_CONFIG_FILE);
    fd = open(config_filename, O_RDONLY);
    if(fd == -1)
        return errno;

    if(fstat(fd, &sb) < 0) {
        close(fd);
        return errno;
    }

    config_string = (char *)mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (config_string == MAP_FAILED) {
        close(fd);
        return errno;
    }

    begin = config_string;
    end   = begin;

    while(end < config_string + sb.st_size) {
        while(*end != '\n' && end < config_string + sb.st_size) 
            end++;

        if(*end == '\n' && end >= (config_string + sb.st_size)) 
            break;

        if((end > begin) && (end - begin) < PATH_MAX) {
            memset(module_name, 0, KYLIN_NAME_LENGTH);
            strncpy(module_name, begin, end - begin);

            ret = kylin_module_load(session, module_name);
            if(ret != KYLIN_ERROR_OK && ret != KYLIN_ERROR_EXIST) { 
                munmap(config_string, sb.st_size);
                close(fd);
                return ret;
            }
        }

        end++;
        begin = end;
    }

    munmap(config_string, sb.st_size);
    close(fd);

    return KYLIN_ERROR_OK;
}

kerr_t kylin_module_unload(kmod_session_t *session, const char *mod_name)
{
    kerr_t ret = KYLIN_ERROR_OK;
    kmod_internal_t  cmp;
    kmod_internal_t *mod_unload;

    memset(&cmp, 0, sizeof(kmod_internal_t));
    memcpy(cmp.name, mod_name, KYLIN_MIN(strlen(mod_name), (KYLIN_NAME_LENGTH - 1)));

    mod_unload = kylin_list_find(session->module_list, &cmp);
    if(!mod_unload)
        return KYLIN_ERROR_NOENT;

    ret = __module_unload(session, mod_unload);
    if(ret != KYLIN_ERROR_OK)
        return ret;

    mod_unload = kylin_list_unlink(session->module_list, &cmp);
    if(!mod_unload)
        return KYLIN_ERROR_NOENT;

    kylin_free(mod_unload);

    return KYLIN_ERROR_OK;
}

kerr_t kylin_module_unload_all(kmod_session_t *session)
{
    kerr_t ret = KYLIN_ERROR_OK;
    klist_node_t    *mod_node   = NULL;
    kmod_internal_t *mod_unload = NULL;

    KYLIN_LIST_FOREACH_REVERSE(session->module_list, mod_node) {
        mod_unload = kylin_list_val(session->module_list, mod_node);
        if(!mod_unload)
            return KYLIN_ERROR_INVAL;

        ret = __module_unload(session, mod_unload);
        if(ret != KYLIN_ERROR_OK)
            return ret;
    }

    kylin_list_destroy(session->module_list);

    return KYLIN_ERROR_OK;
}

size_t kylin_module_count(kmod_session_t *session)
{
    return kylin_list_count(session->module_list);
}

void kylin_module_dump(kmod_session_t *session, kylin_print print)
{
    return;
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

