# GAS, AT&T
# operand order: src -> dst



.text

.global clock64

clock64:

	push %rdx

	rdtsc

	mov %eax, (%rsp)
	mov %edx, 4(%rsp)
	mov (%rsp), %rax

	pop %rdx

	ret
