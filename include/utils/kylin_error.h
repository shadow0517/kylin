#ifndef _KYLIN_ERROR_H_
#define _KYLIN_ERROR_H_

#include <errno.h>

typedef uint32_t kerr_t;

#define KYLIN_ERROR_OK              0  /* No error*/

#define	KYLIN_ERROR_PERM		    1	/* Operation not permitted */
#define	KYLIN_ERROR_NOENT		    2	/* No such file or directory */
#define	KYLIN_ERROR_SRCH		    3	/* No such process */
#define	KYLIN_ERROR_INTR		    4	/* Interrupted system call */
#define	KYLIN_ERROR_IO		        5	/* I/O error */
#define	KYLIN_ERROR_NXIO		    6	/* No such device or address */
#define	KYLIN_ERROR_2BIG		    7	/* Argument list too long */
#define	KYLIN_ERROR_NOEXEC		    8	/* Exec format error */
#define	KYLIN_ERROR_BADF		    9	/* Bad file number */
#define	KYLIN_ERROR_CHILD		    10	/* No child processes */
#define	KYLIN_ERROR_AGAIN		    11	/* Try again */
#define	KYLIN_ERROR_NOMEM		    12	/* Out of memory */
#define	KYLIN_ERROR_ACCES		    13	/* Permission denied */
#define	KYLIN_ERROR_FAULT		    14	/* Bad address */
#define	KYLIN_ERROR_NOTBLK		    15	/* Block device required */
#define	KYLIN_ERROR_BUSY		    16	/* Device or resource busy */
#define	KYLIN_ERROR_EXIST		    17	/* File exists */
#define	KYLIN_ERROR_XDEV		    18	/* Cross-device link */
#define	KYLIN_ERROR_NODEV		    19	/* No such device */
#define	KYLIN_ERROR_NOTDIR		    20	/* Not a directory */
#define	KYLIN_ERROR_ISDIR		    21	/* Is a directory */
#define	KYLIN_ERROR_INVAL		    22	/* Invalid argument */
#define	KYLIN_ERROR_NFILE		    23	/* File table overflow */
#define	KYLIN_ERROR_MFILE	    	24	/* Too many open files */
#define	KYLIN_ERROR_NOTTY		    25	/* Not a typewriter */
#define	KYLIN_ERROR_TXTBSY		    26	/* Text file busy */
#define	KYLIN_ERROR_FBIG		    27	/* File too large */
#define	KYLIN_ERROR_NOSPC		    28	/* No space left on device */
#define	KYLIN_ERROR_SPIPE		    29	/* Illegal seek */
#define	KYLIN_ERROR_ROFS		    30	/* Read-only file system */
#define	KYLIN_ERROR_MLINK		    31	/* Too many links */
#define	KYLIN_ERROR_PIPE		    32	/* Broken pipe */
#define	KYLIN_ERROR_DOM		        33	/* Math argument out of domain of func */
#define	KYLIN_ERROR_RANGE		    34	/* Math result not representable */
#define	KYLIN_ERROR_DEADLK		    35	/* Resource deadlock would occur */
#define	KYLIN_ERROR_NAME2LONG	    36	/* File name too long */
#define	KYLIN_ERROR_NOLCK		    37	/* No record locks available */
#define	KYLIN_ERROR_NOSYS		    38	/* Invalid system call number */
#define	KYLIN_ERROR_NOTEMPTY	    39	/* Directory not empty */
#define	KYLIN_ERROR_LOOP		    40	/* Too many symbolic links encountered */
#define	KYLIN_ERROR_WOULDBLOCK	    EAGAIN	/* Operation would block */
#define	KYLIN_ERROR_NOMSG		    42	/* No message of desired type */
#define	KYLIN_ERROR_IDRM		    43	/* Identifier removed */
#define	KYLIN_ERROR_CHRNG		    44	/* Channel number out of range */
#define	KYLIN_ERROR_L2NSYNC	        45	/* Level 2 not synchronized */
#define	KYLIN_ERROR_L3HLT		    46	/* Level 3 halted */
#define	KYLIN_ERROR_L3RST		    47	/* Level 3 reset */
#define	KYLIN_ERROR_LNRNG		    48	/* Link number out of range */
#define	KYLIN_ERROR_UNATCH		    49	/* Protocol driver not attached */
#define	KYLIN_ERROR_NOCSI		    50	/* No CSI structure available */
#define	KYLIN_ERROR_L2HLT		    51	/* Level 2 halted */
#define	KYLIN_ERROR_BADE		    52	/* Invalid exchange */
#define	KYLIN_ERROR_BADR		    53	/* Invalid request descriptor */
#define	KYLIN_ERROR_XFULL		    54	/* Exchange full */
#define	KYLIN_ERROR_NOANO		    55	/* No anode */
#define	KYLIN_ERROR_BADRQC		    56	/* Invalid request code */
#define	KYLIN_ERROR_BADSLT		    57	/* Invalid slot */
#define	KYLIN_ERROR_DEADLOCK	    EDEADLK
#define	KYLIN_ERROR_BFONT		    59	/* Bad font file format */
#define	KYLIN_ERROR_NOSTR		    60	/* Device not a stream */
#define	KYLIN_ERROR_NODATA		    61	/* No data available */
#define	KYLIN_ERROR_TIME		    62	/* Timer expired */
#define	KYLIN_ERROR_NOSR		    63	/* Out of streams resources */
#define	KYLIN_ERROR_NONET		    64	/* Machine is not on the network */
#define	KYLIN_ERROR_NOPKG		    65	/* Package not installed */
#define	KYLIN_ERROR_REMOTE		    66	/* Object is remote */
#define	KYLIN_ERROR_NOLINK		    67	/* Link has been severed */
#define	KYLIN_ERROR_ADV		        68	/* Advertise error */
#define	KYLIN_ERROR_SRMNT		    69	/* Srmount error */
#define	KYLIN_ERROR_COMM		    70	/* Communication error on send */
#define	KYLIN_ERROR_PROTO		    71	/* Protocol error */
#define	KYLIN_ERROR_MULTIHOP	    72	/* Multihop attempted */
#define	KYLIN_ERROR_DOTDOT		    73	/* RFS specific error */
#define	KYLIN_ERROR_BADMSG		    74	/* Not a data message */
#define	KYLIN_ERROR_OVERFLOW	    75	/* Value too large for defined data type */
#define	KYLIN_ERROR_NOTUNIQ	        76	/* Name not unique on network */
#define	KYLIN_ERROR_BADFD		    77	/* File descriptor in bad state */
#define	KYLIN_ERROR_REMCHG		    78	/* Remote address changed */
#define	KYLIN_ERROR_LIBACC		    79	/* Can not access a needed shared library */
#define	KYLIN_ERROR_LIBBAD		    80	/* Accessing a corrupted shared library */
#define	KYLIN_ERROR_LIBSCN		    81	/* .lib section in a.out corrupted */
#define	KYLIN_ERROR_LIBMAX		    82	/* Attempting to link in too many shared libraries */
#define	KYLIN_ERROR_LIBEXEC	        83	/* Cannot exec a shared library directly */
#define	KYLIN_ERROR_ILSEQ		    84	/* Illegal byte sequence */
#define	KYLIN_ERROR_RESTART	        85	/* Interrupted system call should be restarted */
#define	KYLIN_ERROR_STRPIPE	        86	/* Streams pipe error */
#define	KYLIN_ERROR_USERS		    87	/* Too many users */
#define	KYLIN_ERROR_NOTSOCK	        88	/* Socket operation on non-socket */
#define	KYLIN_ERROR_DESTADDRREQ	    89	/* Destination address required */
#define	KYLIN_ERROR_MSGSIZE	        90	/* Message too long */
#define	KYLIN_ERROR_PROTOTYPE	    91	/* Protocol wrong type for socket */
#define	KYLIN_ERROR_NOPROTOOPT	    92	/* Protocol not available */
#define	KYLIN_ERROR_PROTONOSUPPORT	93	/* Protocol not supported */
#define	KYLIN_ERROR_SOCKTNOSUPPORT	94	/* Socket type not supported */
#define	KYLIN_ERROR_OPNOTSUPP	    95	/* Operation not supported on transport endpoint */
#define	KYLIN_ERROR_PFNOSUPPORT	    96	/* Protocol family not supported */
#define	KYLIN_ERROR_AFNOSUPPORT	    97	/* Address family not supported by protocol */
#define	KYLIN_ERROR_ADDRINUSE	    98	/* Address already in use */
#define	KYLIN_ERROR_ADDRNOTAVAIL	99	/* Cannot assign requested address */
#define	KYLIN_ERROR_NETDOWN	        100	/* Network is down */
#define	KYLIN_ERROR_NETUNREACH	    101	/* Network is unreachable */
#define	KYLIN_ERROR_NETRESET	    102	/* Network dropped connection because of reset */
#define	KYLIN_ERROR_CONNABORTED	    103	/* Software caused connection abort */
#define	KYLIN_ERROR_CONNRESET	    104	/* Connection reset by peer */
#define	KYLIN_ERROR_NOBUFS		    105	/* No buffer space available */
#define	KYLIN_ERROR_ISCONN		    106	/* Transport endpoint is already connected */
#define	KYLIN_ERROR_NOTCONN	        107	/* Transport endpoint is not connected */
#define	KYLIN_ERROR_SHUTDOWN	    108	/* Cannot send after transport endpoint shutdown */
#define	KYLIN_ERROR_TOOMANYREFS	    109	/* Too many references: cannot splice */
#define	KYLIN_ERROR_TIMEDOUT	    110	/* Connection timed out */
#define	KYLIN_ERROR_CONNREFUSED	    111	/* Connection refused */
#define	KYLIN_ERROR_HOSTDOWN	    112	/* Host is down */
#define	KYLIN_ERROR_HOSTUNREACH	    113	/* No route to host */
#define	KYLIN_ERROR_ALREADY	        114	/* Operation already in progress */
#define	KYLIN_ERROR_INPROGRESS	    115	/* Operation now in progress */
#define	KYLIN_ERROR_STALE		    116	/* Stale file handle */
#define	KYLIN_ERROR_UCLEAN		    117	/* Structure needs cleaning */
#define	KYLIN_ERROR_NOTNAM		    118	/* Not a XENIX named type file */
#define	KYLIN_ERROR_NAVAIL		    119	/* No XENIX semaphores available */
#define	KYLIN_ERROR_ISNAM		    120	/* Is a named type file */
#define	KYLIN_ERROR_REMOTEIO	    121	/* Remote I/O error */
#define	KYLIN_ERROR_DQUOT		    122	/* Quota exceeded */
#define	KYLIN_ERROR_NOMEDIUM	    123	/* No medium found */
#define	KYLIN_ERROR_MEDIUMTYPE	    124	/* Wrong medium type */
#define	KYLIN_ERROR_CANCELED	    125	/* Operation Canceled */
#define	KYLIN_ERROR_NOKEY		    126	/* Required key not available */
#define	KYLIN_ERROR_KEYEXPIRED	    127	/* Key has expired */
#define	KYLIN_ERROR_KEYREVOKED	    128	/* Key has been revoked */
#define	KYLIN_ERROR_KEYREJECTED	    129	/* Key was rejected by service */
#define	KYLIN_ERROR_OWNERDEAD	    130	/* Owner died */
#define	KYLIN_ERROR_NOTRECOVERABLE	131	/* State not recoverable */
#define KYLIN_ERROR_RFKILL		    132	/* Operation not possible due to RF-kill */
#define KYLIN_ERROR_HWPOISON	    133	/* Memory page has hardware error */

#endif /*_KYLIN_ERROR_H_*/
