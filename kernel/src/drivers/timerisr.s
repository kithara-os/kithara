bits 64

global apic_timer_isr
extern apic_timer_handler
extern local_storage_get
extern local_storage_set

section .text

apic_timer_isr:
	push qword 0
	push rax
	push rbx
	push rcx
	push rdx
	push rdi
	push rsi
	push rbp
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	mov rax, es
	push rax
	mov rax, ds
	push rax
	mov rax, gs
	push rax
	mov rax, fs
	push rax
	mov rsi, rsp
	call apic_timer_handler

	call local_storage_get
	mov rdi, rax
	pop rax
	mov fs, rax
	call local_storage_set

	pop rax
	mov gs, rax
	pop rax
	mov ds, rax
	pop rax
	mov es, rax
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rbp
	pop rsi
	pop rdi
	pop rdx
	pop rcx
	pop rbx
	pop rax
	add rsp, 8
	iretq