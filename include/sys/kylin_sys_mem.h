#ifndef _KYLIN_SYS_MEM_H_
#define _KYLIN_SYS_MEM_H_

#include <kylin/include/kylin.h>

#define KYLIN_MAX_MEMSEG_PER_NODE 256

typedef enum {
    KYLIN_HUGEPAGE_NONE = 0;
    KYLIN_HUGEPAGE_2M;
    KYLIN_HUGEPAGE_4M;
    KYLIN_HUGEPAGE_1G;
    KYLIN_HUGEPAGE_MAX;
}ksys_hp_t;

/*
 * Memory info Per <NUMA or UMA> Node
 *
 * It's Running Time Environment Variables, Not Configure
 * */
typedef struct {
    uint64_t     phys_addr;              /*Start physical address.*/
    KYLIN_STD_C11 union {
        void    *addr;                   /*Start virtual address.*/
        uint64_t addr_64;                /*Makes sure addr is always 64 bits*/
    };
    size_t       len;                    /*Length of the segment.*/
    ksys_hp_t    hugepage_sz;            /*The pagesize of underlying memory*/
    uint32_t     channel_cnt;            /*Count of channels.*/
    uint32_t     rank_cnt;               /*Count of ranks.*/
} ksys_memseg_t;

typedef struct {
	volatile uint32_t magic;   /*Magic number - Sanity check.*/

	uint32_t          channel_cnt;    /*Count of channels (0 if unknown).*/
	uint32_t          rank_cnt;       /*Count of ranks (0 if unknown).*/

    uint32_t          seg_cnt;
	ksys_memseg_t     memseg[KYLIN_MAX_MEMSEG_PER_NODE];
} ksys_mem_t;

#endif /*_KYLIN_SYS_MEM_H_*/
