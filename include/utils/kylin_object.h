#ifndef _KYLIN_UTILS_OBJECT_H_
#define _KYLIN_UTILS_OBJECT_H_

typedef enum {
    KOBJ_INT8         = 1,
    KOBJ_INT16,
    KOBJ_INT32,
    KOBJ_INT64,
    KOBJ_SIGNED       = 8,
    KOBJ_UINT8,
    KOBJ_UINT16,
    KOBJ_UINT32,
    KOBJ_UINT64,
    KOBJ_UNSIGNED     = 16,
    KOBJ_NORMAL       = 32, /*整数类型分割*/
    KOBJ_STRING,
    KOBJ_KMAP,
    KOBJ_KSET,
    KOBJ_KSTR,
    KOBJ_OTHERS       = 64
} kobj_t;

#endif /*_KYLIN_UTILS_OBJECT_H_*/
