global _start
_start:
push rbp
mov rbp, rsp
and rsp, 0xFFFFFFFFFFFFFFF0
push rax
push rcx
push rdx
push r8
push r9
push r10
push r11
push rax
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
pop rax
pop r11
pop r10
pop r9
pop r8
pop rdx
pop rcx
pop rax
mov rsp, rbp
pop rbp

str:
call write
db '....WOODY....', 0x0A
