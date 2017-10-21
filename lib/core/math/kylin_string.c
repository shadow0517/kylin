#include <kylin/include/kylin.h>
#include <kylin/include/math/kylin_string.h>

struct kylin_string {
    kstr_opts_t opts;
    size_t      used; /*不包括字符串最后的'\0'*/
    char        val[];
};

#define STRING_GUARD_MALLOC(opts) ((opts)->allocator.guard_ctor ? (opts)->allocator.guard_ctor : kylin_malloc)
#define STRING_GUARD_FREE(opts)   ((opts)->allocator.guard_dtor ? (opts)->allocator.guard_dtor : kylin_free)

kstr_t *kylin_string_create(const kstr_opts_t *opts)
{
    kstr_t *str = NULL;

    str = STRING_GUARD_MALLOC(opts)(sizeof(kstr_t) + (__SIZEOF_CHAR__ * opts->cap + 1));
    if(!str) {
        kerrno = KYLIN_ERROR_NOMEM;
        return NULL;
    }

    memcpy(&str->opts, opts, sizeof(kstr_opts_t));
    memset(str->val, 0, (__SIZEOF_CHAR__ * opts->cap + 1));
    str->used = 0;

    return str;
}

void kylin_string_destroy(kstr_t *s)
{
    STRING_GUARD_FREE(&s->opts)(s);
}

char *kylin_string_val(kstr_t *s)
{
    return s->val;
}

void kylin_string_zero(kstr_t *s)
{
    memset(s->val, 0, s->used);
    s->used = 0;
}

size_t kylin_strlen(const kstr_t *s)
{
    return s->used;
}

kstr_t *kylin_strdup(const kstr_t *s)
{
    return kylin_strndup(s, s->used);
}

char *kylin_strdup_raw(const kstr_t *s)
{
    return kylin_strndup_raw(s, s->used);
}

kstr_t *kylin_strndup(const kstr_t *s, size_t n)
{
    kstr_t *s_dup = NULL;

    s_dup = kylin_string_create(&s->opts);
    if(!s_dup)
        return NULL;

    memcpy(s_dup->val, s->val, KYLIN_MIN(s->used, n));
    s_dup->used = KYLIN_MIN(s->used, n);

    return s_dup;
}

char *kylin_strndup_raw(const kstr_t *s, size_t n)
{
    char *s_dup = NULL;

    s_dup = kylin_malloc(__SIZEOF_CHAR__ * KYLIN_MIN(s->used, n) + 1);
    if(!s_dup) {
        kerrno = KYLIN_ERROR_NOMEM;
        return NULL;
    }

    memcpy(s_dup, s->val, KYLIN_MIN(s->used, n));
    s_dup[KYLIN_MIN(s->used, n)] = '\0';

    return s_dup;
}

int kylin_strcmp(const kstr_t *s1, const kstr_t *s2)
{
    return memcmp(s1->val, s2->val, KYLIN_MIN(s1->used, s2->used));
}

int kylin_strcmp_raw(const kstr_t *s1, const char *s2)
{
    return memcmp(s1->val, s2, KYLIN_MIN(s1->used, strlen(s2)));
}

int kylin_strncmp(const kstr_t *s1, const kstr_t *s2, size_t n)
{
    return memcmp(s1->val, s2->val, n);
}

int kylin_strncmp_raw(const kstr_t *s1, const char *s2, size_t n)
{
    return memcmp(s1->val, s2, n);
}

kstr_t *kylin_strcpy(kstr_t *dest, const kstr_t *src)
{
    if(src->used > dest->opts.cap) {
        kerrno = KYLIN_ERROR_INVAL;
        return NULL;
    }

    memcpy(dest->val, src->val, src->used);
    dest->used = src->used;

    return dest;
}

kstr_t *kylin_strcpy_raw(kstr_t *dest, const char *src)
{
    size_t len = strlen(src);

    if(len > dest->opts.cap) {
        kerrno = KYLIN_ERROR_INVAL;
        return NULL;
    }

    memcpy(dest->val, src, len);
    dest->used = len;

    return dest;
}

kstr_t *kylin_strncpy(kstr_t *dest, const kstr_t *src, size_t n)
{
    if(n > src->used || n > dest->opts.cap) {
        kerrno = KYLIN_ERROR_INVAL;
        return NULL;
    }

    memcpy(dest->val, src->val, n);
    dest->used = n;

    return dest;
}

kstr_t *kylin_strncpy_raw(kstr_t *dest, const char *src, size_t n)
{
    size_t len = strlen(src);

    if(n > len || n > dest->opts.cap) {
        kerrno = KYLIN_ERROR_INVAL;
        return NULL;
    }

    memcpy(dest->val, src, n);
    dest->used = n;

    return dest;
}

kstr_t *kylin_strcat(kstr_t *dest, const kstr_t *src)
{
    return kylin_strncat(dest, src, src->used);
}

kstr_t *kylin_strcat_raw(kstr_t *dest, const char *src)
{
    return kylin_strncat_raw(dest, src, strlen(src));
}

kstr_t *kylin_strncat(kstr_t *dest, const kstr_t *src, size_t n)
{
    if(n > src->used || n > (dest->opts.cap - dest->used)) {
        kerrno = KYLIN_ERROR_INVAL;
        return NULL;
    }

    memcpy(dest->val + dest->used, src->val, n);

    return dest;
}

kstr_t *kylin_strncat_raw(kstr_t *dest, const char *src, size_t n)
{
    size_t len = strlen(src);

    if(n > len || n > (dest->opts.cap - dest->used)) {
        kerrno = KYLIN_ERROR_INVAL;
        return NULL;
    }

    memcpy(dest->val + dest->used, src, len);

    return dest;
}

char *kylin_strchr(kstr_t *s, char c)
{
    for(int i = 0; i < s->used; i++) {
        if(s->val[i] == c)
            return (s->val + i);
    }

    return NULL;
}

char *kylin_strrchr(kstr_t *s, char c)
{
    for(int i = s->used - 1; i >= 0; i--) {
        if(s->val[i] == c)
            return (s->val + i);
    }

    return NULL;
}

char *kylin_strpbrk(kstr_t *s, const char *acpt)
{
    size_t len = strlen(acpt);

    for(int i = 0; i < s->used; i++) {
        for(int j = 0; j < len; j++) {
            if(s->val[i] == acpt[j])
                return (s->val + i);
        }
    }

    return NULL;
}

char *kylin_strstr(kstr_t *s, const kstr_t *acpt)
{
    if(acpt->used > s->used) {
        kerrno = KYLIN_ERROR_INVAL;
        return NULL;
    }

    for(int i = 0; s->val[i] == acpt->val[0]; i++) {
        if(kylin_strncmp_raw(acpt, (s->val + i), acpt->used) == 0)
            return (s->val + i);
    }

    return NULL;
}

char *kylin_strstr_raw(kstr_t *s, const char *acpt)
{
    size_t len = strlen(acpt);

    if(len > s->used) {
        kerrno = KYLIN_ERROR_INVAL;
        return NULL;
    }

    for(int i = 0; s->val[i] == acpt[0]; i++) {
        if(memcmp(acpt, (s->val + i), len) == 0)
            return (s->val + i);
    }

    return NULL;
}

kstr_t *kylin_strtok(const kstr_t *s, const char *acpt)
{
    size_t len = strlen(acpt);
    kstr_t *s_dup = NULL;

    s_dup = kylin_strdup(s);
    if(!s_dup)
        return NULL;

    for(int i = 0; i < s->used; i++) {
        for(int j = 0; j < len; j++) {
            if(s->val[i] == acpt[j])
                s_dup->val[i] = '\0';
        }
    }

    return s_dup;
}

