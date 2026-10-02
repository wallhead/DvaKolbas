
/mnt/data/aio18_nr_work/SkyrimUpscaler.dll:	file format coff-x86-64

Disassembly of section .text:

0000000180001000 <.text>:
1802a2300: 48 89 5c 24 18              	movq	%rbx, 0x18(%rsp)
1802a2305: 48 89 7c 24 20              	movq	%rdi, 0x20(%rsp)
1802a230a: 55                          	pushq	%rbp
1802a230b: 41 56                       	pushq	%r14
1802a230d: 41 57                       	pushq	%r15
1802a230f: 48 8d ac 24 70 ff ff ff     	leaq	-0x90(%rsp), %rbp
1802a2317: 48 81 ec 90 01 00 00        	subq	$0x190, %rsp            # imm = 0x190
1802a231e: 8b 05 bc ed 1b 00           	movl	0x1bedbc(%rip), %eax    # 0x1804610e0
1802a2324: 45 0f b6 f9                 	movzbl	%r9b, %r15d
1802a2328: 45 0f b6 f0                 	movzbl	%r8b, %r14d
1802a232c: 48 8b fa                    	movq	%rdx, %rdi
1802a232f: 48 8b d9                    	movq	%rcx, %rbx
1802a2332: 39 81 60 04 00 00           	cmpl	%eax, 0x460(%rcx)
1802a2338: 75 0d                       	jne	0x1802a2347 <.text+0x2a1347>
1802a233a: 80 3d 03 3a 1d 00 00        	cmpb	$0x0, 0x1d3a03(%rip)    # 0x180475d44 <SKSEPlugin_Version+0x14994>
1802a2341: 0f 85 a7 02 00 00           	jne	0x1802a25ee <.text+0x2a15ee>
1802a2347: e8 04 42 ff ff              	callq	0x180296550 <.text+0x295550>
1802a234c: 84 c0                       	testb	%al, %al
1802a234e: 0f 84 9a 02 00 00           	je	0x1802a25ee <.text+0x2a15ee>
1802a2354: 80 bb a8 02 00 00 00        	cmpb	$0x0, 0x2a8(%rbx)
1802a235b: 0f 84 8d 02 00 00           	je	0x1802a25ee <.text+0x2a15ee>
1802a2361: 80 bb a9 02 00 00 00        	cmpb	$0x0, 0x2a9(%rbx)
1802a2368: 0f 84 80 02 00 00           	je	0x1802a25ee <.text+0x2a15ee>
1802a236e: 48 85 ff                    	testq	%rdi, %rdi
1802a2371: 0f 84 77 02 00 00           	je	0x1802a25ee <.text+0x2a15ee>
1802a2377: 48 8b 07                    	movq	(%rdi), %rax
1802a237a: 48 85 c0                    	testq	%rax, %rax
1802a237d: 0f 84 6b 02 00 00           	je	0x1802a25ee <.text+0x2a15ee>
1802a2383: 48 8b 8b 50 0b 00 00        	movq	0xb50(%rbx), %rcx
1802a238a: 48 85 c9                    	testq	%rcx, %rcx
1802a238d: 0f 84 5b 02 00 00           	je	0x1802a25ee <.text+0x2a15ee>
1802a2393: 48 8b 93 a8 0b 00 00        	movq	0xba8(%rbx), %rdx
1802a239a: 48 85 d2                    	testq	%rdx, %rdx
1802a239d: 0f 84 4b 02 00 00           	je	0x1802a25ee <.text+0x2a15ee>
1802a23a3: c5 f9 ef c0                 	vpxor	%xmm0, %xmm0, %xmm0
1802a23a7: c5 f1 ef c9                 	vpxor	%xmm1, %xmm1, %xmm1
1802a23ab: 48 89 b4 24 b0 01 00 00     	movq	%rsi, 0x1b0(%rsp)
1802a23b3: 4c 89 a4 24 b8 01 00 00     	movq	%r12, 0x1b8(%rsp)
1802a23bb: 45 33 e4                    	xorl	%r12d, %r12d
1802a23be: 4c 89 64 24 48              	movq	%r12, 0x48(%rsp)
1802a23c3: 41 8b f4                    	movl	%r12d, %esi
1802a23c6: 44 89 64 24 20              	movl	%r12d, 0x20(%rsp)
1802a23cb: 48 89 44 24 28              	movq	%rax, 0x28(%rsp)
1802a23d0: 48 89 54 24 30              	movq	%rdx, 0x30(%rsp)
1802a23d5: 48 89 4c 24 38              	movq	%rcx, 0x38(%rsp)
1802a23da: 48 89 44 24 40              	movq	%rax, 0x40(%rsp)
1802a23df: c5 fa 7f 44 24 60           	vmovdqu	%xmm0, 0x60(%rsp)
1802a23e5: c5 f8 11 4c 24 70           	vmovups	%xmm1, 0x70(%rsp)
1802a23eb: c5 f8 11 45 80              	vmovups	%xmm0, -0x80(%rbp)
1802a23f0: c5 f8 11 4d 90              	vmovups	%xmm1, -0x70(%rbp)
1802a23f5: c5 f8 11 45 a0              	vmovups	%xmm0, -0x60(%rbp)
1802a23fa: c5 f8 11 4d b0              	vmovups	%xmm1, -0x50(%rbp)
1802a23ff: c5 f8 11 45 c0              	vmovups	%xmm0, -0x40(%rbp)
1802a2404: c5 f8 11 4d d0              	vmovups	%xmm1, -0x30(%rbp)
1802a2409: c5 f8 11 45 e0              	vmovups	%xmm0, -0x20(%rbp)
1802a240e: c5 f8 11 4d f0              	vmovups	%xmm1, -0x10(%rbp)
1802a2413: 45 84 ff                    	testb	%r15b, %r15b
1802a2416: 75 6a                       	jne	0x1802a2482 <.text+0x2a1482>
1802a2418: 48 8b cb                    	movq	%rbx, %rcx
1802a241b: e8 f0 ed ff ff              	callq	0x1802a1210 <.text+0x2a0210>
1802a2420: 84 c0                       	testb	%al, %al
1802a2422: 75 28                       	jne	0x1802a244c <.text+0x2a144c>
1802a2424: 45 84 f6                    	testb	%r14b, %r14b
1802a2427: 75 23                       	jne	0x1802a244c <.text+0x2a144c>
1802a2429: e8 22 1c ff ff              	callq	0x180294050 <.text+0x293050>
1802a242e: 84 c0                       	testb	%al, %al
1802a2430: 74 1a                       	je	0x1802a244c <.text+0x2a144c>
1802a2432: 40 38 b3 d2 03 00 00        	cmpb	%sil, 0x3d2(%rbx)
1802a2439: 74 11                       	je	0x1802a244c <.text+0x2a144c>
1802a243b: 48 8b 8b 88 09 00 00        	movq	0x988(%rbx), %rcx
1802a2442: 48 85 c9                    	testq	%rcx, %rcx
1802a2445: 74 41                       	je	0x1802a2488 <.text+0x2a1488>
1802a2447: 48 8b 37                    	movq	(%rdi), %rsi
1802a244a: eb 3c                       	jmp	0x1802a2488 <.text+0x2a1488>
1802a244c: 48 8b cb                    	movq	%rbx, %rcx
1802a244f: e8 bc ed ff ff              	callq	0x1802a1210 <.text+0x2a0210>
1802a2454: 84 c0                       	testb	%al, %al
1802a2456: 75 2a                       	jne	0x1802a2482 <.text+0x2a1482>
1802a2458: e8 33 1c ff ff              	callq	0x180294090 <.text+0x293090>
1802a245d: 84 c0                       	testb	%al, %al
1802a245f: 74 21                       	je	0x1802a2482 <.text+0x2a1482>
1802a2461: 48 8b b3 38 0a 00 00        	movq	0xa38(%rbx), %rsi
1802a2468: 48 85 f6                    	testq	%rsi, %rsi
1802a246b: 74 15                       	je	0x1802a2482 <.text+0x2a1482>
1802a246d: 48 8b 8b 88 09 00 00        	movq	0x988(%rbx), %rcx
1802a2474: 48 85 c9                    	testq	%rcx, %rcx
1802a2477: 74 09                       	je	0x1802a2482 <.text+0x2a1482>
1802a2479: 44 38 a3 d2 03 00 00        	cmpb	%r12b, 0x3d2(%rbx)
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
1802a25f6: 49 8b 5b 30                 	movq	0x30(%r11), %rbx
1802a25fa: 49 8b 7b 38                 	movq	0x38(%r11), %rdi
1802a25fe: 49 8b e3                    	movq	%r11, %rsp
1802a2601: 41 5f                       	popq	%r15
1802a2603: 41 5e                       	popq	%r14
1802a2605: 5d                          	popq	%rbp
1802a2606: c3                          	retq
