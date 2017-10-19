#include <kylin/include/kylin.h>
#include <kylin/include/utils/kylin_error.h>
#include <kylin/include/ipc/kylin_shm.h>
#include <kylin/include/kylin_ddb.h>

#include <kylin/lib/core/ddb/kylin_ddb.h>
#include <kylin/lib/core/ddb/kylin_ddb_log.h>
#include <kylin/lib/core/ddb/kylin_ddb_service.h>

#define KYLIN_DDB_SHM_NAME_PREFIX "ddb"

static kshm_t *ddb_shm = NULL;

kddb_t *kylin_ddb_get(void)
{
    if(!ddb_shm)
        return NULL;

    return (kddb_t *)kylin_shm_addr(ddb_shm);
}

static void __signal_process_normal(int sig)
{
    kddb_t         *ddb_addr = NULL;
    kddb_service_t *service  = NULL;

#if defined(__linux__)
    char *sig_name = strsignal(sig);
#elif defined(__FreeBSD__)
    char *sig_name = sys_signame[sig];
#endif /*__SYS_LINUX__*/

    kylin_ddb_printf("time: %s, process on core %d: recv sig: id: %d, name: %s\n", kylin_timer_get_ctime, KYLIN_CPU_ID, sig, sig_name);

    service_foreach(service) 
        service->exec();

    if(!ddb_shm ||
            !(ddb_addr = (kddb_t *)kylin_shm_addr(ddb_shm)))
        exit(1);

    ddb_addr->proc[KYLIN_DDB_PROC_CURRENT].status = KDDB_STATUS_CORE; 
    ddb_addr->proc[KYLIN_DDB_PROC_CURRENT].stop  = time(NULL); 

    exit(1);
}

static int __signal_init(void)
{
    signal(SIGILL,  __signal_process_normal);
    signal(SIGTRAP, __signal_process_normal);
    signal(SIGABRT, __signal_process_normal);
    signal(SIGIOT,  __signal_process_normal);
    signal(SIGSYS,  __signal_process_normal);
    signal(SIGXCPU, __signal_process_normal);
    signal(SIGXFSZ, __signal_process_normal);
    signal(SIGFPE,  __signal_process_normal);
    signal(SIGKILL, __signal_process_normal);
    signal(SIGSEGV, __signal_process_normal);
    signal(SIGFPE,  __signal_process_normal);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGCLD, SIG_IGN); /* 僵尸进程 */

#if defined(__FreeBSD__)
    signal(SIGEMT,  __signal_process_normal);
#endif

    return KYLIN_ERROR_OK;
}

static kerr_t __shm_init(void)
{
    kerr_t  ret          = KYLIN_ERROR_OK;
    kddb_t *ddb_addr     = NULL;
    kbool_t first_create = KYLIN_FALSE;
    char    shm_name[KYLIN_NAME_LENGTH] = {0};

    snprintf(shm_name, KYLIN_NAME_LENGTH, "%s#%s", KYLIN_DDB_SHM_NAME_PREFIX, KYLIN_PNAME);
    ret = kylin_shm_create(shm_name, sizeof(kddb_t)); 
    if(ret == KYLIN_ERROR_OK)
        first_create = KYLIN_TRUE;

    if(ret != KYLIN_ERROR_OK &&
            ret != KYLIN_ERROR_EXIST)
        return ret;

    ddb_shm = kylin_shm_open(shm_name);
    if(!ddb_shm)
        return KYLIN_ERROR_NOMEM;

    ddb_addr = (kddb_t *)kylin_shm_addr(ddb_shm);
    if(!ddb_addr)
        return KYLIN_ERROR_NOENT;

    if(first_create) {
        memset(ddb_addr, 0, sizeof(kddb_t));
        memcpy(ddb_addr->name, KYLIN_PNAME, 
                KYLIN_MIN(strlen(KYLIN_PNAME), (KYLIN_NAME_LENGTH - 1)));
        ddb_addr->runs = 1;
        ddb_addr->proc[KYLIN_DDB_PROC_CURRENT].status = KDDB_STATUS_RUNNING; 
        ddb_addr->proc[KYLIN_DDB_PROC_CURRENT].start  = time(NULL); 
    }
    else {
        ddb_addr->runs += 1;
        memcpy(&ddb_addr->proc[KYLIN_DDB_PROC_BEFORE], 
                &ddb_addr->proc[KYLIN_DDB_PROC_CURRENT], 
                sizeof(ddb_addr->proc[KYLIN_DDB_PROC_CURRENT]));

        memset(&ddb_addr->proc[KYLIN_DDB_PROC_CURRENT], 0, 
                sizeof(ddb_addr->proc[KYLIN_DDB_PROC_CURRENT]));

        ddb_addr->proc[KYLIN_DDB_PROC_CURRENT].status = KDDB_STATUS_RUNNING; 
        ddb_addr->proc[KYLIN_DDB_PROC_CURRENT].start  = time(NULL); 
    }

    return KYLIN_ERROR_OK;
}

static void __shm_fini(void)
{
    kddb_t *ddb_addr = NULL;

    if(ddb_shm) {
        ddb_addr = (kddb_t *)kylin_shm_addr(ddb_shm);
        if(ddb_addr) {
            ddb_addr->proc[KYLIN_DDB_PROC_CURRENT].status = KDDB_STATUS_STOPPED; 
            ddb_addr->proc[KYLIN_DDB_PROC_CURRENT].stop   = time(NULL); 
        }
        kylin_shm_close(shm);
    }

    return;
}

kerr_t kylin_ddb_init(void)
{
    kerr_t ret = KYLIN_ERROR_OK;

    ret = service_init();
    if(ret != KYLIN_ERROR_OK)
        return ret;

    ret = log_init();
    if(ret != KYLIN_ERROR_OK)
        return ret;

    return __shm_init();
}

void kylin_ddb_fini(void)
{
    __shm_fini();
    log_fini();
    service_fini();
}

