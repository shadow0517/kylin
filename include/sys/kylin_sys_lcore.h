#ifndef _KYLIN_SYS_LCORE_H_
#define _KYLIN_SYS_LCORE_H_

typedef enum {
    KYLIN_LCORE_ROLE_SHARE;
    KYLIN_LCORE_ROLE_EXCL;
    KYLIN_LCORE_ROLE_MASTER;
    KYLIN_LCORE_ROLE_SLAVE;
    KYLIN_LCORE_ROLE_OFF;
}ksys_lcore_role_t;

/*
 * CPU Logical Core info Per <NUMA or UMA> Node
 *
 * It's Running Time Environment Variables, Not Configure
 * */
typedef struct {
    uint32_t                id;
    kcpuset_t               set;
    kylin_cpu_flag_t        flag[KYLIN_CPUFLAG_NUM_MAX];
    ksys_lcore_role_t       role;
    pthread_t               thread[32];
    pid_t                   pid[32]
} ksys_lcore_t;

#endif /*_KYLIN_SYS_LCORE_H_*/
