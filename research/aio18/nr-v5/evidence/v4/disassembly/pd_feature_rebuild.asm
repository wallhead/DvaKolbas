
/mnt/data/aio18_nr_work/PDPerfPlugin.dll:	file format coff-x86-64

Disassembly of section .text:

0000000180078890 <SetPDFrameWarpNativeCameraSource>:
18009c870: 48 89 5c 24 08              	movq	%rbx, 0x8(%rsp)
18009c875: 48 89 6c 24 10              	movq	%rbp, 0x10(%rsp)
18009c87a: 48 89 74 24 18              	movq	%rsi, 0x18(%rsp)
18009c87f: 48 89 7c 24 20              	movq	%rdi, 0x20(%rsp)
18009c884: 41 56                       	pushq	%r14
18009c886: 48 83 ec 40                 	subq	$0x40, %rsp
18009c88a: 0f 29 74 24 30              	movaps	%xmm6, 0x30(%rsp)
18009c88f: 41 8b f9                    	movl	%r9d, %edi
18009c892: 41 8b f0                    	movl	%r8d, %esi
18009c895: 4c 8b f2                    	movq	%rdx, %r14
18009c898: 48 8b d9                    	movq	%rcx, %rbx
18009c89b: 45 85 c0                    	testl	%r8d, %r8d
18009c89e: 0f 8e cb 00 00 00           	jle	0x18009c96f <SetPDFrameWarpNativeCameraSource+0x240df>
18009c8a4: 45 85 c9                    	testl	%r9d, %r9d
18009c8a7: 0f 8e c2 00 00 00           	jle	0x18009c96f <SetPDFrameWarpNativeCameraSource+0x240df>
18009c8ad: e8 ae 50 00 00              	callq	0x1800a1960 <SetPDFrameWarpNativeCameraSource+0x290d0>
18009c8b2: 41 8b 16                    	movl	(%r14), %edx
18009c8b5: 48 8b cb                    	movq	%rbx, %rcx
18009c8b8: e8 b3 5c 00 00              	callq	0x1800a2570 <SetPDFrameWarpNativeCameraSource+0x29ce0>
18009c8bd: 84 c0                       	testb	%al, %al
18009c8bf: 74 05                       	je	0x18009c8c6 <SetPDFrameWarpNativeCameraSource+0x24036>
18009c8c1: e8 5a 54 00 00              	callq	0x1800a1d20 <SetPDFrameWarpNativeCameraSource+0x29490>
18009c8c6: f3 0f 10 74 24 70           	movss	0x70(%rsp), %xmm6
18009c8cc: 0f 57 c0                    	xorps	%xmm0, %xmm0
18009c8cf: 0f 2f c6                    	comiss	%xmm6, %xmm0
18009c8d2: 72 08                       	jb	0x18009c8dc <SetPDFrameWarpNativeCameraSource+0x2404c>
18009c8d4: f3 0f 10 35 f4 01 11 01     	movss	0x11101f4(%rip), %xmm6  # 0x1811acad0
18009c8dc: 66 0f 6e c6                 	movd	%esi, %xmm0
18009c8e0: 0f 5b c0                    	cvtdq2ps	%xmm0, %xmm0
18009c8e3: f3 0f 11 b3 08 01 00 00     	movss	%xmm6, 0x108(%rbx)
18009c8eb: f3 0f 5e c6                 	divss	%xmm6, %xmm0
18009c8ef: ff 15 2b 6e 07 00           	callq	*0x76e2b(%rip)          # 0x180113720
18009c8f5: f3 0f 2c c0                 	cvttss2si	%xmm0, %eax
18009c8f9: 66 0f 6e c7                 	movd	%edi, %xmm0
18009c8fd: 0f 5b c0                    	cvtdq2ps	%xmm0, %xmm0
18009c900: 89 83 00 01 00 00           	movl	%eax, 0x100(%rbx)
18009c906: f3 0f 5e c6                 	divss	%xmm6, %xmm0
18009c90a: ff 15 10 6e 07 00           	callq	*0x76e10(%rip)          # 0x180113720
18009c910: 0f 28 de                    	movaps	%xmm6, %xmm3
18009c913: 89 b3 0c 01 00 00           	movl	%esi, 0x10c(%rbx)
18009c919: f3 0f 2c c0                 	cvttss2si	%xmm0, %eax
18009c91d: 44 8b c7                    	movl	%edi, %r8d
18009c920: 89 bb 10 01 00 00           	movl	%edi, 0x110(%rbx)
18009c926: 8b d6                       	movl	%esi, %edx
18009c928: 48 8b cb                    	movq	%rbx, %rcx
18009c92b: 89 83 04 01 00 00           	movl	%eax, 0x104(%rbx)
18009c931: e8 da fb ff ff              	callq	0x18009c510 <SetPDFrameWarpNativeCameraSource+0x23c80>
18009c936: 48 8b e8                    	movq	%rax, %rbp
18009c939: 48 85 c0                    	testq	%rax, %rax
18009c93c: 74 31                       	je	0x18009c96f <SetPDFrameWarpNativeCameraSource+0x240df>
18009c93e: 41 8b 16                    	movl	(%r14), %edx
18009c941: 48 8d 0d 40 78 10 01        	leaq	0x1107840(%rip), %rcx   # 0x1811a4188
18009c948: 0f 5a c6                    	cvtps2pd	%xmm6, %xmm0
18009c94b: 44 8b cf                    	movl	%edi, %r9d
18009c94e: 44 8b c6                    	movl	%esi, %r8d
18009c951: f2 0f 11 44 24 20           	movsd	%xmm0, 0x20(%rsp)
18009c957: e8 e4 f1 05 00              	callq	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
18009c95c: 48 8d 4b 38                 	leaq	0x38(%rbx), %rcx
18009c960: 49 8b d6                    	movq	%r14, %rdx
18009c963: e8 08 55 ff ff              	callq	0x180091e70 <SetPDFrameWarpNativeCameraSource+0x195e0>
18009c968: 48 89 28                    	movq	%rbp, (%rax)
18009c96b: b0 01                       	movb	$0x1, %al
18009c96d: eb 02                       	jmp	0x18009c971 <SetPDFrameWarpNativeCameraSource+0x240e1>
18009c96f: 32 c0                       	xorb	%al, %al
18009c971: 48 8b 5c 24 50              	movq	0x50(%rsp), %rbx
18009c976: 48 8b 6c 24 58              	movq	0x58(%rsp), %rbp
18009c97b: 48 8b 74 24 60              	movq	0x60(%rsp), %rsi
18009c980: 48 8b 7c 24 68              	movq	0x68(%rsp), %rdi
18009c985: 0f 28 74 24 30              	movaps	0x30(%rsp), %xmm6
18009c98a: 48 83 c4 40                 	addq	$0x40, %rsp
18009c98e: 41 5e                       	popq	%r14
18009c990: c3                          	retq
18009c991: cc                          	int3
18009c992: cc                          	int3
18009c993: cc                          	int3
18009c994: cc                          	int3
18009c995: cc                          	int3
18009c996: cc                          	int3
18009c997: cc                          	int3
18009c998: cc                          	int3
18009c999: cc                          	int3
18009c99a: cc                          	int3
18009c99b: cc                          	int3
18009c99c: cc                          	int3
18009c99d: cc                          	int3
18009c99e: cc                          	int3
18009c99f: cc                          	int3
