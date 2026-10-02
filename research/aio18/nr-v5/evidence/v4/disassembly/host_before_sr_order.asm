
/mnt/data/aio18_nr_work/SkyrimUpscaler.dll:	file format coff-x86-64

Disassembly of section .text:

0000000180001000 <.text>:
180294d70: c4 c1 7a 10 b6 68 02 00 00  	vmovss	0x268(%r14), %xmm6
180294d79: eb 04                       	jmp	0x180294d7f <.text+0x293d7f>
180294d7b: c5 c8 57 f6                 	vxorps	%xmm6, %xmm6, %xmm6
180294d7f: 49 8b 8e f8 0a 00 00        	movq	0xaf8(%r14), %rcx
180294d86: c5 f0 57 c9                 	vxorps	%xmm1, %xmm1, %xmm1
180294d8a: c4 c1 72 2a 8e 74 02 00 00  	vcvtsi2ssl	0x274(%r14), %xmm1, %xmm1
180294d93: c5 f8 57 c0                 	vxorps	%xmm0, %xmm0, %xmm0
180294d97: c4 c1 7a 2a 86 70 02 00 00  	vcvtsi2ssl	0x270(%r14), %xmm0, %xmm0
180294da0: c5 f2 5e c8                 	vdivss	%xmm0, %xmm1, %xmm1
180294da4: c4 c1 72 59 96 f0 02 00 00  	vmulss	0x2f0(%r14), %xmm1, %xmm2
180294dad: c5 ea 59 3d a3 6e 17 00     	vmulss	0x176ea3(%rip), %xmm2, %xmm7 # 0x18040bc58
180294db5: 48 85 c9                    	testq	%rcx, %rcx
180294db8: 74 0d                       	je	0x180294dc7 <.text+0x293dc7>
180294dba: 48 8b 01                    	movq	(%rcx), %rax
180294dbd: 49 8d 96 20 0b 00 00        	leaq	0xb20(%r14), %rdx
180294dc4: ff 50 50                    	callq	*0x50(%rax)
180294dc7: 41 8b 86 30 0b 00 00        	movl	0xb30(%r14), %eax
180294dce: 41 39 86 a4 02 00 00        	cmpl	%eax, 0x2a4(%r14)
180294dd5: 74 33                       	je	0x180294e0a <.text+0x293e0a>
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
180294e54: c5 f9 ef c0                 	vpxor	%xmm0, %xmm0, %xmm0
180294e58: c5 fc 11 45 10              	vmovups	%ymm0, 0x10(%rbp)
180294e5d: c5 f8 11 45 30              	vmovups	%xmm0, 0x30(%rbp)
180294e62: 48 89 45 e8                 	movq	%rax, -0x18(%rbp)
180294e66: 48 8b 45 88                 	movq	-0x78(%rbp), %rax
180294e6a: 48 89 45 f0                 	movq	%rax, -0x10(%rbp)
180294e6e: 48 8b 85 a8 01 00 00        	movq	0x1a8(%rbp), %rax
180294e75: c5 f8 57 c0                 	vxorps	%xmm0, %xmm0, %xmm0
180294e79: c4 c1 7a 2a 86 78 02 00 00  	vcvtsi2ssl	0x278(%r14), %xmm0, %xmm0
180294e82: c5 fa 11 45 18              	vmovss	%xmm0, 0x18(%rbp)
180294e87: c4 c1 7a 10 46 08           	vmovss	0x8(%r14), %xmm0
180294e8d: 48 89 45 f8                 	movq	%rax, -0x8(%rbp)
180294e91: c4 c1 7a 2c 46 10           	vcvttss2si	0x10(%r14), %eax
180294e97: c5 fa 11 45 24              	vmovss	%xmm0, 0x24(%rbp)
180294e9c: c5 f0 57 c9                 	vxorps	%xmm1, %xmm1, %xmm1
180294ea0: c4 c1 72 2a 8e 7c 02 00 00  	vcvtsi2ssl	0x27c(%r14), %xmm1, %xmm1
180294ea9: c5 fa 11 4d 1c              	vmovss	%xmm1, 0x1c(%rbp)
180294eae: c4 c1 7a 10 4e 0c           	vmovss	0xc(%r14), %xmm1
180294eb4: c5 fa 11 4d 28              	vmovss	%xmm1, 0x28(%rbp)
180294eb9: c5 f8 57 c0                 	vxorps	%xmm0, %xmm0, %xmm0
180294ebd: c5 fa 2a c0                 	vcvtsi2ss	%eax, %xmm0, %xmm0
180294ec1: c4 c1 7a 2c 46 14           	vcvttss2si	0x14(%r14), %eax
180294ec7: c5 fa 11 45 2c              	vmovss	%xmm0, 0x2c(%rbp)
180294ecc: c4 c1 7a 10 86 f8 02 00 00  	vmovss	0x2f8(%r14), %xmm0
180294ed5: c5 fa 11 75 20              	vmovss	%xmm6, 0x20(%rbp)
180294eda: c5 fa 11 45 38              	vmovss	%xmm0, 0x38(%rbp)
180294edf: c5 fa 11 7d 40              	vmovss	%xmm7, 0x40(%rbp)
180294ee4: 89 95 80 00 00 00           	movl	%edx, 0x80(%rbp)
180294eea: 48 8d 95 90 00 00 00        	leaq	0x90(%rbp), %rdx
180294ef1: 4c 89 7d e0                 	movq	%r15, -0x20(%rbp)
180294ef5: 4c 89 7d 08                 	movq	%r15, 0x8(%rbp)
180294ef9: 4c 89 7d 50                 	movq	%r15, 0x50(%rbp)
180294efd: 4c 89 7d 58                 	movq	%r15, 0x58(%rbp)
180294f01: 4c 89 7d 60                 	movq	%r15, 0x60(%rbp)
180294f05: 4c 89 7d 48                 	movq	%r15, 0x48(%rbp)
180294f09: 4c 89 7d 00                 	movq	%r15, (%rbp)
180294f0d: 4c 89 7d 10                 	movq	%r15, 0x10(%rbp)
180294f11: 4c 89 6d 68                 	movq	%r13, 0x68(%rbp)
180294f15: 48 89 7d 70                 	movq	%rdi, 0x70(%rbp)
180294f19: 44 88 7d 34                 	movb	%r15b, 0x34(%rbp)
180294f1d: 89 4d 78                    	movl	%ecx, 0x78(%rbp)
180294f20: c6 45 44 01                 	movb	$0x1, 0x44(%rbp)
180294f24: c6 45 7c 01                 	movb	$0x1, 0x7c(%rbp)
180294f28: 4c 89 bd 88 00 00 00        	movq	%r15, 0x88(%rbp)
180294f2f: c5 f0 57 c9                 	vxorps	%xmm1, %xmm1, %xmm1
180294f33: c5 f2 2a c8                 	vcvtsi2ss	%eax, %xmm1, %xmm1
180294f37: c5 fa 11 4d 30              	vmovss	%xmm1, 0x30(%rbp)
180294f3c: c4 c1 7a 10 8e f4 02 00 00  	vmovss	0x2f4(%r14), %xmm1
180294f45: c5 fa 11 4d 3c              	vmovss	%xmm1, 0x3c(%rbp)
180294f4a: 48 8d 45 e0                 	leaq	-0x20(%rbp), %rax
180294f4e: c5 fc 10 00                 	vmovups	(%rax), %ymm0
180294f52: c5 fc 10 90 80 00 00 00     	vmovups	0x80(%rax), %ymm2
180294f5a: c5 fc 11 02                 	vmovups	%ymm0, (%rdx)
180294f5e: c5 fc 10 40 20              	vmovups	0x20(%rax), %ymm0
180294f63: c5 fc 11 42 20              	vmovups	%ymm0, 0x20(%rdx)
180294f68: c5 fc 10 40 40              	vmovups	0x40(%rax), %ymm0
180294f6d: c5 fc 11 42 40              	vmovups	%ymm0, 0x40(%rdx)
180294f72: c5 fc 10 40 60              	vmovups	0x60(%rax), %ymm0
180294f77: c5 fc 11 42 60              	vmovups	%ymm0, 0x60(%rdx)
180294f7c: c5 fc 11 92 80 00 00 00     	vmovups	%ymm2, 0x80(%rdx)
180294f84: c5 f8 10 90 a0 00 00 00     	vmovups	0xa0(%rax), %xmm2
180294f8c: 41 0f b6 86 64 02 00 00     	movzbl	0x264(%r14), %eax
180294f94: c5 f8 11 92 a0 00 00 00     	vmovups	%xmm2, 0xa0(%rdx)
180294f9c: c5 f8 28 cf                 	vmovaps	%xmm7, %xmm1
180294fa0: 88 85 e4 00 00 00           	movb	%al, 0xe4(%rbp)
180294fa6: c5 f8 77                    	vzeroupper
180294fa9: e8 32 7d fe ff              	callq	0x18027cce0 <.text+0x27bce0>
180294fae: 48 8d 8d 90 00 00 00        	leaq	0x90(%rbp), %rcx
180294fb5: c5 fa 11 85 f0 00 00 00     	vmovss	%xmm0, 0xf0(%rbp)
180294fbd: ff 15 5d dc 1d 00           	callq	*0x1ddc5d(%rip)         # 0x180472c20 <SKSEPlugin_Version+0x11870>
180294fc3: c5 f8 28 bc 24 40 02 00 00  	vmovaps	0x240(%rsp), %xmm7
180294fcc: c5 f8 28 b4 24 50 02 00 00  	vmovaps	0x250(%rsp), %xmm6
180294fd5: 4c 8b ac 24 a0 02 00 00     	movq	0x2a0(%rsp), %r13
180294fdd: 45 88 be 64 02 00 00        	movb	%r15b, 0x264(%r14)
180294fe4: 45 38 be 8e 04 00 00        	cmpb	%r15b, 0x48e(%r14)
180294feb: 0f 85 0f 01 00 00           	jne	0x180295100 <.text+0x294100>
180294ff1: 49 8b 8e 80 16 00 00        	movq	0x1680(%r14), %rcx
180294ff8: 4c 8d 85 a8 01 00 00        	leaq	0x1a8(%rbp), %r8
180294fff: 48 8d 15 6a 2b 17 00        	leaq	0x172b6a(%rip), %rdx    # 0x180407b70
180295006: 48 8b 01                    	movq	(%rcx), %rax
180295009: ff 10                       	callq	*(%rax)
18029500b: 48 8b 8d a8 01 00 00        	movq	0x1a8(%rbp), %rcx
180295012: 48 8d 15 a7 22 17 00        	leaq	0x1722a7(%rip), %rdx    # 0x1804072c0
180295019: 48 8b 01                    	movq	(%rcx), %rax
18029501c: ff 50 18                    	callq	*0x18(%rax)
18029501f: 49 8b 8e 80 16 00 00        	movq	0x1680(%r14), %rcx
