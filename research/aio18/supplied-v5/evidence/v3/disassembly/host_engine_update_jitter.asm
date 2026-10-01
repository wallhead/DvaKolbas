; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x19FD90..0x19FFBF; unnamed
0019FD90: rex       push rbx
0019FD92: push      rbp
0019FD93: push      rdi
0019FD94: sub       rsp,0xe0
0019FD9B: mov       eax,DWORD PTR [rip+0x2bf74f]        # 0x18045f4f0
0019FDA1: xor       ebp,ebp
0019FDA3: and       eax,0xb8
0019FDA8: mov       rdi,rcx
0019FDAB: cmp       al,0xb8
0019FDAD: jne       0x18019fe8a
0019FDB3: mov       rax,QWORD PTR [rip+0xce1026]        # 0x180e80de0
0019FDBA: mov       QWORD PTR [rsp+0x110],rsi
0019FDC2: mov       esi,DWORD PTR [rax+0x460]
0019FDC8: mov       eax,DWORD PTR [rip+0x2bf722]        # 0x18045f4f0
0019FDCE: and       eax,0xb8
0019FDD3: cmp       al,0xb8
0019FDD5: jne       0x18019fe82
0019FDDB: cmp       DWORD PTR [rip+0x2d591f],ebp        # 0x180475700
0019FDE1: je        0x18019fe82
0019FDE7: call      0x18024ab10
0019FDEC: call      0x1801503a0
0019FDF1: mov       rcx,rax
0019FDF4: mov       rbx,rax
0019FDF7: call      0x180258fc0
0019FDFC: cmp       BYTE PTR [rip+0x2d596d],bpl        # 0x180475770
0019FE03: je        0x18019fe0d
0019FE05: cmp       DWORD PTR [rip+0x2d5955],esi        # 0x180475760
0019FE0B: je        0x18019fe82
0019FE0D: cmp       BYTE PTR [rip+0x2d593c],bpl        # 0x180475750
0019FE14: je        0x18019fe1c
0019FE16: call      QWORD PTR [rip+0x2d2da4]        # 0x180472bc0 ; PDPerfPlugin.dll!CancelPDFrameWarpFrame
0019FE1C: xor       ecx,ecx
0019FE1E: mov       BYTE PTR [rip+0x2d594b],0x1        # 0x180475770
0019FE25: mov       DWORD PTR [rip+0x2d5935],esi        # 0x180475760
0019FE2B: mov       BYTE PTR [rip+0x2d58de],bpl        # 0x180475710
0019FE32: mov       BYTE PTR [rip+0x2d58e8],bpl        # 0x180475721
0019FE39: mov       BYTE PTR [rip+0x2d5910],bpl        # 0x180475750
0019FE40: mov       QWORD PTR [rip+0x2d58e9],rbp        # 0x180475730
0019FE47: call      0x180259d30
0019FE4C: mov       rcx,rbx
0019FE4F: call      0x180259ed0
0019FE54: movzx     ecx,al
0019FE57: movzx     esi,al
0019FE5A: call      0x180259ff0
0019FE5F: cmp       DWORD PTR [rip+0x2d589a],0x2        # 0x180475700
0019FE66: jne       0x18019fe82
0019FE68: test      sil,sil
0019FE6B: je        0x18019fe82
0019FE6D: cmp       BYTE PTR [rbx+0x4e4],bpl
0019FE74: je        0x18019fe82
0019FE76: call      QWORD PTR [rip+0x2d2d14]        # 0x180472b90 ; PDPerfPlugin.dll!TryBeginPDFrameWarpFrame
0019FE7C: mov       BYTE PTR [rip+0x2d58ce],al        # 0x180475750
0019FE82: mov       rsi,QWORD PTR [rsp+0x110]
0019FE8A: mov       rcx,rdi
0019FE8D: call      QWORD PTR [rip+0x2dd525]        # 0x18047d3b8
0019FE93: mov       rbx,QWORD PTR [rip+0xce0f46]        # 0x180e80de0
0019FE9A: mov       QWORD PTR [rbx+0x4f0],rdi
0019FEA1: cmp       BYTE PTR [rbx+0x343],bpl
0019FEA8: jne       0x18019ffb4
0019FEAE: cmp       BYTE PTR [rbx],bpl
0019FEB1: je        0x18019ff80
0019FEB7: cmp       BYTE PTR [rbx+0x26c],bpl
0019FEBE: je        0x18019ff80
0019FEC4: vxorps    xmm0,xmm0,xmm0
0019FEC8: xor       ecx,ecx
0019FECA: vmovss    DWORD PTR [rsp+0x100],xmm0
0019FED3: vmovss    DWORD PTR [rsp+0x108],xmm0
0019FEDC: call      QWORD PTR [rip+0x2d2d8e]        # 0x180472c70 ; PDPerfPlugin.dll!GetJitterPhaseCount
0019FEE2: vmovss    xmm0,DWORD PTR [rbx+0x4]
0019FEE7: vaddss    xmm1,xmm0,DWORD PTR [rip+0x26be45]        # 0x18040bd34
0019FEEF: vcvttss2si r8d,xmm1
0019FEF3: mov       r9d,eax
0019FEF6: lea       rdx,[rsp+0x108]
0019FEFE: lea       rcx,[rsp+0x100]
0019FF06: vmovss    DWORD PTR [rbx+0x4],xmm1
0019FF0B: call      QWORD PTR [rip+0x2d2d67]        # 0x180472c78 ; PDPerfPlugin.dll!GetJitterOffset
0019FF11: mov       rax,QWORD PTR [rip+0xce0ec8]        # 0x180e80de0
0019FF18: vmovss    xmm4,DWORD PTR [rsp+0x100]
0019FF21: vmovss    xmm3,DWORD PTR [rsp+0x108]
0019FF2A: vmulss    xmm1,xmm4,DWORD PTR [rip+0x26c006]        # 0x18040bf38
0019FF32: vxorps    xmm0,xmm0,xmm0
0019FF36: vcvtsi2ss xmm0,xmm0,DWORD PTR [rax+0x278]
0019FF3E: vdivss    xmm1,xmm1,xmm0
0019FF42: vmulss    xmm0,xmm3,DWORD PTR [rip+0x26be6e]        # 0x18040bdb8
0019FF4A: vxorps    xmm3,xmm3,XMMWORD PTR [rip+0x26c98e]        # 0x18040c8e0
0019FF52: vxorps    xmm2,xmm2,xmm2
0019FF56: vcvtsi2ss xmm2,xmm2,DWORD PTR [rax+0x27c]
0019FF5E: vmovss    DWORD PTR [rdi+0x44],xmm1
0019FF63: vdivss    xmm1,xmm0,xmm2
0019FF67: vxorps    xmm0,xmm4,XMMWORD PTR [rip+0x26c971]        # 0x18040c8e0
0019FF6F: vmovss    DWORD PTR [rdi+0x48],xmm1
0019FF74: vmovss    DWORD PTR [rax+0x8],xmm0
0019FF79: vmovss    DWORD PTR [rax+0xc],xmm3
0019FF7E: jmp       0x18019ff88
0019FF80: mov       QWORD PTR [rdi+0x44],rbp
0019FF84: mov       QWORD PTR [rbx+0x8],rbp
0019FF88: mov       rcx,rdi
0019FF8B: call      0x1801698b0
0019FF90: mov       rdx,rax
0019FF93: lea       rcx,[rsp+0x20]
0019FF98: call      0x1801a25b0
0019FF9D: cmp       DWORD PTR [rsp+0x78],0x1
0019FFA2: jbe       0x18019ffaa
0019FFA4: mov       eax,DWORD PTR [rip+0x2bf546]        # 0x18045f4f0
0019FFAA: lea       rcx,[rsp+0x20]
0019FFAF: call      0x1801a27f0
0019FFB4: add       rsp,0xe0
0019FFBB: pop       rdi
0019FFBC: pop       rbp
0019FFBD: pop       rbx
0019FFBE: ret       
