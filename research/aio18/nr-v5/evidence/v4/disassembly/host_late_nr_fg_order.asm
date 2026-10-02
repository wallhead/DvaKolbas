
/mnt/data/aio18_nr_work/SkyrimUpscaler.dll:	file format coff-x86-64

Disassembly of section .text:

0000000180001000 <.text>:
1802aaf80: 48 48                       	rex64
1802aaf82: 8b 0d 58 5e bd 00           	movl	0xbd5e58(%rip), %ecx    # 0x180e80de0 <SKSEPlugin_Version+0xa1fa30>
1802aaf88: 89 44 24 40                 	movl	%eax, 0x40(%rsp)
1802aaf8c: 48 8d 85 80 03 00 00        	leaq	0x380(%rbp), %rax
1802aaf93: 4c 89 54 24 38              	movq	%r10, 0x38(%rsp)
1802aaf98: 4c 89 6c 24 30              	movq	%r13, 0x30(%rsp)
1802aaf9d: 48 89 44 24 28              	movq	%rax, 0x28(%rsp)
1802aafa2: c7 44 24 20 01 00 00 00     	movl	$0x1, 0x20(%rsp)
1802aafaa: e8 91 62 ff ff              	callq	0x1802a1240 <.text+0x2a0240>
1802aafaf: e9 9a 03 00 00              	jmp	0x1802ab34e <.text+0x2aa34e>
1802aafb4: 48 81 c2 48 01 00 00        	addq	$0x148, %rdx            # imm = 0x148
1802aafbb: 74 2c                       	je	0x1802aafe9 <.text+0x2a9fe9>
1802aafbd: c4 e1 f9 7e d0              	vmovq	%xmm2, %rax
1802aafc2: 48 85 c0                    	testq	%rax, %rax
1802aafc5: 74 22                       	je	0x1802aafe9 <.text+0x2a9fe9>
1802aafc7: 48 8b 12                    	movq	(%rdx), %rdx
1802aafca: 48 85 d2                    	testq	%rdx, %rdx
1802aafcd: 74 1a                       	je	0x1802aafe9 <.text+0x2a9fe9>
1802aafcf: 4c 39 a9 80 16 00 00        	cmpq	%r13, 0x1680(%rcx)
1802aafd6: 74 11                       	je	0x1802aafe9 <.text+0x2a9fe9>
1802aafd8: 48 3b c2                    	cmpq	%rdx, %rax
1802aafdb: 74 0c                       	je	0x1802aafe9 <.text+0x2a9fe9>
1802aafdd: 4c 8d 45 b0                 	leaq	-0x50(%rbp), %r8
1802aafe1: c5 f8 77                    	vzeroupper
1802aafe4: e8 87 d7 fe ff              	callq	0x180298770 <.text+0x297770>
1802aafe9: c5 f8 77                    	vzeroupper
1802aafec: e8 9f be fe ff              	callq	0x180296e90 <.text+0x295e90>
1802aaff1: 48 8b 0d e8 5d bd 00        	movq	0xbd5de8(%rip), %rcx    # 0x180e80de0 <SKSEPlugin_Version+0xa1fa30>
1802aaff8: 83 b9 dc 03 00 00 02        	cmpl	$0x2, 0x3dc(%rcx)
1802aafff: 74 0d                       	je	0x1802ab00e <.text+0x2aa00e>
1802ab001: 44 38 a1 a5 04 00 00        	cmpb	%r12b, 0x4a5(%rcx)
1802ab008: 0f 84 f1 01 00 00           	je	0x1802ab1ff <.text+0x2aa1ff>
1802ab00e: 44 38 a1 d2 03 00 00        	cmpb	%r12b, 0x3d2(%rcx)
1802ab015: 0f 84 e4 01 00 00           	je	0x1802ab1ff <.text+0x2aa1ff>
1802ab01b: 44 38 a1 aa 02 00 00        	cmpb	%r12b, 0x2aa(%rcx)
1802ab022: 0f 85 d7 01 00 00           	jne	0x1802ab1ff <.text+0x2aa1ff>
1802ab028: 44 38 a1 a8 02 00 00        	cmpb	%r12b, 0x2a8(%rcx)
1802ab02f: 0f 84 ca 01 00 00           	je	0x1802ab1ff <.text+0x2aa1ff>
1802ab035: 44 38 a1 a9 02 00 00        	cmpb	%r12b, 0x2a9(%rcx)
1802ab03c: 0f 84 bd 01 00 00           	je	0x1802ab1ff <.text+0x2aa1ff>
1802ab042: 4c 39 a9 50 0b 00 00        	cmpq	%r13, 0xb50(%rcx)
1802ab049: 0f 84 b0 01 00 00           	je	0x1802ab1ff <.text+0x2aa1ff>
1802ab04f: 4c 39 a9 a8 0b 00 00        	cmpq	%r13, 0xba8(%rcx)
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
1802ab175: 48 8d 4d b0                 	leaq	-0x50(%rbp), %rcx
1802ab179: 48 8b 05 60 5c bd 00        	movq	0xbd5c60(%rip), %rax    # 0x180e80de0 <SKSEPlugin_Version+0xa1fa30>
1802ab180: 8b 98 74 02 00 00           	movl	0x274(%rax), %ebx
1802ab186: 8b b8 70 02 00 00           	movl	0x270(%rax), %edi
1802ab18c: e8 af 0f ec ff              	callq	0x18016c140 <.text+0x16b140>
1802ab191: 48 8b 0d 48 5c bd 00        	movq	0xbd5c48(%rip), %rcx    # 0x180e80de0 <SKSEPlugin_Version+0xa1fa30>
1802ab198: 41 b9 02 00 00 00           	movl	$0x2, %r9d
1802ab19e: 4c 89 6c 24 68              	movq	%r13, 0x68(%rsp)
1802ab1a3: 45 33 c0                    	xorl	%r8d, %r8d
1802ab1a6: 44 88 64 24 60              	movb	%r12b, 0x60(%rsp)
1802ab1ab: ba 01 00 00 00              	movl	$0x1, %edx
1802ab1b0: 44 89 6c 24 58              	movl	%r13d, 0x58(%rsp)
1802ab1b5: 44 89 6c 24 50              	movl	%r13d, 0x50(%rsp)
1802ab1ba: 89 5c 24 48                 	movl	%ebx, 0x48(%rsp)
1802ab1be: 89 7c 24 40                 	movl	%edi, 0x40(%rsp)
1802ab1c2: 48 89 44 24 38              	movq	%rax, 0x38(%rsp)
1802ab1c7: 48 8d 85 78 03 00 00        	leaq	0x378(%rbp), %rax
1802ab1ce: 4c 89 6c 24 30              	movq	%r13, 0x30(%rsp)
1802ab1d3: 48 89 44 24 28              	movq	%rax, 0x28(%rsp)
1802ab1d8: c7 44 24 20 01 00 00 00     	movl	$0x1, 0x20(%rsp)
1802ab1e0: e8 5b 60 ff ff              	callq	0x1802a1240 <.text+0x2a0240>
1802ab1e5: 48 8b 0d f4 5b bd 00        	movq	0xbd5bf4(%rip), %rcx    # 0x180e80de0 <SKSEPlugin_Version+0xa1fa30>
1802ab1ec: 41 b4 01                    	movb	$0x1, %r12b
1802ab1ef: c6 81 8c 04 00 00 01        	movb	$0x1, 0x48c(%rcx)
1802ab1f6: eb 07                       	jmp	0x1802ab1ff <.text+0x2aa1ff>
1802ab1f8: 48 8b 0d e1 5b bd 00        	movq	0xbd5be1(%rip), %rcx    # 0x180e80de0 <SKSEPlugin_Version+0xa1fa30>
1802ab1ff: 44 38 a9 d3 03 00 00        	cmpb	%r13b, 0x3d3(%rcx)
1802ab206: 0f 84 b3 00 00 00           	je	0x1802ab2bf <.text+0x2aa2bf>
1802ab20c: 44 38 a9 d2 03 00 00        	cmpb	%r13b, 0x3d2(%rcx)
1802ab213: 0f 84 a6 00 00 00           	je	0x1802ab2bf <.text+0x2aa2bf>
1802ab219: 45 84 e4                    	testb	%r12b, %r12b
1802ab21c: 0f 85 9d 00 00 00           	jne	0x1802ab2bf <.text+0x2aa2bf>
1802ab222: 48 81 c1 88 09 00 00        	addq	$0x988, %rcx            # imm = 0x988
1802ab229: e8 62 10 ec ff              	callq	0x18016c290 <.text+0x16b290>
1802ab22e: 48 89 85 80 03 00 00        	movq	%rax, 0x380(%rbp)
1802ab235: 48 85 c0                    	testq	%rax, %rax
1802ab238: 0f 84 81 00 00 00           	je	0x1802ab2bf <.text+0x2aa2bf>
1802ab23e: 48 8d 4d b0                 	leaq	-0x50(%rbp), %rcx
1802ab242: e8 f9 0e ec ff              	callq	0x18016c140 <.text+0x16b140>
1802ab247: 48 85 c0                    	testq	%rax, %rax
1802ab24a: 74 73                       	je	0x1802ab2bf <.text+0x2aa2bf>
1802ab24c: 48 8b 05 8d 5b bd 00        	movq	0xbd5b8d(%rip), %rax    # 0x180e80de0 <SKSEPlugin_Version+0xa1fa30>
1802ab253: 48 8d 4d b0                 	leaq	-0x50(%rbp), %rcx
1802ab257: 8b 98 74 02 00 00           	movl	0x274(%rax), %ebx
1802ab25d: 8b b8 70 02 00 00           	movl	0x270(%rax), %edi
1802ab263: e8 d8 0e ec ff              	callq	0x18016c140 <.text+0x16b140>
1802ab268: 48 8b 0d 71 5b bd 00        	movq	0xbd5b71(%rip), %rcx    # 0x180e80de0 <SKSEPlugin_Version+0xa1fa30>
1802ab26f: 41 b9 02 00 00 00           	movl	$0x2, %r9d
1802ab275: 4c 89 6c 24 68              	movq	%r13, 0x68(%rsp)
1802ab27a: 45 33 c0                    	xorl	%r8d, %r8d
1802ab27d: 44 88 6c 24 60              	movb	%r13b, 0x60(%rsp)
1802ab282: ba 01 00 00 00              	movl	$0x1, %edx
1802ab287: 44 89 6c 24 58              	movl	%r13d, 0x58(%rsp)
1802ab28c: 44 89 6c 24 50              	movl	%r13d, 0x50(%rsp)
1802ab291: 89 5c 24 48                 	movl	%ebx, 0x48(%rsp)
1802ab295: 89 7c 24 40                 	movl	%edi, 0x40(%rsp)
1802ab299: 48 89 44 24 38              	movq	%rax, 0x38(%rsp)
1802ab29e: 48 8d 85 80 03 00 00        	leaq	0x380(%rbp), %rax
1802ab2a5: 4c 89 6c 24 30              	movq	%r13, 0x30(%rsp)
1802ab2aa: 48 89 44 24 28              	movq	%rax, 0x28(%rsp)
1802ab2af: c7 44 24 20 01 00 00 00     	movl	$0x1, 0x20(%rsp)
1802ab2b7: e8 84 5f ff ff              	callq	0x1802a1240 <.text+0x2a0240>
1802ab2bc: 41 b4 01                    	movb	$0x1, %r12b
1802ab2bf: 48 8d 4d b0                 	leaq	-0x50(%rbp), %rcx
1802ab2c3: e8 78 0e ec ff              	callq	0x18016c140 <.text+0x16b140>
1802ab2c8: 48 89 85 88 03 00 00        	movq	%rax, 0x388(%rbp)
1802ab2cf: 48 85 c0                    	testq	%rax, %rax
1802ab2d2: 74 26                       	je	0x1802ab2fa <.text+0x2aa2fa>
1802ab2d4: 48 8b 05 05 5b bd 00        	movq	0xbd5b05(%rip), %rax    # 0x180e80de0 <SKSEPlugin_Version+0xa1fa30>
1802ab2db: 4c 8d 85 88 03 00 00        	leaq	0x388(%rbp), %r8
1802ab2e2: 45 33 c9                    	xorl	%r9d, %r9d
1802ab2e5: ba 01 00 00 00              	movl	$0x1, %edx
1802ab2ea: 48 8b 88 80 16 00 00        	movq	0x1680(%rax), %rcx
1802ab2f1: 48 8b 01                    	movq	(%rcx), %rax
1802ab2f4: ff 90 08 01 00 00           	callq	*0x108(%rax)
1802ab2fa: e8 51 01 fe ff              	callq	0x18028b450 <.text+0x28a450>
1802ab2ff: 48 8b 05 da 5a bd 00        	movq	0xbd5ada(%rip), %rax    # 0x180e80de0 <SKSEPlugin_Version+0xa1fa30>
1802ab306: 44 38 a8 d2 03 00 00        	cmpb	%r13b, 0x3d2(%rax)
1802ab30d: 74 1b                       	je	0x1802ab32a <.text+0x2aa32a>
1802ab30f: 48 8d 88 88 09 00 00        	leaq	0x988(%rax), %rcx
1802ab316: e8 25 0e ec ff              	callq	0x18016c140 <.text+0x16b140>
1802ab31b: 48 8b d0                    	movq	%rax, %rdx
1802ab31e: e8 dd a1 fd ff              	callq	0x180285500 <.text+0x284500>
1802ab323: 48 8b 05 b6 5a bd 00        	movq	0xbd5ab6(%rip), %rax    # 0x180e80de0 <SKSEPlugin_Version+0xa1fa30>
1802ab32a: 44 38 a8 e4 04 00 00        	cmpb	%r13b, 0x4e4(%rax)
1802ab331: 75 1b                       	jne	0x1802ab34e <.text+0x2aa34e>
1802ab333: 83 b8 dc 03 00 00 02        	cmpl	$0x2, 0x3dc(%rax)
1802ab33a: 74 12                       	je	0x1802ab34e <.text+0x2aa34e>
1802ab33c: 44 38 a8 a5 04 00 00        	cmpb	%r13b, 0x4a5(%rax)
1802ab343: 75 09                       	jne	0x1802ab34e <.text+0x2aa34e>
1802ab345: 48 8d 55 b0                 	leaq	-0x50(%rbp), %rdx
1802ab349: e8 52 8e fe ff              	callq	0x1802941a0 <.text+0x2931a0>
1802ab34e: c5 f8 77                    	vzeroupper
1802ab351: e8 aa 7b ea ff              	callq	0x180152f00 <.text+0x151f00>
1802ab356: 44 38 68 11                 	cmpb	%r13b, 0x11(%rax)
1802ab35a: 75 3d                       	jne	0x1802ab399 <.text+0x2aa399>
1802ab35c: 44 38 68 12                 	cmpb	%r13b, 0x12(%rax)
1802ab360: 75 37                       	jne	0x1802ab399 <.text+0x2aa399>
1802ab362: e8 69 10 f0 ff              	callq	0x1801ac3d0 <.text+0x1ab3d0>
1802ab367: 48 8b d8                    	movq	%rax, %rbx
1802ab36a: 48 85 c0                    	testq	%rax, %rax
1802ab36d: 74 26                       	je	0x1802ab395 <.text+0x2aa395>
1802ab36f: 48 8d 15 c2 a3 13 00        	leaq	0x13a3c2(%rip), %rdx    # 0x1803e5738
1802ab376: 48 8b c8                    	movq	%rax, %rcx
1802ab379: e8 02 11 f0 ff              	callq	0x1801ac480 <.text+0x1ab480>
1802ab37e: 84 c0                       	testb	%al, %al
1802ab380: 75 17                       	jne	0x1802ab399 <.text+0x2aa399>
1802ab382: 48 8d 15 8f a3 13 00        	leaq	0x13a38f(%rip), %rdx    # 0x1803e5718
1802ab389: 48 8b cb                    	movq	%rbx, %rcx
1802ab38c: e8 ef 10 f0 ff              	callq	0x1801ac480 <.text+0x1ab480>
