; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xEE680..0xEE803; unnamed
000EE680: mov       QWORD PTR [rsp+0x18],rbx
000EE685: mov       QWORD PTR [rsp+0x8],rcx
000EE68A: push      rsi
000EE68B: push      rdi
000EE68C: push      r14
000EE68E: sub       rsp,0x20
000EE692: mov       rdi,r9
000EE695: mov       rsi,r8
000EE698: mov       rbx,rdx
000EE69B: mov       r14,rcx
000EE69E: call      0x18007a900
000EE6A3: nop       
000EE6A4: lea       rax,[rip+0x10bb87d]        # 0x1811a9f28
000EE6AB: mov       QWORD PTR [r14],rax
000EE6AE: mov       QWORD PTR [r14+0x150],rbx
000EE6B5: mov       QWORD PTR [r14+0x158],rdi
000EE6BC: mov       QWORD PTR [r14+0x160],rsi
000EE6C3: xor       eax,eax
000EE6C5: mov       QWORD PTR [r14+0x168],rax
000EE6CC: mov       QWORD PTR [r14+0x170],rax
000EE6D3: mov       QWORD PTR [r14+0x178],rax
000EE6DA: mov       QWORD PTR [r14+0x180],rax
000EE6E1: mov       QWORD PTR [r14+0x188],rax
000EE6E8: mov       QWORD PTR [r14+0x190],rax
000EE6EF: mov       QWORD PTR [r14+0x198],rax
000EE6F6: mov       QWORD PTR [r14+0x1a0],rax
000EE6FD: mov       QWORD PTR [r14+0x1a8],rax
000EE704: mov       QWORD PTR [r14+0x1b0],rax
000EE70B: mov       QWORD PTR [r14+0x1b8],rax
000EE712: mov       QWORD PTR [r14+0x1c0],rax
000EE719: mov       WORD PTR [r14+0x1c8],ax
000EE721: mov       QWORD PTR [r14+0x1d0],rax
000EE728: lea       rcx,[r14+0x1d8]
000EE72F: mov       DWORD PTR [rcx],eax
000EE731: mov       QWORD PTR [rcx+0x8],rax
000EE735: mov       QWORD PTR [rcx+0x10],rax
000EE739: mov       QWORD PTR [rcx+0x18],rax
000EE73D: mov       QWORD PTR [rcx+0x20],rax
000EE741: mov       QWORD PTR [rcx+0x28],rax
000EE745: mov       WORD PTR [rcx+0x30],ax
000EE749: add       rcx,0x38
000EE74D: call      0x180080280
000EE752: nop       
000EE753: mov       BYTE PTR [r14+0x810],0x0
000EE75B: mov       DWORD PTR [r14+0x118],0x1
000EE766: mov       DWORD PTR [r14+0x148],0x3
000EE771: call      0x1800cc790
000EE776: mov       rbx,rax
000EE779: cmp       BYTE PTR [rax+0xca],0x0
000EE780: jne       0x1800ee78a
000EE782: mov       rcx,rax
000EE785: call      0x1800cc8c0
000EE78A: cmp       BYTE PTR [rbx+0xcb],0x0
000EE791: je        0x1800ee7a7
000EE793: cmp       BYTE PTR [rbx+0xcc],0x0
000EE79A: je        0x1800ee7a7
000EE79C: mov       DWORD PTR [r14+0x148],0x4
000EE7A7: mov       ecx,DWORD PTR [rsp+0x60]
000EE7AB: mov       DWORD PTR [r14+0x8],ecx
000EE7AF: test      ecx,ecx
000EE7B1: je        0x1800ee7cf
000EE7B3: sub       ecx,0x1
000EE7B6: je        0x1800ee7c6
000EE7B8: cmp       ecx,0x1
000EE7BB: jne       0x1800ee7db
000EE7BD: lea       rcx,[rip+0x10bb304]        # 0x1811a9ac8 ; 'Creating FrameGenMethod_FSR3_FFXAPI Graphic API: VULKAN'
000EE7C4: jmp       0x1800ee7d6
000EE7C6: lea       rcx,[rip+0x10bb26b]        # 0x1811a9a38 ; 'Creating FrameGenMethod_FSR3_FFXAPI Graphic API: D3D12'
000EE7CD: jmp       0x1800ee7d6
000EE7CF: lea       rcx,[rip+0x10bb29a]        # 0x1811a9a70 ; 'Creating FrameGenMethod_FSR3_FFXAPI Graphic API: D3D11'
000EE7D6: call      0x1800fbb40
000EE7DB: cmp       DWORD PTR [r14+0x148],0x4
000EE7E3: jne       0x1800ee7f2
000EE7E5: lea       rcx,[rip+0x10bb2bc]        # 0x1811a9aa8 ; 'Using FSR4 Frame Generation.'
000EE7EC: call      0x1800fbb40
000EE7F1: nop       
000EE7F2: mov       rax,r14
000EE7F5: mov       rbx,QWORD PTR [rsp+0x50]
000EE7FA: add       rsp,0x20
000EE7FE: pop       r14
000EE800: pop       rdi
000EE801: pop       rsi
000EE802: ret       
