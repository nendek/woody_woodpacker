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

sub rsp, 0x10
mov [rsp], DWORD 0x2E2E2E2E
mov [rsp + 0x4], DWORD 0x574f4f44
mov [rsp + 0x8], DWORD 0x592e2e2e
mov [rsp + 0x10], DWORD 0x2e0a
;db '....WOODY....', 0x0A
mov rax, 1
mov rdi, 1
mov rsi, rsp
mov rdx, 14
syscall
;mov rax, 60
;mov rdi, 0
;syscall


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

