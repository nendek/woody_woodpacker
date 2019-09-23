global _start
_start:

xor eax, eax
xor ebx, ebx
xor edx, edx

sub esp, 0x10
mov [esp], DWORD 0x2E2E2E2E
mov [esp + 0x4], DWORD 0x444F4F57
mov [esp + 0x8], DWORD 0x2E2E2E59
mov [esp + 0xC], DWORD 0x0A2E
mov edx, 14
lea ecx, [esp]
mov ebx, 1
mov eax, 4
int 0x80

add esp, 0x10
