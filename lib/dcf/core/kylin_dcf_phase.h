#ifndef _KYLIN_DCF_PHASE_H_
#define _KYLIN_DCF_PHASE_H_

#include <kylin/include/kylin_dcf.h>

/*
 *提供框架I/O重要阶段的处理函数，每个处理函数进行参数的合法性检查，默认动作的执行以及调用钩子函数。
 */
extern kerr_t phoenix_dcf_phase_parse(kylin_dcf_t *);
extern kerr_t phoenix_dcf_phase_collect(kylin_dcf_t *);
extern kerr_t phoenix_dcf_phase_event(kylin_dcf_t *);
extern kerr_t phoenix_dcf_phase_process(kylin_dcf_t *);
extern kerr_t phoenix_dcf_phase_feedback(kylin_dcf_t *);

#endif /*_KYLIN_DCF_PHASE_H_*/
