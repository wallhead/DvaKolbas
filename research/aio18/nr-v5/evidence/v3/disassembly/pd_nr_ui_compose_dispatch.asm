
/mnt/data/aio18_nr_work/PDPerfPlugin.dll:	file format coff-x86-64

Disassembly of section .text:

0000000180078890 <SetPDFrameWarpNativeCameraSource>:
18009dca0: 40 55                       	push	rbp
18009dca2: 53                          	push	rbx
18009dca3: 56                          	push	rsi
18009dca4: 57                          	push	rdi
18009dca5: 41 54                       	push	r12
18009dca7: 41 55                       	push	r13
18009dca9: 41 56                       	push	r14
18009dcab: 48 8d 6c 24 f9              	lea	rbp, [rsp - 0x7]
18009dcb0: 48 81 ec c0 00 00 00        	sub	rsp, 0xc0
18009dcb7: 48 8b 05 02 cd 12 01        	mov	rax, qword ptr [rip + 0x112cd02] # 0x1811ca9c0
18009dcbe: 48 33 c4                    	xor	rax, rsp
18009dcc1: 48 89 45 f7                 	mov	qword ptr [rbp - 0x9], rax
18009dcc5: 4c 8b 75 67                 	mov	r14, qword ptr [rbp + 0x67]
18009dcc9: 49 8b c0                    	mov	rax, r8
18009dccc: 48 8b 75 6f                 	mov	rsi, qword ptr [rbp + 0x6f]
18009dcd0: 4c 8b e2                    	mov	r12, rdx
18009dcd3: 48 8b 5d 77                 	mov	rbx, qword ptr [rbp + 0x77]
18009dcd7: 48 8b f9                    	mov	rdi, rcx
18009dcda: 4c 8b 6d 7f                 	mov	r13, qword ptr [rbp + 0x7f]
18009dcde: 48 89 45 b7                 	mov	qword ptr [rbp - 0x49], rax
18009dce2: 48 85 d2                    	test	rdx, rdx
18009dce5: 0f 84 4a 03 00 00           	je	0x18009e035 <SetPDFrameWarpNativeCameraSource+0x257a5>
18009dceb: 48 85 c0                    	test	rax, rax
18009dcee: 0f 84 41 03 00 00           	je	0x18009e035 <SetPDFrameWarpNativeCameraSource+0x257a5>
18009dcf4: 4d 85 ed                    	test	r13, r13
18009dcf7: 0f 84 38 03 00 00           	je	0x18009e035 <SetPDFrameWarpNativeCameraSource+0x257a5>
18009dcfd: 48 83 b9 40 01 00 00 00     	cmp	qword ptr [rcx + 0x140], 0x0
18009dd05: 0f 84 2a 03 00 00           	je	0x18009e035 <SetPDFrameWarpNativeCameraSource+0x257a5>
18009dd0b: 48 8b 91 58 01 00 00        	mov	rdx, qword ptr [rcx + 0x158]
18009dd12: 48 85 d2                    	test	rdx, rdx
18009dd15: 0f 84 1a 03 00 00           	je	0x18009e035 <SetPDFrameWarpNativeCameraSource+0x257a5>
18009dd1b: 8b 89 64 01 00 00           	mov	ecx, dword ptr [rcx + 0x164]
18009dd21: 41 0f 10 01                 	movups	xmm0, xmmword ptr [r9]
18009dd25: 4c 89 bc 24 18 01 00 00     	mov	qword ptr [rsp + 0x118], r15
18009dd2d: 44 8b f9                    	mov	r15d, ecx
18009dd30: 41 0f 10 49 10              	movups	xmm1, xmmword ptr [r9 + 0x10]
18009dd35: 8d 41 01                    	lea	eax, [rcx + 0x1]
18009dd38: 41 83 e7 1f                 	and	r15d, 0x1f
18009dd3c: 89 87 64 01 00 00           	mov	dword ptr [rdi + 0x164], eax
18009dd42: 41 8b c7                    	mov	eax, r15d
18009dd45: c1 e0 08                    	shl	eax, 0x8
18009dd48: 0f 11 04 10                 	movups	xmmword ptr [rax + rdx], xmm0
18009dd4c: 41 0f 10 41 20              	movups	xmm0, xmmword ptr [r9 + 0x20]
18009dd51: 0f 11 4c 10 10              	movups	xmmword ptr [rax + rdx + 0x10], xmm1
18009dd56: f2 41 0f 10 49 30           	movsd	xmm1, qword ptr [r9 + 0x30]
18009dd5c: 0f 11 44 10 20              	movups	xmmword ptr [rax + rdx + 0x20], xmm0
18009dd61: f2 0f 11 4c 10 30           	movsd	qword ptr [rax + rdx + 0x30], xmm1
18009dd67: 48 8d 54 24 30              	lea	rdx, [rsp + 0x30]
18009dd6c: 48 8b 8f 40 01 00 00        	mov	rcx, qword ptr [rdi + 0x140]
18009dd73: 48 8b 01                    	mov	rax, qword ptr [rcx]
18009dd76: ff 50 48                    	call	qword ptr [rax + 0x48]
18009dd79: 48 8b 44 24 30              	mov	rax, qword ptr [rsp + 0x30]
18009dd7e: 8b 8f 60 01 00 00           	mov	ecx, dword ptr [rdi + 0x160]
18009dd84: 49 0f af cf                 	imul	rcx, r15
18009dd88: 48 8d 0c 88                 	lea	rcx, [rax + 4*rcx]
18009dd8c: 48 89 4c 24 30              	mov	qword ptr [rsp + 0x30], rcx
18009dd91: 4d 85 f6                    	test	r14, r14
18009dd94: 75 07                       	jne	0x18009dd9d <SetPDFrameWarpNativeCameraSource+0x2550d>
18009dd96: 4c 8b b7 50 01 00 00        	mov	r14, qword ptr [rdi + 0x150]
18009dd9d: 48 85 f6                    	test	rsi, rsi
18009dda0: 75 07                       	jne	0x18009dda9 <SetPDFrameWarpNativeCameraSource+0x25519>
18009dda2: 48 8b b7 50 01 00 00        	mov	rsi, qword ptr [rdi + 0x150]
18009dda9: 48 85 db                    	test	rbx, rbx
18009ddac: 75 07                       	jne	0x18009ddb5 <SetPDFrameWarpNativeCameraSource+0x25525>
18009ddae: 48 8b 9f 50 01 00 00        	mov	rbx, qword ptr [rdi + 0x150]
18009ddb5: 0f 57 c0                    	xorps	xmm0, xmm0
18009ddb8: 48 c7 45 8b 00 00 00 00     	mov	qword ptr [rbp - 0x75], 0x0
18009ddc0: f3 0f 7f 45 97              	movdqu	xmmword ptr [rbp - 0x69], xmm0
18009ddc5: 49 8b 06                    	mov	rax, qword ptr [r14]
18009ddc8: 48 8d 55 bf                 	lea	rdx, [rbp - 0x41]
18009ddcc: 49 8b ce                    	mov	rcx, r14
18009ddcf: ff 50 50                    	call	qword ptr [rax + 0x50]
18009ddd2: 8b 4d df                    	mov	ecx, dword ptr [rbp - 0x21]
18009ddd5: e8 86 f0 ff ff              	call	0x18009ce60 <SetPDFrameWarpNativeCameraSource+0x245d0>
18009ddda: 48 8b 4f 08                 	mov	rcx, qword ptr [rdi + 0x8]
18009ddde: 4c 8d 44 24 38              	lea	r8, [rsp + 0x38]
18009dde3: 4c 8b 4c 24 30              	mov	r9, qword ptr [rsp + 0x30]
18009dde8: 49 8b d6                    	mov	rdx, r14
18009ddeb: 89 44 24 38                 	mov	dword ptr [rsp + 0x38], eax
18009ddef: c7 45 83 04 00 00 00        	mov	dword ptr [rbp - 0x7d], 0x4
18009ddf6: c7 45 87 88 16 00 00        	mov	dword ptr [rbp - 0x79], 0x1688
18009ddfd: c7 45 93 01 00 00 00        	mov	dword ptr [rbp - 0x6d], 0x1
18009de04: 48 8b 01                    	mov	rax, qword ptr [rcx]
18009de07: ff 90 90 00 00 00           	call	qword ptr [rax + 0x90]
18009de0d: 0f 57 c0                    	xorps	xmm0, xmm0
18009de10: 48 8d 55 bf                 	lea	rdx, [rbp - 0x41]
18009de14: 33 c0                       	xor	eax, eax
18009de16: 48 8b ce                    	mov	rcx, rsi
18009de19: 0f 11 44 24 38              	movups	xmmword ptr [rsp + 0x38], xmm0
18009de1e: 48 89 45 9f                 	mov	qword ptr [rbp - 0x61], rax
18009de22: 0f 11 45 8f                 	movups	xmmword ptr [rbp - 0x71], xmm0
18009de26: 48 8b 06                    	mov	rax, qword ptr [rsi]
18009de29: ff 50 50                    	call	qword ptr [rax + 0x50]
18009de2c: 8b 4d df                    	mov	ecx, dword ptr [rbp - 0x21]
18009de2f: e8 2c f0 ff ff              	call	0x18009ce60 <SetPDFrameWarpNativeCameraSource+0x245d0>
18009de34: 48 8b 4f 08                 	mov	rcx, qword ptr [rdi + 0x8]
18009de38: 4c 8d 44 24 38              	lea	r8, [rsp + 0x38]
18009de3d: 44 8b 8f 60 01 00 00        	mov	r9d, dword ptr [rdi + 0x160]
18009de44: 48 8b d6                    	mov	rdx, rsi
18009de47: 4c 03 4c 24 30              	add	r9, qword ptr [rsp + 0x30]
18009de4c: 89 44 24 38                 	mov	dword ptr [rsp + 0x38], eax
18009de50: c7 45 83 04 00 00 00        	mov	dword ptr [rbp - 0x7d], 0x4
18009de57: c7 45 87 88 16 00 00        	mov	dword ptr [rbp - 0x79], 0x1688
18009de5e: c7 45 93 01 00 00 00        	mov	dword ptr [rbp - 0x6d], 0x1
18009de65: 48 8b 01                    	mov	rax, qword ptr [rcx]
18009de68: ff 90 90 00 00 00           	call	qword ptr [rax + 0x90]
18009de6e: 0f 57 c0                    	xorps	xmm0, xmm0
18009de71: 48 8d 55 bf                 	lea	rdx, [rbp - 0x41]
18009de75: 33 c0                       	xor	eax, eax
18009de77: 48 8b cb                    	mov	rcx, rbx
18009de7a: 0f 11 44 24 38              	movups	xmmword ptr [rsp + 0x38], xmm0
18009de7f: 48 89 45 9f                 	mov	qword ptr [rbp - 0x61], rax
18009de83: 0f 11 45 8f                 	movups	xmmword ptr [rbp - 0x71], xmm0
18009de87: 48 8b 03                    	mov	rax, qword ptr [rbx]
18009de8a: ff 50 50                    	call	qword ptr [rax + 0x50]
18009de8d: 8b 4d df                    	mov	ecx, dword ptr [rbp - 0x21]
18009de90: e8 cb ef ff ff              	call	0x18009ce60 <SetPDFrameWarpNativeCameraSource+0x245d0>
18009de95: 8b 8f 60 01 00 00           	mov	ecx, dword ptr [rdi + 0x160]
18009de9b: 4c 8d 44 24 38              	lea	r8, [rsp + 0x38]
18009dea0: 89 44 24 38                 	mov	dword ptr [rsp + 0x38], eax
18009dea4: 48 8b d3                    	mov	rdx, rbx
18009dea7: 48 8b 44 24 30              	mov	rax, qword ptr [rsp + 0x30]
18009deac: c7 45 83 04 00 00 00        	mov	dword ptr [rbp - 0x7d], 0x4
18009deb3: c7 45 87 88 16 00 00        	mov	dword ptr [rbp - 0x79], 0x1688
18009deba: c7 45 93 01 00 00 00        	mov	dword ptr [rbp - 0x6d], 0x1
18009dec1: 4c 8d 0c 48                 	lea	r9, [rax + 2*rcx]
18009dec5: 48 8b 4f 08                 	mov	rcx, qword ptr [rdi + 0x8]
18009dec9: 48 8b 01                    	mov	rax, qword ptr [rcx]
18009decc: ff 90 90 00 00 00           	call	qword ptr [rax + 0x90]
18009ded2: 49 8b 45 00                 	mov	rax, qword ptr [r13]
18009ded6: 48 8d 55 bf                 	lea	rdx, [rbp - 0x41]
18009deda: 0f 57 c0                    	xorps	xmm0, xmm0
18009dedd: 0f 57 c9                    	xorps	xmm1, xmm1
18009dee0: 49 8b cd                    	mov	rcx, r13
18009dee3: f3 0f 7f 45 87              	movdqu	xmmword ptr [rbp - 0x79], xmm0
18009dee8: f3 0f 7f 4d 97              	movdqu	xmmword ptr [rbp - 0x69], xmm1
18009deed: ff 50 50                    	call	qword ptr [rax + 0x50]
18009def0: 8b 4d df                    	mov	ecx, dword ptr [rbp - 0x21]
18009def3: e8 68 ef ff ff              	call	0x18009ce60 <SetPDFrameWarpNativeCameraSource+0x245d0>
18009def8: 48 8b 4c 24 30              	mov	rcx, qword ptr [rsp + 0x30]
18009defd: 89 44 24 38                 	mov	dword ptr [rsp + 0x38], eax
18009df01: 8b 87 60 01 00 00           	mov	eax, dword ptr [rdi + 0x160]
18009df07: c7 45 83 04 00 00 00        	mov	dword ptr [rbp - 0x7d], 0x4
18009df0e: 48 8d 14 41                 	lea	rdx, [rcx + 2*rax]
18009df12: 48 8b 4f 08                 	mov	rcx, qword ptr [rdi + 0x8]
18009df16: 48 03 c2                    	add	rax, rdx
18009df19: 48 8b 11                    	mov	rdx, qword ptr [rcx]
18009df1c: 4c 8d 4c 24 38              	lea	r9, [rsp + 0x38]
18009df21: 45 33 c0                    	xor	r8d, r8d
18009df24: 48 89 44 24 20              	mov	qword ptr [rsp + 0x20], rax
18009df29: 4c 8b 92 98 00 00 00        	mov	r10, qword ptr [rdx + 0x98]
18009df30: 49 8b d5                    	mov	rdx, r13
18009df33: 41 ff d2                    	call	r10
18009df36: 48 8b 87 40 01 00 00        	mov	rax, qword ptr [rdi + 0x140]
18009df3d: 4c 8d 45 af                 	lea	r8, [rbp - 0x51]
18009df41: 48 89 45 af                 	mov	qword ptr [rbp - 0x51], rax
18009df45: ba 01 00 00 00              	mov	edx, 0x1
18009df4a: 49 8b 04 24                 	mov	rax, qword ptr [r12]
18009df4e: 49 8b cc                    	mov	rcx, r12
18009df51: ff 90 e0 00 00 00           	call	qword ptr [rax + 0xe0]
18009df57: 49 8b 04 24                 	mov	rax, qword ptr [r12]
18009df5b: 49 8b cc                    	mov	rcx, r12
18009df5e: 48 8b 97 18 01 00 00        	mov	rdx, qword ptr [rdi + 0x118]
18009df65: ff 90 e8 00 00 00           	call	qword ptr [rax + 0xe8]
18009df6b: 49 8b 04 24                 	mov	rax, qword ptr [r12]
18009df6f: 49 8b cc                    	mov	rcx, r12
18009df72: 48 8b 55 b7                 	mov	rdx, qword ptr [rbp - 0x49]
18009df76: ff 90 c8 00 00 00           	call	qword ptr [rax + 0xc8]
18009df7c: 49 8b 04 24                 	mov	rax, qword ptr [r12]
18009df80: 48 8b 8f 48 01 00 00        	mov	rcx, qword ptr [rdi + 0x148]
18009df87: 48 8b 98 28 01 00 00        	mov	rbx, qword ptr [rax + 0x128]
18009df8e: 48 8b 01                    	mov	rax, qword ptr [rcx]
18009df91: ff 50 58                    	call	qword ptr [rax + 0x58]
18009df94: 4d 8b c7                    	mov	r8, r15
18009df97: 33 d2                       	xor	edx, edx
18009df99: 49 c1 e0 08                 	shl	r8, 0x8
18009df9d: 49 8b cc                    	mov	rcx, r12
18009dfa0: 4c 03 c0                    	add	r8, rax
18009dfa3: ff d3                       	call	rbx
18009dfa5: 48 8b 8f 40 01 00 00        	mov	rcx, qword ptr [rdi + 0x140]
18009dfac: 48 8d 55 a7                 	lea	rdx, [rbp - 0x59]
18009dfb0: 48 8b 01                    	mov	rax, qword ptr [rcx]
18009dfb3: ff 50 50                    	call	qword ptr [rax + 0x50]
18009dfb6: 48 8b 45 a7                 	mov	rax, qword ptr [rbp - 0x59]
18009dfba: ba 01 00 00 00              	mov	edx, 0x1
18009dfbf: 8b 8f 60 01 00 00           	mov	ecx, dword ptr [rdi + 0x160]
18009dfc5: 49 0f af cf                 	imul	rcx, r15
18009dfc9: 4c 8d 04 88                 	lea	r8, [rax + 4*rcx]
18009dfcd: 49 8b 04 24                 	mov	rax, qword ptr [r12]
18009dfd1: 49 8b cc                    	mov	rcx, r12
18009dfd4: 4c 89 45 a7                 	mov	qword ptr [rbp - 0x59], r8
18009dfd8: ff 90 f8 00 00 00           	call	qword ptr [rax + 0xf8]
18009dfde: 44 8b 87 60 01 00 00        	mov	r8d, dword ptr [rdi + 0x160]
18009dfe5: ba 02 00 00 00              	mov	edx, 0x2
18009dfea: 48 8b 45 a7                 	mov	rax, qword ptr [rbp - 0x59]
18009dfee: 4a 8d 0c 40                 	lea	rcx, [rax + 2*r8]
18009dff2: 49 8b 04 24                 	mov	rax, qword ptr [r12]
18009dff6: 4c 03 c1                    	add	r8, rcx
18009dff9: 49 8b cc                    	mov	rcx, r12
18009dffc: ff 90 f8 00 00 00           	call	qword ptr [rax + 0xf8]
18009e002: 44 8b 85 8f 00 00 00        	mov	r8d, dword ptr [rbp + 0x8f]
18009e009: 41 b9 01 00 00 00           	mov	r9d, 0x1
18009e00f: 8b 95 87 00 00 00           	mov	edx, dword ptr [rbp + 0x87]
18009e015: 41 83 c0 07                 	add	r8d, 0x7
18009e019: 49 8b 04 24                 	mov	rax, qword ptr [r12]
18009e01d: 83 c2 07                    	add	edx, 0x7
18009e020: 41 c1 e8 03                 	shr	r8d, 0x3
18009e024: 49 8b cc                    	mov	rcx, r12
18009e027: c1 ea 03                    	shr	edx, 0x3
18009e02a: ff 50 70                    	call	qword ptr [rax + 0x70]
18009e02d: 4c 8b bc 24 18 01 00 00     	mov	r15, qword ptr [rsp + 0x118]
18009e035: 48 8b 4d f7                 	mov	rcx, qword ptr [rbp - 0x9]
18009e039: 48 33 cc                    	xor	rcx, rsp
18009e03c: e8 2f e2 06 00              	call	0x18010c270 <NVSDK_NGX_UpdateFeature+0x2e30>
18009e041: 48 81 c4 c0 00 00 00        	add	rsp, 0xc0
18009e048: 41 5e                       	pop	r14
18009e04a: 41 5d                       	pop	r13
18009e04c: 41 5c                       	pop	r12
18009e04e: 5f                          	pop	rdi
18009e04f: 5e                          	pop	rsi
18009e050: 5b                          	pop	rbx
18009e051: 5d                          	pop	rbp
18009e052: c3                          	ret
18009e053: cc                          	int3
18009e054: cc                          	int3
18009e055: cc                          	int3
18009e056: cc                          	int3
18009e057: cc                          	int3
18009e058: cc                          	int3
18009e059: cc                          	int3
18009e05a: cc                          	int3
18009e05b: cc                          	int3
18009e05c: cc                          	int3
18009e05d: cc                          	int3
18009e05e: cc                          	int3
18009e05f: cc                          	int3
