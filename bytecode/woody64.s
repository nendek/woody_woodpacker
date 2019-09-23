default rel
global _start
_start:

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

mov [rsp + 0x8], DWORD 0x42 ; cle de dechiffrement MODULABLE
mov [rsp], DWORD -1 ; taille du .text MODULABLE

; MPROTEC
mov rdx, 0x3 ; WRITE | READ
mov rsi, [rsp] ; taille du .text
add rsi, 0x1000 ; + une page
lea rdi, [$ - 0x1] ; debut du .text MODULABLE
and rdi, 0xFFFFFFFFFFFFF000
mov rax, 0xa
syscall

mov ecx, DWORD [rsp] ; taille du .text
mov edx, DWORD [rsp + 0x8] ; cle chiffrement
lea rdi, [$ - 0x2E7C] ; debut du .text MODULABLE
loop:
	mov al, BYTE [rdi]
	xor rax, rdx
	stosb
	dec ecx
	test ecx, ecx
	jne loop

; MPROTEC
mov rdx, 0x5 ; EXEC | READ
mov rsi, [rsp] ; taille du .text
add rsi, 0x1000 ; + une page
lea rdi, [$ - 0x1] ; debut du .text MODULABLE
and rdi, 0xFFFFFFFFFFFFF000
mov rax, 0xa
syscall

	


add rsp, 0x10
