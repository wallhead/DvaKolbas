; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x2949C0..0x295263; unnamed
002949C0: rex       push rbp
002949C2: push      rsi
002949C3: push      r12
002949C5: push      r14
002949C7: lea       rbp,[rsp-0x168]
002949CF: sub       rsp,0x268
002949D6: mov       r14,rcx
002949D9: mov       rsi,r8
002949DC: lea       rcx,[rbp-0x70]
002949E0: mov       r12,rdx
002949E3: call      0x180179bf0
002949E8: mov       rcx,r14
002949EB: call      0x180296550
002949F0: test      al,al
002949F2: jne       0x180294a4e
002949F4: test      r12,r12
002949F7: je        0x180295255
002949FD: test      rsi,rsi
00294A00: je        0x180295255
00294A06: mov       rax,QWORD PTR [r12]
00294A0A: test      rax,rax
00294A0D: je        0x180295255
00294A13: mov       rdx,QWORD PTR [rsi]
00294A16: test      rdx,rdx
00294A19: je        0x180295255
00294A1F: cmp       QWORD PTR [r14+0x1680],0x0
00294A27: je        0x180295255
00294A2D: cmp       rax,rdx
00294A30: je        0x180295255
00294A36: mov       r8,r12
00294A39: mov       rcx,r14
00294A3C: add       rsp,0x268
00294A43: pop       r14
00294A45: pop       r12
00294A47: pop       rsi
00294A48: pop       rbp
00294A49: jmp       0x180298770
00294A4E: cmp       QWORD PTR [r14+0xb50],0x0
00294A56: mov       QWORD PTR [rsp+0x290],rbx
00294A5E: mov       QWORD PTR [rsp+0x298],rdi
00294A66: mov       QWORD PTR [rsp+0x260],r15
00294A6E: je        0x180295208
00294A74: cmp       QWORD PTR [r14+0xba8],0x0
00294A7C: je        0x180295208
00294A82: cmp       QWORD PTR [r14+0x7d0],0x0
00294A8A: je        0x180295208
00294A90: cmp       QWORD PTR [r14+0x828],0x0
00294A98: je        0x180295208
00294A9E: mov       rcx,QWORD PTR [r14+0x1680]
00294AA5: mov       r8,QWORD PTR [rsi]
00294AA8: mov       rdx,QWORD PTR [r14+0xaf8]
00294AAF: mov       rax,QWORD PTR [rcx]
00294AB2: call      QWORD PTR [rax+0x178]
00294AB8: mov       rcx,QWORD PTR [r14+0x1680]
00294ABF: mov       r8,QWORD PTR [r14+0x7d0]
00294AC6: mov       rdx,QWORD PTR [r14+0xb50]
00294ACD: mov       rax,QWORD PTR [rcx]
00294AD0: call      QWORD PTR [rax+0x178]
00294AD6: mov       rcx,QWORD PTR [r14+0x1680]
00294ADD: mov       r8,QWORD PTR [r14+0x828]
00294AE4: mov       rdx,QWORD PTR [r14+0xba8]
00294AEB: mov       rax,QWORD PTR [rcx]
00294AEE: call      QWORD PTR [rax+0x178]
00294AF4: cmp       BYTE PTR [r14+0x4ad],0x0
00294AFC: lea       rbx,[r14+0x1211]
00294B03: jne       0x180294b0a
00294B05: cmp       BYTE PTR [rbx],0x0
00294B08: je        0x180294b1d
00294B0A: mov       rcx,r14
00294B0D: mov       rdi,rbx
00294B10: call      0x1802aa770
00294B15: test      al,al
00294B17: je        0x180294b1d
00294B19: mov       cl,0x1
00294B1B: jmp       0x180294b22
00294B1D: xor       cl,cl
00294B1F: mov       rdi,rbx
00294B22: mov       eax,DWORD PTR [rip+0x1cc5b8]        # 0x1804610e0
00294B28: lea       rbx,[r14+0x460]
00294B2F: cmp       DWORD PTR [rbx],eax
00294B31: jne       0x180294c89
00294B37: cmp       BYTE PTR [rip+0x1e1206],0x0        # 0x180475d44
00294B3E: je        0x180294c89
00294B44: mov       BYTE PTR [rdi],0x1
00294B47: lea       rdi,[rip+0x1715e2]        # 0x180406130
00294B4E: lea       rsi,[rip+0x17270b]        # 0x180407260 ; 'void __cdecl SkyrimUpscaler::EvaluateUpscaling(struct ImageWrapper *,struct ImageWrapper *)'
00294B55: test      cl,cl
00294B57: je        0x180294b65
00294B59: mov       rcx,r14
00294B5C: call      0x180266e20
00294B61: test      al,al
00294B63: jne       0x180294bbf
00294B65: call      0x180222050
00294B6A: mov       ecx,DWORD PTR [rbp-0x44]
00294B6D: lea       r9,[rbp-0x70]
00294B71: mov       DWORD PTR [rsp+0x7c],ecx
00294B75: lea       rdx,[rbp-0x50]
00294B79: lea       rcx,[rip+0x172690]        # 0x180407210
00294B80: mov       QWORD PTR [rsp+0x70],rdi
00294B85: mov       QWORD PTR [rbp-0x70],rcx
00294B89: mov       rcx,rax
00294B8C: mov       DWORD PTR [rsp+0x78],0x900
00294B94: vmovups   xmm0,XMMWORD PTR [rsp+0x70]
00294B9A: mov       QWORD PTR [rbp-0x80],rsi
00294B9E: vmovsd    xmm1,QWORD PTR [rbp-0x80]
00294BA3: mov       QWORD PTR [rbp-0x68],0x4b
00294BAB: vmovups   XMMWORD PTR [rbp-0x50],xmm0
00294BB0: vmovsd    QWORD PTR [rbp-0x40],xmm1
00294BB5: mov       QWORD PTR [rsp+0x20],rbx
00294BBA: call      0x180154dd0
00294BBF: mov       rcx,r14
00294BC2: call      0x180265e20
00294BC7: test      al,al
00294BC9: jne       0x180294c25
00294BCB: call      0x180222050
00294BD0: mov       ecx,DWORD PTR [rbp-0x44]
00294BD3: lea       r9,[rbp-0x70]
00294BD7: mov       DWORD PTR [rsp+0x7c],ecx
00294BDB: lea       rdx,[rbp-0x50]
00294BDF: lea       rcx,[rip+0x172702]        # 0x1804072e8
00294BE6: mov       QWORD PTR [rsp+0x70],rdi
00294BEB: mov       QWORD PTR [rbp-0x70],rcx
00294BEF: mov       rcx,rax
00294BF2: mov       DWORD PTR [rsp+0x78],0x904
00294BFA: vmovups   xmm0,XMMWORD PTR [rsp+0x70]
00294C00: mov       QWORD PTR [rbp-0x80],rsi
00294C04: vmovsd    xmm1,QWORD PTR [rbp-0x80]
00294C09: mov       QWORD PTR [rbp-0x68],0x2b
00294C11: vmovups   XMMWORD PTR [rbp-0x50],xmm0
00294C16: vmovsd    QWORD PTR [rbp-0x40],xmm1
00294C1B: mov       QWORD PTR [rsp+0x20],rbx
00294C20: call      0x180154dd0
00294C25: cmp       QWORD PTR [r14+0xa38],0x0
00294C2D: mov       edx,0xa38
00294C32: mov       ecx,0x778
00294C37: cmove     edx,ecx
00294C3A: add       rdx,r14
00294C3D: test      r12,r12
00294C40: je        0x180294c72
00294C42: test      rdx,rdx
00294C45: je        0x180294c72
00294C47: mov       rax,QWORD PTR [r12]
00294C4B: test      rax,rax
00294C4E: je        0x180294c72
00294C50: mov       rdx,QWORD PTR [rdx]
00294C53: test      rdx,rdx
00294C56: je        0x180294c72
00294C58: cmp       QWORD PTR [r14+0x1680],0x0
00294C60: je        0x180294c72
00294C62: cmp       rax,rdx
00294C65: je        0x180294c72
00294C67: mov       r8,r12
00294C6A: mov       rcx,r14
00294C6D: call      0x180298770
00294C72: xor       r15d,r15d
00294C75: mov       BYTE PTR [r14+0x48b],0x1
00294C7D: mov       DWORD PTR [r14+0x500],r15d
00294C84: jmp       0x18029523d
00294C89: cmp       BYTE PTR [rdi],0x0
00294C8C: je        0x180294ca4
00294C8E: mov       rcx,r14
00294C91: call      0x180266330
00294C96: test      al,al
00294C98: jne       0x180294ca4
00294C9A: mov       BYTE PTR [r14+0x264],0x1
00294CA2: mov       BYTE PTR [rdi],al
00294CA4: cmp       BYTE PTR [r14+0x484],0x0
00294CAC: mov       BYTE PTR [r14+0xaec],0x0
00294CB4: jne       0x1802951e0
00294CBA: mov       rax,QWORD PTR [r14+0xaf8]
00294CC1: xor       r15d,r15d
00294CC4: mov       QWORD PTR [rsp+0x2a0],r13
00294CCC: vmovaps   XMMWORD PTR [rsp+0x250],xmm6
00294CD5: vmovaps   XMMWORD PTR [rsp+0x240],xmm7
00294CDE: mov       QWORD PTR [rsp+0x70],rax
00294CE3: cmp       BYTE PTR [r14+0x772],r15b
00294CEA: je        0x180294cf5
00294CEC: mov       QWORD PTR [rbp+0x1a8],r15
00294CF3: jmp       0x180294d03
00294CF5: mov       rax,QWORD PTR [r14+0xb50]
00294CFC: mov       QWORD PTR [rbp+0x1a8],rax
00294D03: mov       rax,QWORD PTR [r14+0xba8]
00294D0A: mov       rcx,r14
00294D0D: mov       QWORD PTR [rbp-0x78],rax
00294D11: call      0x180294110
00294D16: lea       rdx,[r14+0x3dc]
00294D1D: test      al,al
00294D1F: je        0x180294d38
00294D21: cmp       DWORD PTR [r14+0x4fc],r15d
00294D28: je        0x180294d2f
00294D2A: cmp       DWORD PTR [rdx],0x3
00294D2D: jne       0x180294d38
00294D2F: mov       r13,QWORD PTR [r14+0x988]
00294D36: jmp       0x180294d3b
00294D38: mov       r13,r15
00294D3B: call      0x180294110
00294D40: test      al,al
00294D42: je        0x180294d64
00294D44: cmp       BYTE PTR [r14+0x168b],r15b
00294D4B: jne       0x180294d64
00294D4D: cmp       DWORD PTR [rdx],0x3
00294D50: jne       0x180294d5b
00294D52: cmp       BYTE PTR [r14+0x4a5],r15b
00294D59: je        0x180294d64
00294D5B: mov       rdi,QWORD PTR [r14+0xa38]
00294D62: jmp       0x180294d67
00294D64: mov       rdi,r15
00294D67: cmp       BYTE PTR [r14+0x265],r15b
00294D6E: je        0x180294d7b
00294D70: vmovss    xmm6,DWORD PTR [r14+0x268]
00294D79: jmp       0x180294d7f
00294D7B: vxorps    xmm6,xmm6,xmm6
00294D7F: mov       rcx,QWORD PTR [r14+0xaf8]
00294D86: vxorps    xmm1,xmm1,xmm1
00294D8A: vcvtsi2ss xmm1,xmm1,DWORD PTR [r14+0x274]
00294D93: vxorps    xmm0,xmm0,xmm0
00294D97: vcvtsi2ss xmm0,xmm0,DWORD PTR [r14+0x270]
00294DA0: vdivss    xmm1,xmm1,xmm0
00294DA4: vmulss    xmm2,xmm1,DWORD PTR [r14+0x2f0]
00294DAD: vmulss    xmm7,xmm2,DWORD PTR [rip+0x176ea3]        # 0x18040bc58
00294DB5: test      rcx,rcx
00294DB8: je        0x180294dc7
00294DBA: mov       rax,QWORD PTR [rcx]
00294DBD: lea       rdx,[r14+0xb20]
00294DC4: call      QWORD PTR [rax+0x50]
00294DC7: mov       eax,DWORD PTR [r14+0xb30]
00294DCE: cmp       DWORD PTR [r14+0x2a4],eax
00294DD5: je        0x180294e0a
00294DD7: mov       rcx,QWORD PTR [r14+0xaf8]
00294DDE: test      rcx,rcx
00294DE1: je        0x180294df0
00294DE3: mov       rax,QWORD PTR [rcx]
00294DE6: lea       rdx,[r14+0xb20]
00294DED: call      QWORD PTR [rax+0x50]
00294DF0: vmovups   xmm0,XMMWORD PTR [r14+0xb30]
00294DF9: mov       rcx,r14
00294DFC: vmovd     DWORD PTR [r14+0x2a4],xmm0
00294E05: call      0x1802a2120
00294E0A: cmp       BYTE PTR [r14+0x2aa],r15b
00294E11: je        0x180294e29
00294E13: mov       r9b,0x1
00294E16: lea       rdx,[r14+0xaf8]
00294E1D: movzx     r8d,r9b
00294E21: mov       rcx,r14
00294E24: call      0x1802a2300
00294E29: cmp       BYTE PTR [r14+0x4e4],r15b
00294E30: je        0x180294e39
00294E32: mov       edx,0x1
00294E37: jmp       0x180294e42
00294E39: mov       edx,DWORD PTR [r14+0x16a0]
00294E40: dec       edx
00294E42: mov       ecx,DWORD PTR [r14+0x460]
00294E49: xor       eax,eax
00294E4B: mov       QWORD PTR [rbp+0x40],rax
00294E4F: mov       rax,QWORD PTR [rsp+0x70]
00294E54: vpxor     xmm0,xmm0,xmm0
00294E58: vmovups   YMMWORD PTR [rbp+0x10],ymm0
00294E5D: vmovups   XMMWORD PTR [rbp+0x30],xmm0
00294E62: mov       QWORD PTR [rbp-0x18],rax
00294E66: mov       rax,QWORD PTR [rbp-0x78]
00294E6A: mov       QWORD PTR [rbp-0x10],rax
00294E6E: mov       rax,QWORD PTR [rbp+0x1a8]
00294E75: vxorps    xmm0,xmm0,xmm0
00294E79: vcvtsi2ss xmm0,xmm0,DWORD PTR [r14+0x278]
00294E82: vmovss    DWORD PTR [rbp+0x18],xmm0
00294E87: vmovss    xmm0,DWORD PTR [r14+0x8]
00294E8D: mov       QWORD PTR [rbp-0x8],rax
00294E91: vcvttss2si eax,DWORD PTR [r14+0x10]
00294E97: vmovss    DWORD PTR [rbp+0x24],xmm0
00294E9C: vxorps    xmm1,xmm1,xmm1
00294EA0: vcvtsi2ss xmm1,xmm1,DWORD PTR [r14+0x27c]
00294EA9: vmovss    DWORD PTR [rbp+0x1c],xmm1
00294EAE: vmovss    xmm1,DWORD PTR [r14+0xc]
00294EB4: vmovss    DWORD PTR [rbp+0x28],xmm1
00294EB9: vxorps    xmm0,xmm0,xmm0
00294EBD: vcvtsi2ss xmm0,xmm0,eax
00294EC1: vcvttss2si eax,DWORD PTR [r14+0x14]
00294EC7: vmovss    DWORD PTR [rbp+0x2c],xmm0
00294ECC: vmovss    xmm0,DWORD PTR [r14+0x2f8]
00294ED5: vmovss    DWORD PTR [rbp+0x20],xmm6
00294EDA: vmovss    DWORD PTR [rbp+0x38],xmm0
00294EDF: vmovss    DWORD PTR [rbp+0x40],xmm7
00294EE4: mov       DWORD PTR [rbp+0x80],edx
00294EEA: lea       rdx,[rbp+0x90]
00294EF1: mov       QWORD PTR [rbp-0x20],r15
00294EF5: mov       QWORD PTR [rbp+0x8],r15
00294EF9: mov       QWORD PTR [rbp+0x50],r15
00294EFD: mov       QWORD PTR [rbp+0x58],r15
00294F01: mov       QWORD PTR [rbp+0x60],r15
00294F05: mov       QWORD PTR [rbp+0x48],r15
00294F09: mov       QWORD PTR [rbp+0x0],r15
00294F0D: mov       QWORD PTR [rbp+0x10],r15
00294F11: mov       QWORD PTR [rbp+0x68],r13
00294F15: mov       QWORD PTR [rbp+0x70],rdi
00294F19: mov       BYTE PTR [rbp+0x34],r15b
00294F1D: mov       DWORD PTR [rbp+0x78],ecx
00294F20: mov       BYTE PTR [rbp+0x44],0x1
00294F24: mov       BYTE PTR [rbp+0x7c],0x1
00294F28: mov       QWORD PTR [rbp+0x88],r15
00294F2F: vxorps    xmm1,xmm1,xmm1
00294F33: vcvtsi2ss xmm1,xmm1,eax
00294F37: vmovss    DWORD PTR [rbp+0x30],xmm1
00294F3C: vmovss    xmm1,DWORD PTR [r14+0x2f4]
00294F45: vmovss    DWORD PTR [rbp+0x3c],xmm1
00294F4A: lea       rax,[rbp-0x20]
00294F4E: vmovups   ymm0,YMMWORD PTR [rax]
00294F52: vmovups   ymm2,YMMWORD PTR [rax+0x80]
00294F5A: vmovups   YMMWORD PTR [rdx],ymm0
00294F5E: vmovups   ymm0,YMMWORD PTR [rax+0x20]
00294F63: vmovups   YMMWORD PTR [rdx+0x20],ymm0
00294F68: vmovups   ymm0,YMMWORD PTR [rax+0x40]
00294F6D: vmovups   YMMWORD PTR [rdx+0x40],ymm0
00294F72: vmovups   ymm0,YMMWORD PTR [rax+0x60]
00294F77: vmovups   YMMWORD PTR [rdx+0x60],ymm0
00294F7C: vmovups   YMMWORD PTR [rdx+0x80],ymm2
00294F84: vmovups   xmm2,XMMWORD PTR [rax+0xa0]
00294F8C: movzx     eax,BYTE PTR [r14+0x264]
00294F94: vmovups   XMMWORD PTR [rdx+0xa0],xmm2
00294F9C: vmovaps   xmm1,xmm7
00294FA0: mov       BYTE PTR [rbp+0xe4],al
00294FA6: vzeroupper 
00294FA9: call      0x18027cce0
00294FAE: lea       rcx,[rbp+0x90]
00294FB5: vmovss    DWORD PTR [rbp+0xf0],xmm0
00294FBD: call      QWORD PTR [rip+0x1ddc5d]        # 0x180472c20 ; PDPerfPlugin.dll!EvaluateUpscaler
00294FC3: vmovaps   xmm7,XMMWORD PTR [rsp+0x240]
00294FCC: vmovaps   xmm6,XMMWORD PTR [rsp+0x250]
00294FD5: mov       r13,QWORD PTR [rsp+0x2a0]
00294FDD: mov       BYTE PTR [r14+0x264],r15b
00294FE4: cmp       BYTE PTR [r14+0x48e],r15b
00294FEB: jne       0x180295100
00294FF1: mov       rcx,QWORD PTR [r14+0x1680]
00294FF8: lea       r8,[rbp+0x1a8]
00294FFF: lea       rdx,[rip+0x172b6a]        # 0x180407b70
00295006: mov       rax,QWORD PTR [rcx]
00295009: call      QWORD PTR [rax]
0029500B: mov       rcx,QWORD PTR [rbp+0x1a8]
00295012: lea       rdx,[rip+0x1722a7]        # 0x1804072c0 ; 'EvaluateUpscaling'
00295019: mov       rax,QWORD PTR [rcx]
0029501C: call      QWORD PTR [rax+0x18]
0029501F: mov       rcx,QWORD PTR [r14+0x1680]
00295026: mov       r8,QWORD PTR [rsi]
00295029: mov       rdx,QWORD PTR [r14+0xaf8]
00295030: mov       rax,QWORD PTR [rcx]
00295033: call      QWORD PTR [rax+0x178]
00295039: mov       rdi,QWORD PTR [r14+0x1680]
00295040: lea       rcx,[r14+0x778]
00295047: vxorps    xmm0,xmm0,xmm0
0029504B: vmovups   XMMWORD PTR [rsp+0x70],xmm0
00295051: mov       rax,QWORD PTR [rdi]
00295054: mov       rbx,QWORD PTR [rax+0x190]
0029505B: call      0x18016c140
00295060: lea       r8,[rsp+0x70]
00295065: mov       rdx,rax
00295068: mov       rcx,rdi
0029506B: call      rbx
0029506D: lea       rcx,[r14+0xaf8]
00295074: call      0x18016c290
00295079: mov       ebx,DWORD PTR [r14+0x274]
00295080: lea       rcx,[r14+0x778]
00295087: mov       edi,DWORD PTR [r14+0x270]
0029508E: mov       QWORD PTR [rbp-0x78],rax
00295092: call      0x18016c140
00295097: mov       QWORD PTR [rsp+0x68],r15
0029509C: mov       edx,0x1
002950A1: mov       BYTE PTR [rsp+0x60],r15b
002950A6: xor       r9d,r9d
002950A9: mov       DWORD PTR [rsp+0x58],r15d
002950AE: mov       r8d,edx
002950B1: mov       DWORD PTR [rsp+0x50],r15d
002950B6: mov       rcx,r14
002950B9: mov       DWORD PTR [rsp+0x48],ebx
002950BD: mov       DWORD PTR [rsp+0x40],edi
002950C1: mov       QWORD PTR [rsp+0x38],rax
002950C6: lea       rax,[rbp-0x78]
002950CA: mov       QWORD PTR [rsp+0x30],r15
002950CF: mov       QWORD PTR [rsp+0x28],rax
002950D4: mov       DWORD PTR [rsp+0x20],0x1
002950DC: call      0x1802a1240
002950E1: mov       rcx,QWORD PTR [rbp+0x1a8]
002950E8: mov       rax,QWORD PTR [rcx]
002950EB: call      QWORD PTR [rax+0x20]
002950EE: mov       rcx,QWORD PTR [rbp+0x1a8]
002950F5: test      rcx,rcx
002950F8: je        0x180295100
002950FA: mov       rax,QWORD PTR [rcx]
002950FD: call      QWORD PTR [rax+0x10]
00295100: lea       rcx,[r14+0x778]
00295107: call      0x18016c290
0029510C: mov       ebx,DWORD PTR [r14+0x274]
00295113: mov       rcx,r12
00295116: mov       edi,DWORD PTR [r14+0x270]
0029511D: mov       QWORD PTR [rbp-0x60],rax
00295121: call      0x18016c140
00295126: mov       QWORD PTR [rsp+0x68],r15
0029512B: mov       edx,0x1
00295130: mov       BYTE PTR [rsp+0x60],r15b
00295135: xor       r9d,r9d
00295138: mov       DWORD PTR [rsp+0x58],r15d
0029513D: mov       r8d,edx
00295140: mov       DWORD PTR [rsp+0x50],r15d
00295145: mov       rcx,r14
00295148: mov       DWORD PTR [rsp+0x48],ebx
0029514C: mov       DWORD PTR [rsp+0x40],edi
00295150: mov       QWORD PTR [rsp+0x38],rax
00295155: lea       rax,[rbp-0x60]
00295159: mov       QWORD PTR [rsp+0x30],r15
0029515E: mov       QWORD PTR [rsp+0x28],rax
00295163: mov       DWORD PTR [rsp+0x20],0x1
0029516B: call      0x1802a1240
00295170: lea       rcx,[r14+0xa38]
00295177: mov       rax,QWORD PTR [rcx]
0029517A: cmp       QWORD PTR [r14+0x778],rax
00295181: je        0x1802951e0
00295183: mov       ebx,DWORD PTR [r14+0x274]
0029518A: mov       edi,DWORD PTR [r14+0x270]
00295191: call      0x18016c140
00295196: mov       QWORD PTR [rsp+0x68],r15
0029519B: mov       edx,0x1
002951A0: mov       BYTE PTR [rsp+0x60],r15b
002951A5: xor       r9d,r9d
002951A8: mov       DWORD PTR [rsp+0x58],r15d
002951AD: mov       r8d,edx
002951B0: mov       DWORD PTR [rsp+0x50],r15d
002951B5: mov       rcx,r14
002951B8: mov       DWORD PTR [rsp+0x48],ebx
002951BC: mov       DWORD PTR [rsp+0x40],edi
002951C0: mov       QWORD PTR [rsp+0x38],rax
002951C5: lea       rax,[rbp-0x60]
002951C9: mov       QWORD PTR [rsp+0x30],r15
002951CE: mov       QWORD PTR [rsp+0x28],rax
002951D3: mov       DWORD PTR [rsp+0x20],0x1
002951DB: call      0x1802a1240
002951E0: movzx     ecx,BYTE PTR [r14+0x484]
002951E8: test      cl,cl
002951EA: mov       BYTE PTR [r14+0x48b],0x1
002951F2: sete      al
002951F5: mov       BYTE PTR [r14+0x1210],al
002951FC: test      cl,cl
002951FE: jne       0x180295208
00295200: mov       rcx,r14
00295203: call      0x180266230
00295208: lea       rcx,[rbp+0x1a8]
0029520F: call      0x180179bf0
00295214: mov       rax,QWORD PTR [rbp+0x1a8]
0029521B: sub       rax,QWORD PTR [rbp-0x70]
0029521F: vxorps    xmm0,xmm0,xmm0
00295223: vcvtsi2sd xmm0,xmm0,rax
00295228: vmulsd    xmm0,xmm0,QWORD PTR [rip+0x176a70]        # 0x18040bca0
00295230: vcvtsd2ss xmm1,xmm0,xmm0
00295234: vmovss    DWORD PTR [r14+0x500],xmm1
0029523D: mov       rdi,QWORD PTR [rsp+0x298]
00295245: mov       rbx,QWORD PTR [rsp+0x290]
0029524D: mov       r15,QWORD PTR [rsp+0x260]
00295255: add       rsp,0x268
0029525C: pop       r14
0029525E: pop       r12
00295260: pop       rsi
00295261: pop       rbp
00295262: ret       
