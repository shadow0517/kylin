#ifndef _KYLIN_UTILS_HASH_H_
#define _KYLIN_UTILS_HASH_H_

#include <kylin/include/kylin.h>  

/* Best hash sizes are of power of two */
#define kylin_hash_size(n)   ((uint32_t)1<<(n))
/* Mask the hash value, i.e (value & kylin_hash_mask(n)) instead of (value % n) */
#define kylin_hash_mask(n)   (kylin_hash_size(n)-1)

/**
 * rol32 - rotate a 32-bit value left
 * @word: value to rotate
 * @shift: bits to roll
 */
#define rol32(word, shift) (((word) << (shift)) | ((word) >> (32 - (shift))))

/* __kylin_hash_mix -- mix 3 32-bit values reversibly. */
#define __kylin_hash_mix(a, b, c)       \
{                                       \
    a -= c;  a ^= rol32(c, 4);  c += b; \
    b -= a;  b ^= rol32(a, 6);  a += c; \
    c -= b;  c ^= rol32(b, 8);  b += a; \
    a -= c;  a ^= rol32(c, 16); c += b; \
    b -= a;  b ^= rol32(a, 19); a += c; \
    c -= b;  c ^= rol32(b, 4);  b += a; \
}

/* __kylin_hash_final - final mixing of 3 32-bit values (a,b,c) into c */
#define __kylin_hash_final(a, b, c) \
{                                   \
    c ^= b; c -= rol32(b, 14);      \
    a ^= c; a -= rol32(c, 11);      \
    b ^= a; b -= rol32(a, 25);      \
    c ^= b; c -= rol32(b, 16);      \
    a ^= c; a -= rol32(c, 4);       \
    b ^= a; b -= rol32(a, 14);      \
    c ^= b; c -= rol32(b, 24);      \
}

/* An arbitrary initial parameter */
#define JHASH_INITVAL       0xdeadbeef

/* kylin_hash - hash an arbitrary key
 * @k: sequence of bytes as key
 * @length: the length of the key
 * @initval: the previous hash, or an arbitray value
 *
 * The generic version, hashes an arbitrary sequence of bytes.
 * No alignment or length assumptions are made about the input key.
 *
 * Returns the hash value of the key. The result depends on endianness.
 */
static inline uint32_t kylin_hash(void *key, uint32_t length, uint32_t initval)
{
    uint32_t a, b, c;
    uint8_t *k = key;

    /* Set up the internal state */
    a = b = c = JHASH_INITVAL + length + initval;

    /* All but the last block: affect some 32 bits of (a,b,c) */
    while (length > 12) {
        a += *(uint32_t *)(k);
        b += *(uint32_t *)(k + 4);
        c += *(uint32_t *)(k + 8);
        __kylin_hash_mix(a, b, c);
        length -= 12;
        k += 12;
    }
    /* Last block: affect all 32 bits of (c) */
    /* All the case statements fall through */
    switch (length) {
    case 12: c += (uint32_t)k[11]<<24;
    case 11: c += (uint32_t)k[10]<<16;
    case 10: c += (uint32_t)k[9]<<8;
    case 9:  c += k[8];
    case 8:  b += (uint32_t)k[7]<<24;
    case 7:  b += (uint32_t)k[6]<<16;
    case 6:  b += (uint32_t)k[5]<<8;
    case 5:  b += k[4];
    case 4:  a += (uint32_t)k[3]<<24;
    case 3:  a += (uint32_t)k[2]<<16;
    case 2:  a += (uint32_t)k[1]<<8;
    case 1:  a += k[0];
         __kylin_hash_final(a, b, c);
    case 0: /* Nothing left to add */
        break;
    }

    return c;
}

/* kylin_hash2 - hash an array of u32's
 * @k: the key which must be an array of u32's
 * @length: the number of u32's in the key
 * @initval: the previous hash, or an arbitray value
 *
 * Returns the hash value of the key.
 */
static inline uint32_t kylin_hash2(const uint32_t *k, uint32_t length, uint32_t initval)
{
    uint32_t a, b, c;

    /* Set up the internal state */
    a = b = c = JHASH_INITVAL + (length<<2) + initval;

    /* Handle most of the key */
    while (length > 3) {
        a += k[0];
        b += k[1];
        c += k[2];
        __kylin_hash_mix(a, b, c);
        length -= 3;
        k += 3;
    }

    /* Handle the last 3 uint32_t's: all the case statements fall through */
    switch (length) {
    case 3: c += k[2];
    case 2: b += k[1];
    case 1: a += k[0];
        __kylin_hash_final(a, b, c);
    case 0: /* Nothing left to add */
        break;
    }

    return c;
}

/* kylin_hash_3words - hash exactly 3, 2 or 1 word(s) */
static inline uint32_t kylin_hash_3words(uint32_t a, uint32_t b, uint32_t c, 
        uint32_t initval)
{
    a += JHASH_INITVAL;
    b += JHASH_INITVAL;
    c += initval;

    __kylin_hash_final(a, b, c);

    return c;
}

static inline uint32_t kylin_hash_2words(uint32_t a, uint32_t b, uint32_t initval)
{
    return kylin_hash_3words(a, b, 0, initval);
}

static inline uint32_t kylin_hash_1word(uint32_t a, uint32_t initval)
{
    return kylin_hash_3words(a, 0, 0, initval);
}

#endif /*_KYLIN_UTILS_HASH_H_*/

