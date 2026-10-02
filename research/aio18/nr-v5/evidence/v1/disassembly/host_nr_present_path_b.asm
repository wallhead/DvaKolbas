
/mnt/data/aio18_nr_work/SkyrimUpscaler.dll:	file format coff-x86-64

Disassembly of section .text:

0000000180001000 <.text>:
1802ab560: 48 8b 05 79 58 bd 00        	movq	0xbd5879(%rip), %rax    # 0x180e80de0 <SKSEPlugin_Version+0xa1fa30>
1802ab567: c6 80 d2 03 00 00 01        	movb	$0x1, 0x3d2(%rax)
1802ab56e: 44 38 a8 aa 02 00 00        	cmpb	%r13b, 0x2aa(%rax)
1802ab575: 75 61                       	jne	0x1802ab5d8 <.text+0x2aa5d8>
1802ab577: 44 38 a8 e4 04 00 00        	cmpb	%r13b, 0x4e4(%rax)
1802ab57e: 74 1f                       	je	0x1802ab59f <.text+0x2aa59f>
1802ab580: 44 38 a8 a5 04 00 00        	cmpb	%r13b, 0x4a5(%rax)
1802ab587: 74 16                       	je	0x1802ab59f <.text+0x2aa59f>
1802ab589: 8b 05 61 3f 1b 00           	movl	0x1b3f61(%rip), %eax    # 0x18045f4f0
1802ab58f: 25 b8 00 00 00              	andl	$0xb8, %eax
1802ab594: 3c b8                       	cmpb	$-0x48, %al
1802ab596: 48 8b 05 43 58 bd 00        	movq	0xbd5843(%rip), %rax    # 0x180e80de0 <SKSEPlugin_Version+0xa1fa30>
1802ab59d: 74 39                       	je	0x1802ab5d8 <.text+0x2aa5d8>
1802ab59f: 83 b8 dc 03 00 00 02        	cmpl	$0x2, 0x3dc(%rax)
1802ab5a6: 74 0e                       	je	0x1802ab5b6 <.text+0x2aa5b6>
1802ab5a8: 44 38 a8 a5 04 00 00        	cmpb	%r13b, 0x4a5(%rax)
1802ab5af: 75 05                       	jne	0x1802ab5b6 <.text+0x2aa5b6>
1802ab5b1: 45 33 c0                    	xorl	%r8d, %r8d
1802ab5b4: eb 0c                       	jmp	0x1802ab5c2 <.text+0x2aa5c2>
1802ab5b6: 44 38 a8 8c 04 00 00        	cmpb	%r13b, 0x48c(%rax)
1802ab5bd: 75 19                       	jne	0x1802ab5d8 <.text+0x2aa5d8>
1802ab5bf: 41 b0 01                    	movb	$0x1, %r8b
1802ab5c2: 45 33 c9                    	xorl	%r9d, %r9d
1802ab5c5: 48 8d 55 b0                 	leaq	-0x50(%rbp), %rdx
1802ab5c9: 48 8b c8                    	movq	%rax, %rcx
1802ab5cc: e8 2f 6d ff ff              	callq	0x1802a2300 <.text+0x2a1300>
1802ab5d1: 48 8b 05 08 58 bd 00        	movq	0xbd5808(%rip), %rax    # 0x180e80de0 <SKSEPlugin_Version+0xa1fa30>
1802ab5d8: 44 38 a8 43 03 00 00        	cmpb	%r13b, 0x343(%rax)
1802ab5df: 75 4c                       	jne	0x1802ab62d <.text+0x2aa62d>
1802ab5e1: 40 84 ff                    	testb	%dil, %dil
1802ab5e4: 75 47                       	jne	0x1802ab62d <.text+0x2aa62d>
1802ab5e6: 45 84 f6                    	testb	%r14b, %r14b
1802ab5e9: 75 42                       	jne	0x1802ab62d <.text+0x2aa62d>
1802ab5eb: 48 8d 4d b0                 	leaq	-0x50(%rbp), %rcx
1802ab5ef: 44 88 a8 d2 03 00 00        	movb	%r13b, 0x3d2(%rax)
1802ab5f6: e8 45 0b ec ff              	callq	0x18016c140 <.text+0x16b140>
1802ab5fb: 48 89 85 70 03 00 00        	movq	%rax, 0x370(%rbp)
1802ab602: 4c 8d 85 70 03 00 00        	leaq	0x370(%rbp), %r8
1802ab609: 48 8b 05 d0 57 bd 00        	movq	0xbd57d0(%rip), %rax    # 0x180e80de0 <SKSEPlugin_Version+0xa1fa30>
1802ab610: 45 33 c9                    	xorl	%r9d, %r9d
1802ab613: ba 01 00 00 00              	movl	$0x1, %edx
1802ab618: 48 8b 88 80 16 00 00        	movq	0x1680(%rax), %rcx
1802ab61f: 48 8b 01                    	movq	(%rcx), %rax
1802ab622: ff 90 08 01 00 00           	callq	*0x108(%rax)
1802ab628: e8 23 fe fd ff              	callq	0x18028b450 <.text+0x28a450>
1802ab62d: e8 ce 78 ea ff              	callq	0x180152f00 <.text+0x151f00>
