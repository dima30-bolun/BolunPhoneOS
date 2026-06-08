section .text
global bolun_interrupt_stub
bolun_interrupt_stub:
    iretq

section .note.GNU-stack noalloc noexec nowrite progbits
