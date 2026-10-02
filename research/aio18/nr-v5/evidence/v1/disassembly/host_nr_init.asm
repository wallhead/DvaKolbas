
/mnt/data/aio18_nr_work/SkyrimUpscaler.dll:	file format coff-x86-64

Disassembly of section .text:

0000000180001000 <.text>:
1802a2120: 48 89 7c 24 10              	movq	%rdi, 0x10(%rsp)
1802a2125: 55                          	pushq	%rbp
1802a2126: 48 8d 6c 24 a9              	leaq	-0x57(%rsp), %rbp
1802a212b: 48 81 ec a0 00 00 00        	subq	$0xa0, %rsp
1802a2132: 48 8b f9                    	movq	%rcx, %rdi
1802a2135: 33 c9                       	xorl	%ecx, %ecx
1802a2137: ff 15 43 0b 1d 00           	callq	*0x1d0b43(%rip)         # 0x180472c80 <SKSEPlugin_Version+0x118d0>
1802a213d: c6 87 a9 02 00 00 00        	movb	$0x0, 0x2a9(%rdi)
1802a2144: ff 15 3e 0b 1d 00           	callq	*0x1d0b3e(%rip)         # 0x180472c88 <SKSEPlugin_Version+0x118d8>
1802a214a: 84 c0                       	testb	%al, %al
1802a214c: 0f 84 8e 01 00 00           	je	0x1802a22e0 <.text+0x2a12e0>
1802a2152: 44 0f b6 87 aa 02 00 00     	movzbl	0x2aa(%rdi), %r8d
1802a215a: 0f b6 97 43 03 00 00        	movzbl	0x343(%rdi), %edx
1802a2161: c7 45 d7 00 00 00 00        	movl	$0x0, -0x29(%rbp)
1802a2168: 45 84 c0                    	testb	%r8b, %r8b
1802a216b: 74 2d                       	je	0x1802a219a <.text+0x2a119a>
1802a216d: 84 d2                       	testb	%dl, %dl
1802a216f: 74 0a                       	je	0x1802a217b <.text+0x2a117b>
1802a2171: 8b 87 4c 03 00 00           	movl	0x34c(%rdi), %eax
1802a2177: 85 c0                       	testl	%eax, %eax
1802a2179: 75 06                       	jne	0x1802a2181 <.text+0x2a1181>
1802a217b: 8b 87 78 02 00 00           	movl	0x278(%rdi), %eax
1802a2181: 89 45 db                    	movl	%eax, -0x25(%rbp)
1802a2184: 84 d2                       	testb	%dl, %dl
1802a2186: 74 0a                       	je	0x1802a2192 <.text+0x2a1192>
1802a2188: 8b 8f 50 03 00 00           	movl	0x350(%rdi), %ecx
1802a218e: 85 c9                       	testl	%ecx, %ecx
1802a2190: 75 17                       	jne	0x1802a21a9 <.text+0x2a11a9>
1802a2192: 8b 8f 7c 02 00 00           	movl	0x27c(%rdi), %ecx
1802a2198: eb 0f                       	jmp	0x1802a21a9 <.text+0x2a11a9>
1802a219a: 8b 87 70 02 00 00           	movl	0x270(%rdi), %eax
1802a21a0: 8b 8f 74 02 00 00           	movl	0x274(%rdi), %ecx
1802a21a6: 89 45 db                    	movl	%eax, -0x25(%rbp)
1802a21a9: 89 87 64 03 00 00           	movl	%eax, 0x364(%rdi)
1802a21af: 8b 87 9c 02 00 00           	movl	0x29c(%rdi), %eax
1802a21b5: 89 45 e3                    	movl	%eax, -0x1d(%rbp)
1802a21b8: 89 4d df                    	movl	%ecx, -0x21(%rbp)
1802a21bb: 89 8f 68 03 00 00           	movl	%ecx, 0x368(%rdi)
1802a21c1: c5 fa 10 05 6b 9b 16 00     	vmovss	0x169b6b(%rip), %xmm0   # 0x18040bd34
1802a21c9: c5 fa 11 45 e7              	vmovss	%xmm0, -0x19(%rbp)
1802a21ce: 84 d2                       	testb	%dl, %dl
1802a21d0: 74 54                       	je	0x1802a2226 <.text+0x2a1226>
1802a21d2: 45 84 c0                    	testb	%r8b, %r8b
1802a21d5: 75 4f                       	jne	0x1802a2226 <.text+0x2a1226>
1802a21d7: 48 8b 8f 38 0a 00 00        	movq	0xa38(%rdi), %rcx
1802a21de: 48 85 c9                    	testq	%rcx, %rcx
1802a21e1: 74 43                       	je	0x1802a2226 <.text+0x2a1226>
1802a21e3: 48 8b 01                    	movq	(%rcx), %rax
1802a21e6: 48 8d 97 60 0a 00 00        	leaq	0xa60(%rdi), %rdx
1802a21ed: 48 89 9c 24 b0 00 00 00     	movq	%rbx, 0xb0(%rsp)
1802a21f5: ff 50 50                    	callq	*0x50(%rax)
1802a21f8: c5 fb 10 87 80 0a 00 00     	vmovsd	0xa80(%rdi), %xmm0
1802a2200: 8b 87 88 0a 00 00           	movl	0xa88(%rdi), %eax
1802a2206: 48 8b 9c 24 b0 00 00 00     	movq	0xb0(%rsp), %rbx
1802a220e: c5 fb 11 45 47              	vmovsd	%xmm0, 0x47(%rbp)
1802a2213: c5 f8 10 87 70 0a 00 00     	vmovups	0xa70(%rdi), %xmm0
1802a221b: c5 f9 7e 87 a4 02 00 00     	vmovd	%xmm0, 0x2a4(%rdi)
1802a2223: 89 45 4f                    	movl	%eax, 0x4f(%rbp)
1802a2226: 80 bf f4 03 00 00 00        	cmpb	$0x0, 0x3f4(%rdi)
1802a222d: 8b 87 a4 02 00 00           	movl	0x2a4(%rdi), %eax
1802a2233: 89 45 eb                    	movl	%eax, -0x15(%rbp)
1802a2236: 0f b6 87 fc 02 00 00        	movzbl	0x2fc(%rdi), %eax
1802a223d: 88 45 ef                    	movb	%al, -0x11(%rbp)
1802a2240: 8b 87 ac 02 00 00           	movl	0x2ac(%rdi), %eax
1802a2246: 89 45 f3                    	movl	%eax, -0xd(%rbp)
1802a2249: 75 13                       	jne	0x1802a225e <.text+0x2a125e>
1802a224b: 8b 87 ec 03 00 00           	movl	0x3ec(%rdi), %eax
1802a2251: 89 87 f0 03 00 00           	movl	%eax, 0x3f0(%rdi)
1802a2257: c6 87 f4 03 00 00 01        	movb	$0x1, 0x3f4(%rdi)
1802a225e: 8b 87 f0 03 00 00           	movl	0x3f0(%rdi), %eax
1802a2264: 48 8d 4d d7                 	leaq	-0x29(%rbp), %rcx
1802a2268: 89 45 f7                    	movl	%eax, -0x9(%rbp)
1802a226b: ff 15 1f 0a 1d 00           	callq	*0x1d0a1f(%rip)         # 0x180472c90 <SKSEPlugin_Version+0x118e0>
1802a2271: 88 87 a9 02 00 00           	movb	%al, 0x2a9(%rdi)
1802a2277: 84 c0                       	testb	%al, %al
1802a2279: 75 65                       	jne	0x1802a22e0 <.text+0x2a12e0>
1802a227b: e8 d0 fd f7 ff              	callq	0x180222050 <.text+0x221050>
1802a2280: 48 8d 0d a9 3e 16 00        	leaq	0x163ea9(%rip), %rcx    # 0x180406130
1802a2287: c7 45 07 c5 0c 00 00        	movl	$0xcc5, 0x7(%rbp)       # imm = 0xCC5
1802a228e: 48 89 4d ff                 	movq	%rcx, -0x1(%rbp)
1802a2292: 4c 8d 4d 17                 	leaq	0x17(%rbp), %r9
1802a2296: 8b 4d 33                    	movl	0x33(%rbp), %ecx
1802a2299: 48 8d 55 27                 	leaq	0x27(%rbp), %rdx
1802a229d: 89 4d 0b                    	movl	%ecx, 0xb(%rbp)
1802a22a0: 41 b8 04 00 00 00           	movl	$0x4, %r8d
1802a22a6: c5 f8 10 45 ff              	vmovups	-0x1(%rbp), %xmm0
1802a22ab: 48 8d 0d 8e 53 16 00        	leaq	0x16538e(%rip), %rcx    # 0x180407640
1802a22b2: 48 c7 45 1f 12 00 00 00     	movq	$0x12, 0x1f(%rbp)
1802a22ba: 48 89 4d 0f                 	movq	%rcx, 0xf(%rbp)
1802a22be: 48 8d 0d e3 53 16 00        	leaq	0x1653e3(%rip), %rcx    # 0x1804076a8
1802a22c5: c5 fb 10 4d 0f              	vmovsd	0xf(%rbp), %xmm1
1802a22ca: 48 89 4d 17                 	movq	%rcx, 0x17(%rbp)
1802a22ce: 48 8b c8                    	movq	%rax, %rcx
1802a22d1: c5 f8 11 45 27              	vmovups	%xmm0, 0x27(%rbp)
1802a22d6: c5 fb 11 4d 37              	vmovsd	%xmm1, 0x37(%rbp)
1802a22db: e8 b0 f6 ea ff              	callq	0x180151990 <.text+0x150990>
1802a22e0: 48 8b bc 24 b8 00 00 00     	movq	0xb8(%rsp), %rdi
1802a22e8: 48 81 c4 a0 00 00 00        	addq	$0xa0, %rsp
1802a22ef: 5d                          	popq	%rbp
1802a22f0: c3                          	retq
