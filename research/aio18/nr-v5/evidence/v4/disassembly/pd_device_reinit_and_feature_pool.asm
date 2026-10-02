
/mnt/data/aio18_nr_work/PDPerfPlugin.dll:	file format coff-x86-64

Disassembly of section .text:

0000000180078890 <SetPDFrameWarpNativeCameraSource>:
1800a1830: 40 53                       	pushq	%rbx
1800a1832: 48 83 ec 50                 	subq	$0x50, %rsp
1800a1836: 0f 10 81 dc 00 00 00        	movups	0xdc(%rcx), %xmm0
1800a183d: 8b 81 fc 00 00 00           	movl	0xfc(%rcx), %eax
1800a1843: 48 8b d9                    	movq	%rcx, %rbx
1800a1846: 0f 10 89 ec 00 00 00        	movups	0xec(%rcx), %xmm1
1800a184d: 89 44 24 40                 	movl	%eax, 0x40(%rsp)
1800a1851: 0f 11 44 24 20              	movups	%xmm0, 0x20(%rsp)
1800a1856: 0f 11 4c 24 30              	movups	%xmm1, 0x30(%rsp)
1800a185b: e8 e0 05 00 00              	callq	0x1800a1e40 <SetPDFrameWarpNativeCameraSource+0x295b0>
1800a1860: 33 c0                       	xorl	%eax, %eax
1800a1862: 48 8d 54 24 20              	leaq	0x20(%rsp), %rdx
1800a1867: 48 8b cb                    	movq	%rbx, %rcx
1800a186a: 48 89 43 08                 	movq	%rax, 0x8(%rbx)
1800a186e: 48 89 43 10                 	movq	%rax, 0x10(%rbx)
1800a1872: 88 83 b4 01 00 00           	movb	%al, 0x1b4(%rbx)
1800a1878: e8 23 b1 ff ff              	callq	0x18009c9a0 <SetPDFrameWarpNativeCameraSource+0x24110>
1800a187d: 0f b6 d8                    	movzbl	%al, %ebx
1800a1880: 48 8d 15 21 19 10 01        	leaq	0x1101921(%rip), %rdx   # 0x1811a31a8
1800a1887: 84 db                       	testb	%bl, %bl
1800a1889: 48 8d 05 d8 33 10 01        	leaq	0x11033d8(%rip), %rax   # 0x1811a4c68
1800a1890: 48 8d 0d a9 33 10 01        	leaq	0x11033a9(%rip), %rcx   # 0x1811a4c40
1800a1897: 48 0f 45 d0                 	cmovneq	%rax, %rdx
1800a189b: e8 a0 a2 05 00              	callq	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
1800a18a0: 0f b6 c3                    	movzbl	%bl, %eax
1800a18a3: 48 83 c4 50                 	addq	$0x50, %rsp
1800a18a7: 5b                          	popq	%rbx
1800a18a8: c3                          	retq
1800a18a9: cc                          	int3
1800a18aa: cc                          	int3
1800a18ab: cc                          	int3
1800a18ac: cc                          	int3
1800a18ad: cc                          	int3
1800a18ae: cc                          	int3
1800a18af: cc                          	int3
1800a18b0: 83 fa 10                    	cmpl	$0x10, %edx
1800a18b3: 0f 83 98 00 00 00           	jae	0x1800a1951 <SetPDFrameWarpNativeCameraSource+0x290c1>
1800a18b9: 48 89 5c 24 18              	movq	%rbx, 0x18(%rsp)
1800a18be: 57                          	pushq	%rdi
1800a18bf: 48 83 ec 20                 	subq	$0x20, %rsp
1800a18c3: 8b da                       	movl	%edx, %ebx
1800a18c5: 48 8b f9                    	movq	%rcx, %rdi
1800a18c8: 48 89 74 24 38              	movq	%rsi, 0x38(%rsp)
1800a18cd: 48 8b 4c d9 48              	movq	0x48(%rcx,%rbx,8), %rcx
1800a18d2: 48 85 c9                    	testq	%rcx, %rcx
1800a18d5: 74 6b                       	je	0x1800a1942 <SetPDFrameWarpNativeCameraSource+0x290b2>
1800a18d7: 48 89 6c 24 30              	movq	%rbp, 0x30(%rsp)
1800a18dc: e8 7f 28 00 00              	callq	0x1800a4160 <SetPDFrameWarpNativeCameraSource+0x2b8d0>
1800a18e1: 8b c8                       	movl	%eax, %ecx
1800a18e3: 8b e8                       	movl	%eax, %ebp
1800a18e5: 81 e1 00 00 f0 ff           	andl	$0xfff00000, %ecx       # imm = 0xFFF00000
1800a18eb: 81 f9 00 00 d0 ba           	cmpl	$0xbad00000, %ecx       # imm = 0xBAD00000
1800a18f1: 75 2a                       	jne	0x1800a191d <SetPDFrameWarpNativeCameraSource+0x2908d>
1800a18f3: 8b c8                       	movl	%eax, %ecx
1800a18f5: e8 c6 18 06 00              	callq	0x1801031c0 <NVSDK_NGX_D3D12_Shutdown1+0x170>
1800a18fa: 4c 8b c8                    	movq	%rax, %r9
1800a18fd: 8d 53 01                    	leal	0x1(%rbx), %edx
1800a1900: 44 8b c5                    	movl	%ebp, %r8d
1800a1903: 48 8d 0d 8e 33 10 01        	leaq	0x110338e(%rip), %rcx   # 0x1811a4c98
1800a190a: e8 31 a2 05 00              	callq	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
1800a190f: 48 8d 0d 5a 33 10 01        	leaq	0x110335a(%rip), %rcx   # 0x1811a4c70
1800a1916: e8 d5 a9 ff ff              	callq	0x18009c2f0 <SetPDFrameWarpNativeCameraSource+0x23a60>
1800a191b: eb 0f                       	jmp	0x1800a192c <SetPDFrameWarpNativeCameraSource+0x2909c>
1800a191d: 8d 53 01                    	leal	0x1(%rbx), %edx
1800a1920: 48 8d 0d f1 33 10 01        	leaq	0x11033f1(%rip), %rcx   # 0x1811a4d18
1800a1927: e8 14 a2 05 00              	callq	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
1800a192c: 48 8b 6c 24 30              	movq	0x30(%rsp), %rbp
1800a1931: 48 c7 44 df 48 00 00 00 00  	movq	$0x0, 0x48(%rdi,%rbx,8)
1800a193a: c6 84 3b c8 00 00 00 00     	movb	$0x0, 0xc8(%rbx,%rdi)
1800a1942: 48 8b 74 24 38              	movq	0x38(%rsp), %rsi
1800a1947: 48 8b 5c 24 40              	movq	0x40(%rsp), %rbx
1800a194c: 48 83 c4 20                 	addq	$0x20, %rsp
1800a1950: 5f                          	popq	%rdi
1800a1951: c3                          	retq
1800a1952: cc                          	int3
1800a1953: cc                          	int3
1800a1954: cc                          	int3
1800a1955: cc                          	int3
1800a1956: cc                          	int3
1800a1957: cc                          	int3
1800a1958: cc                          	int3
1800a1959: cc                          	int3
1800a195a: cc                          	int3
1800a195b: cc                          	int3
1800a195c: cc                          	int3
1800a195d: cc                          	int3
1800a195e: cc                          	int3
1800a195f: cc                          	int3
1800a1960: 48 89 5c 24 08              	movq	%rbx, 0x8(%rsp)
1800a1965: 57                          	pushq	%rdi
1800a1966: 48 83 ec 20                 	subq	$0x20, %rsp
1800a196a: bb 01 00 00 00              	movl	$0x1, %ebx
1800a196f: 48 8d 41 50                 	leaq	0x50(%rcx), %rax
1800a1973: 8b d3                       	movl	%ebx, %edx
1800a1975: 48 8b f9                    	movq	%rcx, %rdi
1800a1978: 48 83 38 00                 	cmpq	$0x0, (%rax)
1800a197c: 75 20                       	jne	0x1800a199e <SetPDFrameWarpNativeCameraSource+0x2910e>
1800a197e: ff c2                       	incl	%edx
1800a1980: 48 83 c0 08                 	addq	$0x8, %rax
1800a1984: 83 fa 10                    	cmpl	$0x10, %edx
1800a1987: 72 ef                       	jb	0x1800a1978 <SetPDFrameWarpNativeCameraSource+0x290e8>
1800a1989: c7 81 d8 00 00 00 10 00 00 00       	movl	$0x10, 0xd8(%rcx)
1800a1993: 48 8b 5c 24 30              	movq	0x30(%rsp), %rbx
1800a1998: 48 83 c4 20                 	addq	$0x20, %rsp
1800a199c: 5f                          	popq	%rdi
1800a199d: c3                          	retq
1800a199e: 48 8b 0d 4b 3f 17 01        	movq	0x1173f4b(%rip), %rcx   # 0x1812158f0
1800a19a5: e8 16 e3 fc ff              	callq	0x18006fcc0 <SetPDFrameWarpDiagnosticHud+0x47270>
1800a19aa: 84 c0                       	testb	%al, %al
1800a19ac: 75 08                       	jne	0x1800a19b6 <SetPDFrameWarpNativeCameraSource+0x29126>
1800a19ae: 48 8b cf                    	movq	%rdi, %rcx
1800a19b1: e8 ba fd ff ff              	callq	0x1800a1770 <SetPDFrameWarpNativeCameraSource+0x28ee0>
1800a19b6: 8b d3                       	movl	%ebx, %edx
1800a19b8: 48 8b cf                    	movq	%rdi, %rcx
1800a19bb: e8 f0 fe ff ff              	callq	0x1800a18b0 <SetPDFrameWarpNativeCameraSource+0x29020>
1800a19c0: ff c3                       	incl	%ebx
1800a19c2: 83 fb 10                    	cmpl	$0x10, %ebx
1800a19c5: 72 ef                       	jb	0x1800a19b6 <SetPDFrameWarpNativeCameraSource+0x29126>
1800a19c7: c7 87 d8 00 00 00 10 00 00 00       	movl	$0x10, 0xd8(%rdi)
1800a19d1: 48 8b 5c 24 30              	movq	0x30(%rsp), %rbx
1800a19d6: 48 83 c4 20                 	addq	$0x20, %rsp
1800a19da: 5f                          	popq	%rdi
1800a19db: c3                          	retq
1800a19dc: cc                          	int3
1800a19dd: cc                          	int3
1800a19de: cc                          	int3
1800a19df: cc                          	int3
1800a19e0: 40 53                       	pushq	%rbx
1800a19e2: 56                          	pushq	%rsi
1800a19e3: 41 56                       	pushq	%r14
1800a19e5: 48 83 ec 20                 	subq	$0x20, %rsp
1800a19e9: 41 be 01 00 00 00           	movl	$0x1, %r14d
1800a19ef: 48 8b d9                    	movq	%rcx, %rbx
1800a19f2: 41 3b d6                    	cmpl	%r14d, %edx
1800a19f5: 41 8b f6                    	movl	%r14d, %esi
1800a19f8: 0f 4d f2                    	cmovgel	%edx, %esi
1800a19fb: 83 fe 10                    	cmpl	$0x10, %esi
1800a19fe: 7e 07                       	jle	0x1800a1a07 <SetPDFrameWarpNativeCameraSource+0x29177>
1800a1a00: be 10 00 00 00              	movl	$0x10, %esi
1800a1a05: eb 7e                       	jmp	0x1800a1a85 <SetPDFrameWarpNativeCameraSource+0x291f5>
1800a1a07: 48 89 7c 24 48              	movq	%rdi, 0x48(%rsp)
1800a1a0c: 8b fe                       	movl	%esi, %edi
1800a1a0e: 4c 89 7c 24 50              	movq	%r15, 0x50(%rsp)
1800a1a13: 45 32 ff                    	xorb	%r15b, %r15b
1800a1a16: 83 fe 10                    	cmpl	$0x10, %esi
1800a1a19: 73 57                       	jae	0x1800a1a72 <SetPDFrameWarpNativeCameraSource+0x291e2>
1800a1a1b: 48 89 6c 24 40              	movq	%rbp, 0x40(%rsp)
1800a1a20: 8b ee                       	movl	%esi, %ebp
1800a1a22: 48 83 c5 09                 	addq	$0x9, %rbp
1800a1a26: 48 8d 2c e9                 	leaq	(%rcx,%rbp,8), %rbp
1800a1a2a: 66 0f 1f 44 00 00           	nopw	(%rax,%rax)
1800a1a30: 48 83 7d 00 00              	cmpq	$0x0, (%rbp)
1800a1a35: 74 2b                       	je	0x1800a1a62 <SetPDFrameWarpNativeCameraSource+0x291d2>
1800a1a37: 45 84 ff                    	testb	%r15b, %r15b
1800a1a3a: 75 18                       	jne	0x1800a1a54 <SetPDFrameWarpNativeCameraSource+0x291c4>
1800a1a3c: 48 8b 0d ad 3e 17 01        	movq	0x1173ead(%rip), %rcx   # 0x1812158f0
1800a1a43: e8 78 e2 fc ff              	callq	0x18006fcc0 <SetPDFrameWarpDiagnosticHud+0x47270>
1800a1a48: 84 c0                       	testb	%al, %al
1800a1a4a: 75 08                       	jne	0x1800a1a54 <SetPDFrameWarpNativeCameraSource+0x291c4>
1800a1a4c: 48 8b cb                    	movq	%rbx, %rcx
1800a1a4f: e8 1c fd ff ff              	callq	0x1800a1770 <SetPDFrameWarpNativeCameraSource+0x28ee0>
1800a1a54: 8b d7                       	movl	%edi, %edx
1800a1a56: 48 8b cb                    	movq	%rbx, %rcx
1800a1a59: 45 0f b6 fe                 	movzbl	%r14b, %r15d
1800a1a5d: e8 4e fe ff ff              	callq	0x1800a18b0 <SetPDFrameWarpNativeCameraSource+0x29020>
1800a1a62: ff c7                       	incl	%edi
1800a1a64: 48 83 c5 08                 	addq	$0x8, %rbp
1800a1a68: 83 ff 10                    	cmpl	$0x10, %edi
1800a1a6b: 72 c3                       	jb	0x1800a1a30 <SetPDFrameWarpNativeCameraSource+0x291a0>
1800a1a6d: 48 8b 6c 24 40              	movq	0x40(%rsp), %rbp
1800a1a72: 4c 8b 7c 24 50              	movq	0x50(%rsp), %r15
1800a1a77: 48 8b 7c 24 48              	movq	0x48(%rsp), %rdi
1800a1a7c: 41 3b f6                    	cmpl	%r14d, %esi
1800a1a7f: 0f 8e d9 00 00 00           	jle	0x1800a1b5e <SetPDFrameWarpNativeCameraSource+0x292ce>
1800a1a85: 8b 83 d8 00 00 00           	movl	0xd8(%rbx), %eax
1800a1a8b: 41 3b c6                    	cmpl	%r14d, %eax
1800a1a8e: 0f 86 ca 00 00 00           	jbe	0x1800a1b5e <SetPDFrameWarpNativeCameraSource+0x292ce>
1800a1a94: 8b 93 0c 01 00 00           	movl	0x10c(%rbx), %edx
1800a1a9a: 85 d2                       	testl	%edx, %edx
1800a1a9c: 0f 8e bc 00 00 00           	jle	0x1800a1b5e <SetPDFrameWarpNativeCameraSource+0x292ce>
1800a1aa2: 44 8b 83 10 01 00 00        	movl	0x110(%rbx), %r8d
1800a1aa9: 45 85 c0                    	testl	%r8d, %r8d
1800a1aac: 0f 8e ac 00 00 00           	jle	0x1800a1b5e <SetPDFrameWarpNativeCameraSource+0x292ce>
1800a1ab2: 3b c6                       	cmpl	%esi, %eax
1800a1ab4: 0f 42 f0                    	cmovbl	%eax, %esi
1800a1ab7: 41 3b f6                    	cmpl	%r14d, %esi
1800a1aba: 0f 86 9e 00 00 00           	jbe	0x1800a1b5e <SetPDFrameWarpNativeCameraSource+0x292ce>
1800a1ac0: 48 8d 43 50                 	leaq	0x50(%rbx), %rax
1800a1ac4: 48 83 38 00                 	cmpq	$0x0, (%rax)
1800a1ac8: 74 15                       	je	0x1800a1adf <SetPDFrameWarpNativeCameraSource+0x2924f>
1800a1aca: 41 ff c6                    	incl	%r14d
1800a1acd: 48 83 c0 08                 	addq	$0x8, %rax
1800a1ad1: 44 3b f6                    	cmpl	%esi, %r14d
1800a1ad4: 72 ee                       	jb	0x1800a1ac4 <SetPDFrameWarpNativeCameraSource+0x29234>
1800a1ad6: 48 83 c4 20                 	addq	$0x20, %rsp
1800a1ada: 41 5e                       	popq	%r14
1800a1adc: 5e                          	popq	%rsi
1800a1add: 5b                          	popq	%rbx
1800a1ade: c3                          	retq
1800a1adf: 80 bb 14 01 00 00 00        	cmpb	$0x0, 0x114(%rbx)
1800a1ae6: 74 0a                       	je	0x1800a1af2 <SetPDFrameWarpNativeCameraSource+0x29262>
1800a1ae8: f3 0f 10 1d e0 af 10 01     	movss	0x110afe0(%rip), %xmm3  # 0x1811acad0
1800a1af0: eb 08                       	jmp	0x1800a1afa <SetPDFrameWarpNativeCameraSource+0x2926a>
1800a1af2: f3 0f 10 9b 08 01 00 00     	movss	0x108(%rbx), %xmm3
1800a1afa: 48 8b cb                    	movq	%rbx, %rcx
1800a1afd: e8 0e aa ff ff              	callq	0x18009c510 <SetPDFrameWarpNativeCameraSource+0x23c80>
1800a1b02: 41 8d 56 01                 	leal	0x1(%r14), %edx
1800a1b06: 48 8b c8                    	movq	%rax, %rcx
1800a1b09: 48 85 c0                    	testq	%rax, %rax
1800a1b0c: 75 1e                       	jne	0x1800a1b2c <SetPDFrameWarpNativeCameraSource+0x2929c>
1800a1b0e: 45 8b c6                    	movl	%r14d, %r8d
1800a1b11: 44 89 b3 d8 00 00 00        	movl	%r14d, 0xd8(%rbx)
1800a1b18: 48 8d 0d b9 31 10 01        	leaq	0x11031b9(%rip), %rcx   # 0x1811a4cd8
1800a1b1f: 48 83 c4 20                 	addq	$0x20, %rsp
1800a1b23: 41 5e                       	popq	%r14
1800a1b25: 5e                          	popq	%rsi
1800a1b26: 5b                          	popq	%rbx
1800a1b27: e9 14 a0 05 00              	jmp	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
1800a1b2c: 41 8b c6                    	movl	%r14d, %eax
1800a1b2f: 48 89 4c c3 48              	movq	%rcx, 0x48(%rbx,%rax,8)
1800a1b34: 48 8d 0d 3d 32 10 01        	leaq	0x110323d(%rip), %rcx   # 0x1811a4d78
1800a1b3b: c6 84 18 c8 00 00 00 01     	movb	$0x1, 0xc8(%rax,%rbx)
1800a1b43: 44 8b 8b 10 01 00 00        	movl	0x110(%rbx), %r9d
1800a1b4a: 44 8b 83 0c 01 00 00        	movl	0x10c(%rbx), %r8d
1800a1b51: 48 83 c4 20                 	addq	$0x20, %rsp
1800a1b55: 41 5e                       	popq	%r14
1800a1b57: 5e                          	popq	%rsi
1800a1b58: 5b                          	popq	%rbx
1800a1b59: e9 e2 9f 05 00              	jmp	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
1800a1b5e: 48 83 c4 20                 	addq	$0x20, %rsp
1800a1b62: 41 5e                       	popq	%r14
1800a1b64: 5e                          	popq	%rsi
1800a1b65: 5b                          	popq	%rbx
1800a1b66: c3                          	retq
1800a1b67: cc                          	int3
1800a1b68: cc                          	int3
1800a1b69: cc                          	int3
1800a1b6a: cc                          	int3
1800a1b6b: cc                          	int3
1800a1b6c: cc                          	int3
1800a1b6d: cc                          	int3
1800a1b6e: cc                          	int3
1800a1b6f: cc                          	int3
