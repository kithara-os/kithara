bits 64

section .data
global apcode_start
global apcode_end
apcode_start:
incbin "aptrampoline.bin"
apcode_end: