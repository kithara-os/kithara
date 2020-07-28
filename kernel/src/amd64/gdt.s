bits 64

global gdt_load

section .text

gdt_load:
	lgdt [rdi]
	ret