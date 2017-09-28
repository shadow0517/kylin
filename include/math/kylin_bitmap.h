#ifndef _KYLIN_MATH_BITMAP_H_
#define _KYLIN_MATH_BITMAP_H_

#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_error.h>

/*
 * 位图
 */

#ifndef __SIZEOF_LONG__
#define __SIZEOF_LONG__ sizeof(long)
#endif

#define __SIZEOF_WORD__ __SIZEOF_LONG__

#define BITS_PER_WORD	    (__SIZEOF_WORD__ * 8)
#define BITS_TO_WORDS(bits) (((bits) + BITS_PER_WORD - 1)/BITS_PER_WORD)

struct kylin_bitmap;
typedef struct kylin_bitmap kbitmap_t;

typedef struct {
    const char *name;
    size_t     cap;  /*容量，单位为bit*/
    struct {
        void *(*guard_ctor)(size_t);
        void  (*guard_dtor)(void *);
    } allocator;
} kbitmap_opts_t;

extern kbitmap_t *kylin_bitmap_create(const kbitmap_opts_t *opts);
extern void kylin_bitmap_destroy(kbitmap_t *);

extern kbitmap_t *kylin_bitmp_dup(const kbitmap_t *);
extern kbitmap_t *kylin_bitmap_loads(kbitmap_opts_t *opts, const char *buf);
extern kbitmap_t *kylin_bitmap_loadf(kbitmap_opts_t *opts, const FILE *buf);

extern unsigned long *kylin_bitmap_val(kbitmap_t *);
extern size_t kylin_bitmap_len(kbitmap_t *);

extern kerr_t kylin_bitmap_set(kbitmap_t *, unsigned int start, int len);
extern kerr_t kylin_bitmap_clear(kbitmap_t *, unsigned int start, int len);
extern kerr_t kylin_bitmap_copy(kbitmap_t *dst, const kbitmap_t *src, size_t size);

extern kerr_t kylin_bitmap_shift_right(kbitmap_t *, unsigned int shift);
extern kerr_t kylin_bitmap_shift_left(kbitmap_t *, unsigned int shift);

/*
 * bit operations
 */
extern kerr_t kylin_bitmap_and(kbitmap_t *dst, const kbitmap_t *src1, const kbitmap_t *src2);
extern kerr_t kylin_bitmap_or(kbitmap_t *dst, const kbitmap_t *src1, const kbitmap_t *src2);
extern kerr_t kylin_bitmap_xor(kbitmap_t *dst, const kbitmap_t *src1, const kbitmap_t *src2);
extern kerr_t kylin_bitmap_andnot(kbitmap_t *dst, const kbitmap_t *src1, const kbitmap_t *src2);
extern kerr_t kylin_bitmap_complement(kbitmap_t *dst, const kbitmap_t *src);

extern kbool_t kylin_bitmap_is_equal(const kbitmap_t *src1, const kbitmap_t *src2);
extern kbool_t kylin_bitmap_is_intersects(const kbitmap_t *src1, const kbitmap_t *src2);
extern kbool_t kylin_bitmap_is_subset(const kbitmap_t *src1, const kbitmap_t *src2);
extern kbool_t kylin_bitmap_is_empty(const kbitmap_t *src);
extern kbool_t kylin_bitmap_is_full(const kbitmap_t *src);

#endif /*_KYLIN_MATH_BITMAP_H_*/
