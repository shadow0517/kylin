#ifndef _KYLIN_CORE_THREAD_H_
#define _KYLIN_CORE_THREAD_H_

#include <kylin/include/kylin.h>
#include <kylin/include/kylin_thread.h>
#include <kylin/include/utils/kylin_limit.h>

struct kylin_thread {
    uint32_t        id;
    pthread_t       thread_id;
    char            name[KYLIN_NAME_LENGTH];

    kbool_t         terminated;
    void          (*exec)(kthread_t *);
    size_t          param_size;
    kbyte_t         param[];
};

extern kerr_t kylin_thread_insert(kthread_t *);
extern kerr_t kylin_thread_remove(kthread_t *);

extern kthread_t *kylin_thread_unlink(kthread_t *);

extern kthread_t *kylin_thread_find(uint32_t);
extern kthread_t *kylin_thread_find_by_tid(pthread_t);
extern kthread_t *kylin_thread_find_by_name(char *);

extern kthread_t *kylin_thread_get_first(void);
extern kthread_t *kylin_thread_get_next(kthread_t *);

#define KYLIN_THREAD_FOREACH(thread)                \
    for(thread = kylin_thread_get_first();          \
            thread != NULL;                         \
            thread = kylin_thread_get_next(thread))

extern kerr_t kylin_thread_init(void);
extern void kylin_thread_fini(void);

#endif /*_KYLIN_CORE_THREAD_H_*/
