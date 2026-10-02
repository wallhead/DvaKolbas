
/mnt/data/aio18_nr_work/SkyrimUpscaler.dll:	file format coff-x86-64

Disassembly of section .text:

0000000180001000 <.text>:
1802abef0: 85 c9                       	testl	%ecx, %ecx
1802abef2: 74 21                       	je	0x1802abf15 <.text+0x2aaf15>
1802abef4: 48 85 d2                    	testq	%rdx, %rdx
1802abef7: 74 1c                       	je	0x1802abf15 <.text+0x2aaf15>
1802abef9: 4c 39 a8 80 16 00 00        	cmpq	%r13, 0x1680(%rax)
1802abf00: 74 13                       	je	0x1802abf15 <.text+0x2aaf15>
1802abf02: 4c 8d 45 b0                 	leaq	-0x50(%rbp), %r8
1802abf06: 48 8b c8                    	movq	%rax, %rcx
1802abf09: e8 62 c8 fe ff              	callq	0x180298770 <.text+0x297770>
1802abf0e: 48 8b 05 cb 4e bd 00        	movq	0xbd4ecb(%rip), %rax    # 0x180e80de0 <SKSEPlugin_Version+0xa1fa30>
1802abf15: 44 38 a8 aa 02 00 00        	cmpb	%r13b, 0x2aa(%rax)
1802abf1c: 75 37                       	jne	0x1802abf55 <.text+0x2aaf55>
1802abf1e: 44 38 a8 e4 04 00 00        	cmpb	%r13b, 0x4e4(%rax)
1802abf25: 74 2e                       	je	0x1802abf55 <.text+0x2aaf55>
1802abf27: 44 38 a8 a5 04 00 00        	cmpb	%r13b, 0x4a5(%rax)
1802abf2e: 74 25                       	je	0x1802abf55 <.text+0x2aaf55>
1802abf30: 8b 05 ba 35 1b 00           	movl	0x1b35ba(%rip), %eax    # 0x18045f4f0
1802abf36: 25 b8 00 00 00              	andl	$0xb8, %eax
1802abf3b: 3c b8                       	cmpb	$-0x48, %al
1802abf3d: 75 16                       	jne	0x1802abf55 <.text+0x2aaf55>
1802abf3f: 48 8b 0d 9a 4e bd 00        	movq	0xbd4e9a(%rip), %rcx    # 0x180e80de0 <SKSEPlugin_Version+0xa1fa30>
1802abf46: 48 8d 55 b0                 	leaq	-0x50(%rbp), %rdx
1802abf4a: 45 33 c9                    	xorl	%r9d, %r9d
1802abf4d: 41 b0 01                    	movb	$0x1, %r8b
1802abf50: e8 ab 63 ff ff              	callq	0x1802a2300 <.text+0x2a1300>
1802abf55: ff 15 3d 6d 1c 00           	callq	*0x1c6d3d(%rip)         # 0x180472c98 <SKSEPlugin_Version+0x118e8>
