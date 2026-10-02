
/mnt/data/aio18_nr_work/PDPerfPlugin.dll:	file format coff-x86-64

Disassembly of section .text:

0000000180078890 <SetPDFrameWarpNativeCameraSource>:
1800a0910: 48 89 54 24 10              	mov	qword ptr [rsp + 0x10], rdx
1800a0915: 48 89 4c 24 08              	mov	qword ptr [rsp + 0x8], rcx
1800a091a: 53                          	push	rbx
1800a091b: 55                          	push	rbp
1800a091c: 56                          	push	rsi
1800a091d: 57                          	push	rdi
1800a091e: 41 54                       	push	r12
1800a0920: 41 55                       	push	r13
1800a0922: 41 56                       	push	r14
1800a0924: 41 57                       	push	r15
1800a0926: 48 81 ec 98 00 00 00        	sub	rsp, 0x98
1800a092d: 48 8b fa                    	mov	rdi, rdx
1800a0930: e8 bb b5 ff ff              	call	0x18009bef0 <SetPDFrameWarpNativeCameraSource+0x23660>
1800a0935: 84 c0                       	test	al, al
1800a0937: 0f 84 ab 01 00 00           	je	0x1800a0ae8 <SetPDFrameWarpNativeCameraSource+0x28258>
1800a093d: 48 8b 05 ac 4f 17 01        	mov	rax, qword ptr [rip + 0x1174fac] # 0x1812158f0
1800a0944: 48 89 44 24 70              	mov	qword ptr [rsp + 0x70], rax
1800a0949: 48 83 b8 a8 00 00 00 00     	cmp	qword ptr [rax + 0xa8], 0x0
1800a0951: 0f 84 91 01 00 00           	je	0x1800a0ae8 <SetPDFrameWarpNativeCameraSource+0x28258>
1800a0957: 48 83 7f 20 00              	cmp	qword ptr [rdi + 0x20], 0x0
1800a095c: 75 11                       	jne	0x1800a096f <SetPDFrameWarpNativeCameraSource+0x280df>
1800a095e: 48 8d 0d 03 40 10 01        	lea	rcx, [rip + 0x1104003]  # 0x1811a4968    ; STRING: DLSSNR: immediate D3D12 evaluate missing output.
1800a0965: e8 d6 b1 05 00              	call	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
1800a096a: e9 79 01 00 00              	jmp	0x1800a0ae8 <SetPDFrameWarpNativeCameraSource+0x28258>
1800a096f: 48 8b 8f 08 01 00 00        	mov	rcx, qword ptr [rdi + 0x108]
1800a0976: 48 89 8c 24 f0 00 00 00     	mov	qword ptr [rsp + 0xf0], rcx
1800a097e: 48 8d 98 c8 0a 00 00        	lea	rbx, [rax + 0xac8]
1800a0985: 48 89 5c 24 78              	mov	qword ptr [rsp + 0x78], rbx
1800a098a: c6 84 24 80 00 00 00 00     	mov	byte ptr [rsp + 0x80], 0x0
1800a0992: 48 85 c9                    	test	rcx, rcx
1800a0995: 75 5f                       	jne	0x1800a09f6 <SetPDFrameWarpNativeCameraSource+0x28166>
1800a0997: 48 85 db                    	test	rbx, rbx
1800a099a: 0f 84 5c 01 00 00           	je	0x1800a0afc <SetPDFrameWarpNativeCameraSource+0x2826c>
1800a09a0: 48 8b cb                    	mov	rcx, rbx
1800a09a3: ff 15 8f 2b 07 00           	call	qword ptr [rip + 0x72b8f] # 0x180113538
1800a09a9: 85 c0                       	test	eax, eax
1800a09ab: 74 0c                       	je	0x1800a09b9 <SetPDFrameWarpNativeCameraSource+0x28129>
1800a09ad: b9 05 00 00 00              	mov	ecx, 0x5
1800a09b2: ff 15 88 2b 07 00           	call	qword ptr [rip + 0x72b88] # 0x180113540
1800a09b8: cc                          	int3
1800a09b9: 81 7b 4c ff ff ff 7f        	cmp	dword ptr [rbx + 0x4c], 0x7fffffff
1800a09c0: 75 13                       	jne	0x1800a09d5 <SetPDFrameWarpNativeCameraSource+0x28145>
1800a09c2: c7 43 4c fe ff ff 7f        	mov	dword ptr [rbx + 0x4c], 0x7ffffffe
1800a09c9: b9 06 00 00 00              	mov	ecx, 0x6
1800a09ce: ff 15 6c 2b 07 00           	call	qword ptr [rip + 0x72b6c] # 0x180113540
1800a09d4: cc                          	int3
1800a09d5: c6 84 24 80 00 00 00 01     	mov	byte ptr [rsp + 0x80], 0x1
1800a09dd: ba 02 00 00 00              	mov	edx, 0x2
1800a09e2: 48 8b 0d 07 4f 17 01        	mov	rcx, qword ptr [rip + 0x1174f07] # 0x1812158f0
1800a09e9: e8 22 2a fd ff              	call	0x180073410 <SetPDFrameWarpDiagnosticHud+0x4a9c0>
1800a09ee: 48 89 84 24 f0 00 00 00     	mov	qword ptr [rsp + 0xf0], rax
1800a09f6: 48 8b 47 08                 	mov	rax, qword ptr [rdi + 0x8]
1800a09fa: 48 89 84 24 f8 00 00 00     	mov	qword ptr [rsp + 0xf8], rax
1800a0a02: 4c 8b 6f 20                 	mov	r13, qword ptr [rdi + 0x20]
1800a0a06: 4c 8b 67 18                 	mov	r12, qword ptr [rdi + 0x18]
1800a0a0a: 4c 8b 7f 10                 	mov	r15, qword ptr [rdi + 0x10]
1800a0a0e: 4c 8b 77 28                 	mov	r14, qword ptr [rdi + 0x28]
1800a0a12: 48 8b 6f 30                 	mov	rbp, qword ptr [rdi + 0x30]
1800a0a16: 48 8b 77 38                 	mov	rsi, qword ptr [rdi + 0x38]
1800a0a1a: 48 8b 7f 48                 	mov	rdi, qword ptr [rdi + 0x48]
1800a0a1e: 48 8b 84 24 e8 00 00 00     	mov	rax, qword ptr [rsp + 0xe8]
1800a0a26: 48 8b 48 40                 	mov	rcx, qword ptr [rax + 0x40]
1800a0a2a: 49 8b dd                    	mov	rbx, r13
1800a0a2d: 48 85 c9                    	test	rcx, rcx
1800a0a30: 48 0f 45 d9                 	cmovne	rbx, rcx
1800a0a34: 48 8b 8c 24 e0 00 00 00     	mov	rcx, qword ptr [rsp + 0xe0]
1800a0a3c: 48 83 c1 38                 	add	rcx, 0x38
1800a0a40: 48 8b d0                    	mov	rdx, rax
1800a0a43: e8 28 14 ff ff              	call	0x180091e70 <SetPDFrameWarpNativeCameraSource+0x195e0>
1800a0a48: 48 89 5c 24 60              	mov	qword ptr [rsp + 0x60], rbx
1800a0a4d: 48 89 7c 24 58              	mov	qword ptr [rsp + 0x58], rdi
1800a0a52: 48 89 74 24 50              	mov	qword ptr [rsp + 0x50], rsi
1800a0a57: 48 89 6c 24 48              	mov	qword ptr [rsp + 0x48], rbp
1800a0a5c: 4c 89 74 24 40              	mov	qword ptr [rsp + 0x40], r14
1800a0a61: 4c 89 7c 24 38              	mov	qword ptr [rsp + 0x38], r15
1800a0a66: 4c 89 64 24 30              	mov	qword ptr [rsp + 0x30], r12
1800a0a6b: 4c 89 6c 24 28              	mov	qword ptr [rsp + 0x28], r13
1800a0a70: 48 8b 8c 24 f8 00 00 00     	mov	rcx, qword ptr [rsp + 0xf8]
1800a0a78: 48 89 4c 24 20              	mov	qword ptr [rsp + 0x20], rcx
1800a0a7d: 48 8b 9c 24 e8 00 00 00     	mov	rbx, qword ptr [rsp + 0xe8]
1800a0a85: 4c 8b cb                    	mov	r9, rbx
1800a0a88: 4c 8b 00                    	mov	r8, qword ptr [rax]
1800a0a8b: 48 8b 94 24 f0 00 00 00     	mov	rdx, qword ptr [rsp + 0xf0]
1800a0a93: 48 8b 8c 24 e0 00 00 00     	mov	rcx, qword ptr [rsp + 0xe0]
1800a0a9b: e8 80 e6 ff ff              	call	0x18009f120 <SetPDFrameWarpNativeCameraSource+0x26890>
1800a0aa0: 84 c0                       	test	al, al
1800a0aa2: 75 0c                       	jne	0x1800a0ab0 <SetPDFrameWarpNativeCameraSource+0x28220>
1800a0aa4: 48 8d 0d 5d 3f 10 01        	lea	rcx, [rip + 0x1103f5d]  # 0x1811a4a08    ; STRING: NGX_D3D12_EVALUATE_DLSSNR_EXT immediate D3D12 failed
1800a0aab: e8 40 b8 ff ff              	call	0x18009c2f0 <SetPDFrameWarpNativeCameraSource+0x23a60>
1800a0ab0: 48 83 bb 08 01 00 00 00     	cmp	qword ptr [rbx + 0x108], 0x0
1800a0ab8: 75 12                       	jne	0x1800a0acc <SetPDFrameWarpNativeCameraSource+0x2823c>
1800a0aba: ba 02 00 00 00              	mov	edx, 0x2
1800a0abf: 48 8b 0d 2a 4e 17 01        	mov	rcx, qword ptr [rip + 0x1174e2a] # 0x1812158f0
1800a0ac6: e8 95 2b fd ff              	call	0x180073660 <SetPDFrameWarpDiagnosticHud+0x4ac10>
1800a0acb: 90                          	nop
1800a0acc: 80 bc 24 80 00 00 00 00     	cmp	byte ptr [rsp + 0x80], 0x0
1800a0ad4: 74 12                       	je	0x1800a0ae8 <SetPDFrameWarpNativeCameraSource+0x28258>
1800a0ad6: 48 8b 4c 24 70              	mov	rcx, qword ptr [rsp + 0x70]
1800a0adb: 48 81 c1 c8 0a 00 00        	add	rcx, 0xac8
1800a0ae2: ff 15 40 2a 07 00           	call	qword ptr [rip + 0x72a40] # 0x180113528
1800a0ae8: 48 81 c4 98 00 00 00        	add	rsp, 0x98
1800a0aef: 41 5f                       	pop	r15
1800a0af1: 41 5e                       	pop	r14
1800a0af3: 41 5d                       	pop	r13
1800a0af5: 41 5c                       	pop	r12
1800a0af7: 5f                          	pop	rdi
1800a0af8: 5e                          	pop	rsi
1800a0af9: 5d                          	pop	rbp
1800a0afa: 5b                          	pop	rbx
1800a0afb: c3                          	ret
1800a0afc: b9 01 00 00 00              	mov	ecx, 0x1
1800a0b01: e8 ca c4 fc ff              	call	0x18006cfd0 <SetPDFrameWarpDiagnosticHud+0x44580>
1800a0b06: cc                          	int3