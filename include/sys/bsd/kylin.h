#ifndef _KYLIN_SYS_BSD_H_
#define _KYLIN_SYS_BSD_H_

#include <kenv.h>
#include <ctype.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sched.h>
#include <md5.h>
#include <libgen.h>
#include <sys/elf32.h>
#include <sys/elf64.h>
#include <sys/endian.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <sys/endian.h>
#include <sys/time.h>
#include <sys/sockio.h>
#include <sys/ctype.h>  /* string functions */
#include <sys/random.h>
#include <sys/syslog.h>
#include <sys/sysctl.h>
#include <sys/cdefs.h>
#include <sys/elf32.h>
#include <sys/elf64.h>
#include <sys/clock.h>
#include <vm/uma.h>
#include <vm/vm.h>
#include <vm/vm_page.h>
#include <vm/vm_object.h>
#include <vm/vm_pageout.h>
#include <sys/module.h>
#include <sys/priv.h>
#include <sys/proc.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/rwlock.h>
#include <sys/ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/selinfo.h>
#include <sys/poll.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <sys/cpuset.h>
#include <sys/socket.h>
#include <sys/cdefs.h>
#include <sys/un.h>
#include <sys/user.h>
#include <net/if.h>
#include <net/bpf.h>
#include <net/route.h>
#include <machine/bus.h>
#include <net/if_var.h>
#include <net/if_types.h>
#include <net/if_dl.h>
#include <net/ethernet.h>
#include <net/if_vlan_var.h>
#include <netinet/in.h>
#include <netinet/in_var.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <limits.h>
#include <machine/atomic.h>
#include <opt.h>
#include <libutil.h>

#define KYLIN_PAGE_SIZE PAGE_SIZE

#define KYLIN_PID    getpid()
#define KYLIN_PNAME  kinfo_getproc(getpid())->ki_comm 

extern __thread uint8_t kylin_cpu_id;
#define KYLIN_CPU_ID kylin_cpu_id

#include <pthread_np.h>
typedef cpuset_t kcpuset_t;

#endif /*_KYLIN_SYS_BSD_H_*/
