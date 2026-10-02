
/mnt/data/aio18_nr_work/PDPerfPlugin.dll:	file format coff-x86-64

Disassembly of section .text:

0000000180078890 <SetPDFrameWarpNativeCameraSource>:
1800a1e40: 48 89 5c 24 18              	movq	%rbx, 0x18(%rsp)
1800a1e45: 55                          	pushq	%rbp
1800a1e46: 56                          	pushq	%rsi
1800a1e47: 57                          	pushq	%rdi
1800a1e48: 41 54                       	pushq	%r12
1800a1e4a: 41 55                       	pushq	%r13
1800a1e4c: 41 56                       	pushq	%r14
1800a1e4e: 41 57                       	pushq	%r15
1800a1e50: 48 83 ec 30                 	subq	$0x30, %rsp
1800a1e54: 48 8b e9                    	movq	%rcx, %rbp
1800a1e57: 66 c7 81 b1 01 00 00 00 00  	movw	$0x0, 0x1b1(%rcx)
1800a1e60: 48 8b 3d 89 3a 17 01        	movq	0x1173a89(%rip), %rdi   # 0x1812158f0
1800a1e67: 48 81 c7 78 0a 00 00        	addq	$0xa78, %rdi            # imm = 0xA78
1800a1e6e: 48 89 7c 24 70              	movq	%rdi, 0x70(%rsp)
1800a1e73: 48 8b cf                    	movq	%rdi, %rcx
1800a1e76: ff 15 bc 16 07 00           	callq	*0x716bc(%rip)          # 0x180113538
1800a1e7c: 85 c0                       	testl	%eax, %eax
1800a1e7e: 74 0c                       	je	0x1800a1e8c <SetPDFrameWarpNativeCameraSource+0x295fc>
1800a1e80: b9 05 00 00 00              	movl	$0x5, %ecx
1800a1e85: ff 15 b5 16 07 00           	callq	*0x716b5(%rip)          # 0x180113540
1800a1e8b: cc                          	int3
1800a1e8c: 81 7f 4c ff ff ff 7f        	cmpl	$0x7fffffff, 0x4c(%rdi) # imm = 0x7FFFFFFF
1800a1e93: 75 13                       	jne	0x1800a1ea8 <SetPDFrameWarpNativeCameraSource+0x29618>
1800a1e95: c7 47 4c fe ff ff 7f        	movl	$0x7ffffffe, 0x4c(%rdi) # imm = 0x7FFFFFFE
1800a1e9c: b9 06 00 00 00              	movl	$0x6, %ecx
1800a1ea1: ff 15 99 16 07 00           	callq	*0x71699(%rip)          # 0x180113540
1800a1ea7: 90                          	nop
1800a1ea8: 48 8b 35 41 3a 17 01        	movq	0x1173a41(%rip), %rsi   # 0x1812158f0
1800a1eaf: 48 81 c6 c8 0a 00 00        	addq	$0xac8, %rsi            # imm = 0xAC8
1800a1eb6: 48 89 74 24 78              	movq	%rsi, 0x78(%rsp)
1800a1ebb: 48 8b ce                    	movq	%rsi, %rcx
1800a1ebe: ff 15 74 16 07 00           	callq	*0x71674(%rip)          # 0x180113538
1800a1ec4: 85 c0                       	testl	%eax, %eax
1800a1ec6: 74 0c                       	je	0x1800a1ed4 <SetPDFrameWarpNativeCameraSource+0x29644>
1800a1ec8: b9 05 00 00 00              	movl	$0x5, %ecx
1800a1ecd: ff 15 6d 16 07 00           	callq	*0x7166d(%rip)          # 0x180113540
1800a1ed3: cc                          	int3
1800a1ed4: 81 7e 4c ff ff ff 7f        	cmpl	$0x7fffffff, 0x4c(%rsi) # imm = 0x7FFFFFFF
1800a1edb: 75 13                       	jne	0x1800a1ef0 <SetPDFrameWarpNativeCameraSource+0x29660>
1800a1edd: c7 46 4c fe ff ff 7f        	movl	$0x7ffffffe, 0x4c(%rsi) # imm = 0x7FFFFFFE
1800a1ee4: b9 06 00 00 00              	movl	$0x6, %ecx
1800a1ee9: ff 15 51 16 07 00           	callq	*0x71651(%rip)          # 0x180113540
1800a1eef: 90                          	nop
1800a1ef0: 48 8b 0d f9 39 17 01        	movq	0x11739f9(%rip), %rcx   # 0x1812158f0
1800a1ef7: e8 c4 dd fc ff              	callq	0x18006fcc0 <SetPDFrameWarpDiagnosticHud+0x47270>
1800a1efc: 84 c0                       	testb	%al, %al
1800a1efe: 75 08                       	jne	0x1800a1f08 <SetPDFrameWarpNativeCameraSource+0x29678>
1800a1f00: 48 8b cd                    	movq	%rbp, %rcx
1800a1f03: e8 68 f8 ff ff              	callq	0x1800a1770 <SetPDFrameWarpNativeCameraSource+0x28ee0>
1800a1f08: 41 bc 01 00 00 00           	movl	$0x1, %r12d
1800a1f0e: 41 8b dc                    	movl	%r12d, %ebx
1800a1f11: 8b d3                       	movl	%ebx, %edx
1800a1f13: 48 8b cd                    	movq	%rbp, %rcx
1800a1f16: e8 95 f9 ff ff              	callq	0x1800a18b0 <SetPDFrameWarpNativeCameraSource+0x29020>
1800a1f1b: ff c3                       	incl	%ebx
1800a1f1d: 83 fb 10                    	cmpl	$0x10, %ebx
1800a1f20: 72 ef                       	jb	0x1800a1f11 <SetPDFrameWarpNativeCameraSource+0x29681>
1800a1f22: c7 85 d8 00 00 00 10 00 00 00       	movl	$0x10, 0xd8(%rbp)
1800a1f2c: 48 8b 5d 38                 	movq	0x38(%rbp), %rbx
1800a1f30: 48 8b 1b                    	movq	(%rbx), %rbx
1800a1f33: 80 7b 19 00                 	cmpb	$0x0, 0x19(%rbx)
1800a1f37: 75 6c                       	jne	0x1800a1fa5 <SetPDFrameWarpNativeCameraSource+0x29715>
1800a1f39: 0f 1f 80 00 00 00 00        	nopl	(%rax)
1800a1f40: 48 8b 4b 28                 	movq	0x28(%rbx), %rcx
1800a1f44: 48 85 c9                    	testq	%rcx, %rcx
1800a1f47: 74 05                       	je	0x1800a1f4e <SetPDFrameWarpNativeCameraSource+0x296be>
1800a1f49: e8 12 22 00 00              	callq	0x1800a4160 <SetPDFrameWarpNativeCameraSource+0x2b8d0>
1800a1f4e: 48 8b 4b 10                 	movq	0x10(%rbx), %rcx
1800a1f52: 80 79 19 00                 	cmpb	$0x0, 0x19(%rcx)
1800a1f56: 74 22                       	je	0x1800a1f7a <SetPDFrameWarpNativeCameraSource+0x296ea>
1800a1f58: 48 8b 43 08                 	movq	0x8(%rbx), %rax
1800a1f5c: 80 78 19 00                 	cmpb	$0x0, 0x19(%rax)
1800a1f60: 75 13                       	jne	0x1800a1f75 <SetPDFrameWarpNativeCameraSource+0x296e5>
1800a1f62: 48 3b 58 10                 	cmpq	0x10(%rax), %rbx
1800a1f66: 75 0d                       	jne	0x1800a1f75 <SetPDFrameWarpNativeCameraSource+0x296e5>
1800a1f68: 48 8b d8                    	movq	%rax, %rbx
1800a1f6b: 48 8b 40 08                 	movq	0x8(%rax), %rax
1800a1f6f: 80 78 19 00                 	cmpb	$0x0, 0x19(%rax)
1800a1f73: 74 ed                       	je	0x1800a1f62 <SetPDFrameWarpNativeCameraSource+0x296d2>
1800a1f75: 48 8b d8                    	movq	%rax, %rbx
1800a1f78: eb 25                       	jmp	0x1800a1f9f <SetPDFrameWarpNativeCameraSource+0x2970f>
1800a1f7a: 48 8b d9                    	movq	%rcx, %rbx
1800a1f7d: 48 8b 09                    	movq	(%rcx), %rcx
1800a1f80: 80 79 19 00                 	cmpb	$0x0, 0x19(%rcx)
1800a1f84: 75 19                       	jne	0x1800a1f9f <SetPDFrameWarpNativeCameraSource+0x2970f>
1800a1f86: 66 66 0f 1f 84 00 00 00 00 00       	nopw	(%rax,%rax)
1800a1f90: 48 8b d9                    	movq	%rcx, %rbx
1800a1f93: 48 8b 01                    	movq	(%rcx), %rax
1800a1f96: 48 8b c8                    	movq	%rax, %rcx
1800a1f99: 80 78 19 00                 	cmpb	$0x0, 0x19(%rax)
1800a1f9d: 74 f1                       	je	0x1800a1f90 <SetPDFrameWarpNativeCameraSource+0x29700>
1800a1f9f: 80 7b 19 00                 	cmpb	$0x0, 0x19(%rbx)
1800a1fa3: 74 9b                       	je	0x1800a1f40 <SetPDFrameWarpNativeCameraSource+0x296b0>
1800a1fa5: 4c 8b 75 38                 	movq	0x38(%rbp), %r14
1800a1fa9: 49 8b 5e 08                 	movq	0x8(%r14), %rbx
1800a1fad: 80 7b 19 00                 	cmpb	$0x0, 0x19(%rbx)
1800a1fb1: 75 27                       	jne	0x1800a1fda <SetPDFrameWarpNativeCameraSource+0x2974a>
1800a1fb3: 4c 8b 43 10                 	movq	0x10(%rbx), %r8
1800a1fb7: 48 8d 55 38                 	leaq	0x38(%rbp), %rdx
1800a1fbb: 48 8d 4d 38                 	leaq	0x38(%rbp), %rcx
1800a1fbf: e8 0c 62 fd ff              	callq	0x1800781d0 <SetPDFrameWarpDiagnosticHud+0x4f780>
1800a1fc4: 48 8b cb                    	movq	%rbx, %rcx
1800a1fc7: 48 8b 1b                    	movq	(%rbx), %rbx
1800a1fca: ba 30 00 00 00              	movl	$0x30, %edx
1800a1fcf: e8 c8 a3 06 00              	callq	0x18010c39c <NVSDK_NGX_UpdateFeature+0x2f5c>
1800a1fd4: 80 7b 19 00                 	cmpb	$0x0, 0x19(%rbx)
1800a1fd8: 74 d9                       	je	0x1800a1fb3 <SetPDFrameWarpNativeCameraSource+0x29723>
1800a1fda: 4d 89 76 08                 	movq	%r14, 0x8(%r14)
1800a1fde: 4d 89 36                    	movq	%r14, (%r14)
1800a1fe1: 4d 89 76 10                 	movq	%r14, 0x10(%r14)
1800a1fe5: 45 33 ed                    	xorl	%r13d, %r13d
1800a1fe8: 4c 89 6d 40                 	movq	%r13, 0x40(%rbp)
1800a1fec: 48 8d 9d 28 07 00 00        	leaq	0x728(%rbp), %rbx
1800a1ff3: 41 be 03 00 00 00           	movl	$0x3, %r14d
1800a1ff9: 0f 1f 80 00 00 00 00        	nopl	(%rax)
1800a2000: 48 8b d3                    	movq	%rbx, %rdx
1800a2003: e8 e8 b3 ff ff              	callq	0x18009d3f0 <SetPDFrameWarpNativeCameraSource+0x24b60>
1800a2008: 48 83 c3 08                 	addq	$0x8, %rbx
1800a200c: 4d 2b f4                    	subq	%r12, %r14
1800a200f: 75 ef                       	jne	0x1800a2000 <SetPDFrameWarpNativeCameraSource+0x29770>
1800a2011: 48 8d 95 40 07 00 00        	leaq	0x740(%rbp), %rdx
1800a2018: e8 d3 b3 ff ff              	callq	0x18009d3f0 <SetPDFrameWarpNativeCameraSource+0x24b60>
1800a201d: 48 8b cd                    	movq	%rbp, %rcx
1800a2020: e8 0b b4 ff ff              	callq	0x18009d430 <SetPDFrameWarpNativeCameraSource+0x24ba0>
1800a2025: 48 8b cd                    	movq	%rbp, %rcx
1800a2028: e8 b3 b2 ff ff              	callq	0x18009d2e0 <SetPDFrameWarpNativeCameraSource+0x24a50>
1800a202d: 90                          	nop
1800a202e: 48 8b ce                    	movq	%rsi, %rcx
1800a2031: ff 15 f1 14 07 00           	callq	*0x714f1(%rip)          # 0x180113528
1800a2037: 90                          	nop
1800a2038: 48 8b cf                    	movq	%rdi, %rcx
1800a203b: ff 15 e7 14 07 00           	callq	*0x714e7(%rip)          # 0x180113528
1800a2041: 48 8b 8d f8 02 00 00        	movq	0x2f8(%rbp), %rcx
1800a2048: 48 85 c9                    	testq	%rcx, %rcx
1800a204b: 74 0d                       	je	0x1800a205a <SetPDFrameWarpNativeCameraSource+0x297ca>
1800a204d: 48 8b 01                    	movq	(%rcx), %rax
1800a2050: ff 50 10                    	callq	*0x10(%rax)
1800a2053: 4c 89 ad f8 02 00 00        	movq	%r13, 0x2f8(%rbp)
1800a205a: 48 8b 8d 00 03 00 00        	movq	0x300(%rbp), %rcx
1800a2061: 48 85 c9                    	testq	%rcx, %rcx
1800a2064: 74 0d                       	je	0x1800a2073 <SetPDFrameWarpNativeCameraSource+0x297e3>
1800a2066: 48 8b 01                    	movq	(%rcx), %rax
1800a2069: ff 50 10                    	callq	*0x10(%rax)
1800a206c: 4c 89 ad 00 03 00 00        	movq	%r13, 0x300(%rbp)
1800a2073: 48 8b 8d 08 03 00 00        	movq	0x308(%rbp), %rcx
1800a207a: 48 85 c9                    	testq	%rcx, %rcx
1800a207d: 74 0d                       	je	0x1800a208c <SetPDFrameWarpNativeCameraSource+0x297fc>
1800a207f: 48 8b 01                    	movq	(%rcx), %rax
1800a2082: ff 50 10                    	callq	*0x10(%rax)
1800a2085: 4c 89 ad 08 03 00 00        	movq	%r13, 0x308(%rbp)
1800a208c: 48 8b 8d f0 02 00 00        	movq	0x2f0(%rbp), %rcx
1800a2093: 48 85 c9                    	testq	%rcx, %rcx
1800a2096: 74 0d                       	je	0x1800a20a5 <SetPDFrameWarpNativeCameraSource+0x29815>
1800a2098: 48 8b 01                    	movq	(%rcx), %rax
1800a209b: ff 50 10                    	callq	*0x10(%rax)
1800a209e: 4c 89 ad f0 02 00 00        	movq	%r13, 0x2f0(%rbp)
1800a20a5: 48 8b 8d 68 03 00 00        	movq	0x368(%rbp), %rcx
1800a20ac: 48 85 c9                    	testq	%rcx, %rcx
1800a20af: 74 0d                       	je	0x1800a20be <SetPDFrameWarpNativeCameraSource+0x2982e>
1800a20b1: 48 8b 01                    	movq	(%rcx), %rax
1800a20b4: ff 50 10                    	callq	*0x10(%rax)
1800a20b7: 4c 89 ad 68 03 00 00        	movq	%r13, 0x368(%rbp)
1800a20be: 48 8b 8d 70 03 00 00        	movq	0x370(%rbp), %rcx
1800a20c5: 48 85 c9                    	testq	%rcx, %rcx
1800a20c8: 74 0d                       	je	0x1800a20d7 <SetPDFrameWarpNativeCameraSource+0x29847>
1800a20ca: 48 8b 01                    	movq	(%rcx), %rax
1800a20cd: ff 50 10                    	callq	*0x10(%rax)
1800a20d0: 4c 89 ad 70 03 00 00        	movq	%r13, 0x370(%rbp)
1800a20d7: 48 8b 8d 78 03 00 00        	movq	0x378(%rbp), %rcx
1800a20de: 48 85 c9                    	testq	%rcx, %rcx
1800a20e1: 74 0d                       	je	0x1800a20f0 <SetPDFrameWarpNativeCameraSource+0x29860>
1800a20e3: 48 8b 01                    	movq	(%rcx), %rax
1800a20e6: ff 50 10                    	callq	*0x10(%rax)
1800a20e9: 4c 89 ad 78 03 00 00        	movq	%r13, 0x378(%rbp)
1800a20f0: 48 8b 8d 60 03 00 00        	movq	0x360(%rbp), %rcx
1800a20f7: 48 85 c9                    	testq	%rcx, %rcx
1800a20fa: 74 0d                       	je	0x1800a2109 <SetPDFrameWarpNativeCameraSource+0x29879>
1800a20fc: 48 8b 01                    	movq	(%rcx), %rax
1800a20ff: ff 50 10                    	callq	*0x10(%rax)
1800a2102: 4c 89 ad 60 03 00 00        	movq	%r13, 0x360(%rbp)
1800a2109: 48 8b 8d d8 03 00 00        	movq	0x3d8(%rbp), %rcx
1800a2110: 48 85 c9                    	testq	%rcx, %rcx
1800a2113: 74 0d                       	je	0x1800a2122 <SetPDFrameWarpNativeCameraSource+0x29892>
1800a2115: 48 8b 01                    	movq	(%rcx), %rax
1800a2118: ff 50 10                    	callq	*0x10(%rax)
1800a211b: 4c 89 ad d8 03 00 00        	movq	%r13, 0x3d8(%rbp)
1800a2122: 48 8b 8d e0 03 00 00        	movq	0x3e0(%rbp), %rcx
1800a2129: 48 85 c9                    	testq	%rcx, %rcx
1800a212c: 74 0d                       	je	0x1800a213b <SetPDFrameWarpNativeCameraSource+0x298ab>
1800a212e: 48 8b 01                    	movq	(%rcx), %rax
1800a2131: ff 50 10                    	callq	*0x10(%rax)
1800a2134: 4c 89 ad e0 03 00 00        	movq	%r13, 0x3e0(%rbp)
1800a213b: 48 8b 8d e8 03 00 00        	movq	0x3e8(%rbp), %rcx
1800a2142: 48 85 c9                    	testq	%rcx, %rcx
1800a2145: 74 0d                       	je	0x1800a2154 <SetPDFrameWarpNativeCameraSource+0x298c4>
1800a2147: 48 8b 01                    	movq	(%rcx), %rax
1800a214a: ff 50 10                    	callq	*0x10(%rax)
1800a214d: 4c 89 ad e8 03 00 00        	movq	%r13, 0x3e8(%rbp)
1800a2154: 48 8b 8d d0 03 00 00        	movq	0x3d0(%rbp), %rcx
1800a215b: 48 85 c9                    	testq	%rcx, %rcx
1800a215e: 74 0d                       	je	0x1800a216d <SetPDFrameWarpNativeCameraSource+0x298dd>
1800a2160: 48 8b 01                    	movq	(%rcx), %rax
1800a2163: ff 50 10                    	callq	*0x10(%rax)
1800a2166: 4c 89 ad d0 03 00 00        	movq	%r13, 0x3d0(%rbp)
1800a216d: 48 8b 8d 48 04 00 00        	movq	0x448(%rbp), %rcx
1800a2174: 48 85 c9                    	testq	%rcx, %rcx
1800a2177: 74 0d                       	je	0x1800a2186 <SetPDFrameWarpNativeCameraSource+0x298f6>
1800a2179: 48 8b 01                    	movq	(%rcx), %rax
1800a217c: ff 50 10                    	callq	*0x10(%rax)
1800a217f: 4c 89 ad 48 04 00 00        	movq	%r13, 0x448(%rbp)
1800a2186: 48 8b 8d 50 04 00 00        	movq	0x450(%rbp), %rcx
1800a218d: 48 85 c9                    	testq	%rcx, %rcx
1800a2190: 74 0d                       	je	0x1800a219f <SetPDFrameWarpNativeCameraSource+0x2990f>
1800a2192: 48 8b 01                    	movq	(%rcx), %rax
1800a2195: ff 50 10                    	callq	*0x10(%rax)
1800a2198: 4c 89 ad 50 04 00 00        	movq	%r13, 0x450(%rbp)
1800a219f: 48 8b 8d 58 04 00 00        	movq	0x458(%rbp), %rcx
1800a21a6: 48 85 c9                    	testq	%rcx, %rcx
1800a21a9: 74 0d                       	je	0x1800a21b8 <SetPDFrameWarpNativeCameraSource+0x29928>
1800a21ab: 48 8b 01                    	movq	(%rcx), %rax
1800a21ae: ff 50 10                    	callq	*0x10(%rax)
1800a21b1: 4c 89 ad 58 04 00 00        	movq	%r13, 0x458(%rbp)
1800a21b8: 48 8b 8d 40 04 00 00        	movq	0x440(%rbp), %rcx
1800a21bf: 48 85 c9                    	testq	%rcx, %rcx
1800a21c2: 74 0d                       	je	0x1800a21d1 <SetPDFrameWarpNativeCameraSource+0x29941>
1800a21c4: 48 8b 01                    	movq	(%rcx), %rax
1800a21c7: ff 50 10                    	callq	*0x10(%rax)
1800a21ca: 4c 89 ad 40 04 00 00        	movq	%r13, 0x440(%rbp)
1800a21d1: 48 8b 8d b8 04 00 00        	movq	0x4b8(%rbp), %rcx
1800a21d8: 48 85 c9                    	testq	%rcx, %rcx
1800a21db: 74 0d                       	je	0x1800a21ea <SetPDFrameWarpNativeCameraSource+0x2995a>
1800a21dd: 48 8b 01                    	movq	(%rcx), %rax
1800a21e0: ff 50 10                    	callq	*0x10(%rax)
1800a21e3: 4c 89 ad b8 04 00 00        	movq	%r13, 0x4b8(%rbp)
1800a21ea: 48 8b 8d c0 04 00 00        	movq	0x4c0(%rbp), %rcx
1800a21f1: 48 85 c9                    	testq	%rcx, %rcx
1800a21f4: 74 0d                       	je	0x1800a2203 <SetPDFrameWarpNativeCameraSource+0x29973>
1800a21f6: 48 8b 01                    	movq	(%rcx), %rax
1800a21f9: ff 50 10                    	callq	*0x10(%rax)
1800a21fc: 4c 89 ad c0 04 00 00        	movq	%r13, 0x4c0(%rbp)
1800a2203: 48 8b 8d c8 04 00 00        	movq	0x4c8(%rbp), %rcx
1800a220a: 48 85 c9                    	testq	%rcx, %rcx
1800a220d: 74 0d                       	je	0x1800a221c <SetPDFrameWarpNativeCameraSource+0x2998c>
1800a220f: 48 8b 01                    	movq	(%rcx), %rax
1800a2212: ff 50 10                    	callq	*0x10(%rax)
1800a2215: 4c 89 ad c8 04 00 00        	movq	%r13, 0x4c8(%rbp)
1800a221c: 48 8b 8d b0 04 00 00        	movq	0x4b0(%rbp), %rcx
1800a2223: 48 85 c9                    	testq	%rcx, %rcx
1800a2226: 74 0d                       	je	0x1800a2235 <SetPDFrameWarpNativeCameraSource+0x299a5>
1800a2228: 48 8b 01                    	movq	(%rcx), %rax
1800a222b: ff 50 10                    	callq	*0x10(%rax)
1800a222e: 4c 89 ad b0 04 00 00        	movq	%r13, 0x4b0(%rbp)
1800a2235: 48 8b 8d 28 05 00 00        	movq	0x528(%rbp), %rcx
1800a223c: 48 85 c9                    	testq	%rcx, %rcx
1800a223f: 74 0d                       	je	0x1800a224e <SetPDFrameWarpNativeCameraSource+0x299be>
1800a2241: 48 8b 01                    	movq	(%rcx), %rax
1800a2244: ff 50 10                    	callq	*0x10(%rax)
1800a2247: 4c 89 ad 28 05 00 00        	movq	%r13, 0x528(%rbp)
1800a224e: 48 8b 8d 30 05 00 00        	movq	0x530(%rbp), %rcx
1800a2255: 48 85 c9                    	testq	%rcx, %rcx
1800a2258: 74 0d                       	je	0x1800a2267 <SetPDFrameWarpNativeCameraSource+0x299d7>
1800a225a: 48 8b 01                    	movq	(%rcx), %rax
1800a225d: ff 50 10                    	callq	*0x10(%rax)
1800a2260: 4c 89 ad 30 05 00 00        	movq	%r13, 0x530(%rbp)
1800a2267: 48 8b 8d 38 05 00 00        	movq	0x538(%rbp), %rcx
1800a226e: 48 85 c9                    	testq	%rcx, %rcx
1800a2271: 74 0d                       	je	0x1800a2280 <SetPDFrameWarpNativeCameraSource+0x299f0>
1800a2273: 48 8b 01                    	movq	(%rcx), %rax
1800a2276: ff 50 10                    	callq	*0x10(%rax)
1800a2279: 4c 89 ad 38 05 00 00        	movq	%r13, 0x538(%rbp)
1800a2280: 48 8b 8d 20 05 00 00        	movq	0x520(%rbp), %rcx
1800a2287: 48 85 c9                    	testq	%rcx, %rcx
1800a228a: 74 0d                       	je	0x1800a2299 <SetPDFrameWarpNativeCameraSource+0x29a09>
1800a228c: 48 8b 01                    	movq	(%rcx), %rax
1800a228f: ff 50 10                    	callq	*0x10(%rax)
1800a2292: 4c 89 ad 20 05 00 00        	movq	%r13, 0x520(%rbp)
1800a2299: 48 8b 8d 98 05 00 00        	movq	0x598(%rbp), %rcx
1800a22a0: 48 85 c9                    	testq	%rcx, %rcx
1800a22a3: 74 0d                       	je	0x1800a22b2 <SetPDFrameWarpNativeCameraSource+0x29a22>
1800a22a5: 48 8b 01                    	movq	(%rcx), %rax
1800a22a8: ff 50 10                    	callq	*0x10(%rax)
1800a22ab: 4c 89 ad 98 05 00 00        	movq	%r13, 0x598(%rbp)
1800a22b2: 48 8b 8d a0 05 00 00        	movq	0x5a0(%rbp), %rcx
1800a22b9: 48 85 c9                    	testq	%rcx, %rcx
1800a22bc: 74 0d                       	je	0x1800a22cb <SetPDFrameWarpNativeCameraSource+0x29a3b>
1800a22be: 48 8b 01                    	movq	(%rcx), %rax
1800a22c1: ff 50 10                    	callq	*0x10(%rax)
1800a22c4: 4c 89 ad a0 05 00 00        	movq	%r13, 0x5a0(%rbp)
1800a22cb: 48 8b 8d a8 05 00 00        	movq	0x5a8(%rbp), %rcx
1800a22d2: 48 85 c9                    	testq	%rcx, %rcx
1800a22d5: 74 0d                       	je	0x1800a22e4 <SetPDFrameWarpNativeCameraSource+0x29a54>
1800a22d7: 48 8b 01                    	movq	(%rcx), %rax
1800a22da: ff 50 10                    	callq	*0x10(%rax)
1800a22dd: 4c 89 ad a8 05 00 00        	movq	%r13, 0x5a8(%rbp)
1800a22e4: 48 8b 8d 90 05 00 00        	movq	0x590(%rbp), %rcx
1800a22eb: 48 85 c9                    	testq	%rcx, %rcx
1800a22ee: 74 0d                       	je	0x1800a22fd <SetPDFrameWarpNativeCameraSource+0x29a6d>
1800a22f0: 48 8b 01                    	movq	(%rcx), %rax
1800a22f3: ff 50 10                    	callq	*0x10(%rax)
1800a22f6: 4c 89 ad 90 05 00 00        	movq	%r13, 0x590(%rbp)
1800a22fd: 48 8b 8d 08 06 00 00        	movq	0x608(%rbp), %rcx
1800a2304: 48 85 c9                    	testq	%rcx, %rcx
1800a2307: 74 0d                       	je	0x1800a2316 <SetPDFrameWarpNativeCameraSource+0x29a86>
1800a2309: 48 8b 01                    	movq	(%rcx), %rax
1800a230c: ff 50 10                    	callq	*0x10(%rax)
1800a230f: 4c 89 ad 08 06 00 00        	movq	%r13, 0x608(%rbp)
1800a2316: 48 8b 8d 10 06 00 00        	movq	0x610(%rbp), %rcx
1800a231d: 48 85 c9                    	testq	%rcx, %rcx
1800a2320: 74 0d                       	je	0x1800a232f <SetPDFrameWarpNativeCameraSource+0x29a9f>
1800a2322: 48 8b 01                    	movq	(%rcx), %rax
1800a2325: ff 50 10                    	callq	*0x10(%rax)
1800a2328: 4c 89 ad 10 06 00 00        	movq	%r13, 0x610(%rbp)
1800a232f: 48 8b 8d 18 06 00 00        	movq	0x618(%rbp), %rcx
1800a2336: 48 85 c9                    	testq	%rcx, %rcx
1800a2339: 74 0d                       	je	0x1800a2348 <SetPDFrameWarpNativeCameraSource+0x29ab8>
1800a233b: 48 8b 01                    	movq	(%rcx), %rax
1800a233e: ff 50 10                    	callq	*0x10(%rax)
1800a2341: 4c 89 ad 18 06 00 00        	movq	%r13, 0x618(%rbp)
1800a2348: 48 8b 8d 00 06 00 00        	movq	0x600(%rbp), %rcx
1800a234f: 48 85 c9                    	testq	%rcx, %rcx
1800a2352: 74 0d                       	je	0x1800a2361 <SetPDFrameWarpNativeCameraSource+0x29ad1>
1800a2354: 48 8b 01                    	movq	(%rcx), %rax
1800a2357: ff 50 10                    	callq	*0x10(%rax)
1800a235a: 4c 89 ad 00 06 00 00        	movq	%r13, 0x600(%rbp)
1800a2361: 48 8b 8d 78 06 00 00        	movq	0x678(%rbp), %rcx
1800a2368: 48 85 c9                    	testq	%rcx, %rcx
1800a236b: 74 0d                       	je	0x1800a237a <SetPDFrameWarpNativeCameraSource+0x29aea>
1800a236d: 48 8b 01                    	movq	(%rcx), %rax
1800a2370: ff 50 10                    	callq	*0x10(%rax)
1800a2373: 4c 89 ad 78 06 00 00        	movq	%r13, 0x678(%rbp)
1800a237a: 48 8b 8d 80 06 00 00        	movq	0x680(%rbp), %rcx
1800a2381: 48 85 c9                    	testq	%rcx, %rcx
1800a2384: 74 0d                       	je	0x1800a2393 <SetPDFrameWarpNativeCameraSource+0x29b03>
1800a2386: 48 8b 01                    	movq	(%rcx), %rax
1800a2389: ff 50 10                    	callq	*0x10(%rax)
1800a238c: 4c 89 ad 80 06 00 00        	movq	%r13, 0x680(%rbp)
1800a2393: 48 8b 8d 88 06 00 00        	movq	0x688(%rbp), %rcx
1800a239a: 48 85 c9                    	testq	%rcx, %rcx
1800a239d: 74 0d                       	je	0x1800a23ac <SetPDFrameWarpNativeCameraSource+0x29b1c>
1800a239f: 48 8b 01                    	movq	(%rcx), %rax
1800a23a2: ff 50 10                    	callq	*0x10(%rax)
1800a23a5: 4c 89 ad 88 06 00 00        	movq	%r13, 0x688(%rbp)
1800a23ac: 48 8b 8d 70 06 00 00        	movq	0x670(%rbp), %rcx
1800a23b3: 48 85 c9                    	testq	%rcx, %rcx
1800a23b6: 74 0d                       	je	0x1800a23c5 <SetPDFrameWarpNativeCameraSource+0x29b35>
1800a23b8: 48 8b 01                    	movq	(%rcx), %rax
1800a23bb: ff 50 10                    	callq	*0x10(%rax)
1800a23be: 4c 89 ad 70 06 00 00        	movq	%r13, 0x670(%rbp)
1800a23c5: 48 8d 95 e0 06 00 00        	leaq	0x6e0(%rbp), %rdx
1800a23cc: e8 1f b0 ff ff              	callq	0x18009d3f0 <SetPDFrameWarpNativeCameraSource+0x24b60>
1800a23d1: 48 8d 95 e8 06 00 00        	leaq	0x6e8(%rbp), %rdx
1800a23d8: e8 13 b0 ff ff              	callq	0x18009d3f0 <SetPDFrameWarpNativeCameraSource+0x24b60>
1800a23dd: 48 8d 95 f0 06 00 00        	leaq	0x6f0(%rbp), %rdx
1800a23e4: e8 07 b0 ff ff              	callq	0x18009d3f0 <SetPDFrameWarpNativeCameraSource+0x24b60>
1800a23e9: 48 8d 95 f8 06 00 00        	leaq	0x6f8(%rbp), %rdx
1800a23f0: e8 fb af ff ff              	callq	0x18009d3f0 <SetPDFrameWarpNativeCameraSource+0x24b60>
1800a23f5: 48 8d 95 00 07 00 00        	leaq	0x700(%rbp), %rdx
1800a23fc: e8 ef af ff ff              	callq	0x18009d3f0 <SetPDFrameWarpNativeCameraSource+0x24b60>
1800a2401: 48 8d 95 08 07 00 00        	leaq	0x708(%rbp), %rdx
1800a2408: e8 e3 af ff ff              	callq	0x18009d3f0 <SetPDFrameWarpNativeCameraSource+0x24b60>
1800a240d: 48 8d 95 10 07 00 00        	leaq	0x710(%rbp), %rdx
1800a2414: e8 d7 af ff ff              	callq	0x18009d3f0 <SetPDFrameWarpNativeCameraSource+0x24b60>
1800a2419: 48 8d 95 18 07 00 00        	leaq	0x718(%rbp), %rdx
1800a2420: e8 cb af ff ff              	callq	0x18009d3f0 <SetPDFrameWarpNativeCameraSource+0x24b60>
1800a2425: 48 8d 95 20 07 00 00        	leaq	0x720(%rbp), %rdx
1800a242c: e8 bf af ff ff              	callq	0x18009d3f0 <SetPDFrameWarpNativeCameraSource+0x24b60>
1800a2431: 48 8b 5d 20                 	movq	0x20(%rbp), %rbx
1800a2435: 48 85 db                    	testq	%rbx, %rbx
1800a2438: 74 23                       	je	0x1800a245d <SetPDFrameWarpNativeCameraSource+0x29bcd>
1800a243a: 48 8d 4b 28                 	leaq	0x28(%rbx), %rcx
1800a243e: e8 dd c8 f6 ff              	callq	0x18000ed20 <.text+0xdd20>
1800a2443: 48 8d 4b 18                 	leaq	0x18(%rbx), %rcx
1800a2447: e8 74 01 00 00              	callq	0x1800a25c0 <SetPDFrameWarpNativeCameraSource+0x29d30>
1800a244c: ba 50 00 00 00              	movl	$0x50, %edx
1800a2451: 48 8b cb                    	movq	%rbx, %rcx
1800a2454: e8 43 9f 06 00              	callq	0x18010c39c <NVSDK_NGX_UpdateFeature+0x2f5c>
1800a2459: 4c 89 6d 20                 	movq	%r13, 0x20(%rbp)
1800a245d: 48 8b 4d 28                 	movq	0x28(%rbp), %rcx
1800a2461: 48 85 c9                    	testq	%rcx, %rcx
1800a2464: 74 09                       	je	0x1800a246f <SetPDFrameWarpNativeCameraSource+0x29bdf>
1800a2466: e8 05 07 06 00              	callq	0x180102b70 <NVSDK_NGX_D3D12_DestroyParameters>
1800a246b: 4c 89 6d 28                 	movq	%r13, 0x28(%rbp)
1800a246f: 48 8b 4d 30                 	movq	0x30(%rbp), %rcx
1800a2473: 48 85 c9                    	testq	%rcx, %rcx
1800a2476: 74 09                       	je	0x1800a2481 <SetPDFrameWarpNativeCameraSource+0x29bf1>
1800a2478: e8 63 1c 00 00              	callq	0x1800a40e0 <SetPDFrameWarpNativeCameraSource+0x2b850>
1800a247d: 4c 89 6d 30                 	movq	%r13, 0x30(%rbp)
1800a2481: 80 7d 18 00                 	cmpb	$0x0, 0x18(%rbp)
1800a2485: 0f 84 a4 00 00 00           	je	0x1800a252f <SetPDFrameWarpNativeCameraSource+0x29c9f>
1800a248b: 48 8b 5d 08                 	movq	0x8(%rbp), %rbx
1800a248f: 48 85 db                    	testq	%rbx, %rbx
1800a2492: 75 18                       	jne	0x1800a24ac <SetPDFrameWarpNativeCameraSource+0x29c1c>
1800a2494: 48 8b 1d 55 34 17 01        	movq	0x1173455(%rip), %rbx   # 0x1812158f0
1800a249b: 48 85 db                    	testq	%rbx, %rbx
1800a249e: 74 09                       	je	0x1800a24a9 <SetPDFrameWarpNativeCameraSource+0x29c19>
1800a24a0: 48 8b 9b 98 00 00 00        	movq	0x98(%rbx), %rbx
1800a24a7: eb 03                       	jmp	0x1800a24ac <SetPDFrameWarpNativeCameraSource+0x29c1c>
1800a24a9: 49 8b dd                    	movq	%r13, %rbx
1800a24ac: 48 8b cb                    	movq	%rbx, %rcx
1800a24af: e8 3c 1b 00 00              	callq	0x1800a3ff0 <SetPDFrameWarpNativeCameraSource+0x2b760>
1800a24b4: 8b f8                       	movl	%eax, %edi
1800a24b6: 8b c8                       	movl	%eax, %ecx
1800a24b8: 81 e1 00 00 f0 ff           	andl	$0xfff00000, %ecx       # imm = 0xFFF00000
1800a24be: 81 f9 00 00 d0 ba           	cmpl	$0xbad00000, %ecx       # imm = 0xBAD00000
1800a24c4: 75 3d                       	jne	0x1800a2503 <SetPDFrameWarpNativeCameraSource+0x29c73>
1800a24c6: 48 8b cb                    	movq	%rbx, %rcx
1800a24c9: e8 d2 91 ff ff              	callq	0x18009b6a0 <SetPDFrameWarpNativeCameraSource+0x22e10>
1800a24ce: 8b f0                       	movl	%eax, %esi
1800a24d0: 48 8b 0d 19 34 17 01        	movq	0x1173419(%rip), %rcx   # 0x1812158f0
1800a24d7: 48 85 c9                    	testq	%rcx, %rcx
1800a24da: 74 09                       	je	0x1800a24e5 <SetPDFrameWarpNativeCameraSource+0x29c55>
1800a24dc: e8 df d7 fc ff              	callq	0x18006fcc0 <SetPDFrameWarpDiagnosticHud+0x47270>
1800a24e1: 84 c0                       	testb	%al, %al
1800a24e3: 75 03                       	jne	0x1800a24e8 <SetPDFrameWarpNativeCameraSource+0x29c58>
1800a24e5: 45 8b e5                    	movl	%r13d, %r12d
1800a24e8: 44 89 64 24 20              	movl	%r12d, 0x20(%rsp)
1800a24ed: 4c 8b cb                    	movq	%rbx, %r9
1800a24f0: 44 8b c6                    	movl	%esi, %r8d
1800a24f3: 8b d7                       	movl	%edi, %edx
1800a24f5: 48 8d 0d 24 29 10 01        	leaq	0x1102924(%rip), %rcx   # 0x1811a4e20
1800a24fc: e8 3f 96 05 00              	callq	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
1800a2501: eb 2c                       	jmp	0x1800a252f <SetPDFrameWarpNativeCameraSource+0x29c9f>
1800a2503: 48 8b 0d e6 33 17 01        	movq	0x11733e6(%rip), %rcx   # 0x1812158f0
1800a250a: 48 85 c9                    	testq	%rcx, %rcx
1800a250d: 74 09                       	je	0x1800a2518 <SetPDFrameWarpNativeCameraSource+0x29c88>
1800a250f: e8 ac d7 fc ff              	callq	0x18006fcc0 <SetPDFrameWarpDiagnosticHud+0x47270>
1800a2514: 84 c0                       	testb	%al, %al
1800a2516: 75 03                       	jne	0x1800a251b <SetPDFrameWarpNativeCameraSource+0x29c8b>
1800a2518: 45 8b e5                    	movl	%r13d, %r12d
1800a251b: 45 8b cc                    	movl	%r12d, %r9d
1800a251e: 4c 8b c3                    	movq	%rbx, %r8
1800a2521: 8b d7                       	movl	%edi, %edx
1800a2523: 48 8d 0d b6 28 10 01        	leaq	0x11028b6(%rip), %rcx   # 0x1811a4de0
1800a252a: e8 11 96 05 00              	callq	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
1800a252f: e8 8c 18 00 00              	callq	0x1800a3dc0 <SetPDFrameWarpNativeCameraSource+0x2b530>
1800a2534: c6 45 18 00                 	movb	$0x0, 0x18(%rbp)
1800a2538: c6 85 b0 01 00 00 00        	movb	$0x0, 0x1b0(%rbp)
1800a253f: 48 8b 4d 08                 	movq	0x8(%rbp), %rcx
1800a2543: 48 85 c9                    	testq	%rcx, %rcx
1800a2546: 74 0a                       	je	0x1800a2552 <SetPDFrameWarpNativeCameraSource+0x29cc2>
1800a2548: 48 8b 01                    	movq	(%rcx), %rax
1800a254b: ff 50 10                    	callq	*0x10(%rax)
1800a254e: 4c 89 6d 08                 	movq	%r13, 0x8(%rbp)
1800a2552: 48 8b 9c 24 80 00 00 00     	movq	0x80(%rsp), %rbx
1800a255a: 48 83 c4 30                 	addq	$0x30, %rsp
1800a255e: 41 5f                       	popq	%r15
1800a2560: 41 5e                       	popq	%r14
1800a2562: 41 5d                       	popq	%r13
1800a2564: 41 5c                       	popq	%r12
1800a2566: 5f                          	popq	%rdi
1800a2567: 5e                          	popq	%rsi
1800a2568: 5d                          	popq	%rbp
1800a2569: c3                          	retq
1800a256a: cc                          	int3
1800a256b: cc                          	int3
1800a256c: cc                          	int3
1800a256d: cc                          	int3
1800a256e: cc                          	int3
1800a256f: cc                          	int3
1800a2570: 4c 8b 49 38                 	movq	0x38(%rcx), %r9
1800a2574: 4d 8b c1                    	movq	%r9, %r8
1800a2577: 49 8b 41 08                 	movq	0x8(%r9), %rax
1800a257b: 80 78 19 00                 	cmpb	$0x0, 0x19(%rax)
1800a257f: 75 17                       	jne	0x1800a2598 <SetPDFrameWarpNativeCameraSource+0x29d08>
1800a2581: 39 50 20                    	cmpl	%edx, 0x20(%rax)
1800a2584: 7d 06                       	jge	0x1800a258c <SetPDFrameWarpNativeCameraSource+0x29cfc>
1800a2586: 48 83 c0 10                 	addq	$0x10, %rax
1800a258a: eb 03                       	jmp	0x1800a258f <SetPDFrameWarpNativeCameraSource+0x29cff>
1800a258c: 4c 8b c0                    	movq	%rax, %r8
1800a258f: 48 8b 00                    	movq	(%rax), %rax
1800a2592: 80 78 19 00                 	cmpb	$0x0, 0x19(%rax)
1800a2596: 74 e9                       	je	0x1800a2581 <SetPDFrameWarpNativeCameraSource+0x29cf1>
1800a2598: 41 80 78 19 00              	cmpb	$0x0, 0x19(%r8)
1800a259d: 75 06                       	jne	0x1800a25a5 <SetPDFrameWarpNativeCameraSource+0x29d15>
1800a259f: 41 3b 50 20                 	cmpl	0x20(%r8), %edx
1800a25a3: 7d 03                       	jge	0x1800a25a8 <SetPDFrameWarpNativeCameraSource+0x29d18>
1800a25a5: 4d 8b c1                    	movq	%r9, %r8
1800a25a8: 80 79 18 00                 	cmpb	$0x0, 0x18(%rcx)
1800a25ac: 74 0f                       	je	0x1800a25bd <SetPDFrameWarpNativeCameraSource+0x29d2d>
1800a25ae: 4d 3b c1                    	cmpq	%r9, %r8
1800a25b1: 74 0a                       	je	0x1800a25bd <SetPDFrameWarpNativeCameraSource+0x29d2d>
1800a25b3: 49 83 78 28 00              	cmpq	$0x0, 0x28(%r8)
1800a25b8: 74 03                       	je	0x1800a25bd <SetPDFrameWarpNativeCameraSource+0x29d2d>
1800a25ba: b0 01                       	movb	$0x1, %al
1800a25bc: c3                          	retq
1800a25bd: 32 c0                       	xorb	%al, %al
1800a25bf: c3                          	retq
1800a25c0: 48 89 5c 24 10              	movq	%rbx, 0x10(%rsp)
1800a25c5: 56                          	pushq	%rsi
1800a25c6: 48 83 ec 20                 	subq	$0x20, %rsp
1800a25ca: 48 8b 19                    	movq	(%rcx), %rbx
1800a25cd: 48 8b f1                    	movq	%rcx, %rsi
1800a25d0: 48 8b 43 08                 	movq	0x8(%rbx), %rax
1800a25d4: 48 c7 00 00 00 00 00        	movq	$0x0, (%rax)
1800a25db: 48 8b 1b                    	movq	(%rbx), %rbx
1800a25de: 48 85 db                    	testq	%rbx, %rbx
1800a25e1: 74 33                       	je	0x1800a2616 <SetPDFrameWarpNativeCameraSource+0x29d86>
1800a25e3: 48 89 7c 24 30              	movq	%rdi, 0x30(%rsp)
1800a25e8: 0f 1f 84 00 00 00 00 00     	nopl	(%rax,%rax)
1800a25f0: 48 8b 3b                    	movq	(%rbx), %rdi
1800a25f3: 48 8d 4b 10                 	leaq	0x10(%rbx), %rcx
1800a25f7: e8 a4 5e fd ff              	callq	0x1800784a0 <SetPDFrameWarpDiagnosticHud+0x4fa50>
1800a25fc: ba 50 00 00 00              	movl	$0x50, %edx
