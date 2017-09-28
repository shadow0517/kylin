#ifndef _KYLIN_ARCH_IO_H_
#define _KYLIN_ARCH_IO_H_

static __kylin_inline uint8_t kylin_read8_relaxed(const volatile void *addr)
{
	return *(const volatile uint8_t *)addr;
}

static __kylin_inline uint16_t kylin_read16_relaxed(const volatile void *addr)
{
	return *(const volatile uint16_t *)addr;
}

static __kylin_inline uint32_t kylin_read32_relaxed(const volatile void *addr)
{
	return *(const volatile uint32_t *)addr;
}

static __kylin_inline uint64_t kylin_read64_relaxed(const volatile void *addr)
{
	return *(const volatile uint64_t *)addr;
}

static __kylin_inline void kylin_write8_relaxed(uint8_t value, volatile void *addr)
{
	*(volatile uint8_t *)addr = value;
}

static __kylin_inline void kylin_write16_relaxed(uint16_t value, volatile void *addr)
{
	*(volatile uint16_t *)addr = value;
}

static __kylin_inline void kylin_write32_relaxed(uint32_t value, volatile void *addr)
{
	*(volatile uint32_t *)addr = value;
}

static __kylin_inline void kylin_write64_relaxed(uint64_t value, volatile void *addr)
{
	*(volatile uint64_t *)addr = value;
}

static __kylin_inline uint8_t kylin_read8(const volatile void *addr)
{
	uint8_t val;
	val = kylin_read8_relaxed(addr);
	kylin_io_rmb();
	return val;
}

static __kylin_inline uint16_t kylin_read16(const volatile void *addr)
{
	uint16_t val;
	val = kylin_read16_relaxed(addr);
	kylin_io_rmb();
	return val;
}

static __kylin_inline uint32_t kylin_read32(const volatile void *addr)
{
	uint32_t val;
	val = kylin_read32_relaxed(addr);
	kylin_io_rmb();
	return val;
}

static __kylin_inline uint64_t kylin_read64(const volatile void *addr)
{
	uint64_t val;
	val = kylin_read64_relaxed(addr);
	kylin_io_rmb();
	return val;
}

static __kylin_inline void kylin_write8(uint8_t value, volatile void *addr)
{
	kylin_io_wmb();
	kylin_write8_relaxed(value, addr);
}

static __kylin_inline void kylin_write16(uint16_t value, volatile void *addr)
{
	kylin_io_wmb();
	kylin_write16_relaxed(value, addr);
}

static __kylin_inline void kylin_write32(uint32_t value, volatile void *addr)
{
	kylin_io_wmb();
	kylin_write32_relaxed(value, addr);
}

static __kylin_inline void kylin_write64(uint64_t value, volatile void *addr)
{
	kylin_io_wmb();
	kylin_write64_relaxed(value, addr);
}

#endif /*_KYLIN_ARCH_IO_H_*/
