
/mnt/data/aio18_nr_work/SkyrimUpscaler.dll:	file format coff-x86-64

Disassembly of section .text:

0000000180001000 <.text>:
1802a2480: 75 06                       	jne	0x1802a2488 <.text+0x2a1488>
1802a2482: 49 8b f4                    	movq	%r12, %rsi
1802a2485: 49 8b cc                    	movq	%r12, %rcx
1802a2488: 0f b6 83 c4 02 00 00        	movzbl	0x2c4(%rbx), %eax
1802a248f: 48 85 f6                    	testq	%rsi, %rsi
1802a2492: c5 fa 10 43 10              	vmovss	0x10(%rbx), %xmm0
1802a2497: c5 fa 10 4b 14              	vmovss	0x14(%rbx), %xmm1
1802a249c: 88 45 18                    	movb	%al, 0x18(%rbp)
1802a249f: 8b 83 b0 02 00 00           	movl	0x2b0(%rbx), %eax
1802a24a5: 89 45 1c                    	movl	%eax, 0x1c(%rbp)
1802a24a8: 0f b6 83 fc 02 00 00        	movzbl	0x2fc(%rbx), %eax
1802a24af: 88 45 21                    	movb	%al, 0x21(%rbp)
1802a24b2: 0f b6 83 a8 02 00 00        	movzbl	0x2a8(%rbx), %eax
1802a24b9: c5 fa 11 45 00              	vmovss	%xmm0, (%rbp)
1802a24be: c5 f8 10 83 b4 02 00 00     	vmovups	0x2b4(%rbx), %xmm0
1802a24c6: 48 89 74 24 50              	movq	%rsi, 0x50(%rsp)
1802a24cb: 48 8b b4 24 b0 01 00 00     	movq	0x1b0(%rsp), %rsi
1802a24d3: 88 45 22                    	movb	%al, 0x22(%rbp)
1802a24d6: c5 fa 11 4d 04              	vmovss	%xmm1, 0x4(%rbp)
1802a24db: 48 89 4c 24 58              	movq	%rcx, 0x58(%rsp)
1802a24e0: c5 f8 11 45 08              	vmovups	%xmm0, 0x8(%rbp)
1802a24e5: 44 88 65 20                 	movb	%r12b, 0x20(%rbp)
1802a24e9: 74 09                       	je	0x1802a24f4 <.text+0x2a14f4>
1802a24eb: c6 45 23 01                 	movb	$0x1, 0x23(%rbp)
1802a24ef: 48 85 c9                    	testq	%rcx, %rcx
1802a24f2: 75 04                       	jne	0x1802a24f8 <.text+0x2a14f8>
1802a24f4: 44 88 65 23                 	movb	%r12b, 0x23(%rbp)
1802a24f8: 4c 89 65 28                 	movq	%r12, 0x28(%rbp)
1802a24fc: 44 38 a3 e4 04 00 00        	cmpb	%r12b, 0x4e4(%rbx)
1802a2503: 75 1b                       	jne	0x1802a2520 <.text+0x2a1520>
1802a2505: 45 84 f6                    	testb	%r14b, %r14b
1802a2508: 75 16                       	jne	0x1802a2520 <.text+0x2a1520>
1802a250a: 44 38 a3 aa 02 00 00        	cmpb	%r12b, 0x2aa(%rbx)
1802a2511: 75 0d                       	jne	0x1802a2520 <.text+0x2a1520>
1802a2513: c6 45 30 01                 	movb	$0x1, 0x30(%rbp)
1802a2517: 44 39 a3 dc 03 00 00        	cmpl	%r12d, 0x3dc(%rbx)
1802a251e: 7f 04                       	jg	0x1802a2524 <.text+0x2a1524>
1802a2520: 44 88 65 30                 	movb	%r12b, 0x30(%rbp)
1802a2524: c5 f8 10 83 cc 02 00 00     	vmovups	0x2cc(%rbx), %xmm0
1802a252c: 48 8b 0f                    	movq	(%rdi), %rcx
1802a252f: 8b 93 c8 02 00 00           	movl	0x2c8(%rbx), %edx
1802a2535: 89 55 34                    	movl	%edx, 0x34(%rbp)
1802a2538: c5 f8 11 45 38              	vmovups	%xmm0, 0x38(%rbp)
1802a253d: c5 fa 10 83 dc 02 00 00     	vmovss	0x2dc(%rbx), %xmm0
1802a2545: c5 fa 11 45 48              	vmovss	%xmm0, 0x48(%rbp)
1802a254a: 48 85 c9                    	testq	%rcx, %rcx
1802a254d: 74 0d                       	je	0x1802a255c <.text+0x2a155c>
1802a254f: 48 8b 01                    	movq	(%rcx), %rax
1802a2552: 48 8d 57 28                 	leaq	0x28(%rdi), %rdx
1802a2556: ff 50 50                    	callq	*0x50(%rax)
1802a2559: 8b 55 34                    	movl	0x34(%rbp), %edx
1802a255c: b8 28 00 00 00              	movl	$0x28, %eax
1802a2561: c5 fc 10 0c 38              	vmovups	(%rax,%rdi), %ymm1
1802a2566: 44 38 a3 43 03 00 00        	cmpb	%r12b, 0x343(%rbx)
1802a256d: 74 2b                       	je	0x1802a259a <.text+0x2a159a>
1802a256f: 44 38 a3 aa 02 00 00        	cmpb	%r12b, 0x2aa(%rbx)
1802a2576: 74 22                       	je	0x1802a259a <.text+0x2a159a>
1802a2578: 45 84 ff                    	testb	%r15b, %r15b
1802a257b: 74 1d                       	je	0x1802a259a <.text+0x2a159a>
1802a257d: c4 e3 7d 19 c8 01           	vextractf128	$0x1, %ymm1, %xmm0
1802a2583: c5 f9 7e c1                 	vmovd	%xmm0, %ecx
1802a2587: 8d 41 fe                    	leal	-0x2(%rcx), %eax
1802a258a: a9 e7 ff ff ff              	testl	$0xffffffe7, %eax       # imm = 0xFFFFFFE7
1802a258f: 75 09                       	jne	0x1802a259a <.text+0x2a159a>
1802a2591: 83 f9 12                    	cmpl	$0x12, %ecx
1802a2594: 74 04                       	je	0x1802a259a <.text+0x2a159a>
1802a2596: b0 01                       	movb	$0x1, %al
1802a2598: eb 02                       	jmp	0x1802a259c <.text+0x2a159c>
1802a259a: 32 c0                       	xorb	%al, %al
1802a259c: 44 38 a3 e0 02 00 00        	cmpb	%r12b, 0x2e0(%rbx)
1802a25a3: 75 0a                       	jne	0x1802a25af <.text+0x2a15af>
1802a25a5: 84 c0                       	testb	%al, %al
1802a25a7: 75 06                       	jne	0x1802a25af <.text+0x2a15af>
1802a25a9: 44 88 65 4c                 	movb	%r12b, 0x4c(%rbp)
1802a25ad: eb 11                       	jmp	0x1802a25c0 <.text+0x2a15c0>
1802a25af: 84 c0                       	testb	%al, %al
1802a25b1: c6 45 4c 01                 	movb	$0x1, 0x4c(%rbp)
1802a25b5: b9 02 00 00 00              	movl	$0x2, %ecx
1802a25ba: 0f 45 d1                    	cmovnel	%ecx, %edx
1802a25bd: 89 55 34                    	movl	%edx, 0x34(%rbp)
1802a25c0: 44 38 a3 e1 02 00 00        	cmpb	%r12b, 0x2e1(%rbx)
1802a25c7: 74 06                       	je	0x1802a25cf <.text+0x2a15cf>
1802a25c9: 44 89 65 50                 	movl	%r12d, 0x50(%rbp)
1802a25cd: eb 09                       	jmp	0x1802a25d8 <.text+0x2a15d8>
1802a25cf: 8b 83 e4 02 00 00           	movl	0x2e4(%rbx), %eax
1802a25d5: 89 45 50                    	movl	%eax, 0x50(%rbp)
1802a25d8: 48 8d 4c 24 20              	leaq	0x20(%rsp), %rcx
1802a25dd: c5 f8 77                    	vzeroupper
1802a25e0: ff 15 7a 06 1d 00           	callq	*0x1d067a(%rip)         # 0x180472c60 <SKSEPlugin_Version+0x118b0>
1802a25e6: 4c 8b a4 24 b8 01 00 00     	movq	0x1b8(%rsp), %r12
1802a25ee: 4c 8d 9c 24 90 01 00 00     	leaq	0x190(%rsp), %r11
