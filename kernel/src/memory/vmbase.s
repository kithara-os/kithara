bits 64

global vmbase_invalidate_at
global vmbase_invalidate_all
global vmbase_load_page_table

section .text

vmbase_invalidate_at:
	invlpg [rdi]
	ret

vmbase_invalidate_all:
	push rax
	mov rax, cr3
	mov cr3, rax
	pop rax
	ret

vmbase_load_page_table:
	mov cr3, rdi
	ret