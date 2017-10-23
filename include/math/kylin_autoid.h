#ifndef _KYLIN_MATH_AUTOID_H_
#define _KYLIN_MATH_AUTOID_H_

#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_error.h>

/*
 * 基于位图实现的自动索引
 */
struct kylin_autoid;
typedef struct kylin_autoid kautoid_t;

extern kautoid_t *kylin_autoid_create(uint32_t);
extern void kylin_autoid_destroy(kautoid_t *);

extern void kylin_autoid_clear(kautoid_t *);

extern int kylin_autoid_total_count(kautoid_t *);
extern int kylin_autoid_unset_count(kautoid_t *);

extern int kylin_autoid_get(kautoid_t *);
extern kerr_t kylin_autoid_put(kautoid_t *, uint32_t);
extern kerr_t kylin_autoid_set(kautoid_t *, uint32_t);

#endif /*_KYLIN_MATH_AUTOID_H_*/
