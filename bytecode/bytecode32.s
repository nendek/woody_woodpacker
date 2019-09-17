global _start
_start:
jmp short str

write:
pop ecx
mov eax, 4
mov ebx, 1
mov edx, 14
int 0x80
mov eax, 1
int 0x80

str:
call write
db '....WOODY....', 0x0A
