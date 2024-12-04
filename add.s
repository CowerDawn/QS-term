.global add

add:
    push %rbp
    mov %rsp, %rbp
    mov %rdi, %rax
    add %rsi, %rax
    pop %rbp
    ret
