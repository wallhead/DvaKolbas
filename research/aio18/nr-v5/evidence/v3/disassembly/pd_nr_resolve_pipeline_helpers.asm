
/mnt/data/aio18_nr_work/PDPerfPlugin.dll:	file format coff-x86-64

Disassembly of section .text:

0000000180078890 <SetPDFrameWarpNativeCameraSource>:
18009d4b0: 40 55                       	push	rbp
18009d4b2: 41 54                       	push	r12
18009d4b4: 41 55                       	push	r13
18009d4b6: 41 56                       	push	r14
18009d4b8: 41 57                       	push	r15
18009d4ba: 48 8d ac 24 50 ff ff ff     	lea	rbp, [rsp - 0xb0]
18009d4c2: 48 81 ec b0 01 00 00        	sub	rsp, 0x1b0
18009d4c9: 48 8b 05 f0 d4 12 01        	mov	rax, qword ptr [rip + 0x112d4f0] # 0x1811ca9c0
18009d4d0: 48 33 c4                    	xor	rax, rsp
18009d4d3: 48 89 85 a0 00 00 00        	mov	qword ptr [rbp + 0xa0], rax
18009d4da: 48 83 79 08 00              	cmp	qword ptr [rcx + 0x8], 0x0
18009d4df: 4c 8b f1                    	mov	r14, rcx
18009d4e2: 0f 84 37 01 00 00           	je	0x18009d61f <SetPDFrameWarpNativeCameraSource+0x24d8f>
18009d4e8: 4c 8d b9 18 01 00 00        	lea	r15, [rcx + 0x118]
18009d4ef: 49 83 3f 00                 	cmp	qword ptr [r15], 0x0
18009d4f3: 74 43                       	je	0x18009d538 <SetPDFrameWarpNativeCameraSource+0x24ca8>
18009d4f5: 48 83 b9 20 01 00 00 00     	cmp	qword ptr [rcx + 0x120], 0x0
18009d4fd: 74 39                       	je	0x18009d538 <SetPDFrameWarpNativeCameraSource+0x24ca8>
18009d4ff: 48 83 b9 28 01 00 00 00     	cmp	qword ptr [rcx + 0x128], 0x0
18009d507: 74 2f                       	je	0x18009d538 <SetPDFrameWarpNativeCameraSource+0x24ca8>
18009d509: 48 83 b9 30 01 00 00 00     	cmp	qword ptr [rcx + 0x130], 0x0
18009d511: 74 25                       	je	0x18009d538 <SetPDFrameWarpNativeCameraSource+0x24ca8>
18009d513: 48 83 b9 38 01 00 00 00     	cmp	qword ptr [rcx + 0x138], 0x0
18009d51b: 74 1b                       	je	0x18009d538 <SetPDFrameWarpNativeCameraSource+0x24ca8>
18009d51d: 48 83 b9 40 01 00 00 00     	cmp	qword ptr [rcx + 0x140], 0x0
18009d525: 74 11                       	je	0x18009d538 <SetPDFrameWarpNativeCameraSource+0x24ca8>
18009d527: 48 83 b9 48 01 00 00 00     	cmp	qword ptr [rcx + 0x148], 0x0
18009d52f: 74 07                       	je	0x18009d538 <SetPDFrameWarpNativeCameraSource+0x24ca8>
18009d531: b0 01                       	mov	al, 0x1
18009d533: e9 fe 03 00 00              	jmp	0x18009d936 <SetPDFrameWarpNativeCameraSource+0x250a6>
18009d538: e8 a3 fd ff ff              	call	0x18009d2e0 <SetPDFrameWarpNativeCameraSource+0x24a50>
18009d53d: 45 33 e4                    	xor	r12d, r12d
18009d540: 48 c7 45 14 03 00 00 00     	mov	qword ptr [rbp + 0x14], 0x3
18009d548: 0f 57 c0                    	xorps	xmm0, xmm0
18009d54b: 44 89 65 10                 	mov	dword ptr [rbp + 0x10], r12d
18009d54f: 41 bd 01 00 00 00           	mov	r13d, 0x1
18009d555: 4c 89 65 1c                 	mov	qword ptr [rbp + 0x1c], r12
18009d559: 0f 11 45 70                 	movups	xmmword ptr [rbp + 0x70], xmm0
18009d55d: 48 8d 45 10                 	lea	rax, [rbp + 0x10]
18009d561: 41 8b d5                    	mov	edx, r13d
18009d564: 48 89 45 70                 	mov	qword ptr [rbp + 0x70], rax
18009d568: 4c 8d 4d 80                 	lea	r9, [rbp - 0x80]
18009d56c: 0f 11 85 90 00 00 00        	movups	xmmword ptr [rbp + 0x90], xmm0
18009d573: 48 8d 45 24                 	lea	rax, [rbp + 0x24]
18009d577: 44 89 6d 24                 	mov	dword ptr [rbp + 0x24], r13d
18009d57b: 0f 11 45 40                 	movups	xmmword ptr [rbp + 0x40], xmm0
18009d57f: 48 89 85 90 00 00 00        	mov	qword ptr [rbp + 0x90], rax
18009d586: 48 8d 45 40                 	lea	rax, [rbp + 0x40]
18009d58a: 0f 11 45 60                 	movups	xmmword ptr [rbp + 0x60], xmm0
18009d58e: 4c 8d 44 24 40              	lea	r8, [rsp + 0x40]
18009d593: 48 89 45 90                 	mov	qword ptr [rbp - 0x70], rax
18009d597: 0f 11 85 80 00 00 00        	movups	xmmword ptr [rbp + 0x80], xmm0
18009d59e: 48 8d 4d 88                 	lea	rcx, [rbp - 0x78]
18009d5a2: 4c 89 6d 28                 	mov	qword ptr [rbp + 0x28], r13
18009d5a6: 0f 11 45 50                 	movups	xmmword ptr [rbp + 0x50], xmm0
18009d5aa: 4c 89 65 30                 	mov	qword ptr [rbp + 0x30], r12
18009d5ae: c7 45 40 02 00 00 00        	mov	dword ptr [rbp + 0x40], 0x2
18009d5b5: 44 89 65 58                 	mov	dword ptr [rbp + 0x58], r12d
18009d5b9: 4c 89 65 48                 	mov	qword ptr [rbp + 0x48], r12
18009d5bd: 44 89 65 60                 	mov	dword ptr [rbp + 0x60], r12d
18009d5c1: 44 89 65 78                 	mov	dword ptr [rbp + 0x78], r12d
18009d5c5: 44 89 6d 68                 	mov	dword ptr [rbp + 0x68], r13d
18009d5c9: 44 89 a5 80 00 00 00        	mov	dword ptr [rbp + 0x80], r12d
18009d5d0: 44 89 a5 98 00 00 00        	mov	dword ptr [rbp + 0x98], r12d
18009d5d7: 44 89 ad 88 00 00 00        	mov	dword ptr [rbp + 0x88], r13d
18009d5de: 48 c7 45 88 03 00 00 00     	mov	qword ptr [rbp - 0x78], 0x3
18009d5e6: f3 0f 7f 45 98              	movdqu	xmmword ptr [rbp - 0x68], xmm0
18009d5eb: 4c 89 65 a8                 	mov	qword ptr [rbp - 0x58], r12
18009d5ef: 4c 89 64 24 40              	mov	qword ptr [rsp + 0x40], r12
18009d5f4: 4c 89 65 80                 	mov	qword ptr [rbp - 0x80], r12
18009d5f8: ff 15 aa 63 07 00           	call	qword ptr [rip + 0x763aa] # 0x1801139a8
18009d5fe: 85 c0                       	test	eax, eax
18009d600: 79 24                       	jns	0x18009d626 <SetPDFrameWarpNativeCameraSource+0x24d96>
18009d602: 8b d0                       	mov	edx, eax
18009d604: 48 8d 0d a5 6d 10 01        	lea	rcx, [rip + 0x1106da5]  # 0x1811a43b0
18009d60b: e8 30 e5 05 00              	call	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
18009d610: 48 8b 4d 80                 	mov	rcx, qword ptr [rbp - 0x80]
18009d614: 48 85 c9                    	test	rcx, rcx
18009d617: 74 06                       	je	0x18009d61f <SetPDFrameWarpNativeCameraSource+0x24d8f>
18009d619: 48 8b 01                    	mov	rax, qword ptr [rcx]
18009d61c: ff 50 10                    	call	qword ptr [rax + 0x10]
18009d61f: 32 c0                       	xor	al, al
18009d621: e9 10 03 00 00              	jmp	0x18009d936 <SetPDFrameWarpNativeCameraSource+0x250a6>
18009d626: 48 8b 4c 24 40              	mov	rcx, qword ptr [rsp + 0x40]
18009d62b: 48 89 9c 24 e8 01 00 00     	mov	qword ptr [rsp + 0x1e8], rbx
18009d633: 48 89 b4 24 f0 01 00 00     	mov	qword ptr [rsp + 0x1f0], rsi
18009d63b: 49 8b 76 08                 	mov	rsi, qword ptr [r14 + 0x8]
18009d63f: 48 89 bc 24 f8 01 00 00     	mov	qword ptr [rsp + 0x1f8], rdi
18009d647: 48 8b 06                    	mov	rax, qword ptr [rsi]
18009d64a: 48 8b b8 80 00 00 00        	mov	rdi, qword ptr [rax + 0x80]
18009d651: 48 8b 01                    	mov	rax, qword ptr [rcx]
18009d654: ff 50 20                    	call	qword ptr [rax + 0x20]
18009d657: 4c 8b 44 24 40              	mov	r8, qword ptr [rsp + 0x40]
18009d65c: 48 8b d8                    	mov	rbx, rax
18009d65f: 49 8b 08                    	mov	rcx, qword ptr [r8]
18009d662: 48 8b 51 18                 	mov	rdx, qword ptr [rcx + 0x18]
18009d666: 49 8b c8                    	mov	rcx, r8
18009d669: ff d2                       	call	rdx
18009d66b: 49 8b cf                    	mov	rcx, r15
18009d66e: 4c 8b c0                    	mov	r8, rax
18009d671: e8 aa ad f6 ff              	call	0x180008420 <.text+0x7420>
18009d676: 48 89 44 24 28              	mov	qword ptr [rsp + 0x28], rax
18009d67b: 4c 8b cb                    	mov	r9, rbx
18009d67e: 48 8d 05 fb 1c 10 01        	lea	rax, [rip + 0x1101cfb]  # 0x18119f380
18009d685: 33 d2                       	xor	edx, edx
18009d687: 48 8b ce                    	mov	rcx, rsi
18009d68a: 48 89 44 24 20              	mov	qword ptr [rsp + 0x20], rax
18009d68f: ff d7                       	call	rdi
18009d691: 48 8b bc 24 f8 01 00 00     	mov	rdi, qword ptr [rsp + 0x1f8]
18009d699: 8b d8                       	mov	ebx, eax
18009d69b: 48 8b 44 24 40              	mov	rax, qword ptr [rsp + 0x40]
18009d6a0: 48 8b b4 24 f0 01 00 00     	mov	rsi, qword ptr [rsp + 0x1f0]
18009d6a8: 48 85 c0                    	test	rax, rax
18009d6ab: 74 11                       	je	0x18009d6be <SetPDFrameWarpNativeCameraSource+0x24e2e>
18009d6ad: 48 8b 08                    	mov	rcx, qword ptr [rax]
18009d6b0: 48 8b 51 10                 	mov	rdx, qword ptr [rcx + 0x10]
18009d6b4: 48 8b c8                    	mov	rcx, rax
18009d6b7: ff d2                       	call	rdx
18009d6b9: 4c 89 64 24 40              	mov	qword ptr [rsp + 0x40], r12
18009d6be: 48 8b 4d 80                 	mov	rcx, qword ptr [rbp - 0x80]
18009d6c2: 48 85 c9                    	test	rcx, rcx
18009d6c5: 74 0a                       	je	0x18009d6d1 <SetPDFrameWarpNativeCameraSource+0x24e41>
18009d6c7: 48 8b 01                    	mov	rax, qword ptr [rcx]
18009d6ca: ff 50 10                    	call	qword ptr [rax + 0x10]
18009d6cd: 4c 89 65 80                 	mov	qword ptr [rbp - 0x80], r12
18009d6d1: 85 db                       	test	ebx, ebx
18009d6d3: 79 13                       	jns	0x18009d6e8 <SetPDFrameWarpNativeCameraSource+0x24e58>
18009d6d5: 8b d3                       	mov	edx, ebx
18009d6d7: 48 8d 0d 42 6d 10 01        	lea	rcx, [rip + 0x1106d42]  # 0x1811a4420
18009d6de: e8 5d e4 05 00              	call	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
18009d6e3: e9 44 02 00 00              	jmp	0x18009d92c <SetPDFrameWarpNativeCameraSource+0x2509c>
18009d6e8: 49 8b 07                    	mov	rax, qword ptr [r15]
18009d6eb: 49 8d 8e 20 01 00 00        	lea	rcx, [r14 + 0x120]
18009d6f2: 4d 8b 56 08                 	mov	r10, qword ptr [r14 + 0x8]
18009d6f6: 0f 57 c0                    	xorps	xmm0, xmm0
18009d6f9: 48 89 44 24 48              	mov	qword ptr [rsp + 0x48], rax
18009d6fe: 0f 57 c9                    	xorps	xmm1, xmm1
18009d701: 48 8d 05 a8 53 75 00        	lea	rax, [rip + 0x7553a8]   # 0x1807f2ab0
18009d708: 48 c7 44 24 58 60 51 00 00  	mov	qword ptr [rsp + 0x58], 0x5160
18009d711: 48 89 44 24 50              	mov	qword ptr [rsp + 0x50], rax
18009d716: f3 0f 7f 44 24 60           	movdqu	xmmword ptr [rsp + 0x60], xmm0
18009d71c: f3 0f 7f 4c 24 70           	movdqu	xmmword ptr [rsp + 0x70], xmm1
18009d722: e8 f9 ac f6 ff              	call	0x180008420 <.text+0x7420>
18009d727: 4c 8b c8                    	mov	r9, rax
18009d72a: 4c 8d 05 6f 1a 10 01        	lea	r8, [rip + 0x1101a6f]   # 0x18119f1a0
18009d731: 49 8b 02                    	mov	rax, qword ptr [r10]
18009d734: 48 8d 54 24 48              	lea	rdx, [rsp + 0x48]
18009d739: 49 8b ca                    	mov	rcx, r10
18009d73c: ff 50 58                    	call	qword ptr [rax + 0x58]
18009d73f: 8b d8                       	mov	ebx, eax
18009d741: 85 c0                       	test	eax, eax
18009d743: 79 16                       	jns	0x18009d75b <SetPDFrameWarpNativeCameraSource+0x24ecb>
18009d745: 44 8b c0                    	mov	r8d, eax
18009d748: 48 8d 15 c1 6c 10 01        	lea	rdx, [rip + 0x1106cc1]  # 0x1811a4410
18009d74f: 48 8d 0d 92 6c 10 01        	lea	rcx, [rip + 0x1106c92]  # 0x1811a43e8
18009d756: e8 e5 e3 05 00              	call	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
18009d75b: c1 eb 1f                    	shr	ebx, 0x1f
18009d75e: 84 db                       	test	bl, bl
18009d760: 0f 85 be 01 00 00           	jne	0x18009d924 <SetPDFrameWarpNativeCameraSource+0x25094>
18009d766: 49 8b 07                    	mov	rax, qword ptr [r15]
18009d769: 49 8d 8e 28 01 00 00        	lea	rcx, [r14 + 0x128]
18009d770: 4d 8b 56 08                 	mov	r10, qword ptr [r14 + 0x8]
18009d774: 0f 57 c0                    	xorps	xmm0, xmm0
18009d777: 48 89 44 24 48              	mov	qword ptr [rsp + 0x48], rax
18009d77c: 0f 57 c9                    	xorps	xmm1, xmm1
18009d77f: 48 8d 05 ba 79 68 00        	lea	rax, [rip + 0x6879ba]   # 0x180725140
18009d786: 48 c7 44 24 58 1c 98 00 00  	mov	qword ptr [rsp + 0x58], 0x981c
18009d78f: 48 89 44 24 50              	mov	qword ptr [rsp + 0x50], rax
18009d794: f3 0f 7f 44 24 60           	movdqu	xmmword ptr [rsp + 0x60], xmm0
18009d79a: f3 0f 7f 4c 24 70           	movdqu	xmmword ptr [rsp + 0x70], xmm1
18009d7a0: e8 7b ac f6 ff              	call	0x180008420 <.text+0x7420>
18009d7a5: 4c 8b c8                    	mov	r9, rax
18009d7a8: 4c 8d 05 f1 19 10 01        	lea	r8, [rip + 0x11019f1]   # 0x18119f1a0
18009d7af: 49 8b 02                    	mov	rax, qword ptr [r10]
18009d7b2: 48 8d 54 24 48              	lea	rdx, [rsp + 0x48]
18009d7b7: 49 8b ca                    	mov	rcx, r10
18009d7ba: ff 50 58                    	call	qword ptr [rax + 0x58]
18009d7bd: 8b d8                       	mov	ebx, eax
18009d7bf: 85 c0                       	test	eax, eax
18009d7c1: 79 16                       	jns	0x18009d7d9 <SetPDFrameWarpNativeCameraSource+0x24f49>
18009d7c3: 44 8b c0                    	mov	r8d, eax
18009d7c6: 48 8d 15 93 6c 10 01        	lea	rdx, [rip + 0x1106c93]  # 0x1811a4460
18009d7cd: 48 8d 0d 14 6c 10 01        	lea	rcx, [rip + 0x1106c14]  # 0x1811a43e8
18009d7d4: e8 67 e3 05 00              	call	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
18009d7d9: c1 eb 1f                    	shr	ebx, 0x1f
18009d7dc: 84 db                       	test	bl, bl
18009d7de: 0f 85 40 01 00 00           	jne	0x18009d924 <SetPDFrameWarpNativeCameraSource+0x25094>
18009d7e4: 49 8b 07                    	mov	rax, qword ptr [r15]
18009d7e7: 49 8d 8e 30 01 00 00        	lea	rcx, [r14 + 0x130]
18009d7ee: 4d 8b 56 08                 	mov	r10, qword ptr [r14 + 0x8]
18009d7f2: 0f 57 c0                    	xorps	xmm0, xmm0
18009d7f5: 48 89 44 24 48              	mov	qword ptr [rsp + 0x48], rax
18009d7fa: 0f 57 c9                    	xorps	xmm1, xmm1
18009d7fd: 48 8d 05 5c 11 69 00        	lea	rax, [rip + 0x69115c]   # 0x18072e960
18009d804: 48 c7 44 24 58 54 1e 03 00  	mov	qword ptr [rsp + 0x58], 0x31e54
18009d80d: 48 89 44 24 50              	mov	qword ptr [rsp + 0x50], rax
18009d812: f3 0f 7f 44 24 60           	movdqu	xmmword ptr [rsp + 0x60], xmm0
18009d818: f3 0f 7f 4c 24 70           	movdqu	xmmword ptr [rsp + 0x70], xmm1
18009d81e: e8 fd ab f6 ff              	call	0x180008420 <.text+0x7420>
18009d823: 4c 8b c8                    	mov	r9, rax
18009d826: 4c 8d 05 73 19 10 01        	lea	r8, [rip + 0x1101973]   # 0x18119f1a0
18009d82d: 49 8b 02                    	mov	rax, qword ptr [r10]
18009d830: 48 8d 54 24 48              	lea	rdx, [rsp + 0x48]
18009d835: 49 8b ca                    	mov	rcx, r10
18009d838: ff 50 58                    	call	qword ptr [rax + 0x58]
18009d83b: 8b d8                       	mov	ebx, eax
18009d83d: 85 c0                       	test	eax, eax
18009d83f: 79 16                       	jns	0x18009d857 <SetPDFrameWarpNativeCameraSource+0x24fc7>
18009d841: 44 8b c0                    	mov	r8d, eax
18009d844: 48 8d 15 09 6c 10 01        	lea	rdx, [rip + 0x1106c09]  # 0x1811a4454
18009d84b: 48 8d 0d 96 6b 10 01        	lea	rcx, [rip + 0x1106b96]  # 0x1811a43e8
18009d852: e8 e9 e2 05 00              	call	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
18009d857: c1 eb 1f                    	shr	ebx, 0x1f
18009d85a: 84 db                       	test	bl, bl
18009d85c: 0f 85 c2 00 00 00           	jne	0x18009d924 <SetPDFrameWarpNativeCameraSource+0x25094>
18009d862: 49 8b 07                    	mov	rax, qword ptr [r15]
18009d865: 49 8d 8e 38 01 00 00        	lea	rcx, [r14 + 0x138]
18009d86c: 4d 8b 56 08                 	mov	r10, qword ptr [r14 + 0x8]
18009d870: 0f 57 c0                    	xorps	xmm0, xmm0
18009d873: 48 89 44 24 48              	mov	qword ptr [rsp + 0x48], rax
18009d878: 0f 57 c9                    	xorps	xmm1, xmm1
18009d87b: 48 8d 05 7e c6 67 00        	lea	rax, [rip + 0x67c67e]   # 0x180719f00
18009d882: 48 c7 44 24 58 cc 50 00 00  	mov	qword ptr [rsp + 0x58], 0x50cc
18009d88b: 48 89 44 24 50              	mov	qword ptr [rsp + 0x50], rax
18009d890: f3 0f 7f 44 24 60           	movdqu	xmmword ptr [rsp + 0x60], xmm0
18009d896: f3 0f 7f 4c 24 70           	movdqu	xmmword ptr [rsp + 0x70], xmm1
18009d89c: e8 7f ab f6 ff              	call	0x180008420 <.text+0x7420>
18009d8a1: 4c 8b c8                    	mov	r9, rax
18009d8a4: 4c 8d 05 f5 18 10 01        	lea	r8, [rip + 0x11018f5]   # 0x18119f1a0
18009d8ab: 49 8b 02                    	mov	rax, qword ptr [r10]
18009d8ae: 48 8d 54 24 48              	lea	rdx, [rsp + 0x48]
18009d8b3: 49 8b ca                    	mov	rcx, r10
18009d8b6: ff 50 58                    	call	qword ptr [rax + 0x58]
18009d8b9: 8b d8                       	mov	ebx, eax
18009d8bb: 85 c0                       	test	eax, eax
18009d8bd: 79 16                       	jns	0x18009d8d5 <SetPDFrameWarpNativeCameraSource+0x25045>
18009d8bf: 44 8b c0                    	mov	r8d, eax
18009d8c2: 48 8d 15 df 6b 10 01        	lea	rdx, [rip + 0x1106bdf]  # 0x1811a44a8
18009d8c9: 48 8d 0d 18 6b 10 01        	lea	rcx, [rip + 0x1106b18]  # 0x1811a43e8
18009d8d0: e8 6b e2 05 00              	call	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
18009d8d5: c1 eb 1f                    	shr	ebx, 0x1f
18009d8d8: 84 db                       	test	bl, bl
18009d8da: 75 48                       	jne	0x18009d924 <SetPDFrameWarpNativeCameraSource+0x25094>
18009d8dc: 4d 8b 56 08                 	mov	r10, qword ptr [r14 + 0x8]
18009d8e0: 49 8d 8e 40 01 00 00        	lea	rcx, [r14 + 0x140]
18009d8e7: 4c 89 6d b8                 	mov	qword ptr [rbp - 0x48], r13
18009d8eb: 44 89 65 b0                 	mov	dword ptr [rbp - 0x50], r12d
18009d8ef: c7 45 b4 80 00 00 00        	mov	dword ptr [rbp - 0x4c], 0x80
18009d8f6: e8 25 ab f6 ff              	call	0x180008420 <.text+0x7420>
18009d8fb: 4c 8b c8                    	mov	r9, rax
18009d8fe: 4c 8d 05 c3 17 10 01        	lea	r8, [rip + 0x11017c3]   # 0x18119f0c8
18009d905: 49 8b 02                    	mov	rax, qword ptr [r10]
18009d908: 48 8d 55 b0                 	lea	rdx, [rbp - 0x50]
18009d90c: 49 8b ca                    	mov	rcx, r10
18009d90f: ff 50 70                    	call	qword ptr [rax + 0x70]
18009d912: 85 c0                       	test	eax, eax
18009d914: 79 40                       	jns	0x18009d956 <SetPDFrameWarpNativeCameraSource+0x250c6>
18009d916: 8b d0                       	mov	edx, eax
18009d918: 48 8d 0d 51 6b 10 01        	lea	rcx, [rip + 0x1106b51]  # 0x1811a4470
18009d91f: e8 1c e2 05 00              	call	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
18009d924: 49 8b ce                    	mov	rcx, r14
18009d927: e8 b4 f9 ff ff              	call	0x18009d2e0 <SetPDFrameWarpNativeCameraSource+0x24a50>
18009d92c: 32 c0                       	xor	al, al
18009d92e: 48 8b 9c 24 e8 01 00 00     	mov	rbx, qword ptr [rsp + 0x1e8]
18009d936: 48 8b 8d a0 00 00 00        	mov	rcx, qword ptr [rbp + 0xa0]
18009d93d: 48 33 cc                    	xor	rcx, rsp
18009d940: e8 2b e9 06 00              	call	0x18010c270 <NVSDK_NGX_UpdateFeature+0x2e30>
18009d945: 48 81 c4 b0 01 00 00        	add	rsp, 0x1b0
18009d94c: 41 5f                       	pop	r15
18009d94e: 41 5e                       	pop	r14
18009d950: 41 5d                       	pop	r13
18009d952: 41 5c                       	pop	r12
18009d954: 5d                          	pop	rbp
18009d955: c3                          	ret
18009d956: 49 8b 4e 08                 	mov	rcx, qword ptr [r14 + 0x8]
18009d95a: 33 d2                       	xor	edx, edx
18009d95c: 48 8b 01                    	mov	rax, qword ptr [rcx]
18009d95f: ff 50 78                    	call	qword ptr [rax + 0x78]
18009d962: 4d 8b 5e 08                 	mov	r11, qword ptr [r14 + 0x8]
18009d966: 49 8d 8e 48 01 00 00        	lea	rcx, [r14 + 0x148]
18009d96d: 0f 57 c0                    	xorps	xmm0, xmm0
18009d970: 48 c7 44 24 64 01 00 01 00  	mov	qword ptr [rsp + 0x64], 0x10001
18009d979: 0f 11 45 c4                 	movups	xmmword ptr [rbp - 0x3c], xmm0
18009d97d: 4c 89 6c 24 74              	mov	qword ptr [rsp + 0x74], r13
18009d982: 44 89 6c 24 48              	mov	dword ptr [rsp + 0x48], r13d
18009d987: 4c 89 64 24 50              	mov	qword ptr [rsp + 0x50], r12
18009d98c: 0f 10 44 24 48              	movups	xmm0, xmmword ptr [rsp + 0x48]
18009d991: 48 c7 44 24 58 00 20 00 00  	mov	qword ptr [rsp + 0x58], 0x2000
18009d99a: 44 89 6c 24 60              	mov	dword ptr [rsp + 0x60], r13d
18009d99f: 0f 10 4c 24 58              	movups	xmm1, xmmword ptr [rsp + 0x58]
18009d9a4: 4c 89 6c 24 6c              	mov	qword ptr [rsp + 0x6c], r13
18009d9a9: 0f 11 45 d8                 	movups	xmmword ptr [rbp - 0x28], xmm0
18009d9ad: 41 89 86 60 01 00 00        	mov	dword ptr [r14 + 0x160], eax
18009d9b4: 0f 10 44 24 68              	movups	xmm0, xmmword ptr [rsp + 0x68]
18009d9b9: c7 45 c0 02 00 00 00        	mov	dword ptr [rbp - 0x40], 0x2
18009d9c0: 0f 11 4d e8                 	movups	xmmword ptr [rbp - 0x18], xmm1
18009d9c4: f2 0f 10 4c 24 78           	movsd	xmm1, qword ptr [rsp + 0x78]
18009d9ca: 0f 11 45 f8                 	movups	xmmword ptr [rbp - 0x8], xmm0
18009d9ce: f2 0f 11 4d 08              	movsd	qword ptr [rbp + 0x8], xmm1
18009d9d3: e8 48 aa f6 ff              	call	0x180008420 <.text+0x7420>
18009d9d8: 49 8b 13                    	mov	rdx, qword ptr [r11]
18009d9db: 4c 8d 4d d8                 	lea	r9, [rbp - 0x28]
18009d9df: 48 89 44 24 38              	mov	qword ptr [rsp + 0x38], rax
18009d9e4: 45 33 c0                    	xor	r8d, r8d
18009d9e7: 48 8d 05 82 17 10 01        	lea	rax, [rip + 0x1101782]  # 0x18119f170
18009d9ee: 49 8b cb                    	mov	rcx, r11
18009d9f1: 48 89 44 24 30              	mov	qword ptr [rsp + 0x30], rax
18009d9f6: 4c 8b 92 d8 00 00 00        	mov	r10, qword ptr [rdx + 0xd8]
18009d9fd: 48 8d 55 c0                 	lea	rdx, [rbp - 0x40]
18009da01: 4c 89 64 24 28              	mov	qword ptr [rsp + 0x28], r12
18009da06: c7 44 24 20 c3 0a 00 00     	mov	dword ptr [rsp + 0x20], 0xac3
18009da0e: 41 ff d2                    	call	r10
18009da11: 85 c0                       	test	eax, eax
18009da13: 79 0e                       	jns	0x18009da23 <SetPDFrameWarpNativeCameraSource+0x25193>
18009da15: 8b d0                       	mov	edx, eax
18009da17: 48 8d 0d d2 6a 10 01        	lea	rcx, [rip + 0x1106ad2]  # 0x1811a44f0
18009da1e: e9 fc fe ff ff              	jmp	0x18009d91f <SetPDFrameWarpNativeCameraSource+0x2508f>
18009da23: 49 8b 8e 48 01 00 00        	mov	rcx, qword ptr [r14 + 0x148]
18009da2a: 4d 8d 8e 58 01 00 00        	lea	r9, [r14 + 0x158]
18009da31: 45 33 c0                    	xor	r8d, r8d
18009da34: 33 d2                       	xor	edx, edx
18009da36: 48 8b 01                    	mov	rax, qword ptr [rcx]
18009da39: ff 50 40                    	call	qword ptr [rax + 0x40]
18009da3c: 85 c0                       	test	eax, eax
18009da3e: 78 37                       	js	0x18009da77 <SetPDFrameWarpNativeCameraSource+0x251e7>
18009da40: 4d 39 a6 58 01 00 00        	cmp	qword ptr [r14 + 0x158], r12
18009da47: 74 2e                       	je	0x18009da77 <SetPDFrameWarpNativeCameraSource+0x251e7>
18009da49: 49 8b 56 08                 	mov	rdx, qword ptr [r14 + 0x8]
18009da4d: 45 8b cd                    	mov	r9d, r13d
18009da50: 44 88 6c 24 28              	mov	byte ptr [rsp + 0x28], r13b
18009da55: 45 8b c5                    	mov	r8d, r13d
18009da58: c7 44 24 20 1c 00 00 00     	mov	dword ptr [rsp + 0x20], 0x1c
18009da60: e8 bb 6e fd ff              	call	0x180074920 <SetPDFrameWarpDiagnosticHud+0x4bed0>
18009da65: 48 85 c0                    	test	rax, rax
18009da68: 49 89 86 50 01 00 00        	mov	qword ptr [r14 + 0x150], rax
18009da6f: 0f 95 c0                    	setne	al
18009da72: e9 b7 fe ff ff              	jmp	0x18009d92e <SetPDFrameWarpNativeCameraSource+0x2509e>
18009da77: 8b d0                       	mov	edx, eax
18009da79: 48 8d 0d 38 6a 10 01        	lea	rcx, [rip + 0x1106a38]  # 0x1811a44b8
18009da80: e9 9a fe ff ff              	jmp	0x18009d91f <SetPDFrameWarpNativeCameraSource+0x2508f>
18009da85: cc                          	int3
18009da86: cc                          	int3
18009da87: cc                          	int3
18009da88: cc                          	int3
18009da89: cc                          	int3
18009da8a: cc                          	int3
18009da8b: cc                          	int3
18009da8c: cc                          	int3
18009da8d: cc                          	int3
18009da8e: cc                          	int3
18009da8f: cc                          	int3
18009da90: 40 53                       	push	rbx
18009da92: 55                          	push	rbp
18009da93: 57                          	push	rdi
18009da94: 41 56                       	push	r14
18009da96: 48 83 ec 78                 	sub	rsp, 0x78
18009da9a: 48 8b 05 1f cf 12 01        	mov	rax, qword ptr [rip + 0x112cf1f] # 0x1811ca9c0
18009daa1: 48 33 c4                    	xor	rax, rsp
18009daa4: 48 89 44 24 68              	mov	qword ptr [rsp + 0x68], rax
18009daa9: 4c 8b f1                    	mov	r14, rcx
18009daac: 8b 8c 24 c0 00 00 00        	mov	ecx, dword ptr [rsp + 0xc0]
18009dab3: 41 8b f9                    	mov	edi, r9d
18009dab6: 41 8b e8                    	mov	ebp, r8d
18009dab9: 48 8b da                    	mov	rbx, rdx
18009dabc: 45 85 c0                    	test	r8d, r8d
18009dabf: 0f 8e 89 00 00 00           	jle	0x18009db4e <SetPDFrameWarpNativeCameraSource+0x252be>
18009dac5: 45 85 c9                    	test	r9d, r9d
18009dac8: 0f 8e 80 00 00 00           	jle	0x18009db4e <SetPDFrameWarpNativeCameraSource+0x252be>
18009dace: 48 89 74 24 70              	mov	qword ptr [rsp + 0x70], rsi
18009dad3: e8 88 f3 ff ff              	call	0x18009ce60 <SetPDFrameWarpNativeCameraSource+0x245d0>
18009dad8: 48 8b 0b                    	mov	rcx, qword ptr [rbx]
18009dadb: 8b f0                       	mov	esi, eax
18009dadd: 48 85 c9                    	test	rcx, rcx
18009dae0: 74 48                       	je	0x18009db2a <SetPDFrameWarpNativeCameraSource+0x2529a>
18009dae2: 48 8b 11                    	mov	rdx, qword ptr [rcx]
18009dae5: 4c 8b 42 50                 	mov	r8, qword ptr [rdx + 0x50]
18009dae9: 48 8d 54 24 30              	lea	rdx, [rsp + 0x30]
18009daee: 41 ff d0                    	call	r8
18009daf1: 39 6c 24 40                 	cmp	dword ptr [rsp + 0x40], ebp
18009daf5: 75 2b                       	jne	0x18009db22 <SetPDFrameWarpNativeCameraSource+0x25292>
18009daf7: 39 7c 24 48                 	cmp	dword ptr [rsp + 0x48], edi
18009dafb: 75 25                       	jne	0x18009db22 <SetPDFrameWarpNativeCameraSource+0x25292>
18009dafd: 39 74 24 50                 	cmp	dword ptr [rsp + 0x50], esi
18009db01: 75 1f                       	jne	0x18009db22 <SetPDFrameWarpNativeCameraSource+0x25292>
18009db03: 48 8b 03                    	mov	rax, qword ptr [rbx]
18009db06: 48 8b 74 24 70              	mov	rsi, qword ptr [rsp + 0x70]
18009db0b: 48 8b 4c 24 68              	mov	rcx, qword ptr [rsp + 0x68]
18009db10: 48 33 cc                    	xor	rcx, rsp
18009db13: e8 58 e7 06 00              	call	0x18010c270 <NVSDK_NGX_UpdateFeature+0x2e30>
18009db18: 48 83 c4 78                 	add	rsp, 0x78
18009db1c: 41 5e                       	pop	r14
18009db1e: 5f                          	pop	rdi
18009db1f: 5d                          	pop	rbp
18009db20: 5b                          	pop	rbx
18009db21: c3                          	ret
18009db22: 48 8b d3                    	mov	rdx, rbx
18009db25: e8 c6 f8 ff ff              	call	0x18009d3f0 <SetPDFrameWarpNativeCameraSource+0x24b60>
18009db2a: 0f b6 84 24 c8 00 00 00     	movzx	eax, byte ptr [rsp + 0xc8]
18009db32: 44 8b cf                    	mov	r9d, edi
18009db35: 49 8b 56 08                 	mov	rdx, qword ptr [r14 + 0x8]
18009db39: 44 8b c5                    	mov	r8d, ebp
18009db3c: 88 44 24 28                 	mov	byte ptr [rsp + 0x28], al
18009db40: 89 74 24 20                 	mov	dword ptr [rsp + 0x20], esi
18009db44: e8 d7 6d fd ff              	call	0x180074920 <SetPDFrameWarpDiagnosticHud+0x4bed0>
18009db49: 48 89 03                    	mov	qword ptr [rbx], rax
18009db4c: eb b8                       	jmp	0x18009db06 <SetPDFrameWarpNativeCameraSource+0x25276>
18009db4e: 33 c0                       	xor	eax, eax
18009db50: eb b9                       	jmp	0x18009db0b <SetPDFrameWarpNativeCameraSource+0x2527b>
18009db52: cc                          	int3
18009db53: cc                          	int3
18009db54: cc                          	int3
18009db55: cc                          	int3
18009db56: cc                          	int3
18009db57: cc                          	int3
18009db58: cc                          	int3
18009db59: cc                          	int3
18009db5a: cc                          	int3
18009db5b: cc                          	int3
18009db5c: cc                          	int3
18009db5d: cc                          	int3
18009db5e: cc                          	int3
18009db5f: cc                          	int3
18009db60: 48 89 5c 24 08              	mov	qword ptr [rsp + 0x8], rbx
18009db65: 48 89 6c 24 10              	mov	qword ptr [rsp + 0x10], rbp
18009db6a: 48 89 74 24 18              	mov	qword ptr [rsp + 0x18], rsi
18009db6f: 57                          	push	rdi
18009db70: 41 54                       	push	r12
18009db72: 41 56                       	push	r14
18009db74: 48 83 ec 30                 	sub	rsp, 0x30
18009db78: 44 8b 64 24 78              	mov	r12d, dword ptr [rsp + 0x78]
18009db7d: 8b f2                       	mov	esi, edx
18009db7f: 41 8b e9                    	mov	ebp, r9d
18009db82: c6 44 24 28 00              	mov	byte ptr [rsp + 0x28], 0x0
18009db87: 45 8b c8                    	mov	r9d, r8d
18009db8a: 44 89 64 24 20              	mov	dword ptr [rsp + 0x20], r12d
18009db8f: 45 8b f0                    	mov	r14d, r8d
18009db92: 48 8d 91 78 01 00 00        	lea	rdx, [rcx + 0x178]
18009db99: 44 8b c6                    	mov	r8d, esi
18009db9c: 48 8b d9                    	mov	rbx, rcx
18009db9f: e8 ec fe ff ff              	call	0x18009da90 <SetPDFrameWarpNativeCameraSource+0x25200>
18009dba4: 48 85 c0                    	test	rax, rax
18009dba7: 0f 84 d2 00 00 00           	je	0x18009dc7f <SetPDFrameWarpNativeCameraSource+0x253ef>
18009dbad: 80 bc 24 88 00 00 00 00     	cmp	byte ptr [rsp + 0x88], 0x0
18009dbb5: 74 28                       	je	0x18009dbdf <SetPDFrameWarpNativeCameraSource+0x2534f>
18009dbb7: 48 8d 93 80 01 00 00        	lea	rdx, [rbx + 0x180]
18009dbbe: c6 44 24 28 01              	mov	byte ptr [rsp + 0x28], 0x1
18009dbc3: 45 8b ce                    	mov	r9d, r14d
18009dbc6: 44 89 64 24 20              	mov	dword ptr [rsp + 0x20], r12d
18009dbcb: 44 8b c6                    	mov	r8d, esi
18009dbce: 48 8b cb                    	mov	rcx, rbx
18009dbd1: e8 ba fe ff ff              	call	0x18009da90 <SetPDFrameWarpNativeCameraSource+0x25200>
18009dbd6: 48 85 c0                    	test	rax, rax
18009dbd9: 0f 84 a0 00 00 00           	je	0x18009dc7f <SetPDFrameWarpNativeCameraSource+0x253ef>
18009dbdf: 8b 7c 24 70                 	mov	edi, dword ptr [rsp + 0x70]
18009dbe3: 3b ee                       	cmp	ebp, esi
18009dbe5: 75 0f                       	jne	0x18009dbf6 <SetPDFrameWarpNativeCameraSource+0x25366>
18009dbe7: 41 3b fe                    	cmp	edi, r14d
18009dbea: 75 0a                       	jne	0x18009dbf6 <SetPDFrameWarpNativeCameraSource+0x25366>
18009dbec: 83 bc 24 80 00 00 00 02     	cmp	dword ptr [rsp + 0x80], 0x2
18009dbf4: 75 48                       	jne	0x18009dc3e <SetPDFrameWarpNativeCameraSource+0x253ae>
18009dbf6: 48 8d 93 68 01 00 00        	lea	rdx, [rbx + 0x168]
18009dbfd: c6 44 24 28 01              	mov	byte ptr [rsp + 0x28], 0x1
18009dc02: 44 8b cf                    	mov	r9d, edi
18009dc05: 44 89 64 24 20              	mov	dword ptr [rsp + 0x20], r12d
18009dc0a: 44 8b c5                    	mov	r8d, ebp
18009dc0d: 48 8b cb                    	mov	rcx, rbx
18009dc10: e8 7b fe ff ff              	call	0x18009da90 <SetPDFrameWarpNativeCameraSource+0x25200>
18009dc15: 48 85 c0                    	test	rax, rax
18009dc18: 74 65                       	je	0x18009dc7f <SetPDFrameWarpNativeCameraSource+0x253ef>
18009dc1a: 48 8d 93 70 01 00 00        	lea	rdx, [rbx + 0x170]
18009dc21: c6 44 24 28 01              	mov	byte ptr [rsp + 0x28], 0x1
18009dc26: 44 8b cf                    	mov	r9d, edi
18009dc29: 44 89 64 24 20              	mov	dword ptr [rsp + 0x20], r12d
18009dc2e: 44 8b c5                    	mov	r8d, ebp
18009dc31: 48 8b cb                    	mov	rcx, rbx
18009dc34: e8 57 fe ff ff              	call	0x18009da90 <SetPDFrameWarpNativeCameraSource+0x25200>
18009dc39: 48 85 c0                    	test	rax, rax
18009dc3c: 74 41                       	je	0x18009dc7f <SetPDFrameWarpNativeCameraSource+0x253ef>
18009dc3e: 83 bc 24 80 00 00 00 01     	cmp	dword ptr [rsp + 0x80], 0x1
18009dc46: 75 33                       	jne	0x18009dc7b <SetPDFrameWarpNativeCameraSource+0x253eb>
18009dc48: 3b ee                       	cmp	ebp, esi
18009dc4a: 75 05                       	jne	0x18009dc51 <SetPDFrameWarpNativeCameraSource+0x253c1>
18009dc4c: 41 3b fe                    	cmp	edi, r14d
18009dc4f: 74 2a                       	je	0x18009dc7b <SetPDFrameWarpNativeCameraSource+0x253eb>
18009dc51: 48 8d 93 88 01 00 00        	lea	rdx, [rbx + 0x188]
18009dc58: c6 44 24 28 01              	mov	byte ptr [rsp + 0x28], 0x1
18009dc5d: 44 8b cf                    	mov	r9d, edi
18009dc60: c7 44 24 20 0a 00 00 00     	mov	dword ptr [rsp + 0x20], 0xa
18009dc68: 44 8b c6                    	mov	r8d, esi
18009dc6b: 48 8b cb                    	mov	rcx, rbx
18009dc6e: e8 1d fe ff ff              	call	0x18009da90 <SetPDFrameWarpNativeCameraSource+0x25200>
18009dc73: 48 85 c0                    	test	rax, rax
18009dc76: 0f 95 c0                    	setne	al
18009dc79: eb 06                       	jmp	0x18009dc81 <SetPDFrameWarpNativeCameraSource+0x253f1>
18009dc7b: b0 01                       	mov	al, 0x1
18009dc7d: eb 02                       	jmp	0x18009dc81 <SetPDFrameWarpNativeCameraSource+0x253f1>
18009dc7f: 32 c0                       	xor	al, al
18009dc81: 48 8b 5c 24 50              	mov	rbx, qword ptr [rsp + 0x50]
18009dc86: 48 8b 6c 24 58              	mov	rbp, qword ptr [rsp + 0x58]
18009dc8b: 48 8b 74 24 60              	mov	rsi, qword ptr [rsp + 0x60]
18009dc90: 48 83 c4 30                 	add	rsp, 0x30
18009dc94: 41 5e                       	pop	r14
18009dc96: 41 5c                       	pop	r12
18009dc98: 5f                          	pop	rdi
18009dc99: c3                          	ret
18009dc9a: cc                          	int3
18009dc9b: cc                          	int3
18009dc9c: cc                          	int3
18009dc9d: cc                          	int3
18009dc9e: cc                          	int3
18009dc9f: cc                          	int3
