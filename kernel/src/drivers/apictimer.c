#include <amd64/idt.h>
#include <drivers/apictimer.h>
#include <drivers/lapic.h>
#include <drivers/pit.h>
#include <smp/localstorage.h>
#include <lock.h>

void apic_timer_handler(struct frame *frame) {
	struct local_storage *storage = local_storage_get();
	if (storage->apictimer_handler != NULL) {
		storage->apictimer_handler(frame);
	}
	lapic_write(LAPIC_REG_EOI, 0);
}

void apic_timer_init(uint64_t ms) {
	static struct ticketlock pit_lock = TICKETLOCK_INIT_STATE;

	extern void apic_timer_isr(void);
	idt_set_gate(32, (uint64_t)apic_timer_isr, 0, false);

	local_storage_get()->apictimer_handler = NULL;

	ticketlock_lock(&pit_lock);

	lapic_write(LAPIC_REG_TMRDIV, 0x3);
	lapic_write(LAPIC_REG_TMRINITCNT, 0xFFFFFFFF);
	for (uint64_t i = 0; i < ms; ++i) {
		pit_wait(1);
	}
	lapic_write(LAPIC_REG_LVT_TMR, 1ULL << 16);
	uint32_t ticks = 0xFFFFFFFF - lapic_read(LAPIC_REG_TMRCURRENT);
	lapic_write(LAPIC_REG_LVT_TMR, 32 | LAPIC_TMR_PERIODIC);
	lapic_write(LAPIC_REG_TMRDIV, 0x3);
	lapic_write(LAPIC_REG_TMRINITCNT, ticks);

	ticketlock_unlock(&pit_lock);
}