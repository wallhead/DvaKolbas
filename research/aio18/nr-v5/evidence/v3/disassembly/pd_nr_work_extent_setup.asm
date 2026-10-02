
/mnt/data/aio18_nr_work/PDPerfPlugin.dll:	file format coff-x86-64

Disassembly of section .text:

0000000180078890 <SetPDFrameWarpNativeCameraSource>:
18009d0c0: 40 53                       	push	rbx
18009d0c2: 57                          	push	rdi
18009d0c3: 48 81 ec 88 00 00 00        	sub	rsp, 0x88
18009d0ca: f3 0f 10 82 18 01 00 00     	movss	xmm0, dword ptr [rdx + 0x118]
18009d0d2: 48 8b fa                    	mov	rdi, rdx
18009d0d5: 0f 29 7c 24 60              	movaps	xmmword ptr [rsp + 0x60], xmm7
18009d0da: 48 8b d9                    	mov	rbx, rcx
18009d0dd: e8 de fe ff ff              	call	0x18009cfc0 <SetPDFrameWarpNativeCameraSource+0x24730>
18009d0e2: 33 c0                       	xor	eax, eax
18009d0e4: 4c 8d 8c 24 b0 00 00 00     	lea	r9, [rsp + 0xb0]
18009d0ec: 89 84 24 b0 00 00 00        	mov	dword ptr [rsp + 0xb0], eax
18009d0f3: 45 33 c0                    	xor	r8d, r8d
18009d0f6: 89 84 24 a8 00 00 00        	mov	dword ptr [rsp + 0xa8], eax
18009d0fd: 0f 28 f8                    	movaps	xmm7, xmm0
18009d100: 48 8d 84 24 a8 00 00 00     	lea	rax, [rsp + 0xa8]
18009d108: 48 89 44 24 20              	mov	qword ptr [rsp + 0x20], rax
18009d10d: e8 fe fe ff ff              	call	0x18009d010 <SetPDFrameWarpNativeCameraSource+0x24780>
18009d112: 8b 84 24 b0 00 00 00        	mov	eax, dword ptr [rsp + 0xb0]
18009d119: 85 c0                       	test	eax, eax
18009d11b: 0f 8e 9e 01 00 00           	jle	0x18009d2bf <SetPDFrameWarpNativeCameraSource+0x24a2f>
18009d121: 83 bc 24 a8 00 00 00 00     	cmp	dword ptr [rsp + 0xa8], 0x0
18009d129: 0f 8e 90 01 00 00           	jle	0x18009d2bf <SetPDFrameWarpNativeCameraSource+0x24a2f>
18009d12f: 48 89 ac 24 a0 00 00 00     	mov	qword ptr [rsp + 0xa0], rbp
18009d137: 66 0f 6e c0                 	movd	xmm0, eax
18009d13b: f3 0f e6 c0                 	cvtdq2pd	xmm0, xmm0
18009d13f: 48 89 b4 24 80 00 00 00     	mov	qword ptr [rsp + 0x80], rsi
18009d147: 0f 29 74 24 70              	movaps	xmmword ptr [rsp + 0x70], xmm6
18009d14c: 0f 57 f6                    	xorps	xmm6, xmm6
18009d14f: f3 0f 5a f7                 	cvtss2sd	xmm6, xmm7
18009d153: f2 0f 59 c6                 	mulsd	xmm0, xmm6
18009d157: ff 15 fb 65 07 00           	call	qword ptr [rip + 0x765fb] # 0x180113758
18009d15d: 66 0f 6e 84 24 a8 00 00 00  	movd	xmm0, dword ptr [rsp + 0xa8]
18009d166: be 01 00 00 00              	mov	esi, 0x1
18009d16b: f3 0f e6 c0                 	cvtdq2pd	xmm0, xmm0
18009d16f: 3b c6                       	cmp	eax, esi
18009d171: 8b ee                       	mov	ebp, esi
18009d173: 0f 4f e8                    	cmovg	ebp, eax
18009d176: f2 0f 59 c6                 	mulsd	xmm0, xmm6
18009d17a: ff 15 d8 65 07 00           	call	qword ptr [rip + 0x765d8] # 0x180113758
18009d180: f3 0f 10 15 48 f9 10 01     	movss	xmm2, dword ptr [rip + 0x110f948] # 0x1811acad0
18009d188: 3b c6                       	cmp	eax, esi
18009d18a: 0f 4f f0                    	cmovg	esi, eax
18009d18d: 0f b6 83 14 01 00 00        	movzx	eax, byte ptr [rbx + 0x114]
18009d194: 0f 2f d7                    	comiss	xmm2, xmm7
18009d197: 0f 86 c7 00 00 00           	jbe	0x18009d264 <SetPDFrameWarpNativeCameraSource+0x249d4>
18009d19d: 84 c0                       	test	al, al
18009d19f: 74 22                       	je	0x18009d1c3 <SetPDFrameWarpNativeCameraSource+0x24933>
18009d1a1: 39 ab 0c 01 00 00           	cmp	dword ptr [rbx + 0x10c], ebp
18009d1a7: 75 1a                       	jne	0x18009d1c3 <SetPDFrameWarpNativeCameraSource+0x24933>
18009d1a9: 39 b3 10 01 00 00           	cmp	dword ptr [rbx + 0x110], esi
18009d1af: 75 12                       	jne	0x18009d1c3 <SetPDFrameWarpNativeCameraSource+0x24933>
18009d1b1: 8b 17                       	mov	edx, dword ptr [rdi]
18009d1b3: 48 8b cb                    	mov	rcx, rbx
18009d1b6: e8 b5 53 00 00              	call	0x1800a2570 <SetPDFrameWarpNativeCameraSource+0x29ce0>
18009d1bb: 84 c0                       	test	al, al
18009d1bd: 0f 85 9d 00 00 00           	jne	0x18009d260 <SetPDFrameWarpNativeCameraSource+0x249d0>
18009d1c3: 8b 83 fc 00 00 00           	mov	eax, dword ptr [rbx + 0xfc]
18009d1c9: 48 8d 54 24 30              	lea	rdx, [rsp + 0x30]
18009d1ce: 0f 10 83 dc 00 00 00        	movups	xmm0, xmmword ptr [rbx + 0xdc]
18009d1d5: 89 44 24 50                 	mov	dword ptr [rsp + 0x50], eax
18009d1d9: 44 8b ce                    	mov	r9d, esi
18009d1dc: 0f 10 8b ec 00 00 00        	movups	xmm1, xmmword ptr [rbx + 0xec]
18009d1e3: 8b 07                       	mov	eax, dword ptr [rdi]
18009d1e5: 44 8b c5                    	mov	r8d, ebp
18009d1e8: 0f 11 44 24 30              	movups	xmmword ptr [rsp + 0x30], xmm0
18009d1ed: 48 8b cb                    	mov	rcx, rbx
18009d1f0: 89 44 24 30                 	mov	dword ptr [rsp + 0x30], eax
18009d1f4: f3 0f 11 54 24 20           	movss	dword ptr [rsp + 0x20], xmm2
18009d1fa: 0f 11 4c 24 40              	movups	xmmword ptr [rsp + 0x40], xmm1
18009d1ff: e8 6c f6 ff ff              	call	0x18009c870 <SetPDFrameWarpNativeCameraSource+0x23fe0>
18009d204: 84 c0                       	test	al, al
18009d206: 75 37                       	jne	0x18009d23f <SetPDFrameWarpNativeCameraSource+0x249af>
18009d208: 44 8b c6                    	mov	r8d, esi
18009d20b: 48 8d 0d 66 71 10 01        	lea	rcx, [rip + 0x1107166]  # 0x1811a4378
18009d212: 8b d5                       	mov	edx, ebp
18009d214: e8 27 e9 05 00              	call	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
18009d219: 32 c0                       	xor	al, al
18009d21b: 48 8b b4 24 80 00 00 00     	mov	rsi, qword ptr [rsp + 0x80]
18009d223: 48 8b ac 24 a0 00 00 00     	mov	rbp, qword ptr [rsp + 0xa0]
18009d22b: 0f 28 74 24 70              	movaps	xmm6, xmmword ptr [rsp + 0x70]
18009d230: 0f 28 7c 24 60              	movaps	xmm7, xmmword ptr [rsp + 0x60]
18009d235: 48 81 c4 88 00 00 00        	add	rsp, 0x88
18009d23c: 5f                          	pop	rdi
18009d23d: 5b                          	pop	rbx
18009d23e: c3                          	ret
18009d23f: 44 8b ce                    	mov	r9d, esi
18009d242: c6 83 14 01 00 00 01        	mov	byte ptr [rbx + 0x114], 0x1
18009d249: 44 8b c5                    	mov	r8d, ebp
18009d24c: 48 8d 0d ed 70 10 01        	lea	rcx, [rip + 0x11070ed]  # 0x1811a4340
18009d253: 0f 28 ce                    	movaps	xmm1, xmm6
18009d256: 66 48 0f 7e f2              	movq	rdx, xmm6
18009d25b: e8 e0 e8 05 00              	call	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
18009d260: b0 01                       	mov	al, 0x1
18009d262: eb b7                       	jmp	0x18009d21b <SetPDFrameWarpNativeCameraSource+0x2498b>
18009d264: 84 c0                       	test	al, al
18009d266: 75 2a                       	jne	0x18009d292 <SetPDFrameWarpNativeCameraSource+0x24a02>
18009d268: 8b 17                       	mov	edx, dword ptr [rdi]
18009d26a: 48 8b cb                    	mov	rcx, rbx
18009d26d: e8 fe 52 00 00              	call	0x1800a2570 <SetPDFrameWarpNativeCameraSource+0x29ce0>
18009d272: 84 c0                       	test	al, al
18009d274: 74 1c                       	je	0x18009d292 <SetPDFrameWarpNativeCameraSource+0x24a02>
18009d276: 8b 83 e0 00 00 00           	mov	eax, dword ptr [rbx + 0xe0]
18009d27c: 39 83 0c 01 00 00           	cmp	dword ptr [rbx + 0x10c], eax
18009d282: 75 0e                       	jne	0x18009d292 <SetPDFrameWarpNativeCameraSource+0x24a02>
18009d284: 8b 83 e4 00 00 00           	mov	eax, dword ptr [rbx + 0xe4]
18009d28a: 39 83 10 01 00 00           	cmp	dword ptr [rbx + 0x110], eax
18009d290: 74 1e                       	je	0x18009d2b0 <SetPDFrameWarpNativeCameraSource+0x24a20>
18009d292: 48 8d 93 dc 00 00 00        	lea	rdx, [rbx + 0xdc]
18009d299: 48 8b cb                    	mov	rcx, rbx
18009d29c: e8 af f0 ff ff              	call	0x18009c350 <SetPDFrameWarpNativeCameraSource+0x23ac0>
18009d2a1: 84 c0                       	test	al, al
18009d2a3: 0f 84 70 ff ff ff           	je	0x18009d219 <SetPDFrameWarpNativeCameraSource+0x24989>
18009d2a9: c6 83 14 01 00 00 00        	mov	byte ptr [rbx + 0x114], 0x0
18009d2b0: 8b 17                       	mov	edx, dword ptr [rdi]
18009d2b2: 48 8b cb                    	mov	rcx, rbx
18009d2b5: e8 b6 52 00 00              	call	0x1800a2570 <SetPDFrameWarpNativeCameraSource+0x29ce0>
18009d2ba: e9 5c ff ff ff              	jmp	0x18009d21b <SetPDFrameWarpNativeCameraSource+0x2498b>
18009d2bf: 8b 17                       	mov	edx, dword ptr [rdi]
18009d2c1: 48 8b cb                    	mov	rcx, rbx
18009d2c4: 0f 28 7c 24 60              	movaps	xmm7, xmmword ptr [rsp + 0x60]
18009d2c9: 48 81 c4 88 00 00 00        	add	rsp, 0x88
