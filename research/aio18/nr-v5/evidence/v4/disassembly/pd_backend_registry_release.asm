
/mnt/data/aio18_nr_work/PDPerfPlugin.dll:	file format coff-x86-64

Disassembly of section .text:

0000000180078890 <SetPDFrameWarpNativeCameraSource>:
1800a1bb0: 48 89 6c 24 10              	movq	%rbp, 0x10(%rsp)
1800a1bb5: 48 89 74 24 18              	movq	%rsi, 0x18(%rsp)
1800a1bba: 48 89 7c 24 20              	movq	%rdi, 0x20(%rsp)
1800a1bbf: 41 56                       	pushq	%r14
1800a1bc1: 48 83 ec 20                 	subq	$0x20, %rsp
1800a1bc5: 48 8b e9                    	movq	%rcx, %rbp
1800a1bc8: 8b f2                       	movl	%edx, %esi
1800a1bca: 48 8b 49 38                 	movq	0x38(%rcx), %rcx
1800a1bce: 48 8b f9                    	movq	%rcx, %rdi
1800a1bd1: 48 8b 41 08                 	movq	0x8(%rcx), %rax
1800a1bd5: 80 78 19 00                 	cmpb	$0x0, 0x19(%rax)
1800a1bd9: 75 1c                       	jne	0x1800a1bf7 <SetPDFrameWarpNativeCameraSource+0x29367>
1800a1bdb: 0f 1f 44 00 00              	nopl	(%rax,%rax)
1800a1be0: 39 70 20                    	cmpl	%esi, 0x20(%rax)
1800a1be3: 7d 06                       	jge	0x1800a1beb <SetPDFrameWarpNativeCameraSource+0x2935b>
1800a1be5: 48 83 c0 10                 	addq	$0x10, %rax
1800a1be9: eb 03                       	jmp	0x1800a1bee <SetPDFrameWarpNativeCameraSource+0x2935e>
1800a1beb: 48 8b f8                    	movq	%rax, %rdi
1800a1bee: 48 8b 00                    	movq	(%rax), %rax
1800a1bf1: 80 78 19 00                 	cmpb	$0x0, 0x19(%rax)
1800a1bf5: 74 e9                       	je	0x1800a1be0 <SetPDFrameWarpNativeCameraSource+0x29350>
1800a1bf7: 80 7f 19 00                 	cmpb	$0x0, 0x19(%rdi)
1800a1bfb: 0f 85 04 01 00 00           	jne	0x1800a1d05 <SetPDFrameWarpNativeCameraSource+0x29475>
1800a1c01: 3b 77 20                    	cmpl	0x20(%rdi), %esi
1800a1c04: 0f 8c fb 00 00 00           	jl	0x1800a1d05 <SetPDFrameWarpNativeCameraSource+0x29475>
1800a1c0a: 48 3b f9                    	cmpq	%rcx, %rdi
1800a1c0d: 0f 84 f2 00 00 00           	je	0x1800a1d05 <SetPDFrameWarpNativeCameraSource+0x29475>
1800a1c13: 48 83 7f 28 00              	cmpq	$0x0, 0x28(%rdi)
1800a1c18: 0f 84 e7 00 00 00           	je	0x1800a1d05 <SetPDFrameWarpNativeCameraSource+0x29475>
1800a1c1e: 48 8b cd                    	movq	%rbp, %rcx
1800a1c21: 48 89 5c 24 30              	movq	%rbx, 0x30(%rsp)
1800a1c26: e8 45 fb ff ff              	callq	0x1800a1770 <SetPDFrameWarpNativeCameraSource+0x28ee0>
1800a1c2b: bb 01 00 00 00              	movl	$0x1, %ebx
1800a1c30: 8b d3                       	movl	%ebx, %edx
1800a1c32: 48 8b cd                    	movq	%rbp, %rcx
1800a1c35: e8 76 fc ff ff              	callq	0x1800a18b0 <SetPDFrameWarpNativeCameraSource+0x29020>
1800a1c3a: ff c3                       	incl	%ebx
1800a1c3c: 83 fb 10                    	cmpl	$0x10, %ebx
1800a1c3f: 72 ef                       	jb	0x1800a1c30 <SetPDFrameWarpNativeCameraSource+0x293a0>
1800a1c41: c7 85 d8 00 00 00 10 00 00 00       	movl	$0x10, 0xd8(%rbp)
1800a1c4b: 48 8b 4f 28                 	movq	0x28(%rdi), %rcx
1800a1c4f: e8 0c 25 00 00              	callq	0x1800a4160 <SetPDFrameWarpNativeCameraSource+0x2b8d0>
1800a1c54: 8b c8                       	movl	%eax, %ecx
1800a1c56: 8b d8                       	movl	%eax, %ebx
1800a1c58: 81 e1 00 00 f0 ff           	andl	$0xfff00000, %ecx       # imm = 0xFFF00000
1800a1c5e: 81 f9 00 00 d0 ba           	cmpl	$0xbad00000, %ecx       # imm = 0xBAD00000
1800a1c64: 75 29                       	jne	0x1800a1c8f <SetPDFrameWarpNativeCameraSource+0x293ff>
1800a1c66: 8b c8                       	movl	%eax, %ecx
1800a1c68: e8 53 15 06 00              	callq	0x1801031c0 <NVSDK_NGX_D3D12_Shutdown1+0x170>
1800a1c6d: 4c 8b c8                    	movq	%rax, %r9
1800a1c70: 48 8d 0d c1 30 10 01        	leaq	0x11030c1(%rip), %rcx   # 0x1811a4d38
1800a1c77: 44 8b c3                    	movl	%ebx, %r8d
1800a1c7a: 8b d6                       	movl	%esi, %edx
1800a1c7c: e8 bf 9e 05 00              	callq	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
1800a1c81: 48 8d 0d 38 31 10 01        	leaq	0x1103138(%rip), %rcx   # 0x1811a4dc0
1800a1c88: e8 63 a6 ff ff              	callq	0x18009c2f0 <SetPDFrameWarpNativeCameraSource+0x23a60>
1800a1c8d: eb 0e                       	jmp	0x1800a1c9d <SetPDFrameWarpNativeCameraSource+0x2940d>
1800a1c8f: 8b d6                       	movl	%esi, %edx
1800a1c91: 48 8d 0d 08 31 10 01        	leaq	0x1103108(%rip), %rcx   # 0x1811a4da0
1800a1c98: e8 a3 9e 05 00              	callq	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
1800a1c9d: 4c 8b 47 10                 	movq	0x10(%rdi), %r8
1800a1ca1: 48 8b c7                    	movq	%rdi, %rax
1800a1ca4: 48 8b 5c 24 30              	movq	0x30(%rsp), %rbx
1800a1ca9: 41 80 78 19 00              	cmpb	$0x0, 0x19(%r8)
1800a1cae: 74 25                       	je	0x1800a1cd5 <SetPDFrameWarpNativeCameraSource+0x29445>
1800a1cb0: 48 8b 4f 08                 	movq	0x8(%rdi), %rcx
1800a1cb4: 80 79 19 00                 	cmpb	$0x0, 0x19(%rcx)
1800a1cb8: 75 32                       	jne	0x1800a1cec <SetPDFrameWarpNativeCameraSource+0x2945c>
1800a1cba: 66 0f 1f 44 00 00           	nopw	(%rax,%rax)
1800a1cc0: 48 3b 41 10                 	cmpq	0x10(%rcx), %rax
1800a1cc4: 75 26                       	jne	0x1800a1cec <SetPDFrameWarpNativeCameraSource+0x2945c>
1800a1cc6: 48 8b c1                    	movq	%rcx, %rax
1800a1cc9: 48 8b 49 08                 	movq	0x8(%rcx), %rcx
1800a1ccd: 80 79 19 00                 	cmpb	$0x0, 0x19(%rcx)
1800a1cd1: 74 ed                       	je	0x1800a1cc0 <SetPDFrameWarpNativeCameraSource+0x29430>
1800a1cd3: eb 17                       	jmp	0x1800a1cec <SetPDFrameWarpNativeCameraSource+0x2945c>
1800a1cd5: 4d 8b 00                    	movq	(%r8), %r8
1800a1cd8: 41 80 78 19 00              	cmpb	$0x0, 0x19(%r8)
1800a1cdd: 75 0d                       	jne	0x1800a1cec <SetPDFrameWarpNativeCameraSource+0x2945c>
1800a1cdf: 90                          	nop
1800a1ce0: 49 8b 00                    	movq	(%r8), %rax
1800a1ce3: 4c 8b c0                    	movq	%rax, %r8
1800a1ce6: 80 78 19 00                 	cmpb	$0x0, 0x19(%rax)
1800a1cea: 74 f4                       	je	0x1800a1ce0 <SetPDFrameWarpNativeCameraSource+0x29450>
1800a1cec: 48 8b d7                    	movq	%rdi, %rdx
1800a1cef: 48 8d 4d 38                 	leaq	0x38(%rbp), %rcx
1800a1cf3: e8 18 51 fc ff              	callq	0x180066e10 <SetPDFrameWarpDiagnosticHud+0x3e3c0>
1800a1cf8: ba 30 00 00 00              	movl	$0x30, %edx
1800a1cfd: 48 8b c8                    	movq	%rax, %rcx
1800a1d00: e8 97 a6 06 00              	callq	0x18010c39c <NVSDK_NGX_UpdateFeature+0x2f5c>
1800a1d05: 48 8b 6c 24 38              	movq	0x38(%rsp), %rbp
1800a1d0a: 48 8b 74 24 40              	movq	0x40(%rsp), %rsi
1800a1d0f: 48 8b 7c 24 48              	movq	0x48(%rsp), %rdi
1800a1d14: 48 83 c4 20                 	addq	$0x20, %rsp
1800a1d18: 41 5e                       	popq	%r14
1800a1d1a: c3                          	retq
1800a1d1b: cc                          	int3
1800a1d1c: cc                          	int3
1800a1d1d: cc                          	int3
1800a1d1e: cc                          	int3
1800a1d1f: cc                          	int3
1800a1d20: 48 89 5c 24 10              	movq	%rbx, 0x10(%rsp)
1800a1d25: 48 89 74 24 18              	movq	%rsi, 0x18(%rsp)
1800a1d2a: 57                          	pushq	%rdi
1800a1d2b: 48 83 ec 20                 	subq	$0x20, %rsp
1800a1d2f: 8b fa                       	movl	%edx, %edi
1800a1d31: 48 8b f1                    	movq	%rcx, %rsi
1800a1d34: c6 81 b1 01 00 00 00        	movb	$0x0, 0x1b1(%rcx)
1800a1d3b: 48 8b 49 38                 	movq	0x38(%rcx), %rcx
1800a1d3f: 48 8b 41 08                 	movq	0x8(%rcx), %rax
1800a1d43: 4c 8b c1                    	movq	%rcx, %r8
1800a1d46: 80 78 19 00                 	cmpb	$0x0, 0x19(%rax)
1800a1d4a: 75 1b                       	jne	0x1800a1d67 <SetPDFrameWarpNativeCameraSource+0x294d7>
1800a1d4c: 0f 1f 40 00                 	nopl	(%rax)
1800a1d50: 39 78 20                    	cmpl	%edi, 0x20(%rax)
1800a1d53: 7d 06                       	jge	0x1800a1d5b <SetPDFrameWarpNativeCameraSource+0x294cb>
1800a1d55: 48 83 c0 10                 	addq	$0x10, %rax
1800a1d59: eb 03                       	jmp	0x1800a1d5e <SetPDFrameWarpNativeCameraSource+0x294ce>
1800a1d5b: 4c 8b c0                    	movq	%rax, %r8
1800a1d5e: 48 8b 00                    	movq	(%rax), %rax
1800a1d61: 80 78 19 00                 	cmpb	$0x0, 0x19(%rax)
1800a1d65: 74 e9                       	je	0x1800a1d50 <SetPDFrameWarpNativeCameraSource+0x294c0>
1800a1d67: 41 80 78 19 00              	cmpb	$0x0, 0x19(%r8)
1800a1d6c: 0f 85 b5 00 00 00           	jne	0x1800a1e27 <SetPDFrameWarpNativeCameraSource+0x29597>
1800a1d72: 41 3b 78 20                 	cmpl	0x20(%r8), %edi
1800a1d76: 0f 8c ab 00 00 00           	jl	0x1800a1e27 <SetPDFrameWarpNativeCameraSource+0x29597>
1800a1d7c: 4c 3b c1                    	cmpq	%rcx, %r8
1800a1d7f: 0f 84 a2 00 00 00           	je	0x1800a1e27 <SetPDFrameWarpNativeCameraSource+0x29597>
1800a1d85: 49 83 78 28 00              	cmpq	$0x0, 0x28(%r8)
1800a1d8a: 0f 84 97 00 00 00           	je	0x1800a1e27 <SetPDFrameWarpNativeCameraSource+0x29597>
1800a1d90: 48 8b ce                    	movq	%rsi, %rcx
1800a1d93: e8 28 a1 ff ff              	callq	0x18009bec0 <SetPDFrameWarpNativeCameraSource+0x23630>
1800a1d98: 48 8b 1d 51 3b 17 01        	movq	0x1173b51(%rip), %rbx   # 0x1812158f0
1800a1d9f: 84 c0                       	testb	%al, %al
1800a1da1: 74 2f                       	je	0x1800a1dd2 <SetPDFrameWarpNativeCameraSource+0x29542>
1800a1da3: 48 81 c3 78 0a 00 00        	addq	$0xa78, %rbx            # imm = 0xA78
1800a1daa: 48 89 5c 24 30              	movq	%rbx, 0x30(%rsp)
1800a1daf: 48 8b cb                    	movq	%rbx, %rcx
1800a1db2: ff 15 80 17 07 00           	callq	*0x71780(%rip)          # 0x180113538
1800a1db8: 85 c0                       	testl	%eax, %eax
1800a1dba: 75 2f                       	jne	0x1800a1deb <SetPDFrameWarpNativeCameraSource+0x2955b>
1800a1dbc: 81 7b 4c ff ff ff 7f        	cmpl	$0x7fffffff, 0x4c(%rbx) # imm = 0x7FFFFFFF
1800a1dc3: 74 3b                       	je	0x1800a1e00 <SetPDFrameWarpNativeCameraSource+0x29570>
1800a1dc5: 8b d7                       	movl	%edi, %edx
1800a1dc7: 48 8b ce                    	movq	%rsi, %rcx
1800a1dca: e8 e1 fd ff ff              	callq	0x1800a1bb0 <SetPDFrameWarpNativeCameraSource+0x29320>
1800a1dcf: 90                          	nop
1800a1dd0: eb 4c                       	jmp	0x1800a1e1e <SetPDFrameWarpNativeCameraSource+0x2958e>
1800a1dd2: 48 81 c3 c8 0a 00 00        	addq	$0xac8, %rbx            # imm = 0xAC8
1800a1dd9: 48 89 5c 24 30              	movq	%rbx, 0x30(%rsp)
1800a1dde: 48 8b cb                    	movq	%rbx, %rcx
1800a1de1: ff 15 51 17 07 00           	callq	*0x71751(%rip)          # 0x180113538
1800a1de7: 85 c0                       	testl	%eax, %eax
1800a1de9: 74 0c                       	je	0x1800a1df7 <SetPDFrameWarpNativeCameraSource+0x29567>
1800a1deb: b9 05 00 00 00              	movl	$0x5, %ecx
1800a1df0: ff 15 4a 17 07 00           	callq	*0x7174a(%rip)          # 0x180113540
1800a1df6: cc                          	int3
1800a1df7: 81 7b 4c ff ff ff 7f        	cmpl	$0x7fffffff, 0x4c(%rbx) # imm = 0x7FFFFFFF
1800a1dfe: 75 13                       	jne	0x1800a1e13 <SetPDFrameWarpNativeCameraSource+0x29583>
1800a1e00: c7 43 4c fe ff ff 7f        	movl	$0x7ffffffe, 0x4c(%rbx) # imm = 0x7FFFFFFE
1800a1e07: b9 06 00 00 00              	movl	$0x6, %ecx
1800a1e0c: ff 15 2e 17 07 00           	callq	*0x7172e(%rip)          # 0x180113540
1800a1e12: 90                          	nop
1800a1e13: 8b d7                       	movl	%edi, %edx
1800a1e15: 48 8b ce                    	movq	%rsi, %rcx
1800a1e18: e8 93 fd ff ff              	callq	0x1800a1bb0 <SetPDFrameWarpNativeCameraSource+0x29320>
1800a1e1d: 90                          	nop
1800a1e1e: 48 8b cb                    	movq	%rbx, %rcx
1800a1e21: ff 15 01 17 07 00           	callq	*0x71701(%rip)          # 0x180113528
1800a1e27: 48 8b 5c 24 38              	movq	0x38(%rsp), %rbx
1800a1e2c: 48 8b 74 24 40              	movq	0x40(%rsp), %rsi
1800a1e31: 48 83 c4 20                 	addq	$0x20, %rsp
1800a1e35: 5f                          	popq	%rdi
1800a1e36: c3                          	retq
1800a1e37: cc                          	int3
1800a1e38: cc                          	int3
1800a1e39: cc                          	int3
1800a1e3a: cc                          	int3
1800a1e3b: cc                          	int3
1800a1e3c: cc                          	int3
1800a1e3d: cc                          	int3
1800a1e3e: cc                          	int3
1800a1e3f: cc                          	int3
