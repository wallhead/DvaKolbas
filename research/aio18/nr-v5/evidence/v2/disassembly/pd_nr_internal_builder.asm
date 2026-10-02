
/mnt/data/aio18_nr_work/PDPerfPlugin.dll:	file format coff-x86-64

Disassembly of section .text:

0000000180078890 <SetPDFrameWarpNativeCameraSource>:
18009fdb0: 48 83 ec 38                 	sub	rsp, 0x38
18009fdb4: 41 80 b8 03 01 00 00 00     	cmp	byte ptr [r8 + 0x103], 0x0
18009fdbc: 4d 8b d8                    	mov	r11, r8
18009fdbf: 4c 8b d2                    	mov	r10, rdx
18009fdc2: 74 0c                       	je	0x18009fdd0 <SetPDFrameWarpNativeCameraSource+0x27540>
18009fdc4: 49 83 78 30 00              	cmp	qword ptr [r8 + 0x30], 0x0
18009fdc9: b8 a0 00 00 00              	mov	eax, 0xa0
18009fdce: 75 05                       	jne	0x18009fdd5 <SetPDFrameWarpNativeCameraSource+0x27545>
18009fdd0: b8 50 00 00 00              	mov	eax, 0x50
18009fdd5: 4c 03 c0                    	add	r8, rax
18009fdd8: 48 8d 54 24 20              	lea	rdx, [rsp + 0x20]
18009fddd: e8 be cc ff ff              	call	0x18009caa0 <SetPDFrameWarpNativeCameraSource+0x24210>
18009fde2: 4d 8d 43 60                 	lea	r8, [r11 + 0x60]
18009fde6: 48 8d 54 24 20              	lea	rdx, [rsp + 0x20]
18009fdeb: 0f 10 00                    	movups	xmm0, xmmword ptr [rax]
18009fdee: 41 0f 11 42 48              	movups	xmmword ptr [r10 + 0x48], xmm0
18009fdf3: e8 a8 cc ff ff              	call	0x18009caa0 <SetPDFrameWarpNativeCameraSource+0x24210>
18009fdf8: 4d 8d 43 70                 	lea	r8, [r11 + 0x70]
18009fdfc: 48 8d 54 24 20              	lea	rdx, [rsp + 0x20]
18009fe01: 0f 10 00                    	movups	xmm0, xmmword ptr [rax]
18009fe04: 41 0f 11 42 58              	movups	xmmword ptr [r10 + 0x58], xmm0
18009fe09: e8 92 cc ff ff              	call	0x18009caa0 <SetPDFrameWarpNativeCameraSource+0x24210>
18009fe0e: 4d 8d 83 80 00 00 00        	lea	r8, [r11 + 0x80]
18009fe15: 48 8d 54 24 20              	lea	rdx, [rsp + 0x20]
18009fe1a: 0f 10 00                    	movups	xmm0, xmmword ptr [rax]
18009fe1d: 41 0f 11 42 68              	movups	xmmword ptr [r10 + 0x68], xmm0
18009fe22: e8 79 cc ff ff              	call	0x18009caa0 <SetPDFrameWarpNativeCameraSource+0x24210>
18009fe27: 33 c9                       	xor	ecx, ecx
18009fe29: 0f 10 00                    	movups	xmm0, xmmword ptr [rax]
18009fe2c: 41 0f 11 42 78              	movups	xmmword ptr [r10 + 0x78], xmm0
18009fe31: 45 85 c9                    	test	r9d, r9d
18009fe34: 7e 26                       	jle	0x18009fe5c <SetPDFrameWarpNativeCameraSource+0x275cc>
18009fe36: 8b 44 24 60                 	mov	eax, dword ptr [rsp + 0x60]
18009fe3a: 85 c0                       	test	eax, eax
18009fe3c: 7e 1e                       	jle	0x18009fe5c <SetPDFrameWarpNativeCameraSource+0x275cc>
18009fe3e: 49 89 4a 48                 	mov	qword ptr [r10 + 0x48], rcx
18009fe42: 45 89 4a 50                 	mov	dword ptr [r10 + 0x50], r9d
18009fe46: 41 89 42 54                 	mov	dword ptr [r10 + 0x54], eax
18009fe4a: 49 89 4a 78                 	mov	qword ptr [r10 + 0x78], rcx
18009fe4e: 45 89 8a 80 00 00 00        	mov	dword ptr [r10 + 0x80], r9d
18009fe55: 41 89 82 84 00 00 00        	mov	dword ptr [r10 + 0x84], eax
18009fe5c: 4d 8d 83 90 00 00 00        	lea	r8, [r11 + 0x90]
18009fe63: 48 8d 54 24 20              	lea	rdx, [rsp + 0x20]
18009fe68: e8 33 cc ff ff              	call	0x18009caa0 <SetPDFrameWarpNativeCameraSource+0x24210>
18009fe6d: 4d 8d 83 b0 00 00 00        	lea	r8, [r11 + 0xb0]
18009fe74: 48 8d 54 24 20              	lea	rdx, [rsp + 0x20]
18009fe79: 0f 10 00                    	movups	xmm0, xmmword ptr [rax]
18009fe7c: 41 0f 11 82 88 00 00 00     	movups	xmmword ptr [r10 + 0x88], xmm0
18009fe84: e8 17 cc ff ff              	call	0x18009caa0 <SetPDFrameWarpNativeCameraSource+0x24210>
18009fe89: 48 8d 54 24 20              	lea	rdx, [rsp + 0x20]
18009fe8e: 0f 10 00                    	movups	xmm0, xmmword ptr [rax]
18009fe91: 41 0f 11 82 98 00 00 00     	movups	xmmword ptr [r10 + 0x98], xmm0
18009fe99: e8 02 cc ff ff              	call	0x18009caa0 <SetPDFrameWarpNativeCameraSource+0x24210>
18009fe9e: 4d 8d 83 c0 00 00 00        	lea	r8, [r11 + 0xc0]
18009fea5: 48 8d 54 24 20              	lea	rdx, [rsp + 0x20]
18009feaa: 0f 10 00                    	movups	xmm0, xmmword ptr [rax]
18009fead: 41 0f 11 82 a8 00 00 00     	movups	xmmword ptr [r10 + 0xa8], xmm0
18009feb5: e8 e6 cb ff ff              	call	0x18009caa0 <SetPDFrameWarpNativeCameraSource+0x24210>
18009feba: 4d 8d 83 d0 00 00 00        	lea	r8, [r11 + 0xd0]
18009fec1: 48 8d 54 24 20              	lea	rdx, [rsp + 0x20]
18009fec6: 0f 10 00                    	movups	xmm0, xmmword ptr [rax]
18009fec9: 41 0f 11 82 b8 00 00 00     	movups	xmmword ptr [r10 + 0xb8], xmm0
18009fed1: e8 ca cb ff ff              	call	0x18009caa0 <SetPDFrameWarpNativeCameraSource+0x24210>
18009fed6: f3 0f 10 54 24 68           	movss	xmm2, dword ptr [rsp + 0x68]
18009fedc: 0f 28 ca                    	movaps	xmm1, xmm2
18009fedf: 0f 10 00                    	movups	xmm0, xmmword ptr [rax]
18009fee2: 41 0f 11 82 c8 00 00 00     	movups	xmmword ptr [r10 + 0xc8], xmm0
18009feea: f3 41 0f 59 8b e0 00 00 00  	mulss	xmm1, dword ptr [r11 + 0xe0]
18009fef3: f3 41 0f 11 8a d8 00 00 00  	movss	dword ptr [r10 + 0xd8], xmm1
18009fefc: f3 41 0f 59 93 e4 00 00 00  	mulss	xmm2, dword ptr [r11 + 0xe4]
18009ff05: f3 41 0f 11 92 dc 00 00 00  	movss	dword ptr [r10 + 0xdc], xmm2
18009ff0e: 41 8b 83 e8 00 00 00        	mov	eax, dword ptr [r11 + 0xe8]
18009ff15: 41 89 82 e0 00 00 00        	mov	dword ptr [r10 + 0xe0], eax
18009ff1c: 41 8b 83 ec 00 00 00        	mov	eax, dword ptr [r11 + 0xec]
18009ff23: 41 89 82 e4 00 00 00        	mov	dword ptr [r10 + 0xe4], eax
18009ff2a: 41 8b 83 f0 00 00 00        	mov	eax, dword ptr [r11 + 0xf0]
18009ff31: 41 89 82 e8 00 00 00        	mov	dword ptr [r10 + 0xe8], eax
18009ff38: 41 8b 83 f4 00 00 00        	mov	eax, dword ptr [r11 + 0xf4]
18009ff3f: 41 89 82 ec 00 00 00        	mov	dword ptr [r10 + 0xec], eax
18009ff46: 8b c1                       	mov	eax, ecx
18009ff48: 41 38 8b f8 00 00 00        	cmp	byte ptr [r11 + 0xf8], cl
18009ff4f: 0f 95 c0                    	setne	al
18009ff52: 41 89 82 f0 00 00 00        	mov	dword ptr [r10 + 0xf0], eax
18009ff59: 41 8b 83 fc 00 00 00        	mov	eax, dword ptr [r11 + 0xfc]
18009ff60: 41 89 82 f4 00 00 00        	mov	dword ptr [r10 + 0xf4], eax
18009ff67: 8b c1                       	mov	eax, ecx
18009ff69: 41 38 8b 00 01 00 00        	cmp	byte ptr [r11 + 0x100], cl
18009ff70: 0f 95 c0                    	setne	al
18009ff73: 41 89 82 f8 00 00 00        	mov	dword ptr [r10 + 0xf8], eax
18009ff7a: 8b c1                       	mov	eax, ecx
18009ff7c: 41 38 8b 01 01 00 00        	cmp	byte ptr [r11 + 0x101], cl
18009ff83: 0f 95 c0                    	setne	al
18009ff86: 41 89 82 fc 00 00 00        	mov	dword ptr [r10 + 0xfc], eax
18009ff8d: 8b c1                       	mov	eax, ecx
18009ff8f: 41 38 8b 02 01 00 00        	cmp	byte ptr [r11 + 0x102], cl
18009ff96: 41 89 8a 04 01 00 00        	mov	dword ptr [r10 + 0x104], ecx
18009ff9d: 0f 95 c0                    	setne	al
18009ffa0: 41 89 82 00 01 00 00        	mov	dword ptr [r10 + 0x100], eax
18009ffa7: 48 83 c4 38                 	add	rsp, 0x38
18009ffab: c3                          	ret
18009ffac: cc                          	int3
18009ffad: cc                          	int3
18009ffae: cc                          	int3
18009ffaf: cc                          	int3
18009ffb0: 48 89 5c 24 18              	mov	qword ptr [rsp + 0x18], rbx
18009ffb5: 55                          	push	rbp
18009ffb6: 56                          	push	rsi
18009ffb7: 57                          	push	rdi
18009ffb8: 41 54                       	push	r12
18009ffba: 41 55                       	push	r13
18009ffbc: 41 56                       	push	r14
18009ffbe: 41 57                       	push	r15
18009ffc0: 48 8d 6c 24 d9              	lea	rbp, [rsp - 0x27]
18009ffc5: 48 81 ec e0 00 00 00        	sub	rsp, 0xe0
18009ffcc: 48 8b 05 ed a9 12 01        	mov	rax, qword ptr [rip + 0x112a9ed] # 0x1811ca9c0
18009ffd3: 48 33 c4                    	xor	rax, rsp
18009ffd6: 48 89 45 1f                 	mov	qword ptr [rbp + 0x1f], rax
18009ffda: 48 8b f2                    	mov	rsi, rdx
18009ffdd: 48 8b d9                    	mov	rbx, rcx
18009ffe0: 48 89 4d e7                 	mov	qword ptr [rbp - 0x19], rcx
18009ffe4: e8 07 bf ff ff              	call	0x18009bef0 <SetPDFrameWarpNativeCameraSource+0x23660>
18009ffe9: 84 c0                       	test	al, al
18009ffeb: 0f 84 e6 08 00 00           	je	0x1800a08d7 <SetPDFrameWarpNativeCameraSource+0x28047>
18009fff1: 48 8b 05 f8 58 17 01        	mov	rax, qword ptr [rip + 0x11758f8] # 0x1812158f0
18009fff8: 48 83 78 10 00              	cmp	qword ptr [rax + 0x10], 0x0
18009fffd: 0f 84 d4 08 00 00           	je	0x1800a08d7 <SetPDFrameWarpNativeCameraSource+0x28047>
1800a0003: 48 83 b8 a8 04 00 00 00     	cmp	qword ptr [rax + 0x4a8], 0x0
1800a000b: 0f 84 ae 08 00 00           	je	0x1800a08bf <SetPDFrameWarpNativeCameraSource+0x2802f>
1800a0011: 48 83 b8 38 08 00 00 00     	cmp	qword ptr [rax + 0x838], 0x0
1800a0019: 0f 84 a0 08 00 00           	je	0x1800a08bf <SetPDFrameWarpNativeCameraSource+0x2802f>
1800a001f: e8 fc b5 ff ff              	call	0x18009b620 <SetPDFrameWarpNativeCameraSource+0x22d90>
1800a0024: 4c 8b f0                    	mov	r14, rax
1800a0027: 48 8b cb                    	mov	rcx, rbx
1800a002a: e8 91 be ff ff              	call	0x18009bec0 <SetPDFrameWarpNativeCameraSource+0x23630>
1800a002f: 84 c0                       	test	al, al
1800a0031: 0f 84 ba 04 00 00           	je	0x1800a04f1 <SetPDFrameWarpNativeCameraSource+0x27c61>
1800a0037: 48 8b 1d b2 58 17 01        	mov	rbx, qword ptr [rip + 0x11758b2] # 0x1812158f0
1800a003e: 48 81 c3 78 0a 00 00        	add	rbx, 0xa78
1800a0045: 48 89 5d f7                 	mov	qword ptr [rbp - 0x9], rbx
1800a0049: 48 8b cb                    	mov	rcx, rbx
1800a004c: ff 15 e6 34 07 00           	call	qword ptr [rip + 0x734e6] # 0x180113538
1800a0052: 85 c0                       	test	eax, eax
1800a0054: 0f 85 59 08 00 00           	jne	0x1800a08b3 <SetPDFrameWarpNativeCameraSource+0x28023>
1800a005a: 81 7b 4c ff ff ff 7f        	cmp	dword ptr [rbx + 0x4c], 0x7fffffff
1800a0061: 0f 84 94 05 00 00           	je	0x1800a05fb <SetPDFrameWarpNativeCameraSource+0x27d6b>
1800a0067: 4c 8b 05 82 58 17 01        	mov	r8, qword ptr [rip + 0x1175882] # 0x1812158f0
1800a006e: 49 8b 90 00 0c 00 00        	mov	rdx, qword ptr [r8 + 0xc00]
1800a0075: 48 85 d2                    	test	rdx, rdx
1800a0078: 74 54                       	je	0x1800a00ce <SetPDFrameWarpNativeCameraSource+0x2783e>
1800a007a: 49 83 b8 f0 0b 00 00 00     	cmp	qword ptr [r8 + 0xbf0], 0x0
1800a0082: 74 4a                       	je	0x1800a00ce <SetPDFrameWarpNativeCameraSource+0x2783e>
1800a0084: 49 8b 06                    	mov	rax, qword ptr [r14]
1800a0087: 49 ff 80 10 0c 00 00        	inc	qword ptr [r8 + 0xc10]
1800a008e: 4d 8b 80 10 0c 00 00        	mov	r8, qword ptr [r8 + 0xc10]
1800a0095: 49 8b ce                    	mov	rcx, r14
1800a0098: ff 90 98 04 00 00           	call	qword ptr [rax + 0x498]
1800a009e: 49 8b 06                    	mov	rax, qword ptr [r14]
1800a00a1: 49 8b ce                    	mov	rcx, r14
1800a00a4: ff 90 78 03 00 00           	call	qword ptr [rax + 0x378]
1800a00aa: 48 8b 15 3f 58 17 01        	mov	rdx, qword ptr [rip + 0x117583f] # 0x1812158f0
1800a00b1: 48 8b 8a 20 0b 00 00        	mov	rcx, qword ptr [rdx + 0xb20]
1800a00b8: 48 8b 01                    	mov	rax, qword ptr [rcx]
1800a00bb: 4c 8b 82 10 0c 00 00        	mov	r8, qword ptr [rdx + 0xc10]
1800a00c2: 48 8b 92 f0 0b 00 00        	mov	rdx, qword ptr [rdx + 0xbf0]
1800a00c9: ff 50 78                    	call	qword ptr [rax + 0x78]
1800a00cc: eb 0c                       	jmp	0x1800a00da <SetPDFrameWarpNativeCameraSource+0x2784a>
1800a00ce: 49 8b 06                    	mov	rax, qword ptr [r14]
1800a00d1: 49 8b ce                    	mov	rcx, r14
1800a00d4: ff 90 78 03 00 00           	call	qword ptr [rax + 0x378]
1800a00da: e8 71 45 fd ff              	call	0x180074650 <SetPDFrameWarpDiagnosticHud+0x4bc00>
1800a00df: 4c 8b e0                    	mov	r12, rax
1800a00e2: 48 89 45 ef                 	mov	qword ptr [rbp - 0x11], rax
1800a00e6: 48 85 c0                    	test	rax, rax
1800a00e9: 75 11                       	jne	0x1800a00fc <SetPDFrameWarpNativeCameraSource+0x2786c>
1800a00eb: 48 8d 0d 46 47 10 01        	lea	rcx, [rip + 0x1104746]  # 0x1811a4838
1800a00f2: e8 49 ba 05 00              	call	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
1800a00f7: e9 da 03 00 00              	jmp	0x1800a04d6 <SetPDFrameWarpNativeCameraSource+0x27c46>
1800a00fc: 33 ff                       	xor	edi, edi
1800a00fe: 48 39 7e 08                 	cmp	qword ptr [rsi + 0x8], rdi
1800a0102: 74 33                       	je	0x1800a0137 <SetPDFrameWarpNativeCameraSource+0x278a7>
1800a0104: 48 8b 05 e5 57 17 01        	mov	rax, qword ptr [rip + 0x11757e5] # 0x1812158f0
1800a010b: 4c 8b b8 a0 04 00 00        	mov	r15, qword ptr [rax + 0x4a0]
1800a0112: 4d 85 ff                    	test	r15, r15
1800a0115: 74 20                       	je	0x1800a0137 <SetPDFrameWarpNativeCameraSource+0x278a7>
1800a0117: 49 8b d7                    	mov	rdx, r15
1800a011a: e8 41 3f fd ff              	call	0x180074060 <SetPDFrameWarpDiagnosticHud+0x4b610>
1800a011f: 48 89 45 b7                 	mov	qword ptr [rbp - 0x49], rax
1800a0123: 48 85 c0                    	test	rax, rax
1800a0126: 74 0f                       	je	0x1800a0137 <SetPDFrameWarpNativeCameraSource+0x278a7>
1800a0128: 4d 8b c7                    	mov	r8, r15
1800a012b: 49 8b d4                    	mov	rdx, r12
1800a012e: e8 0d 40 fd ff              	call	0x180074140 <SetPDFrameWarpDiagnosticHud+0x4b6f0>
1800a0133: 84 c0                       	test	al, al
1800a0135: 75 04                       	jne	0x1800a013b <SetPDFrameWarpNativeCameraSource+0x278ab>
1800a0137: 48 89 7d b7                 	mov	qword ptr [rbp - 0x49], rdi
1800a013b: 48 8b 05 ae 57 17 01        	mov	rax, qword ptr [rip + 0x11757ae] # 0x1812158f0
1800a0142: 4c 8b b8 a8 04 00 00        	mov	r15, qword ptr [rax + 0x4a8]
1800a0149: 4d 85 ff                    	test	r15, r15
1800a014c: 74 20                       	je	0x1800a016e <SetPDFrameWarpNativeCameraSource+0x278de>
1800a014e: 49 8b d7                    	mov	rdx, r15
1800a0151: e8 0a 3f fd ff              	call	0x180074060 <SetPDFrameWarpDiagnosticHud+0x4b610>
1800a0156: 48 89 45 07                 	mov	qword ptr [rbp + 0x7], rax
1800a015a: 48 85 c0                    	test	rax, rax
1800a015d: 74 0f                       	je	0x1800a016e <SetPDFrameWarpNativeCameraSource+0x278de>
1800a015f: 4d 8b c7                    	mov	r8, r15
1800a0162: 49 8b d4                    	mov	rdx, r12
1800a0165: e8 d6 3f fd ff              	call	0x180074140 <SetPDFrameWarpDiagnosticHud+0x4b6f0>
1800a016a: 84 c0                       	test	al, al
1800a016c: 75 04                       	jne	0x1800a0172 <SetPDFrameWarpNativeCameraSource+0x278e2>
1800a016e: 48 89 7d 07                 	mov	qword ptr [rbp + 0x7], rdi
1800a0172: 48 83 7e 18 00              	cmp	qword ptr [rsi + 0x18], 0x0
1800a0177: 74 33                       	je	0x1800a01ac <SetPDFrameWarpNativeCameraSource+0x2791c>
1800a0179: 48 8b 05 70 57 17 01        	mov	rax, qword ptr [rip + 0x1175770] # 0x1812158f0
1800a0180: 4c 8b b8 90 03 00 00        	mov	r15, qword ptr [rax + 0x390]
1800a0187: 4d 85 ff                    	test	r15, r15
1800a018a: 74 20                       	je	0x1800a01ac <SetPDFrameWarpNativeCameraSource+0x2791c>
1800a018c: 49 8b d7                    	mov	rdx, r15
1800a018f: e8 cc 3e fd ff              	call	0x180074060 <SetPDFrameWarpDiagnosticHud+0x4b610>
1800a0194: 48 89 45 bf                 	mov	qword ptr [rbp - 0x41], rax
1800a0198: 48 85 c0                    	test	rax, rax
1800a019b: 74 0f                       	je	0x1800a01ac <SetPDFrameWarpNativeCameraSource+0x2791c>
1800a019d: 4d 8b c7                    	mov	r8, r15
1800a01a0: 49 8b d4                    	mov	rdx, r12
1800a01a3: e8 98 3f fd ff              	call	0x180074140 <SetPDFrameWarpDiagnosticHud+0x4b6f0>
1800a01a8: 84 c0                       	test	al, al
1800a01aa: 75 04                       	jne	0x1800a01b0 <SetPDFrameWarpNativeCameraSource+0x27920>
1800a01ac: 48 89 7d bf                 	mov	qword ptr [rbp - 0x41], rdi
1800a01b0: 48 83 7e 10 00              	cmp	qword ptr [rsi + 0x10], 0x0
1800a01b5: 74 33                       	je	0x1800a01ea <SetPDFrameWarpNativeCameraSource+0x2795a>
1800a01b7: 48 8b 05 32 57 17 01        	mov	rax, qword ptr [rip + 0x1175732] # 0x1812158f0
1800a01be: 4c 8b b8 98 03 00 00        	mov	r15, qword ptr [rax + 0x398]
1800a01c5: 4d 85 ff                    	test	r15, r15
1800a01c8: 74 20                       	je	0x1800a01ea <SetPDFrameWarpNativeCameraSource+0x2795a>
1800a01ca: 49 8b d7                    	mov	rdx, r15
1800a01cd: e8 8e 3e fd ff              	call	0x180074060 <SetPDFrameWarpDiagnosticHud+0x4b610>
1800a01d2: 48 89 45 c7                 	mov	qword ptr [rbp - 0x39], rax
1800a01d6: 48 85 c0                    	test	rax, rax
1800a01d9: 74 0f                       	je	0x1800a01ea <SetPDFrameWarpNativeCameraSource+0x2795a>
1800a01db: 4d 8b c7                    	mov	r8, r15
1800a01de: 49 8b d4                    	mov	rdx, r12
1800a01e1: e8 5a 3f fd ff              	call	0x180074140 <SetPDFrameWarpDiagnosticHud+0x4b6f0>
1800a01e6: 84 c0                       	test	al, al
1800a01e8: 75 04                       	jne	0x1800a01ee <SetPDFrameWarpNativeCameraSource+0x2795e>
1800a01ea: 48 89 7d c7                 	mov	qword ptr [rbp - 0x39], rdi
1800a01ee: 48 83 7e 28 00              	cmp	qword ptr [rsi + 0x28], 0x0
1800a01f3: 74 33                       	je	0x1800a0228 <SetPDFrameWarpNativeCameraSource+0x27998>
1800a01f5: 48 8b 05 f4 56 17 01        	mov	rax, qword ptr [rip + 0x11756f4] # 0x1812158f0
1800a01fc: 4c 8b b8 78 04 00 00        	mov	r15, qword ptr [rax + 0x478]
1800a0203: 4d 85 ff                    	test	r15, r15
1800a0206: 74 20                       	je	0x1800a0228 <SetPDFrameWarpNativeCameraSource+0x27998>
1800a0208: 49 8b d7                    	mov	rdx, r15
1800a020b: e8 50 3e fd ff              	call	0x180074060 <SetPDFrameWarpDiagnosticHud+0x4b610>
1800a0210: 48 89 45 df                 	mov	qword ptr [rbp - 0x21], rax
1800a0214: 48 85 c0                    	test	rax, rax
1800a0217: 74 0f                       	je	0x1800a0228 <SetPDFrameWarpNativeCameraSource+0x27998>
1800a0219: 4d 8b c7                    	mov	r8, r15
1800a021c: 49 8b d4                    	mov	rdx, r12
1800a021f: e8 1c 3f fd ff              	call	0x180074140 <SetPDFrameWarpDiagnosticHud+0x4b6f0>
1800a0224: 84 c0                       	test	al, al
1800a0226: 75 04                       	jne	0x1800a022c <SetPDFrameWarpNativeCameraSource+0x2799c>
1800a0228: 48 89 7d df                 	mov	qword ptr [rbp - 0x21], rdi
1800a022c: 48 83 7e 30 00              	cmp	qword ptr [rsi + 0x30], 0x0
1800a0231: 74 33                       	je	0x1800a0266 <SetPDFrameWarpNativeCameraSource+0x279d6>
1800a0233: 48 8b 05 b6 56 17 01        	mov	rax, qword ptr [rip + 0x11756b6] # 0x1812158f0
1800a023a: 4c 8b b8 88 04 00 00        	mov	r15, qword ptr [rax + 0x488]
1800a0241: 4d 85 ff                    	test	r15, r15
1800a0244: 74 20                       	je	0x1800a0266 <SetPDFrameWarpNativeCameraSource+0x279d6>
1800a0246: 49 8b d7                    	mov	rdx, r15
1800a0249: e8 12 3e fd ff              	call	0x180074060 <SetPDFrameWarpDiagnosticHud+0x4b610>
1800a024e: 48 89 45 d7                 	mov	qword ptr [rbp - 0x29], rax
1800a0252: 48 85 c0                    	test	rax, rax
1800a0255: 74 0f                       	je	0x1800a0266 <SetPDFrameWarpNativeCameraSource+0x279d6>
1800a0257: 4d 8b c7                    	mov	r8, r15
1800a025a: 49 8b d4                    	mov	rdx, r12
1800a025d: e8 de 3e fd ff              	call	0x180074140 <SetPDFrameWarpDiagnosticHud+0x4b6f0>
1800a0262: 84 c0                       	test	al, al
1800a0264: 75 04                       	jne	0x1800a026a <SetPDFrameWarpNativeCameraSource+0x279da>
1800a0266: 48 89 7d d7                 	mov	qword ptr [rbp - 0x29], rdi
1800a026a: 48 83 7e 38 00              	cmp	qword ptr [rsi + 0x38], 0x0
1800a026f: 74 33                       	je	0x1800a02a4 <SetPDFrameWarpNativeCameraSource+0x27a14>
1800a0271: 48 8b 05 78 56 17 01        	mov	rax, qword ptr [rip + 0x1175678] # 0x1812158f0
1800a0278: 4c 8b b8 80 04 00 00        	mov	r15, qword ptr [rax + 0x480]
1800a027f: 4d 85 ff                    	test	r15, r15
1800a0282: 74 20                       	je	0x1800a02a4 <SetPDFrameWarpNativeCameraSource+0x27a14>
1800a0284: 49 8b d7                    	mov	rdx, r15
1800a0287: e8 d4 3d fd ff              	call	0x180074060 <SetPDFrameWarpDiagnosticHud+0x4b610>
1800a028c: 48 89 45 cf                 	mov	qword ptr [rbp - 0x31], rax
1800a0290: 48 85 c0                    	test	rax, rax
1800a0293: 74 0f                       	je	0x1800a02a4 <SetPDFrameWarpNativeCameraSource+0x27a14>
1800a0295: 4d 8b c7                    	mov	r8, r15
1800a0298: 49 8b d4                    	mov	rdx, r12
1800a029b: e8 a0 3e fd ff              	call	0x180074140 <SetPDFrameWarpDiagnosticHud+0x4b6f0>
1800a02a0: 84 c0                       	test	al, al
1800a02a2: 75 04                       	jne	0x1800a02a8 <SetPDFrameWarpNativeCameraSource+0x27a18>
1800a02a4: 48 89 7d cf                 	mov	qword ptr [rbp - 0x31], rdi
1800a02a8: 48 83 7e 48 00              	cmp	qword ptr [rsi + 0x48], 0x0
1800a02ad: 74 32                       	je	0x1800a02e1 <SetPDFrameWarpNativeCameraSource+0x27a51>
1800a02af: 48 8b 05 3a 56 17 01        	mov	rax, qword ptr [rip + 0x117563a] # 0x1812158f0
