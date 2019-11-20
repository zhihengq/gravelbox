# Compile with -no-pie to avoid overflow
# `filename` symbol address must be representable with 32-bit %ebx

.data

filename:
    .string "gravelbox_config.json"

attempt_str:
    .string "Attempting to open %s with %s\n"

return_str:
    .string "Returned: %d\n"

int80:
    .string "int 80"

syscall:
    .string "syscall"

.text

.global main

main:
    call open_file_int
    call open_file_syscall
    ret

open_file_int:
    lea attempt_str(%rip), %rdi
    lea filename(%rip), %rsi
    lea int80(%rip), %rdx
    call printf

    mov $5, %eax                # `open`
    lea filename(%rip), %ebx    # arg1: filename
    mov $0, %ecx                # arg2: flags
    mov $0, %edx                # arg3: mode
    int $0x80

    lea return_str(%rip), %rdi
    mov %rax, %rsi
    call printf
    ret

open_file_syscall:
    lea attempt_str(%rip), %rdi
    lea filename(%rip), %rsi
    lea syscall(%rip), %rdx
    call printf

    mov $2, %rax                # `open`
    lea filename(%rip), %rdi    # arg1: filename
    mov $0, %rsi                # arg2: flags
    mov $0, %rdx                # arg3: mode
    syscall

    lea return_str(%rip), %rdi
    mov %rax, %rsi
    call printf
    ret
