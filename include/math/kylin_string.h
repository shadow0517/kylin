#ifndef _KYLIN_MATH_STRING_H_
#define _KYLIN_MATH_STRING_H_

#include <kylin/include/utils/kylin_error.h>

/*
 * 提供字符串的操作，将字符串作为一种基础类型
 */

#ifndef __SIZEOF_CHAR__
#define __SIZEOF_CHAR__ sizeof(char)
#endif

struct kylin_string;
/*struct kylin_string_iterator;*/
typedef struct kylin_string          kstr_t;
/*typedef struct kylin_string_iterator kstr_iter_t;*/

typedef struct {
    const char *name;
    size_t      cap; /*容量，单位为byte，实际容量为cap + 1，最后一个byte用来存放'\0'*/
    struct {
        void *(*guard_ctor)(size_t);
        void  (*guard_dtor)(void *);
    } allocator;
} kstr_opts_t;

extern kstr_t *kylin_string_create(const kstr_opts_t *opts);
extern void kylin_string_destroy(kstr_t *);

extern char *kylin_string_val(kstr_t *);
extern void kylin_string_zero(kstr_t *);

/*
 * 实现部分由库函数提供的字符串操作
 */
/*Return the length of S*/
extern size_t kylin_strlen(const kstr_t *s);

/*Duplicate S, returning an identical malloc'd string*/
extern kstr_t *kylin_strdup(const kstr_t *s);
extern char *kylin_strdup_raw(const kstr_t *s);

/*Duplicate S, returning a copy of at most N bytes of S*/
extern kstr_t *kylin_strndup(const kstr_t *s, size_t n);
extern char *kylin_strndup_raw(const kstr_t *s, size_t n);

/*Compare S1 and S2*/
extern int kylin_strcmp(const kstr_t *s1, const kstr_t *s2);
extern int kylin_strcmp_raw(const kstr_t *s1, const char *s2);

/*Compare N characters of S1 and S2*/
extern int kylin_strncmp(const kstr_t *s1, const kstr_t *s2, size_t n);
extern int kylin_strncmp_raw(const kstr_t *s1, const char *s2, size_t n);

/*Copy SRC to DEST*/
extern kstr_t *kylin_strcpy(kstr_t *dest, const kstr_t *src);
extern kstr_t *kylin_strcpy_raw(kstr_t *dest, const char *src);

/*Copy no more than N characters of SRC to DEST*/
extern kstr_t *kylin_strncpy(kstr_t *dest, const kstr_t *src, size_t n);
extern kstr_t *kylin_strncpy_raw(kstr_t *dest, const char *src, size_t n);

/*Append SRC onto DEST*/
extern kstr_t *kylin_strcat(kstr_t *dest, const kstr_t *src);
extern kstr_t *kylin_strcat_raw(kstr_t *dest, const char *src);

/*Append no more than N characters from SRC onto DEST*/
extern kstr_t *kylin_strncat(kstr_t *dest, const kstr_t *src, size_t n);
extern kstr_t *kylin_strncat_raw(kstr_t *dest, const char *src, size_t n);

/*Find the first occurrence of C in S*/
extern char *kylin_strchr(kstr_t *s, char c);

/*Find the last occurrence of C in S*/
extern char *kylin_strrchr(kstr_t *s, char c);

/*Find the first occurrence in S of any character in ACPT*/
extern char *kylin_strpbrk(kstr_t *s, const char *acpt);

/*Find the first occurrence of ACPT in S*/
extern char *kylin_strstr(kstr_t *s, const kstr_t *acpt);
extern char *kylin_strstr_raw(kstr_t *s, const char *acpt);

/*Divide S into tokens separated by characters in ACPT, returns array of string*/
extern kstr_t *kylin_strtok(const kstr_t *s, const char *acpt);

#endif /*_KYLIN_MATH_STRING_H_*/
