#ifndef __PERCPU_H_INCLUDED__
#define __PERCPU_H_INCLUDED__

struct frame;
struct local_storage {
	struct local_storage *self;
	uint64_t stack_top;
	uint64_t id;
	struct idt *idt;
	struct gdt *gdt;
	struct tss *tss;
	void (*apictimer_handler)(struct frame *);
} PACKED;

void local_storage_init(void);
struct local_storage *local_storage_get(void) PURE;

#endif