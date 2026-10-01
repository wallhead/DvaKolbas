; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x2967A0..0x296CCB; unnamed
002967A0: mov       rax,rsp
002967A3: mov       QWORD PTR [rax+0x10],rbx
002967A7: push      rbp
002967A8: push      rsi
002967A9: push      rdi
002967AA: push      r14
002967AC: push      r15
002967AE: lea       rbp,[rax-0x58]
002967B2: sub       rsp,0x130
002967B9: vmovaps   XMMWORD PTR [rax-0x38],xmm6
002967BE: vmovaps   XMMWORD PTR [rax-0x48],xmm7
002967C3: vmovaps   XMMWORD PTR [rax-0x58],xmm8
002967C8: vmovaps   XMMWORD PTR [rax-0x68],xmm9
002967CD: vmovaps   XMMWORD PTR [rax-0x78],xmm10
002967D2: vmovaps   XMMWORD PTR [rax-0x88],xmm11
002967DA: mov       r14,rdx
002967DD: mov       rsi,rcx
002967E0: cmp       BYTE PTR [rcx+0x342],0x0
002967E7: je        0x180296c90
002967ED: call      0x1801abeb0
002967F2: mov       rbx,rax
002967F5: test      rax,rax
002967F8: je        0x180296c90
002967FE: call      0x180135920
00296803: nop       
00296804: call      0x180135920
00296809: movzx     edx,BYTE PTR [rax+0x118]
00296810: sub       edx,0x1
00296813: je        0x18029681f
00296815: cmp       edx,0x3
00296818: mov       edi,0x1cc
0029681D: je        0x180296824
0029681F: mov       edi,0x150
00296824: cmp       BYTE PTR [rdi+rbx*1+0x18],0x0
00296829: jne       0x180296c90
0029682F: lea       r8,[rbx+0xa0]
00296836: lea       rdx,[r14+0x280]
0029683D: mov       rcx,r14
00296840: call      0x180152930
00296845: test      al,al
00296847: je        0x180296c90
0029684D: vmovss    xmm2,DWORD PTR [rdi+rbx*1]
00296852: vmovss    xmm3,DWORD PTR [rdi+rbx*1+0x4]
00296858: vmovss    xmm4,DWORD PTR [rdi+rbx*1+0xc]
0029685E: vmovss    xmm5,DWORD PTR [rdi+rbx*1+0x8]
00296864: lea       r15,[rsi+0x460]
0029686B: mov       eax,DWORD PTR [rip+0x1ca86f]        # 0x1804610e0
00296871: cmp       DWORD PTR [r15],eax
00296874: jne       0x18029689f
00296876: cmp       BYTE PTR [rip+0x1df6eb],0x0        # 0x180475f68
0029687D: je        0x18029689f
0029687F: vmovss    xmm2,DWORD PTR [rip+0x1df6f1]        # 0x180475f78
00296887: vmovss    xmm3,DWORD PTR [rip+0x1df6ed]        # 0x180475f7c
0029688F: vmovss    xmm4,DWORD PTR [rip+0x1df6e9]        # 0x180475f80
00296897: vmovss    xmm5,DWORD PTR [rip+0x1df6e5]        # 0x180475f84
0029689F: vmovss    xmm0,DWORD PTR [rbx+0x84]
002968A7: vmovss    DWORD PTR [rsp+0x60],xmm0
002968AD: vmovss    xmm1,DWORD PTR [rbx+0x90]
002968B5: vmovss    DWORD PTR [rsp+0x64],xmm1
002968BB: vmovss    xmm0,DWORD PTR [rbx+0x9c]
002968C3: vmovss    DWORD PTR [rsp+0x68],xmm0
002968C9: vmovss    xmm1,DWORD PTR [rbx+0x80]
002968D1: vmovss    DWORD PTR [rsp+0x6c],xmm1
002968D7: vmovss    xmm0,DWORD PTR [rbx+0x8c]
002968DF: vmovss    DWORD PTR [rsp+0x70],xmm0
002968E5: vmovss    xmm1,DWORD PTR [rbx+0x98]
002968ED: vmovss    DWORD PTR [rsp+0x74],xmm1
002968F3: vmovss    xmm0,DWORD PTR [rbx+0x7c]
002968F8: vmovss    DWORD PTR [rsp+0x78],xmm0
002968FE: vmovss    xmm1,DWORD PTR [rbx+0x88]
00296906: vmovss    DWORD PTR [rsp+0x7c],xmm1
0029690C: vmovss    xmm0,DWORD PTR [rbx+0x94]
00296914: vmovss    DWORD PTR [rbp-0x80],xmm0
00296919: vmovss    DWORD PTR [rsp+0x40],xmm2
0029691F: vmovss    DWORD PTR [rsp+0x44],xmm3
00296925: vmovss    DWORD PTR [rsp+0x48],xmm4
0029692B: vmovss    DWORD PTR [rsp+0x4c],xmm5
00296931: mov       BYTE PTR [rsp+0x50],0x0
00296936: lea       rdi,[r14+0x180]
0029693D: xor       ebx,ebx
0029693F: mov       QWORD PTR [rsp+0x28],rbx
00296944: mov       QWORD PTR [rsp+0x20],rbx
00296949: lea       r9,[rsp+0x40]
0029694E: lea       r8,[rsp+0x60]
00296953: mov       rdx,rdi
00296956: mov       rcx,r14
00296959: call      0x180152a20
0029695E: test      eax,eax
00296960: jne       0x180296c90
00296966: vmovups   xmm4,XMMWORD PTR [rdi]
0029696A: vmovups   xmm6,XMMWORD PTR [rdi+0x20]
0029696F: vmovaps   ymm0,YMMWORD PTR [rip+0x175fc9]        # 0x18040c940
00296977: vmovups   YMMWORD PTR [rsp+0x60],ymm0
0029697D: vmovaps   ymm0,YMMWORD PTR [rip+0x17613b]        # 0x18040cac0
00296985: vmovups   YMMWORD PTR [rbp-0x80],ymm0
0029698A: vshufps   xmm2,xmm4,XMMWORD PTR [rdi+0x10],0x44
00296990: vshufps   xmm4,xmm4,XMMWORD PTR [rdi+0x10],0xee
00296996: vshufps   xmm1,xmm6,XMMWORD PTR [rdi+0x30],0x44
0029699C: vshufps   xmm3,xmm6,XMMWORD PTR [rdi+0x30],0xee
002969A2: vshufps   xmm0,xmm2,xmm1,0x88
002969A7: vshufps   xmm1,xmm2,xmm1,0xdd
002969AC: vshufps   xmm2,xmm4,xmm3,0x88
002969B1: vshufps   xmm3,xmm4,xmm3,0xdd
002969B6: lea       rcx,[rsp+0x60]
002969BB: vzeroupper 
002969BE: call      0x180152860
002969C3: mov       BYTE PTR [rsp+0x40],bl
002969C7: vxorps    xmm8,xmm8,xmm8
002969CC: vmovss    DWORD PTR [rsp+0x44],xmm8
002969D2: vmovss    DWORD PTR [rsp+0x48],xmm8
002969D8: lea       rdi,[rsp+0x60]
002969DD: nop       DWORD PTR [rax]
002969E0: vmovss    xmm0,DWORD PTR [rdi]
002969E4: call      0x180130d10
002969E9: test      al,al
002969EB: je        0x180296c90
002969F1: inc       ebx
002969F3: add       rdi,0x4
002969F7: cmp       ebx,0x10
002969FA: jb        0x1802969e0
002969FC: vmovss    xmm0,DWORD PTR [rbp-0x64]
00296A01: call      0x180130090
00296A06: vmovss    xmm6,DWORD PTR [rip+0x17521a]        # 0x18040bc28
00296A0E: vcomiss   xmm0,xmm6
00296A12: ja        0x180296c90
00296A18: vmovss    xmm7,DWORD PTR [rbp-0x74]
00296A1D: vmovaps   xmm0,xmm7
00296A21: call      0x180130090
00296A26: vcomiss   xmm0,xmm6
00296A2A: jb        0x180296c90
00296A30: vmovss    xmm6,DWORD PTR [rbp-0x68]
00296A35: vmovaps   xmm0,xmm6
00296A39: call      0x180130090
00296A3E: vcomiss   xmm0,DWORD PTR [rip+0x1751da]        # 0x18040bc20
00296A46: jb        0x180296c90
00296A4C: vmovss    xmm0,DWORD PTR [rsp+0x60]
00296A52: vcomiss   xmm0,xmm8
00296A57: jbe       0x180296c90
00296A5D: vmovss    xmm0,DWORD PTR [rsp+0x74]
00296A63: vcomiss   xmm0,xmm8
00296A68: jbe       0x180296c90
00296A6E: vmovss    xmm8,DWORD PTR [rbp-0x78]
00296A73: vcvtss2sd xmm8,xmm8,xmm8
00296A78: vcvtss2sd xmm11,xmm6,xmm6
00296A7C: vcvtss2sd xmm9,xmm7,xmm7
00296A80: vmovsd    xmm6,QWORD PTR [rip+0x175490]        # 0x18040bf18
00296A88: vucomisd  xmm8,QWORD PTR [rip+0x175490]        # 0x18040bf20
00296A90: jne       0x180296a98
00296A92: vmovaps   xmm7,xmm6
00296A96: jmp       0x180296ac1
00296A98: vmovaps   xmm1,xmm9
00296A9D: vmovsd    xmm0,QWORD PTR [rip+0x1752fb]        # 0x18040bda0
00296AA5: call      QWORD PTR [rip+0x2d1e5]        # 0x1802c3c90
00296AAB: vmovsd    xmm1,QWORD PTR [rip+0x17547d]        # 0x18040bf30
00296AB3: vdivsd    xmm1,xmm1,xmm8
00296AB8: vmulsd    xmm2,xmm1,xmm11
00296ABD: vmulsd    xmm7,xmm0,xmm2
00296AC1: vmovsd    QWORD PTR [rbp+0x60],xmm7
00296AC6: vsubsd    xmm10,xmm9,xmm8
00296ACB: vxorpd    xmm8,xmm8,xmm8
00296AD0: vucomisd  xmm10,xmm8
00296AD5: je        0x180296af8
00296AD7: vmovaps   xmm1,xmm9
00296ADC: vmovsd    xmm0,QWORD PTR [rip+0x1752bc]        # 0x18040bda0
00296AE4: call      QWORD PTR [rip+0x2d1a6]        # 0x1802c3c90
00296AEA: vdivsd    xmm1,xmm11,xmm10
00296AEF: vmulsd    xmm6,xmm0,xmm1
00296AF3: vmovsd    xmm7,QWORD PTR [rbp+0x60]
00296AF8: vmovsd    QWORD PTR [rbp+0x70],xmm6
00296AFD: vcomisd   xmm7,xmm8
00296B02: jbe       0x180296c90
00296B08: vcomisd   xmm6,xmm8
00296B0D: jbe       0x180296c90
00296B13: vucomisd  xmm7,xmm6
00296B17: je        0x180296c90
00296B1D: lea       rdx,[rbp+0x70]
00296B21: lea       rcx,[rbp+0x60]
00296B25: call      0x180157660
00296B2A: vmovsd    xmm8,QWORD PTR [rax]
00296B2E: lea       rdx,[rbp+0x70]
00296B32: lea       rcx,[rbp+0x60]
00296B36: call      0x18014ce70
00296B3B: vmovsd    xmm0,QWORD PTR [rax]
00296B3F: vmovsd    QWORD PTR [rbp-0x60],xmm0
00296B44: vmovaps   xmm0,xmm8
00296B49: call      0x180130d90
00296B4E: test      al,al
00296B50: je        0x180296c90
00296B56: vmovsd    xmm0,QWORD PTR [rip+0x1753a2]        # 0x18040bf00
00296B5E: vcomisd   xmm8,xmm0
00296B62: jae       0x180296c90
00296B68: vcomisd   xmm7,xmm6
00296B6C: seta      bl
00296B6F: vcvtsd2ss xmm6,xmm8,xmm8
00296B74: vmovsd    QWORD PTR [rbp+0x78],xmm0
00296B79: lea       rdx,[rbp+0x78]
00296B7D: lea       rcx,[rbp-0x60]
00296B81: call      0x180157660
00296B86: vmovsd    xmm7,QWORD PTR [rax]
00296B8A: vcvtpd2ps xmm7,xmm7
00296B8E: mov       BYTE PTR [rsp+0x40],bl
00296B92: movzx     eax,WORD PTR [rsp+0x41]
00296B97: mov       WORD PTR [rsp+0x41],ax
00296B9C: movzx     eax,BYTE PTR [rsp+0x43]
00296BA1: mov       BYTE PTR [rsp+0x43],al
00296BA5: vmovss    DWORD PTR [rsp+0x44],xmm6
00296BAB: vmovss    DWORD PTR [rsp+0x48],xmm7
00296BB1: movzx     eax,BYTE PTR [rsi+0x2fc]
00296BB8: cmp       al,bl
00296BBA: setne     dil
00296BBE: cmp       BYTE PTR [rsi+0x2fd],0x0
00296BC5: je        0x180296bcf
00296BC7: cmp       al,bl
00296BC9: je        0x180296c6c
00296BCF: lea       rcx,[rip+0x17034a]        # 0x180406f20
00296BD6: lea       rax,[rip+0x170333]        # 0x180406f10
00296BDD: test      bl,bl
00296BDF: cmovne    rax,rcx
00296BE3: mov       QWORD PTR [rbp+0x60],rax
00296BE7: call      0x180222050
00296BEC: lea       rcx,[rip+0x16f53d]        # 0x180406130
00296BF3: mov       QWORD PTR [rbp-0x50],rcx
00296BF7: mov       DWORD PTR [rbp-0x48],0x7a2
00296BFE: mov       ecx,DWORD PTR [rsp+0x6c]
00296C02: mov       DWORD PTR [rbp-0x44],ecx
00296C05: lea       rcx,[rip+0x1705b4]        # 0x1804071c0
00296C0C: mov       QWORD PTR [rbp-0x40],rcx
00296C10: lea       rcx,[rip+0x170569]        # 0x180407180
00296C17: mov       QWORD PTR [rbp-0x60],rcx
00296C1B: mov       QWORD PTR [rbp-0x58],0x31
00296C23: vmovups   xmm0,XMMWORD PTR [rbp-0x50]
00296C28: vmovups   XMMWORD PTR [rsp+0x60],xmm0
00296C2E: vmovsd    xmm1,QWORD PTR [rbp-0x40]
00296C33: vmovsd    QWORD PTR [rsp+0x70],xmm1
00296C39: mov       QWORD PTR [rsp+0x38],r15
00296C3E: lea       rcx,[rsp+0x48]
00296C43: mov       QWORD PTR [rsp+0x30],rcx
00296C48: lea       rcx,[rsp+0x44]
00296C4D: mov       QWORD PTR [rsp+0x28],rcx
00296C52: lea       rcx,[rbp+0x60]
00296C56: mov       QWORD PTR [rsp+0x20],rcx
00296C5B: lea       r9,[rbp-0x60]
00296C5F: lea       rdx,[rsp+0x60]
00296C64: mov       rcx,rax
00296C67: call      0x1801969f0
00296C6C: mov       BYTE PTR [rsi+0x2fd],0x1
00296C73: mov       BYTE PTR [rsi+0x2fc],bl
00296C79: vmovss    DWORD PTR [rsi+0x2f8],xmm6
00296C81: vmovss    DWORD PTR [rsi+0x2f4],xmm7
00296C89: or        BYTE PTR [rsi+0x2fe],dil
00296C90: lea       r11,[rsp+0x130]
00296C98: mov       rbx,QWORD PTR [r11+0x38]
00296C9C: vmovaps   xmm6,XMMWORD PTR [r11-0x10]
00296CA2: vmovaps   xmm7,XMMWORD PTR [r11-0x20]
00296CA8: vmovaps   xmm8,XMMWORD PTR [r11-0x30]
00296CAE: vmovaps   xmm9,XMMWORD PTR [r11-0x40]
00296CB4: vmovaps   xmm10,XMMWORD PTR [r11-0x50]
00296CBA: vmovaps   xmm11,XMMWORD PTR [r11-0x60]
00296CC0: mov       rsp,r11
00296CC3: pop       r15
00296CC5: pop       r14
00296CC7: pop       rdi
00296CC8: pop       rsi
00296CC9: pop       rbp
00296CCA: ret       
