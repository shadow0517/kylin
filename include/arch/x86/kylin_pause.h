#ifndef _KYLIN_ARCH_PAUSE_X86_64_H_
#define _KYLIN_ARCH_PAUSE_X86_64_H_

#include <emmintrin.h>
#include <kylin/include/arch/common/kylin_pause.h>

static inline void kylin_pause(void)
{
	_mm_pause();
}

#endif /*_KYLIN_ARCH_PAUSE_X86_64_H_*/
