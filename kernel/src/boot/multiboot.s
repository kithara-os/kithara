section .header
header:

multibootMagic equ 0xE85250D6
architecture equ 0
headerLength equ .end - .start
checksum equ -(multibootMagic + architecture + headerLength)

terminatingTagType equ 0
terminatingTagFlags equ 0
terminatingTagSize equ 8

.start:
    align 8
    dd multibootMagic
    dd architecture
    dd headerLength
    dd checksum

    align 8
    dw terminatingTagType
    dw terminatingTagFlags
    dw terminatingTagSize
.end: