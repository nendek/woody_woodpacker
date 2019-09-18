_start:
start:
	mov $0x5, %rdx
	mov $0x5D, %rsi
p1:
	lea 0x2e3e(%rip), %rdi
	mov $0xa, %rax
	syscall

end:
	jmp 0x2e58

