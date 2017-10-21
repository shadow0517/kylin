#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_error.h>

#include <kylin/lib/core/kylin_core_module_elf.h>

#if defined(__linux__)
/* Note header in a PT_NOTE section */
typedef struct elf_note {
    Elf32_Word    n_namesz;   /* Name size */
    Elf32_Word    n_descsz;   /* Content size */
    Elf32_Word    n_type;     /* Content type */
} Elf_Note;
#endif /* __linux__ */

typedef enum elf_member {
	D_TAG = 1, D_PTR, D_VAL,

	E_CLASS, E_DATA, E_OSABI, E_TYPE, E_MACHINE, E_VERSION, E_ENTRY,
	E_PHOFF, E_SHOFF, E_FLAGS, E_EHSIZE, E_PHENTSIZE, E_PHNUM, E_SHENTSIZE,
	E_SHNUM, E_SHSTRNDX,

	N_NAMESZ, N_DESCSZ, N_TYPE,

	P_TYPE, P_OFFSET, P_VADDR, P_PADDR, P_FILESZ, P_MEMSZ, P_FLAGS,
	P_ALIGN,

	SH_NAME, SH_TYPE, SH_FLAGS, SH_ADDR, SH_OFFSET, SH_SIZE, SH_LINK,
	SH_INFO, SH_ADDRALIGN, SH_ENTSIZE,

	ST_NAME, ST_VALUE, ST_SIZE, ST_INFO, ST_SHNDX,

	R_OFFSET, R_INFO,

	RA_OFFSET, RA_INFO, RA_ADDEND
} elf_member_t;

int elf32_offsets[] = {
	0,

	offsetof(Elf32_Dyn, d_tag), offsetof(Elf32_Dyn, d_un.d_ptr),
	offsetof(Elf32_Dyn, d_un.d_val),

	offsetof(Elf32_Ehdr, e_ident[EI_CLASS]),
	offsetof(Elf32_Ehdr, e_ident[EI_DATA]),
	offsetof(Elf32_Ehdr, e_ident[EI_OSABI]),
	offsetof(Elf32_Ehdr, e_type), offsetof(Elf32_Ehdr, e_machine),
	offsetof(Elf32_Ehdr, e_version), offsetof(Elf32_Ehdr, e_entry),
	offsetof(Elf32_Ehdr, e_phoff), offsetof(Elf32_Ehdr, e_shoff),
	offsetof(Elf32_Ehdr, e_flags), offsetof(Elf32_Ehdr, e_ehsize),
	offsetof(Elf32_Ehdr, e_phentsize), offsetof(Elf32_Ehdr, e_phnum),
	offsetof(Elf32_Ehdr, e_shentsize), offsetof(Elf32_Ehdr, e_shnum),
	offsetof(Elf32_Ehdr, e_shstrndx),

	offsetof(Elf_Note, n_namesz), offsetof(Elf_Note, n_descsz),
	offsetof(Elf_Note, n_type),

	offsetof(Elf32_Phdr, p_type), offsetof(Elf32_Phdr, p_offset),
	offsetof(Elf32_Phdr, p_vaddr), offsetof(Elf32_Phdr, p_paddr),
	offsetof(Elf32_Phdr, p_filesz), offsetof(Elf32_Phdr, p_memsz),
	offsetof(Elf32_Phdr, p_flags), offsetof(Elf32_Phdr, p_align),

	offsetof(Elf32_Shdr, sh_name), offsetof(Elf32_Shdr, sh_type),
	offsetof(Elf32_Shdr, sh_flags), offsetof(Elf32_Shdr, sh_addr),
	offsetof(Elf32_Shdr, sh_offset), offsetof(Elf32_Shdr, sh_size),
	offsetof(Elf32_Shdr, sh_link), offsetof(Elf32_Shdr, sh_info),
	offsetof(Elf32_Shdr, sh_addralign), offsetof(Elf32_Shdr, sh_entsize),

	offsetof(Elf32_Sym, st_name), offsetof(Elf32_Sym, st_value),
	offsetof(Elf32_Sym, st_size), offsetof(Elf32_Sym, st_info),
	offsetof(Elf32_Sym, st_shndx),

	offsetof(Elf32_Rel, r_offset), offsetof(Elf32_Rel, r_info),

	offsetof(Elf32_Rela, r_offset), offsetof(Elf32_Rela, r_info),
	offsetof(Elf32_Rela, r_addend)
};

int elf64_offsets[] = {
	0,

	offsetof(Elf64_Dyn, d_tag), offsetof(Elf64_Dyn, d_un.d_ptr),
	offsetof(Elf64_Dyn, d_un.d_val),

	offsetof(Elf32_Ehdr, e_ident[EI_CLASS]),
	offsetof(Elf32_Ehdr, e_ident[EI_DATA]),
	offsetof(Elf32_Ehdr, e_ident[EI_OSABI]),
	offsetof(Elf64_Ehdr, e_type), offsetof(Elf64_Ehdr, e_machine),
	offsetof(Elf64_Ehdr, e_version), offsetof(Elf64_Ehdr, e_entry),
	offsetof(Elf64_Ehdr, e_phoff), offsetof(Elf64_Ehdr, e_shoff),
	offsetof(Elf64_Ehdr, e_flags), offsetof(Elf64_Ehdr, e_ehsize),
	offsetof(Elf64_Ehdr, e_phentsize), offsetof(Elf64_Ehdr, e_phnum),
	offsetof(Elf64_Ehdr, e_shentsize), offsetof(Elf64_Ehdr, e_shnum),
	offsetof(Elf64_Ehdr, e_shstrndx),

	offsetof(Elf_Note, n_namesz), offsetof(Elf_Note, n_descsz),
	offsetof(Elf_Note, n_type),

	offsetof(Elf64_Phdr, p_type), offsetof(Elf64_Phdr, p_offset),
	offsetof(Elf64_Phdr, p_vaddr), offsetof(Elf64_Phdr, p_paddr),
	offsetof(Elf64_Phdr, p_filesz), offsetof(Elf64_Phdr, p_memsz),
	offsetof(Elf64_Phdr, p_flags), offsetof(Elf64_Phdr, p_align),

	offsetof(Elf64_Shdr, sh_name), offsetof(Elf64_Shdr, sh_type),
	offsetof(Elf64_Shdr, sh_flags), offsetof(Elf64_Shdr, sh_addr),
	offsetof(Elf64_Shdr, sh_offset), offsetof(Elf64_Shdr, sh_size),
	offsetof(Elf64_Shdr, sh_link), offsetof(Elf64_Shdr, sh_info),
	offsetof(Elf64_Shdr, sh_addralign), offsetof(Elf64_Shdr, sh_entsize),

	offsetof(Elf64_Sym, st_name), offsetof(Elf64_Sym, st_value),
	offsetof(Elf64_Sym, st_size), offsetof(Elf64_Sym, st_info),
	offsetof(Elf64_Sym, st_shndx),

	offsetof(Elf64_Rel, r_offset), offsetof(Elf64_Rel, r_info),

	offsetof(Elf64_Rela, r_offset), offsetof(Elf64_Rela, r_info),
	offsetof(Elf64_Rela, r_addend)
};

const char *e_types[] = {
	"ET_NONE", "ET_REL", "ET_EXEC", "ET_DYN", "ET_CORE"
};

const char *ei_versions[] = {
	"EV_NONE", "EV_CURRENT"
};

const char *ei_classes[] = {
	"ELFCLASSNONE", "ELFCLASS32", "ELFCLASS64"
};

const char *ei_data[] = {
	"ELFDATANONE", "ELFDATA2LSB", "ELFDATA2MSB"
};

const char *ei_abis[] = {
	"ELFOSABI_SYSV", "ELFOSABI_HPUX", "ELFOSABI_NETBSD", "ELFOSABI_LINUX",
	"ELFOSABI_HURD", "ELFOSABI_86OPEN", "ELFOSABI_SOLARIS",
	"ELFOSABI_MONTEREY", "ELFOSABI_IRIX", "ELFOSABI_FREEBSD",
	"ELFOSABI_TRU64", "ELFOSABI_MODESTO", "ELFOSABI_OPENBSD"
};

const char *p_types[] = {
	"PT_NULL", "PT_LOAD", "PT_DYNAMIC", "PT_INTERP", "PT_NOTE",
	"PT_SHLIB", "PT_PHDR", "PT_TLS"
};

const char *p_flags[] = {
	"", "PF_X", "PF_W", "PF_X|PF_W", "PF_R", "PF_X|PF_R", "PF_W|PF_R",
	"PF_X|PF_W|PF_R"
};

const char *sh_flags[] = {
	"", "SHF_WRITE", "SHF_ALLOC", "SHF_WRITE|SHF_ALLOC", "SHF_EXECINSTR",
	"SHF_WRITE|SHF_EXECINSTR", "SHF_ALLOC|SHF_EXECINSTR",
	"SHF_WRITE|SHF_ALLOC|SHF_EXECINSTR"
};

const char *st_types[] = {
	"STT_NOTYPE", "STT_OBJECT", "STT_FUNC", "STT_SECTION", "STT_FILE"
};

const char *st_bindings[] = {
	"STB_LOCAL", "STB_GLOBAL", "STB_WEAK"
};

static uint64_t __elf_get_byte(Elf32_Ehdr *e, void *base, elf_member_t member)
{
	u_int64_t val;

	val = 0;
	switch(e->e_ident[EI_CLASS]) {
	case ELFCLASS32:
		val = ((char *)base)[elf32_offsets[member]];
		break;
	case ELFCLASS64:
		val = ((char *)base)[elf64_offsets[member]];
		break;
	case ELFCLASSNONE:
        break;
	}

	return val;
}

static uint64_t __elf_get_word(Elf32_Ehdr *e, void *base, elf_member_t member)
{
	u_int64_t val;

	val = 0;
	switch(e->e_ident[EI_CLASS]) {
	case ELFCLASS32:
		base = (char *)base + elf32_offsets[member];
		switch(e->e_ident[EI_DATA]) {
		case ELFDATA2MSB:
			val = be32dec(base);
			break;
		case ELFDATA2LSB:
			val = le32dec(base);
			break;
		case ELFDATANONE:
            break;
		}
		break;
	case ELFCLASS64:
		base = (char *)base + elf64_offsets[member];
		switch(e->e_ident[EI_DATA]) {
		case ELFDATA2MSB:
			val = be32dec(base);
			break;
		case ELFDATA2LSB:
			val = le32dec(base);
			break;
		case ELFDATANONE:
            break;
		}
		break;
	case ELFCLASSNONE:
        break;
	}

	return val;
}

#define	__elf_get_addr	__elf_get_quad
#define	__elf_get_off	__elf_get_quad
#define	__elf_get_size	__elf_get_quad

static uint64_t __elf_get_quad(Elf32_Ehdr *e, void *base, elf_member_t member)
{
	u_int64_t val;

	val = 0;
	switch (e->e_ident[EI_CLASS]) {
	case ELFCLASS32:
		base = (char *)base + elf32_offsets[member];
		switch(e->e_ident[EI_DATA]) {
		case ELFDATA2MSB:
			val = be32dec(base);
			break;
		case ELFDATA2LSB:
			val = le32dec(base);
			break;
		case ELFDATANONE:
            break;
		}
		break;
	case ELFCLASS64:
		base = (char *)base + elf64_offsets[member];
		switch(e->e_ident[EI_DATA]) {
		case ELFDATA2MSB:
			val = be64dec(base);
			break;
		case ELFDATA2LSB:
			val = le64dec(base);
			break;
		case ELFDATANONE:
            break;
		}
		break;
	case ELFCLASSNONE:
        break;
	}

	return val;
}

static uint64_t __elf_get_quarter(Elf32_Ehdr *e, void *base, elf_member_t member)
{
	u_int64_t val;

	val = 0;
	switch(e->e_ident[EI_CLASS]) {
	case ELFCLASS32:
		base = (char *)base + elf32_offsets[member];
		switch(e->e_ident[EI_DATA]) {
		case ELFDATA2MSB:
			val = be16dec(base);
			break;
		case ELFDATA2LSB:
			val = le16dec(base);
			break;
		case ELFDATANONE:
            break;
		}
		break;
	case ELFCLASS64:
		base = (char *)base + elf64_offsets[member];
		switch(e->e_ident[EI_DATA]) {
		case ELFDATA2MSB:
			val = be16dec(base);
			break;
		case ELFDATA2LSB:
			val = le16dec(base);
			break;
		case ELFDATANONE:
            break;
		}
		break;
	case ELFCLASSNONE:
        break;
	}

	return val;
}

static void __elf_symtab_print(Elf32_Ehdr *e, void *sh, char *shstrtab, char *str, kylin_print print)
{
	uint64_t offset;
	uint64_t entsize;
	uint64_t size;
	uint64_t name;
	uint64_t value;
	void *st;
	int len;

	offset  = __elf_get_off(e, sh, SH_OFFSET);
	entsize = __elf_get_size(e, sh, SH_ENTSIZE);
	size    = __elf_get_size(e, sh, SH_SIZE);
	name    = __elf_get_word(e, sh, SH_NAME);
	len     = size / entsize;

	print("\nsymbol table (%s):\n", shstrtab + name);

	for(int i = 0; i < len; i++) {
	    uint64_t info;
	    uint64_t shndx;

		st    = (char *)e + offset + i * entsize;
		name  = __elf_get_word(e, st, ST_NAME);
		value = __elf_get_addr(e, st, ST_VALUE);
		size  = __elf_get_size(e, st, ST_SIZE);
		info  = __elf_get_byte(e, st, ST_INFO);
		shndx = __elf_get_quarter(e, st, ST_SHNDX);

		print("\n");
		print("entry: %d\n", i);
		print("\tst_name: %s\n", str + name);
		print("\tst_value: %#jx\n", value);
		print("\tst_size: %d\n", (int)size);

		print("\tst_info: %s %s\n",
		    st_types[ELF32_ST_TYPE(info)],
		    st_bindings[ELF32_ST_BIND(info)]);

		print("\tst_shndx: %d\n", (int)shndx);
	}
}

static int __elf_symtab_get(Elf32_Ehdr *e, void *sh, char *shstrtab, char *str, symbol_hook hook, void *data)
{
    int ret = KYLIN_ERROR_OK;
	uint64_t offset;
	uint64_t entsize;
	uint64_t size;
	uint64_t name;
	uint64_t value;
	void *st;
	int len;

	offset  = __elf_get_off(e, sh, SH_OFFSET);
	entsize = __elf_get_size(e, sh, SH_ENTSIZE);
	size    = __elf_get_size(e, sh, SH_SIZE);
	name    = __elf_get_word(e, sh, SH_NAME);
	len     = size / entsize;

	for(int i = 0; i < len; i++) {
        st    = (char *)e + offset + i * entsize;
        name  = __elf_get_word(e, st, ST_NAME);
        value = __elf_get_addr(e, st, ST_VALUE);
		size  = __elf_get_size(e, st, ST_SIZE);

        if(*(str + name) != '\0' && value != 0) {
            ret = hook(str + name, value, data);
            if(ret != KYLIN_ERROR_OK) 
                return ret;
        }
    }

    return KYLIN_ERROR_OK;
}

static kerr_t __elf_symtab_process(ked_t *ed, kbool_t dump, kylin_print print, symbol_hook hook, void *data)
{
    kerr_t   ret = KYLIN_ERROR_OK;
    uint64_t shnum;
    uint64_t shoff;
    uint64_t shentsize;
    uint64_t shstrndx;
    uint64_t type;
    uint64_t name;
    uint64_t offset;

    char *shstrtab;
    char *strtab = NULL;
    void *sh;
    void *v;

    shoff     = __elf_get_off(ed->ehdr, ed->ehdr, E_SHOFF);
	shentsize = __elf_get_quarter(ed->ehdr, ed->ehdr, E_SHENTSIZE);
    shnum     = __elf_get_quarter(ed->ehdr, ed->ehdr, E_SHNUM);
    shstrndx  = __elf_get_quarter(ed->ehdr, ed->ehdr, E_SHSTRNDX);

    sh        = (char *)ed->ehdr + shoff;
	offset    = __elf_get_off(ed->ehdr, (char *)sh + shstrndx * shentsize, SH_OFFSET);
	shstrtab  = (char *)ed->ehdr + offset;

    for(uint64_t i = 0; i < shnum; i++) {
		name   = __elf_get_word(ed->ehdr, (char *)sh + i * shentsize, SH_NAME);
		offset = __elf_get_off(ed->ehdr, (char *)sh + i * shentsize, SH_OFFSET);
		if(strcmp(shstrtab + name, ".strtab") == 0) 
			strtab = (char *)ed->ehdr + offset;
	}

    for(uint64_t i = 0; i < shnum; i++) {
		v    = (char *)sh + i * shentsize;
		type = __elf_get_word(ed->ehdr, v, SH_TYPE);
		switch(type) {
		case SHT_SYMTAB:
            if(dump) 
			    __elf_symtab_print(ed->ehdr, v, shstrtab, strtab, print);
            else {
                ret = __elf_symtab_get(ed->ehdr, v, shstrtab, strtab, hook, data);
                if(ret != KYLIN_ERROR_OK) 
                    return ret;
            }
			break;
        }
    }

    return KYLIN_ERROR_OK;
}

ked_t *kylin_elf_create(const char *filename)
{
    int         fd;
    struct stat sb;
    Elf32_Ehdr *hdr = NULL;
    ked_t      *ed  = NULL;

    fd = open(filename, O_RDONLY);
    if(fd == -1) 
        return NULL;

    if(fstat(fd, &sb) < 0) {
        close(fd);
        return NULL;
    }

    hdr = (Elf32_Ehdr *)mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if(hdr == MAP_FAILED) {
        close(fd);
        return NULL;
    }

    ed = kylin_malloc(sizeof(ked_t));
    if(!ed) {
        munmap(hdr, sb.st_size);
        close(fd);
        return NULL;
    }

    ed->fd   = fd;
    ed->size = sb.st_size;
    ed->ehdr = hdr;

    return ed;
}

void kylin_elf_destroy(ked_t *ed)
{
    if(ed && ed->ehdr != MAP_FAILED)
        munmap(ed->ehdr, ed->size);
    if(ed && ed->fd)
        close(ed->fd);
    if(ed) 
        kylin_free(ed);

    return;
}

kerr_t kylin_elf_symbol(ked_t *ed, symbol_hook hook, void *data)
{
    return __elf_symtab_process(ed, KYLIN_FALSE, NULL, hook, data);
}

kerr_t kylin_elf_dump(ked_t *ed, kylin_print print)
{
    return __elf_symtab_process(ed, KYLIN_TRUE, print, NULL, NULL);
}

