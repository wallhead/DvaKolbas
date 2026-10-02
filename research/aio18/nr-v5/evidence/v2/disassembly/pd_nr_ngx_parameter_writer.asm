
/mnt/data/aio18_nr_work/PDPerfPlugin.dll:	file format coff-x86-64

Disassembly of section .text:

0000000180078890 <SetPDFrameWarpNativeCameraSource>:
18009af70: 48 89 5c 24 08              	mov	qword ptr [rsp + 0x8], rbx
18009af75: 48 89 6c 24 10              	mov	qword ptr [rsp + 0x10], rbp
18009af7a: 48 89 74 24 18              	mov	qword ptr [rsp + 0x18], rsi
18009af7f: 57                          	push	rdi
18009af80: 48 83 ec 40                 	sub	rsp, 0x40
18009af84: 49 8b d8                    	mov	rbx, r8
18009af87: 0f 29 74 24 30              	movaps	xmmword ptr [rsp + 0x30], xmm6
18009af8c: 4d 8b 01                    	mov	r8, qword ptr [r9]
18009af8f: 48 8b f2                    	mov	rsi, rdx
18009af92: 48 8b e9                    	mov	rbp, rcx
18009af95: 0f 29 7c 24 20              	movaps	xmmword ptr [rsp + 0x20], xmm7
18009af9a: 48 8b cb                    	mov	rcx, rbx
18009af9d: 48 8d 15 34 87 10 01        	lea	rdx, [rip + 0x1108734]  # 0x1811a36d8    ; STRING: DLSSNR.Color
18009afa4: 49 8b f9                    	mov	rdi, r9
18009afa7: e8 f4 86 06 00              	call	0x1801036a0 <NVSDK_NGX_Parameter_SetD3d12Resource>
18009afac: 4c 8b 47 08                 	mov	r8, qword ptr [rdi + 0x8]
18009afb0: 48 8d 15 61 87 10 01        	lea	rdx, [rip + 0x1108761]  # 0x1811a3718    ; STRING: DLSSNR.MVec
18009afb7: 48 8b cb                    	mov	rcx, rbx
18009afba: e8 e1 86 06 00              	call	0x1801036a0 <NVSDK_NGX_Parameter_SetD3d12Resource>
18009afbf: 4c 8b 47 10                 	mov	r8, qword ptr [rdi + 0x10]
18009afc3: 48 8d 15 3e 87 10 01        	lea	rdx, [rip + 0x110873e]  # 0x1811a3708    ; STRING: DLSSNR.Depth
18009afca: 48 8b cb                    	mov	rcx, rbx
18009afcd: e8 ce 86 06 00              	call	0x1801036a0 <NVSDK_NGX_Parameter_SetD3d12Resource>
18009afd2: 4c 8b 47 18                 	mov	r8, qword ptr [rdi + 0x18]
18009afd6: 48 8d 15 63 87 10 01        	lea	rdx, [rip + 0x1108763]  # 0x1811a3740    ; STRING: DLSSNR.Output
18009afdd: 48 8b cb                    	mov	rcx, rbx
18009afe0: e8 bb 86 06 00              	call	0x1801036a0 <NVSDK_NGX_Parameter_SetD3d12Resource>
18009afe5: 4c 8b 47 20                 	mov	r8, qword ptr [rdi + 0x20]
18009afe9: 48 8d 15 38 87 10 01        	lea	rdx, [rip + 0x1108738]  # 0x1811a3728    ; STRING: DLSSNR.ControlMask
18009aff0: 48 8b cb                    	mov	rcx, rbx
18009aff3: e8 a8 86 06 00              	call	0x1801036a0 <NVSDK_NGX_Parameter_SetD3d12Resource>
18009aff8: 4c 8b 47 28                 	mov	r8, qword ptr [rdi + 0x28]
18009affc: 48 8d 15 5d 87 10 01        	lea	rdx, [rip + 0x110875d]  # 0x1811a3760    ; STRING: DLSSNR.UI
18009b003: 48 8b cb                    	mov	rcx, rbx
18009b006: e8 95 86 06 00              	call	0x1801036a0 <NVSDK_NGX_Parameter_SetD3d12Resource>
18009b00b: 4c 8b 47 30                 	mov	r8, qword ptr [rdi + 0x30]
18009b00f: 48 8d 15 3a 87 10 01        	lea	rdx, [rip + 0x110873a]  # 0x1811a3750    ; STRING: DLSSNR.UIAlpha
18009b016: 48 8b cb                    	mov	rcx, rbx
18009b019: e8 82 86 06 00              	call	0x1801036a0 <NVSDK_NGX_Parameter_SetD3d12Resource>
18009b01e: 4c 8b 47 38                 	mov	r8, qword ptr [rdi + 0x38]
18009b022: 48 8d 15 6f 87 10 01        	lea	rdx, [rip + 0x110876f]  # 0x1811a3798    ; STRING: DLSSNR.Backbuffer
18009b029: 48 8b cb                    	mov	rcx, rbx
18009b02c: e8 6f 86 06 00              	call	0x1801036a0 <NVSDK_NGX_Parameter_SetD3d12Resource>
18009b031: 4c 8b 47 40                 	mov	r8, qword ptr [rdi + 0x40]
18009b035: 48 8d 15 34 87 10 01        	lea	rdx, [rip + 0x1108734]  # 0x1811a3770    ; STRING: DLSSNR.BidirectionalDistortionField
18009b03c: 48 8b cb                    	mov	rcx, rbx
18009b03f: e8 5c 86 06 00              	call	0x1801036a0 <NVSDK_NGX_Parameter_SetD3d12Resource>
18009b044: 44 8b 47 48                 	mov	r8d, dword ptr [rdi + 0x48]
18009b048: 48 8d 15 a1 87 10 01        	lea	rdx, [rip + 0x11087a1]  # 0x1811a37f0    ; STRING: DLSSNR.ColorSubrectBaseX
18009b04f: 48 8b cb                    	mov	rcx, rbx
18009b052: e8 59 87 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b057: 44 8b 47 4c                 	mov	r8d, dword ptr [rdi + 0x4c]
18009b05b: 48 8d 15 ae 87 10 01        	lea	rdx, [rip + 0x11087ae]  # 0x1811a3810    ; STRING: DLSSNR.ColorSubrectBaseY
18009b062: 48 8b cb                    	mov	rcx, rbx
18009b065: e8 46 87 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b06a: 44 8b 47 50                 	mov	r8d, dword ptr [rdi + 0x50]
18009b06e: 48 8d 15 3b 87 10 01        	lea	rdx, [rip + 0x110873b]  # 0x1811a37b0    ; STRING: DLSSNR.ColorSubrectWidth
18009b075: 48 8b cb                    	mov	rcx, rbx
18009b078: e8 33 87 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b07d: 44 8b 47 54                 	mov	r8d, dword ptr [rdi + 0x54]
18009b081: 48 8d 15 48 87 10 01        	lea	rdx, [rip + 0x1108748]  # 0x1811a37d0    ; STRING: DLSSNR.ColorSubrectHeight
18009b088: 48 8b cb                    	mov	rcx, rbx
18009b08b: e8 20 87 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b090: 44 8b 47 58                 	mov	r8d, dword ptr [rdi + 0x58]
18009b094: 48 8d 15 cd 87 10 01        	lea	rdx, [rip + 0x11087cd]  # 0x1811a3868    ; STRING: DLSSNR.MVecSubrectBaseX
18009b09b: 48 8b cb                    	mov	rcx, rbx
18009b09e: e8 0d 87 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b0a3: 44 8b 47 5c                 	mov	r8d, dword ptr [rdi + 0x5c]
18009b0a7: 48 8d 15 d2 87 10 01        	lea	rdx, [rip + 0x11087d2]  # 0x1811a3880    ; STRING: DLSSNR.MVecSubrectBaseY
18009b0ae: 48 8b cb                    	mov	rcx, rbx
18009b0b1: e8 fa 86 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b0b6: 44 8b 47 60                 	mov	r8d, dword ptr [rdi + 0x60]
18009b0ba: 48 8d 15 6f 87 10 01        	lea	rdx, [rip + 0x110876f]  # 0x1811a3830    ; STRING: DLSSNR.MVecSubrectWidth
18009b0c1: 48 8b cb                    	mov	rcx, rbx
18009b0c4: e8 e7 86 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b0c9: 44 8b 47 64                 	mov	r8d, dword ptr [rdi + 0x64]
18009b0cd: 48 8d 15 74 87 10 01        	lea	rdx, [rip + 0x1108774]  # 0x1811a3848    ; STRING: DLSSNR.MVecSubrectHeight
18009b0d4: 48 8b cb                    	mov	rcx, rbx
18009b0d7: e8 d4 86 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b0dc: 44 8b 47 68                 	mov	r8d, dword ptr [rdi + 0x68]
18009b0e0: 48 8d 15 f1 87 10 01        	lea	rdx, [rip + 0x11087f1]  # 0x1811a38d8    ; STRING: DLSSNR.DepthSubrectBaseX
18009b0e7: 48 8b cb                    	mov	rcx, rbx
18009b0ea: e8 c1 86 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b0ef: 44 8b 47 6c                 	mov	r8d, dword ptr [rdi + 0x6c]
18009b0f3: 48 8d 15 fe 87 10 01        	lea	rdx, [rip + 0x11087fe]  # 0x1811a38f8    ; STRING: DLSSNR.DepthSubrectBaseY
18009b0fa: 48 8b cb                    	mov	rcx, rbx
18009b0fd: e8 ae 86 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b102: 44 8b 47 70                 	mov	r8d, dword ptr [rdi + 0x70]
18009b106: 48 8d 15 8b 87 10 01        	lea	rdx, [rip + 0x110878b]  # 0x1811a3898    ; STRING: DLSSNR.DepthSubrectWidth
18009b10d: 48 8b cb                    	mov	rcx, rbx
18009b110: e8 9b 86 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b115: 44 8b 47 74                 	mov	r8d, dword ptr [rdi + 0x74]
18009b119: 48 8d 15 98 87 10 01        	lea	rdx, [rip + 0x1108798]  # 0x1811a38b8    ; STRING: DLSSNR.DepthSubrectHeight
18009b120: 48 8b cb                    	mov	rcx, rbx
18009b123: e8 88 86 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b128: 44 8b 47 78                 	mov	r8d, dword ptr [rdi + 0x78]
18009b12c: 48 8d 15 25 88 10 01        	lea	rdx, [rip + 0x1108825]  # 0x1811a3958    ; STRING: DLSSNR.OutputSubrectBaseX
18009b133: 48 8b cb                    	mov	rcx, rbx
18009b136: e8 75 86 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b13b: 44 8b 47 7c                 	mov	r8d, dword ptr [rdi + 0x7c]
18009b13f: 48 8d 15 32 88 10 01        	lea	rdx, [rip + 0x1108832]  # 0x1811a3978    ; STRING: DLSSNR.OutputSubrectBaseY
18009b146: 48 8b cb                    	mov	rcx, rbx
18009b149: e8 62 86 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b14e: 44 8b 87 80 00 00 00        	mov	r8d, dword ptr [rdi + 0x80]
18009b155: 48 8d 15 bc 87 10 01        	lea	rdx, [rip + 0x11087bc]  # 0x1811a3918    ; STRING: DLSSNR.OutputSubrectWidth
18009b15c: 48 8b cb                    	mov	rcx, rbx
18009b15f: e8 4c 86 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b164: 44 8b 87 84 00 00 00        	mov	r8d, dword ptr [rdi + 0x84]
18009b16b: 48 8d 15 c6 87 10 01        	lea	rdx, [rip + 0x11087c6]  # 0x1811a3938    ; STRING: DLSSNR.OutputSubrectHeight
18009b172: 48 8b cb                    	mov	rcx, rbx
18009b175: e8 36 86 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b17a: 44 8b 87 88 00 00 00        	mov	r8d, dword ptr [rdi + 0x88]
18009b181: 48 8d 15 50 88 10 01        	lea	rdx, [rip + 0x1108850]  # 0x1811a39d8    ; STRING: DLSSNR.ControlMaskSubrectBaseX
18009b188: 48 8b cb                    	mov	rcx, rbx
18009b18b: e8 20 86 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b190: 44 8b 87 8c 00 00 00        	mov	r8d, dword ptr [rdi + 0x8c]
18009b197: 48 8d 15 5a 88 10 01        	lea	rdx, [rip + 0x110885a]  # 0x1811a39f8    ; STRING: DLSSNR.ControlMaskSubrectBaseY
18009b19e: 48 8b cb                    	mov	rcx, rbx
18009b1a1: e8 0a 86 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b1a6: 44 8b 87 90 00 00 00        	mov	r8d, dword ptr [rdi + 0x90]
18009b1ad: 48 8d 15 e4 87 10 01        	lea	rdx, [rip + 0x11087e4]  # 0x1811a3998    ; STRING: DLSSNR.ControlMaskSubrectWidth
18009b1b4: 48 8b cb                    	mov	rcx, rbx
18009b1b7: e8 f4 85 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b1bc: 44 8b 87 94 00 00 00        	mov	r8d, dword ptr [rdi + 0x94]
18009b1c3: 48 8d 15 ee 87 10 01        	lea	rdx, [rip + 0x11087ee]  # 0x1811a39b8    ; STRING: DLSSNR.ControlMaskSubrectHeight
18009b1ca: 48 8b cb                    	mov	rcx, rbx
18009b1cd: e8 de 85 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b1d2: 44 8b 87 98 00 00 00        	mov	r8d, dword ptr [rdi + 0x98]
18009b1d9: 48 8d 15 68 88 10 01        	lea	rdx, [rip + 0x1108868]  # 0x1811a3a48    ; STRING: DLSSNR.UISubrectBaseX
18009b1e0: 48 8b cb                    	mov	rcx, rbx
18009b1e3: e8 c8 85 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b1e8: 44 8b 87 9c 00 00 00        	mov	r8d, dword ptr [rdi + 0x9c]
18009b1ef: 48 8d 15 6a 88 10 01        	lea	rdx, [rip + 0x110886a]  # 0x1811a3a60    ; STRING: DLSSNR.UISubrectBaseY
18009b1f6: 48 8b cb                    	mov	rcx, rbx
18009b1f9: e8 b2 85 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b1fe: 44 8b 87 a0 00 00 00        	mov	r8d, dword ptr [rdi + 0xa0]
18009b205: 48 8d 15 0c 88 10 01        	lea	rdx, [rip + 0x110880c]  # 0x1811a3a18    ; STRING: DLSSNR.UISubrectWidth
18009b20c: 48 8b cb                    	mov	rcx, rbx
18009b20f: e8 9c 85 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b214: 44 8b 87 a4 00 00 00        	mov	r8d, dword ptr [rdi + 0xa4]
18009b21b: 48 8d 15 0e 88 10 01        	lea	rdx, [rip + 0x110880e]  # 0x1811a3a30    ; STRING: DLSSNR.UISubrectHeight
18009b222: 48 8b cb                    	mov	rcx, rbx
18009b225: e8 86 85 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b22a: 44 8b 87 a8 00 00 00        	mov	r8d, dword ptr [rdi + 0xa8]
18009b231: 48 8d 15 80 88 10 01        	lea	rdx, [rip + 0x1108880]  # 0x1811a3ab8    ; STRING: DLSSNR.UIAlphaSubrectBaseX
18009b238: 48 8b cb                    	mov	rcx, rbx
18009b23b: e8 70 85 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b240: 44 8b 87 ac 00 00 00        	mov	r8d, dword ptr [rdi + 0xac]
18009b247: 48 8d 15 8a 88 10 01        	lea	rdx, [rip + 0x110888a]  # 0x1811a3ad8    ; STRING: DLSSNR.UIAlphaSubrectBaseY
18009b24e: 48 8b cb                    	mov	rcx, rbx
18009b251: e8 5a 85 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b256: 44 8b 87 b0 00 00 00        	mov	r8d, dword ptr [rdi + 0xb0]
18009b25d: 48 8d 15 14 88 10 01        	lea	rdx, [rip + 0x1108814]  # 0x1811a3a78    ; STRING: DLSSNR.UIAlphaSubrectWidth
18009b264: 48 8b cb                    	mov	rcx, rbx
18009b267: e8 44 85 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b26c: 44 8b 87 b4 00 00 00        	mov	r8d, dword ptr [rdi + 0xb4]
18009b273: 48 8d 15 1e 88 10 01        	lea	rdx, [rip + 0x110881e]  # 0x1811a3a98    ; STRING: DLSSNR.UIAlphaSubrectHeight
18009b27a: 48 8b cb                    	mov	rcx, rbx
18009b27d: e8 2e 85 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b282: 44 8b 87 b8 00 00 00        	mov	r8d, dword ptr [rdi + 0xb8]
18009b289: 48 8d 15 a8 88 10 01        	lea	rdx, [rip + 0x11088a8]  # 0x1811a3b38    ; STRING: DLSSNR.BackbufferSubrectBaseX
18009b290: 48 8b cb                    	mov	rcx, rbx
18009b293: e8 18 85 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b298: 44 8b 87 bc 00 00 00        	mov	r8d, dword ptr [rdi + 0xbc]
18009b29f: 48 8d 15 b2 88 10 01        	lea	rdx, [rip + 0x11088b2]  # 0x1811a3b58    ; STRING: DLSSNR.BackbufferSubrectBaseY
18009b2a6: 48 8b cb                    	mov	rcx, rbx
18009b2a9: e8 02 85 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b2ae: 44 8b 87 c0 00 00 00        	mov	r8d, dword ptr [rdi + 0xc0]
18009b2b5: 48 8d 15 3c 88 10 01        	lea	rdx, [rip + 0x110883c]  # 0x1811a3af8    ; STRING: DLSSNR.BackbufferSubrectWidth
18009b2bc: 48 8b cb                    	mov	rcx, rbx
18009b2bf: e8 ec 84 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b2c4: 44 8b 87 c4 00 00 00        	mov	r8d, dword ptr [rdi + 0xc4]
18009b2cb: 48 8d 15 46 88 10 01        	lea	rdx, [rip + 0x1108846]  # 0x1811a3b18    ; STRING: DLSSNR.BackbufferSubrectHeight
18009b2d2: 48 8b cb                    	mov	rcx, rbx
18009b2d5: e8 d6 84 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b2da: 44 8b 87 c8 00 00 00        	mov	r8d, dword ptr [rdi + 0xc8]
18009b2e1: 48 8d 15 f8 88 10 01        	lea	rdx, [rip + 0x11088f8]  # 0x1811a3be0    ; STRING: DLSSNR.BidirectionalDistortionFieldSubrectBaseX
18009b2e8: 48 8b cb                    	mov	rcx, rbx
18009b2eb: e8 c0 84 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b2f0: 44 8b 87 cc 00 00 00        	mov	r8d, dword ptr [rdi + 0xcc]
18009b2f7: 48 8d 15 12 89 10 01        	lea	rdx, [rip + 0x1108912]  # 0x1811a3c10    ; STRING: DLSSNR.BidirectionalDistortionFieldSubrectBaseY
18009b2fe: 48 8b cb                    	mov	rcx, rbx
18009b301: e8 aa 84 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b306: 44 8b 87 d0 00 00 00        	mov	r8d, dword ptr [rdi + 0xd0]
18009b30d: 48 8d 15 64 88 10 01        	lea	rdx, [rip + 0x1108864]  # 0x1811a3b78    ; STRING: DLSSNR.BidirectionalDistortionFieldSubrectWidth
18009b314: 48 8b cb                    	mov	rcx, rbx
18009b317: e8 94 84 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b31c: 44 8b 87 d4 00 00 00        	mov	r8d, dword ptr [rdi + 0xd4]
18009b323: 48 8d 15 7e 88 10 01        	lea	rdx, [rip + 0x110887e]  # 0x1811a3ba8    ; STRING: DLSSNR.BidirectionalDistortionFieldSubrectHeight
18009b32a: 48 8b cb                    	mov	rcx, rbx
18009b32d: e8 7e 84 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b332: f3 0f 10 97 d8 00 00 00     	movss	xmm2, dword ptr [rdi + 0xd8]
18009b33a: 0f 57 ff                    	xorps	xmm7, xmm7
18009b33d: 0f 2e d7                    	ucomiss	xmm2, xmm7
18009b340: f3 0f 10 35 88 17 11 01     	movss	xmm6, dword ptr [rip + 0x1111788] # 0x1811acad0
18009b348: 7a 05                       	jp	0x18009b34f <SetPDFrameWarpNativeCameraSource+0x22abf>
18009b34a: 75 03                       	jne	0x18009b34f <SetPDFrameWarpNativeCameraSource+0x22abf>
18009b34c: 0f 28 d6                    	movaps	xmm2, xmm6
18009b34f: 48 8d 15 02 89 10 01        	lea	rdx, [rip + 0x1108902]  # 0x1811a3c58    ; STRING: DLSSNR.MVecScaleX
18009b356: 48 8b cb                    	mov	rcx, rbx
18009b359: e8 a2 83 06 00              	call	0x180103700 <NVSDK_NGX_Parameter_SetF>
18009b35e: f3 0f 10 87 dc 00 00 00     	movss	xmm0, dword ptr [rdi + 0xdc]
18009b366: 0f 2e c7                    	ucomiss	xmm0, xmm7
18009b369: 7a 02                       	jp	0x18009b36d <SetPDFrameWarpNativeCameraSource+0x22add>
18009b36b: 74 03                       	je	0x18009b370 <SetPDFrameWarpNativeCameraSource+0x22ae0>
18009b36d: 0f 28 f0                    	movaps	xmm6, xmm0
18009b370: 0f 28 d6                    	movaps	xmm2, xmm6
18009b373: 48 8d 15 c6 88 10 01        	lea	rdx, [rip + 0x11088c6]  # 0x1811a3c40    ; STRING: DLSSNR.MVecScaleY
18009b37a: 48 8b cb                    	mov	rcx, rbx
18009b37d: e8 7e 83 06 00              	call	0x180103700 <NVSDK_NGX_Parameter_SetF>
18009b382: f3 0f 10 97 e0 00 00 00     	movss	xmm2, dword ptr [rdi + 0xe0]
18009b38a: 48 8d 15 ff 88 10 01        	lea	rdx, [rip + 0x11088ff]  # 0x1811a3c90    ; STRING: DLSSNR.Intensity
18009b391: 48 8b cb                    	mov	rcx, rbx
18009b394: e8 67 83 06 00              	call	0x180103700 <NVSDK_NGX_Parameter_SetF>
18009b399: f3 0f 10 97 e4 00 00 00     	movss	xmm2, dword ptr [rdi + 0xe4]
18009b3a1: 48 8d 15 c8 88 10 01        	lea	rdx, [rip + 0x11088c8]  # 0x1811a3c70    ; STRING: DLSSNR.LocalToneStrength
18009b3a8: 48 8b cb                    	mov	rcx, rbx
18009b3ab: e8 50 83 06 00              	call	0x180103700 <NVSDK_NGX_Parameter_SetF>
18009b3b0: f3 0f 10 97 e8 00 00 00     	movss	xmm2, dword ptr [rdi + 0xe8]
18009b3b8: 48 8d 15 09 89 10 01        	lea	rdx, [rip + 0x1108909]  # 0x1811a3cc8    ; STRING: DLSSNR.LocalStructureStrength
18009b3bf: 48 8b cb                    	mov	rcx, rbx
18009b3c2: e8 39 83 06 00              	call	0x180103700 <NVSDK_NGX_Parameter_SetF>
18009b3c7: f3 0f 10 97 ec 00 00 00     	movss	xmm2, dword ptr [rdi + 0xec]
18009b3cf: 48 8d 15 d2 88 10 01        	lea	rdx, [rip + 0x11088d2]  # 0x1811a3ca8    ; STRING: DLSSNR.SkinStructureStrength
18009b3d6: 48 8b cb                    	mov	rcx, rbx
18009b3d9: e8 22 83 06 00              	call	0x180103700 <NVSDK_NGX_Parameter_SetF>
18009b3de: 44 8b 87 f0 00 00 00        	mov	r8d, dword ptr [rdi + 0xf0]
18009b3e5: 48 8d 15 0c 89 10 01        	lea	rdx, [rip + 0x110890c]  # 0x1811a3cf8    ; STRING: DLSSNR.UseAutoMask
18009b3ec: 48 8b cb                    	mov	rcx, rbx
18009b3ef: e8 5c 83 06 00              	call	0x180103750 <NVSDK_NGX_Parameter_SetI>
18009b3f4: 44 8b 87 f4 00 00 00        	mov	r8d, dword ptr [rdi + 0xf4]
18009b3fb: 48 8d 15 e6 88 10 01        	lea	rdx, [rip + 0x11088e6]  # 0x1811a3ce8    ; STRING: DLSSNR.Style
18009b402: 48 8b cb                    	mov	rcx, rbx
18009b405: e8 a6 83 06 00              	call	0x1801037b0 <NVSDK_NGX_Parameter_SetUI>
18009b40a: 44 8b 87 f8 00 00 00        	mov	r8d, dword ptr [rdi + 0xf8]
18009b411: 48 8d 15 10 89 10 01        	lea	rdx, [rip + 0x1108910]  # 0x1811a3d28    ; STRING: DLSSNR.Reset
18009b418: 48 8b cb                    	mov	rcx, rbx
18009b41b: e8 30 83 06 00              	call	0x180103750 <NVSDK_NGX_Parameter_SetI>
18009b420: 44 8b 87 fc 00 00 00        	mov	r8d, dword ptr [rdi + 0xfc]
18009b427: 48 8d 15 e2 88 10 01        	lea	rdx, [rip + 0x11088e2]  # 0x1811a3d10    ; STRING: DLSSNR.DepthInverted
18009b42e: 48 8b cb                    	mov	rcx, rbx
18009b431: e8 1a 83 06 00              	call	0x180103750 <NVSDK_NGX_Parameter_SetI>
18009b436: 44 8b 87 00 01 00 00        	mov	r8d, dword ptr [rdi + 0x100]
18009b43d: 48 8d 15 0c 89 10 01        	lea	rdx, [rip + 0x110890c]  # 0x1811a3d50    ; STRING: DLSSNR.Enabled
18009b444: 48 8b cb                    	mov	rcx, rbx
18009b447: e8 04 83 06 00              	call	0x180103750 <NVSDK_NGX_Parameter_SetI>
18009b44c: 44 8b 87 04 01 00 00        	mov	r8d, dword ptr [rdi + 0x104]
18009b453: 48 8d 15 de 88 10 01        	lea	rdx, [rip + 0x11088de]  # 0x1811a3d38    ; STRING: DLSSNR.UICorrection
18009b45a: 48 8b cb                    	mov	rcx, rbx
18009b45d: e8 ee 82 06 00              	call	0x180103750 <NVSDK_NGX_Parameter_SetI>
18009b462: 44 8b 87 08 01 00 00        	mov	r8d, dword ptr [rdi + 0x108]
18009b469: 48 8d 15 b8 81 10 01        	lea	rdx, [rip + 0x11081b8]  # 0x1811a3628    ; STRING: DLSS.Indicator.Invert.X.Axis
18009b470: 48 8b cb                    	mov	rcx, rbx
18009b473: e8 d8 82 06 00              	call	0x180103750 <NVSDK_NGX_Parameter_SetI>
18009b478: 44 8b 87 0c 01 00 00        	mov	r8d, dword ptr [rdi + 0x10c]
18009b47f: 48 8d 15 82 81 10 01        	lea	rdx, [rip + 0x1108182]  # 0x1811a3608    ; STRING: DLSS.Indicator.Invert.Y.Axis
18009b486: 48 8b cb                    	mov	rcx, rbx
18009b489: e8 c2 82 06 00              	call	0x180103750 <NVSDK_NGX_Parameter_SetI>
18009b48e: 4c 8b c3                    	mov	r8, rbx
18009b491: 48 8b d6                    	mov	rdx, rsi
18009b494: 48 8b cd                    	mov	rcx, rbp
18009b497: 48 8b 5c 24 50              	mov	rbx, qword ptr [rsp + 0x50]
18009b49c: 48 8b 6c 24 58              	mov	rbp, qword ptr [rsp + 0x58]
18009b4a1: 48 8b 74 24 60              	mov	rsi, qword ptr [rsp + 0x60]
18009b4a6: 0f 28 74 24 30              	movaps	xmm6, xmmword ptr [rsp + 0x30]
18009b4ab: 0f 28 7c 24 20              	movaps	xmm7, xmmword ptr [rsp + 0x20]
18009b4b0: 48 83 c4 40                 	add	rsp, 0x40
18009b4b4: 5f                          	pop	rdi
18009b4b5: e9 86 8d 00 00              	jmp	0x1800a4240 <SetPDFrameWarpNativeCameraSource+0x2b9b0>