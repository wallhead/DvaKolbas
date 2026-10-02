
/mnt/data/aio18_nr_work/PDPerfPlugin.dll:	file format coff-x86-64

Disassembly of section .text:

0000000180078890 <SetPDFrameWarpNativeCameraSource>:
1800a0b10: 48 89 5c 24 18              	mov	qword ptr [rsp + 0x18], rbx
1800a0b15: 57                          	push	rdi
1800a0b16: 48 83 ec 20                 	sub	rsp, 0x20
1800a0b1a: 80 b9 b4 01 00 00 00        	cmp	byte ptr [rcx + 0x1b4], 0x0
1800a0b21: 48 8b fa                    	mov	rdi, rdx
1800a0b24: 48 8b d9                    	mov	rbx, rcx
1800a0b27: 0f 85 d4 01 00 00           	jne	0x1800a0d01 <SetPDFrameWarpNativeCameraSource+0x28471>
1800a0b2d: 48 8b 0d bc 4d 17 01        	mov	rcx, qword ptr [rip + 0x1174dbc] # 0x1812158f0
1800a0b34: 80 b9 27 09 00 00 00        	cmp	byte ptr [rcx + 0x927], 0x0
1800a0b3b: 0f 85 c0 01 00 00           	jne	0x1800a0d01 <SetPDFrameWarpNativeCameraSource+0x28471>
1800a0b41: 80 b9 26 09 00 00 00        	cmp	byte ptr [rcx + 0x926], 0x0
1800a0b48: 0f 85 b3 01 00 00           	jne	0x1800a0d01 <SetPDFrameWarpNativeCameraSource+0x28471>
1800a0b4e: e8 6d f1 fc ff              	call	0x18006fcc0 <SetPDFrameWarpDiagnosticHud+0x47270>
1800a0b53: 84 c0                       	test	al, al
1800a0b55: 0f 85 a6 01 00 00           	jne	0x1800a0d01 <SetPDFrameWarpNativeCameraSource+0x28471>
1800a0b5b: 8b 17                       	mov	edx, dword ptr [rdi]
1800a0b5d: 48 8b cb                    	mov	rcx, rbx
1800a0b60: e8 0b 1a 00 00              	call	0x1800a2570 <SetPDFrameWarpNativeCameraSource+0x29ce0>
1800a0b65: 84 c0                       	test	al, al
1800a0b67: 75 17                       	jne	0x1800a0b80 <SetPDFrameWarpNativeCameraSource+0x282f0>
1800a0b69: 48 8d 0d 68 3e 10 01        	lea	rcx, [rip + 0x1103e68]  # 0x1811a49d8    ; STRING: DLSSNR: Evaluate called before Init for id=%d
1800a0b70: e8 cb af 05 00              	call	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
1800a0b75: 48 8b 5c 24 40              	mov	rbx, qword ptr [rsp + 0x40]
1800a0b7a: 48 83 c4 20                 	add	rsp, 0x20
1800a0b7e: 5f                          	pop	rdi
1800a0b7f: c3                          	ret
1800a0b80: 48 8d 8b b8 01 00 00        	lea	rcx, [rbx + 0x1b8]
1800a0b87: 48 8b c7                    	mov	rax, rdi
1800a0b8a: ba 02 00 00 00              	mov	edx, 0x2
1800a0b8f: 90                          	nop
1800a0b90: 48 8d 89 80 00 00 00        	lea	rcx, [rcx + 0x80]
1800a0b97: 0f 10 00                    	movups	xmm0, xmmword ptr [rax]
1800a0b9a: 0f 10 48 10                 	movups	xmm1, xmmword ptr [rax + 0x10]
1800a0b9e: 48 8d 80 80 00 00 00        	lea	rax, [rax + 0x80]
1800a0ba5: 0f 11 41 80                 	movups	xmmword ptr [rcx - 0x80], xmm0
1800a0ba9: 0f 10 40 a0                 	movups	xmm0, xmmword ptr [rax - 0x60]
1800a0bad: 0f 11 49 90                 	movups	xmmword ptr [rcx - 0x70], xmm1
1800a0bb1: 0f 10 48 b0                 	movups	xmm1, xmmword ptr [rax - 0x50]
1800a0bb5: 0f 11 41 a0                 	movups	xmmword ptr [rcx - 0x60], xmm0
1800a0bb9: 0f 10 40 c0                 	movups	xmm0, xmmword ptr [rax - 0x40]
1800a0bbd: 0f 11 49 b0                 	movups	xmmword ptr [rcx - 0x50], xmm1
1800a0bc1: 0f 10 48 d0                 	movups	xmm1, xmmword ptr [rax - 0x30]
1800a0bc5: 0f 11 41 c0                 	movups	xmmword ptr [rcx - 0x40], xmm0
1800a0bc9: 0f 10 40 e0                 	movups	xmm0, xmmword ptr [rax - 0x20]
1800a0bcd: 0f 11 49 d0                 	movups	xmmword ptr [rcx - 0x30], xmm1
1800a0bd1: 0f 10 48 f0                 	movups	xmm1, xmmword ptr [rax - 0x10]
1800a0bd5: 0f 11 41 e0                 	movups	xmmword ptr [rcx - 0x20], xmm0
1800a0bd9: 0f 11 49 f0                 	movups	xmmword ptr [rcx - 0x10], xmm1
1800a0bdd: 48 83 ea 01                 	sub	rdx, 0x1
1800a0be1: 75 ad                       	jne	0x1800a0b90 <SetPDFrameWarpNativeCameraSource+0x28300>
1800a0be3: 0f 10 00                    	movups	xmm0, xmmword ptr [rax]
1800a0be6: 88 93 b1 01 00 00           	mov	byte ptr [rbx + 0x1b1], dl
1800a0bec: 48 8b d7                    	mov	rdx, rdi
1800a0bef: 0f 10 48 10                 	movups	xmm1, xmmword ptr [rax + 0x10]
1800a0bf3: 0f 11 01                    	movups	xmmword ptr [rcx], xmm0
1800a0bf6: 0f 10 40 20                 	movups	xmm0, xmmword ptr [rax + 0x20]
1800a0bfa: 48 8b 40 30                 	mov	rax, qword ptr [rax + 0x30]
1800a0bfe: 0f 11 49 10                 	movups	xmmword ptr [rcx + 0x10], xmm1
1800a0c02: 0f 11 41 20                 	movups	xmmword ptr [rcx + 0x20], xmm0
1800a0c06: 48 89 41 30                 	mov	qword ptr [rcx + 0x30], rax
1800a0c0a: 48 8b cb                    	mov	rcx, rbx
1800a0c0d: 0f b6 87 10 01 00 00        	movzx	eax, byte ptr [rdi + 0x110]
1800a0c14: 88 83 b2 01 00 00           	mov	byte ptr [rbx + 0x1b2], al
1800a0c1a: e8 a1 c4 ff ff              	call	0x18009d0c0 <SetPDFrameWarpNativeCameraSource+0x24830>
1800a0c1f: 84 c0                       	test	al, al
1800a0c21: 75 19                       	jne	0x1800a0c3c <SetPDFrameWarpNativeCameraSource+0x283ac>
1800a0c23: 8b 17                       	mov	edx, dword ptr [rdi]
1800a0c25: 48 8d 0d 54 3e 10 01        	lea	rcx, [rip + 0x1103e54]  # 0x1811a4a80    ; STRING: DLSSNR: EnsureFeatureMatchesEval failed for id=%d
1800a0c2c: e8 0f af 05 00              	call	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
1800a0c31: 48 8b 5c 24 40              	mov	rbx, qword ptr [rsp + 0x40]
1800a0c36: 48 83 c4 20                 	add	rsp, 0x20
1800a0c3a: 5f                          	pop	rdi
1800a0c3b: c3                          	ret
1800a0c3c: 8b 8f 30 01 00 00           	mov	ecx, dword ptr [rdi + 0x130]
1800a0c42: e8 69 c3 ff ff              	call	0x18009cfb0 <SetPDFrameWarpNativeCameraSource+0x24720>
1800a0c47: 8b d0                       	mov	edx, eax
1800a0c49: 48 8b cb                    	mov	rcx, rbx
1800a0c4c: e8 8f 0d 00 00              	call	0x1800a19e0 <SetPDFrameWarpNativeCameraSource+0x29150>
1800a0c51: 80 3b 00                    	cmp	byte ptr [rbx], 0x0
1800a0c54: 74 0b                       	je	0x1800a0c61 <SetPDFrameWarpNativeCameraSource+0x283d1>
1800a0c56: 48 8b d7                    	mov	rdx, rdi
1800a0c59: 48 8b cb                    	mov	rcx, rbx
1800a0c5c: e8 bf ec ff ff              	call	0x18009f920 <SetPDFrameWarpNativeCameraSource+0x27090>
1800a0c61: 80 bf 10 01 00 00 00        	cmp	byte ptr [rdi + 0x110], 0x0
1800a0c68: 0f b6 03                    	movzx	eax, byte ptr [rbx]
1800a0c6b: 75 42                       	jne	0x1800a0caf <SetPDFrameWarpNativeCameraSource+0x2841f>
1800a0c6d: 48 8b d7                    	mov	rdx, rdi
1800a0c70: 48 8b cb                    	mov	rcx, rbx
1800a0c73: 84 c0                       	test	al, al
1800a0c75: 74 1c                       	je	0x1800a0c93 <SetPDFrameWarpNativeCameraSource+0x28403>
1800a0c77: e8 34 f3 ff ff              	call	0x18009ffb0 <SetPDFrameWarpNativeCameraSource+0x27720>
1800a0c7c: 48 8b 0d 6d 4c 17 01        	mov	rcx, qword ptr [rip + 0x1174c6d] # 0x1812158f0
1800a0c83: e8 08 69 fd ff              	call	0x180077590 <SetPDFrameWarpDiagnosticHud+0x4eb40>
1800a0c88: 48 8b 5c 24 40              	mov	rbx, qword ptr [rsp + 0x40]
1800a0c8d: 48 83 c4 20                 	add	rsp, 0x20
1800a0c91: 5f                          	pop	rdi
1800a0c92: c3                          	ret
1800a0c93: e8 78 fc ff ff              	call	0x1800a0910 <SetPDFrameWarpNativeCameraSource+0x28080>
1800a0c98: 48 8b 0d 51 4c 17 01        	mov	rcx, qword ptr [rip + 0x1174c51] # 0x1812158f0
1800a0c9f: e8 ec 68 fd ff              	call	0x180077590 <SetPDFrameWarpDiagnosticHud+0x4eb40>
1800a0ca4: 48 8b 5c 24 40              	mov	rbx, qword ptr [rsp + 0x40]
1800a0ca9: 48 83 c4 20                 	add	rsp, 0x20
1800a0cad: 5f                          	pop	rdi
1800a0cae: c3                          	ret
1800a0caf: 84 c0                       	test	al, al
1800a0cb1: 74 47                       	je	0x1800a0cfa <SetPDFrameWarpNativeCameraSource+0x2846a>
1800a0cb3: 48 8b 0d 36 4c 17 01        	mov	rcx, qword ptr [rip + 0x1174c36] # 0x1812158f0
1800a0cba: 4c 8b 49 10                 	mov	r9, qword ptr [rcx + 0x10]
1800a0cbe: 4d 85 c9                    	test	r9, r9
1800a0cc1: 74 37                       	je	0x1800a0cfa <SetPDFrameWarpNativeCameraSource+0x2846a>
1800a0cc3: 48 8b 51 30                 	mov	rdx, qword ptr [rcx + 0x30]
1800a0cc7: 48 85 d2                    	test	rdx, rdx
1800a0cca: 74 21                       	je	0x1800a0ced <SetPDFrameWarpNativeCameraSource+0x2845d>
1800a0ccc: 49 8b 01                    	mov	rax, qword ptr [r9]
1800a0ccf: 48 ff 81 18 02 00 00        	inc	qword ptr [rcx + 0x218]
1800a0cd6: 4c 8b 81 18 02 00 00        	mov	r8, qword ptr [rcx + 0x218]
1800a0cdd: 49 8b c9                    	mov	rcx, r9
1800a0ce0: ff 90 98 04 00 00           	call	qword ptr [rax + 0x498]
1800a0ce6: 48 8b 0d 03 4c 17 01        	mov	rcx, qword ptr [rip + 0x1174c03] # 0x1812158f0
1800a0ced: 48 8b 49 10                 	mov	rcx, qword ptr [rcx + 0x10]
1800a0cf1: 48 8b 01                    	mov	rax, qword ptr [rcx]
1800a0cf4: ff 90 78 03 00 00           	call	qword ptr [rax + 0x378]
1800a0cfa: c6 83 b1 01 00 00 01        	mov	byte ptr [rbx + 0x1b1], 0x1
1800a0d01: 48 8b 5c 24 40              	mov	rbx, qword ptr [rsp + 0x40]
1800a0d06: 48 83 c4 20                 	add	rsp, 0x20
1800a0d0a: 5f                          	pop	rdi
1800a0d0b: c3                          	ret