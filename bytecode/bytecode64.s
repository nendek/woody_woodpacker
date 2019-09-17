global _start
_start:
jmp short str

write:
pop rsi
mov rax, 1
mov rdi, 1
mov rdx, 14
syscall
mov rax, 60
mov rdi, 0
syscall

str:
call write
db '....WOODY....', 0x0A
