global _start
_start:
push ebp
mov ebp, esp
and esp, 0xFFFFFFF0
pusha
jmp short str

write:
mov edx, 14
pop ecx
mov ebx, 1
mov eax, 4
int 0x80
mov eax, 1
int 0x80
popa
mov esp, ebp
pop ebp

str:
call write
db '....WOODY....', 0x0A
