
/mnt/data/aio18_nr_work/SkyrimUpscaler.dll:	file format coff-x86-64

Disassembly of section .text:

0000000180001000 <.text>:
1802ab050: 39 a9 a8 0b 00 00           	cmpl	%ebp, 0xba8(%rcx)
1802ab056: 0f 84 a3 01 00 00           	je	0x1802ab1ff <.text+0x2aa1ff>
1802ab05c: 48 81 c1 38 0a 00 00        	addq	$0xa38, %rcx            # imm = 0xA38
1802ab063: e8 28 12 ec ff              	callq	0x18016c290 <.text+0x16b290>
1802ab068: 48 85 c0                    	testq	%rax, %rax
1802ab06b: 0f 84 87 01 00 00           	je	0x1802ab1f8 <.text+0x2aa1f8>
1802ab071: 48 8b 0d 68 5d bd 00        	movq	0xbd5d68(%rip), %rcx    # 0x180e80de0 <SKSEPlugin_Version+0xa1fa30>
1802ab078: 48 81 c1 88 09 00 00        	addq	$0x988, %rcx            # imm = 0x988
1802ab07f: e8 0c 12 ec ff              	callq	0x18016c290 <.text+0x16b290>
1802ab084: 48 85 c0                    	testq	%rax, %rax
1802ab087: 0f 84 6b 01 00 00           	je	0x1802ab1f8 <.text+0x2aa1f8>
1802ab08d: 48 8d 4d b0                 	leaq	-0x50(%rbp), %rcx
1802ab091: e8 aa 10 ec ff              	callq	0x18016c140 <.text+0x16b140>
1802ab096: 48 85 c0                    	testq	%rax, %rax
1802ab099: 0f 84 59 01 00 00           	je	0x1802ab1f8 <.text+0x2aa1f8>
1802ab09f: 48 8b 0d 3a 5d bd 00        	movq	0xbd5d3a(%rip), %rcx    # 0x180e80de0 <SKSEPlugin_Version+0xa1fa30>
1802ab0a6: 8b 05 34 60 1b 00           	movl	0x1b6034(%rip), %eax    # 0x1804610e0
1802ab0ac: 39 81 60 04 00 00           	cmpl	%eax, 0x460(%rcx)
1802ab0b2: 75 0d                       	jne	0x1802ab0c1 <.text+0x2aa0c1>
1802ab0b4: 44 38 25 89 ac 1c 00        	cmpb	%r12b, 0x1cac89(%rip)   # 0x180475d44 <SKSEPlugin_Version+0x14994>
1802ab0bb: 0f 85 3e 01 00 00           	jne	0x1802ab1ff <.text+0x2aa1ff>
1802ab0c1: 41 b1 01                    	movb	$0x1, %r9b
1802ab0c4: 48 8d 91 38 0a 00 00        	leaq	0xa38(%rcx), %rdx
1802ab0cb: 45 0f b6 c1                 	movzbl	%r9b, %r8d
1802ab0cf: e8 2c 72 ff ff              	callq	0x1802a2300 <.text+0x2a1300>
1802ab0d4: 48 8b 0d 05 5d bd 00        	movq	0xbd5d05(%rip), %rcx    # 0x180e80de0 <SKSEPlugin_Version+0xa1fa30>
1802ab0db: 48 81 c1 38 0a 00 00        	addq	$0xa38, %rcx            # imm = 0xA38
1802ab0e2: e8 a9 11 ec ff              	callq	0x18016c290 <.text+0x16b290>
1802ab0e7: 48 89 85 70 03 00 00        	movq	%rax, 0x370(%rbp)
1802ab0ee: 48 8d 4d b0                 	leaq	-0x50(%rbp), %rcx
1802ab0f2: 48 8b 05 e7 5c bd 00        	movq	0xbd5ce7(%rip), %rax    # 0x180e80de0 <SKSEPlugin_Version+0xa1fa30>
1802ab0f9: 8b 98 74 02 00 00           	movl	0x274(%rax), %ebx
1802ab0ff: 8b b8 70 02 00 00           	movl	0x270(%rax), %edi
1802ab105: e8 36 10 ec ff              	callq	0x18016c140 <.text+0x16b140>
1802ab10a: 48 8b 0d cf 5c bd 00        	movq	0xbd5ccf(%rip), %rcx    # 0x180e80de0 <SKSEPlugin_Version+0xa1fa30>
1802ab111: 45 33 c9                    	xorl	%r9d, %r9d
1802ab114: 4c 89 6c 24 68              	movq	%r13, 0x68(%rsp)
1802ab119: 45 33 c0                    	xorl	%r8d, %r8d
1802ab11c: 44 88 64 24 60              	movb	%r12b, 0x60(%rsp)
1802ab121: ba 01 00 00 00              	movl	$0x1, %edx
1802ab126: 44 89 6c 24 58              	movl	%r13d, 0x58(%rsp)
1802ab12b: 44 89 6c 24 50              	movl	%r13d, 0x50(%rsp)
1802ab130: 89 5c 24 48                 	movl	%ebx, 0x48(%rsp)
1802ab134: 89 7c 24 40                 	movl	%edi, 0x40(%rsp)
1802ab138: 48 89 44 24 38              	movq	%rax, 0x38(%rsp)
1802ab13d: 48 8d 85 70 03 00 00        	leaq	0x370(%rbp), %rax
1802ab144: 4c 89 6c 24 30              	movq	%r13, 0x30(%rsp)
1802ab149: 48 89 44 24 28              	movq	%rax, 0x28(%rsp)
1802ab14e: c7 44 24 20 01 00 00 00     	movl	$0x1, 0x20(%rsp)
1802ab156: e8 e5 60 ff ff              	callq	0x1802a1240 <.text+0x2a0240>
1802ab15b: 48 8b 0d 7e 5c bd 00        	movq	0xbd5c7e(%rip), %rcx    # 0x180e80de0 <SKSEPlugin_Version+0xa1fa30>
1802ab162: 48 81 c1 88 09 00 00        	addq	$0x988, %rcx            # imm = 0x988
1802ab169: e8 22 11 ec ff              	callq	0x18016c290 <.text+0x16b290>
1802ab16e: 48 89 85 78 03 00 00        	movq	%rax, 0x378(%rbp)
