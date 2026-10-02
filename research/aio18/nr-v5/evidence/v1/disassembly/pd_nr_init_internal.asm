
/mnt/data/aio18_nr_work/PDPerfPlugin.dll:	file format coff-x86-64

Disassembly of section .text:

00000001800f4940 <GetPDFrameWarpContext>:
1800f4960: 48 89 5c 24 10              	movq	%rbx, 0x10(%rsp)
1800f4965: 48 89 6c 24 18              	movq	%rbp, 0x18(%rsp)
1800f496a: 48 89 74 24 20              	movq	%rsi, 0x20(%rsp)
1800f496f: 57                          	pushq	%rdi
1800f4970: 48 83 ec 30                 	subq	$0x30, %rsp
1800f4974: 48 8b f2                    	movq	%rdx, %rsi
1800f4977: 48 8b f9                    	movq	%rcx, %rdi
1800f497a: 33 ed                       	xorl	%ebp, %ebp
1800f497c: e8 5f ec fd ff              	callq	0x1800d35e0 <SetPDFrameWarpNativeCameraSource+0x5ad50>
1800f4981: 40 38 6f 6c                 	cmpb	%bpl, 0x6c(%rdi)
1800f4985: 74 05                       	je	0x1800f498c <GetPDFrameWarpContext+0x4c>
1800f4987: e8 14 fa ff ff              	callq	0x1800f43a0 <SetPDFrameWarpNativeCameraSource+0x7bb10>
1800f498c: 48 8b 07                    	movq	(%rdi), %rax
1800f498f: 48 8b cf                    	movq	%rdi, %rcx
1800f4992: ff 90 08 01 00 00           	callq	*0x108(%rax)
1800f4998: 48 8b d0                    	movq	%rax, %rdx
1800f499b: 48 8b 0d 66 1a 12 01        	movq	0x1121a66(%rip), %rcx   # 0x181216408
1800f49a2: e8 59 a6 f7 ff              	callq	0x18006f000 <SetPDFrameWarpDiagnosticHud+0x465b0>
1800f49a7: 8b 56 20                    	movl	0x20(%rsi), %edx
1800f49aa: 48 8b 0d 57 1a 12 01        	movq	0x1121a57(%rip), %rcx   # 0x181216408
1800f49b1: e8 6a a7 f7 ff              	callq	0x18006f120 <SetPDFrameWarpDiagnosticHud+0x466d0>
1800f49b6: 44 0f b6 c0                 	movzbl	%al, %r8d
1800f49ba: 41 0f b6 d8                 	movzbl	%r8b, %ebx
1800f49be: 80 f3 01                    	xorb	$0x1, %bl
1800f49c1: 48 8b 0d 40 1a 12 01        	movq	0x1121a40(%rip), %rcx   # 0x181216408
1800f49c8: 88 99 25 09 00 00           	movb	%bl, 0x925(%rcx)
1800f49ce: 48 8d 15 83 6c 0b 01        	leaq	0x10b6c83(%rip), %rdx   # 0x1811ab658
1800f49d5: 48 8d 05 cc 6b 0b 01        	leaq	0x10b6bcc(%rip), %rax   # 0x1811ab5a8
1800f49dc: 48 0f 45 c2                 	cmovneq	%rdx, %rax
1800f49e0: 48 39 a9 98 00 00 00        	cmpq	%rbp, 0x98(%rcx)
1800f49e7: 40 0f 95 c5                 	setne	%bpl
1800f49eb: 48 89 44 24 20              	movq	%rax, 0x20(%rsp)
1800f49f0: 44 8b cd                    	movl	%ebp, %r9d
1800f49f3: 8b 56 20                    	movl	0x20(%rsi), %edx
1800f49f6: 48 8d 0d c3 6b 0b 01        	leaq	0x10b6bc3(%rip), %rcx   # 0x1811ab5c0
1800f49fd: e8 3e 71 00 00              	callq	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
1800f4a02: 48 8b 0d ff 19 12 01        	movq	0x11219ff(%rip), %rcx   # 0x181216408
1800f4a09: 84 db                       	testb	%bl, %bl
1800f4a0b: 74 2c                       	je	0x1800f4a39 <GetPDFrameWarpContext+0xf9>
1800f4a0d: e8 0e a9 f7 ff              	callq	0x18006f320 <SetPDFrameWarpDiagnosticHud+0x468d0>
1800f4a12: 84 c0                       	testb	%al, %al
1800f4a14: 75 13                       	jne	0x1800f4a29 <GetPDFrameWarpContext+0xe9>
1800f4a16: 48 8d 0d b3 6c 0b 01        	leaq	0x10b6cb3(%rip), %rcx   # 0x1811ab6d0
1800f4a1d: e8 1e 71 00 00              	callq	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
1800f4a22: 32 c0                       	xorb	%al, %al
1800f4a24: e9 a2 00 00 00              	jmp	0x1800f4acb <GetPDFrameWarpContext+0x18b>
1800f4a29: 48 8b 05 d8 19 12 01        	movq	0x11219d8(%rip), %rax   # 0x181216408
1800f4a30: 48 8b 98 20 0b 00 00        	movq	0xb20(%rax), %rbx
1800f4a37: eb 2a                       	jmp	0x1800f4a63 <GetPDFrameWarpContext+0x123>
1800f4a39: 8b 56 20                    	movl	0x20(%rsi), %edx
1800f4a3c: e8 3f ac f7 ff              	callq	0x18006f680 <SetPDFrameWarpDiagnosticHud+0x46c30>
1800f4a41: 84 c0                       	testb	%al, %al
1800f4a43: 75 10                       	jne	0x1800f4a55 <GetPDFrameWarpContext+0x115>
1800f4a45: 48 8d 0d c4 6c 0b 01        	leaq	0x10b6cc4(%rip), %rcx   # 0x1811ab710
1800f4a4c: e8 ef 70 00 00              	callq	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
1800f4a51: 32 c0                       	xorb	%al, %al
1800f4a53: eb 76                       	jmp	0x1800f4acb <GetPDFrameWarpContext+0x18b>
1800f4a55: 48 8b 05 ac 19 12 01        	movq	0x11219ac(%rip), %rax   # 0x181216408
1800f4a5c: 48 8b 98 a8 00 00 00        	movq	0xa8(%rax), %rbx
1800f4a63: 48 85 db                    	testq	%rbx, %rbx
1800f4a66: 75 10                       	jne	0x1800f4a78 <GetPDFrameWarpContext+0x138>
1800f4a68: 48 8d 0d 01 6c 0b 01        	leaq	0x10b6c01(%rip), %rcx   # 0x1811ab670
1800f4a6f: e8 cc 70 00 00              	callq	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
1800f4a74: 32 c0                       	xorb	%al, %al
1800f4a76: eb 53                       	jmp	0x1800f4acb <GetPDFrameWarpContext+0x18b>
1800f4a78: 48 83 7f 78 00              	cmpq	$0x0, 0x78(%rdi)
1800f4a7d: 75 40                       	jne	0x1800f4abf <GetPDFrameWarpContext+0x17f>
1800f4a7f: b9 48 07 00 00              	movl	$0x748, %ecx            # imm = 0x748
1800f4a84: e8 07 78 01 00              	callq	0x18010c290 <NVSDK_NGX_UpdateFeature+0x2e50>
1800f4a89: 48 89 44 24 40              	movq	%rax, 0x40(%rsp)
1800f4a8e: 41 b0 01                    	movb	$0x1, %r8b
1800f4a91: 48 8b d3                    	movq	%rbx, %rdx
1800f4a94: 48 8b c8                    	movq	%rax, %rcx
1800f4a97: e8 64 6d fa ff              	callq	0x18009b800 <SetPDFrameWarpNativeCameraSource+0x22f70>
1800f4a9c: 90                          	nop
1800f4a9d: 48 8b 5f 78                 	movq	0x78(%rdi), %rbx
1800f4aa1: 48 89 47 78                 	movq	%rax, 0x78(%rdi)
1800f4aa5: 48 85 db                    	testq	%rbx, %rbx
1800f4aa8: 74 15                       	je	0x1800f4abf <GetPDFrameWarpContext+0x17f>
1800f4aaa: 48 8b cb                    	movq	%rbx, %rcx
1800f4aad: e8 5e 73 fa ff              	callq	0x18009be10 <SetPDFrameWarpNativeCameraSource+0x23580>
1800f4ab2: ba 48 07 00 00              	movl	$0x748, %edx            # imm = 0x748
1800f4ab7: 48 8b cb                    	movq	%rbx, %rcx
1800f4aba: e8 dd 78 01 00              	callq	0x18010c39c <NVSDK_NGX_UpdateFeature+0x2f5c>
1800f4abf: 48 8b d6                    	movq	%rsi, %rdx
1800f4ac2: 48 8b 4f 78                 	movq	0x78(%rdi), %rcx
1800f4ac6: e8 d5 7e fa ff              	callq	0x18009c9a0 <SetPDFrameWarpNativeCameraSource+0x24110>
1800f4acb: 48 8b 5c 24 48              	movq	0x48(%rsp), %rbx
1800f4ad0: 48 8b 6c 24 50              	movq	0x50(%rsp), %rbp
1800f4ad5: 48 8b 74 24 58              	movq	0x58(%rsp), %rsi
1800f4ada: 48 83 c4 30                 	addq	$0x30, %rsp
1800f4ade: 5f                          	popq	%rdi
1800f4adf: c3                          	retq
