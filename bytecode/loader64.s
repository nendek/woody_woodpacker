default rel
global _start
_start:
; OFFSET = 0x2e7A
	push rdi
	push rsi
	push rax
	push rcx
	push rdx
	push r8
	push r9
	push r10
	push r11
	push r12
	mov rdx, 0x5 ; EXEC | READ
	mov rsi, 0x105D ; size shell code + 1 page, modulable (ou non ?)
p1:
	lea rdi, [$ + 0x2E7C - (p1 - _start)]
	and rdi, 0xFFFFFFFFFFFFF000
	mov rax, 0xa
	syscall

p2:
	jmp 0x2E7C - (p2 - _start)
	
	mov rdx, 0x3 ; WRITE | READ
	mov rsi, 0x105D ; size shellcode + 1 page
p3:
	lea rdi, [$ + 0x2E7C - (p3 - _start)]
	and rdi, 0xFFFFFFFFFFFFF000
	mov rax, 0xa
	syscall
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rdx
	pop rcx
	pop rax
	pop rsi
	pop rdi
