	.file	"exemplo3restrict.c"
	.text
	.globl	f1
	.type	f1, @function
f1:
.LFB13:
	.cfi_startproc
	movl	4(%esp), %eax
	movl	8(%esp), %edx
	movl	(%edx), %edx
	addl	%edx, %edx
	addl	%edx, (%eax)
	ret
	.cfi_endproc
.LFE13:
	.size	f1, .-f1
	.globl	f2
	.type	f2, @function
f2:
.LFB14:
	.cfi_startproc
	movl	4(%esp), %eax
	movl	8(%esp), %edx
	movl	(%edx), %edx
	addl	%edx, %edx
	addl	%edx, (%eax)
	ret
	.cfi_endproc
.LFE14:
	.size	f2, .-f2
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"Para xp != yp:\n"
.LC1:
	.string	"Apos f1, x=%d\n"
.LC2:
	.string	"Apos f2, x=%d\n"
.LC3:
	.string	"Para xp == yp:\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB15:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	%esi
	pushl	%ebx
	andl	$-16, %esp
	subl	$32, %esp
	.cfi_offset 6, -12
	.cfi_offset 3, -16
	movl	$.LC0, 4(%esp)
	movl	$1, (%esp)
	call	__printf_chk
	movl	$5, 28(%esp)
	movl	$4, 24(%esp)
	leal	24(%esp), %esi
	movl	%esi, 4(%esp)
	leal	28(%esp), %ebx
	movl	%ebx, (%esp)
	call	f1
	movl	28(%esp), %eax
	movl	%eax, 8(%esp)
	movl	$.LC1, 4(%esp)
	movl	$1, (%esp)
	call	__printf_chk
	movl	$5, 28(%esp)
	movl	$4, 24(%esp)
	movl	%esi, 4(%esp)
	movl	%ebx, (%esp)
	call	f2
	movl	28(%esp), %eax
	movl	%eax, 8(%esp)
	movl	$.LC2, 4(%esp)
	movl	$1, (%esp)
	call	__printf_chk
	movl	$.LC3, 4(%esp)
	movl	$1, (%esp)
	call	__printf_chk
	movl	$5, 28(%esp)
	movl	%ebx, 4(%esp)
	movl	%ebx, (%esp)
	call	f1
	movl	28(%esp), %eax
	movl	%eax, 8(%esp)
	movl	$.LC1, 4(%esp)
	movl	$1, (%esp)
	call	__printf_chk
	movl	$5, 28(%esp)
	movl	%ebx, 4(%esp)
	movl	%ebx, (%esp)
	call	f2
	movl	28(%esp), %eax
	movl	%eax, 8(%esp)
	movl	$.LC2, 4(%esp)
	movl	$1, (%esp)
	call	__printf_chk
	movl	$1, %eax
	leal	-8(%ebp), %esp
	popl	%ebx
	.cfi_restore 3
	popl	%esi
	.cfi_restore 6
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE15:
	.size	main, .-main
	.ident	"GCC: (Gentoo 4.7.3 p1.2, pie-0.5.5) 4.7.3"
	.section	.note.GNU-stack,"",@progbits
