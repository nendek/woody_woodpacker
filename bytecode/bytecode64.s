global _start
_start:

push rax
push rcx
push rdx
push r8
push r9
push r10
push r11
push rax

xor rax, rax
xor rbx, rbx
xor rdx, rdx

sub rsp, 0x10
mov [rsp], DWORD 0x2E2E2E2E
mov [rsp + 0x4], DWORD 0x444F4F57
mov [rsp + 0x8], DWORD 0x2E2E2E59
mov [rsp + 0xC], DWORD 0x0A2E
mov rdx, 14
mov rax, 1
mov rdi, 1
lea rsi, [rsp]
syscall

add rsp, 0x10

pop rax
pop r11
pop r10
pop r9
pop r8
pop rdx
pop rcx
pop rax
