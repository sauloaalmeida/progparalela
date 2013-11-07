	.file	"ex1.c"
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"--Cria a thread %d\n"
.LC1:
	.string	"--ERRO: pthread_create()\n"
.LC2:
	.string	"--ERRO: pthread_join() \n"
.LC3:
	.string	"Valor de s = %d\n"
	.text
.globl main
	.type	main, @function
main:
	pushl	%ebp
	movl	%esp, %ebp
	andl	$-16, %esp
	subl	$32, %esp
	movl	$0, 8(%esp)
	movl	$.LC0, 4(%esp)
	movl	$1, (%esp)
	call	__printf_chk
	movl	$0, 12(%esp)
	movl	$ExecutaTarefa, 8(%esp)
	movl	$0, 4(%esp)
	leal	24(%esp), %eax
	movl	%eax, (%esp)
	call	pthread_create
	testl	%eax, %eax
	jne	.L2
	movl	$1, 8(%esp)
	movl	$.LC0, 4(%esp)
	movl	$1, (%esp)
	call	__printf_chk
	movl	$1, 12(%esp)
	movl	$ExecutaTarefa, 8(%esp)
	movl	$0, 4(%esp)
	leal	28(%esp), %eax
	movl	%eax, (%esp)
	call	pthread_create
	testl	%eax, %eax
	je	.L3
.L2:
	movl	$.LC1, 4(%esp)
	movl	$1, (%esp)
	call	__printf_chk
	movl	$-1, (%esp)
	call	exit
.L3:
	movl	$0, 4(%esp)
	movl	24(%esp), %eax
	movl	%eax, (%esp)
	call	pthread_join
	testl	%eax, %eax
	jne	.L4
	movl	$0, 4(%esp)
	movl	28(%esp), %eax
	movl	%eax, (%esp)
	call	pthread_join
	testl	%eax, %eax
	je	.L5
.L4:
	movl	$.LC2, 4(%esp)
	movl	$1, (%esp)
	call	__printf_chk
	movl	$-1, (%esp)
	call	exit
.L5:
	movl	s, %eax
	movl	%eax, 8(%esp)
	movl	$.LC3, 4(%esp)
	movl	$1, (%esp)
	call	__printf_chk
	movl	$0, (%esp)
	call	pthread_exit
	.size	main, .-main
	.section	.rodata.str1.4,"aMS",@progbits,1
	.align 4
.LC4:
	.string	"Thread : %d esta executando...\n"
	.section	.rodata.str1.1
.LC5:
	.string	"Thread : %d terminou!\n"
	.text
.globl ExecutaTarefa
	.type	ExecutaTarefa, @function
ExecutaTarefa:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$20, %esp
	movl	8(%ebp), %ebx
	movl	%ebx, 8(%esp)
	movl	$.LC4, 4(%esp)
	movl	$1, (%esp)
	call	__printf_chk
	movl	$0, %eax
.L8:
	movl	s, %edx
	addl	$1, %edx
	movl	%edx, s
	addl	$1, %eax
	cmpl	$10000000, %eax
	jne	.L8
	movl	%ebx, 8(%esp)
	movl	$.LC5, 4(%esp)
	movl	$1, (%esp)
	call	__printf_chk
	movl	$0, (%esp)
	call	pthread_exit
	.size	ExecutaTarefa, .-ExecutaTarefa
.globl s
	.bss
	.align 4
	.type	s, @object
	.size	s, 4
s:
	.zero	4
	.ident	"GCC: (Ubuntu 4.4.3-4ubuntu5) 4.4.3"
	.section	.note.GNU-stack,"",@progbits
