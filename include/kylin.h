#ifndef _KYLIN_H_
#define _KYLIN_H_

/*
 * 通用头文件
 * libc and posix api
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <assert.h>
#include <errno.h>

#include <pthread.h>

/*系统相关头文件*/
#if defined(__linux__)
#include <kylin/include/sys/linux/kylin.h>
#elif defined(__FreeBSD__)
#include <kylin/include/sys/linux/kylin.h>
#endif /*__SYS_LINUX__*/

/*静态断言*/
#include <assert.h>
#define kylin_assert assert

#ifndef typeof
#define typeof __typeof__
#endif

/*汇编指令*/
#ifndef asm
#define asm __asm__
#endif

/** C extension macro for environments lacking C11 features. */
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 201112L
#define KYLIN_STD_C11 __extension__
#else
#define KYLIN_STD_C11
#endif


/*汇编指令锁前缀*/
#define MPLOCK "lock ; "

/*编译属性*/
#define __kylin_aligned(a)      __attribute__((__aligned__(a)))
#define __kylin_packed          __attribute__((__packed__))
#define __kylin_deprecated	    __attribute__((__deprecated__))
#define __kylin_unused          __attribute__((__unused__))
#define __kylin_inline   inline __attribute__((always_inline))
#define __kylin_noinline        __attribute__((noinline))
#define __kylin_nonnull(params) __attribute__((__nonnull__ params))
#define __kylin_wur             __attribute__((warn_unused_result))

/*判断一个数是否为编译时常量*/
#define IS_CONSTANT(nr) (__builtin_constant_p(nr))

/*布尔类型*/
#define KYLIN_TRUE  1
#define KYLIN_FALSE 0
typedef uint8_t kbool_t;

/*字节流类型*/
typedef unsigned char kbyte_t;

/*指针类型*/
typedef int kaddr_t;

/*字类型*/
typedef unsigned long kword_t;

/*文件描述符类型*/
typedef int kfd_t;

/*打印类型*/
#define __kylin_format_print(a, b) __attribute__((__format__(__printf__, a, b)))
typedef int (*kylin_print)(const char *, ...)  __kylin_format_print(1, 2);

/*
 * 2的幂次方运算，其余两个函数
 * kylin_roundup_pow_of_two 和 kylin_rounddown_pow_of_two
 * 定义在<arch/kylin_atomic.h>文件中
 */
static inline kbool_t kylin_is_pow_of_two(unsigned long n)
{
	return ((n && !(n & (n - 1))) ? KYLIN_TRUE : KYLIN_FALSE);
}

/*container_of宏*/
#define kylin_container_of(ptr, type, member) ({                       \
            const typeof( ((type *)0)->member ) *__mptr = (ptr);       \
                (type *)( (char *)__mptr - offsetof(type,member) );})

/*值比较和交换*/
#define KYLIN_MIN(a, b)             \
    __extension__ ({                \
            typeof (a) _a = (a);    \
            typeof (b) _b = (b);    \
            _a < _b ? _a : _b;      \
            })

#define KYLIN_MAX(a, b)             \
    __extension__ ({                \
            typeof (a) _a = (a);    \
            typeof (b) _b = (b);    \
            _a > _b ? _a : _b;      \
            })

#define KYLIN_SWAP(a, b)            \
    __extension__ ({                \
            typeof (a) _tmp = (a);  \
            (a) = (b);              \
            (b) = _tmp;             \
            })

#endif /*_KYLIN_H_*/
