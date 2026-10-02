
/mnt/data/aio18_nr_work/PDPerfPlugin.dll:	file format coff-x86-64

Disassembly of section .text:

0000000180078890 <SetPDFrameWarpNativeCameraSource>:
18009cfb0: b8 01 00 00 00              	mov	eax, 0x1
18009cfb5: 85 c9                       	test	ecx, ecx
18009cfb7: 78 05                       	js	0x18009cfbe <SetPDFrameWarpNativeCameraSource+0x2472e>
18009cfb9: 0f 44 c8                    	cmove	ecx, eax
18009cfbc: 8b c1                       	mov	eax, ecx
18009cfbe: c3                          	ret
18009cfbf: cc                          	int3
18009cfc0: f3 0f 10 15 08 fb 10 01     	movss	xmm2, dword ptr [rip + 0x110fb08] # 0x1811acad0
18009cfc8: 0f 57 c9                    	xorps	xmm1, xmm1
18009cfcb: 0f 2f c8                    	comiss	xmm1, xmm0
18009cfce: 73 0e                       	jae	0x18009cfde <SetPDFrameWarpNativeCameraSource+0x2474e>
18009cfd0: 0f 2f c2                    	comiss	xmm0, xmm2
18009cfd3: 73 09                       	jae	0x18009cfde <SetPDFrameWarpNativeCameraSource+0x2474e>
18009cfd5: f3 0f 5f 05 af fa 10 01     	maxss	xmm0, dword ptr [rip + 0x110faaf] # 0x1811aca8c
18009cfdd: c3                          	ret
18009cfde: 0f 28 c2                    	movaps	xmm0, xmm2
18009cfe1: c3                          	ret
18009cfe2: cc                          	int3
18009cfe3: cc                          	int3
18009cfe4: cc                          	int3
18009cfe5: cc                          	int3
18009cfe6: cc                          	int3
18009cfe7: cc                          	int3
18009cfe8: cc                          	int3
18009cfe9: cc                          	int3
18009cfea: cc                          	int3
18009cfeb: cc                          	int3
18009cfec: cc                          	int3
18009cfed: cc                          	int3
18009cfee: cc                          	int3
18009cfef: cc                          	int3
18009cff0: 83 b9 14 01 00 00 02        	cmp	dword ptr [rcx + 0x114], 0x2
18009cff7: 75 06                       	jne	0x18009cfff <SetPDFrameWarpNativeCameraSource+0x2476f>
18009cff9: b8 02 00 00 00              	mov	eax, 0x2
18009cffe: c3                          	ret
18009cfff: f3 0f 10 05 c9 fa 10 01     	movss	xmm0, dword ptr [rip + 0x110fac9] # 0x1811acad0
18009d007: 33 c0                       	xor	eax, eax
18009d009: 0f 2f c1                    	comiss	xmm0, xmm1
18009d00c: 0f 97 c0                    	seta	al
18009d00f: c3                          	ret
