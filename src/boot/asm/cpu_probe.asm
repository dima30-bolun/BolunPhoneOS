section .text
global bolun_cpu_probe
bolun_cpu_probe:
    ; Educational HAL probe stub: bit flags are invented by Bolun Phone OS.
    mov rax, 0xB0100001
    ret

section .note.GNU-stack noalloc noexec nowrite progbits
