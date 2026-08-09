.global _start
.type _start,@function
_start:
    mov (%rsp), %rdi
    lea 8(%rsp), %rsi
    call main
    mov %eax, %edi
    mov $60, %eax
    syscall
