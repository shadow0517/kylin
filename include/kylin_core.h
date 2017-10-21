#ifndef _KYLIN_CORE_H_
#define _KYLIN_CORE_H_

#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_error.h>

extern kerr_t kylin_core_init(void);
extern void kylin_core_fini(void);

/*
 * 版本规范
 */


/*
 * 模块规范
 */
struct kylin_module_session;
typedef struct kylin_module_session kmod_session_t;

typedef enum {
    KMOD_ACTION_LOAD    = 0x01,        
    KMOD_ACTION_UNLOAD  = 0x02
} kmod_action_t;

typedef struct kylin_module {
	char *name;
    kerr_t (*handle)(kmod_action_t);
} kmod_t;

typedef struct kylin_module_depend {
    char *name;
} kmod_depend_t;

#define KYLIN_MODULE_DECLARE_INCLUDE(name)                          \
    char *name##_module_name(void);       

#define	KYLIN_MODULE_DECLARE(name, handle)		                    \
    static kmod_t __##name##_module = {                             \
        .name = name,                                               \
        .handle = handle                                            \
    };                                                              \
                                                                    \
    kmod_t *name##_module(void);                                    \
    kmod_t *name##_module(void)                                     \
    {                                                               \
        return &__##name##_module;                                  \
    }

#define KYLIN_MODULE_DECLARE_SOURCE(name)                           \
    char *name##_module_name(void)                                  \
    {                                                               \
        kmod_t *mod = name##_module();                              \
        return mod->name;                                           \
    }                                                               

#define	KYLIN_MODULE_DEPEND(module, depend)		                    \
	static kmod_depend_t __##module##_depend_on_##depend = {        \
		.name = depend,		    	                                \
	};							                                    \
                                                                    \
    kmod_depend_t *module##_depend_on_##depend(void);               \
    kmod_depend_t *module##_depend_on_##depend(void)                \
    {                                                               \
        return &__##module##_depend_on_##depend;                    \
    }

extern kmod_session_t *kylin_module_session_create(const char *config_path);
extern void kylin_module_session_destroy(kmod_session_t *);

/*模块加载*/
extern kerr_t kylin_module_load(kmod_session_t *, const char *mod_name);
extern kerr_t kylin_module_load_all(kmod_session_t *);

/*模块卸载*/
extern kerr_t kylin_module_unload(kmod_session_t *, const char *mod_name);
extern kerr_t kylin_module_unload_all(kmod_session_t *);

extern size_t kylin_module_count(kmod_session_t *);
extern void kylin_module_dump(kmod_session_t *, kylin_print print);

#endif /*_KYLIN_CORE_H_*/
