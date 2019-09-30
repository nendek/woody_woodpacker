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

mov [esp + 0xc], DWORD edi
call get_eip
mov esi, edi
add esi, 0x12345678 ; debut du.text MODULABLE
mov [esp + 0x4], DWORD 0x42 ; cle de dechiffrement MODULABLE
mov [esp], DWORD -1 ; taille du .text MODULABLE

; MPROTEC
mov edx, 0x7 ; WRITE | READ
mov ecx, DWORD [esp] ; taille du .text
add ecx, 0x1000 ; + une page
mov ebx, esi ; debut du .text
and ebx, 0xFFFFF000
mov eax, 0x7d
int 0x80

mov [esp + 0x8], DWORD 0x37B4D3DD ; TIMING
mov ebx, 8 ; NB_TIMING
mov edx, DWORD [esp + 0x4] ; cle chiffrement
loop2:
	mov ecx, DWORD [esp]
	mov edi, esi; debut du .text
loop1:
	mov eax, DWORD [edi]
	xor eax, edx
	stosd
	sub ecx, 4
	cmp ecx, 0
	jg loop1

	add edx, DWORD [esp + 0x8]
	dec ebx
	test ebx, ebx
	jne loop2

mov edx, 0x7 ; EXEC | READ
mov ecx, DWORD [esp] ; taille du .text
add ecx, 0x1000 ; + une page
mov ebx, esi; debut du .text
and ebx, 0xFFFFF000
mov eax, 0x7d
int 0x80

mov edi, [esp + 0xc]

add esp, 0x10
jmp end

get_eip:
	mov edi, [esp]
	ret
end:
