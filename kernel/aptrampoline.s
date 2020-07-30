bits 16
org 0x8000

KERNEL_MAPPING_BASE: equ 0xffff800000000000

start:
	jmp 0x0000:.start16
.start16:
	cli

	xor ax, ax
	mov ss, ax
	mov ds, ax
	mov fs, ax
	mov gs, ax

	mov eax, 10100000b
    	mov cr4, eax
 
    	mov edx, dword [0x7ff8]
    	mov cr3, edx
 
    	mov ecx, 0xC0000080
 	rdmsr
	or eax, 1 << 8 ; enable long mode
	or eax, 1 << 11 ; enable no execute
	or eax, 1 << 0 ; enable syscall/sysret instructions
    	wrmsr
 
    	mov ebx, cr0
    	or ebx, 0x80000001
    	mov cr0, ebx

	lgdt [gdt64.pointer]

	jmp gdt64.kernel_code:start64

bits 64
start64:
	mov ax, gdt64.kernel_data
	mov es, ax
	mov gs, ax
	mov ss, ax
	mov fs, ax
	mov ds, ax

	mov rsp, qword [0x7fe8]
	mov rax, qword [0x7ff0]
	call rax
	
	int3

gdt64:
	dq 0 ; zero entry
.kernel_code: equ $ - gdt64
	dq (1<<44) | (1<<47) | (1<<41) | (1<<43) | (1<<53) ; kernel code segment
.kernel_data: equ $ - gdt64
	dq (1<<44) | (1<<47) | (1<<41) ; kernel data
.pointer:
	dw gdt64.end - gdt64 - 1
	dq gdt64
.end: