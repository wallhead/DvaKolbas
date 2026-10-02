
/mnt/data/aio18_nr_work/PDPerfPlugin.dll:	file format coff-x86-64

Disassembly of section .text:

0000000180078890 <SetPDFrameWarpNativeCameraSource>:
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
1800a00eb: 48 8d 0d 46 47 10 01        	lea	rcx, [rip + 0x1104746]  # 0x1811a4838    ; STRING: DLSSNR: BeginLocalCommandList failed for immediate evaluate.
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
1800a02b6: 4c 8b b8 b0 04 00 00        	mov	r15, qword ptr [rax + 0x4b0]
1800a02bd: 4d 85 ff                    	test	r15, r15
1800a02c0: 74 1f                       	je	0x1800a02e1 <SetPDFrameWarpNativeCameraSource+0x27a51>
1800a02c2: 49 8b d7                    	mov	rdx, r15
1800a02c5: e8 96 3d fd ff              	call	0x180074060 <SetPDFrameWarpDiagnosticHud+0x4b610>
1800a02ca: 4c 8b e8                    	mov	r13, rax
1800a02cd: 48 85 c0                    	test	rax, rax
1800a02d0: 74 0f                       	je	0x1800a02e1 <SetPDFrameWarpNativeCameraSource+0x27a51>
1800a02d2: 4d 8b c7                    	mov	r8, r15
1800a02d5: 49 8b d4                    	mov	rdx, r12
1800a02d8: e8 63 3e fd ff              	call	0x180074140 <SetPDFrameWarpDiagnosticHud+0x4b6f0>
1800a02dd: 84 c0                       	test	al, al
1800a02df: 75 03                       	jne	0x1800a02e4 <SetPDFrameWarpNativeCameraSource+0x27a54>
1800a02e1: 4c 8b ef                    	mov	r13, rdi
1800a02e4: 48 8b 45 07                 	mov	rax, qword ptr [rbp + 0x7]
1800a02e8: 48 85 c0                    	test	rax, rax
1800a02eb: 74 11                       	je	0x1800a02fe <SetPDFrameWarpNativeCameraSource+0x27a6e>
1800a02ed: 48 8b d0                    	mov	rdx, rax
1800a02f0: 48 8b 4d e7                 	mov	rcx, qword ptr [rbp - 0x19]
1800a02f4: e8 27 c9 ff ff              	call	0x18009cc20 <SetPDFrameWarpNativeCameraSource+0x24390>
1800a02f9: 4c 8b f8                    	mov	r15, rax
1800a02fc: eb 03                       	jmp	0x1800a0301 <SetPDFrameWarpNativeCameraSource+0x27a71>
1800a02fe: 4c 8b ff                    	mov	r15, rdi
1800a0301: 4d 8b e7                    	mov	r12, r15
1800a0304: 48 8b 46 40                 	mov	rax, qword ptr [rsi + 0x40]
1800a0308: 48 8b 4d b7                 	mov	rcx, qword ptr [rbp - 0x49]
1800a030c: 48 85 c0                    	test	rax, rax
1800a030f: 74 0d                       	je	0x1800a031e <SetPDFrameWarpNativeCameraSource+0x27a8e>
1800a0311: 48 3b 46 08                 	cmp	rax, qword ptr [rsi + 0x8]
1800a0315: 75 07                       	jne	0x1800a031e <SetPDFrameWarpNativeCameraSource+0x27a8e>
1800a0317: 48 85 c9                    	test	rcx, rcx
1800a031a: 4c 0f 45 e1                 	cmovne	r12, rcx
1800a031e: 4d 85 ff                    	test	r15, r15
1800a0321: 0f 84 8b 01 00 00           	je	0x1800a04b2 <SetPDFrameWarpNativeCameraSource+0x27c22>
1800a0327: 48 83 7e 08 00              	cmp	qword ptr [rsi + 0x8], 0x0
1800a032c: 74 09                       	je	0x1800a0337 <SetPDFrameWarpNativeCameraSource+0x27aa7>
1800a032e: 48 85 c9                    	test	rcx, rcx
1800a0331: 0f 84 7b 01 00 00           	je	0x1800a04b2 <SetPDFrameWarpNativeCameraSource+0x27c22>
1800a0337: 48 83 7e 18 00              	cmp	qword ptr [rsi + 0x18], 0x0
1800a033c: 74 0b                       	je	0x1800a0349 <SetPDFrameWarpNativeCameraSource+0x27ab9>
1800a033e: 48 83 7d bf 00              	cmp	qword ptr [rbp - 0x41], 0x0
1800a0343: 0f 84 69 01 00 00           	je	0x1800a04b2 <SetPDFrameWarpNativeCameraSource+0x27c22>
1800a0349: 48 83 7e 10 00              	cmp	qword ptr [rsi + 0x10], 0x0
1800a034e: 74 0d                       	je	0x1800a035d <SetPDFrameWarpNativeCameraSource+0x27acd>
1800a0350: 48 8b 45 c7                 	mov	rax, qword ptr [rbp - 0x39]
1800a0354: 48 85 c0                    	test	rax, rax
1800a0357: 0f 84 59 01 00 00           	je	0x1800a04b6 <SetPDFrameWarpNativeCameraSource+0x27c26>
1800a035d: 48 8b 4d e7                 	mov	rcx, qword ptr [rbp - 0x19]
1800a0361: 48 83 c1 38                 	add	rcx, 0x38
1800a0365: 48 8b d6                    	mov	rdx, rsi
1800a0368: e8 03 1b ff ff              	call	0x180091e70 <SetPDFrameWarpNativeCameraSource+0x195e0>
1800a036d: 4c 89 64 24 60              	mov	qword ptr [rsp + 0x60], r12
1800a0372: 4c 89 6c 24 58              	mov	qword ptr [rsp + 0x58], r13
1800a0377: 48 8b 55 cf                 	mov	rdx, qword ptr [rbp - 0x31]
1800a037b: 48 89 54 24 50              	mov	qword ptr [rsp + 0x50], rdx
1800a0380: 48 8b 55 d7                 	mov	rdx, qword ptr [rbp - 0x29]
1800a0384: 48 89 54 24 48              	mov	qword ptr [rsp + 0x48], rdx
1800a0389: 48 8b 55 df                 	mov	rdx, qword ptr [rbp - 0x21]
1800a038d: 48 89 54 24 40              	mov	qword ptr [rsp + 0x40], rdx
1800a0392: 48 8b 4d c7                 	mov	rcx, qword ptr [rbp - 0x39]
1800a0396: 48 89 4c 24 38              	mov	qword ptr [rsp + 0x38], rcx
1800a039b: 48 8b 55 bf                 	mov	rdx, qword ptr [rbp - 0x41]
1800a039f: 48 89 54 24 30              	mov	qword ptr [rsp + 0x30], rdx
1800a03a4: 4c 89 7c 24 28              	mov	qword ptr [rsp + 0x28], r15
1800a03a9: 48 8b 4d b7                 	mov	rcx, qword ptr [rbp - 0x49]
1800a03ad: 48 89 4c 24 20              	mov	qword ptr [rsp + 0x20], rcx
1800a03b2: 4c 8b ce                    	mov	r9, rsi
1800a03b5: 4c 8b 00                    	mov	r8, qword ptr [rax]
1800a03b8: 4c 8b 65 ef                 	mov	r12, qword ptr [rbp - 0x11]
1800a03bc: 49 8b d4                    	mov	rdx, r12
1800a03bf: 48 8b 4d e7                 	mov	rcx, qword ptr [rbp - 0x19]
1800a03c3: e8 58 ed ff ff              	call	0x18009f120 <SetPDFrameWarpNativeCameraSource+0x26890>
1800a03c8: 84 c0                       	test	al, al
1800a03ca: 75 0c                       	jne	0x1800a03d8 <SetPDFrameWarpNativeCameraSource+0x27b48>
1800a03cc: 48 8d 0d e5 44 10 01        	lea	rcx, [rip + 0x11044e5]  # 0x1811a48b8    ; STRING: NGX_D3D12_EVALUATE_DLSSNR_EXT immediate local D3D11 failed
1800a03d3: e8 18 bf ff ff              	call	0x18009c2f0 <SetPDFrameWarpNativeCameraSource+0x23a60>
1800a03d8: 4d 8b cf                    	mov	r9, r15
1800a03db: 4c 8b 05 0e 55 17 01        	mov	r8, qword ptr [rip + 0x117550e] # 0x1812158f0
1800a03e2: 4d 8b 80 a8 04 00 00        	mov	r8, qword ptr [r8 + 0x4a8]
1800a03e9: 49 8b d4                    	mov	rdx, r12
1800a03ec: e8 0f 3f fd ff              	call	0x180074300 <SetPDFrameWarpDiagnosticHud+0x4b8b0>
1800a03f1: e8 1a 43 fd ff              	call	0x180074710 <SetPDFrameWarpDiagnosticHud+0x4bcc0>
1800a03f6: 4c 8b 05 f3 54 17 01        	mov	r8, qword ptr [rip + 0x11754f3] # 0x1812158f0
1800a03fd: 49 8b 90 00 0c 00 00        	mov	rdx, qword ptr [r8 + 0xc00]
1800a0404: 48 85 d2                    	test	rdx, rdx
1800a0407: 74 1f                       	je	0x1800a0428 <SetPDFrameWarpNativeCameraSource+0x27b98>
1800a0409: 48 85 c0                    	test	rax, rax
1800a040c: 74 1a                       	je	0x1800a0428 <SetPDFrameWarpNativeCameraSource+0x27b98>
1800a040e: 49 8b 0e                    	mov	rcx, qword ptr [r14]
1800a0411: 4c 8b 89 a0 04 00 00        	mov	r9, qword ptr [rcx + 0x4a0]
1800a0418: 4c 8b c0                    	mov	r8, rax
1800a041b: 49 8b ce                    	mov	rcx, r14
1800a041e: 41 ff d1                    	call	r9
1800a0421: 4c 8b 05 c8 54 17 01        	mov	r8, qword ptr [rip + 0x11754c8] # 0x1812158f0
1800a0428: 48 8b 56 20                 	mov	rdx, qword ptr [rsi + 0x20]
1800a042c: 48 85 d2                    	test	rdx, rdx
1800a042f: 0f 84 ae 00 00 00           	je	0x1800a04e3 <SetPDFrameWarpNativeCameraSource+0x27c53>
1800a0435: 8b 86 88 00 00 00           	mov	eax, dword ptr [rsi + 0x88]
1800a043b: 49 8b ce                    	mov	rcx, r14
1800a043e: 85 c0                       	test	eax, eax
1800a0440: 74 5e                       	je	0x1800a04a0 <SetPDFrameWarpNativeCameraSource+0x27c10>
1800a0442: 48 c7 45 07 00 00 00 00     	mov	qword ptr [rbp + 0x7], 0x0
1800a044a: 89 45 13                    	mov	dword ptr [rbp + 0x13], eax
1800a044d: 8b 86 8c 00 00 00           	mov	eax, dword ptr [rsi + 0x8c]
1800a0453: 89 45 17                    	mov	dword ptr [rbp + 0x17], eax
1800a0456: 89 7d 0f                    	mov	dword ptr [rbp + 0xf], edi
1800a0459: c7 45 1b 01 00 00 00        	mov	dword ptr [rbp + 0x1b], 0x1
1800a0460: 49 8b 06                    	mov	rax, qword ptr [r14]
1800a0463: 4c 8b 90 70 01 00 00        	mov	r10, qword ptr [rax + 0x170]
1800a046a: 48 8d 45 07                 	lea	rax, [rbp + 0x7]
1800a046e: 48 89 44 24 40              	mov	qword ptr [rsp + 0x40], rax
1800a0473: 89 7c 24 38                 	mov	dword ptr [rsp + 0x38], edi
1800a0477: 49 8b 80 38 08 00 00        	mov	rax, qword ptr [r8 + 0x838]
1800a047e: 48 89 44 24 30              	mov	qword ptr [rsp + 0x30], rax
1800a0483: 89 7c 24 28                 	mov	dword ptr [rsp + 0x28], edi
1800a0487: 8b 86 84 00 00 00           	mov	eax, dword ptr [rsi + 0x84]
1800a048d: 89 44 24 20                 	mov	dword ptr [rsp + 0x20], eax
1800a0491: 44 8b 8e 80 00 00 00        	mov	r9d, dword ptr [rsi + 0x80]
1800a0498: 45 33 c0                    	xor	r8d, r8d
1800a049b: 41 ff d2                    	call	r10
1800a049e: eb 43                       	jmp	0x1800a04e3 <SetPDFrameWarpNativeCameraSource+0x27c53>
1800a04a0: 49 8b 06                    	mov	rax, qword ptr [r14]
1800a04a3: 4d 8b 80 38 08 00 00        	mov	r8, qword ptr [r8 + 0x838]
1800a04aa: ff 90 78 01 00 00           	call	qword ptr [rax + 0x178]
1800a04b0: eb 31                       	jmp	0x1800a04e3 <SetPDFrameWarpNativeCameraSource+0x27c53>
1800a04b2: 48 8b 45 c7                 	mov	rax, qword ptr [rbp - 0x39]
1800a04b6: 48 89 44 24 20              	mov	qword ptr [rsp + 0x20], rax
1800a04bb: 4c 8b 4d bf                 	mov	r9, qword ptr [rbp - 0x41]
1800a04bf: 4c 8b c1                    	mov	r8, rcx
1800a04c2: 49 8b d7                    	mov	rdx, r15
1800a04c5: 48 8d 0d 34 44 10 01        	lea	rcx, [rip + 0x1104434]  # 0x1811a4900    ; STRING: DLSSNR: skip immediate local eval, missing committed staging (output=%p color=%p depth=%p motion=%p)
1800a04cc: e8 6f b6 05 00              	call	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
1800a04d1: e8 3a 42 fd ff              	call	0x180074710 <SetPDFrameWarpDiagnosticHud+0x4bcc0>
1800a04d6: 48 8b 0d 13 54 17 01        	mov	rcx, qword ptr [rip + 0x1175413] # 0x1812158f0
1800a04dd: e8 ae 70 fd ff              	call	0x180077590 <SetPDFrameWarpDiagnosticHud+0x4eb40>
1800a04e2: 90                          	nop
1800a04e3: 48 8b cb                    	mov	rcx, rbx
1800a04e6: ff 15 3c 30 07 00           	call	qword ptr [rip + 0x7303c] # 0x180113528
1800a04ec: e9 e6 03 00 00              	jmp	0x1800a08d7 <SetPDFrameWarpNativeCameraSource+0x28047>
1800a04f1: 4c 8b 05 f8 53 17 01        	mov	r8, qword ptr [rip + 0x11753f8] # 0x1812158f0
1800a04f8: 49 83 b8 a8 00 00 00 00     	cmp	qword ptr [r8 + 0xa8], 0x0
1800a0500: 0f 84 d1 03 00 00           	je	0x1800a08d7 <SetPDFrameWarpNativeCameraSource+0x28047>
1800a0506: 49 8b 50 30                 	mov	rdx, qword ptr [r8 + 0x30]
1800a050a: 48 85 d2                    	test	rdx, rdx
1800a050d: 74 1a                       	je	0x1800a0529 <SetPDFrameWarpNativeCameraSource+0x27c99>
1800a050f: 49 8b 06                    	mov	rax, qword ptr [r14]
1800a0512: 49 ff 80 18 02 00 00        	inc	qword ptr [r8 + 0x218]
1800a0519: 4d 8b 80 18 02 00 00        	mov	r8, qword ptr [r8 + 0x218]
1800a0520: 49 8b ce                    	mov	rcx, r14
1800a0523: ff 90 98 04 00 00           	call	qword ptr [rax + 0x498]
1800a0529: 49 8b 06                    	mov	rax, qword ptr [r14]
1800a052c: 49 8b ce                    	mov	rcx, r14
1800a052f: ff 90 78 03 00 00           	call	qword ptr [rax + 0x378]
1800a0535: 33 ff                       	xor	edi, edi
1800a0537: 48 89 7d 0f                 	mov	qword ptr [rbp + 0xf], rdi
1800a053b: c7 45 07 04 00 00 00        	mov	dword ptr [rbp + 0x7], 0x4
1800a0542: c7 45 0b ff ff ff ff        	mov	dword ptr [rbp + 0xb], 0xffffffff
1800a0549: 48 8b 1d a0 53 17 01        	mov	rbx, qword ptr [rip + 0x11753a0] # 0x1812158f0
1800a0550: 48 8b 83 98 0c 00 00        	mov	rax, qword ptr [rbx + 0xc98]
1800a0557: 48 39 83 90 0c 00 00        	cmp	qword ptr [rbx + 0xc90], rax
1800a055e: 74 1a                       	je	0x1800a057a <SetPDFrameWarpNativeCameraSource+0x27cea>
1800a0560: 4c 8d 4d 07                 	lea	r9, [rbp + 0x7]
1800a0564: 4c 8b 83 18 02 00 00        	mov	r8, qword ptr [rbx + 0x218]
1800a056b: ba 02 00 00 00              	mov	edx, 0x2
1800a0570: 48 8b cb                    	mov	rcx, rbx
1800a0573: e8 a8 53 fd ff              	call	0x180075920 <SetPDFrameWarpDiagnosticHud+0x4ced0>
1800a0578: eb 51                       	jmp	0x1800a05cb <SetPDFrameWarpNativeCameraSource+0x27d3b>
1800a057a: 48 81 c3 38 09 00 00        	add	rbx, 0x938
1800a0581: 48 89 5d f7                 	mov	qword ptr [rbp - 0x9], rbx
1800a0585: 48 8b cb                    	mov	rcx, rbx
1800a0588: ff 15 aa 2f 07 00           	call	qword ptr [rip + 0x72faa] # 0x180113538
1800a058e: 85 c0                       	test	eax, eax
1800a0590: 0f 85 1d 03 00 00           	jne	0x1800a08b3 <SetPDFrameWarpNativeCameraSource+0x28023>
1800a0596: 81 7b 4c ff ff ff 7f        	cmp	dword ptr [rbx + 0x4c], 0x7fffffff
1800a059d: 74 5c                       	je	0x1800a05fb <SetPDFrameWarpNativeCameraSource+0x27d6b>
1800a059f: 48 8b 15 4a 53 17 01        	mov	rdx, qword ptr [rip + 0x117534a] # 0x1812158f0
1800a05a6: 48 8b 8a a8 00 00 00        	mov	rcx, qword ptr [rdx + 0xa8]
1800a05ad: 48 8b 01                    	mov	rax, qword ptr [rcx]
1800a05b0: 4c 8b 82 18 02 00 00        	mov	r8, qword ptr [rdx + 0x218]
1800a05b7: 48 8b 92 88 01 00 00        	mov	rdx, qword ptr [rdx + 0x188]
1800a05be: ff 50 78                    	call	qword ptr [rax + 0x78]
1800a05c1: 90                          	nop
1800a05c2: 48 8b cb                    	mov	rcx, rbx
1800a05c5: ff 15 5d 2f 07 00           	call	qword ptr [rip + 0x72f5d] # 0x180113528
1800a05cb: 48 8b 1d 1e 53 17 01        	mov	rbx, qword ptr [rip + 0x117531e] # 0x1812158f0
1800a05d2: 48 81 c3 c8 0a 00 00        	add	rbx, 0xac8
1800a05d9: 48 89 5d f7                 	mov	qword ptr [rbp - 0x9], rbx
1800a05dd: c6 45 ff 00                 	mov	byte ptr [rbp - 0x1], 0x0
1800a05e1: 48 8b cb                    	mov	rcx, rbx
1800a05e4: ff 15 4e 2f 07 00           	call	qword ptr [rip + 0x72f4e] # 0x180113538
1800a05ea: 85 c0                       	test	eax, eax
1800a05ec: 0f 85 c1 02 00 00           	jne	0x1800a08b3 <SetPDFrameWarpNativeCameraSource+0x28023>
1800a05f2: 81 7b 4c ff ff ff 7f        	cmp	dword ptr [rbx + 0x4c], 0x7fffffff
1800a05f9: 75 13                       	jne	0x1800a060e <SetPDFrameWarpNativeCameraSource+0x27d7e>
1800a05fb: c7 43 4c fe ff ff 7f        	mov	dword ptr [rbx + 0x4c], 0x7ffffffe
1800a0602: b9 06 00 00 00              	mov	ecx, 0x6
1800a0607: ff 15 33 2f 07 00           	call	qword ptr [rip + 0x72f33] # 0x180113540
1800a060d: cc                          	int3
1800a060e: c6 45 ff 01                 	mov	byte ptr [rbp - 0x1], 0x1
1800a0612: ba 02 00 00 00              	mov	edx, 0x2
1800a0617: 48 8b 0d d2 52 17 01        	mov	rcx, qword ptr [rip + 0x11752d2] # 0x1812158f0
1800a061e: e8 ed 2d fd ff              	call	0x180073410 <SetPDFrameWarpDiagnosticHud+0x4a9c0>
1800a0623: 48 8b c8                    	mov	rcx, rax
1800a0626: 48 89 45 ef                 	mov	qword ptr [rbp - 0x11], rax
1800a062a: 48 8b 05 bf 52 17 01        	mov	rax, qword ptr [rip + 0x11752bf] # 0x1812158f0
1800a0631: 48 8b 90 98 0c 00 00        	mov	rdx, qword ptr [rax + 0xc98]
1800a0638: 48 39 90 90 0c 00 00        	cmp	qword ptr [rax + 0xc90], rdx
1800a063f: 74 1c                       	je	0x1800a065d <SetPDFrameWarpNativeCameraSource+0x27dcd>
1800a0641: 4c 8d 4d 07                 	lea	r9, [rbp + 0x7]
1800a0645: 41 b8 02 00 00 00           	mov	r8d, 0x2
1800a064b: 48 8b d1                    	mov	rdx, rcx
1800a064e: 48 8b c8                    	mov	rcx, rax
1800a0651: e8 8a 59 fd ff              	call	0x180075fe0 <SetPDFrameWarpDiagnosticHud+0x4d590>
1800a0656: 48 8b 05 93 52 17 01        	mov	rax, qword ptr [rip + 0x1175293] # 0x1812158f0
1800a065d: 48 8b 56 08                 	mov	rdx, qword ptr [rsi + 0x8]
1800a0661: 48 85 d2                    	test	rdx, rdx
1800a0664: 74 0d                       	je	0x1800a0673 <SetPDFrameWarpNativeCameraSource+0x27de3>
1800a0666: 48 8b 88 a0 04 00 00        	mov	rcx, qword ptr [rax + 0x4a0]
1800a066d: 48 89 4d b7                 	mov	qword ptr [rbp - 0x49], rcx
1800a0671: eb 04                       	jmp	0x1800a0677 <SetPDFrameWarpNativeCameraSource+0x27de7>
1800a0673: 48 89 7d b7                 	mov	qword ptr [rbp - 0x49], rdi
1800a0677: 4c 8b a0 a8 04 00 00        	mov	r12, qword ptr [rax + 0x4a8]
1800a067e: 48 83 7e 18 00              	cmp	qword ptr [rsi + 0x18], 0x0
1800a0683: 74 0d                       	je	0x1800a0692 <SetPDFrameWarpNativeCameraSource+0x27e02>
1800a0685: 48 8b 88 90 03 00 00        	mov	rcx, qword ptr [rax + 0x390]
1800a068c: 48 89 4d bf                 	mov	qword ptr [rbp - 0x41], rcx
1800a0690: eb 04                       	jmp	0x1800a0696 <SetPDFrameWarpNativeCameraSource+0x27e06>
1800a0692: 48 89 7d bf                 	mov	qword ptr [rbp - 0x41], rdi
1800a0696: 48 83 7e 10 00              	cmp	qword ptr [rsi + 0x10], 0x0
1800a069b: 74 0d                       	je	0x1800a06aa <SetPDFrameWarpNativeCameraSource+0x27e1a>
1800a069d: 48 8b 88 98 03 00 00        	mov	rcx, qword ptr [rax + 0x398]
1800a06a4: 48 89 4d cf                 	mov	qword ptr [rbp - 0x31], rcx
1800a06a8: eb 04                       	jmp	0x1800a06ae <SetPDFrameWarpNativeCameraSource+0x27e1e>
1800a06aa: 48 89 7d cf                 	mov	qword ptr [rbp - 0x31], rdi
1800a06ae: 48 83 7e 28 00              	cmp	qword ptr [rsi + 0x28], 0x0
1800a06b3: 74 0d                       	je	0x1800a06c2 <SetPDFrameWarpNativeCameraSource+0x27e32>
1800a06b5: 48 8b 88 78 04 00 00        	mov	rcx, qword ptr [rax + 0x478]
1800a06bc: 48 89 4d d7                 	mov	qword ptr [rbp - 0x29], rcx
1800a06c0: eb 04                       	jmp	0x1800a06c6 <SetPDFrameWarpNativeCameraSource+0x27e36>
1800a06c2: 48 89 7d d7                 	mov	qword ptr [rbp - 0x29], rdi
1800a06c6: 48 83 7e 30 00              	cmp	qword ptr [rsi + 0x30], 0x0
1800a06cb: 74 0d                       	je	0x1800a06da <SetPDFrameWarpNativeCameraSource+0x27e4a>
1800a06cd: 48 8b 88 88 04 00 00        	mov	rcx, qword ptr [rax + 0x488]
1800a06d4: 48 89 4d df                 	mov	qword ptr [rbp - 0x21], rcx
1800a06d8: eb 04                       	jmp	0x1800a06de <SetPDFrameWarpNativeCameraSource+0x27e4e>
1800a06da: 48 89 7d df                 	mov	qword ptr [rbp - 0x21], rdi
1800a06de: 48 83 7e 38 00              	cmp	qword ptr [rsi + 0x38], 0x0
1800a06e3: 74 0d                       	je	0x1800a06f2 <SetPDFrameWarpNativeCameraSource+0x27e62>
1800a06e5: 48 8b 88 80 04 00 00        	mov	rcx, qword ptr [rax + 0x480]
1800a06ec: 48 89 4d 07                 	mov	qword ptr [rbp + 0x7], rcx
1800a06f0: eb 04                       	jmp	0x1800a06f6 <SetPDFrameWarpNativeCameraSource+0x27e66>
1800a06f2: 48 89 7d 07                 	mov	qword ptr [rbp + 0x7], rdi
1800a06f6: 48 83 7e 48 00              	cmp	qword ptr [rsi + 0x48], 0x0
1800a06fb: 74 09                       	je	0x1800a0706 <SetPDFrameWarpNativeCameraSource+0x27e76>
1800a06fd: 4c 8b a8 b0 04 00 00        	mov	r13, qword ptr [rax + 0x4b0]
1800a0704: eb 03                       	jmp	0x1800a0709 <SetPDFrameWarpNativeCameraSource+0x27e79>
1800a0706: 4c 8b ef                    	mov	r13, rdi
1800a0709: 4d 8b fc                    	mov	r15, r12
1800a070c: 48 8b 4e 40                 	mov	rcx, qword ptr [rsi + 0x40]
1800a0710: 48 85 c9                    	test	rcx, rcx
1800a0713: 74 13                       	je	0x1800a0728 <SetPDFrameWarpNativeCameraSource+0x27e98>
1800a0715: 48 3b ca                    	cmp	rcx, rdx
1800a0718: 75 0e                       	jne	0x1800a0728 <SetPDFrameWarpNativeCameraSource+0x27e98>
1800a071a: 48 8b 80 a0 04 00 00        	mov	rax, qword ptr [rax + 0x4a0]
1800a0721: 48 85 c0                    	test	rax, rax
1800a0724: 4c 0f 45 f8                 	cmovne	r15, rax
1800a0728: 48 8b 4d e7                 	mov	rcx, qword ptr [rbp - 0x19]
1800a072c: 48 83 c1 38                 	add	rcx, 0x38
1800a0730: 48 8b d6                    	mov	rdx, rsi
1800a0733: e8 38 17 ff ff              	call	0x180091e70 <SetPDFrameWarpNativeCameraSource+0x195e0>
1800a0738: 4c 89 7c 24 60              	mov	qword ptr [rsp + 0x60], r15
1800a073d: 4c 89 6c 24 58              	mov	qword ptr [rsp + 0x58], r13
1800a0742: 48 8b 4d 07                 	mov	rcx, qword ptr [rbp + 0x7]
1800a0746: 48 89 4c 24 50              	mov	qword ptr [rsp + 0x50], rcx
1800a074b: 48 8b 4d df                 	mov	rcx, qword ptr [rbp - 0x21]
1800a074f: 48 89 4c 24 48              	mov	qword ptr [rsp + 0x48], rcx
1800a0754: 48 8b 4d d7                 	mov	rcx, qword ptr [rbp - 0x29]
1800a0758: 48 89 4c 24 40              	mov	qword ptr [rsp + 0x40], rcx
1800a075d: 48 8b 4d cf                 	mov	rcx, qword ptr [rbp - 0x31]
1800a0761: 48 89 4c 24 38              	mov	qword ptr [rsp + 0x38], rcx
1800a0766: 48 8b 4d bf                 	mov	rcx, qword ptr [rbp - 0x41]
1800a076a: 48 89 4c 24 30              	mov	qword ptr [rsp + 0x30], rcx
1800a076f: 4c 89 64 24 28              	mov	qword ptr [rsp + 0x28], r12
1800a0774: 48 8b 4d b7                 	mov	rcx, qword ptr [rbp - 0x49]
1800a0778: 48 89 4c 24 20              	mov	qword ptr [rsp + 0x20], rcx
1800a077d: 4c 8b ce                    	mov	r9, rsi
1800a0780: 4c 8b 00                    	mov	r8, qword ptr [rax]
1800a0783: 4c 8b 7d ef                 	mov	r15, qword ptr [rbp - 0x11]
1800a0787: 49 8b d7                    	mov	rdx, r15
1800a078a: 48 8b 4d e7                 	mov	rcx, qword ptr [rbp - 0x19]
1800a078e: e8 8d e9 ff ff              	call	0x18009f120 <SetPDFrameWarpNativeCameraSource+0x26890>
1800a0793: 84 c0                       	test	al, al
1800a0795: 75 0c                       	jne	0x1800a07a3 <SetPDFrameWarpNativeCameraSource+0x27f13>
1800a0797: 48 8d 0d 02 42 10 01        	lea	rcx, [rip + 0x1104202]  # 0x1811a49a0    ; STRING: NGX_D3D12_EVALUATE_DLSSNR_EXT immediate D3D11 failed
1800a079e: e8 4d bb ff ff              	call	0x18009c2f0 <SetPDFrameWarpNativeCameraSource+0x23a60>
1800a07a3: 48 8b 0d 46 51 17 01        	mov	rcx, qword ptr [rip + 0x1175146] # 0x1812158f0
1800a07aa: 48 8b 81 98 0c 00 00        	mov	rax, qword ptr [rcx + 0xc98]
1800a07b1: 48 39 81 90 0c 00 00        	cmp	qword ptr [rcx + 0xc90], rax
1800a07b8: 74 12                       	je	0x1800a07cc <SetPDFrameWarpNativeCameraSource+0x27f3c>
1800a07ba: 4d 8b c4                    	mov	r8, r12
1800a07bd: 49 8b d7                    	mov	rdx, r15
1800a07c0: e8 8b 5a fd ff              	call	0x180076250 <SetPDFrameWarpDiagnosticHud+0x4d800>
1800a07c5: 48 8b 0d 24 51 17 01        	mov	rcx, qword ptr [rip + 0x1175124] # 0x1812158f0
1800a07cc: ba 02 00 00 00              	mov	edx, 0x2
1800a07d1: e8 8a 2e fd ff              	call	0x180073660 <SetPDFrameWarpDiagnosticHud+0x4ac10>
1800a07d6: 48 85 db                    	test	rbx, rbx
1800a07d9: 0f 84 1f 01 00 00           	je	0x1800a08fe <SetPDFrameWarpNativeCameraSource+0x2806e>
1800a07df: 48 8b cb                    	mov	rcx, rbx
1800a07e2: ff 15 40 2d 07 00           	call	qword ptr [rip + 0x72d40] # 0x180113528
1800a07e8: c6 45 ff 00                 	mov	byte ptr [rbp - 0x1], 0x0
1800a07ec: 4c 8b 05 fd 50 17 01        	mov	r8, qword ptr [rip + 0x11750fd] # 0x1812158f0
1800a07f3: 49 8b 80 98 0c 00 00        	mov	rax, qword ptr [r8 + 0xc98]
1800a07fa: 49 39 80 90 0c 00 00        	cmp	qword ptr [r8 + 0xc90], rax
1800a0801: 74 0a                       	je	0x1800a080d <SetPDFrameWarpNativeCameraSource+0x27f7d>
1800a0803: 4d 8b c4                    	mov	r8, r12
1800a0806: e8 45 5d fd ff              	call	0x180076550 <SetPDFrameWarpDiagnosticHud+0x4db00>
1800a080b: eb 1c                       	jmp	0x1800a0829 <SetPDFrameWarpNativeCameraSource+0x27f99>
1800a080d: 49 8b 50 30                 	mov	rdx, qword ptr [r8 + 0x30]
1800a0811: 48 85 d2                    	test	rdx, rdx
1800a0814: 74 1a                       	je	0x1800a0830 <SetPDFrameWarpNativeCameraSource+0x27fa0>
1800a0816: 49 8b 06                    	mov	rax, qword ptr [r14]
1800a0819: 4d 8b 80 18 02 00 00        	mov	r8, qword ptr [r8 + 0x218]
1800a0820: 49 8b ce                    	mov	rcx, r14
1800a0823: ff 90 a0 04 00 00           	call	qword ptr [rax + 0x4a0]
1800a0829: 4c 8b 05 c0 50 17 01        	mov	r8, qword ptr [rip + 0x11750c0] # 0x1812158f0
1800a0830: 48 8b 56 20                 	mov	rdx, qword ptr [rsi + 0x20]
1800a0834: 48 85 d2                    	test	rdx, rdx
1800a0837: 74 78                       	je	0x1800a08b1 <SetPDFrameWarpNativeCameraSource+0x28021>
1800a0839: 8b 86 88 00 00 00           	mov	eax, dword ptr [rsi + 0x88]
1800a083f: 49 8b ce                    	mov	rcx, r14
1800a0842: 85 c0                       	test	eax, eax
1800a0844: 74 5a                       	je	0x1800a08a0 <SetPDFrameWarpNativeCameraSource+0x28010>
1800a0846: 48 89 7d 07                 	mov	qword ptr [rbp + 0x7], rdi
1800a084a: 89 45 13                    	mov	dword ptr [rbp + 0x13], eax
1800a084d: 8b 86 8c 00 00 00           	mov	eax, dword ptr [rsi + 0x8c]
1800a0853: 89 45 17                    	mov	dword ptr [rbp + 0x17], eax
1800a0856: 89 7d 0f                    	mov	dword ptr [rbp + 0xf], edi
1800a0859: c7 45 1b 01 00 00 00        	mov	dword ptr [rbp + 0x1b], 0x1
1800a0860: 49 8b 06                    	mov	rax, qword ptr [r14]
1800a0863: 4c 8b 90 70 01 00 00        	mov	r10, qword ptr [rax + 0x170]
1800a086a: 48 8d 45 07                 	lea	rax, [rbp + 0x7]
1800a086e: 48 89 44 24 40              	mov	qword ptr [rsp + 0x40], rax
1800a0873: 89 7c 24 38                 	mov	dword ptr [rsp + 0x38], edi
1800a0877: 49 8b 80 38 08 00 00        	mov	rax, qword ptr [r8 + 0x838]
1800a087e: 48 89 44 24 30              	mov	qword ptr [rsp + 0x30], rax
1800a0883: 89 7c 24 28                 	mov	dword ptr [rsp + 0x28], edi
1800a0887: 8b 86 84 00 00 00           	mov	eax, dword ptr [rsi + 0x84]
1800a088d: 89 44 24 20                 	mov	dword ptr [rsp + 0x20], eax
1800a0891: 44 8b 8e 80 00 00 00        	mov	r9d, dword ptr [rsi + 0x80]
1800a0898: 45 33 c0                    	xor	r8d, r8d
1800a089b: 41 ff d2                    	call	r10
1800a089e: eb 11                       	jmp	0x1800a08b1 <SetPDFrameWarpNativeCameraSource+0x28021>
1800a08a0: 49 8b 06                    	mov	rax, qword ptr [r14]
1800a08a3: 4d 8b 80 38 08 00 00        	mov	r8, qword ptr [r8 + 0x838]
1800a08aa: ff 90 78 01 00 00           	call	qword ptr [rax + 0x178]
1800a08b0: 90                          	nop
1800a08b1: eb 24                       	jmp	0x1800a08d7 <SetPDFrameWarpNativeCameraSource+0x28047>
1800a08b3: b9 05 00 00 00              	mov	ecx, 0x5
1800a08b8: ff 15 82 2c 07 00           	call	qword ptr [rip + 0x72c82] # 0x180113540
1800a08be: cc                          	int3
1800a08bf: 48 8d 0d b2 3f 10 01        	lea	rcx, [rip + 0x1103fb2]  # 0x1811a4878    ; STRING: DLSSNR: missing output share pair for immediate evaluate.
1800a08c6: e8 75 b2 05 00              	call	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
1800a08cb: 48 8b 0d 1e 50 17 01        	mov	rcx, qword ptr [rip + 0x117501e] # 0x1812158f0
1800a08d2: e8 b9 6c fd ff              	call	0x180077590 <SetPDFrameWarpDiagnosticHud+0x4eb40>
1800a08d7: 48 8b 4d 1f                 	mov	rcx, qword ptr [rbp + 0x1f]
1800a08db: 48 33 cc                    	xor	rcx, rsp
1800a08de: e8 8d b9 06 00              	call	0x18010c270 <NVSDK_NGX_UpdateFeature+0x2e30>
1800a08e3: 48 8b 9c 24 30 01 00 00     	mov	rbx, qword ptr [rsp + 0x130]
1800a08eb: 48 81 c4 e0 00 00 00        	add	rsp, 0xe0
1800a08f2: 41 5f                       	pop	r15
1800a08f4: 41 5e                       	pop	r14
1800a08f6: 41 5d                       	pop	r13
1800a08f8: 41 5c                       	pop	r12
1800a08fa: 5f                          	pop	rdi
1800a08fb: 5e                          	pop	rsi
1800a08fc: 5d                          	pop	rbp
1800a08fd: c3                          	ret
1800a08fe: b9 01 00 00 00              	mov	ecx, 0x1
1800a0903: e8 c8 c6 fc ff              	call	0x18006cfd0 <SetPDFrameWarpDiagnosticHud+0x44580>
1800a0908: cc                          	int3