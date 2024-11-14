#B - Base Pointer
#H - Stack Pointer

Start:
	xor H, H
	xor B, B
	mov A, 7
	str A, H
	
	add H, 1
	mov C, halt
	str C, H
	jmp fib
halt:
	str A, 0
	hlt

fib:
	#Save previous bp register
	add H, 1
	str B, H
	mov B, H

	#Save e register
	add H, 1
	str E, H

	#Get argument to A
	mov D, B
	sub D, 2
	lod A, D
	
	#check if A == 0
	jz A, _done
	
	#check if A == 1
	sub A, 1
	jz A, ret_one

	#call first fibonacci
	add H, 1
	str A, H

	add H, 1
	mov C, second_call
	str C, H
	jmp fib
	
second_call:
	mov E, A
	
	#get n - 2 from stack
	mov D, B
	sub D, 2
	lod A, D
	
	#push n - 2 on stack
	add H, 1
	sub A, 2
	str A, H
	
	#call
	add H, 1
	mov C, fib_end
	str C, H
	jmp fib

fib_end:
	add A, E
	jmp _done
ret_one:
	mov A, 1	
_done:
	mov H, B
	add H, 1
	lod E, H
	sub H, 1
	lod B, H
	sub H, 1
	lod C, H
	jmp C
