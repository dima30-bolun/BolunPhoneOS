section .rodata
banner: db 'Bolun Phone boot stub by dima30', 0

section .text
global bolun_boot_banner
bolun_boot_banner:
    lea rax, [rel banner]
    ret

section .note.GNU-stack noalloc noexec nowrite progbits
