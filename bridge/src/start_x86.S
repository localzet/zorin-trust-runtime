.global _start
.type _start,@function
_start:
    mov (%esp), %eax
    lea 4(%esp), %edx
    push %edx
    push %eax
    call main
    add $8, %esp
    mov %eax, %ebx
    mov $1, %eax
    int $0x80
