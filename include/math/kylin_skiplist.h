#ifndef _KYLIN_MATH_SKIP_LIST_H_
#define _KYLIN_MATH_SKIP_LIST_H_

/*
 * 跳跃表
 */

struct kylin_skiplist;
struct kylin_skiplist_node;
/*struct kylin_skiplist_iterator;*/
typedef struct kylin_skiplist          kslist_t;
typedef struct kylin_skiplist_node     kslist_node_t;
/*typedef struct kylin_skiplist_iterator kslist_iter_t;*/

extern kslist_t *kylin_skiplist_create(void);
extern void kylin_skiplist_destroy(kslist_t *);
extern void *kylin_skiplist_val(kslist_t *);

extern kslist_node_t *kylin_skiplist_insert(kslist_t *, double score, void *data);

#endif /*_KYLIN_MATH_SKIP_LIST_H_*/
