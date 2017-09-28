#ifndef _KYLIN_ARCH_CPUFLAG_X86_64_H_
#define _KYLIN_ARCH_CPUFLAG_X86_64_H_

#include <kylin/include/arch/common/kylin_cpuflag.h>

enum kylin_cpu_flag {
	/* (EAX 01h) ECX features*/
	KYLIN_CPUFLAG_SSE3 = 0,               /**< SSE3 */
	KYLIN_CPUFLAG_PCLMULQDQ,              /**< PCLMULQDQ */
	KYLIN_CPUFLAG_DTES64,                 /**< DTES64 */
	KYLIN_CPUFLAG_MONITOR,                /**< MONITOR */
	KYLIN_CPUFLAG_DS_CPL,                 /**< DS_CPL */
	KYLIN_CPUFLAG_VMX,                    /**< VMX */
	KYLIN_CPUFLAG_SMX,                    /**< SMX */
	KYLIN_CPUFLAG_EIST,                   /**< EIST */
	KYLIN_CPUFLAG_TM2,                    /**< TM2 */
	KYLIN_CPUFLAG_SSSE3,                  /**< SSSE3 */
	KYLIN_CPUFLAG_CNXT_ID,                /**< CNXT_ID */
	KYLIN_CPUFLAG_FMA,                    /**< FMA */
	KYLIN_CPUFLAG_CMPXCHG16B,             /**< CMPXCHG16B */
	KYLIN_CPUFLAG_XTPR,                   /**< XTPR */
	KYLIN_CPUFLAG_PDCM,                   /**< PDCM */
	KYLIN_CPUFLAG_PCID,                   /**< PCID */
	KYLIN_CPUFLAG_DCA,                    /**< DCA */
	KYLIN_CPUFLAG_SSE4_1,                 /**< SSE4_1 */
	KYLIN_CPUFLAG_SSE4_2,                 /**< SSE4_2 */
	KYLIN_CPUFLAG_X2APIC,                 /**< X2APIC */
	KYLIN_CPUFLAG_MOVBE,                  /**< MOVBE */
	KYLIN_CPUFLAG_POPCNT,                 /**< POPCNT */
	KYLIN_CPUFLAG_TSC_DEADLINE,           /**< TSC_DEADLINE */
	KYLIN_CPUFLAG_AES,                    /**< AES */
	KYLIN_CPUFLAG_XSAVE,                  /**< XSAVE */
	KYLIN_CPUFLAG_OSXSAVE,                /**< OSXSAVE */
	KYLIN_CPUFLAG_AVX,                    /**< AVX */
	KYLIN_CPUFLAG_F16C,                   /**< F16C */
	KYLIN_CPUFLAG_RDRAND,                 /**< RDRAND */

	/* (EAX 01h) EDX features */
	KYLIN_CPUFLAG_FPU,                    /**< FPU */
	KYLIN_CPUFLAG_VME,                    /**< VME */
	KYLIN_CPUFLAG_DE,                     /**< DE */
	KYLIN_CPUFLAG_PSE,                    /**< PSE */
	KYLIN_CPUFLAG_TSC,                    /**< TSC */
	KYLIN_CPUFLAG_MSR,                    /**< MSR */
	KYLIN_CPUFLAG_PAE,                    /**< PAE */
	KYLIN_CPUFLAG_MCE,                    /**< MCE */
	KYLIN_CPUFLAG_CX8,                    /**< CX8 */
	KYLIN_CPUFLAG_APIC,                   /**< APIC */
	KYLIN_CPUFLAG_SEP,                    /**< SEP */
	KYLIN_CPUFLAG_MTRR,                   /**< MTRR */
	KYLIN_CPUFLAG_PGE,                    /**< PGE */
	KYLIN_CPUFLAG_MCA,                    /**< MCA */
	KYLIN_CPUFLAG_CMOV,                   /**< CMOV */
	KYLIN_CPUFLAG_PAT,                    /**< PAT */
	KYLIN_CPUFLAG_PSE36,                  /**< PSE36 */
	KYLIN_CPUFLAG_PSN,                    /**< PSN */
	KYLIN_CPUFLAG_CLFSH,                  /**< CLFSH */
	KYLIN_CPUFLAG_DS,                     /**< DS */
	KYLIN_CPUFLAG_ACPI,                   /**< ACPI */
	KYLIN_CPUFLAG_MMX,                    /**< MMX */
	KYLIN_CPUFLAG_FXSR,                   /**< FXSR */
	KYLIN_CPUFLAG_SSE,                    /**< SSE */
	KYLIN_CPUFLAG_SSE2,                   /**< SSE2 */
	KYLIN_CPUFLAG_SS,                     /**< SS */
	KYLIN_CPUFLAG_HTT,                    /**< HTT */
	KYLIN_CPUFLAG_TM,                     /**< TM */
	KYLIN_CPUFLAG_PBE,                    /**< PBE */

	/* (EAX 06h) EAX features */
	KYLIN_CPUFLAG_DIGTEMP,                /**< DIGTEMP */
	KYLIN_CPUFLAG_TRBOBST,                /**< TRBOBST */
	KYLIN_CPUFLAG_ARAT,                   /**< ARAT */
	KYLIN_CPUFLAG_PLN,                    /**< PLN */
	KYLIN_CPUFLAG_ECMD,                   /**< ECMD */
	KYLIN_CPUFLAG_PTM,                    /**< PTM */

	/* (EAX 06h) ECX features */
	KYLIN_CPUFLAG_MPERF_APERF_MSR,        /**< MPERF_APERF_MSR */
	KYLIN_CPUFLAG_ACNT2,                  /**< ACNT2 */
	KYLIN_CPUFLAG_ENERGY_EFF,             /**< ENERGY_EFF */

	/* (EAX 07h, ECX 0h) EBX features */
	KYLIN_CPUFLAG_FSGSBASE,               /**< FSGSBASE */
	KYLIN_CPUFLAG_BMI1,                   /**< BMI1 */
	KYLIN_CPUFLAG_HLE,                    /**< Hardware Lock elision */
	KYLIN_CPUFLAG_AVX2,                   /**< AVX2 */
	KYLIN_CPUFLAG_SMEP,                   /**< SMEP */
	KYLIN_CPUFLAG_BMI2,                   /**< BMI2 */
	KYLIN_CPUFLAG_ERMS,                   /**< ERMS */
	KYLIN_CPUFLAG_INVPCID,                /**< INVPCID */
	KYLIN_CPUFLAG_RTM,                    /**< Transactional memory */
	KYLIN_CPUFLAG_AVX512F,                /**< AVX512F */

	/* (EAX 80000001h) ECX features */
	KYLIN_CPUFLAG_LAHF_SAHF,              /**< LAHF_SAHF */
	KYLIN_CPUFLAG_LZCNT,                  /**< LZCNT */

	/* (EAX 80000001h) EDX features */
	KYLIN_CPUFLAG_SYSCALL,                /**< SYSCALL */
	KYLIN_CPUFLAG_XD,                     /**< XD */
	KYLIN_CPUFLAG_1GB_PG,                 /**< 1GB_PG */
	KYLIN_CPUFLAG_RDTSCP,                 /**< RDTSCP */
	KYLIN_CPUFLAG_EM64T,                  /**< EM64T */

	/* (EAX 80000007h) EDX features */
	KYLIN_CPUFLAG_INVTSC,                 /**< INVTSC */

	/* The last item */
	KYLIN_CPUFLAG_NUMFLAGS,               /**< This should always be the last! */
};

#endif /*_KYLIN_ARCH_CPUFLAG_X86_64_H_*/
