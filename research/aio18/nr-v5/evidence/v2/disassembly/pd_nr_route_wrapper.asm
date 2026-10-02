
/mnt/data/aio18_nr_work/PDPerfPlugin.dll:	file format coff-x86-64

Disassembly of section .text:

0000000180078890 <SetPDFrameWarpNativeCameraSource>:
18009f120: 40 55                       	push	rbp
18009f122: 53                          	push	rbx
18009f123: 56                          	push	rsi
18009f124: 57                          	push	rdi
18009f125: 41 54                       	push	r12
18009f127: 41 55                       	push	r13
18009f129: 41 56                       	push	r14
18009f12b: 41 57                       	push	r15
18009f12d: 48 8d ac 24 68 ff ff ff     	lea	rbp, [rsp - 0x98]
18009f135: 48 81 ec 98 01 00 00        	sub	rsp, 0x198
18009f13c: 48 8b 05 7d b8 12 01        	mov	rax, qword ptr [rip + 0x112b87d] # 0x1811ca9c0
18009f143: 48 33 c4                    	xor	rax, rsp
18009f146: 48 89 85 88 00 00 00        	mov	qword ptr [rbp + 0x88], rax
18009f14d: 48 8b 85 18 01 00 00        	mov	rax, qword ptr [rbp + 0x118]
18009f154: 4d 8b e9                    	mov	r13, r9
18009f157: 48 8b bd 00 01 00 00        	mov	rdi, qword ptr [rbp + 0x100]
18009f15e: 49 8b f0                    	mov	rsi, r8
18009f161: 4c 8b a5 10 01 00 00        	mov	r12, qword ptr [rbp + 0x110]
18009f168: 4c 8b f2                    	mov	r14, rdx
18009f16b: 4c 8b bd 08 01 00 00        	mov	r15, qword ptr [rbp + 0x108]
18009f172: 48 8b d9                    	mov	rbx, rcx
18009f175: 48 89 45 98                 	mov	qword ptr [rbp - 0x68], rax
18009f179: 48 8b 85 20 01 00 00        	mov	rax, qword ptr [rbp + 0x120]
18009f180: 48 89 45 90                 	mov	qword ptr [rbp - 0x70], rax
18009f184: 48 8b 85 28 01 00 00        	mov	rax, qword ptr [rbp + 0x128]
18009f18b: 48 89 45 a0                 	mov	qword ptr [rbp - 0x60], rax
18009f18f: 48 8b 85 30 01 00 00        	mov	rax, qword ptr [rbp + 0x130]
18009f196: 48 89 45 a8                 	mov	qword ptr [rbp - 0x58], rax
18009f19a: 48 8b 85 38 01 00 00        	mov	rax, qword ptr [rbp + 0x138]
18009f1a1: 48 89 45 88                 	mov	qword ptr [rbp - 0x78], rax
18009f1a5: 48 8b 85 40 01 00 00        	mov	rax, qword ptr [rbp + 0x140]
18009f1ac: 48 89 45 80                 	mov	qword ptr [rbp - 0x80], rax
18009f1b0: 4c 89 45 c0                 	mov	qword ptr [rbp - 0x40], r8
18009f1b4: 48 89 7d d0                 	mov	qword ptr [rbp - 0x30], rdi
18009f1b8: 4c 89 65 c8                 	mov	qword ptr [rbp - 0x38], r12
18009f1bc: e8 2f fc ff ff              	call	0x18009edf0 <SetPDFrameWarpNativeCameraSource+0x26560>
18009f1c1: 41 80 bd 03 01 00 00 00     	cmp	byte ptr [r13 + 0x103], 0x0
18009f1c9: 74 0c                       	je	0x18009f1d7 <SetPDFrameWarpNativeCameraSource+0x26947>
18009f1cb: 48 8b 45 a0                 	mov	rax, qword ptr [rbp - 0x60]
18009f1cf: 48 85 c0                    	test	rax, rax
18009f1d2: 74 03                       	je	0x18009f1d7 <SetPDFrameWarpNativeCameraSource+0x26947>
18009f1d4: 48 8b f8                    	mov	rdi, rax
18009f1d7: 48 89 7c 24 78              	mov	qword ptr [rsp + 0x78], rdi
18009f1dc: 4d 85 f6                    	test	r14, r14
18009f1df: 0f 84 3c 06 00 00           	je	0x18009f821 <SetPDFrameWarpNativeCameraSource+0x26f91>
18009f1e5: 48 85 f6                    	test	rsi, rsi
18009f1e8: 0f 84 33 06 00 00           	je	0x18009f821 <SetPDFrameWarpNativeCameraSource+0x26f91>
18009f1ee: 48 85 ff                    	test	rdi, rdi
18009f1f1: 0f 84 2a 06 00 00           	je	0x18009f821 <SetPDFrameWarpNativeCameraSource+0x26f91>
18009f1f7: 4d 85 ff                    	test	r15, r15
18009f1fa: 0f 84 21 06 00 00           	je	0x18009f821 <SetPDFrameWarpNativeCameraSource+0x26f91>
18009f200: 41 83 bd 30 01 00 00 00     	cmp	dword ptr [r13 + 0x130], 0x0
18009f208: 75 29                       	jne	0x18009f233 <SetPDFrameWarpNativeCameraSource+0x269a3>
18009f20a: 41 80 bd 00 01 00 00 00     	cmp	byte ptr [r13 + 0x100], 0x0
18009f212: 75 1f                       	jne	0x18009f233 <SetPDFrameWarpNativeCameraSource+0x269a3>
18009f214: 48 8b 8b 98 01 00 00        	mov	rcx, qword ptr [rbx + 0x198]
18009f21b: 49 8b d7                    	mov	rdx, r15
18009f21e: e8 fd dc ff ff              	call	0x18009cf20 <SetPDFrameWarpNativeCameraSource+0x24690>
18009f223: 84 c0                       	test	al, al
18009f225: 74 0c                       	je	0x18009f233 <SetPDFrameWarpNativeCameraSource+0x269a3>
18009f227: 48 8b bb 98 01 00 00        	mov	rdi, qword ptr [rbx + 0x198]
18009f22e: 48 89 7c 24 78              	mov	qword ptr [rsp + 0x78], rdi
18009f233: 41 8b 8d 30 01 00 00        	mov	ecx, dword ptr [r13 + 0x130]
18009f23a: e8 71 dd ff ff              	call	0x18009cfb0 <SetPDFrameWarpNativeCameraSource+0x24720>
18009f23f: 8b d0                       	mov	edx, eax
18009f241: 48 8b cb                    	mov	rcx, rbx
18009f244: e8 27 29 00 00              	call	0x1800a1b70 <SetPDFrameWarpNativeCameraSource+0x292e0>
18009f249: 41 80 bd 00 01 00 00 00     	cmp	byte ptr [r13 + 0x100], 0x0
18009f251: 89 44 24 74                 	mov	dword ptr [rsp + 0x74], eax
18009f255: 74 2e                       	je	0x18009f285 <SetPDFrameWarpNativeCameraSource+0x269f5>
18009f257: c7 83 c9 00 00 00 01 01 01 01       	mov	dword ptr [rbx + 0xc9], 0x1010101
18009f261: c7 83 cd 00 00 00 01 01 01 01       	mov	dword ptr [rbx + 0xcd], 0x1010101
18009f26b: c7 83 d1 00 00 00 01 01 01 01       	mov	dword ptr [rbx + 0xd1], 0x1010101
18009f275: 66 c7 83 d5 00 00 00 01 01  	mov	word ptr [rbx + 0xd5], 0x101
18009f27e: c6 83 d7 00 00 00 01        	mov	byte ptr [rbx + 0xd7], 0x1
18009f285: 49 8b d6                    	mov	rdx, r14
18009f288: 48 8b cb                    	mov	rcx, rbx
18009f28b: 83 f8 01                    	cmp	eax, 0x1
18009f28e: 0f 86 06 02 00 00           	jbe	0x18009f49a <SetPDFrameWarpNativeCameraSource+0x26c0a>
18009f294: 0f 57 c0                    	xorps	xmm0, xmm0
18009f297: c7 45 40 00 00 80 3f        	mov	dword ptr [rbp + 0x40], 0x3f800000
18009f29e: 48 8d 45 10                 	lea	rax, [rbp + 0x10]
18009f2a2: 66 0f 7f 45 10              	movdqa	xmmword ptr [rbp + 0x10], xmm0
18009f2a7: 0f 57 c9                    	xorps	xmm1, xmm1
18009f2aa: 48 89 44 24 28              	mov	qword ptr [rsp + 0x28], rax
18009f2af: 4c 8b cf                    	mov	r9, rdi
18009f2b2: 4c 89 7c 24 20              	mov	qword ptr [rsp + 0x20], r15
18009f2b7: 4d 8b c5                    	mov	r8, r13
18009f2ba: 66 0f 7f 4d 20              	movdqa	xmmword ptr [rbp + 0x20], xmm1
18009f2bf: 66 0f 7f 45 30              	movdqa	xmmword ptr [rbp + 0x30], xmm0
18009f2c4: 48 c7 45 44 00 00 80 3f     	mov	qword ptr [rbp + 0x44], 0x3f800000
18009f2cc: 66 c7 45 4c 00 00           	mov	word ptr [rbp + 0x4c], 0x0
18009f2d2: e8 89 ed ff ff              	call	0x18009e060 <SetPDFrameWarpNativeCameraSource+0x257d0>
18009f2d7: 84 c0                       	test	al, al
18009f2d9: 0f 84 42 05 00 00           	je	0x18009f821 <SetPDFrameWarpNativeCameraSource+0x26f91>
18009f2df: 48 8b 07                    	mov	rax, qword ptr [rdi]
18009f2e2: 48 8d 55 50                 	lea	rdx, [rbp + 0x50]
18009f2e6: 48 8b cf                    	mov	rcx, rdi
18009f2e9: ff 50 50                    	call	qword ptr [rax + 0x50]
18009f2ec: 8b 45 70                    	mov	eax, dword ptr [rbp + 0x70]
18009f2ef: 48 8d 93 70 01 00 00        	lea	rdx, [rbx + 0x170]
18009f2f6: 44 8b 4d 3c                 	mov	r9d, dword ptr [rbp + 0x3c]
18009f2fa: 48 8b cb                    	mov	rcx, rbx
18009f2fd: 44 8b 45 38                 	mov	r8d, dword ptr [rbp + 0x38]
18009f301: c6 44 24 28 01              	mov	byte ptr [rsp + 0x28], 0x1
18009f306: 89 44 24 20                 	mov	dword ptr [rsp + 0x20], eax
18009f30a: e8 81 e7 ff ff              	call	0x18009da90 <SetPDFrameWarpNativeCameraSource+0x25200>
18009f30f: 48 85 c0                    	test	rax, rax
18009f312: 0f 84 70 01 00 00           	je	0x18009f488 <SetPDFrameWarpNativeCameraSource+0x26bf8>
18009f318: 8b 45 70                    	mov	eax, dword ptr [rbp + 0x70]
18009f31b: 48 8d 93 a8 01 00 00        	lea	rdx, [rbx + 0x1a8]
18009f322: 44 8b 4d 3c                 	mov	r9d, dword ptr [rbp + 0x3c]
18009f326: 48 8b cb                    	mov	rcx, rbx
18009f329: 44 8b 45 38                 	mov	r8d, dword ptr [rbp + 0x38]
18009f32d: c6 44 24 28 01              	mov	byte ptr [rsp + 0x28], 0x1
18009f332: 89 44 24 20                 	mov	dword ptr [rsp + 0x20], eax
18009f336: e8 55 e7 ff ff              	call	0x18009da90 <SetPDFrameWarpNativeCameraSource+0x25200>
18009f33b: 48 85 c0                    	test	rax, rax
18009f33e: 0f 84 44 01 00 00           	je	0x18009f488 <SetPDFrameWarpNativeCameraSource+0x26bf8>
18009f344: 48 8b 83 70 01 00 00        	mov	rax, qword ptr [rbx + 0x170]
18009f34b: 48 8d b3 c8 00 00 00        	lea	rsi, [rbx + 0xc8]
18009f352: 48 89 45 b0                 	mov	qword ptr [rbp - 0x50], rax
18009f356: 4c 8d 63 48                 	lea	r12, [rbx + 0x48]
18009f35a: 48 8b 83 a8 01 00 00        	mov	rax, qword ptr [rbx + 0x1a8]
18009f361: 48 89 45 b8                 	mov	qword ptr [rbp - 0x48], rax
18009f365: 33 c0                       	xor	eax, eax
18009f367: 89 44 24 70                 	mov	dword ptr [rsp + 0x70], eax
18009f36b: 33 ff                       	xor	edi, edi
18009f36d: 0f 1f 00                    	nop	dword ptr [rax]
18009f370: 8b cf                       	mov	ecx, edi
18009f372: 85 ff                       	test	edi, edi
18009f374: 75 11                       	jne	0x18009f387 <SetPDFrameWarpNativeCameraSource+0x26af7>
18009f376: 48 8b 55 20                 	mov	rdx, qword ptr [rbp + 0x20]
18009f37a: 4d 8d 8d 00 01 00 00        	lea	r9, [r13 + 0x100]
18009f381: 4c 8b 45 c0                 	mov	r8, qword ptr [rbp - 0x40]
18009f385: eb 1b                       	jmp	0x18009f3a2 <SetPDFrameWarpNativeCameraSource+0x26b12>
18009f387: 4d 8b 04 24                 	mov	r8, qword ptr [r12]
18009f38b: 4d 85 c0                    	test	r8, r8
18009f38e: 0f 84 b7 00 00 00           	je	0x18009f44b <SetPDFrameWarpNativeCameraSource+0x26bbb>
18009f394: 8d 47 ff                    	lea	eax, [rdi - 0x1]
18009f397: 4c 8b ce                    	mov	r9, rsi
18009f39a: 83 e0 01                    	and	eax, 0x1
18009f39d: 48 8b 54 c5 b0              	mov	rdx, qword ptr [rbp + 8*rax - 0x50]
18009f3a2: 41 0f b6 01                 	movzx	eax, byte ptr [r9]
18009f3a6: 83 e1 01                    	and	ecx, 0x1
18009f3a9: 88 44 24 60                 	mov	byte ptr [rsp + 0x60], al
18009f3ad: 4d 8b cd                    	mov	r9, r13
18009f3b0: 48 8b 45 80                 	mov	rax, qword ptr [rbp - 0x80]
18009f3b4: 48 89 44 24 58              	mov	qword ptr [rsp + 0x58], rax
18009f3b9: 4c 8b 54 cd b0              	mov	r10, qword ptr [rbp + 8*rcx - 0x50]
18009f3be: 48 8b cb                    	mov	rcx, rbx
18009f3c1: 48 8b 45 88                 	mov	rax, qword ptr [rbp - 0x78]
18009f3c5: 48 89 44 24 50              	mov	qword ptr [rsp + 0x50], rax
18009f3ca: 48 8b 45 90                 	mov	rax, qword ptr [rbp - 0x70]
18009f3ce: 48 89 44 24 48              	mov	qword ptr [rsp + 0x48], rax
18009f3d3: 48 8b 45 98                 	mov	rax, qword ptr [rbp - 0x68]
18009f3d7: 48 89 44 24 40              	mov	qword ptr [rsp + 0x40], rax
18009f3dc: 48 8b 45 c8                 	mov	rax, qword ptr [rbp - 0x38]
18009f3e0: 48 89 44 24 38              	mov	qword ptr [rsp + 0x38], rax
18009f3e5: 48 8d 45 10                 	lea	rax, [rbp + 0x10]
18009f3e9: 4c 89 54 24 30              	mov	qword ptr [rsp + 0x30], r10
18009f3ee: 48 89 54 24 28              	mov	qword ptr [rsp + 0x28], rdx
18009f3f3: 49 8b d6                    	mov	rdx, r14
18009f3f6: 48 89 44 24 20              	mov	qword ptr [rsp + 0x20], rax
18009f3fb: e8 a0 f1 ff ff              	call	0x18009e5a0 <SetPDFrameWarpNativeCameraSource+0x25d10>
18009f400: 84 c0                       	test	al, al
18009f402: 74 23                       	je	0x18009f427 <SetPDFrameWarpNativeCameraSource+0x26b97>
18009f404: 85 ff                       	test	edi, edi
18009f406: 74 03                       	je	0x18009f40b <SetPDFrameWarpNativeCameraSource+0x26b7b>
18009f408: c6 06 00                    	mov	byte ptr [rsi], 0x0
18009f40b: 8d 47 01                    	lea	eax, [rdi + 0x1]
18009f40e: 49 83 c4 08                 	add	r12, 0x8
18009f412: 48 ff c6                    	inc	rsi
18009f415: 89 44 24 70                 	mov	dword ptr [rsp + 0x70], eax
18009f419: 8b f8                       	mov	edi, eax
18009f41b: 3b 44 24 74                 	cmp	eax, dword ptr [rsp + 0x74]
18009f41f: 0f 82 4b ff ff ff           	jb	0x18009f370 <SetPDFrameWarpNativeCameraSource+0x26ae0>
18009f425: eb 24                       	jmp	0x18009f44b <SetPDFrameWarpNativeCameraSource+0x26bbb>
18009f427: 85 ff                       	test	edi, edi
18009f429: 0f 84 f2 03 00 00           	je	0x18009f821 <SetPDFrameWarpNativeCameraSource+0x26f91>
18009f42f: 8d 57 01                    	lea	edx, [rdi + 0x1]
18009f432: 44 8b c7                    	mov	r8d, edi
18009f435: 48 8d 0d ec 52 10 01        	lea	rcx, [rip + 0x11052ec]  # 0x1811a4728    ; STRING: DLSSNR: extra pass %u failed, keeping %u pass(es)
18009f43c: e8 ff c6 05 00              	call	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
18009f441: 8b 44 24 70                 	mov	eax, dword ptr [rsp + 0x70]
18009f445: 89 bb d8 00 00 00           	mov	dword ptr [rbx + 0xd8], edi
18009f44b: 85 c0                       	test	eax, eax
18009f44d: 0f 84 ce 03 00 00           	je	0x18009f821 <SetPDFrameWarpNativeCameraSource+0x26f91>
18009f453: ff c8                       	dec	eax
18009f455: 4c 89 7c 24 28              	mov	qword ptr [rsp + 0x28], r15
18009f45a: 83 e0 01                    	and	eax, 0x1
18009f45d: 4c 8d 4d 10                 	lea	r9, [rbp + 0x10]
18009f461: 4d 8b c5                    	mov	r8, r13
18009f464: 49 8b d6                    	mov	rdx, r14
18009f467: 48 8b cb                    	mov	rcx, rbx
18009f46a: 48 8b 44 c5 b0              	mov	rax, qword ptr [rbp + 8*rax - 0x50]
18009f46f: 48 89 44 24 20              	mov	qword ptr [rsp + 0x20], rax
18009f474: e8 97 f4 ff ff              	call	0x18009e910 <SetPDFrameWarpNativeCameraSource+0x26080>
18009f479: 84 c0                       	test	al, al
18009f47b: 0f 84 a0 03 00 00           	je	0x18009f821 <SetPDFrameWarpNativeCameraSource+0x26f91>
18009f481: 48 8b 7c 24 78              	mov	rdi, qword ptr [rsp + 0x78]
18009f486: eb 58                       	jmp	0x18009f4e0 <SetPDFrameWarpNativeCameraSource+0x26c50>
18009f488: 48 8d 0d e9 51 10 01        	lea	rcx, [rip + 0x11051e9]  # 0x1811a4678    ; STRING: DLSSNR: work-surface alloc failed, falling back to one pass
18009f48f: e8 ac c6 05 00              	call	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
18009f494: 49 8b d6                    	mov	rdx, r14
18009f497: 48 8b cb                    	mov	rcx, rbx
18009f49a: 48 8b 45 80                 	mov	rax, qword ptr [rbp - 0x80]
18009f49e: 4d 8b cd                    	mov	r9, r13
18009f4a1: 48 89 44 24 50              	mov	qword ptr [rsp + 0x50], rax
18009f4a6: 4c 8b c6                    	mov	r8, rsi
18009f4a9: 48 8b 45 88                 	mov	rax, qword ptr [rbp - 0x78]
18009f4ad: 48 89 44 24 48              	mov	qword ptr [rsp + 0x48], rax
18009f4b2: 48 8b 45 90                 	mov	rax, qword ptr [rbp - 0x70]
18009f4b6: 48 89 44 24 40              	mov	qword ptr [rsp + 0x40], rax
18009f4bb: 48 8b 45 98                 	mov	rax, qword ptr [rbp - 0x68]
18009f4bf: 48 89 44 24 38              	mov	qword ptr [rsp + 0x38], rax
18009f4c4: 4c 89 64 24 30              	mov	qword ptr [rsp + 0x30], r12
18009f4c9: 4c 89 7c 24 28              	mov	qword ptr [rsp + 0x28], r15
18009f4ce: 48 89 7c 24 20              	mov	qword ptr [rsp + 0x20], rdi
18009f4d3: e8 a8 f7 ff ff              	call	0x18009ec80 <SetPDFrameWarpNativeCameraSource+0x263f0>
18009f4d8: 84 c0                       	test	al, al
18009f4da: 0f 84 41 03 00 00           	je	0x18009f821 <SetPDFrameWarpNativeCameraSource+0x26f91>
18009f4e0: 49 8b 07                    	mov	rax, qword ptr [r15]
18009f4e3: 48 8d 55 10                 	lea	rdx, [rbp + 0x10]
18009f4e7: 49 8b cf                    	mov	rcx, r15
18009f4ea: ff 50 50                    	call	qword ptr [rax + 0x50]
18009f4ed: 8b 45 30                    	mov	eax, dword ptr [rbp + 0x30]
18009f4f0: 48 8d 93 98 01 00 00        	lea	rdx, [rbx + 0x198]
18009f4f7: 44 8b 4d 28                 	mov	r9d, dword ptr [rbp + 0x28]
18009f4fb: 48 8b cb                    	mov	rcx, rbx
18009f4fe: 44 8b 45 20                 	mov	r8d, dword ptr [rbp + 0x20]
18009f502: c6 44 24 28 00              	mov	byte ptr [rsp + 0x28], 0x0
18009f507: 89 44 24 20                 	mov	dword ptr [rsp + 0x20], eax
18009f50b: e8 80 e5 ff ff              	call	0x18009da90 <SetPDFrameWarpNativeCameraSource+0x25200>
18009f510: 48 85 c0                    	test	rax, rax
18009f513: 74 78                       	je	0x18009f58d <SetPDFrameWarpNativeCameraSource+0x26cfd>
18009f515: 48 8b 0d d4 63 17 01        	mov	rcx, qword ptr [rip + 0x11763d4] # 0x1812158f0
18009f51c: 45 33 e4                    	xor	r12d, r12d
18009f51f: 41 b9 00 08 00 00           	mov	r9d, 0x800
18009f525: 44 89 64 24 20              	mov	dword ptr [rsp + 0x20], r12d
18009f52a: 4d 8b c7                    	mov	r8, r15
18009f52d: 49 8b d6                    	mov	rdx, r14
18009f530: e8 cb 62 fd ff              	call	0x180075800 <SetPDFrameWarpDiagnosticHud+0x4cdb0>
18009f535: 4c 8b 83 98 01 00 00        	mov	r8, qword ptr [rbx + 0x198]
18009f53c: 41 b9 00 04 00 00           	mov	r9d, 0x400
18009f542: 48 8b 0d a7 63 17 01        	mov	rcx, qword ptr [rip + 0x11763a7] # 0x1812158f0
18009f549: 49 8b d6                    	mov	rdx, r14
18009f54c: 44 89 64 24 20              	mov	dword ptr [rsp + 0x20], r12d
18009f551: e8 aa 62 fd ff              	call	0x180075800 <SetPDFrameWarpDiagnosticHud+0x4cdb0>
18009f556: 49 8b 06                    	mov	rax, qword ptr [r14]
18009f559: 4d 8b c7                    	mov	r8, r15
18009f55c: 48 8b 93 98 01 00 00        	mov	rdx, qword ptr [rbx + 0x198]
18009f563: 49 8b ce                    	mov	rcx, r14
18009f566: ff 90 88 00 00 00           	call	qword ptr [rax + 0x88]
18009f56c: 4c 8b 83 98 01 00 00        	mov	r8, qword ptr [rbx + 0x198]
18009f573: 41 b9 40 00 00 00           	mov	r9d, 0x40
18009f579: 48 8b 0d 70 63 17 01        	mov	rcx, qword ptr [rip + 0x1176370] # 0x1812158f0
18009f580: 49 8b d6                    	mov	rdx, r14
18009f583: 44 89 64 24 20              	mov	dword ptr [rsp + 0x20], r12d
18009f588: e8 73 62 fd ff              	call	0x180075800 <SetPDFrameWarpDiagnosticHud+0x4cdb0>
18009f58d: 80 bb b5 01 00 00 00        	cmp	byte ptr [rbx + 0x1b5], 0x0
18009f594: 74 31                       	je	0x18009f5c7 <SetPDFrameWarpNativeCameraSource+0x26d37>
18009f596: 4c 8b 45 d0                 	mov	r8, qword ptr [rbp - 0x30]
18009f59a: 4d 85 c0                    	test	r8, r8
18009f59d: 74 15                       	je	0x18009f5b4 <SetPDFrameWarpNativeCameraSource+0x26d24>
18009f59f: 4d 8b cf                    	mov	r9, r15
18009f5a2: 49 8b d6                    	mov	rdx, r14
18009f5a5: 48 8b cb                    	mov	rcx, rbx
18009f5a8: e8 73 f9 ff ff              	call	0x18009ef20 <SetPDFrameWarpNativeCameraSource+0x26690>
18009f5ad: b0 01                       	mov	al, 0x1
18009f5af: e9 6f 02 00 00              	jmp	0x18009f823 <SetPDFrameWarpNativeCameraSource+0x26f93>
18009f5b4: 48 8d 0d 2d 51 10 01        	lea	rcx, [rip + 0x110512d]  # 0x1811a46e8    ; STRING: DLSSNR debug: color overwrite requested but color is null
18009f5bb: e8 80 c5 05 00              	call	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
18009f5c0: b0 01                       	mov	al, 0x1
18009f5c2: e9 5c 02 00 00              	jmp	0x18009f823 <SetPDFrameWarpNativeCameraSource+0x26f93>
18009f5c7: 80 bb b6 01 00 00 00        	cmp	byte ptr [rbx + 0x1b6], 0x0
18009f5ce: 74 34                       	je	0x18009f604 <SetPDFrameWarpNativeCameraSource+0x26d74>
18009f5d0: 48 8b 45 a8                 	mov	rax, qword ptr [rbp - 0x58]
18009f5d4: 48 85 c0                    	test	rax, rax
18009f5d7: 74 18                       	je	0x18009f5f1 <SetPDFrameWarpNativeCameraSource+0x26d61>
18009f5d9: 4d 8b cf                    	mov	r9, r15
18009f5dc: 4c 8b c0                    	mov	r8, rax
18009f5df: 49 8b d6                    	mov	rdx, r14
18009f5e2: 48 8b cb                    	mov	rcx, rbx
18009f5e5: e8 36 f9 ff ff              	call	0x18009ef20 <SetPDFrameWarpNativeCameraSource+0x26690>
18009f5ea: b0 01                       	mov	al, 0x1
18009f5ec: e9 32 02 00 00              	jmp	0x18009f823 <SetPDFrameWarpNativeCameraSource+0x26f93>
18009f5f1: 48 8d 0d a8 51 10 01        	lea	rcx, [rip + 0x11051a8]  # 0x1811a47a0    ; STRING: DLSSNR debug: UIAlpha overwrite requested but uiAlpha is null
18009f5f8: e8 43 c5 05 00              	call	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
18009f5fd: b0 01                       	mov	al, 0x1
18009f5ff: e9 1f 02 00 00              	jmp	0x18009f823 <SetPDFrameWarpNativeCameraSource+0x26f93>
18009f604: 41 80 bd 03 01 00 00 00     	cmp	byte ptr [r13 + 0x103], 0x0
18009f60c: 74 ef                       	je	0x18009f5fd <SetPDFrameWarpNativeCameraSource+0x26d6d>
18009f60e: 48 83 7d a0 00              	cmp	qword ptr [rbp - 0x60], 0x0
18009f613: 74 e8                       	je	0x18009f5fd <SetPDFrameWarpNativeCameraSource+0x26d6d>
18009f615: 4c 8b 65 a8                 	mov	r12, qword ptr [rbp - 0x58]
18009f619: 4d 85 e4                    	test	r12, r12
18009f61c: 74 df                       	je	0x18009f5fd <SetPDFrameWarpNativeCameraSource+0x26d6d>
18009f61e: 48 8b cb                    	mov	rcx, rbx
18009f621: e8 8a de ff ff              	call	0x18009d4b0 <SetPDFrameWarpNativeCameraSource+0x24c20>
18009f626: 84 c0                       	test	al, al
18009f628: 75 13                       	jne	0x18009f63d <SetPDFrameWarpNativeCameraSource+0x26dad>
18009f62a: 48 8d 0d 2f 51 10 01        	lea	rcx, [rip + 0x110512f]  # 0x1811a4760    ; STRING: DLSSNR: UI compose pipeline unavailable, skipping UI composite
18009f631: e8 0a c5 05 00              	call	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
18009f636: b0 01                       	mov	al, 0x1
18009f638: e9 e6 01 00 00              	jmp	0x18009f823 <SetPDFrameWarpNativeCameraSource+0x26f93>
18009f63d: 49 8b 07                    	mov	rax, qword ptr [r15]
18009f640: 48 8d 55 10                 	lea	rdx, [rbp + 0x10]
18009f644: 49 8b cf                    	mov	rcx, r15
18009f647: ff 50 50                    	call	qword ptr [rax + 0x50]
18009f64a: 8b 45 30                    	mov	eax, dword ptr [rbp + 0x30]
18009f64d: 48 8d 93 90 01 00 00        	lea	rdx, [rbx + 0x190]
18009f654: 44 8b 4d 28                 	mov	r9d, dword ptr [rbp + 0x28]
18009f658: 48 8b cb                    	mov	rcx, rbx
18009f65b: 44 8b 45 20                 	mov	r8d, dword ptr [rbp + 0x20]
18009f65f: c6 44 24 28 00              	mov	byte ptr [rsp + 0x28], 0x0
18009f664: 89 44 24 20                 	mov	dword ptr [rsp + 0x20], eax
18009f668: e8 23 e4 ff ff              	call	0x18009da90 <SetPDFrameWarpNativeCameraSource+0x25200>
18009f66d: 48 85 c0                    	test	rax, rax
18009f670: 75 13                       	jne	0x18009f685 <SetPDFrameWarpNativeCameraSource+0x26df5>
18009f672: 48 8d 0d 7f 51 10 01        	lea	rcx, [rip + 0x110517f]  # 0x1811a47f8    ; STRING: DLSSNR: UI compose scratch alloc failed, skipping UI composite
18009f679: e8 c2 c4 05 00              	call	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
18009f67e: b0 01                       	mov	al, 0x1
18009f680: e9 9e 01 00 00              	jmp	0x18009f823 <SetPDFrameWarpNativeCameraSource+0x26f93>
18009f685: 48 8b 0d 64 62 17 01        	mov	rcx, qword ptr [rip + 0x1176264] # 0x1812158f0
18009f68c: 41 b9 00 08 00 00           	mov	r9d, 0x800
18009f692: 4d 8b c7                    	mov	r8, r15
18009f695: c7 44 24 20 00 00 00 00     	mov	dword ptr [rsp + 0x20], 0x0
18009f69d: 49 8b d6                    	mov	rdx, r14
18009f6a0: e8 5b 61 fd ff              	call	0x180075800 <SetPDFrameWarpDiagnosticHud+0x4cdb0>
18009f6a5: 4c 8b 83 90 01 00 00        	mov	r8, qword ptr [rbx + 0x190]
18009f6ac: 41 b9 00 04 00 00           	mov	r9d, 0x400
18009f6b2: 48 8b 0d 37 62 17 01        	mov	rcx, qword ptr [rip + 0x1176237] # 0x1812158f0
18009f6b9: 49 8b d6                    	mov	rdx, r14
18009f6bc: c7 44 24 20 00 00 00 00     	mov	dword ptr [rsp + 0x20], 0x0
18009f6c4: e8 37 61 fd ff              	call	0x180075800 <SetPDFrameWarpDiagnosticHud+0x4cdb0>
18009f6c9: 49 8b 06                    	mov	rax, qword ptr [r14]
18009f6cc: 4d 8b c7                    	mov	r8, r15
18009f6cf: 48 8b 93 90 01 00 00        	mov	rdx, qword ptr [rbx + 0x190]
18009f6d6: 49 8b ce                    	mov	rcx, r14
18009f6d9: ff 90 88 00 00 00           	call	qword ptr [rax + 0x88]
18009f6df: 4c 8b 83 90 01 00 00        	mov	r8, qword ptr [rbx + 0x190]
18009f6e6: 41 b9 40 00 00 00           	mov	r9d, 0x40
18009f6ec: 48 8b 0d fd 61 17 01        	mov	rcx, qword ptr [rip + 0x11761fd] # 0x1812158f0
18009f6f3: 49 8b d6                    	mov	rdx, r14
18009f6f6: c7 44 24 20 00 00 00 00     	mov	dword ptr [rsp + 0x20], 0x0
18009f6fe: e8 fd 60 fd ff              	call	0x180075800 <SetPDFrameWarpDiagnosticHud+0x4cdb0>
18009f703: 48 8b 0d e6 61 17 01        	mov	rcx, qword ptr [rip + 0x11761e6] # 0x1812158f0
18009f70a: 41 b9 40 00 00 00           	mov	r9d, 0x40
18009f710: 4d 8b c4                    	mov	r8, r12
18009f713: c7 44 24 20 00 00 00 00     	mov	dword ptr [rsp + 0x20], 0x0
18009f71b: 49 8b d6                    	mov	rdx, r14
18009f71e: e8 dd 60 fd ff              	call	0x180075800 <SetPDFrameWarpDiagnosticHud+0x4cdb0>
18009f723: 48 8b 0d c6 61 17 01        	mov	rcx, qword ptr [rip + 0x11761c6] # 0x1812158f0
18009f72a: 41 b9 08 00 00 00           	mov	r9d, 0x8
18009f730: 4d 8b c7                    	mov	r8, r15
18009f733: c7 44 24 20 00 00 00 00     	mov	dword ptr [rsp + 0x20], 0x0
18009f73b: 49 8b d6                    	mov	rdx, r14
18009f73e: e8 bd 60 fd ff              	call	0x180075800 <SetPDFrameWarpDiagnosticHud+0x4cdb0>
18009f743: 8b 45 20                    	mov	eax, dword ptr [rbp + 0x20]
18009f746: 4c 8d 4c 24 70              	lea	r9, [rsp + 0x70]
18009f74b: 89 44 24 70                 	mov	dword ptr [rsp + 0x70], eax
18009f74f: 4c 8b c7                    	mov	r8, rdi
18009f752: 8b 45 28                    	mov	eax, dword ptr [rbp + 0x28]
18009f755: 49 8b d5                    	mov	rdx, r13
18009f758: 89 44 24 74                 	mov	dword ptr [rsp + 0x74], eax
18009f75c: 48 8b cb                    	mov	rcx, rbx
18009f75f: 48 8d 44 24 74              	lea	rax, [rsp + 0x74]
18009f764: 48 89 44 24 20              	mov	qword ptr [rsp + 0x20], rax
18009f769: e8 a2 d8 ff ff              	call	0x18009d010 <SetPDFrameWarpNativeCameraSource+0x24780>
18009f76e: 49 8b 04 24                 	mov	rax, qword ptr [r12]
18009f772: 48 8d 55 50                 	lea	rdx, [rbp + 0x50]
18009f776: 49 8b cc                    	mov	rcx, r12
18009f779: ff 50 50                    	call	qword ptr [rax + 0x50]
18009f77c: 44 8b 4c 24 70              	mov	r9d, dword ptr [rsp + 0x70]
18009f781: 45 33 c0                    	xor	r8d, r8d
18009f784: 8b 54 24 74                 	mov	edx, dword ptr [rsp + 0x74]
18009f788: 66 0f 6f 05 50 dc 10 01     	movdqa	xmm0, xmmword ptr [rip + 0x110dc50] # 0x1811ad3e0
18009f790: 8b 4d 30                    	mov	ecx, dword ptr [rbp + 0x30]
18009f793: 4c 89 45 f0                 	mov	qword ptr [rbp - 0x10], r8
18009f797: 0f 11 45 00                 	movups	xmmword ptr [rbp], xmm0
18009f79b: 44 89 4d d8                 	mov	dword ptr [rbp - 0x28], r9d
18009f79f: 89 55 dc                    	mov	dword ptr [rbp - 0x24], edx
18009f7a2: 44 89 4d e0                 	mov	dword ptr [rbp - 0x20], r9d
18009f7a6: 89 55 e4                    	mov	dword ptr [rbp - 0x1c], edx
18009f7a9: e8 92 d6 ff ff              	call	0x18009ce40 <SetPDFrameWarpNativeCameraSource+0x245b0>
18009f7ae: 0f b6 c8                    	movzx	ecx, al
18009f7b1: 89 4d e8                    	mov	dword ptr [rbp - 0x18], ecx
18009f7b4: 8b 4d 70                    	mov	ecx, dword ptr [rbp + 0x70]
18009f7b7: e8 84 d6 ff ff              	call	0x18009ce40 <SetPDFrameWarpNativeCameraSource+0x245b0>
18009f7bc: 89 54 24 48                 	mov	dword ptr [rsp + 0x48], edx
18009f7c0: 44 89 4c 24 40              	mov	dword ptr [rsp + 0x40], r9d
18009f7c5: 4c 8d 4d d8                 	lea	r9, [rbp - 0x28]
18009f7c9: 0f b6 c8                    	movzx	ecx, al
18009f7cc: 8b 45 60                    	mov	eax, dword ptr [rbp + 0x60]
18009f7cf: 4c 89 7c 24 38              	mov	qword ptr [rsp + 0x38], r15
18009f7d4: 4c 89 44 24 30              	mov	qword ptr [rsp + 0x30], r8
18009f7d9: 4c 8b 83 38 01 00 00        	mov	r8, qword ptr [rbx + 0x138]
18009f7e0: 89 45 f8                    	mov	dword ptr [rbp - 0x8], eax
18009f7e3: 8b 45 68                    	mov	eax, dword ptr [rbp + 0x68]
18009f7e6: 89 45 fc                    	mov	dword ptr [rbp - 0x4], eax
18009f7e9: 48 8b 83 90 01 00 00        	mov	rax, qword ptr [rbx + 0x190]
18009f7f0: 4c 89 64 24 28              	mov	qword ptr [rsp + 0x28], r12
18009f7f5: 48 89 44 24 20              	mov	qword ptr [rsp + 0x20], rax
18009f7fa: 89 4d ec                    	mov	dword ptr [rbp - 0x14], ecx
18009f7fd: 49 8b d6                    	mov	rdx, r14
18009f800: 48 8b cb                    	mov	rcx, rbx
18009f803: e8 98 e4 ff ff              	call	0x18009dca0 <SetPDFrameWarpNativeCameraSource+0x25410>
18009f808: 48 8b 0d e1 60 17 01        	mov	rcx, qword ptr [rip + 0x11760e1] # 0x1812158f0
18009f80f: 41 b8 08 00 00 00           	mov	r8d, 0x8
18009f815: 49 8b d7                    	mov	rdx, r15
18009f818: e8 e3 5e fd ff              	call	0x180075700 <SetPDFrameWarpDiagnosticHud+0x4ccb0>
18009f81d: b0 01                       	mov	al, 0x1
18009f81f: eb 02                       	jmp	0x18009f823 <SetPDFrameWarpNativeCameraSource+0x26f93>
18009f821: 32 c0                       	xor	al, al
18009f823: 48 8b 8d 88 00 00 00        	mov	rcx, qword ptr [rbp + 0x88]
18009f82a: 48 33 cc                    	xor	rcx, rsp
18009f82d: e8 3e ca 06 00              	call	0x18010c270 <NVSDK_NGX_UpdateFeature+0x2e30>
18009f832: 48 81 c4 98 01 00 00        	add	rsp, 0x198
18009f839: 41 5f                       	pop	r15
18009f83b: 41 5e                       	pop	r14
18009f83d: 41 5d                       	pop	r13
18009f83f: 41 5c                       	pop	r12
18009f841: 5f                          	pop	rdi
18009f842: 5e                          	pop	rsi
18009f843: 5b                          	pop	rbx
18009f844: 5d                          	pop	rbp
18009f845: c3                          	ret
18009f846: cc                          	int3
18009f847: cc                          	int3
18009f848: cc                          	int3
18009f849: cc                          	int3
18009f84a: cc                          	int3
18009f84b: cc                          	int3
18009f84c: cc                          	int3
18009f84d: cc                          	int3
18009f84e: cc                          	int3
18009f84f: cc                          	int3
18009f850: 48 85 c9                    	test	rcx, rcx
18009f853: 0f 84 c1 00 00 00           	je	0x18009f91a <SetPDFrameWarpNativeCameraSource+0x2708a>
18009f859: 53                          	push	rbx
18009f85a: 48 83 ec 70                 	sub	rsp, 0x70
18009f85e: 48 8b 05 5b b1 12 01        	mov	rax, qword ptr [rip + 0x112b15b] # 0x1811ca9c0
18009f865: 48 33 c4                    	xor	rax, rsp
18009f868: 48 89 44 24 68              	mov	qword ptr [rsp + 0x68], rax
18009f86d: 4c 8b 9c 24 a0 00 00 00     	mov	r11, qword ptr [rsp + 0xa0]
18009f875: 48 8b da                    	mov	rbx, rdx
18009f878: 4c 8b d1                    	mov	r10, rcx
18009f87b: 4d 85 c0                    	test	r8, r8
18009f87e: 0f 84 84 00 00 00           	je	0x18009f908 <SetPDFrameWarpNativeCameraSource+0x27078>
18009f884: 49 8b 11                    	mov	rdx, qword ptr [r9]
18009f887: 48 85 d2                    	test	rdx, rdx
18009f88a: 74 7c                       	je	0x18009f908 <SetPDFrameWarpNativeCameraSource+0x27078>
18009f88c: 80 3b 00                    	cmp	byte ptr [rbx], 0x0
18009f88f: 75 77                       	jne	0x18009f908 <SetPDFrameWarpNativeCameraSource+0x27078>
18009f891: 49 3b d0                    	cmp	rdx, r8
18009f894: 74 72                       	je	0x18009f908 <SetPDFrameWarpNativeCameraSource+0x27078>
18009f896: 45 8b 4b 08                 	mov	r9d, dword ptr [r11 + 0x8]
18009f89a: 45 85 c9                    	test	r9d, r9d
18009f89d: 74 5d                       	je	0x18009f8fc <SetPDFrameWarpNativeCameraSource+0x2706c>
18009f89f: 41 8b 4b 04                 	mov	ecx, dword ptr [r11 + 0x4]
18009f8a3: 41 8b 03                    	mov	eax, dword ptr [r11]
18009f8a6: 89 4c 24 54                 	mov	dword ptr [rsp + 0x54], ecx
18009f8aa: 41 03 4b 0c                 	add	ecx, dword ptr [r11 + 0xc]
18009f8ae: 89 44 24 50                 	mov	dword ptr [rsp + 0x50], eax
18009f8b2: 41 03 c1                    	add	eax, r9d
18009f8b5: 45 33 c9                    	xor	r9d, r9d
18009f8b8: 89 4c 24 60                 	mov	dword ptr [rsp + 0x60], ecx
18009f8bc: 48 8d 4c 24 50              	lea	rcx, [rsp + 0x50]
18009f8c1: 89 44 24 5c                 	mov	dword ptr [rsp + 0x5c], eax
18009f8c5: 49 8b 02                    	mov	rax, qword ptr [r10]
18009f8c8: 48 89 4c 24 40              	mov	qword ptr [rsp + 0x40], rcx
18009f8cd: 49 8b ca                    	mov	rcx, r10
18009f8d0: 44 89 4c 24 38              	mov	dword ptr [rsp + 0x38], r9d
18009f8d5: 4c 89 44 24 30              	mov	qword ptr [rsp + 0x30], r8
18009f8da: 45 33 c0                    	xor	r8d, r8d
18009f8dd: 44 89 4c 24 28              	mov	dword ptr [rsp + 0x28], r9d
18009f8e2: 44 89 4c 24 20              	mov	dword ptr [rsp + 0x20], r9d
18009f8e7: 44 89 4c 24 58              	mov	dword ptr [rsp + 0x58], r9d
18009f8ec: c7 44 24 64 01 00 00 00     	mov	dword ptr [rsp + 0x64], 0x1
18009f8f4: ff 90 70 01 00 00           	call	qword ptr [rax + 0x170]
18009f8fa: eb 09                       	jmp	0x18009f905 <SetPDFrameWarpNativeCameraSource+0x27075>
18009f8fc: 48 8b 01                    	mov	rax, qword ptr [rcx]