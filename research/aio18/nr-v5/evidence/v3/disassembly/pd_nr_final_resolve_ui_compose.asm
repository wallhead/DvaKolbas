
/mnt/data/aio18_nr_work/PDPerfPlugin.dll:	file format coff-x86-64

Disassembly of section .text:

0000000180078890 <SetPDFrameWarpNativeCameraSource>:
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
18009f5b4: 48 8d 0d 2d 51 10 01        	lea	rcx, [rip + 0x110512d]  # 0x1811a46e8
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
18009f5f1: 48 8d 0d a8 51 10 01        	lea	rcx, [rip + 0x11051a8]  # 0x1811a47a0
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
18009f62a: 48 8d 0d 2f 51 10 01        	lea	rcx, [rip + 0x110512f]  # 0x1811a4760
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
18009f672: 48 8d 0d 7f 51 10 01        	lea	rcx, [rip + 0x110517f]  # 0x1811a47f8
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
