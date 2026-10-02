
/mnt/data/aio18_nr_work/SkyrimUpscaler.dll:	file format coff-x86-64

Disassembly of section .text:

0000000180001000 <.text>:
180294dd7: 49 8b 8e f8 0a 00 00        	movq	0xaf8(%r14), %rcx
180294dde: 48 85 c9                    	testq	%rcx, %rcx
180294de1: 74 0d                       	je	0x180294df0 <.text+0x293df0>
180294de3: 48 8b 01                    	movq	(%rcx), %rax
180294de6: 49 8d 96 20 0b 00 00        	leaq	0xb20(%r14), %rdx
180294ded: ff 50 50                    	callq	*0x50(%rax)
180294df0: c4 c1 78 10 86 30 0b 00 00  	vmovups	0xb30(%r14), %xmm0
180294df9: 49 8b ce                    	movq	%r14, %rcx
180294dfc: c4 c1 79 7e 86 a4 02 00 00  	vmovd	%xmm0, 0x2a4(%r14)
180294e05: e8 16 d3 00 00              	callq	0x1802a2120 <.text+0x2a1120>
180294e0a: 45 38 be aa 02 00 00        	cmpb	%r15b, 0x2aa(%r14)
180294e11: 74 16                       	je	0x180294e29 <.text+0x293e29>
180294e13: 41 b1 01                    	movb	$0x1, %r9b
180294e16: 49 8d 96 f8 0a 00 00        	leaq	0xaf8(%r14), %rdx
180294e1d: 45 0f b6 c1                 	movzbl	%r9b, %r8d
180294e21: 49 8b ce                    	movq	%r14, %rcx
180294e24: e8 d7 d4 00 00              	callq	0x1802a2300 <.text+0x2a1300>
180294e29: 45 38 be e4 04 00 00        	cmpb	%r15b, 0x4e4(%r14)
180294e30: 74 07                       	je	0x180294e39 <.text+0x293e39>
180294e32: ba 01 00 00 00              	movl	$0x1, %edx
180294e37: eb 09                       	jmp	0x180294e42 <.text+0x293e42>
180294e39: 41 8b 96 a0 16 00 00        	movl	0x16a0(%r14), %edx
180294e40: ff ca                       	decl	%edx
180294e42: 41 8b 8e 60 04 00 00        	movl	0x460(%r14), %ecx
180294e49: 33 c0                       	xorl	%eax, %eax
180294e4b: 48 89 45 40                 	movq	%rax, 0x40(%rbp)
180294e4f: 48 8b 44 24 70              	movq	0x70(%rsp), %rax
