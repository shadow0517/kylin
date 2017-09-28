#ifndef _KYLIN_DCF_H_
#define _KYLIN_DCF_H_

/*
 * 采集者对象
 */
struct kylin_dcf_obj;
typedef struct kylin_dcf_obj kylin_dcf_t;

extern kerr_t kylin_dcf_feed(kylin_dcf_t *, kylin_json_t *);

/*
 * 5个钩子函数对应数据采集处理的I/O
 */
typedef kerr_t (*kylin_dcf_parse)(kjson_t *);
typedef ksock_t (*kylin_dcf_collect)(void *);
typedef kerr_t (*kylin_dcf_event)(void *);
typedef kerr_t (*kylin_dcf_process)(void *);
typedef kerr_t (*kylin_dcf_feedback)(void *);

/*
 * 生成一个采集者对象，对象保存了该类型的采集者在整个数据采集流程中用到的所有信息。
 * 名字全局唯一
 */
extern kylin_dcf_t *kylin_dcf_register(const char *name, 
        kylin_dcf_parse parse,
        kylin_dcf_collect collect,
        kylin_dcf_event event,
        kylin_dcf_process process,
        kylin_dcf_feedback feedback);

extern kerr_t kylin_dcf_unregister(kylin_dcf_t *);

extern kerr_t kylin_dcf_set_collect_period(kylin_dcf_t *, uint32_t period);

#endif /*_KYLIN_DCF_H_*/
