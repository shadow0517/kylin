#ifndef _KYLIN_ARCH_CPUFLAG_H_
#define _KYLIN_ARCH_CPUFLAG_H_

enum kylin_cpu_flag;
typedef enum kylin_cpu_flag kylin_cpu_flag_t;

const char * kylin_cpu_get_flag_name(kylin_cpu_flag_t feature);
int kylin_cpu_get_flag_enabled(kylin_cpu_flag_t feature);

#endif /*_KYLIN_ARCH_CPUFLAG_H_*/
