#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_limit.h>
#include <kylin/include/arch/x86/kylin_bitops.h>
#include <kylin/include/math/kylin_bitmap.h>

#define BITMAP_WORD_INDEX(nr)         (nr / BITS_PER_WORD)
#define BITMAP_FIRST_WORD_MASK(start) (~0UL << ((start) & (BITS_PER_WORD - 1)))
#define BITMAP_LAST_WORD_MASK(nbits)  (~0UL >> (-(nbits) & (BITS_PER_WORD - 1)))

struct kylin_bitmap {
    kbitmap_opts_t opts;
    size_t         size;
    kword_t        val[];
};

#define BITMAP_GUARD_MALLOC(opts) ((opts)->allocator.guard_ctor ? (opts)->allocator.guard_ctor : malloc)
#define BITMAP_GUARD_FREE(opts)   ((opts)->allocator.guard_dtor ? (opts)->allocator.guard_dtor : free)

kbitmap_t *kylin_bitmap_create(const kbitmap_opts_t *opts)
{
    kbitmap_t *bm = NULL;
    size_t len = BITS_TO_WORDS(opts->cap);

    bm = BITMAP_GUARD_MALLOC(opts)(sizeof(kbitmap_t) + (__SIZEOF_WORD__ * len));
    if(bm == NULL)
        return NULL;
    memset(bm, 0, (sizeof(kbitmap_t) + (__SIZEOF_WORD__ * len)));

    memcpy(&bm->opts, opts, sizeof(kbitmap_opts_t));
    bm->size = len;

    return bm;
}

void kylin_bitmap_destroy(kbitmap_t *bm)
{
    BITMAP_GUARD_FREE(&bm->opts)(bm);
}

kbitmap_t *kylin_bitmap_dup(const kbitmap_t *bm)
{
    kbitmap_t *dup = NULL;

    dup = kylin_bitmap_create(&bm->opts);
    if(dup == NULL)
        return NULL;

    if(KYLIN_ERROR_OK != kylin_bitmap_copy(dup, bm, bm->size)) {
        kylin_bitmap_destroy(dup);
        return NULL;
    }

    return dup;
}

kbitmap_t *kylin_bitmap_loads(kbitmap_opts_t *opts, const char *buf)
{
    return NULL;
}

kbitmap_t *kylin_bitmap_loadf(kbitmap_opts_t *opts, const FILE *buf)
{
    return NULL;
}

kword_t *kylin_bitmap_val(kbitmap_t *bm)
{
    return bm->val;
}

size_t kylin_bitmap_len(kbitmap_t *bm)
{
    return bm->size;
}

kerr_t kylin_bitmap_set(kbitmap_t *bm, unsigned int start, int len)
{
    kerr_t ret = KYLIN_ERROR_OK;
    kword_t *val = NULL;
    kword_t mask = 0;
    int bits = 0;

    if(BITS_TO_WORDS(start) > bm->size || BITS_TO_WORDS(start + len) > bm->size) {
        ret = KYLIN_ERROR_INVAL;
        goto exit;
    }

	val = bm->val + BITMAP_WORD_INDEX(start);
	mask = BITMAP_FIRST_WORD_MASK(start);
	bits = BITS_PER_WORD - (start % BITS_PER_WORD);

	while (len - bits >= 0) {
		*val |= mask;
		len -= bits;
		bits = BITS_PER_WORD;
		mask = ~0UL;
		val++;
	}

	if (len) {
		mask &= BITMAP_LAST_WORD_MASK(start + len);
		*val |= mask;
	}

exit:
    return ret;
}

kerr_t kylin_bitmap_clear(kbitmap_t *bm, unsigned int start, int len)
{
    kerr_t ret = KYLIN_ERROR_OK;
    kword_t *val = NULL;
    kword_t mask = 0;
    int bits = 0;

    if(BITS_TO_WORDS(start) > bm->size || BITS_TO_WORDS(start + len) > bm->size) {
        ret = KYLIN_ERROR_INVAL;
        goto exit;
    }

	val = bm->val + BITMAP_WORD_INDEX(start);
	mask = BITMAP_FIRST_WORD_MASK(start);
	bits = BITS_PER_WORD - (start % BITS_PER_WORD);

	while (len - bits >= 0) {
		*val &= ~mask;
		len -= bits;
		bits = BITS_PER_WORD;
		mask = ~0UL;
		val++;
	}

	if (len) {
		mask &= BITMAP_LAST_WORD_MASK(start + len);
		*val &= ~mask;
	}

exit:
    return ret;
}

kerr_t kylin_bitmap_copy(kbitmap_t *dst, const kbitmap_t *src, size_t size)
{
    kerr_t ret = KYLIN_ERROR_OK;
    if(size > src->size || size > dst->size) {
        ret = KYLIN_ERROR_INVAL;
        goto exit;
    }

    memcpy(dst->val, src->val, __SIZEOF_WORD__ * size);

exit:
    return ret;
}

kerr_t kylin_bitmap_shift_right(kbitmap_t *bm, unsigned int shift)
{
    kerr_t ret = KYLIN_ERROR_OK;
    size_t size = bm->size;
    unsigned offset = shift / BITS_PER_WORD;
    unsigned remainder = shift % BITS_PER_WORD;
    kword_t *dst = NULL;
    kword_t upper, lower;

    if(offset > size || (offset == size && remainder)) {
        ret = KYLIN_ERROR_INVAL;
        goto exit;
    }

    dst = malloc(size * __SIZEOF_WORD__);
    if(dst == NULL) {
        ret = KYLIN_ERROR_NOMEM;
        goto exit;
    }

    for(int i = 0; i + offset < size; ++i) {
        if(!remainder || offset + i + 1 >= size)
            upper = 0;
        else {
            upper = bm->val[offset + i + 1];
            upper <<= (BITS_PER_WORD - remainder);
        }
        lower = bm->val[offset + i];
        lower >>= remainder;
        dst[i] = lower | upper;
    }

    if(offset)
        memset(&dst[size - offset], 0, offset * __SIZEOF_WORD__);

    memcpy(bm->val, dst, size * __SIZEOF_WORD__);

exit:
    if(dst)
        free(dst);

    return ret;
}

kerr_t kylin_bitmap_shift_left(kbitmap_t *bm, unsigned int shift)
{
    kerr_t ret = KYLIN_ERROR_OK;
    size_t size = bm->size;
    unsigned offset = shift / BITS_PER_WORD;
    unsigned remainder = shift % BITS_PER_WORD;
    kword_t *dst = NULL;
    kword_t upper, lower;

    if(offset > size || (offset == size && remainder)) {
        ret = KYLIN_ERROR_INVAL;
        goto exit;
    }

    dst = malloc(size * __SIZEOF_WORD__);
    if(dst == NULL) {
        ret = KYLIN_ERROR_NOMEM;
        goto exit;
    }

    for(int i = size - offset - 1; i >= 0; --i) {
        if(remainder && i > 0)
            lower = bm->val[i - 1] >> (BITS_PER_WORD - remainder);
        else
            lower = 0;
        upper = bm->val[i] << remainder;
        dst[i + offset] = lower | upper;
    }

    if(offset)
        memset(dst, 0, offset * __SIZEOF_WORD__);

    memcpy(bm->val, dst, size * __SIZEOF_WORD__);

exit:
    if(dst)
        free(dst);

    return ret;
}

/*
 * bit operations
 */
kerr_t kylin_bitmap_and(kbitmap_t *dst, const kbitmap_t *src1, const kbitmap_t *src2)
{
    kerr_t ret = KYLIN_ERROR_OK;
    size_t size = 0;

    if(dst->size < 1 || src1->size < 1 || src2->size < 1) {
        ret = KYLIN_ERROR_INVAL;
        goto exit;
    }

    size = KYLIN_MIN(dst->size, KYLIN_MIN(src1->size, src2->size));

	for(int i = 0; i < size; i++)
		dst->val[i] = src1->val[i] & src2->val[i];

exit:
    return ret;
}

kerr_t kylin_bitmap_or(kbitmap_t *dst, const kbitmap_t *src1, const kbitmap_t *src2)
{
    kerr_t ret = KYLIN_ERROR_OK;
    size_t size = 0;

    if(dst->size < 1 || src1->size < 1 || src2->size < 1) {
        ret = KYLIN_ERROR_INVAL;
        goto exit;
    }

    size = KYLIN_MAX(dst->size, KYLIN_MAX(src1->size, src2->size));
    if(dst->size != size) {
        ret = KYLIN_ERROR_NOENT;
        goto exit;
    }

    for(int i = 0; i < size; i++) {
        if(i < src1->size && i < src2->size)
            dst->val[i] = src1->val[i] | src2->val[i];
        else if(i < src1->size)
            dst->val[i] = src1->val[i];
        else
            dst->val[i] = src2->val[i];
    }

exit:
    return ret;
}

kerr_t kylin_bitmap_xor(kbitmap_t *dst, const kbitmap_t *src1, const kbitmap_t *src2)
{
    kerr_t ret = KYLIN_ERROR_OK;
    size_t size = 0;

    if(dst->size < 1 || src1->size < 1 || src2->size < 1) {
        ret = KYLIN_ERROR_INVAL;
        goto exit;
    }

    size = KYLIN_MIN(dst->size, KYLIN_MIN(src1->size, src2->size));

	for(int i = 0; i < size; i++)
		dst->val[i] = src1->val[i] ^ src2->val[i];

exit:
    return ret;
}

kerr_t kylin_bitmap_andnot(kbitmap_t *dst, const kbitmap_t *src1, const kbitmap_t *src2)
{
    kerr_t ret = KYLIN_ERROR_OK;
    size_t size = 0;

    if(dst->size < 1 || src1->size < 1 || src2->size < 1) {
        ret = KYLIN_ERROR_INVAL;
        goto exit;
    }

    size = KYLIN_MIN(dst->size, KYLIN_MIN(src1->size, src2->size));

	for(int i = 0; i < size; i++)
		dst->val[i] = src1->val[i] & ~src2->val[i];

exit:
    return ret;
}

kerr_t kylin_bitmap_complement(kbitmap_t *dst, const kbitmap_t *src)
{
    kerr_t ret = KYLIN_ERROR_OK;
    size_t size = 0;

    if(dst->size < 1 || src->size < 1) {
        ret = KYLIN_ERROR_INVAL;
        goto exit;
    }

    size = KYLIN_MIN(dst->size, src->size);

    for(int i = 0; i < size; i++)
        dst->val[i] = ~src->val[i];

exit:
    return ret;
}

kbool_t kylin_bitmap_is_equal(const kbitmap_t *src1, const kbitmap_t *src2)
{
    kbool_t ret = KYLIN_TRUE;
    size_t size = 0;

    if(src1->size < 1 || src2->size < 1 || src1->size != src2->size) {
        ret = KYLIN_FALSE;
        goto exit;
    }

    size = src1->size;

    for(int i = 0; i < size; i++) {
        if(src1->val[i] != src2->val[i]) {
            ret = KYLIN_FALSE;
            goto exit;
        }
    }

exit:
    return ret;
}

kbool_t kylin_bitmap_is_intersects(const kbitmap_t *src1, const kbitmap_t *src2)
{
    size_t size = KYLIN_MIN(src1->size, src2->size);

    for(int i = 0; i < size; i++) {
        if(src1->val[i] & src2->val[i])
            return KYLIN_TRUE;
    }
    return KYLIN_FALSE;
}

kbool_t kylin_bitmap_is_subset(const kbitmap_t *parent, const kbitmap_t *child)
{
    size_t size = KYLIN_MIN(parent->size, child->size);

    if(size > parent->size)
        return KYLIN_FALSE;

    for(int i = 0; i < size; i++) {
        if(child->val[i] & ~parent->val[i])
            return KYLIN_FALSE;
    }
    return KYLIN_TRUE;
}

kbool_t kylin_bitmap_is_empty(const kbitmap_t *src)
{
    size_t size = src->size;

    for(int i = 0; i < size; i++) {
        if(src->val[i])
            return KYLIN_FALSE;
    }
    return KYLIN_TRUE;
}

kbool_t kylin_bitmap_is_full(const kbitmap_t *src)
{
    size_t size = src->size;

    for(int i = 0; i < size; i++) {
        if(~src->val[i])
            return KYLIN_FALSE;
    }
    return KYLIN_TRUE;
}

