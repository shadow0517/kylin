#ifndef _KYLIN_CORE_MODULE_ELF_H_
#define _KYLIN_CORE_MODULE_ELF_H_

#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_error.h>

typedef struct kylin_elf_descriptor
{
    int         fd;
    int         size;
    Elf32_Ehdr *ehdr;
} ked_t;

extern ked_t *kylin_elf_create(const char *);
extern void kylin_elf_destroy(ked_t *);

typedef kerr_t (*symbol_hook)(char *, uint64_t, void *);

extern kerr_t kylin_elf_symbol(ked_t *, symbol_hook, void *);
extern kerr_t kylin_elf_dump(ked_t *, kylin_print);

#endif /*_KYLIN_CORE_MODULE_ELF_H_*/
