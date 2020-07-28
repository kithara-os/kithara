bits 64

global idt_load
global idt_spurious_irq_handler

section .text

idt_load:
	lidt [rdi]
	ret

idt_spurious_irq_handler:
	iretq