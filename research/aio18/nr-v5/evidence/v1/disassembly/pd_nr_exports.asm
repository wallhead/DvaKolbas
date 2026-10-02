
/mnt/data/aio18_nr_work/PDPerfPlugin.dll:	file format coff-x86-64

Disassembly of section .text:

00000001800f9f80 <IsDLSSNRAvailable>:
1800f9f80: 48 89 5c 24 08              	movq	%rbx, 0x8(%rsp)
1800f9f85: 48 89 74 24 10              	movq	%rsi, 0x10(%rsp)
1800f9f8a: 57                          	pushq	%rdi
1800f9f8b: 48 81 ec 90 00 00 00        	subq	$0x90, %rsp
1800f9f92: 0f 29 b4 24 80 00 00 00     	movaps	%xmm6, 0x80(%rsp)
1800f9f9a: 48 8b 05 1f 0a 0d 01        	movq	0x10d0a1f(%rip), %rax   # 0x1811ca9c0
1800f9fa1: 48 33 c4                    	xorq	%rsp, %rax
1800f9fa4: 48 89 44 24 78              	movq	%rax, 0x78(%rsp)
1800f9fa9: 33 db                       	xorl	%ebx, %ebx
1800f9fab: b9 20 00 00 00              	movl	$0x20, %ecx
1800f9fb0: 89 5c 24 34                 	movl	%ebx, 0x34(%rsp)
1800f9fb4: 48 89 5c 24 60              	movq	%rbx, 0x60(%rsp)
1800f9fb9: e8 d2 22 01 00              	callq	0x18010c290 <NVSDK_NGX_UpdateFeature+0x2e50>
1800f9fbe: 66 0f 6f 05 2a 2d 0b 01     	movdqa	0x10b2d2a(%rip), %xmm0  # 0x1811accf0
1800f9fc6: 48 8d 4c 24 58              	leaq	0x58(%rsp), %rcx
1800f9fcb: f3 0f 7f 44 24 68           	movdqu	%xmm0, 0x68(%rsp)
1800f9fd1: 48 89 44 24 58              	movq	%rax, 0x58(%rsp)
1800f9fd6: be 01 00 00 00              	movl	$0x1, %esi
1800f9fdb: 0f 10 05 b6 af 0a 01        	movups	0x10aafb6(%rip), %xmm0  # 0x1811a4f98
1800f9fe2: 0f 11 00                    	movups	%xmm0, (%rax)
1800f9fe5: 88 58 10                    	movb	%bl, 0x10(%rax)
1800f9fe8: e8 13 23 00 00              	callq	0x1800fc300 <InitLogDelegate+0x6f0>
1800f9fed: 66 0f 6f 35 9b 2c 0b 01     	movdqa	0x10b2c9b(%rip), %xmm6  # 0x1811acc90
1800f9ff5: 84 c0                       	testb	%al, %al
1800f9ff7: 75 5a                       	jne	0x1800fa053 <IsDLSSNRAvailable+0xd3>
1800f9ff9: b9 20 00 00 00              	movl	$0x20, %ecx
1800f9ffe: 48 89 5c 24 40              	movq	%rbx, 0x40(%rsp)
1800fa003: e8 88 22 01 00              	callq	0x18010c290 <NVSDK_NGX_UpdateFeature+0x2e50>
1800fa008: 66 0f 6f 05 30 2d 0b 01     	movdqa	0x10b2d30(%rip), %xmm0  # 0x1811acd40
1800fa010: be 03 00 00 00              	movl	$0x3, %esi
1800fa015: f3 0f 7f 44 24 48           	movdqu	%xmm0, 0x48(%rsp)
1800fa01b: 48 89 44 24 38              	movq	%rax, 0x38(%rsp)
1800fa020: 0f 10 05 59 af 0a 01        	movups	0x10aaf59(%rip), %xmm0  # 0x1811a4f80
1800fa027: 0f 11 00                    	movups	%xmm0, (%rax)
1800fa02a: 8b 0d 60 af 0a 01           	movl	0x10aaf60(%rip), %ecx   # 0x1811a4f90
1800fa030: 89 48 10                    	movl	%ecx, 0x10(%rax)
1800fa033: 0f b6 0d 5a af 0a 01        	movzbl	0x10aaf5a(%rip), %ecx   # 0x1811a4f94
1800fa03a: 88 48 14                    	movb	%cl, 0x14(%rax)
1800fa03d: 48 8d 4c 24 38              	leaq	0x38(%rsp), %rcx
1800fa042: 88 58 15                    	movb	%bl, 0x15(%rax)
1800fa045: e8 b6 22 00 00              	callq	0x1800fc300 <InitLogDelegate+0x6f0>
1800fa04a: 84 c0                       	testb	%al, %al
1800fa04c: 75 05                       	jne	0x1800fa053 <IsDLSSNRAvailable+0xd3>
1800fa04e: 40 32 ff                    	xorb	%dil, %dil
1800fa051: eb 09                       	jmp	0x1800fa05c <IsDLSSNRAvailable+0xdc>
1800fa053: 40 b7 01                    	movb	$0x1, %dil
1800fa056: 40 f6 c6 02                 	testb	$0x2, %sil
1800fa05a: 74 43                       	je	0x1800fa09f <IsDLSSNRAvailable+0x11f>
1800fa05c: 48 8b 54 24 50              	movq	0x50(%rsp), %rdx
1800fa061: 48 83 fa 0f                 	cmpq	$0xf, %rdx
1800fa065: 76 2e                       	jbe	0x1800fa095 <IsDLSSNRAvailable+0x115>
1800fa067: 48 8b 4c 24 38              	movq	0x38(%rsp), %rcx
1800fa06c: 48 ff c2                    	incq	%rdx
1800fa06f: 48 8b c1                    	movq	%rcx, %rax
1800fa072: 48 81 fa 00 10 00 00        	cmpq	$0x1000, %rdx           # imm = 0x1000
1800fa079: 72 15                       	jb	0x1800fa090 <IsDLSSNRAvailable+0x110>
1800fa07b: 48 8b 49 f8                 	movq	-0x8(%rcx), %rcx
1800fa07f: 48 83 c2 27                 	addq	$0x27, %rdx
1800fa083: 48 2b c1                    	subq	%rcx, %rax
1800fa086: 48 83 e8 08                 	subq	$0x8, %rax
1800fa08a: 48 83 f8 1f                 	cmpq	$0x1f, %rax
1800fa08e: 77 43                       	ja	0x1800fa0d3 <IsDLSSNRAvailable+0x153>
1800fa090: e8 07 23 01 00              	callq	0x18010c39c <NVSDK_NGX_UpdateFeature+0x2f5c>
1800fa095: f3 0f 7f 74 24 48           	movdqu	%xmm6, 0x48(%rsp)
1800fa09b: 88 5c 24 38                 	movb	%bl, 0x38(%rsp)
1800fa09f: 48 8b 54 24 70              	movq	0x70(%rsp), %rdx
1800fa0a4: 48 83 fa 0f                 	cmpq	$0xf, %rdx
1800fa0a8: 76 4c                       	jbe	0x1800fa0f6 <IsDLSSNRAvailable+0x176>
1800fa0aa: 48 8b 4c 24 58              	movq	0x58(%rsp), %rcx
1800fa0af: 48 ff c2                    	incq	%rdx
1800fa0b2: 48 8b c1                    	movq	%rcx, %rax
1800fa0b5: 48 81 fa 00 10 00 00        	cmpq	$0x1000, %rdx           # imm = 0x1000
1800fa0bc: 72 2b                       	jb	0x1800fa0e9 <IsDLSSNRAvailable+0x169>
1800fa0be: 48 8b 49 f8                 	movq	-0x8(%rcx), %rcx
1800fa0c2: 48 83 c2 27                 	addq	$0x27, %rdx
1800fa0c6: 48 2b c1                    	subq	%rcx, %rax
1800fa0c9: 48 83 e8 08                 	subq	$0x8, %rax
1800fa0cd: 48 83 f8 1f                 	cmpq	$0x1f, %rax
1800fa0d1: 76 16                       	jbe	0x1800fa0e9 <IsDLSSNRAvailable+0x169>
1800fa0d3: 45 33 c9                    	xorl	%r9d, %r9d
1800fa0d6: 48 89 5c 24 20              	movq	%rbx, 0x20(%rsp)
1800fa0db: 45 33 c0                    	xorl	%r8d, %r8d
1800fa0de: 33 d2                       	xorl	%edx, %edx
1800fa0e0: 33 c9                       	xorl	%ecx, %ecx
1800fa0e2: ff 15 d0 96 01 00           	callq	*0x196d0(%rip)          # 0x1801137b8
1800fa0e8: cc                          	int3
1800fa0e9: e8 ae 22 01 00              	callq	0x18010c39c <NVSDK_NGX_UpdateFeature+0x2f5c>
1800fa0ee: 66 0f 6f 35 9a 2b 0b 01     	movdqa	0x10b2b9a(%rip), %xmm6  # 0x1811acc90
1800fa0f6: 48 8b 0d e3 58 12 01        	movq	0x11258e3(%rip), %rcx   # 0x18121f9e0
1800fa0fd: 88 5c 24 58                 	movb	%bl, 0x58(%rsp)
1800fa101: f3 0f 7f 74 24 68           	movdqu	%xmm6, 0x68(%rsp)
1800fa107: 48 85 c9                    	testq	%rcx, %rcx
1800fa10a: 74 0e                       	je	0x1800fa11a <IsDLSSNRAvailable+0x19a>
1800fa10c: 83 79 08 02                 	cmpl	$0x2, 0x8(%rcx)
1800fa110: 40 0f b6 c7                 	movzbl	%dil, %eax
1800fa114: 0f 45 d8                    	cmovnel	%eax, %ebx
1800fa117: 0f b6 fb                    	movzbl	%bl, %edi
1800fa11a: 40 0f b6 c7                 	movzbl	%dil, %eax
1800fa11e: 48 8b 4c 24 78              	movq	0x78(%rsp), %rcx
1800fa123: 48 33 cc                    	xorq	%rsp, %rcx
1800fa126: e8 45 21 01 00              	callq	0x18010c270 <NVSDK_NGX_UpdateFeature+0x2e30>
1800fa12b: 4c 8d 9c 24 90 00 00 00     	leaq	0x90(%rsp), %r11
1800fa133: 49 8b 5b 10                 	movq	0x10(%r11), %rbx
1800fa137: 49 8b 73 18                 	movq	0x18(%r11), %rsi
1800fa13b: 41 0f 28 73 f0              	movaps	-0x10(%r11), %xmm6
1800fa140: 49 8b e3                    	movq	%r11, %rsp
1800fa143: 5f                          	popq	%rdi
1800fa144: c3                          	retq
1800fa145: cc                          	int3
1800fa146: cc                          	int3
1800fa147: cc                          	int3
1800fa148: cc                          	int3
1800fa149: cc                          	int3
1800fa14a: cc                          	int3
1800fa14b: cc                          	int3
1800fa14c: cc                          	int3
1800fa14d: cc                          	int3
1800fa14e: cc                          	int3
1800fa14f: cc                          	int3

00000001800fa150 <InitDLSSNR>:
1800fa150: 48 8b d1                    	movq	%rcx, %rdx
1800fa153: 48 8b 0d 86 58 12 01        	movq	0x1125886(%rip), %rcx   # 0x18121f9e0
1800fa15a: 48 85 c9                    	testq	%rcx, %rcx
1800fa15d: 74 0f                       	je	0x1800fa16e <InitDLSSNR+0x1e>
1800fa15f: 48 85 d2                    	testq	%rdx, %rdx
1800fa162: 74 0a                       	je	0x1800fa16e <InitDLSSNR+0x1e>
1800fa164: 48 8b 01                    	movq	(%rcx), %rax
1800fa167: 48 ff a0 f0 00 00 00        	jmpq	*0xf0(%rax)
1800fa16e: 32 c0                       	xorb	%al, %al
1800fa170: c3                          	retq
1800fa171: cc                          	int3
1800fa172: cc                          	int3
1800fa173: cc                          	int3
1800fa174: cc                          	int3
1800fa175: cc                          	int3
1800fa176: cc                          	int3
1800fa177: cc                          	int3
1800fa178: cc                          	int3
1800fa179: cc                          	int3
1800fa17a: cc                          	int3
1800fa17b: cc                          	int3
1800fa17c: cc                          	int3
1800fa17d: cc                          	int3
1800fa17e: cc                          	int3
1800fa17f: cc                          	int3

00000001800fa180 <EvaluateDLSSNR>:
1800fa180: 48 81 ec 68 01 00 00        	subq	$0x168, %rsp            # imm = 0x168
1800fa187: 4c 8b 05 52 58 12 01        	movq	0x1125852(%rip), %r8    # 0x18121f9e0
1800fa18e: 4d 85 c0                    	testq	%r8, %r8
1800fa191: 0f 84 9b 00 00 00           	je	0x1800fa232 <EvaluateDLSSNR+0xb2>
1800fa197: 48 85 c9                    	testq	%rcx, %rcx
1800fa19a: 0f 84 92 00 00 00           	je	0x1800fa232 <EvaluateDLSSNR+0xb2>
1800fa1a0: 48 8d 54 24 20              	leaq	0x20(%rsp), %rdx
1800fa1a5: b8 02 00 00 00              	movl	$0x2, %eax
1800fa1aa: 66 0f 1f 44 00 00           	nopw	(%rax,%rax)
1800fa1b0: 48 8d 92 80 00 00 00        	leaq	0x80(%rdx), %rdx
1800fa1b7: 0f 10 01                    	movups	(%rcx), %xmm0
1800fa1ba: 0f 10 49 10                 	movups	0x10(%rcx), %xmm1
1800fa1be: 48 8d 89 80 00 00 00        	leaq	0x80(%rcx), %rcx
1800fa1c5: 0f 11 42 80                 	movups	%xmm0, -0x80(%rdx)
1800fa1c9: 0f 10 41 a0                 	movups	-0x60(%rcx), %xmm0
1800fa1cd: 0f 11 4a 90                 	movups	%xmm1, -0x70(%rdx)
1800fa1d1: 0f 10 49 b0                 	movups	-0x50(%rcx), %xmm1
1800fa1d5: 0f 11 42 a0                 	movups	%xmm0, -0x60(%rdx)
1800fa1d9: 0f 10 41 c0                 	movups	-0x40(%rcx), %xmm0
1800fa1dd: 0f 11 4a b0                 	movups	%xmm1, -0x50(%rdx)
1800fa1e1: 0f 10 49 d0                 	movups	-0x30(%rcx), %xmm1
1800fa1e5: 0f 11 42 c0                 	movups	%xmm0, -0x40(%rdx)
1800fa1e9: 0f 10 41 e0                 	movups	-0x20(%rcx), %xmm0
1800fa1ed: 0f 11 4a d0                 	movups	%xmm1, -0x30(%rdx)
1800fa1f1: 0f 10 49 f0                 	movups	-0x10(%rcx), %xmm1
1800fa1f5: 0f 11 42 e0                 	movups	%xmm0, -0x20(%rdx)
1800fa1f9: 0f 11 4a f0                 	movups	%xmm1, -0x10(%rdx)
1800fa1fd: 48 83 e8 01                 	subq	$0x1, %rax
1800fa201: 75 ad                       	jne	0x1800fa1b0 <EvaluateDLSSNR+0x30>
1800fa203: 0f 10 01                    	movups	(%rcx), %xmm0
1800fa206: 48 8b 41 30                 	movq	0x30(%rcx), %rax
1800fa20a: 0f 10 49 10                 	movups	0x10(%rcx), %xmm1
1800fa20e: 0f 11 02                    	movups	%xmm0, (%rdx)
1800fa211: 0f 10 41 20                 	movups	0x20(%rcx), %xmm0
1800fa215: 49 8b c8                    	movq	%r8, %rcx
1800fa218: 0f 11 4a 10                 	movups	%xmm1, 0x10(%rdx)
1800fa21c: 0f 11 42 20                 	movups	%xmm0, 0x20(%rdx)
1800fa220: 48 89 42 30                 	movq	%rax, 0x30(%rdx)
1800fa224: 48 8d 54 24 20              	leaq	0x20(%rsp), %rdx
1800fa229: 49 8b 00                    	movq	(%r8), %rax
1800fa22c: ff 90 f8 00 00 00           	callq	*0xf8(%rax)
1800fa232: 48 81 c4 68 01 00 00        	addq	$0x168, %rsp            # imm = 0x168
1800fa239: c3                          	retq
1800fa23a: cc                          	int3
1800fa23b: cc                          	int3
1800fa23c: cc                          	int3
1800fa23d: cc                          	int3
1800fa23e: cc                          	int3
1800fa23f: cc                          	int3

00000001800fa240 <ReleaseDLSSNR>:
1800fa240: 8b d1                       	movl	%ecx, %edx
1800fa242: 48 8b 0d 97 57 12 01        	movq	0x1125797(%rip), %rcx   # 0x18121f9e0
1800fa249: 48 85 c9                    	testq	%rcx, %rcx
1800fa24c: 74 0a                       	je	0x1800fa258 <ReleaseDLSSNR+0x18>
1800fa24e: 48 8b 01                    	movq	(%rcx), %rax
1800fa251: 48 ff a0 00 01 00 00        	jmpq	*0x100(%rax)
1800fa258: c3                          	retq
1800fa259: cc                          	int3
1800fa25a: cc                          	int3
1800fa25b: cc                          	int3
1800fa25c: cc                          	int3
1800fa25d: cc                          	int3
1800fa25e: cc                          	int3
1800fa25f: cc                          	int3
