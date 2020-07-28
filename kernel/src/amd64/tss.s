bits 64

global tss_load

section .text

tss_load:
	push rax
	mov ax, 0x28
	ltr ax
	pop rax
	ret