; amd_fidelityfx_framegeneration_dx12.dll SHA256=3f5e674a59b400756e98ef31fd583a4eb08ad567ec8dee362886bc96e55ed347
; ImageBase=0x180000000, RVAs in left column
; range 0xF0BD0..0xF1F6D; ffxProvider_MLFrameGeneration::Dispatch
000F0BD0: mov       QWORD PTR [rsp+0x8],rbx
000F0BD5: push      rbp
000F0BD6: push      rsi
000F0BD7: push      rdi
000F0BD8: push      r12
000F0BDA: push      r13
000F0BDC: push      r14
000F0BDE: push      r15
000F0BE0: lea       rbp,[rsp-0x9dd0]
000F0BE8: mov       eax,0x9ed0
000F0BED: call      0x180105750 ; __chkstk
000F0BF2: sub       rsp,rax
000F0BF5: mov       rax,QWORD PTR [rip+0x237d404]        # 0x18246e000 ; __security_cookie
000F0BFC: xor       rax,rsp
000F0BFF: mov       QWORD PTR [rbp+0x9d60],rax
000F0C06: mov       rdi,r8
000F0C09: test      rdx,rdx
000F0C0C: je        0x1800f1e76
000F0C12: mov       r14,QWORD PTR [rdx]
000F0C15: test      r14,r14
000F0C18: je        0x1800f1e76
000F0C1E: test      r8,r8
000F0C21: je        0x1800f1e76
000F0C27: mov       r8,QWORD PTR [r8]
000F0C2A: mov       rax,r8
000F0C2D: sub       rax,0x20003
000F0C33: je        0x1800f198d
000F0C39: sub       rax,0x1
000F0C3D: je        0x1800f0c53
000F0C3F: sub       rax,0x6
000F0C43: je        0x1800f0e03
000F0C49: cmp       rax,0x2
000F0C4D: jne       0x1800f1e76
000F0C53: mov       eax,DWORD PTR [rdi+0x10]
000F0C56: mov       r15d,0x1
000F0C5C: movups    xmm0,XMMWORD PTR [rdi+0x10]
000F0C60: and       eax,0x1
000F0C63: movaps    XMMWORD PTR [rsp+0x9ec0],xmm6
000F0C6B: imul      rbx,rax,0xe8
000F0C72: movaps    XMMWORD PTR [rsp+0x9eb0],xmm7
000F0C7A: movaps    XMMWORD PTR [rsp+0x9ea0],xmm8
000F0C83: movaps    XMMWORD PTR [rsp+0x9e90],xmm9
000F0C8C: movaps    XMMWORD PTR [rsp+0x9e80],xmm10
000F0C95: movaps    XMMWORD PTR [rsp+0x9e70],xmm11
000F0C9E: add       rbx,r14
000F0CA1: lea       rdx,[rbx+0x88]
000F0CA8: movups    XMMWORD PTR [rdx],xmm0
000F0CAB: movups    xmm1,XMMWORD PTR [rdi+0x20]
000F0CAF: movups    XMMWORD PTR [rdx+0x10],xmm1
000F0CB3: movups    xmm0,XMMWORD PTR [rdi+0x30]
000F0CB7: movups    XMMWORD PTR [rdx+0x20],xmm0
000F0CBB: movups    xmm1,XMMWORD PTR [rdi+0x40]
000F0CBF: movups    XMMWORD PTR [rdx+0x30],xmm1
000F0CC3: movups    xmm0,XMMWORD PTR [rdi+0x50]
000F0CC7: movups    XMMWORD PTR [rdx+0x40],xmm0
000F0CCB: movups    xmm1,XMMWORD PTR [rdi+0x60]
000F0CCF: movups    XMMWORD PTR [rdx+0x50],xmm1
000F0CD3: movups    xmm0,XMMWORD PTR [rdi+0x70]
000F0CD7: movups    XMMWORD PTR [rdx+0x60],xmm0
000F0CDB: movups    xmm0,XMMWORD PTR [rdi+0x80]
000F0CE2: movups    XMMWORD PTR [rdx+0x70],xmm0
000F0CE6: movups    xmm1,XMMWORD PTR [rdi+0x90]
000F0CED: movups    XMMWORD PTR [rdx+0x80],xmm1
000F0CF4: movups    xmm0,XMMWORD PTR [rdi+0xa0]
000F0CFB: movups    XMMWORD PTR [rdx+0x90],xmm0
000F0D02: mov       rax,QWORD PTR [rdi+0xb0]
000F0D09: mov       QWORD PTR [rdx+0xa0],rax
000F0D10: cmp       r8,0x20004
000F0D17: jne       0x1800f0ead
000F0D1D: mov       BYTE PTR [rbx+0xbc],0x0
000F0D24: mov       rcx,rdi
000F0D27: cmp       QWORD PTR [rcx],0x2000a
000F0D2E: jne       0x1800f0d97
000F0D30: movsd     xmm0,QWORD PTR [rcx+0x10]
000F0D35: movsd     QWORD PTR [r14+0xd6d51c],xmm0
000F0D3E: mov       eax,DWORD PTR [rcx+0x18]
000F0D41: mov       DWORD PTR [r14+0xd6d524],eax
000F0D48: movsd     xmm0,QWORD PTR [rcx+0x1c]
000F0D4D: movsd     QWORD PTR [r14+0xd6d528],xmm0
000F0D56: mov       eax,DWORD PTR [rcx+0x24]
000F0D59: mov       DWORD PTR [r14+0xd6d530],eax
000F0D60: movsd     xmm0,QWORD PTR [rcx+0x28]
000F0D65: movsd     QWORD PTR [r14+0xd6d534],xmm0
000F0D6E: mov       eax,DWORD PTR [rcx+0x30]
000F0D71: mov       DWORD PTR [r14+0xd6d53c],eax
000F0D78: movsd     xmm0,QWORD PTR [rcx+0x34]
000F0D7D: movsd     QWORD PTR [r14+0xd6d540],xmm0
000F0D86: mov       eax,DWORD PTR [rcx+0x3c]
000F0D89: mov       DWORD PTR [r14+0xd6d548],eax
000F0D90: mov       BYTE PTR [r14+0xd6d519],r15b
000F0D97: mov       rcx,QWORD PTR [rcx+0x8]
000F0D9B: test      rcx,rcx
000F0D9E: jne       0x1800f0d27
000F0DA0: test      BYTE PTR [r14+0xd6d68c],0x40
000F0DA8: je        0x1800f0f2c
000F0DAE: cmp       BYTE PTR [rip+0x23815f4],cl        # 0x1824723a8 ; bOnce
000F0DB4: jne       0x1800f0dcc
000F0DB6: lea       rdx,[rip+0x2375bd3]        # 0x182466990 ; 'ffxDispatchDescFrameGenerationPrepare is deprecated, update to ffxDispatchDescFrameGenerationPrepareV2.'
000F0DBD: mov       ecx,r15d
000F0DC0: call      0x1800120a0 ; ffxPrintMessage
000F0DC5: mov       BYTE PTR [rip+0x23815dc],r15b        # 0x1824723a8 ; bOnce
000F0DCC: cmp       BYTE PTR [rdi+0x44],0x0
000F0DD0: je        0x1800f0de1
000F0DD2: lea       rdx,[rip+0x2375c87]        # 0x182466a60 ; 'ffxDispatchDescFrameGenerationPrepare::unused_reset was never implemented and will be ignored, update to ffxDispatchDescFrameGenerationPrepareV2::reset.'
000F0DD9: mov       ecx,r15d
000F0DDC: call      0x1800120a0 ; ffxPrintMessage
000F0DE1: cmp       BYTE PTR [r14+0xd6d519],0x0
000F0DE9: jne       0x1800f0f2c
000F0DEF: lea       rdx,[rip+0x2375daa]        # 0x182466ba0 ; 'ffxDispatchDescFrameGenerationPrepareCameraInfo is not linked to ffxDispatchDescFrameGenerationPrepare.'
000F0DF6: mov       ecx,r15d
000F0DF9: call      0x1800120a0 ; ffxPrintMessage
000F0DFE: jmp       0x1800f0f2c
000F0E03: test      BYTE PTR [r14+0xd6d68c],0x40
000F0E0B: movsd     xmm0,QWORD PTR [rdi+0x10]
000F0E10: movsd     QWORD PTR [r14+0xd6d51c],xmm0
000F0E19: mov       eax,DWORD PTR [rdi+0x18]
000F0E1C: mov       DWORD PTR [r14+0xd6d524],eax
000F0E23: movsd     xmm0,QWORD PTR [rdi+0x1c]
000F0E28: movsd     QWORD PTR [r14+0xd6d528],xmm0
000F0E31: mov       eax,DWORD PTR [rdi+0x24]
000F0E34: mov       DWORD PTR [r14+0xd6d530],eax
000F0E3B: movsd     xmm0,QWORD PTR [rdi+0x28]
000F0E40: movsd     QWORD PTR [r14+0xd6d534],xmm0
000F0E49: mov       eax,DWORD PTR [rdi+0x30]
000F0E4C: mov       DWORD PTR [r14+0xd6d53c],eax
000F0E53: movsd     xmm0,QWORD PTR [rdi+0x34]
000F0E58: movsd     QWORD PTR [r14+0xd6d540],xmm0
000F0E61: mov       eax,DWORD PTR [rdi+0x3c]
000F0E64: mov       DWORD PTR [r14+0xd6d548],eax
000F0E6B: mov       BYTE PTR [r14+0xd6d519],0x1
000F0E73: je        0x1800f1e76
000F0E79: cmp       BYTE PTR [rip+0x2381529],0x0        # 0x1824723a9
000F0E80: mov       r15d,0x1
000F0E86: jne       0x1800f0e9e
000F0E88: lea       rdx,[rip+0x2376081]        # 0x182466f10 ; 'ffxDispatchDescFrameGenerationPrepareCameraInfo is deprecated, update to ffxDispatchDescFrameGenerationPrepareV2.'
000F0E8F: mov       ecx,r15d
000F0E92: call      0x1800120a0 ; ffxPrintMessage
000F0E97: mov       BYTE PTR [rip+0x238150b],r15b        # 0x1824723a9
000F0E9E: lea       rdx,[rip+0x237615b]        # 0x182467000 ; 'ffxDispatchDescFrameGenerationPrepareCameraInfo struct is not a linked struct during dispatch ffxDispatchDescFrameGenerationPrepare. API expects ffx::Dispatch(fgContext, ffxDispatchDescFrameGenerationPrepare, ffxDispatchDescFrameGenerationPrepare). MLFI conditionally supports this nonstandard API usage to maintain compatibility with shipped titles.'
000F0EA5: mov       ecx,r15d
000F0EA8: jmp       0x1800f1e71
000F0EAD: movsd     xmm0,QWORD PTR [rdi+0xb8]
000F0EB5: movsd     QWORD PTR [r14+0xd6d51c],xmm0
000F0EBE: mov       eax,DWORD PTR [rdi+0xc0]
000F0EC4: mov       DWORD PTR [r14+0xd6d524],eax
000F0ECB: movsd     xmm0,QWORD PTR [rdi+0xc4]
000F0ED3: movsd     QWORD PTR [r14+0xd6d528],xmm0
000F0EDC: mov       eax,DWORD PTR [rdi+0xcc]
000F0EE2: mov       DWORD PTR [r14+0xd6d530],eax
000F0EE9: movsd     xmm0,QWORD PTR [rdi+0xd0]
000F0EF1: movsd     QWORD PTR [r14+0xd6d534],xmm0
000F0EFA: mov       eax,DWORD PTR [rdi+0xd8]
000F0F00: mov       DWORD PTR [r14+0xd6d53c],eax
000F0F07: movsd     xmm0,QWORD PTR [rdi+0xdc]
000F0F0F: movsd     QWORD PTR [r14+0xd6d540],xmm0
000F0F18: mov       eax,DWORD PTR [rdi+0xe4]
000F0F1E: mov       DWORD PTR [r14+0xd6d548],eax
000F0F25: mov       BYTE PTR [r14+0xd6d519],r15b
000F0F2C: test      BYTE PTR [r14+0xd6d68c],0x40
000F0F34: mov       eax,0x78
000F0F39: lea       r13,[rax+rbx*1]
000F0F3D: je        0x1800f0f58
000F0F3F: cmp       BYTE PTR [r14+0xd6d518],0x0
000F0F47: jne       0x1800f0f58
000F0F49: lea       rdx,[rip+0x2375d20]        # 0x182466c70 ; 'ffxConfigure must be called before ffxDispatchDescFrameGenerationPrepare.'
000F0F50: mov       ecx,r15d
000F0F53: call      0x1800120a0 ; ffxPrintMessage
000F0F58: mov       esi,DWORD PTR [r14+0xd6d68c]
000F0F5F: mov       r12d,esi
000F0F62: movss     xmm0,DWORD PTR [r13+0x48]
000F0F68: and       r12d,0x8
000F0F6C: movss     xmm7,DWORD PTR [r13+0x4c]
000F0F72: movaps    xmm8,xmm0
000F0F76: movss     xmm9,DWORD PTR [r13+0x50]
000F0F7C: setne     BYTE PTR [rsp+0x50]
000F0F81: and       esi,0x10
000F0F84: comiss    xmm0,xmm7
000F0F87: jbe       0x1800f0f90
000F0F89: movaps    xmm8,xmm7
000F0F8D: movaps    xmm7,xmm0
000F0F90: mov       eax,DWORD PTR [r13+0x28]
000F0F94: lea       r9,[r14+0x408]
000F0F9B: mov       r8d,DWORD PTR [r14+0x37c]
000F0FA2: xorps     xmm0,xmm0
000F0FA5: xorps     xmm6,xmm6
000F0FA8: cvtsi2ss  xmm6,rax
000F0FAD: mov       eax,DWORD PTR [r13+0x2c]
000F0FB1: cvtsi2ss  xmm0,rax
000F0FB6: mov       eax,0xaaaaaaab
000F0FBB: mul       DWORD PTR [r14+0xd6d690]
000F0FC2: divss     xmm6,xmm0
000F0FC6: mov       eax,0xaaaaaaab
000F0FCB: shr       edx,1
000F0FCD: mov       DWORD PTR [rsp+0x5c],edx
000F0FD1: movups    xmm0,XMMWORD PTR [r13+0x88]
000F0FD9: mul       DWORD PTR [r14+0xd6d694]
000F0FE0: movups    XMMWORD PTR [rbp-0x20],xmm0
000F0FE4: movups    xmm1,XMMWORD PTR [r13+0x98]
000F0FEC: movups    XMMWORD PTR [rbp-0x10],xmm1
000F0FF0: shr       edx,1
000F0FF2: movups    xmm0,XMMWORD PTR [r13+0xa8]
000F0FFA: mov       DWORD PTR [rsp+0x54],edx
000F0FFE: lea       rdx,[rbp-0x20]
000F1002: movups    XMMWORD PTR [rbp+0x0],xmm0
000F1006: movups    xmm1,XMMWORD PTR [r13+0x58]
000F100B: movups    XMMWORD PTR [rbp+0x10],xmm1
000F100F: movups    xmm0,XMMWORD PTR [r13+0x68]
000F1014: movups    XMMWORD PTR [rbp+0x20],xmm0
000F1018: movups    xmm1,XMMWORD PTR [r13+0x78]
000F101D: movups    XMMWORD PTR [rbp+0x30],xmm1
000F1021: mov       ecx,DWORD PTR [r13+0x10]
000F1025: and       ecx,r15d
000F1028: mov       eax,DWORD PTR [r14+rcx*4+0x42c]
000F1030: mov       DWORD PTR [r14+0x428],eax
000F1037: mov       eax,DWORD PTR [r14+rcx*4+0x42c]
000F103F: mov       DWORD PTR [r14+0x4a4],eax
000F1046: mov       eax,DWORD PTR [r14+rcx*4+0x438]
000F104E: mov       DWORD PTR [r14+0x434],eax
000F1055: mov       eax,DWORD PTR [r14+rcx*4+0x438]
000F105D: lea       rcx,[r14+0x2a0]
000F1064: mov       DWORD PTR [r14+0x4b0],eax
000F106B: mov       rax,QWORD PTR [r14+0x2d0]
000F1072: call      rax
000F1074: mov       rax,QWORD PTR [r14+0x2d0]
000F107B: lea       r9,[r14+0x40c]
000F1082: mov       r8d,DWORD PTR [r14+0x37c]
000F1089: lea       rdx,[rbp+0x10]
000F108D: lea       rcx,[r14+0x2a0]
000F1094: call      rax
000F1096: movss     xmm0,DWORD PTR [r14+0xd6d534]
000F109F: movss     xmm1,DWORD PTR [r14+0xd6d538]
000F10A8: mov       eax,DWORD PTR [r14+0x48c]
000F10AF: mov       edi,DWORD PTR [r14+0x450]
000F10B6: mov       ebx,DWORD PTR [r14+0x414]
000F10BD: movss     DWORD PTR [rbp+0x9c60],xmm0
000F10C5: movss     xmm0,DWORD PTR [r14+0xd6d53c]
000F10CE: mov       DWORD PTR [rsp+0x60],eax
000F10D2: mov       eax,DWORD PTR [r14+0x4c4]
000F10D9: movss     DWORD PTR [rbp+0x9c64],xmm1
000F10E1: movss     xmm1,DWORD PTR [r14+0xd6d528]
000F10EA: movss     DWORD PTR [rbp+0x9c68],xmm0
000F10F2: movss     xmm0,DWORD PTR [r14+0xd6d52c]
000F10FB: mov       DWORD PTR [rbp-0x80],eax
000F10FE: mov       eax,DWORD PTR [r14+0x494]
000F1105: movss     DWORD PTR [rbp+0x9c70],xmm1
000F110D: movss     xmm1,DWORD PTR [r14+0xd6d530]
000F1116: movss     DWORD PTR [rbp+0x9c74],xmm0
000F111E: movss     xmm0,DWORD PTR [r14+0xd6d540]
000F1127: mov       DWORD PTR [rsp+0x58],eax
000F112B: mov       eax,DWORD PTR [r14+0x4b0]
000F1132: movss     DWORD PTR [rbp+0x9c78],xmm1
000F113A: movss     xmm1,DWORD PTR [r14+0xd6d544]
000F1143: mov       DWORD PTR [rsp+0x68],eax
000F1147: mov       eax,DWORD PTR [r14+0x428]
000F114E: movss     DWORD PTR [rbp+0x9c80],xmm0
000F1156: movss     xmm0,DWORD PTR [r14+0xd6d548]
000F115F: movss     DWORD PTR [rbp+0x9c84],xmm1
000F1167: mov       DWORD PTR [rsp+0x64],eax
000F116B: mov       DWORD PTR [rsp+0x70],edi
000F116F: mov       DWORD PTR [rbp+0x9c6c],0x0
000F1179: mov       DWORD PTR [rbp+0x9c7c],0x0
000F1183: movss     xmm1,DWORD PTR [r14+0xd6d51c]
000F118C: lea       rdx,[rbp+0x9ca0]
000F1193: movss     xmm2,DWORD PTR [r14+0xd6d520]
000F119C: lea       rcx,[rbp+0x9c60]
000F11A3: movss     xmm11,DWORD PTR [rip+0x23789e0]        # 0x182469b8c ; __real@3f800000
000F11AC: movss     DWORD PTR [rbp+0x9c90],xmm1
000F11B4: movss     xmm1,DWORD PTR [r14+0xd6d524]
000F11BD: movss     DWORD PTR [rbp+0x9c98],xmm1
000F11C5: movss     DWORD PTR [rbp+0x9c88],xmm0
000F11CD: movss     DWORD PTR [rbp+0x9c94],xmm2
000F11D5: mov       DWORD PTR [rbp+0x9c8c],0x0
000F11DF: mov       DWORD PTR [rbp+0x9c9c],0x3f800000
000F11E9: call      0x1800eda90 ; Invert
000F11EE: movss     xmm10,DWORD PTR [rip+0x2378991]        # 0x182469b88 ; __real@3f000000
000F11F7: mulss     xmm9,xmm10
000F11FC: movaps    xmm0,xmm9
000F1200: call      0x1801057d4 ; tanf
000F1205: movaps    xmm9,XMMWORD PTR [rsp+0x9e90]
000F120E: movaps    xmm2,xmm11
000F1212: xorps     xmm1,xmm1
000F1215: divss     xmm2,xmm0
000F1219: movups    XMMWORD PTR [rbp+0x9c20],xmm1
000F1220: movaps    xmm0,xmm2
000F1223: divss     xmm0,xmm6
000F1227: movaps    xmm6,XMMWORD PTR [rsp+0x9ec0]
000F122F: movss     DWORD PTR [rbp+0x9c20],xmm0
000F1237: movups    XMMWORD PTR [rbp+0x9c30],xmm1
000F123E: movss     DWORD PTR [rbp+0x9c34],xmm2
000F1246: movups    XMMWORD PTR [rbp+0x9c40],xmm1
000F124D: movups    XMMWORD PTR [rbp+0x9c50],xmm1
000F1254: test      esi,esi
000F1256: jne       0x1800f1267
000F1258: ucomiss   xmm7,DWORD PTR [rip+0x2378955]        # 0x182469bb4 ; __real@7f7fffff
000F125F: jp        0x1800f1263
000F1261: je        0x1800f1267
000F1263: xor       al,al
000F1265: jmp       0x1800f126b
000F1267: movzx     eax,r15b
000F126B: mov       DWORD PTR [rbp+0x9c4c],0x3f800000
000F1275: test      r12d,r12d
000F1278: je        0x1800f12b9
000F127A: test      al,al
000F127C: je        0x1800f1293
000F127E: movss     DWORD PTR [rbp+0x9c58],xmm8
000F1287: mov       DWORD PTR [rbp+0x9c48],0xb4000000
000F1291: jmp       0x1800f130b
000F1293: movaps    xmm1,xmm7
000F1296: movaps    xmm0,xmm8
000F129A: xorps     xmm0,XMMWORD PTR [rip+0x23789df]        # 0x182469c80 ; __xmm@80000000800000008000000080000000
000F12A1: subss     xmm1,xmm8
000F12A6: mulss     xmm7,xmm8
000F12AB: divss     xmm7,xmm1
000F12AF: movss     DWORD PTR [rbp+0x9c58],xmm7
000F12B7: jmp       0x1800f12ff
000F12B9: movaps    xmm2,xmm8
000F12BD: xorps     xmm2,XMMWORD PTR [rip+0x23789bc]        # 0x182469c80 ; __xmm@80000000800000008000000080000000
000F12C4: test      al,al
000F12C6: je        0x1800f12e4
000F12C8: subss     xmm2,DWORD PTR [rip+0x23788ac]        # 0x182469b7c ; __real@34000000
000F12D0: mov       DWORD PTR [rbp+0x9c48],0x3f800001
000F12DA: movss     DWORD PTR [rbp+0x9c58],xmm2
000F12E2: jmp       0x1800f130b
000F12E4: movaps    xmm1,xmm7
000F12E7: mulss     xmm2,xmm7
000F12EB: subss     xmm1,xmm8
000F12F0: movaps    xmm0,xmm7
000F12F3: divss     xmm2,xmm1
000F12F7: movss     DWORD PTR [rbp+0x9c58],xmm2
000F12FF: divss     xmm0,xmm1
000F1303: movss     DWORD PTR [rbp+0x9c48],xmm0
000F130B: lea       rdx,[rbp+0x9d20]
000F1312: lea       rcx,[rbp+0x9c20]
000F1319: call      0x1800eda90 ; Invert
000F131E: lea       rsi,[r14+0xd6d60c]
000F1325: mov       rcx,rsi
000F1328: lea       rdx,[rbp-0x70]
000F132C: call      0x1800eda90 ; Invert
000F1331: lea       r8,[rbp+0x9be0]
000F1338: lea       rdx,[rbp-0x70]
000F133C: lea       rcx,[rbp+0x9ca0]
000F1343: call      0x1800ee2e0 ; Multiply
000F1348: lea       r8,[rbp+0x9ce0]
000F134F: lea       rcx,[r14+0xd6d64c]
000F1356: lea       rdx,[rbp-0x70]
000F135A: call      0x1800ee2e0 ; Multiply
000F135F: movups    xmm0,XMMWORD PTR [r14+0xd6d58c]
000F1367: mov       ecx,DWORD PTR [r14+0xd6d690]
000F136E: movups    xmm1,XMMWORD PTR [r14+0xd6d59c]
000F1376: mov       r8d,DWORD PTR [r14+0xd6d694]
000F137D: movaps    xmm5,XMMWORD PTR [rbp+0x9c20]
000F1384: movaps    xmm4,XMMWORD PTR [rbp+0x9c30]
000F138B: movaps    xmm3,XMMWORD PTR [rbp+0x9c40]
000F1392: movaps    xmm2,XMMWORD PTR [rbp+0x9c50]
000F1399: movzx     r9d,BYTE PTR [rsp+0x50]
000F139F: movaps    XMMWORD PTR [rbp+0x40],xmm0
000F13A3: movups    xmm0,XMMWORD PTR [r14+0xd6d5ac]
000F13AB: mov       DWORD PTR [rbp+0x180],ecx
000F13B1: movaps    XMMWORD PTR [rbp+0x50],xmm1
000F13B5: movups    xmm1,XMMWORD PTR [r14+0xd6d5bc]
000F13BD: mov       DWORD PTR [rbp+0x184],r8d
000F13C4: movaps    XMMWORD PTR [rbp+0x60],xmm0
000F13C8: movaps    xmm0,XMMWORD PTR [rbp+0x9be0]
000F13CF: movaps    XMMWORD PTR [rbp+0x70],xmm1
000F13D3: movaps    xmm1,XMMWORD PTR [rbp+0x9bf0]
000F13DA: movaps    XMMWORD PTR [rbp+0x80],xmm0
000F13E1: movaps    xmm0,XMMWORD PTR [rbp+0x9c00]
000F13E8: movaps    XMMWORD PTR [rbp+0x90],xmm1
000F13EF: movaps    xmm1,XMMWORD PTR [rbp+0x9c10]
000F13F6: movaps    XMMWORD PTR [rbp+0xa0],xmm0
000F13FD: movaps    xmm0,XMMWORD PTR [rbp+0x9ce0]
000F1404: movaps    XMMWORD PTR [rbp+0xb0],xmm1
000F140B: movaps    xmm1,XMMWORD PTR [rbp+0x9cf0]
000F1412: movaps    XMMWORD PTR [rbp+0x100],xmm0
000F1419: movaps    xmm0,XMMWORD PTR [rbp+0x9d00]
000F1420: movaps    XMMWORD PTR [rbp+0x110],xmm1
000F1427: movaps    xmm1,XMMWORD PTR [rbp+0x9d10]
000F142E: movaps    XMMWORD PTR [rbp+0x120],xmm0
000F1435: movups    xmm0,XMMWORD PTR [r14+0xd6d5cc]
000F143D: movaps    XMMWORD PTR [rbp+0x130],xmm1
000F1444: movups    xmm1,XMMWORD PTR [r14+0xd6d5dc]
000F144C: movaps    XMMWORD PTR [rbp+0x140],xmm0
000F1453: movups    xmm0,XMMWORD PTR [r14+0xd6d5ec]
000F145B: movaps    XMMWORD PTR [rbp+0x150],xmm1
000F1462: movups    xmm1,XMMWORD PTR [r14+0xd6d5fc]
000F146A: movaps    XMMWORD PTR [rbp+0x160],xmm0
000F1471: movaps    XMMWORD PTR [rbp+0x170],xmm1
000F1478: movups    xmm1,XMMWORD PTR [r14+0xd6d55c]
000F1480: movaps    XMMWORD PTR [rbp+0xc0],xmm5
000F1487: movaps    XMMWORD PTR [rbp+0xd0],xmm4
000F148E: movaps    XMMWORD PTR [rbp+0xe0],xmm3
000F1495: movaps    XMMWORD PTR [rbp+0xf0],xmm2
000F149C: movss     xmm0,DWORD PTR [r13+0x38]
000F14A2: movss     DWORD PTR [rbp+0x188],xmm0
000F14AA: movss     xmm0,DWORD PTR [r13+0x3c]
000F14B0: movss     DWORD PTR [rbp+0x18c],xmm0
000F14B8: movups    xmm0,XMMWORD PTR [r14+0xd6d54c]
000F14C0: mov       DWORD PTR [rbp+0x190],r9d
000F14C7: movups    XMMWORD PTR [r14+0xd6d5cc],xmm0
000F14CF: movups    xmm0,XMMWORD PTR [r14+0xd6d56c]
000F14D7: movups    XMMWORD PTR [r14+0xd6d5dc],xmm1
000F14DF: movups    xmm1,XMMWORD PTR [r14+0xd6d57c]
000F14E7: movups    XMMWORD PTR [r14+0xd6d5ec],xmm0
000F14EF: movups    xmm0,XMMWORD PTR [rsi]
000F14F2: movups    XMMWORD PTR [r14+0xd6d5fc],xmm1
000F14FA: movups    xmm1,XMMWORD PTR [rsi+0x10]
000F14FE: movups    XMMWORD PTR [r14+0xd6d64c],xmm0
000F1506: movups    xmm0,XMMWORD PTR [rsi+0x20]
000F150A: movups    XMMWORD PTR [r14+0xd6d65c],xmm1
000F1512: movups    xmm1,XMMWORD PTR [rsi+0x30]
000F1516: movups    XMMWORD PTR [r14+0xd6d66c],xmm0
000F151E: movups    XMMWORD PTR [r14+0xd6d67c],xmm1
000F1526: movaps    xmm0,XMMWORD PTR [rbp+0x9d20]
000F152D: movaps    xmm1,XMMWORD PTR [rbp+0x9d30]
000F1534: mov       edx,DWORD PTR [r14+0xd6d6a0]
000F153B: movaps    xmm8,XMMWORD PTR [rsp+0x9ea0]
000F1544: movaps    xmm7,XMMWORD PTR [rsp+0x9eb0]
000F154C: movups    XMMWORD PTR [r14+0xd6d58c],xmm0
000F1554: mov       DWORD PTR [rbp+0x9c08],r9d
000F155B: movaps    xmm0,XMMWORD PTR [rbp+0x9d40]
000F1562: movups    XMMWORD PTR [r14+0xd6d59c],xmm1
000F156A: movaps    xmm1,XMMWORD PTR [rbp+0x9d50]
000F1571: movups    XMMWORD PTR [r14+0xd6d5ac],xmm0
000F1579: movaps    xmm0,XMMWORD PTR [rbp+0x9ca0]
000F1580: movups    XMMWORD PTR [rsi],xmm0
000F1583: movaps    xmm0,XMMWORD PTR [rbp+0x9cc0]
000F158A: movups    XMMWORD PTR [r14+0xd6d5bc],xmm1
000F1592: movaps    xmm1,XMMWORD PTR [rbp+0x9cb0]
000F1599: movups    XMMWORD PTR [rsi+0x10],xmm1
000F159D: movaps    xmm1,XMMWORD PTR [rbp+0x9cd0]
000F15A4: movups    XMMWORD PTR [rsi+0x20],xmm0
000F15A8: movups    XMMWORD PTR [r14+0xd6d54c],xmm5
000F15B0: movups    XMMWORD PTR [r14+0xd6d55c],xmm4
000F15B8: movups    XMMWORD PTR [r14+0xd6d56c],xmm3
000F15C0: movups    XMMWORD PTR [r14+0xd6d57c],xmm2
000F15C8: movups    XMMWORD PTR [rsi+0x30],xmm1
000F15CC: mov       eax,DWORD PTR [r13+0x28]
000F15D0: mov       DWORD PTR [rbp+0x9be8],eax
000F15D6: xorps     xmm2,xmm2
000F15D9: mov       eax,DWORD PTR [r13+0x2c]
000F15DD: xorps     xmm1,xmm1
000F15E0: mov       DWORD PTR [rbp+0x9bec],eax
000F15E6: mov       eax,DWORD PTR [rsp+0x5c]
000F15EA: cvtsi2ss  xmm2,rcx
000F15EF: mov       ecx,DWORD PTR [r14+0xd6d6a4]
000F15F6: mov       DWORD PTR [rbp+0x9bf0],eax
000F15FC: mov       eax,DWORD PTR [rsp+0x54]
000F1600: mov       DWORD PTR [rbp+0x9bf4],eax
000F1606: mov       eax,DWORD PTR [r14+0xd6d6a8]
000F160D: add       eax,edx
000F160F: movd      xmm0,edx
000F1613: cvtdq2ps  xmm0,xmm0
000F1616: cvtsi2ss  xmm1,r8
000F161B: divss     xmm0,xmm2
000F161F: movss     DWORD PTR [rbp+0x9bf8],xmm0
000F1627: movd      xmm0,ecx
000F162B: cvtdq2ps  xmm0,xmm0
000F162E: divss     xmm0,xmm1
000F1632: movss     DWORD PTR [rbp+0x9bfc],xmm0
000F163A: movd      xmm0,eax
000F163E: mov       eax,DWORD PTR [r14+0xd6d6ac]
000F1645: cvtdq2ps  xmm0,xmm0
000F1648: add       eax,ecx
000F164A: cmp       BYTE PTR [r14+0x9],0x0
000F164F: divss     xmm0,xmm2
000F1653: movss     DWORD PTR [rbp+0x9c00],xmm0
000F165B: movd      xmm0,eax
000F165F: cvtdq2ps  xmm0,xmm0
000F1662: divss     xmm0,xmm1
000F1666: movss     DWORD PTR [rbp+0x9c04],xmm0
000F166E: je        0x1800f16ac
000F1670: movss     xmm0,DWORD PTR [r14+0xd6d510]
000F1679: movss     xmm1,DWORD PTR [r14+0xd6d514]
000F1682: subss     xmm0,DWORD PTR [r13+0x30]
000F1688: subss     xmm1,DWORD PTR [r13+0x34]
000F168E: divss     xmm0,DWORD PTR [r13+0x38]
000F1694: divss     xmm1,DWORD PTR [r13+0x3c]
000F169A: movss     DWORD PTR [rbp+0x9be0],xmm0
000F16A2: movss     DWORD PTR [rbp+0x9be4],xmm1
000F16AA: jmp       0x1800f16b7
000F16AC: mov       QWORD PTR [rbp+0x9be0],0x0
000F16B7: mov       rax,QWORD PTR [r14+0x310]
000F16BE: lea       r12,[r14+0x2a0]
000F16C5: mov       rcx,r12
000F16C8: lea       r9,[r14+0x388]
000F16CF: mov       r8d,0x2c
000F16D5: lea       rdx,[rbp+0x9be0]
000F16DC: call      rax
000F16DE: mov       r9d,DWORD PTR [rbp+0x9bf4]
000F16E5: lea       rdx,[r14+0xd49b88]
000F16EC: mov       r8d,DWORD PTR [rbp+0x9bf0]
000F16F3: add       r9d,0x7
000F16F7: add       r8d,0x7
000F16FB: shr       r9d,0x3
000F16FF: xor       esi,esi
000F1701: shr       r8d,0x3
000F1705: mov       DWORD PTR [rsp+0x28],esi
000F1709: mov       rcx,r14
000F170C: mov       DWORD PTR [rsp+0x20],r15d
000F1711: call      0x1800ed840 ; scheduleDispatch
000F1716: movzx     eax,BYTE PTR [rsp+0x50]
000F171B: lea       r9,[rsp+0x58]
000F1720: mov       BYTE PTR [rsp+0x48],al
000F1724: lea       r8,[rsp+0x60]
000F1729: mov       eax,DWORD PTR [rsp+0x54]
000F172D: mov       rdx,r13
000F1730: mov       DWORD PTR [rsp+0x40],esi
000F1734: mov       rcx,r14
000F1737: movss     DWORD PTR [rsp+0x38],xmm11
000F173E: mov       DWORD PTR [rsp+0x30],eax
000F1742: mov       eax,DWORD PTR [rsp+0x5c]
000F1746: mov       DWORD PTR [rsp+0x28],eax
000F174A: lea       rax,[rsp+0x64]
000F174F: mov       QWORD PTR [rsp+0x20],rax
000F1754: call      0x1800ee490 ; splat
000F1759: mov       rax,QWORD PTR [r14+0x310]
000F1760: lea       r9,[r14+0x3a8]
000F1767: mov       r8d,0x154
000F176D: mov       DWORD PTR [r14+0x440],ebx
000F1774: lea       rdx,[rbp+0x40]
000F1778: mov       DWORD PTR [r14+0x444],edi
000F177F: mov       rcx,r12
000F1782: call      rax
000F1784: mov       r9d,DWORD PTR [rsp+0x54]
000F1789: lea       rdx,[r14+0xd5b848]
000F1790: mov       edi,DWORD PTR [rsp+0x5c]
000F1794: add       r9d,0x7
000F1798: shr       r9d,0x3
000F179C: mov       rcx,r14
000F179F: mov       DWORD PTR [rsp+0x28],esi
000F17A3: mov       DWORD PTR [rsp+0x20],r15d
000F17A8: lea       r8d,[rdi+0x7]
000F17AC: shr       r8d,0x3
000F17B0: call      0x1800ed840 ; scheduleDispatch
000F17B5: movzx     eax,BYTE PTR [rsp+0x50]
000F17BA: lea       r9,[rsp+0x70]
000F17BF: mov       BYTE PTR [rsp+0x48],al
000F17C3: lea       r8,[rbp-0x80]
000F17C7: mov       eax,DWORD PTR [rsp+0x54]
000F17CB: mov       rdx,r13
000F17CE: mov       DWORD PTR [rsp+0x40],r15d
000F17D3: mov       rcx,r14
000F17D6: movss     DWORD PTR [rsp+0x38],xmm10
000F17DD: mov       DWORD PTR [rsp+0x30],eax
000F17E1: lea       rax,[rsp+0x68]
000F17E6: mov       DWORD PTR [rsp+0x28],edi
000F17EA: mov       QWORD PTR [rsp+0x20],rax
000F17EF: call      0x1800ee490 ; splat
000F17F4: movzx     eax,BYTE PTR [rsp+0x50]
000F17F9: lea       r9,[rsp+0x58]
000F17FE: mov       BYTE PTR [rsp+0x48],al
000F1802: lea       r8,[rsp+0x60]
000F1807: mov       eax,DWORD PTR [rsp+0x54]
000F180B: mov       rdx,r13
000F180E: mov       DWORD PTR [rsp+0x40],esi
000F1812: mov       rcx,r14
000F1815: movss     DWORD PTR [rsp+0x38],xmm10
000F181C: mov       DWORD PTR [rsp+0x30],eax
000F1820: lea       rax,[rsp+0x64]
000F1825: mov       DWORD PTR [rsp+0x28],edi
000F1829: mov       QWORD PTR [rsp+0x20],rax
000F182E: call      0x1800ee490 ; splat
000F1833: mov       eax,DWORD PTR [r13+0x30]
000F1837: lea       rcx,[rbp+0x1a4]
000F183E: mov       DWORD PTR [r14+0xd6d510],eax
000F1845: xor       edx,edx
000F1847: mov       eax,DWORD PTR [r13+0x34]
000F184B: mov       r8d,0x9a34
000F1851: mov       DWORD PTR [r14+0xd6d514],eax
000F1858: mov       DWORD PTR [rbp+0x1a0],r15d
000F185F: call      0x1801056da ; memset
000F1864: mov       eax,DWORD PTR [r14+0x418]
000F186B: lea       rdx,[rbp+0x1a0]
000F1872: mov       DWORD PTR [rbp+0x228],eax
000F1878: mov       rcx,r12
000F187B: mov       eax,DWORD PTR [r14+0x450]
000F1882: mov       DWORD PTR [rbp+0x230],eax
000F1888: mov       rax,QWORD PTR [r14+0x328]
000F188F: call      rax
000F1891: mov       eax,DWORD PTR [rsp+0x60]
000F1895: lea       rdx,[rbp+0x1a0]
000F189C: mov       DWORD PTR [rbp+0x228],eax
000F18A2: mov       rcx,r12
000F18A5: mov       rax,QWORD PTR [r14+0x328]
000F18AC: mov       DWORD PTR [rbp+0x230],ebx
000F18B2: call      rax
000F18B4: mov       rax,QWORD PTR [r14+0x330]
000F18BB: mov       rcx,r12
000F18BE: mov       r8d,DWORD PTR [r14+0x37c]
000F18C5: mov       rdx,QWORD PTR [r13+0x20]
000F18C9: call      rax
000F18CB: mov       rax,QWORD PTR [r14+0x2e0]
000F18D2: mov       rcx,r12
000F18D5: mov       r8d,DWORD PTR [r14+0x37c]
000F18DC: mov       rdx,QWORD PTR [r13+0x20]
000F18E0: call      rax
000F18E2: mov       rcx,QWORD PTR [rip+0x2375a1f]        # 0x182467308 ; zeroVector3D
000F18E9: movaps    xmm11,XMMWORD PTR [rsp+0x9e70]
000F18F2: movaps    xmm10,XMMWORD PTR [rsp+0x9e80]
000F18FB: cmp       QWORD PTR [r14+0xd6d51c],rcx
000F1902: jne       0x1800f1f66
000F1908: mov       eax,DWORD PTR [rip+0x2375a02]        # 0x182467310
000F190E: cmp       DWORD PTR [r14+0xd6d524],eax
000F1915: jne       0x1800f1f66
000F191B: cmp       QWORD PTR [r14+0xd6d528],rcx
000F1922: jne       0x1800f1f66
000F1928: cmp       DWORD PTR [r14+0xd6d530],eax
000F192F: jne       0x1800f1f66
000F1935: cmp       QWORD PTR [r14+0xd6d534],rcx
000F193C: jne       0x1800f1f66
000F1942: cmp       DWORD PTR [r14+0xd6d53c],eax
000F1949: jne       0x1800f1f66
000F194F: cmp       QWORD PTR [r14+0xd6d540],rcx
000F1956: jne       0x1800f1f66
000F195C: cmp       DWORD PTR [r14+0xd6d548],eax
000F1963: jne       0x1800f1f66
000F1969: test      BYTE PTR [r14+0xd6d68c],0x40
000F1971: je        0x1800f1f66
000F1977: lea       rdx,[rip+0x2375392]        # 0x182466d10 ; 'Camera view matrix parameters (cameraPosition, cameraUp, cameraRight, cameraForward) are all zero vectors, indicating they remain at their default initialized values. These parameters must be properly set by the application for optimal MLFI quality.'
000F197E: mov       ecx,r15d
000F1981: call      0x1800120a0 ; ffxPrintMessage
000F1986: xor       eax,eax
000F1988: jmp       0x1800f1e7b
000F198D: mov       rcx,QWORD PTR [rdi+0x130]
000F1994: lea       r13,[r14+0x2a0]
000F199B: movups    xmm2,XMMWORD PTR [r14+0x10]
000F19A0: mov       r8d,DWORD PTR [r14+0x37c]
000F19A7: lea       r9,[r14+0x3fc]
000F19AE: mov       r10,QWORD PTR [r14+0x2d0]
000F19B5: mov       rdx,rcx
000F19B8: and       ecx,0x1
000F19BB: and       edx,0x1
000F19BE: imul      rbx,rdx,0xe8
000F19C5: movups    XMMWORD PTR [rbp-0x20],xmm2
000F19C9: mov       eax,DWORD PTR [r14+rcx*4+0x42c]
000F19D1: mov       DWORD PTR [r14+0x428],eax
000F19D8: add       rbx,r14
000F19DB: mov       eax,DWORD PTR [r14+rcx*4+0x42c]
000F19E3: mov       rcx,r13
000F19E6: mov       DWORD PTR [r14+0x4a4],eax
000F19ED: mov       eax,DWORD PTR [r14+rdx*4+0x438]
000F19F5: mov       DWORD PTR [r14+0x434],eax
000F19FC: mov       eax,DWORD PTR [r14+rdx*4+0x438]
000F1A04: mov       DWORD PTR [r14+0x4b0],eax
000F1A0B: movq      rax,xmm2
000F1A10: movups    xmm0,XMMWORD PTR [rdi+0x18]
000F1A14: test      rax,rax
000F1A17: mov       QWORD PTR [rsp+0x70],rbx
000F1A1C: movups    xmm1,XMMWORD PTR [rdi+0x28]
000F1A20: setne     r12b
000F1A24: mov       BYTE PTR [rsp+0x51],r12b
000F1A29: movups    XMMWORD PTR [rbp+0x9be0],xmm0
000F1A30: movups    xmm0,XMMWORD PTR [rdi+0x38]
000F1A34: movups    XMMWORD PTR [rbp+0x9bf0],xmm1
000F1A3B: movups    xmm1,XMMWORD PTR [r14+0x30]
000F1A40: movups    XMMWORD PTR [rbp+0x9c00],xmm0
000F1A47: movups    xmm0,XMMWORD PTR [r14+0x20]
000F1A4C: movups    XMMWORD PTR [rbp+0x0],xmm1
000F1A50: movups    XMMWORD PTR [rbp-0x10],xmm0
000F1A54: test      rax,rax
000F1A57: je        0x1800f1a83
000F1A59: lea       rdx,[rbp-0x20]
000F1A5D: call      r10
000F1A60: mov       rax,QWORD PTR [r14+0x2d0]
000F1A67: lea       r9,[r14+0x454]
000F1A6E: mov       r8d,DWORD PTR [r14+0x37c]
000F1A75: lea       rdx,[rbp+0x9be0]
000F1A7C: mov       rcx,r13
000F1A7F: call      rax
000F1A81: jmp       0x1800f1a8d
000F1A83: lea       rdx,[rbp+0x9be0]
000F1A8A: call      r10
000F1A8D: mov       rcx,QWORD PTR [rdi+0x130]
000F1A94: mov       rax,QWORD PTR [r14+0x298]
000F1A9B: mov       rdx,rcx
000F1A9E: sub       rdx,rax
000F1AA1: cmp       rcx,rax
000F1AA4: jb        0x1800f1ab9
000F1AA6: cmp       rdx,0x1
000F1AAA: ja        0x1800f1ab9
000F1AAC: cmp       rcx,QWORD PTR [r14+0x288]
000F1AB3: jne       0x1800f1ab9
000F1AB5: xor       al,al
000F1AB7: jmp       0x1800f1abb
000F1AB9: mov       al,0x1
000F1ABB: xor       esi,esi
000F1ABD: mov       BYTE PTR [rsp+0x50],al
000F1AC1: mov       r15d,0x1
000F1AC7: cmp       BYTE PTR [rdi+0x10c],sil
000F1ACE: jne       0x1800f1ae0
000F1AD0: cmp       BYTE PTR [rbx+0xbc],sil
000F1AD7: jne       0x1800f1ae0
000F1AD9: mov       r9d,esi
000F1ADC: test      al,al
000F1ADE: je        0x1800f1ae3
000F1AE0: mov       r9d,r15d
000F1AE3: mov       ecx,DWORD PTR [r14+0xd6d690]
000F1AEA: xorps     xmm0,xmm0
000F1AED: mov       edx,DWORD PTR [r14+0xd6d6a0]
000F1AF4: xorps     xmm2,xmm2
000F1AF7: mov       eax,DWORD PTR [rdi+0x110]
000F1AFD: movss     xmm1,DWORD PTR [rdi+0x118]
000F1B05: mov       r8d,DWORD PTR [r14+0xd6d694]
000F1B0C: movdqa    XMMWORD PTR [rbp-0x30],xmm0
000F1B11: movss     xmm0,DWORD PTR [rdi+0x114]
000F1B19: mov       DWORD PTR [rbp-0x5c],eax
000F1B1C: movss     DWORD PTR [rbp-0x50],xmm0
000F1B21: cvtsi2ss  xmm2,rcx
000F1B26: mov       DWORD PTR [rbp-0x68],ecx
000F1B29: mov       ecx,DWORD PTR [r14+0xd6d6a4]
000F1B30: movzx     eax,r12b
000F1B34: mov       DWORD PTR [rbp-0x58],eax
000F1B37: movd      xmm0,edx
000F1B3B: cvtdq2ps  xmm0,xmm0
000F1B3E: mov       DWORD PTR [rbp-0x54],esi
000F1B41: mov       QWORD PTR [rbp-0x38],rsi
000F1B45: mov       DWORD PTR [rbp-0x64],r8d
000F1B49: movss     DWORD PTR [rbp-0x4c],xmm1
000F1B4E: xorps     xmm1,xmm1
000F1B51: mov       eax,DWORD PTR [rbx+0xa0]
000F1B57: mov       DWORD PTR [rbp-0x70],eax
000F1B5A: mov       eax,DWORD PTR [rbx+0xa4]
000F1B60: mov       DWORD PTR [rbp-0x6c],eax
000F1B63: mov       eax,DWORD PTR [r14+0xd6d6a8]
000F1B6A: add       eax,edx
000F1B6C: mov       DWORD PTR [rbp-0x60],r9d
000F1B70: divss     xmm0,xmm2
000F1B74: lea       rdx,[rip+0x221a4c5]        # 0x18230c040 ; debugBarColorSequence
000F1B7B: cvtsi2ss  xmm1,r8
000F1B80: movss     DWORD PTR [rbp-0x48],xmm0
000F1B85: movd      xmm0,ecx
000F1B89: cvtdq2ps  xmm0,xmm0
000F1B8C: divss     xmm0,xmm1
000F1B90: movss     DWORD PTR [rbp-0x44],xmm0
000F1B95: movd      xmm0,eax
000F1B99: mov       eax,DWORD PTR [r14+0xd6d6ac]
000F1BA0: add       eax,ecx
000F1BA2: mov       rcx,QWORD PTR [rip+0x23807f7]        # 0x1824723a0 ; dbgIdx
000F1BA9: cvtdq2ps  xmm0,xmm0
000F1BAC: divss     xmm0,xmm2
000F1BB0: movss     DWORD PTR [rbp-0x40],xmm0
000F1BB5: movd      xmm0,eax
000F1BB9: lea       rax,[rcx+rcx*2]
000F1BBD: inc       rcx
000F1BC0: cvtdq2ps  xmm0,xmm0
000F1BC3: divss     xmm0,xmm1
000F1BC7: movss     DWORD PTR [rbp-0x3c],xmm0
000F1BCC: movsd     xmm0,QWORD PTR [rdx+rax*4]
000F1BD1: mov       eax,DWORD PTR [rdx+rax*4+0x8]
000F1BD5: mov       DWORD PTR [rbp-0x28],eax
000F1BD8: movabs    rax,0x2492492492492493
000F1BE2: mul       rcx
000F1BE5: movsd     QWORD PTR [rbp-0x30],xmm0
000F1BEA: mov       rax,rcx
000F1BED: sub       rax,rdx
000F1BF0: shr       rax,1
000F1BF3: add       rax,rdx
000F1BF6: shr       rax,0x2
000F1BFA: imul      rax,rax,0x7
000F1BFE: sub       rcx,rax
000F1C01: mov       eax,esi
000F1C03: mov       QWORD PTR [rip+0x2380796],rcx        # 0x1824723a0 ; dbgIdx
000F1C0A: mov       ecx,DWORD PTR [r14+0x74]
000F1C0E: test      r15b,cl
000F1C11: cmovne    eax,r15d
000F1C15: mov       DWORD PTR [rbp-0x54],eax
000F1C18: test      cl,0x2
000F1C1B: je        0x1800f1c23
000F1C1D: or        eax,0x2
000F1C20: mov       DWORD PTR [rbp-0x54],eax
000F1C23: test      cl,0x4
000F1C26: je        0x1800f1c2e
000F1C28: or        eax,0x4
000F1C2B: mov       DWORD PTR [rbp-0x54],eax
000F1C2E: test      cl,0x20
000F1C31: je        0x1800f1c39
000F1C33: or        eax,0x10
000F1C36: mov       DWORD PTR [rbp-0x54],eax
000F1C39: test      cl,0x40
000F1C3C: je        0x1800f1c44
000F1C3E: or        eax,0x20
000F1C41: mov       DWORD PTR [rbp-0x54],eax
000F1C44: mov       rax,QWORD PTR [r14+0x310]
000F1C4B: lea       r9,[r14+0x3b8]
000F1C52: mov       r8d,0x50
000F1C58: lea       rdx,[rbp-0x70]
000F1C5C: mov       rcx,r13
000F1C5F: call      rax
000F1C61: movups    xmm0,XMMWORD PTR [rdi+0x48]
000F1C65: mov       rax,QWORD PTR [r14+0x2d0]
000F1C6C: lea       r9,[r14+0x4c8]
000F1C73: movups    xmm1,XMMWORD PTR [rdi+0x58]
000F1C77: mov       r8d,DWORD PTR [r14+0x37c]
000F1C7E: lea       rdx,[rbp+0x10]
000F1C82: movups    XMMWORD PTR [rbp+0x10],xmm0
000F1C86: mov       rcx,r13
000F1C89: movups    xmm0,XMMWORD PTR [rdi+0x68]
000F1C8D: movups    XMMWORD PTR [rbp+0x20],xmm1
000F1C91: movups    XMMWORD PTR [rbp+0x30],xmm0
000F1C95: call      rax
000F1C97: cmp       DWORD PTR [r14+0xd6d690],0xf00
000F1CA2: mov       eax,DWORD PTR [r14+0x4c8]
000F1CA9: mov       DWORD PTR [r14+0x44c],eax
000F1CB0: ja        0x1800f1ccc
000F1CB2: cmp       DWORD PTR [r14+0xd6d694],0x870
000F1CBD: ja        0x1800f1ccc
000F1CBF: mov       DWORD PTR [rsp+0x58],esi
000F1CC3: lea       rdx,[r14+0x4e4]
000F1CCA: jmp       0x1800f1cd8
000F1CCC: mov       DWORD PTR [rsp+0x58],r15d
000F1CD1: lea       rdx,[r14+0x1694]
000F1CD8: mov       rax,QWORD PTR [r14+0x310]
000F1CDF: lea       r9,[r14+0x3d8]
000F1CE6: mov       r8d,0x5c0
000F1CEC: mov       rcx,r13
000F1CEF: call      rax
000F1CF1: mov       r13d,DWORD PTR [rsp+0x58]
000F1CF6: lea       rbx,[r14+0xaa4]
000F1CFD: mov       r12d,esi
000F1D00: mov       QWORD PTR [rbp-0x80],0xbf
000F1D08: nop       DWORD PTR [rax+rax*1+0x0]
000F1D10: movsxd    rax,r12d
000F1D13: imul      rcx,rax,0x8e60
000F1D1A: mov       eax,esi
000F1D1C: test      r13d,r13d
000F1D1F: jne       0x1800f1d34
000F1D21: lea       rdx,[r14+0x2848]
000F1D28: mov       r8,rbx
000F1D2B: add       rdx,rcx
000F1D2E: cmp       BYTE PTR [rbx+0xc],sil
000F1D32: jmp       0x1800f1d4c
000F1D34: lea       rdx,[r14+0x6a61e8]
000F1D3B: add       rdx,rcx
000F1D3E: lea       r8,[rbx+0x11b0]
000F1D45: cmp       BYTE PTR [rbx+0x11bc],sil
000F1D4C: mov       r9d,DWORD PTR [r8+0x4]
000F1D50: setne     al
000F1D53: mov       DWORD PTR [rsp+0x28],eax
000F1D57: mov       rcx,r14
000F1D5A: mov       eax,DWORD PTR [r8+0x8]
000F1D5E: mov       r8d,DWORD PTR [r8]
000F1D61: mov       DWORD PTR [rsp+0x20],eax
000F1D65: call      0x1800ed840 ; scheduleDispatch
000F1D6A: inc       r12d
000F1D6D: add       rbx,0x10
000F1D71: sub       QWORD PTR [rbp-0x80],r15
000F1D75: jne       0x1800f1d10
000F1D77: mov       rcx,QWORD PTR [r14+0xd6d6b0]
000F1D7E: test      rcx,rcx
000F1D81: je        0x1800f1dc5
000F1D83: test      BYTE PTR [r14+0xd6d68c],0x8
000F1D8B: lea       rax,[rip+0x237462e]        # 0x1824663c0
000F1D92: lea       rdx,[rip+0x23745c7]        # 0x182466360
000F1D99: cmove     rdx,rax
000F1D9D: mov       rax,0xffffffffffffffff
000F1DA4: inc       rax
000F1DA7: cmp       BYTE PTR [rdx+rax*1],sil
000F1DAB: jne       0x1800f1da4
000F1DAD: mov       QWORD PTR [rbp-0x80],rdx
000F1DB1: lea       r8,[rbp-0x80]
000F1DB5: mov       edx,DWORD PTR [r14+0x4c8]
000F1DBC: mov       QWORD PTR [rbp-0x78],rax
000F1DC0: call      0x1800f6c60 ; FfxWatermark::Dispatch
000F1DC5: xor       edx,edx
000F1DC7: mov       DWORD PTR [rbp+0x1a0],r15d
000F1DCE: mov       r8d,0x9a34
000F1DD4: lea       rcx,[rbp+0x1a4]
000F1DDB: call      0x1801056da ; memset
000F1DE0: mov       eax,DWORD PTR [r14+0x3fc]
000F1DE7: lea       rdx,[rbp+0x1a0]
000F1DEE: mov       DWORD PTR [rbp+0x228],eax
000F1DF4: lea       rcx,[r14+0x2a0]
000F1DFB: mov       eax,DWORD PTR [r14+0x3f8]
000F1E02: mov       DWORD PTR [rbp+0x230],eax
000F1E08: mov       rax,QWORD PTR [r14+0x328]
000F1E0F: call      rax
000F1E11: mov       rax,QWORD PTR [r14+0x330]
000F1E18: lea       rcx,[r14+0x2a0]
000F1E1F: mov       r8d,DWORD PTR [r14+0x37c]
000F1E26: mov       rdx,QWORD PTR [rdi+0x10]
000F1E2A: call      rax
000F1E2C: mov       rax,QWORD PTR [r14+0x2e0]
000F1E33: lea       rcx,[r14+0x2a0]
000F1E3A: mov       r8d,DWORD PTR [r14+0x37c]
000F1E41: mov       rdx,QWORD PTR [rdi+0x10]
000F1E45: call      rax
000F1E47: test      BYTE PTR [r14+0xd6d68c],0x40
000F1E4F: je        0x1800f1f49
000F1E55: mov       eax,DWORD PTR [r14+0xd6d69c]
000F1E5C: cmp       BYTE PTR [rsp+0x51],sil
000F1E61: je        0x1800f1ea5
000F1E63: cmp       DWORD PTR [rbp-0x14],eax
000F1E66: je        0x1800f1eb6
000F1E68: lea       rdx,[rip+0x23745b1]        # 0x182466420 ; 'ffxConfigureDescFrameGeneration::HUDLessColor format have to be same as one of ffxCreateContextDescFrameGeneration::backBufferFormat or ffxCreateContextDescFrameGenerationHudless::hudlessBackBufferFormat. Otherwise, CopyTextureRegion of current to previous frame backbuffer would fail'
000F1E6F: xor       ecx,ecx
000F1E71: call      0x1800120a0 ; ffxPrintMessage
000F1E76: mov       eax,0x6
000F1E7B: mov       rcx,QWORD PTR [rbp+0x9d60]
000F1E82: xor       rcx,rsp
000F1E85: call      0x180104650 ; __security_check_cookie
000F1E8A: mov       rbx,QWORD PTR [rsp+0x9f10]
000F1E92: add       rsp,0x9ed0
000F1E99: pop       r15
000F1E9B: pop       r14
000F1E9D: pop       r13
000F1E9F: pop       r12
000F1EA1: pop       rdi
000F1EA2: pop       rsi
000F1EA3: pop       rbp
000F1EA4: ret       
000F1EA5: cmp       DWORD PTR [rbp+0x9bec],eax
000F1EAB: je        0x1800f1eb6
000F1EAD: lea       rdx,[rip+0x23747ac]        # 0x182466660 ; 'ffxDispatchDescFrameGeneration::presentColor format and ffxCreateContextDescFrameGeneration::backBufferFormat have to be identical. Or ffxConfigureDescFrameGeneration::HUDLessColor have to be valid.'
000F1EB4: jmp       0x1800f1e6f
000F1EB6: cmp       QWORD PTR [r14+0x298],rsi
000F1EBD: je        0x1800f1eda
000F1EBF: cmp       BYTE PTR [rdi+0x10c],sil
000F1EC6: jne       0x1800f1eda
000F1EC8: mov       rax,QWORD PTR [rsp+0x70]
000F1ECD: cmp       BYTE PTR [rax+0xbc],sil
000F1ED4: jne       0x1800f1eda
000F1ED6: xor       al,al
000F1ED8: jmp       0x1800f1ede
000F1EDA: movzx     eax,r15b
000F1EDE: cmp       BYTE PTR [r14+0x8],sil
000F1EE2: je        0x1800f1efe
000F1EE4: test      al,al
000F1EE6: jne       0x1800f1efe
000F1EE8: cmp       BYTE PTR [rsp+0x50],sil
000F1EED: je        0x1800f1efe
000F1EEF: lea       rdx,[rip+0x23748fa]        # 0x1824667f0 ; 'When async support is enabled, and the reset flag is not set, frame ID must increment in each dispatch'
000F1EF6: mov       ecx,r15d
000F1EF9: call      0x1800120a0 ; ffxPrintMessage
000F1EFE: mov       eax,DWORD PTR [rdi+0x124]
000F1F04: cmp       DWORD PTR [r14+0xd6d6a8],eax
000F1F0B: jne       0x1800f1f3a
000F1F0D: mov       eax,DWORD PTR [rdi+0x128]
000F1F13: cmp       DWORD PTR [r14+0xd6d6ac],eax
000F1F1A: jne       0x1800f1f3a
000F1F1C: mov       eax,DWORD PTR [rdi+0x11c]
000F1F22: cmp       DWORD PTR [r14+0xd6d6a0],eax
000F1F29: jne       0x1800f1f3a
000F1F2B: mov       eax,DWORD PTR [rdi+0x120]
000F1F31: cmp       DWORD PTR [r14+0xd6d6a4],eax
000F1F38: je        0x1800f1f49
000F1F3A: lea       rdx,[rip+0x237497f]        # 0x1824668c0 ; 'The generation rects passed to ffxConfigure and ffxDispatch do not match. This is not supported.'
000F1F41: mov       ecx,r15d
000F1F44: call      0x1800120a0 ; ffxPrintMessage
000F1F49: mov       rax,QWORD PTR [rdi+0x130]
000F1F50: inc       QWORD PTR [r14+0x290]
000F1F57: mov       QWORD PTR [r14+0x298],rax
000F1F5E: mov       WORD PTR [r14+0xd6d518],si
000F1F66: xor       eax,eax
000F1F68: jmp       0x1800f1e7b
