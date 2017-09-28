#ifndef _KYLIN_DCF_DATA_H_
#define _KYLIN_DCF_DATA_H_

#include <kylin/include/kylin_dcf.h>

struct kylin_dcf_obj
{
    char name[KYLIN_NAME_LENGTH];

    struct {
        klist_head conf_q;
        klist_head exec_q;
        klist_head rslt_q;
        klist_head back_q;
    } store;

    struct {
        kylin_dcf_parse    parse;
        kylin_dcf_collect  collect;
        kylin_dcf_event    event;
        kylin_dcf_process  process;
        kylin_dcf_feedback feedback;
    } opr;

    uint32_t period;
};

extern kerr_t kylin_dcf_data_add(const char *name, kylin_dcf_t *obj);
extern kerr_t kylin_dcf_data_remove(const char *name);
extern kerr_t kylin_dcf_data_update(const char *name, kylin_dcf_t *obj);
extern kerr_t kylin_dcf_data_get_first(kylin_dcf_t *obj);
extern kerr_t kylin_dcf_data_get_next(kylin_dcf_t *obj);

#define kylin_dcf_data_foreach(obj) \
    for(int i = kylin_dcf_data_get_first(obj); i == KYLIN_ERROR_OK; i = kylin_dcf_data_get_next(obj))

extern kerr_t kylin_dcf_data_init(void);
extern void kylin_dcf_data_fini(void);
#endif /*_KYLIN_DCF_DATA_H_*/
