
/mnt/data/aio18_nr_work/PDPerfPlugin.dll:	file format coff-x86-64

Disassembly of section .text:

00000001800f4940 <GetPDFrameWarpContext>:
1800f4ae0: 48 89 5c 24 18              	movq	%rbx, 0x18(%rsp)
1800f4ae5: 48 89 74 24 20              	movq	%rsi, 0x20(%rsp)
1800f4aea: 55                          	pushq	%rbp
1800f4aeb: 57                          	pushq	%rdi
1800f4aec: 41 56                       	pushq	%r14
1800f4aee: 48 8d ac 24 a0 fe ff ff     	leaq	-0x160(%rsp), %rbp
1800f4af6: 48 81 ec 60 02 00 00        	subq	$0x260, %rsp            # imm = 0x260
1800f4afd: 48 8b 05 bc 5e 0d 01        	movq	0x10d5ebc(%rip), %rax   # 0x1811ca9c0
1800f4b04: 48 33 c4                    	xorq	%rsp, %rax
1800f4b07: 48 89 85 50 01 00 00        	movq	%rax, 0x150(%rbp)
1800f4b0e: 48 8b da                    	movq	%rdx, %rbx
1800f4b11: 48 8b f1                    	movq	%rcx, %rsi
1800f4b14: 33 c9                       	xorl	%ecx, %ecx
1800f4b16: e8 85 ea fd ff              	callq	0x1800d35a0 <SetPDFrameWarpNativeCameraSource+0x5ad10>
1800f4b1b: 48 83 7e 78 00              	cmpq	$0x0, 0x78(%rsi)
1800f4b20: 0f 84 c7 02 00 00           	je	0x1800f4ded <GetPDFrameWarpContext+0x4ad>
1800f4b26: 45 33 f6                    	xorl	%r14d, %r14d
1800f4b29: 44 38 76 6c                 	cmpb	%r14b, 0x6c(%rsi)
1800f4b2d: 74 12                       	je	0x1800f4b41 <GetPDFrameWarpContext+0x201>
1800f4b2f: e8 8c 4e f9 ff              	callq	0x1800899c0 <SetPDFrameWarpNativeCameraSource+0x11130>
1800f4b34: 48 8b c8                    	movq	%rax, %rcx
1800f4b37: e8 54 c8 f9 ff              	callq	0x180091390 <SetPDFrameWarpNativeCameraSource+0x18b00>
1800f4b3c: 48 8b f8                    	movq	%rax, %rdi
1800f4b3f: eb 03                       	jmp	0x1800f4b44 <GetPDFrameWarpContext+0x204>
1800f4b41: 49 8b fe                    	movq	%r14, %rdi
1800f4b44: 44 38 76 6c                 	cmpb	%r14b, 0x6c(%rsi)
1800f4b48: 74 07                       	je	0x1800f4b51 <GetPDFrameWarpContext+0x211>
1800f4b4a: 44 88 b3 10 01 00 00        	movb	%r14b, 0x110(%rbx)
1800f4b51: 48 85 ff                    	testq	%rdi, %rdi
1800f4b54: 0f 84 09 02 00 00           	je	0x1800f4d63 <GetPDFrameWarpContext+0x423>
1800f4b5a: 44 38 b7 5e 01 00 00        	cmpb	%r14b, 0x15e(%rdi)
1800f4b61: 0f 84 fc 01 00 00           	je	0x1800f4d63 <GetPDFrameWarpContext+0x423>
1800f4b67: 48 8b 43 08                 	movq	0x8(%rbx), %rax
1800f4b6b: 48 89 45 a0                 	movq	%rax, -0x60(%rbp)
1800f4b6f: 48 8b 43 10                 	movq	0x10(%rbx), %rax
1800f4b73: 48 89 45 a8                 	movq	%rax, -0x58(%rbp)
1800f4b77: 48 8b 43 18                 	movq	0x18(%rbx), %rax
1800f4b7b: 48 89 45 b0                 	movq	%rax, -0x50(%rbp)
1800f4b7f: 48 8b 43 20                 	movq	0x20(%rbx), %rax
1800f4b83: 48 89 45 b8                 	movq	%rax, -0x48(%rbp)
1800f4b87: 48 8b 43 28                 	movq	0x28(%rbx), %rax
1800f4b8b: 48 89 45 c0                 	movq	%rax, -0x40(%rbp)
1800f4b8f: 48 8b 43 30                 	movq	0x30(%rbx), %rax
1800f4b93: 48 89 45 c8                 	movq	%rax, -0x38(%rbp)
1800f4b97: 48 8b 43 38                 	movq	0x38(%rbx), %rax
1800f4b9b: 48 89 45 d0                 	movq	%rax, -0x30(%rbp)
1800f4b9f: 48 8b 43 40                 	movq	0x40(%rbx), %rax
1800f4ba3: 48 89 45 d8                 	movq	%rax, -0x28(%rbp)
1800f4ba7: 48 8b 43 48                 	movq	0x48(%rbx), %rax
1800f4bab: 48 89 45 e0                 	movq	%rax, -0x20(%rbp)
1800f4baf: 48 8d 45 a0                 	leaq	-0x60(%rbp), %rax
1800f4bb3: 48 89 44 24 50              	movq	%rax, 0x50(%rsp)
1800f4bb8: 48 8d 45 e8                 	leaq	-0x18(%rbp), %rax
1800f4bbc: 48 89 44 24 58              	movq	%rax, 0x58(%rsp)
1800f4bc1: 48 8d 54 24 50              	leaq	0x50(%rsp), %rdx
1800f4bc6: 48 8d 4c 24 30              	leaq	0x30(%rsp), %rcx
1800f4bcb: e8 30 f7 ff ff              	callq	0x1800f4300 <SetPDFrameWarpNativeCameraSource+0x7ba70>
1800f4bd0: 90                          	nop
1800f4bd1: 48 8d 44 24 60              	leaq	0x60(%rsp), %rax
1800f4bd6: 48 89 44 24 50              	movq	%rax, 0x50(%rsp)
1800f4bdb: 48 8b 46 78                 	movq	0x78(%rsi), %rax
1800f4bdf: 48 89 45 f0                 	movq	%rax, -0x10(%rbp)
1800f4be3: 48 8d 4d f8                 	leaq	-0x8(%rbp), %rcx
1800f4be7: b8 02 00 00 00              	movl	$0x2, %eax
1800f4bec: 0f 1f 40 00                 	nopl	(%rax)
1800f4bf0: 0f 10 03                    	movups	(%rbx), %xmm0
1800f4bf3: 0f 11 01                    	movups	%xmm0, (%rcx)
1800f4bf6: 0f 10 4b 10                 	movups	0x10(%rbx), %xmm1
1800f4bfa: 0f 11 49 10                 	movups	%xmm1, 0x10(%rcx)
1800f4bfe: 0f 10 43 20                 	movups	0x20(%rbx), %xmm0
1800f4c02: 0f 11 41 20                 	movups	%xmm0, 0x20(%rcx)
1800f4c06: 0f 10 4b 30                 	movups	0x30(%rbx), %xmm1
1800f4c0a: 0f 11 49 30                 	movups	%xmm1, 0x30(%rcx)
1800f4c0e: 0f 10 43 40                 	movups	0x40(%rbx), %xmm0
1800f4c12: 0f 11 41 40                 	movups	%xmm0, 0x40(%rcx)
1800f4c16: 0f 10 4b 50                 	movups	0x50(%rbx), %xmm1
1800f4c1a: 0f 11 49 50                 	movups	%xmm1, 0x50(%rcx)
1800f4c1e: 0f 10 43 60                 	movups	0x60(%rbx), %xmm0
1800f4c22: 0f 11 41 60                 	movups	%xmm0, 0x60(%rcx)
1800f4c26: 48 8d 89 80 00 00 00        	leaq	0x80(%rcx), %rcx
1800f4c2d: 0f 10 4b 70                 	movups	0x70(%rbx), %xmm1
1800f4c31: 0f 11 49 f0                 	movups	%xmm1, -0x10(%rcx)
1800f4c35: 48 8d 9b 80 00 00 00        	leaq	0x80(%rbx), %rbx
1800f4c3c: 48 83 e8 01                 	subq	$0x1, %rax
1800f4c40: 75 ae                       	jne	0x1800f4bf0 <GetPDFrameWarpContext+0x2b0>
1800f4c42: 0f 10 03                    	movups	(%rbx), %xmm0
1800f4c45: 0f 11 01                    	movups	%xmm0, (%rcx)
1800f4c48: 0f 10 4b 10                 	movups	0x10(%rbx), %xmm1
1800f4c4c: 0f 11 49 10                 	movups	%xmm1, 0x10(%rcx)
1800f4c50: 0f 10 43 20                 	movups	0x20(%rbx), %xmm0
1800f4c54: 0f 11 41 20                 	movups	%xmm0, 0x20(%rcx)
1800f4c58: 48 8b 43 30                 	movq	0x30(%rbx), %rax
1800f4c5c: 48 89 41 30                 	movq	%rax, 0x30(%rcx)
1800f4c60: 48 8b 54 24 40              	movq	0x40(%rsp), %rdx
1800f4c65: 4c 89 74 24 40              	movq	%r14, 0x40(%rsp)
1800f4c6a: 48 8b 4c 24 38              	movq	0x38(%rsp), %rcx
1800f4c6f: 4c 89 74 24 38              	movq	%r14, 0x38(%rsp)
1800f4c74: 48 8b 44 24 30              	movq	0x30(%rsp), %rax
1800f4c79: 4c 89 74 24 30              	movq	%r14, 0x30(%rsp)
1800f4c7e: 48 89 85 30 01 00 00        	movq	%rax, 0x130(%rbp)
1800f4c85: 48 89 8d 38 01 00 00        	movq	%rcx, 0x138(%rbp)
1800f4c8c: 48 89 95 40 01 00 00        	movq	%rdx, 0x140(%rbp)
1800f4c93: 4c 89 75 98                 	movq	%r14, -0x68(%rbp)
1800f4c97: e8 a4 6e f5 ff              	callq	0x18004bb40 <SetPDFrameWarpDiagnosticHud+0x230f0>
1800f4c9c: 84 c0                       	testb	%al, %al
1800f4c9e: 74 0d                       	je	0x1800f4cad <GetPDFrameWarpContext+0x36d>
1800f4ca0: 48 8d 4d f0                 	leaq	-0x10(%rbp), %rcx
1800f4ca4: e8 87 15 00 00              	callq	0x1800f6230 <GetPDFrameWarpContext+0x18f0>
1800f4ca9: 48 89 45 98                 	movq	%rax, -0x68(%rbp)
1800f4cad: 48 8d 54 24 60              	leaq	0x60(%rsp), %rdx
1800f4cb2: 48 8b cf                    	movq	%rdi, %rcx
1800f4cb5: e8 56 38 f1 ff              	callq	0x180008510 <.text+0x7510>
1800f4cba: 8b d8                       	movl	%eax, %ebx
1800f4cbc: 48 8d 8d 30 01 00 00        	leaq	0x130(%rbp), %rcx
1800f4cc3: e8 d8 a9 f1 ff              	callq	0x18000f6a0 <.text+0xe6a0>
1800f4cc8: 85 db                       	testl	%ebx, %ebx
1800f4cca: 79 0f                       	jns	0x1800f4cdb <GetPDFrameWarpContext+0x39b>
1800f4ccc: 8b d3                       	movl	%ebx, %edx
1800f4cce: 48 8d 0d cb 69 0b 01        	leaq	0x10b69cb(%rip), %rcx   # 0x1811ab6a0
1800f4cd5: e8 66 6e 00 00              	callq	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
1800f4cda: 90                          	nop
1800f4cdb: 48 8b 5c 24 30              	movq	0x30(%rsp), %rbx
1800f4ce0: 48 85 db                    	testq	%rbx, %rbx
1800f4ce3: 0f 84 04 01 00 00           	je	0x1800f4ded <GetPDFrameWarpContext+0x4ad>
1800f4ce9: 48 8b 7c 24 38              	movq	0x38(%rsp), %rdi
1800f4cee: 48 3b df                    	cmpq	%rdi, %rbx
1800f4cf1: 74 20                       	je	0x1800f4d13 <GetPDFrameWarpContext+0x3d3>
1800f4cf3: 48 8b 0b                    	movq	(%rbx), %rcx
1800f4cf6: 48 85 c9                    	testq	%rcx, %rcx
1800f4cf9: 74 0a                       	je	0x1800f4d05 <GetPDFrameWarpContext+0x3c5>
1800f4cfb: 4c 89 33                    	movq	%r14, (%rbx)
1800f4cfe: 48 8b 01                    	movq	(%rcx), %rax
1800f4d01: ff 50 10                    	callq	*0x10(%rax)
1800f4d04: 90                          	nop
1800f4d05: 48 83 c3 08                 	addq	$0x8, %rbx
1800f4d09: 48 3b df                    	cmpq	%rdi, %rbx
1800f4d0c: 75 e5                       	jne	0x1800f4cf3 <GetPDFrameWarpContext+0x3b3>
1800f4d0e: 48 8b 5c 24 30              	movq	0x30(%rsp), %rbx
1800f4d13: 48 8b 54 24 40              	movq	0x40(%rsp), %rdx
1800f4d18: 48 2b d3                    	subq	%rbx, %rdx
1800f4d1b: 48 83 e2 f8                 	andq	$-0x8, %rdx
1800f4d1f: 48 8b c3                    	movq	%rbx, %rax
1800f4d22: 48 81 fa 00 10 00 00        	cmpq	$0x1000, %rdx           # imm = 0x1000
1800f4d29: 72 2b                       	jb	0x1800f4d56 <GetPDFrameWarpContext+0x416>
1800f4d2b: 48 83 c2 27                 	addq	$0x27, %rdx
1800f4d2f: 48 8b 5b f8                 	movq	-0x8(%rbx), %rbx
1800f4d33: 48 2b c3                    	subq	%rbx, %rax
1800f4d36: 48 83 e8 08                 	subq	$0x8, %rax
1800f4d3a: 48 83 f8 1f                 	cmpq	$0x1f, %rax
1800f4d3e: 76 16                       	jbe	0x1800f4d56 <GetPDFrameWarpContext+0x416>
1800f4d40: 4c 89 74 24 20              	movq	%r14, 0x20(%rsp)
1800f4d45: 45 33 c9                    	xorl	%r9d, %r9d
1800f4d48: 45 33 c0                    	xorl	%r8d, %r8d
1800f4d4b: 33 d2                       	xorl	%edx, %edx
1800f4d4d: 33 c9                       	xorl	%ecx, %ecx
1800f4d4f: ff 15 63 ea 01 00           	callq	*0x1ea63(%rip)          # 0x1801137b8
1800f4d55: cc                          	int3
1800f4d56: 48 8b cb                    	movq	%rbx, %rcx
1800f4d59: e8 3e 76 01 00              	callq	0x18010c39c <NVSDK_NGX_UpdateFeature+0x2f5c>
1800f4d5e: e9 8a 00 00 00              	jmp	0x1800f4ded <GetPDFrameWarpContext+0x4ad>
1800f4d63: 48 8d 4d f0                 	leaq	-0x10(%rbp), %rcx
1800f4d67: b8 02 00 00 00              	movl	$0x2, %eax
1800f4d6c: 0f 1f 40 00                 	nopl	(%rax)
1800f4d70: 0f 10 03                    	movups	(%rbx), %xmm0
1800f4d73: 0f 11 01                    	movups	%xmm0, (%rcx)
1800f4d76: 0f 10 4b 10                 	movups	0x10(%rbx), %xmm1
1800f4d7a: 0f 11 49 10                 	movups	%xmm1, 0x10(%rcx)
1800f4d7e: 0f 10 43 20                 	movups	0x20(%rbx), %xmm0
1800f4d82: 0f 11 41 20                 	movups	%xmm0, 0x20(%rcx)
1800f4d86: 0f 10 4b 30                 	movups	0x30(%rbx), %xmm1
1800f4d8a: 0f 11 49 30                 	movups	%xmm1, 0x30(%rcx)
1800f4d8e: 0f 10 43 40                 	movups	0x40(%rbx), %xmm0
1800f4d92: 0f 11 41 40                 	movups	%xmm0, 0x40(%rcx)
1800f4d96: 0f 10 4b 50                 	movups	0x50(%rbx), %xmm1
1800f4d9a: 0f 11 49 50                 	movups	%xmm1, 0x50(%rcx)
1800f4d9e: 0f 10 43 60                 	movups	0x60(%rbx), %xmm0
1800f4da2: 0f 11 41 60                 	movups	%xmm0, 0x60(%rcx)
1800f4da6: 48 8d 89 80 00 00 00        	leaq	0x80(%rcx), %rcx
1800f4dad: 0f 10 4b 70                 	movups	0x70(%rbx), %xmm1
1800f4db1: 0f 11 49 f0                 	movups	%xmm1, -0x10(%rcx)
1800f4db5: 48 8d 9b 80 00 00 00        	leaq	0x80(%rbx), %rbx
1800f4dbc: 48 83 e8 01                 	subq	$0x1, %rax
1800f4dc0: 75 ae                       	jne	0x1800f4d70 <GetPDFrameWarpContext+0x430>
1800f4dc2: 0f 10 03                    	movups	(%rbx), %xmm0
1800f4dc5: 0f 11 01                    	movups	%xmm0, (%rcx)
1800f4dc8: 0f 10 4b 10                 	movups	0x10(%rbx), %xmm1
1800f4dcc: 0f 11 49 10                 	movups	%xmm1, 0x10(%rcx)
1800f4dd0: 0f 10 43 20                 	movups	0x20(%rbx), %xmm0
1800f4dd4: 0f 11 41 20                 	movups	%xmm0, 0x20(%rcx)
1800f4dd8: 48 8b 43 30                 	movq	0x30(%rbx), %rax
1800f4ddc: 48 89 41 30                 	movq	%rax, 0x30(%rcx)
1800f4de0: 48 8d 55 f0                 	leaq	-0x10(%rbp), %rdx
1800f4de4: 48 8b 4e 78                 	movq	0x78(%rsi), %rcx
1800f4de8: e8 23 bd fa ff              	callq	0x1800a0b10 <SetPDFrameWarpNativeCameraSource+0x28280>
1800f4ded: 48 8b 8d 50 01 00 00        	movq	0x150(%rbp), %rcx
1800f4df4: 48 33 cc                    	xorq	%rsp, %rcx
1800f4df7: e8 74 74 01 00              	callq	0x18010c270 <NVSDK_NGX_UpdateFeature+0x2e30>
1800f4dfc: 4c 8d 9c 24 60 02 00 00     	leaq	0x260(%rsp), %r11
1800f4e04: 49 8b 5b 30                 	movq	0x30(%r11), %rbx
1800f4e08: 49 8b 73 38                 	movq	0x38(%r11), %rsi
1800f4e0c: 49 8b e3                    	movq	%r11, %rsp
1800f4e0f: 41 5e                       	popq	%r14
1800f4e11: 5f                          	popq	%rdi
1800f4e12: 5d                          	popq	%rbp
1800f4e13: c3                          	retq
