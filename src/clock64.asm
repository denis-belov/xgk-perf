; MASM, Intel
; operand order: dst <- src



.DATA

.CODE

	clock64 PROC

		; rdtsc writes to eax and edx
		; so rdx value needs to be pushed
		; into stack before calling rdtsc
		push rdx

		rdtsc

		; writing values from eax and edx onto the top of stack
		; and then writing top 64-bit stack value to rax
		mov [rsp], eax
		mov 4[rsp], edx
		mov rax, [rsp]

		pop rdx

		; returning rax value
		ret 0
	clock64 ENDP
END
