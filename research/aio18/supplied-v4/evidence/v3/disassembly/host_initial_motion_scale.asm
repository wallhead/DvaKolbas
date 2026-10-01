; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x2A0F41..0x2A1013; unnamed
002A0F41: xor       r8d,r8d
002A0F44: mov       rax,QWORD PTR [rcx]
002A0F47: call      QWORD PTR [rax+0x28]
002A0F4A: mov       DWORD PTR [rbp+0x40],eax
002A0F4D: test      eax,eax
002A0F4F: jns       0x1802a0fbc
002A0F51: call      0x180222050
002A0F56: lea       rcx,[rip+0x1651d3]        # 0x180406130
002A0F5D: mov       DWORD PTR [rbp-0x78],0xc8b
002A0F64: mov       QWORD PTR [rbp-0x80],rcx
002A0F68: lea       r9,[rsp+0x70]
002A0F6D: mov       ecx,DWORD PTR [rbp-0x24]
002A0F70: lea       rdx,[rbp-0x30]
002A0F74: mov       DWORD PTR [rbp-0x74],ecx
002A0F77: lea       rcx,[rip+0x16666a]        # 0x1804075e8
002A0F7E: vmovups   xmm0,XMMWORD PTR [rbp-0x80]
002A0F83: mov       QWORD PTR [rbp-0x70],rcx
002A0F87: lea       rcx,[rip+0x1666f2]        # 0x180407680
002A0F8E: vmovsd    xmm1,QWORD PTR [rbp-0x70]
002A0F93: mov       QWORD PTR [rsp+0x70],rcx
002A0F98: lea       rcx,[rbp+0x40]
002A0F9C: mov       QWORD PTR [rsp+0x20],rcx
002A0FA1: mov       rcx,rax
002A0FA4: mov       QWORD PTR [rsp+0x78],0x27
002A0FAD: vmovups   XMMWORD PTR [rbp-0x30],xmm0
002A0FB2: vmovsd    QWORD PTR [rbp-0x20],xmm1
002A0FB7: call      0x180196e10
002A0FBC: vxorps    xmm1,xmm1,xmm1
002A0FC0: vcvtsi2ss xmm1,xmm1,DWORD PTR [rbx+0x278]
002A0FC8: vxorps    xmm0,xmm0,xmm0
002A0FCC: vcvtsi2ss xmm0,xmm0,DWORD PTR [rbx+0x27c]
002A0FD4: xor       ecx,ecx
002A0FD6: vmovss    DWORD PTR [rbx+0x10],xmm1
002A0FDB: vmovss    DWORD PTR [rbx+0x14],xmm0
002A0FE0: call      QWORD PTR [rip+0x1d1c2a]        # 0x180472c10 ; PDPerfPlugin.dll!SetMotionScaleX
002A0FE6: vmovss    xmm1,DWORD PTR [rbx+0x14]
002A0FEB: xor       ecx,ecx
002A0FED: call      QWORD PTR [rip+0x1d1c25]        # 0x180472c18 ; PDPerfPlugin.dll!SetMotionScaleY
002A0FF3: cmp       BYTE PTR [rbx],0x0
002A0FF6: je        0x1802a1015
002A0FF8: cmp       BYTE PTR [rbx+0x343],0x0
002A0FFF: jne       0x1802a1015
002A1001: vmovss    xmm6,DWORD PTR [rbx+0x290]
002A1009: call      0x180152f00
002A100E: vmovss    DWORD PTR [rax+0x8],xmm6
