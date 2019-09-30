default rel
global _start
_start:
	pusha
	call	get_eip

	mov edx, 0x7; EXEC | READ
	mov ecx, 0x105D; size shell code + 1 page, modulable (ou non ?)
p1:
	lea ebx, [edi + 0x11111111]
	and ebx, 0xFFFFF000
	mov eax, 0x7d
	int 0x80

p2:
	jmp 0x2e7c - (p2 - _start)

	mov edx, 0x7; WRITE | READ
	mov ecx, 0x105D; size shell code + 1 page, modulable (ou non ?)
p3:
	lea ebx, [edi + 0x11111111]
	and ebx, 0xFFFFF000
	mov eax, 0x7d
	int 0x80
	popa

jmp end

get_eip:
	mov edi, [esp]
	ret
end:
