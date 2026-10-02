
/mnt/data/aio18_nr_work/PDPerfPlugin.dll:	file format coff-x86-64

Disassembly of section .text:

0000000180078890 <SetPDFrameWarpNativeCameraSource>:
18009f340: 44 01 00                    	addl	%r8d, (%rax)
18009f343: 00 48 8b                    	addb	%cl, -0x75(%rax)
18009f346: 83 70 01 00                 	xorl	$0x0, 0x1(%rax)
18009f34a: 00 48 8d                    	addb	%cl, -0x73(%rax)
18009f34d: b3 c8                       	movb	$-0x38, %bl
18009f34f: 00 00                       	addb	%al, (%rax)
18009f351: 00 48 89                    	addb	%cl, -0x77(%rax)
18009f354: 45 b0 4c                    	movb	$0x4c, %r8b
18009f357: 8d 63 48                    	leal	0x48(%rbx), %esp
18009f35a: 48 8b 83 a8 01 00 00        	movq	0x1a8(%rbx), %rax
18009f361: 48 89 45 b8                 	movq	%rax, -0x48(%rbp)
18009f365: 33 c0                       	xorl	%eax, %eax
18009f367: 89 44 24 70                 	movl	%eax, 0x70(%rsp)
18009f36b: 33 ff                       	xorl	%edi, %edi
18009f36d: 0f 1f 00                    	nopl	(%rax)
18009f370: 8b cf                       	movl	%edi, %ecx
18009f372: 85 ff                       	testl	%edi, %edi
18009f374: 75 11                       	jne	0x18009f387 <SetPDFrameWarpNativeCameraSource+0x26af7>
18009f376: 48 8b 55 20                 	movq	0x20(%rbp), %rdx
18009f37a: 4d 8d 8d 00 01 00 00        	leaq	0x100(%r13), %r9
18009f381: 4c 8b 45 c0                 	movq	-0x40(%rbp), %r8
18009f385: eb 1b                       	jmp	0x18009f3a2 <SetPDFrameWarpNativeCameraSource+0x26b12>
18009f387: 4d 8b 04 24                 	movq	(%r12), %r8
18009f38b: 4d 85 c0                    	testq	%r8, %r8
18009f38e: 0f 84 b7 00 00 00           	je	0x18009f44b <SetPDFrameWarpNativeCameraSource+0x26bbb>
18009f394: 8d 47 ff                    	leal	-0x1(%rdi), %eax
18009f397: 4c 8b ce                    	movq	%rsi, %r9
18009f39a: 83 e0 01                    	andl	$0x1, %eax
18009f39d: 48 8b 54 c5 b0              	movq	-0x50(%rbp,%rax,8), %rdx
18009f3a2: 41 0f b6 01                 	movzbl	(%r9), %eax
18009f3a6: 83 e1 01                    	andl	$0x1, %ecx
18009f3a9: 88 44 24 60                 	movb	%al, 0x60(%rsp)
18009f3ad: 4d 8b cd                    	movq	%r13, %r9
18009f3b0: 48 8b 45 80                 	movq	-0x80(%rbp), %rax
18009f3b4: 48 89 44 24 58              	movq	%rax, 0x58(%rsp)
18009f3b9: 4c 8b 54 cd b0              	movq	-0x50(%rbp,%rcx,8), %r10
18009f3be: 48 8b cb                    	movq	%rbx, %rcx
18009f3c1: 48 8b 45 88                 	movq	-0x78(%rbp), %rax
18009f3c5: 48 89 44 24 50              	movq	%rax, 0x50(%rsp)
18009f3ca: 48 8b 45 90                 	movq	-0x70(%rbp), %rax
18009f3ce: 48 89 44 24 48              	movq	%rax, 0x48(%rsp)
18009f3d3: 48 8b 45 98                 	movq	-0x68(%rbp), %rax
18009f3d7: 48 89 44 24 40              	movq	%rax, 0x40(%rsp)
18009f3dc: 48 8b 45 c8                 	movq	-0x38(%rbp), %rax
18009f3e0: 48 89 44 24 38              	movq	%rax, 0x38(%rsp)
18009f3e5: 48 8d 45 10                 	leaq	0x10(%rbp), %rax
18009f3e9: 4c 89 54 24 30              	movq	%r10, 0x30(%rsp)
18009f3ee: 48 89 54 24 28              	movq	%rdx, 0x28(%rsp)
18009f3f3: 49 8b d6                    	movq	%r14, %rdx
18009f3f6: 48 89 44 24 20              	movq	%rax, 0x20(%rsp)
18009f3fb: e8 a0 f1 ff ff              	callq	0x18009e5a0 <SetPDFrameWarpNativeCameraSource+0x25d10>
18009f400: 84 c0                       	testb	%al, %al
18009f402: 74 23                       	je	0x18009f427 <SetPDFrameWarpNativeCameraSource+0x26b97>
18009f404: 85 ff                       	testl	%edi, %edi
18009f406: 74 03                       	je	0x18009f40b <SetPDFrameWarpNativeCameraSource+0x26b7b>
18009f408: c6 06 00                    	movb	$0x0, (%rsi)
18009f40b: 8d 47 01                    	leal	0x1(%rdi), %eax
18009f40e: 49 83 c4 08                 	addq	$0x8, %r12
18009f412: 48 ff c6                    	incq	%rsi
18009f415: 89 44 24 70                 	movl	%eax, 0x70(%rsp)
18009f419: 8b f8                       	movl	%eax, %edi
18009f41b: 3b 44 24 74                 	cmpl	0x74(%rsp), %eax
18009f41f: 0f 82 4b ff ff ff           	jb	0x18009f370 <SetPDFrameWarpNativeCameraSource+0x26ae0>
18009f425: eb 24                       	jmp	0x18009f44b <SetPDFrameWarpNativeCameraSource+0x26bbb>
18009f427: 85 ff                       	testl	%edi, %edi
18009f429: 0f 84 f2 03 00 00           	je	0x18009f821 <SetPDFrameWarpNativeCameraSource+0x26f91>
18009f42f: 8d 57 01                    	leal	0x1(%rdi), %edx
18009f432: 44 8b c7                    	movl	%edi, %r8d
18009f435: 48 8d 0d ec 52 10 01        	leaq	0x11052ec(%rip), %rcx   # 0x1811a4728
18009f43c: e8 ff c6 05 00              	callq	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
18009f441: 8b 44 24 70                 	movl	0x70(%rsp), %eax
18009f445: 89 bb d8 00 00 00           	movl	%edi, 0xd8(%rbx)
18009f44b: 85 c0                       	testl	%eax, %eax
18009f44d: 0f 84 ce 03 00 00           	je	0x18009f821 <SetPDFrameWarpNativeCameraSource+0x26f91>
18009f453: ff c8                       	decl	%eax
18009f455: 4c 89 7c 24 28              	movq	%r15, 0x28(%rsp)
18009f45a: 83 e0 01                    	andl	$0x1, %eax
18009f45d: 4c 8d 4d 10                 	leaq	0x10(%rbp), %r9
