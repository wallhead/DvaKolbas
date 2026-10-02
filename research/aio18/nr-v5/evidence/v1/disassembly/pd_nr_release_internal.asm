
/mnt/data/aio18_nr_work/PDPerfPlugin.dll:	file format coff-x86-64

Disassembly of section .text:

00000001800f4940 <GetPDFrameWarpContext>:
1800f4e30: 48 89 5c 24 08              	movq	%rbx, 0x8(%rsp)
1800f4e35: 57                          	pushq	%rdi
1800f4e36: 48 83 ec 20                 	subq	$0x20, %rsp
1800f4e3a: 48 8b d9                    	movq	%rcx, %rbx
1800f4e3d: 8b fa                       	movl	%edx, %edi
1800f4e3f: b1 01                       	movb	$0x1, %cl
1800f4e41: e8 5a e7 fd ff              	callq	0x1800d35a0 <SetPDFrameWarpNativeCameraSource+0x5ad10>
1800f4e46: 80 7b 6c 00                 	cmpb	$0x0, 0x6c(%rbx)
1800f4e4a: 74 05                       	je	0x1800f4e51 <GetPDFrameWarpContext+0x511>
1800f4e4c: e8 4f f5 ff ff              	callq	0x1800f43a0 <SetPDFrameWarpNativeCameraSource+0x7bb10>
1800f4e51: 48 8b 4b 78                 	movq	0x78(%rbx), %rcx
1800f4e55: 48 85 c9                    	testq	%rcx, %rcx
1800f4e58: 74 07                       	je	0x1800f4e61 <GetPDFrameWarpContext+0x521>
1800f4e5a: 8b d7                       	movl	%edi, %edx
1800f4e5c: e8 bf ce fa ff              	callq	0x1800a1d20 <SetPDFrameWarpNativeCameraSource+0x29490>
1800f4e61: 48 8b 5c 24 30              	movq	0x30(%rsp), %rbx
1800f4e66: 48 83 c4 20                 	addq	$0x20, %rsp
1800f4e6a: 5f                          	popq	%rdi
1800f4e6b: c3                          	retq
1800f4e6c: cc                          	int3
1800f4e6d: cc                          	int3
1800f4e6e: cc                          	int3
1800f4e6f: cc                          	int3
1800f4e70: 40 53                       	pushq	%rbx
1800f4e72: 48 83 ec 20                 	subq	$0x20, %rsp
1800f4e76: 80 79 6c 00                 	cmpb	$0x0, 0x6c(%rcx)
1800f4e7a: 48 8b da                    	movq	%rdx, %rbx
1800f4e7d: 74 15                       	je	0x1800f4e94 <GetPDFrameWarpContext+0x554>
1800f4e7f: e8 3c 4b f9 ff              	callq	0x1800899c0 <SetPDFrameWarpNativeCameraSource+0x11130>
1800f4e84: 48 8b d3                    	movq	%rbx, %rdx
1800f4e87: 48 8b c8                    	movq	%rax, %rcx
1800f4e8a: 48 83 c4 20                 	addq	$0x20, %rsp
1800f4e8e: 5b                          	popq	%rbx
1800f4e8f: e9 1c c5 f9 ff              	jmp	0x1800913b0 <SetPDFrameWarpNativeCameraSource+0x18b20>
