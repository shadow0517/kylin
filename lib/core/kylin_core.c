#include <utils/kylin_base.h>
#include <dlfcn.h>
#include "kylin_module.h"

typedef struct kylin_module* (*kylin_module_get_f)(void);

struct kylin_module_entry
{
    uint16_t            use;
    void                *context;
    struct kylin_module *module;
};

struct kylin_module_session
{
    uint16_t                  count;
    struct kylin_module_entry entry[KYLIN_MOD_CONT_MAX];
};

static struct kylin_module_session session;

int kylin_module_load(const char *name)
{
    int ret   = EINVAL;
    int index = 0;
    void *context      = NULL;
    char func_name[64] = {'\0'};
    struct kylin_module *module = NULL;
    kylin_module_get_f  func;

    for(index = 0; index < session.count; index++) {
        if(memcmp(name, session.entry[index].module->name, KYLIN_MOD_NAME_MAX) == 0) {
            session.entry[index].use++;
            ret = 0;
            goto exit;
        }
    }

    context = dlopen(name, RTLD_NOW | RTLD_GLOBAL);
    if(context == NULL) {
        printf("loading module: %s failed: %s\n", name, dlerror());
        goto exit;
    }

    snprintf(func_name, 64, "%s_module_get", name);
    func = (kylin_module_get_f)dlsym(context, func_name);
    if(func == NULL) {
        printf("loading module: %s failed: %s\n", name, dlerror());
        goto exit;
    }

    module = (*func)();
    if(module == NULL) {
        printf("loading module: %s failed: module data pointer null.\n", name);
        ret = ENOENT;
        goto exit;
    }

    index = session.count++;

    session.entry[index].use     = 1;
    session.entry[index].context = context;
    session.entry[index].module  = module;

    session.entry[index].module->handle(KYLIN_MODULE_LOAD);

    ret = 0;
exit:
    return ret;
}

void kylin_module_unload(const char *name)
{
    int index = 0;

    for(index = 0; index < session.count; index++) {
        if(memcmp(name, session.entry[index].module->name, KYLIN_MOD_NAME_MAX) == 0) {
            if(session.entry[index].use == 1) {
                session.entry[index].module->handle(KYLIN_MODULE_UNLOAD);
                dlclose(session.entry[index].context);
            } else {
                session.entry[index].use--;
            }
        }
    }
}
