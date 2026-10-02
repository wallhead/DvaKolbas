
/mnt/data/aio18_nr_work/SkyrimUpscaler.dll:	file format coff-x86-64

Disassembly of section .text:

0000000180001000 <.text>:
180265a80: 4d 04 48                    	addb	$0x48, %al
180265a83: 89 4c 24 38                 	movl	%ecx, 0x38(%rsp)
180265a87: 48 8d 4d 00                 	leaq	(%rbp), %rcx
180265a8b: 48 89 4c 24 30              	movq	%rcx, 0x30(%rsp)
180265a90: 48 8d 4d bc                 	leaq	-0x44(%rbp), %rcx
180265a94: 48 89 4c 24 28              	movq	%rcx, 0x28(%rsp)
180265a99: 48 8d 4d b8                 	leaq	-0x48(%rbp), %rcx
180265a9d: 48 89 4c 24 20              	movq	%rcx, 0x20(%rsp)
180265aa2: 4c 8d 4d 30                 	leaq	0x30(%rbp), %r9
180265aa6: 48 8d 54 24 70              	leaq	0x70(%rsp), %rdx
180265aab: 48 8b c8                    	movq	%rax, %rcx
180265aae: e8 bd 75 f0 ff              	callq	0x18016d070 <.text+0x16c070>
180265ab3: 80 3f 00                    	cmpb	$0x0, (%rdi)
180265ab6: 0f 84 9b 00 00 00           	je	0x180265b57 <.text+0x264b57>
180265abc: 48 8b 8b 80 16 00 00        	movq	0x1680(%rbx), %rcx
180265ac3: 48 8b 01                    	movq	(%rcx), %rax
180265ac6: 48 8d 55 a0                 	leaq	-0x60(%rbp), %rdx
180265aca: 48 89 54 24 40              	movq	%rdx, 0x40(%rsp)
180265acf: 44 89 74 24 38              	movl	%r14d, 0x38(%rsp)
180265ad4: 48 89 74 24 30              	movq	%rsi, 0x30(%rsp)
180265ad9: 44 89 74 24 28              	movl	%r14d, 0x28(%rsp)
180265ade: 44 89 74 24 20              	movl	%r14d, 0x20(%rsp)
180265ae3: 45 33 c9                    	xorl	%r9d, %r9d
180265ae6: 45 33 c0                    	xorl	%r8d, %r8d
180265ae9: 48 8b 93 f8 0a 00 00        	movq	0xaf8(%rbx), %rdx
180265af0: c5 f8 77                    	vzeroupper
180265af3: ff 90 70 01 00 00           	callq	*0x170(%rax)
180265af9: 41 b1 01                    	movb	$0x1, %r9b
180265afc: 45 0f b6 c1                 	movzbl	%r9b, %r8d
180265b00: 48 8d 93 f8 0a 00 00        	leaq	0xaf8(%rbx), %rdx
180265b07: 48 8b cb                    	movq	%rbx, %rcx
180265b0a: e8 f1 c7 03 00              	callq	0x1802a2300 <.text+0x2a1300>
180265b0f: 48 8b 8b 80 16 00 00        	movq	0x1680(%rbx), %rcx
180265b16: 48 8b 01                    	movq	(%rcx), %rax
180265b19: 4c 8b 90 70 01 00 00        	movq	0x170(%rax), %r10
180265b20: 48 8d 45 a0                 	leaq	-0x60(%rbp), %rax
180265b24: 48 89 44 24 40              	movq	%rax, 0x40(%rsp)
180265b29: 44 89 74 24 38              	movl	%r14d, 0x38(%rsp)
180265b2e: 48 8b 83 f8 0a 00 00        	movq	0xaf8(%rbx), %rax
180265b35: 48 89 44 24 30              	movq	%rax, 0x30(%rsp)
180265b3a: 44 89 74 24 28              	movl	%r14d, 0x28(%rsp)
180265b3f: 44 89 74 24 20              	movl	%r14d, 0x20(%rsp)
180265b44: 45 33 c9                    	xorl	%r9d, %r9d
180265b47: 45 33 c0                    	xorl	%r8d, %r8d
180265b4a: 48 8b d6                    	movq	%rsi, %rdx
180265b4d: 41 ff d2                    	callq	*%r10
180265b50: c6 83 8c 04 00 00 01        	movb	$0x1, 0x48c(%rbx)
180265b57: 32 db                       	xorb	%bl, %bl
180265b59: 48 8d 4d 40                 	leaq	0x40(%rbp), %rcx
180265b5d: c5 f8 77                    	vzeroupper
180265b60: e8 2b 00 00 00              	callq	0x180265b90 <.text+0x264b90>
180265b65: 0f b6 c3                    	movzbl	%bl, %eax
180265b68: eb 02                       	jmp	0x180265b6c <.text+0x264b6c>
180265b6a: 32 c0                       	xorb	%al, %al
180265b6c: 48 8b 9c 24 f0 01 00 00     	movq	0x1f0(%rsp), %rbx
180265b74: 48 81 c4 a0 01 00 00        	addq	$0x1a0, %rsp            # imm = 0x1A0
180265b7b: 41 5f                       	popq	%r15
180265b7d: 41 5e                       	popq	%r14
180265b7f: 41 5d                       	popq	%r13
