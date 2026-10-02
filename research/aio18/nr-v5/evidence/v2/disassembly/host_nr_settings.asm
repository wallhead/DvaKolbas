
/mnt/data/aio18_nr_work/SkyrimUpscaler.dll:	file format coff-x86-64

Disassembly of section .text:

0000000180001000 <.text>:
1802a3800: 05 02 00 cc e8              	add	eax, 0xe8cc0002
1802a3805: 73 5a                       	jae	0x1802a3861 <.text+0x2a2861>
1802a3807: f9                          	stc
1802a3808: ff c5                       	inc	ebp
1802a380a: fa                          	cli
1802a380b: 6f                          	outsd	dx, dword ptr [rsi]
1802a380c: 05 7f 8c 16 00              	add	eax, 0x168c7f
1802a3811: c5 fa 7f 44 24 50           	vmovdqu	xmmword ptr [rsp + 0x50], xmm0
1802a3817: c6 44 24 40 00              	mov	byte ptr [rsp + 0x40], 0x0
1802a381c: 40 f6 c7 10                 	test	dil, 0x10
1802a3820: 74 17                       	je	0x1802a3839 <.text+0x2a2839>
1802a3822: 4c 8b 45 a8                 	mov	r8, qword ptr [rbp - 0x58]
1802a3826: 49 83 f8 0f                 	cmp	r8, 0xf
1802a382a: 76 0d                       	jbe	0x1802a3839 <.text+0x2a2839>
1802a382c: 48 8b 55 90                 	mov	rdx, qword ptr [rbp - 0x70]
1802a3830: 48 8d 4d 90                 	lea	rcx, [rbp - 0x70]
1802a3834: e8 f7 93 e9 ff              	call	0x18013cc30 <.text+0x13bc30>
1802a3839: 48 83 7e 18 0f              	cmp	qword ptr [rsi + 0x18], 0xf
1802a383e: 76 03                       	jbe	0x1802a3843 <.text+0x2a2843>
1802a3840: 48 8b 36                    	mov	rsi, qword ptr [rsi]
1802a3843: c6 44 24 30 01              	mov	byte ptr [rsp + 0x30], 0x1
1802a3848: 4c 89 64 24 20              	mov	qword ptr [rsp + 0x20], r12
1802a384d: 4c 8b ce                    	mov	r9, rsi
1802a3850: 4c 8d 05 89 2d 16 00        	lea	r8, [rip + 0x162d89]    # 0x1804065e0 ; STRING: mDLSSNRAdapterName
1802a3857: 48 8d 15 5a e2 15 00        	lea	rdx, [rip + 0x15e25a]   # 0x180401ab8 ; STRING: DLSS NR
1802a385e: 48 8d 4d d0                 	lea	rcx, [rbp - 0x30]
1802a3862: e8 39 29 eb ff              	call	0x1801561a0 <.text+0x1551a0>
1802a3867: 49 8b d7                    	mov	rdx, r15
1802a386a: 41 80 be aa 02 00 00 00     	cmp	byte ptr [r14 + 0x2aa], 0x0
1802a3872: 48 8d 3d 37 df 13 00        	lea	rdi, [rip + 0x13df37]   # 0x1803e17b0 ; STRING: true
1802a3879: 48 0f 45 d7                 	cmovne	rdx, rdi
1802a387d: 48 8b c3                    	mov	rax, rbx
1802a3880: 48 8d 40 01                 	lea	rax, [rax + 0x1]
1802a3884: 80 3c 02 00                 	cmp	byte ptr [rdx + rax], 0x0
1802a3888: 75 f6                       	jne	0x1802a3880 <.text+0x2a2880>
1802a388a: 4c 8d 40 01                 	lea	r8, [rax + 0x1]
1802a388e: 49 83 f8 40                 	cmp	r8, 0x40
1802a3892: 77 0a                       	ja	0x1802a389e <.text+0x2a289e>
1802a3894: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a3899: e8 88 6f f9 ff              	call	0x18023a826 <.text+0x239826>
1802a389e: c6 44 24 30 01              	mov	byte ptr [rsp + 0x30], 0x1
1802a38a3: 4c 89 64 24 20              	mov	qword ptr [rsp + 0x20], r12
1802a38a8: 4c 8d 4c 24 40              	lea	r9, [rsp + 0x40]
1802a38ad: 4c 8d 05 14 2d 16 00        	lea	r8, [rip + 0x162d14]    # 0x1804065c8 ; STRING: mDLSSNRBeforeUpscaling
1802a38b4: 48 8d 15 fd e1 15 00        	lea	rdx, [rip + 0x15e1fd]   # 0x180401ab8 ; STRING: DLSS NR
1802a38bb: 48 8d 4d d0                 	lea	rcx, [rbp - 0x30]
1802a38bf: e8 dc 28 eb ff              	call	0x1801561a0 <.text+0x1551a0>
1802a38c4: 45 8b 86 ac 02 00 00        	mov	r8d, dword ptr [r14 + 0x2ac]
1802a38cb: 48 8d 15 fa 3d 16 00        	lea	rdx, [rip + 0x163dfa]   # 0x1804076cc ; STRING: %ld
1802a38d2: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a38d7: e8 a4 d1 ee ff              	call	0x180190a80 <.text+0x18fa80>
1802a38dc: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a38e1: 48 8b c3                    	mov	rax, rbx
1802a38e4: 48 8d 40 01                 	lea	rax, [rax + 0x1]
1802a38e8: 80 3c 01 00                 	cmp	byte ptr [rcx + rax], 0x0
1802a38ec: 75 f6                       	jne	0x1802a38e4 <.text+0x2a28e4>
1802a38ee: 4c 8d 40 01                 	lea	r8, [rax + 0x1]
1802a38f2: 49 83 f8 40                 	cmp	r8, 0x40
1802a38f6: 77 0e                       	ja	0x1802a3906 <.text+0x2a2906>
1802a38f8: 48 8d 54 24 40              	lea	rdx, [rsp + 0x40]
1802a38fd: 48 8d 4d 90                 	lea	rcx, [rbp - 0x70]
1802a3901: e8 20 6f f9 ff              	call	0x18023a826 <.text+0x239826>
1802a3906: c6 44 24 30 01              	mov	byte ptr [rsp + 0x30], 0x1
1802a390b: 4c 89 64 24 20              	mov	qword ptr [rsp + 0x20], r12
1802a3910: 4c 8d 4d 90                 	lea	r9, [rbp - 0x70]
1802a3914: 4c 8d 05 ed 2c 16 00        	lea	r8, [rip + 0x162ced]    # 0x180406608 ; STRING: mDLSSNRPreset
1802a391b: 48 8d 15 96 e1 15 00        	lea	rdx, [rip + 0x15e196]   # 0x180401ab8 ; STRING: DLSS NR
1802a3922: 48 8d 4d d0                 	lea	rcx, [rbp - 0x30]
1802a3926: e8 75 28 eb ff              	call	0x1801561a0 <.text+0x1551a0>
1802a392b: 45 8b 86 b0 02 00 00        	mov	r8d, dword ptr [r14 + 0x2b0]
1802a3932: 48 8d 15 93 3d 16 00        	lea	rdx, [rip + 0x163d93]   # 0x1804076cc ; STRING: %ld
1802a3939: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a393e: e8 3d d1 ee ff              	call	0x180190a80 <.text+0x18fa80>
1802a3943: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a3948: 48 8b c3                    	mov	rax, rbx
1802a394b: 0f 1f 44 00 00              	nop	dword ptr [rax + rax]
1802a3950: 48 8d 40 01                 	lea	rax, [rax + 0x1]
1802a3954: 80 3c 01 00                 	cmp	byte ptr [rcx + rax], 0x0
1802a3958: 75 f6                       	jne	0x1802a3950 <.text+0x2a2950>
1802a395a: 4c 8d 40 01                 	lea	r8, [rax + 0x1]
1802a395e: 49 83 f8 40                 	cmp	r8, 0x40
1802a3962: 77 0e                       	ja	0x1802a3972 <.text+0x2a2972>
1802a3964: 48 8d 54 24 40              	lea	rdx, [rsp + 0x40]
1802a3969: 48 8d 4d 90                 	lea	rcx, [rbp - 0x70]
1802a396d: e8 b4 6e f9 ff              	call	0x18023a826 <.text+0x239826>
1802a3972: c6 44 24 30 01              	mov	byte ptr [rsp + 0x30], 0x1
1802a3977: 4c 89 64 24 20              	mov	qword ptr [rsp + 0x20], r12
1802a397c: 4c 8d 4d 90                 	lea	r9, [rbp - 0x70]
1802a3980: 4c 8d 05 71 2c 16 00        	lea	r8, [rip + 0x162c71]    # 0x1804065f8 ; STRING: mDLSSNRStyle
1802a3987: 48 8d 15 2a e1 15 00        	lea	rdx, [rip + 0x15e12a]   # 0x180401ab8 ; STRING: DLSS NR
1802a398e: 48 8d 4d d0                 	lea	rcx, [rbp - 0x30]
1802a3992: e8 09 28 eb ff              	call	0x1801561a0 <.text+0x1551a0>
1802a3997: c4 c1 7a 10 96 b4 02 00 00  	vmovss	xmm2, dword ptr [r14 + 0x2b4]
1802a39a0: c5 ea 5a d2                 	vcvtss2sd	xmm2, xmm2, xmm2
1802a39a4: c4 c1 f9 7e d0              	vmovq	r8, xmm2
1802a39a9: 48 8d 15 b8 41 16 00        	lea	rdx, [rip + 0x1641b8]   # 0x180407b68
1802a39b0: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a39b5: e8 c6 d0 ee ff              	call	0x180190a80 <.text+0x18fa80>
1802a39ba: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a39bf: 48 8b c3                    	mov	rax, rbx
1802a39c2: 48 8d 40 01                 	lea	rax, [rax + 0x1]
1802a39c6: 80 3c 01 00                 	cmp	byte ptr [rcx + rax], 0x0
1802a39ca: 75 f6                       	jne	0x1802a39c2 <.text+0x2a29c2>
1802a39cc: 4c 8d 40 01                 	lea	r8, [rax + 0x1]
1802a39d0: 49 83 f8 40                 	cmp	r8, 0x40
1802a39d4: 77 0e                       	ja	0x1802a39e4 <.text+0x2a29e4>
1802a39d6: 48 8d 54 24 40              	lea	rdx, [rsp + 0x40]
1802a39db: 48 8d 4d 90                 	lea	rcx, [rbp - 0x70]
1802a39df: e8 42 6e f9 ff              	call	0x18023a826 <.text+0x239826>
1802a39e4: c6 44 24 30 01              	mov	byte ptr [rsp + 0x30], 0x1
1802a39e9: 4c 89 64 24 20              	mov	qword ptr [rsp + 0x20], r12
1802a39ee: 4c 8d 4d 90                 	lea	r9, [rbp - 0x70]
1802a39f2: 4c 8d 05 3f 2c 16 00        	lea	r8, [rip + 0x162c3f]    # 0x180406638 ; STRING: mDLSSNRIntensity
1802a39f9: 48 8d 15 b8 e0 15 00        	lea	rdx, [rip + 0x15e0b8]   # 0x180401ab8 ; STRING: DLSS NR
1802a3a00: 48 8d 4d d0                 	lea	rcx, [rbp - 0x30]
1802a3a04: e8 97 27 eb ff              	call	0x1801561a0 <.text+0x1551a0>
1802a3a09: c4 c1 7a 10 96 b8 02 00 00  	vmovss	xmm2, dword ptr [r14 + 0x2b8]
1802a3a12: c5 ea 5a d2                 	vcvtss2sd	xmm2, xmm2, xmm2
1802a3a16: c4 c1 f9 7e d0              	vmovq	r8, xmm2
1802a3a1b: 48 8d 15 46 41 16 00        	lea	rdx, [rip + 0x164146]   # 0x180407b68
1802a3a22: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a3a27: e8 54 d0 ee ff              	call	0x180190a80 <.text+0x18fa80>
1802a3a2c: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a3a31: 48 8b c3                    	mov	rax, rbx
1802a3a34: 48 8d 40 01                 	lea	rax, [rax + 0x1]
1802a3a38: 80 3c 01 00                 	cmp	byte ptr [rcx + rax], 0x0
1802a3a3c: 75 f6                       	jne	0x1802a3a34 <.text+0x2a2a34>
1802a3a3e: 4c 8d 40 01                 	lea	r8, [rax + 0x1]
1802a3a42: 49 83 f8 40                 	cmp	r8, 0x40
1802a3a46: 77 0e                       	ja	0x1802a3a56 <.text+0x2a2a56>
1802a3a48: 48 8d 54 24 40              	lea	rdx, [rsp + 0x40]
1802a3a4d: 48 8d 4d 90                 	lea	rcx, [rbp - 0x70]
1802a3a51: e8 d0 6d f9 ff              	call	0x18023a826 <.text+0x239826>
1802a3a56: c6 44 24 30 01              	mov	byte ptr [rsp + 0x30], 0x1
1802a3a5b: 4c 89 64 24 20              	mov	qword ptr [rsp + 0x20], r12
1802a3a60: 4c 8d 4d 90                 	lea	r9, [rbp - 0x70]
1802a3a64: 4c 8d 05 ad 2b 16 00        	lea	r8, [rip + 0x162bad]    # 0x180406618 ; STRING: mDLSSNRLocalToneStrength
1802a3a6b: 48 8d 15 46 e0 15 00        	lea	rdx, [rip + 0x15e046]   # 0x180401ab8 ; STRING: DLSS NR
1802a3a72: 48 8d 4d d0                 	lea	rcx, [rbp - 0x30]
1802a3a76: e8 25 27 eb ff              	call	0x1801561a0 <.text+0x1551a0>
1802a3a7b: c4 c1 7a 10 96 bc 02 00 00  	vmovss	xmm2, dword ptr [r14 + 0x2bc]
1802a3a84: c5 ea 5a d2                 	vcvtss2sd	xmm2, xmm2, xmm2
1802a3a88: c4 c1 f9 7e d0              	vmovq	r8, xmm2
1802a3a8d: 48 8d 15 d4 40 16 00        	lea	rdx, [rip + 0x1640d4]   # 0x180407b68
1802a3a94: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a3a99: e8 e2 cf ee ff              	call	0x180190a80 <.text+0x18fa80>
1802a3a9e: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a3aa3: 48 8b c3                    	mov	rax, rbx
1802a3aa6: 48 8d 40 01                 	lea	rax, [rax + 0x1]
1802a3aaa: 80 3c 01 00                 	cmp	byte ptr [rcx + rax], 0x0
1802a3aae: 75 f6                       	jne	0x1802a3aa6 <.text+0x2a2aa6>
1802a3ab0: 4c 8d 40 01                 	lea	r8, [rax + 0x1]
1802a3ab4: 49 83 f8 40                 	cmp	r8, 0x40
1802a3ab8: 77 0e                       	ja	0x1802a3ac8 <.text+0x2a2ac8>
1802a3aba: 48 8d 54 24 40              	lea	rdx, [rsp + 0x40]
1802a3abf: 48 8d 4d 90                 	lea	rcx, [rbp - 0x70]
1802a3ac3: e8 5e 6d f9 ff              	call	0x18023a826 <.text+0x239826>
1802a3ac8: c6 44 24 30 01              	mov	byte ptr [rsp + 0x30], 0x1
1802a3acd: 4c 89 64 24 20              	mov	qword ptr [rsp + 0x20], r12
1802a3ad2: 4c 8d 4d 90                 	lea	r9, [rbp - 0x70]
1802a3ad6: 4c 8d 05 93 2b 16 00        	lea	r8, [rip + 0x162b93]    # 0x180406670 ; STRING: mDLSSNRLocalStructureStrength
1802a3add: 48 8d 15 d4 df 15 00        	lea	rdx, [rip + 0x15dfd4]   # 0x180401ab8 ; STRING: DLSS NR
1802a3ae4: 48 8d 4d d0                 	lea	rcx, [rbp - 0x30]
1802a3ae8: e8 b3 26 eb ff              	call	0x1801561a0 <.text+0x1551a0>
1802a3aed: c4 c1 7a 10 96 c0 02 00 00  	vmovss	xmm2, dword ptr [r14 + 0x2c0]
1802a3af6: c5 ea 5a d2                 	vcvtss2sd	xmm2, xmm2, xmm2
1802a3afa: c4 c1 f9 7e d0              	vmovq	r8, xmm2
1802a3aff: 48 8d 15 62 40 16 00        	lea	rdx, [rip + 0x164062]   # 0x180407b68
1802a3b06: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a3b0b: e8 70 cf ee ff              	call	0x180190a80 <.text+0x18fa80>
1802a3b10: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a3b15: 48 8b c3                    	mov	rax, rbx
1802a3b18: 0f 1f 84 00 00 00 00 00     	nop	dword ptr [rax + rax]
1802a3b20: 48 8d 40 01                 	lea	rax, [rax + 0x1]
1802a3b24: 80 3c 01 00                 	cmp	byte ptr [rcx + rax], 0x0
1802a3b28: 75 f6                       	jne	0x1802a3b20 <.text+0x2a2b20>
1802a3b2a: 4c 8d 40 01                 	lea	r8, [rax + 0x1]
1802a3b2e: 49 83 f8 40                 	cmp	r8, 0x40
1802a3b32: 77 0e                       	ja	0x1802a3b42 <.text+0x2a2b42>
1802a3b34: 48 8d 54 24 40              	lea	rdx, [rsp + 0x40]
1802a3b39: 48 8d 4d 90                 	lea	rcx, [rbp - 0x70]
1802a3b3d: e8 e4 6c f9 ff              	call	0x18023a826 <.text+0x239826>
1802a3b42: c6 44 24 30 01              	mov	byte ptr [rsp + 0x30], 0x1
1802a3b47: 4c 89 64 24 20              	mov	qword ptr [rsp + 0x20], r12
1802a3b4c: 4c 8d 4d 90                 	lea	r9, [rbp - 0x70]
1802a3b50: 4c 8d 05 f9 2a 16 00        	lea	r8, [rip + 0x162af9]    # 0x180406650 ; STRING: mDLSSNRSkinStructureStrength
1802a3b57: 48 8d 15 5a df 15 00        	lea	rdx, [rip + 0x15df5a]   # 0x180401ab8 ; STRING: DLSS NR
1802a3b5e: 48 8d 4d d0                 	lea	rcx, [rbp - 0x30]
1802a3b62: e8 39 26 eb ff              	call	0x1801561a0 <.text+0x1551a0>
1802a3b67: 49 8b d7                    	mov	rdx, r15
1802a3b6a: 41 80 be c4 02 00 00 00     	cmp	byte ptr [r14 + 0x2c4], 0x0
1802a3b72: 48 0f 45 d7                 	cmovne	rdx, rdi
1802a3b76: 48 8b c3                    	mov	rax, rbx
1802a3b79: 0f 1f 80 00 00 00 00        	nop	dword ptr [rax]
1802a3b80: 48 8d 40 01                 	lea	rax, [rax + 0x1]
1802a3b84: 80 3c 02 00                 	cmp	byte ptr [rdx + rax], 0x0
1802a3b88: 75 f6                       	jne	0x1802a3b80 <.text+0x2a2b80>
1802a3b8a: 4c 8d 40 01                 	lea	r8, [rax + 0x1]
1802a3b8e: 49 83 f8 40                 	cmp	r8, 0x40
1802a3b92: 77 0a                       	ja	0x1802a3b9e <.text+0x2a2b9e>
1802a3b94: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a3b99: e8 88 6c f9 ff              	call	0x18023a826 <.text+0x239826>
1802a3b9e: c6 44 24 30 01              	mov	byte ptr [rsp + 0x30], 0x1
1802a3ba3: 4c 89 64 24 20              	mov	qword ptr [rsp + 0x20], r12
1802a3ba8: 4c 8d 4c 24 40              	lea	r9, [rsp + 0x40]
1802a3bad: 4c 8d 05 f4 2a 16 00        	lea	r8, [rip + 0x162af4]    # 0x1804066a8 ; STRING: mDLSSNRUseAutoMask
1802a3bb4: 48 8d 15 fd de 15 00        	lea	rdx, [rip + 0x15defd]   # 0x180401ab8 ; STRING: DLSS NR
1802a3bbb: 48 8d 4d d0                 	lea	rcx, [rbp - 0x30]
1802a3bbf: e8 dc 25 eb ff              	call	0x1801561a0 <.text+0x1551a0>
1802a3bc4: 49 8b d7                    	mov	rdx, r15
1802a3bc7: 41 80 be c5 02 00 00 00     	cmp	byte ptr [r14 + 0x2c5], 0x0
1802a3bcf: 48 0f 45 d7                 	cmovne	rdx, rdi
1802a3bd3: 48 8b c3                    	mov	rax, rbx
1802a3bd6: 48 8d 40 01                 	lea	rax, [rax + 0x1]
1802a3bda: 80 3c 02 00                 	cmp	byte ptr [rdx + rax], 0x0
1802a3bde: 75 f6                       	jne	0x1802a3bd6 <.text+0x2a2bd6>
1802a3be0: 4c 8d 40 01                 	lea	r8, [rax + 0x1]
1802a3be4: 49 83 f8 40                 	cmp	r8, 0x40
1802a3be8: 77 0a                       	ja	0x1802a3bf4 <.text+0x2a2bf4>
1802a3bea: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a3bef: e8 32 6c f9 ff              	call	0x18023a826 <.text+0x239826>
1802a3bf4: c6 44 24 30 01              	mov	byte ptr [rsp + 0x30], 0x1
1802a3bf9: 4c 89 64 24 20              	mov	qword ptr [rsp + 0x20], r12
1802a3bfe: 4c 8d 4c 24 40              	lea	r9, [rsp + 0x40]
1802a3c03: 4c 8d 05 86 2a 16 00        	lea	r8, [rip + 0x162a86]    # 0x180406690 ; STRING: mDLSSNRUICorrection
1802a3c0a: 48 8d 15 a7 de 15 00        	lea	rdx, [rip + 0x15dea7]   # 0x180401ab8 ; STRING: DLSS NR
1802a3c11: 48 8d 4d d0                 	lea	rcx, [rbp - 0x30]
1802a3c15: e8 86 25 eb ff              	call	0x1801561a0 <.text+0x1551a0>
1802a3c1a: 45 8b 86 c8 02 00 00        	mov	r8d, dword ptr [r14 + 0x2c8]
1802a3c21: 48 8d 15 a4 3a 16 00        	lea	rdx, [rip + 0x163aa4]   # 0x1804076cc ; STRING: %ld
1802a3c28: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a3c2d: e8 4e ce ee ff              	call	0x180190a80 <.text+0x18fa80>
1802a3c32: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a3c37: 48 8b c3                    	mov	rax, rbx
1802a3c3a: 66 0f 1f 44 00 00           	nop	word ptr [rax + rax]
1802a3c40: 48 8d 40 01                 	lea	rax, [rax + 0x1]
1802a3c44: 80 3c 01 00                 	cmp	byte ptr [rcx + rax], 0x0
1802a3c48: 75 f6                       	jne	0x1802a3c40 <.text+0x2a2c40>
1802a3c4a: 4c 8d 40 01                 	lea	r8, [rax + 0x1]
1802a3c4e: 49 83 f8 40                 	cmp	r8, 0x40
1802a3c52: 77 0e                       	ja	0x1802a3c62 <.text+0x2a2c62>
1802a3c54: 48 8d 54 24 40              	lea	rdx, [rsp + 0x40]
1802a3c59: 48 8d 4d 90                 	lea	rcx, [rbp - 0x70]
1802a3c5d: e8 c4 6b f9 ff              	call	0x18023a826 <.text+0x239826>
1802a3c62: c6 44 24 30 01              	mov	byte ptr [rsp + 0x30], 0x1
1802a3c67: 4c 89 64 24 20              	mov	qword ptr [rsp + 0x20], r12
1802a3c6c: 4c 8d 4d 90                 	lea	r9, [rbp - 0x70]
1802a3c70: 4c 8d 05 69 2a 16 00        	lea	r8, [rip + 0x162a69]    # 0x1804066e0 ; STRING: mDLSSNRResolveMethod
1802a3c77: 48 8d 15 3a de 15 00        	lea	rdx, [rip + 0x15de3a]   # 0x180401ab8 ; STRING: DLSS NR
1802a3c7e: 48 8d 4d d0                 	lea	rcx, [rbp - 0x30]
1802a3c82: e8 19 25 eb ff              	call	0x1801561a0 <.text+0x1551a0>
1802a3c87: c4 c1 7a 10 96 cc 02 00 00  	vmovss	xmm2, dword ptr [r14 + 0x2cc]
1802a3c90: c5 ea 5a d2                 	vcvtss2sd	xmm2, xmm2, xmm2
1802a3c94: c4 c1 f9 7e d0              	vmovq	r8, xmm2
1802a3c99: 48 8d 15 c8 3e 16 00        	lea	rdx, [rip + 0x163ec8]   # 0x180407b68
1802a3ca0: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a3ca5: e8 d6 cd ee ff              	call	0x180190a80 <.text+0x18fa80>
1802a3caa: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a3caf: 48 8b c3                    	mov	rax, rbx
1802a3cb2: 48 8d 40 01                 	lea	rax, [rax + 0x1]
1802a3cb6: 80 3c 01 00                 	cmp	byte ptr [rcx + rax], 0x0
1802a3cba: 75 f6                       	jne	0x1802a3cb2 <.text+0x2a2cb2>
1802a3cbc: 4c 8d 40 01                 	lea	r8, [rax + 0x1]
1802a3cc0: 49 83 f8 40                 	cmp	r8, 0x40
1802a3cc4: 77 0e                       	ja	0x1802a3cd4 <.text+0x2a2cd4>
1802a3cc6: 48 8d 54 24 40              	lea	rdx, [rsp + 0x40]
1802a3ccb: 48 8d 4d 90                 	lea	rcx, [rbp - 0x70]
1802a3ccf: e8 52 6b f9 ff              	call	0x18023a826 <.text+0x239826>
1802a3cd4: c6 44 24 30 01              	mov	byte ptr [rsp + 0x30], 0x1
1802a3cd9: 4c 89 64 24 20              	mov	qword ptr [rsp + 0x20], r12
1802a3cde: 4c 8d 4d 90                 	lea	r9, [rbp - 0x70]
1802a3ce2: 4c 8d 05 d7 29 16 00        	lea	r8, [rip + 0x1629d7]    # 0x1804066c0 ; STRING: mDLSSNRInputResolutionScale
1802a3ce9: 48 8d 15 c8 dd 15 00        	lea	rdx, [rip + 0x15ddc8]   # 0x180401ab8 ; STRING: DLSS NR
1802a3cf0: 48 8d 4d d0                 	lea	rcx, [rbp - 0x30]
1802a3cf4: e8 a7 24 eb ff              	call	0x1801561a0 <.text+0x1551a0>
1802a3cf9: c4 c1 7a 10 96 d0 02 00 00  	vmovss	xmm2, dword ptr [r14 + 0x2d0]
1802a3d02: c5 ea 5a d2                 	vcvtss2sd	xmm2, xmm2, xmm2
1802a3d06: c4 c1 f9 7e d0              	vmovq	r8, xmm2
1802a3d0b: 48 8d 15 56 3e 16 00        	lea	rdx, [rip + 0x163e56]   # 0x180407b68
1802a3d12: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a3d17: e8 64 cd ee ff              	call	0x180190a80 <.text+0x18fa80>
1802a3d1c: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a3d21: 48 8b c3                    	mov	rax, rbx
1802a3d24: 48 8d 40 01                 	lea	rax, [rax + 0x1]
1802a3d28: 80 3c 01 00                 	cmp	byte ptr [rcx + rax], 0x0
1802a3d2c: 75 f6                       	jne	0x1802a3d24 <.text+0x2a2d24>
1802a3d2e: 4c 8d 40 01                 	lea	r8, [rax + 0x1]
1802a3d32: 49 83 f8 40                 	cmp	r8, 0x40
1802a3d36: 77 0e                       	ja	0x1802a3d46 <.text+0x2a2d46>
1802a3d38: 48 8d 54 24 40              	lea	rdx, [rsp + 0x40]
1802a3d3d: 48 8d 4d 90                 	lea	rcx, [rbp - 0x70]
1802a3d41: e8 e0 6a f9 ff              	call	0x18023a826 <.text+0x239826>
1802a3d46: c6 44 24 30 01              	mov	byte ptr [rsp + 0x30], 0x1
1802a3d4b: 4c 89 64 24 20              	mov	qword ptr [rsp + 0x20], r12
1802a3d50: 4c 8d 4d 90                 	lea	r9, [rbp - 0x70]
1802a3d54: 4c 8d 05 b5 29 16 00        	lea	r8, [rip + 0x1629b5]    # 0x180406710 ; STRING: mDLSSNRTransferStrength
1802a3d5b: 48 8d 15 56 dd 15 00        	lea	rdx, [rip + 0x15dd56]   # 0x180401ab8 ; STRING: DLSS NR
1802a3d62: 48 8d 4d d0                 	lea	rcx, [rbp - 0x30]
1802a3d66: e8 35 24 eb ff              	call	0x1801561a0 <.text+0x1551a0>
1802a3d6b: c4 c1 7a 10 96 d4 02 00 00  	vmovss	xmm2, dword ptr [r14 + 0x2d4]
1802a3d74: c5 ea 5a d2                 	vcvtss2sd	xmm2, xmm2, xmm2
1802a3d78: c4 c1 f9 7e d0              	vmovq	r8, xmm2
1802a3d7d: 48 8d 15 e4 3d 16 00        	lea	rdx, [rip + 0x163de4]   # 0x180407b68
1802a3d84: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a3d89: e8 f2 cc ee ff              	call	0x180190a80 <.text+0x18fa80>
1802a3d8e: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a3d93: 48 8b c3                    	mov	rax, rbx
1802a3d96: 48 8d 40 01                 	lea	rax, [rax + 0x1]
1802a3d9a: 80 3c 01 00                 	cmp	byte ptr [rcx + rax], 0x0
1802a3d9e: 75 f6                       	jne	0x1802a3d96 <.text+0x2a2d96>
1802a3da0: 4c 8d 40 01                 	lea	r8, [rax + 0x1]
1802a3da4: 49 83 f8 40                 	cmp	r8, 0x40
1802a3da8: 77 0e                       	ja	0x1802a3db8 <.text+0x2a2db8>
1802a3daa: 48 8d 54 24 40              	lea	rdx, [rsp + 0x40]
1802a3daf: 48 8d 4d 90                 	lea	rcx, [rbp - 0x70]
1802a3db3: e8 6e 6a f9 ff              	call	0x18023a826 <.text+0x239826>
1802a3db8: c6 44 24 30 01              	mov	byte ptr [rsp + 0x30], 0x1
1802a3dbd: 4c 89 64 24 20              	mov	qword ptr [rsp + 0x20], r12
1802a3dc2: 4c 8d 4d 90                 	lea	r9, [rbp - 0x70]
1802a3dc6: 4c 8d 05 2b 29 16 00        	lea	r8, [rip + 0x16292b]    # 0x1804066f8 ; STRING: mDLSSNRColourStrength
1802a3dcd: 48 8d 15 e4 dc 15 00        	lea	rdx, [rip + 0x15dce4]   # 0x180401ab8 ; STRING: DLSS NR
1802a3dd4: 48 8d 4d d0                 	lea	rcx, [rbp - 0x30]
1802a3dd8: e8 c3 23 eb ff              	call	0x1801561a0 <.text+0x1551a0>
1802a3ddd: c4 c1 7a 10 96 d8 02 00 00  	vmovss	xmm2, dword ptr [r14 + 0x2d8]
1802a3de6: c5 ea 5a d2                 	vcvtss2sd	xmm2, xmm2, xmm2
1802a3dea: c4 c1 f9 7e d0              	vmovq	r8, xmm2
1802a3def: 48 8d 15 72 3d 16 00        	lea	rdx, [rip + 0x163d72]   # 0x180407b68
1802a3df6: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a3dfb: e8 80 cc ee ff              	call	0x180190a80 <.text+0x18fa80>
1802a3e00: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a3e05: 48 8b c3                    	mov	rax, rbx
1802a3e08: 0f 1f 84 00 00 00 00 00     	nop	dword ptr [rax + rax]
1802a3e10: 48 8d 40 01                 	lea	rax, [rax + 0x1]
1802a3e14: 80 3c 01 00                 	cmp	byte ptr [rcx + rax], 0x0
1802a3e18: 75 f6                       	jne	0x1802a3e10 <.text+0x2a2e10>
1802a3e1a: 4c 8d 40 01                 	lea	r8, [rax + 0x1]
1802a3e1e: 49 83 f8 40                 	cmp	r8, 0x40
1802a3e22: 77 0e                       	ja	0x1802a3e32 <.text+0x2a2e32>
1802a3e24: 48 8d 54 24 40              	lea	rdx, [rsp + 0x40]
1802a3e29: 48 8d 4d 90                 	lea	rcx, [rbp - 0x70]
1802a3e2d: e8 f4 69 f9 ff              	call	0x18023a826 <.text+0x239826>
1802a3e32: c6 44 24 30 01              	mov	byte ptr [rsp + 0x30], 0x1
1802a3e37: 4c 89 64 24 20              	mov	qword ptr [rsp + 0x20], r12
1802a3e3c: 4c 8d 4d 90                 	lea	r9, [rbp - 0x70]
1802a3e40: 4c 8d 05 f9 28 16 00        	lea	r8, [rip + 0x1628f9]    # 0x180406740 ; STRING: mDLSSNRMaxRatio
1802a3e47: 48 8d 15 6a dc 15 00        	lea	rdx, [rip + 0x15dc6a]   # 0x180401ab8 ; STRING: DLSS NR
1802a3e4e: 48 8d 4d d0                 	lea	rcx, [rbp - 0x30]
1802a3e52: e8 49 23 eb ff              	call	0x1801561a0 <.text+0x1551a0>
1802a3e57: c4 c1 7a 10 96 dc 02 00 00  	vmovss	xmm2, dword ptr [r14 + 0x2dc]
1802a3e60: c5 ea 5a d2                 	vcvtss2sd	xmm2, xmm2, xmm2
1802a3e64: c4 c1 f9 7e d0              	vmovq	r8, xmm2
1802a3e69: 48 8d 15 f8 3c 16 00        	lea	rdx, [rip + 0x163cf8]   # 0x180407b68
1802a3e70: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a3e75: e8 06 cc ee ff              	call	0x180190a80 <.text+0x18fa80>
1802a3e7a: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a3e7f: 48 8b c3                    	mov	rax, rbx
1802a3e82: 48 8d 40 01                 	lea	rax, [rax + 0x1]
1802a3e86: 80 3c 01 00                 	cmp	byte ptr [rcx + rax], 0x0
1802a3e8a: 75 f6                       	jne	0x1802a3e82 <.text+0x2a2e82>
1802a3e8c: 4c 8d 40 01                 	lea	r8, [rax + 0x1]
1802a3e90: 49 83 f8 40                 	cmp	r8, 0x40
1802a3e94: 77 0e                       	ja	0x1802a3ea4 <.text+0x2a2ea4>
1802a3e96: 48 8d 54 24 40              	lea	rdx, [rsp + 0x40]
1802a3e9b: 48 8d 4d 90                 	lea	rcx, [rbp - 0x70]
1802a3e9f: e8 82 69 f9 ff              	call	0x18023a826 <.text+0x239826>
1802a3ea4: c6 44 24 30 01              	mov	byte ptr [rsp + 0x30], 0x1
1802a3ea9: 4c 89 64 24 20              	mov	qword ptr [rsp + 0x20], r12
1802a3eae: 4c 8d 4d 90                 	lea	r9, [rbp - 0x70]
1802a3eb2: 4c 8d 05 6f 28 16 00        	lea	r8, [rip + 0x16286f]    # 0x180406728 ; STRING: mDLSSNRWhitePoint
1802a3eb9: 48 8d 15 f8 db 15 00        	lea	rdx, [rip + 0x15dbf8]   # 0x180401ab8 ; STRING: DLSS NR
1802a3ec0: 48 8d 4d d0                 	lea	rcx, [rbp - 0x30]
1802a3ec4: e8 d7 22 eb ff              	call	0x1801561a0 <.text+0x1551a0>
1802a3ec9: 41 80 be e0 02 00 00 00     	cmp	byte ptr [r14 + 0x2e0], 0x0
1802a3ed1: 4c 0f 45 ff                 	cmovne	r15, rdi
1802a3ed5: 48 8b c3                    	mov	rax, rbx
1802a3ed8: 0f 1f 84 00 00 00 00 00     	nop	dword ptr [rax + rax]
1802a3ee0: 48 8d 40 01                 	lea	rax, [rax + 0x1]
1802a3ee4: 41 80 3c 07 00              	cmp	byte ptr [r15 + rax], 0x0
1802a3ee9: 75 f5                       	jne	0x1802a3ee0 <.text+0x2a2ee0>
1802a3eeb: 4c 8d 40 01                 	lea	r8, [rax + 0x1]
1802a3eef: 49 83 f8 40                 	cmp	r8, 0x40
1802a3ef3: 77 0d                       	ja	0x1802a3f02 <.text+0x2a2f02>
1802a3ef5: 49 8b d7                    	mov	rdx, r15
1802a3ef8: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a3efd: e8 24 69 f9 ff              	call	0x18023a826 <.text+0x239826>
1802a3f02: c6 44 24 30 01              	mov	byte ptr [rsp + 0x30], 0x1
1802a3f07: 4c 89 64 24 20              	mov	qword ptr [rsp + 0x20], r12
1802a3f0c: 4c 8d 4c 24 40              	lea	r9, [rsp + 0x40]
1802a3f11: 4c 8d 05 48 28 16 00        	lea	r8, [rip + 0x162848]    # 0x180406760 ; STRING: mDLSSNRColorIsHdr
1802a3f18: 48 8d 15 99 db 15 00        	lea	rdx, [rip + 0x15db99]   # 0x180401ab8 ; STRING: DLSS NR
1802a3f1f: 48 8d 4d d0                 	lea	rcx, [rbp - 0x30]
1802a3f23: e8 78 22 eb ff              	call	0x1801561a0 <.text+0x1551a0>
1802a3f28: 45 8b 86 e4 02 00 00        	mov	r8d, dword ptr [r14 + 0x2e4]
1802a3f2f: 48 8d 15 96 37 16 00        	lea	rdx, [rip + 0x163796]   # 0x1804076cc ; STRING: %ld
1802a3f36: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a3f3b: e8 40 cb ee ff              	call	0x180190a80 <.text+0x18fa80>
1802a3f40: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a3f45: 48 8b c3                    	mov	rax, rbx
1802a3f48: 0f 1f 84 00 00 00 00 00     	nop	dword ptr [rax + rax]
1802a3f50: 48 8d 40 01                 	lea	rax, [rax + 0x1]
1802a3f54: 80 3c 01 00                 	cmp	byte ptr [rcx + rax], 0x0
1802a3f58: 75 f6                       	jne	0x1802a3f50 <.text+0x2a2f50>
1802a3f5a: 4c 8d 40 01                 	lea	r8, [rax + 0x1]
1802a3f5e: 49 83 f8 40                 	cmp	r8, 0x40
1802a3f62: 77 0e                       	ja	0x1802a3f72 <.text+0x2a2f72>
1802a3f64: 48 8d 54 24 40              	lea	rdx, [rsp + 0x40]
1802a3f69: 48 8d 4d 90                 	lea	rcx, [rbp - 0x70]
1802a3f6d: e8 b4 68 f9 ff              	call	0x18023a826 <.text+0x239826>
1802a3f72: c6 44 24 30 01              	mov	byte ptr [rsp + 0x30], 0x1
1802a3f77: 4c 89 64 24 20              	mov	qword ptr [rsp + 0x20], r12
1802a3f7c: 4c 8d 4d 90                 	lea	r9, [rbp - 0x70]
1802a3f80: 4c 8d 05 c9 27 16 00        	lea	r8, [rip + 0x1627c9]    # 0x180406750 ; STRING: mDLSSNRPass
1802a3f87: 48 8d 15 2a db 15 00        	lea	rdx, [rip + 0x15db2a]   # 0x180401ab8 ; STRING: DLSS NR
1802a3f8e: 48 8d 4d d0                 	lea	rcx, [rbp - 0x30]
1802a3f92: e8 09 22 eb ff              	call	0x1801561a0 <.text+0x1551a0>
1802a3f97: e8 04 ee ea ff              	call	0x180152da0 <.text+0x151da0>
1802a3f9c: 44 8b 40 7c                 	mov	r8d, dword ptr [rax + 0x7c]
1802a3fa0: 48 8d 15 25 37 16 00        	lea	rdx, [rip + 0x163725]   # 0x1804076cc ; STRING: %ld
1802a3fa7: 48 8d 4c 24 40              	lea	rcx, [rsp + 0x40]
1802a3fac: e8 cf ca ee ff              	call	0x180190a80 <.text+0x18fa80>
1802a3fb1: 48 8d 44 24 40              	lea	rax, [rsp + 0x40]
1802a3fb6: 48 8d 5b 01                 	lea	rbx, [rbx + 0x1]
1802a3fba: 80 3c 18 00                 	cmp	byte ptr [rax + rbx], 0x0
1802a3fbe: 75 f6                       	jne	0x1802a3fb6 <.text+0x2a2fb6>
1802a3fc0: 4c 8d 43 01                 	lea	r8, [rbx + 0x1]
1802a3fc4: 49 83 f8 40                 	cmp	r8, 0x40
1802a3fc8: 77 0e                       	ja	0x1802a3fd8 <.text+0x2a2fd8>
1802a3fca: 48 8d 54 24 40              	lea	rdx, [rsp + 0x40]
1802a3fcf: 48 8d 4d 90                 	lea	rcx, [rbp - 0x70]
1802a3fd3: e8 4e 68 f9 ff              	call	0x18023a826 <.text+0x239826>
1802a3fd8: c6 44 24 30 01              	mov	byte ptr [rsp + 0x30], 0x1
1802a3fdd: 4c 89 64 24 20              	mov	qword ptr [rsp + 0x20], r12
1802a3fe2: 4c 8d 4d 90                 	lea	r9, [rbp - 0x70]
1802a3fe6: 4c 8d 05 b3 27 16 00        	lea	r8, [rip + 0x1627b3]    # 0x1804067a0 ; STRING: mFontSize
1802a3fed: 48 8d 15 a0 27 16 00        	lea	rdx, [rip + 0x1627a0]   # 0x180406794 ; STRING: Menu
1802a3ff4: 48 8d 4d d0                 	lea	rcx, [rbp - 0x30]
1802a3ff8: e8 a3 21 eb ff              	call	0x1801561a0 <.text+0x1551a0>
1802a3ffd: 48 8b 35 b4 cd bd 00        	mov	rsi, qword ptr [rip + 0xbdcdb4] # 0x180e80db8 <SKSEPlugin_Version+0xa1fa08>