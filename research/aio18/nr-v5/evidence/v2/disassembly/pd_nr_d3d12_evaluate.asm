
/mnt/data/aio18_nr_work/PDPerfPlugin.dll:	file format coff-x86-64

Disassembly of section .text:

0000000180078890 <SetPDFrameWarpNativeCameraSource>:
18009e5a0: 40 53                       	push	rbx
18009e5a2: 55                          	push	rbp
18009e5a3: 57                          	push	rdi
18009e5a4: 41 54                       	push	r12
18009e5a6: 41 55                       	push	r13
18009e5a8: 41 56                       	push	r14
18009e5aa: 41 57                       	push	r15
18009e5ac: 48 81 ec c0 02 00 00        	sub	rsp, 0x2c0
18009e5b3: 48 8b 05 06 c4 12 01        	mov	rax, qword ptr [rip + 0x112c406] # 0x1811ca9c0
18009e5ba: 48 33 c4                    	xor	rax, rsp
18009e5bd: 48 89 84 24 a0 02 00 00     	mov	qword ptr [rsp + 0x2a0], rax
18009e5c5: 48 8b 84 24 48 03 00 00     	mov	rax, qword ptr [rsp + 0x348]
18009e5cd: 49 8b d9                    	mov	rbx, r9
18009e5d0: 48 8b ac 24 28 03 00 00     	mov	rbp, qword ptr [rsp + 0x328]
18009e5d8: 4c 8b f2                    	mov	r14, rdx
18009e5db: 48 8b bc 24 30 03 00 00     	mov	rdi, qword ptr [rsp + 0x330]
18009e5e3: 4c 8b ac 24 38 03 00 00     	mov	r13, qword ptr [rsp + 0x338]
18009e5eb: 4c 8b bc 24 40 03 00 00     	mov	r15, qword ptr [rsp + 0x340]
18009e5f3: 4c 8b a4 24 50 03 00 00     	mov	r12, qword ptr [rsp + 0x350]
18009e5fb: 48 89 44 24 30              	mov	qword ptr [rsp + 0x30], rax
18009e600: 48 8b 84 24 58 03 00 00     	mov	rax, qword ptr [rsp + 0x358]
18009e608: 48 89 44 24 38              	mov	qword ptr [rsp + 0x38], rax
18009e60d: 4c 89 44 24 48              	mov	qword ptr [rsp + 0x48], r8
18009e612: 48 89 4c 24 40              	mov	qword ptr [rsp + 0x40], rcx
18009e617: 48 85 d2                    	test	rdx, rdx
18009e61a: 0f 84 a9 02 00 00           	je	0x18009e8c9 <SetPDFrameWarpNativeCameraSource+0x26039>
18009e620: 4d 85 c0                    	test	r8, r8
18009e623: 0f 84 a0 02 00 00           	je	0x18009e8c9 <SetPDFrameWarpNativeCameraSource+0x26039>
18009e629: 48 85 ed                    	test	rbp, rbp
18009e62c: 0f 84 97 02 00 00           	je	0x18009e8c9 <SetPDFrameWarpNativeCameraSource+0x26039>
18009e632: 48 85 ff                    	test	rdi, rdi
18009e635: 0f 84 8e 02 00 00           	je	0x18009e8c9 <SetPDFrameWarpNativeCameraSource+0x26039>
18009e63b: 48 89 b4 24 b8 02 00 00     	mov	qword ptr [rsp + 0x2b8], rsi
18009e643: 33 f6                       	xor	esi, esi
18009e645: 48 3b ef                    	cmp	rbp, rdi
18009e648: 74 59                       	je	0x18009e6a3 <SetPDFrameWarpNativeCameraSource+0x25e13>
18009e64a: 48 8b d7                    	mov	rdx, rdi
18009e64d: 48 8b cd                    	mov	rcx, rbp
18009e650: e8 cb e8 ff ff              	call	0x18009cf20 <SetPDFrameWarpNativeCameraSource+0x24690>
18009e655: 84 c0                       	test	al, al
18009e657: 74 4a                       	je	0x18009e6a3 <SetPDFrameWarpNativeCameraSource+0x25e13>
18009e659: 48 8b 0d 90 72 17 01        	mov	rcx, qword ptr [rip + 0x1177290] # 0x1812158f0
18009e660: 41 b9 00 08 00 00           	mov	r9d, 0x800
18009e666: 4c 8b c5                    	mov	r8, rbp
18009e669: 89 74 24 20                 	mov	dword ptr [rsp + 0x20], esi
18009e66d: 49 8b d6                    	mov	rdx, r14
18009e670: e8 8b 71 fd ff              	call	0x180075800 <SetPDFrameWarpDiagnosticHud+0x4cdb0>
18009e675: 48 8b 0d 74 72 17 01        	mov	rcx, qword ptr [rip + 0x1177274] # 0x1812158f0
18009e67c: 41 b9 00 04 00 00           	mov	r9d, 0x400
18009e682: 4c 8b c7                    	mov	r8, rdi
18009e685: 89 74 24 20                 	mov	dword ptr [rsp + 0x20], esi
18009e689: 49 8b d6                    	mov	rdx, r14
18009e68c: e8 6f 71 fd ff              	call	0x180075800 <SetPDFrameWarpDiagnosticHud+0x4cdb0>
18009e691: 49 8b 06                    	mov	rax, qword ptr [r14]
18009e694: 4c 8b c5                    	mov	r8, rbp
18009e697: 48 8b d7                    	mov	rdx, rdi
18009e69a: 49 8b ce                    	mov	rcx, r14
18009e69d: ff 90 88 00 00 00           	call	qword ptr [rax + 0x88]
18009e6a3: 48 8b 0d 46 72 17 01        	mov	rcx, qword ptr [rip + 0x1177246] # 0x1812158f0
18009e6aa: 41 b9 40 00 00 00           	mov	r9d, 0x40
18009e6b0: 4c 8b c5                    	mov	r8, rbp
18009e6b3: 89 74 24 20                 	mov	dword ptr [rsp + 0x20], esi
18009e6b7: 49 8b d6                    	mov	rdx, r14
18009e6ba: e8 41 71 fd ff              	call	0x180075800 <SetPDFrameWarpDiagnosticHud+0x4cdb0>
18009e6bf: 48 8b 0d 2a 72 17 01        	mov	rcx, qword ptr [rip + 0x117722a] # 0x1812158f0
18009e6c6: 41 b9 08 00 00 00           	mov	r9d, 0x8
18009e6cc: 4c 8b c7                    	mov	r8, rdi
18009e6cf: 89 74 24 20                 	mov	dword ptr [rsp + 0x20], esi
18009e6d3: 49 8b d6                    	mov	rdx, r14
18009e6d6: e8 25 71 fd ff              	call	0x180075800 <SetPDFrameWarpDiagnosticHud+0x4cdb0>
18009e6db: 4d 85 ed                    	test	r13, r13
18009e6de: 74 1c                       	je	0x18009e6fc <SetPDFrameWarpNativeCameraSource+0x25e6c>
18009e6e0: 48 8b 0d 09 72 17 01        	mov	rcx, qword ptr [rip + 0x1177209] # 0x1812158f0
18009e6e7: 41 b9 40 00 00 00           	mov	r9d, 0x40
18009e6ed: 4d 8b c5                    	mov	r8, r13
18009e6f0: 89 74 24 20                 	mov	dword ptr [rsp + 0x20], esi
18009e6f4: 49 8b d6                    	mov	rdx, r14
18009e6f7: e8 04 71 fd ff              	call	0x180075800 <SetPDFrameWarpDiagnosticHud+0x4cdb0>
18009e6fc: 4d 85 ff                    	test	r15, r15
18009e6ff: 74 1c                       	je	0x18009e71d <SetPDFrameWarpNativeCameraSource+0x25e8d>
18009e701: 48 8b 0d e8 71 17 01        	mov	rcx, qword ptr [rip + 0x11771e8] # 0x1812158f0
18009e708: 41 b9 40 00 00 00           	mov	r9d, 0x40
18009e70e: 4d 8b c7                    	mov	r8, r15
18009e711: 89 74 24 20                 	mov	dword ptr [rsp + 0x20], esi
18009e715: 49 8b d6                    	mov	rdx, r14
18009e718: e8 e3 70 fd ff              	call	0x180075800 <SetPDFrameWarpDiagnosticHud+0x4cdb0>
18009e71d: 48 8d 8c 24 60 01 00 00     	lea	rcx, [rsp + 0x160]
18009e725: b8 02 00 00 00              	mov	eax, 0x2
18009e72a: 66 0f 1f 44 00 00           	nop	word ptr [rax + rax]
18009e730: 48 8d 89 80 00 00 00        	lea	rcx, [rcx + 0x80]
18009e737: 0f 10 03                    	movups	xmm0, xmmword ptr [rbx]
18009e73a: 0f 10 4b 10                 	movups	xmm1, xmmword ptr [rbx + 0x10]
18009e73e: 48 8d 9b 80 00 00 00        	lea	rbx, [rbx + 0x80]
18009e745: 0f 11 41 80                 	movups	xmmword ptr [rcx - 0x80], xmm0
18009e749: 0f 10 43 a0                 	movups	xmm0, xmmword ptr [rbx - 0x60]
18009e74d: 0f 11 49 90                 	movups	xmmword ptr [rcx - 0x70], xmm1
18009e751: 0f 10 4b b0                 	movups	xmm1, xmmword ptr [rbx - 0x50]
18009e755: 0f 11 41 a0                 	movups	xmmword ptr [rcx - 0x60], xmm0
18009e759: 0f 10 43 c0                 	movups	xmm0, xmmword ptr [rbx - 0x40]
18009e75d: 0f 11 49 b0                 	movups	xmmword ptr [rcx - 0x50], xmm1
18009e761: 0f 10 4b d0                 	movups	xmm1, xmmword ptr [rbx - 0x30]
18009e765: 0f 11 41 c0                 	movups	xmmword ptr [rcx - 0x40], xmm0
18009e769: 0f 10 43 e0                 	movups	xmm0, xmmword ptr [rbx - 0x20]
18009e76d: 0f 11 49 d0                 	movups	xmmword ptr [rcx - 0x30], xmm1
18009e771: 0f 10 4b f0                 	movups	xmm1, xmmword ptr [rbx - 0x10]
18009e775: 0f 11 41 e0                 	movups	xmmword ptr [rcx - 0x20], xmm0
18009e779: 0f 11 49 f0                 	movups	xmmword ptr [rcx - 0x10], xmm1
18009e77d: 48 83 e8 01                 	sub	rax, 0x1
18009e781: 75 ad                       	jne	0x18009e730 <SetPDFrameWarpNativeCameraSource+0x25ea0>
18009e783: 0f 10 03                    	movups	xmm0, xmmword ptr [rbx]
18009e786: 48 8b 43 30                 	mov	rax, qword ptr [rbx + 0x30]
18009e78a: 33 d2                       	xor	edx, edx
18009e78c: 0f 10 4b 10                 	movups	xmm1, xmmword ptr [rbx + 0x10]
18009e790: 41 b8 c8 00 00 00           	mov	r8d, 0xc8
18009e796: 0f 11 01                    	movups	xmmword ptr [rcx], xmm0
18009e799: 0f 10 43 20                 	movups	xmm0, xmmword ptr [rbx + 0x20]
18009e79d: 0f 11 49 10                 	movups	xmmword ptr [rcx + 0x10], xmm1
18009e7a1: 0f 11 41 20                 	movups	xmmword ptr [rcx + 0x20], xmm0
18009e7a5: 48 89 41 30                 	mov	qword ptr [rcx + 0x30], rax
18009e7a9: 48 8d 8c 24 98 00 00 00     	lea	rcx, [rsp + 0x98]
18009e7b1: 0f b6 84 24 60 03 00 00     	movzx	eax, byte ptr [rsp + 0x360]
18009e7b9: 88 84 24 60 02 00 00        	mov	byte ptr [rsp + 0x260], al
18009e7c0: e8 55 ee 06 00              	call	0x18010d61a <NVSDK_NGX_UpdateFeature+0x41da>
18009e7c5: 48 8b 44 24 30              	mov	rax, qword ptr [rsp + 0x30]
18009e7ca: 48 89 44 24 70              	mov	qword ptr [rsp + 0x70], rax
18009e7cf: 48 8b 84 24 20 03 00 00     	mov	rax, qword ptr [rsp + 0x320]
18009e7d7: 48 89 6c 24 50              	mov	qword ptr [rsp + 0x50], rbp
18009e7dc: 4c 89 7c 24 58              	mov	qword ptr [rsp + 0x58], r15
18009e7e1: 4c 89 6c 24 60              	mov	qword ptr [rsp + 0x60], r13
18009e7e6: 48 89 7c 24 68              	mov	qword ptr [rsp + 0x68], rdi
18009e7eb: 48 89 74 24 78              	mov	qword ptr [rsp + 0x78], rsi
18009e7f0: 48 89 b4 24 80 00 00 00     	mov	qword ptr [rsp + 0x80], rsi
18009e7f8: 4c 89 a4 24 90 00 00 00     	mov	qword ptr [rsp + 0x90], r12
18009e800: 40 38 70 3d                 	cmp	byte ptr [rax + 0x3d], sil
18009e804: 74 15                       	je	0x18009e81b <SetPDFrameWarpNativeCameraSource+0x25f8b>
18009e806: 8b 70 28                    	mov	esi, dword ptr [rax + 0x28]
18009e809: 8b 48 2c                    	mov	ecx, dword ptr [rax + 0x2c]
18009e80c: f3 0f 10 40 34              	movss	xmm0, dword ptr [rax + 0x34]
18009e811: 48 89 bc 24 88 00 00 00     	mov	qword ptr [rsp + 0x88], rdi
18009e819: eb 21                       	jmp	0x18009e83c <SetPDFrameWarpNativeCameraSource+0x25fac>
18009e81b: 48 8b 4c 24 38              	mov	rcx, qword ptr [rsp + 0x38]
18009e820: 48 8b c7                    	mov	rax, rdi
18009e823: f3 0f 10 05 a5 e2 10 01     	movss	xmm0, dword ptr [rip + 0x110e2a5] # 0x1811acad0
18009e82b: 48 85 c9                    	test	rcx, rcx
18009e82e: 48 0f 45 c1                 	cmovne	rax, rcx
18009e832: 8b ce                       	mov	ecx, esi
18009e834: 48 89 84 24 88 00 00 00     	mov	qword ptr [rsp + 0x88], rax
18009e83c: f3 0f 11 44 24 28           	movss	dword ptr [rsp + 0x28], xmm0
18009e842: 4c 8d 84 24 60 01 00 00     	lea	r8, [rsp + 0x160]
18009e84a: 44 8b ce                    	mov	r9d, esi
18009e84d: 89 4c 24 20                 	mov	dword ptr [rsp + 0x20], ecx
18009e851: 48 8d 54 24 50              	lea	rdx, [rsp + 0x50]
18009e856: e8 55 15 00 00              	call	0x18009fdb0 <SetPDFrameWarpNativeCameraSource+0x27520>
18009e85b: 4c 8b 44 24 40              	mov	r8, qword ptr [rsp + 0x40]
18009e860: 4c 8d 4c 24 50              	lea	r9, [rsp + 0x50]
18009e865: 48 8b 54 24 48              	mov	rdx, qword ptr [rsp + 0x48]
18009e86a: 49 8b ce                    	mov	rcx, r14
18009e86d: 4d 8b 40 20                 	mov	r8, qword ptr [r8 + 0x20]
18009e871: e8 5a cf ff ff              	call	0x18009b7d0 <SetPDFrameWarpNativeCameraSource+0x22f40>
18009e876: 48 8b b4 24 b8 02 00 00     	mov	rsi, qword ptr [rsp + 0x2b8]
18009e87e: 8b c8                       	mov	ecx, eax
18009e880: 81 e1 00 00 f0 ff           	and	ecx, 0xfff00000
18009e886: 8b d8                       	mov	ebx, eax
18009e888: 81 f9 00 00 d0 ba           	cmp	ecx, 0xbad00000
18009e88e: 75 5e                       	jne	0x18009e8ee <SetPDFrameWarpNativeCameraSource+0x2605e>
18009e890: 8b c8                       	mov	ecx, eax
18009e892: e8 29 49 06 00              	call	0x1801031c0 <NVSDK_NGX_D3D12_Shutdown1+0x170>
18009e897: 4c 8b c0                    	mov	r8, rax
18009e89a: 48 8d 0d 2f 5d 10 01        	lea	rcx, [rip + 0x1105d2f]  # 0x1811a45d0    ; STRING: NGX_D3D12_EVALUATE_DLSSNR_EXT failed = 0x%08x, info: %ls
18009e8a1: 8b d3                       	mov	edx, ebx
18009e8a3: e8 98 d2 05 00              	call	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
18009e8a8: 48 8d 0d f9 5c 10 01        	lea	rcx, [rip + 0x1105cf9]  # 0x1811a45a8    ; STRING: NGX_D3D12_EVALUATE_DLSSNR_EXT failed
18009e8af: e8 3c da ff ff              	call	0x18009c2f0 <SetPDFrameWarpNativeCameraSource+0x23a60>
18009e8b4: 48 8b 0d 35 70 17 01        	mov	rcx, qword ptr [rip + 0x1177035] # 0x1812158f0
18009e8bb: 41 b8 08 00 00 00           	mov	r8d, 0x8
18009e8c1: 48 8b d7                    	mov	rdx, rdi
18009e8c4: e8 37 6e fd ff              	call	0x180075700 <SetPDFrameWarpDiagnosticHud+0x4ccb0>
18009e8c9: 32 c0                       	xor	al, al
18009e8cb: 48 8b 8c 24 a0 02 00 00     	mov	rcx, qword ptr [rsp + 0x2a0]
18009e8d3: 48 33 cc                    	xor	rcx, rsp
18009e8d6: e8 95 d9 06 00              	call	0x18010c270 <NVSDK_NGX_UpdateFeature+0x2e30>
18009e8db: 48 81 c4 c0 02 00 00        	add	rsp, 0x2c0
18009e8e2: 41 5f                       	pop	r15
18009e8e4: 41 5e                       	pop	r14
18009e8e6: 41 5d                       	pop	r13
18009e8e8: 41 5c                       	pop	r12
18009e8ea: 5f                          	pop	rdi
18009e8eb: 5d                          	pop	rbp
18009e8ec: 5b                          	pop	rbx
18009e8ed: c3                          	ret
18009e8ee: 48 8b 0d fb 6f 17 01        	mov	rcx, qword ptr [rip + 0x1176ffb] # 0x1812158f0
18009e8f5: 41 b8 08 00 00 00           	mov	r8d, 0x8
18009e8fb: 48 8b d7                    	mov	rdx, rdi
18009e8fe: e8 fd 6d fd ff              	call	0x180075700 <SetPDFrameWarpDiagnosticHud+0x4ccb0>
18009e903: b0 01                       	mov	al, 0x1
18009e905: eb c4                       	jmp	0x18009e8cb <SetPDFrameWarpNativeCameraSource+0x2603b>