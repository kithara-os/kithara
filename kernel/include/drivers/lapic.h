#ifndef __LAPIC_H_INCLUDED__
#define __LAPIC_H_INCLUDED__

#include <utils.h>

#define LAPIC_REG_ID 0x20
#define LAPIC_REG_VER 0x30
#define LAPIC_REG_TASK_PRIORITY 0x80
#define LAPIC_REG_EOI 0xB0
#define LAPIC_REG_LDR 0xD0
#define LAPIC_REG_DFR 0xE0
#define LAPIC_REG_SPURIOUS 0xF0
#define LAPIC_REG_ESR 0x280
#define LAPIC_REG_ICRL 0x300
#define LAPIC_REG_ICRH 0x310
#define LAPIC_REG_LVT_TMR 0x320
#define LAPIC_REG_LVT_PERF 0x340
#define LAPIC_REG_LVT_LINT0 0x350
#define LAPIC_REG_LVT_LINT1 0x360
#define LAPIC_REG_LVT_ERR 0x370
#define LAPIC_REG_TMRINITCNT 0x380
#define LAPIC_REG_TMRCURRENT 0x390
#define LAPIC_REG_TMRDIV 0x3E0
#define LAPIC_REG_LAST 0x38F
#define LAPIC_REG_DISABLE 0x10000
#define LAPIC_REG_SW_ENABLE 0x100
#define LAPIC_REG_CPUFOCUS 0x200
#define LAPIC_NMI (4ULL << 8)
#define LAPIC_TMR_PERIODIC 0x20000
#define LAPIC_TMR_BASEDIV (1ULL << 20)

void lapic_init(void);
void lapic_core_init(void);
void lapic_write(uint64_t reg, uint32_t val);
uint32_t lapic_read(uint64_t reg);
void lapic_send_ipi_sequence(uint32_t id);
void lapic_send_sipi_sequence(uint32_t id, uint32_t entrypoint);
uint32_t lapic_get_apic_id();

#endif