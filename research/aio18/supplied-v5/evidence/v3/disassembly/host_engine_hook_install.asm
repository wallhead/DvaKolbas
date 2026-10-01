; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x1A0900..0x1A25AF; unnamed
001A0900: mov       rax,rsp
001A0903: push      rbp
001A0904: push      rbx
001A0905: push      rsi
001A0906: push      rdi
001A0907: push      r12
001A0909: push      r13
001A090B: push      r14
001A090D: push      r15
001A090F: lea       rbp,[rax-0x198]
001A0916: sub       rsp,0x258
001A091D: vmovaps   XMMWORD PTR [rax-0x58],xmm6
001A0922: vmovaps   XMMWORD PTR [rax-0x68],xmm7
001A0927: call      0x1801b2350
001A092C: test      eax,eax
001A092E: je        0x1801a0939
001A0930: cmp       eax,0x1
001A0933: jne       0x1801a24d1
001A0939: call      0x180261760
001A093E: call      0x18027cda0
001A0943: lea       rcx,[rip+0x2664be]        # 0x180406e08
001A094A: call      QWORD PTR [rip+0x122918]        # 0x1802c3268
001A0950: call      0x18017cc20
001A0955: mov       r12,rax
001A0958: test      rax,rax
001A095B: setne     cl
001A095E: mov       rax,QWORD PTR [rip+0xce047b]        # 0x180e80de0
001A0965: mov       BYTE PTR [rax+0x1689],cl
001A096B: mov       r13d,0x11
001A0971: lea       r15,[rip+0x2692a0]        # 0x180409c18
001A0978: lea       rdi,[rip+0x26a5c9]        # 0x18040af48
001A097F: test      r12,r12
001A0982: je        0x1801a0a34
001A0988: lea       rdx,[rip+0x26a549]        # 0x18040aed8
001A098F: mov       rcx,r12
001A0992: call      QWORD PTR [rip+0x1227c0]        # 0x1802c3158
001A0998: test      rax,rax
001A099B: setne     dl
001A099E: mov       rcx,QWORD PTR [rip+0xce043b]        # 0x180e80de0
001A09A5: mov       BYTE PTR [rcx+0x168a],dl
001A09AB: test      rax,rax
001A09AE: je        0x1801a09d7
001A09B0: call      0x180222050
001A09B5: mov       DWORD PTR [rsp+0x58],0x973
001A09BD: mov       ecx,DWORD PTR [rsp+0x7c]
001A09C1: mov       DWORD PTR [rsp+0x5c],ecx
001A09C5: lea       rcx,[rip+0x26a55c]        # 0x18040af28
001A09CC: mov       QWORD PTR [rsp+0x48],0x18
001A09D5: jmp       0x1801a09f8
001A09D7: call      0x180222050
001A09DC: mov       DWORD PTR [rsp+0x58],0x975
001A09E4: mov       ecx,DWORD PTR [rsp+0x7c]
001A09E8: mov       DWORD PTR [rsp+0x5c],ecx
001A09EC: lea       rcx,[rip+0x26a59d]        # 0x18040af90
001A09F3: mov       QWORD PTR [rsp+0x48],r13
001A09F8: mov       QWORD PTR [rsp+0x50],r15
001A09FD: mov       QWORD PTR [rsp+0x60],rdi
001A0A02: vmovups   xmm0,XMMWORD PTR [rsp+0x50]
001A0A08: vmovsd    xmm1,QWORD PTR [rsp+0x60]
001A0A0E: mov       QWORD PTR [rsp+0x40],rcx
001A0A13: vmovups   XMMWORD PTR [rbp-0x40],xmm0
001A0A18: vmovsd    QWORD PTR [rbp-0x30],xmm1
001A0A1D: lea       r9,[rsp+0x40]
001A0A22: mov       r8d,0x2
001A0A28: lea       rdx,[rbp-0x40]
001A0A2C: mov       rcx,rax
001A0A2F: call      0x180151990
001A0A34: lea       rcx,[rip+0x269ec5]        # 0x18040a900
001A0A3B: call      QWORD PTR [rip+0x122827]        # 0x1802c3268
001A0A41: lea       rdx,[rip+0x26a530]        # 0x18040af78
001A0A48: mov       rcx,rax
001A0A4B: call      QWORD PTR [rip+0x122707]        # 0x1802c3158
001A0A51: mov       QWORD PTR [rip+0x2dc9a8],rax        # 0x18047d400
001A0A58: lea       r8,[rip+0x2dc9a1]        # 0x18047d400
001A0A5F: lea       rdx,[rip+0x105b0a]        # 0x1802a6570
001A0A66: mov       rcx,rax
001A0A69: call      0x1801b20b0
001A0A6E: test      eax,eax
001A0A70: je        0x1801a0aea
001A0A72: mov       DWORD PTR [rbp+0x1a0],eax
001A0A78: call      0x180222050
001A0A7D: mov       QWORD PTR [rsp+0x50],r15
001A0A82: mov       DWORD PTR [rsp+0x58],0x97d
001A0A8A: mov       ecx,DWORD PTR [rsp+0x7c]
001A0A8E: mov       DWORD PTR [rsp+0x5c],ecx
001A0A92: mov       QWORD PTR [rsp+0x60],rdi
001A0A97: lea       rcx,[rip+0x26a542]        # 0x18040afe0
001A0A9E: mov       QWORD PTR [rsp+0x40],rcx
001A0AA3: mov       QWORD PTR [rsp+0x48],0x22
001A0AAC: vmovups   xmm0,XMMWORD PTR [rsp+0x50]
001A0AB2: vmovups   XMMWORD PTR [rbp-0x40],xmm0
001A0AB7: vmovsd    xmm1,QWORD PTR [rsp+0x60]
001A0ABD: vmovsd    QWORD PTR [rbp-0x30],xmm1
001A0AC2: lea       rcx,[rbp+0x1a0]
001A0AC9: mov       QWORD PTR [rsp+0x20],rcx
001A0ACE: lea       r9,[rsp+0x40]
001A0AD3: mov       r8d,0x4
001A0AD9: lea       rdx,[rbp-0x40]
001A0ADD: mov       rcx,rax
001A0AE0: call      0x180169990
001A0AE5: jmp       0x1801a1134
001A0AEA: xor       ecx,ecx
001A0AEC: call      0x1801b2340
001A0AF1: test      eax,eax
001A0AF3: jne       0x1801a24f4
001A0AF9: xor       edx,edx
001A0AFB: mov       r8d,0x104
001A0B01: lea       rcx,[rbp+0x20]
001A0B05: call      0x18023a82c
001A0B0A: mov       edx,0x104
001A0B0F: lea       rcx,[rbp+0x20]
001A0B13: call      QWORD PTR [rip+0x122757]        # 0x1802c3270
001A0B19: test      eax,eax
001A0B1B: je        0x1801a2517
001A0B21: vpxor     xmm0,xmm0,xmm0
001A0B25: vmovups   XMMWORD PTR [rbp+0x0],xmm0
001A0B2A: vpxor     xmm1,xmm1,xmm1
001A0B2E: vmovdqu   XMMWORD PTR [rbp+0x10],xmm1
001A0B33: lea       rax,[rbp+0x20]
001A0B37: mov       rbx,0xffffffffffffffff
001A0B3E: mov       rsi,rbx
001A0B41: inc       rsi
001A0B44: cmp       BYTE PTR [rax+rsi*1],0x0
001A0B48: jne       0x1801a0b41
001A0B4A: movabs    r14,0x7fffffffffffffff
001A0B54: cmp       rsi,r14
001A0B57: ja        0x1801a253a
001A0B5D: mov       eax,0x16
001A0B62: cmp       rsi,0xf
001A0B66: ja        0x1801a0b8b
001A0B68: mov       QWORD PTR [rbp+0x10],rsi
001A0B6C: mov       QWORD PTR [rbp+0x18],0xf
001A0B74: mov       r8,rsi
001A0B77: lea       rdx,[rbp+0x20]
001A0B7B: lea       rcx,[rbp+0x0]
001A0B7F: call      0x18023a826
001A0B84: mov       BYTE PTR [rbp+rsi*1+0x0],0x0
001A0B89: jmp       0x1801a0bdc
001A0B8B: mov       r15,rsi
001A0B8E: or        r15,0xf
001A0B92: cmp       r15,r14
001A0B95: jbe       0x1801a0b9c
001A0B97: mov       r15,r14
001A0B9A: jmp       0x1801a0ba3
001A0B9C: cmp       r15,rax
001A0B9F: cmovb     r15,rax
001A0BA3: lea       rcx,[r15+0x1]
001A0BA7: call      0x18013dee0
001A0BAC: mov       rdi,rax
001A0BAF: mov       QWORD PTR [rbp+0x0],rax
001A0BB3: mov       QWORD PTR [rbp+0x10],rsi
001A0BB7: mov       QWORD PTR [rbp+0x18],r15
001A0BBB: mov       r8,rsi
001A0BBE: lea       rdx,[rbp+0x20]
001A0BC2: mov       rcx,rax
001A0BC5: call      0x18023a826
001A0BCA: mov       BYTE PTR [rdi+rsi*1],0x0
001A0BCE: lea       r15,[rip+0x269043]        # 0x180409c18
001A0BD5: lea       rdi,[rip+0x26a36c]        # 0x18040af48
001A0BDC: mov       r8d,0xa
001A0BE2: lea       rdx,[rip+0x26a41f]        # 0x18040b008
001A0BE9: lea       rcx,[rbp+0x0]
001A0BED: call      0x180139150
001A0BF2: vpxor     xmm0,xmm0,xmm0
001A0BF6: vmovups   XMMWORD PTR [rbp-0x20],xmm0
001A0BFB: vpxor     xmm1,xmm1,xmm1
001A0BFF: vmovdqu   XMMWORD PTR [rbp-0x10],xmm1
001A0C04: vmovups   ymm0,YMMWORD PTR [rax]
001A0C08: vmovups   YMMWORD PTR [rbp-0x20],ymm0
001A0C0D: mov       BYTE PTR [rax],0x0
001A0C10: xor       esi,esi
001A0C12: mov       QWORD PTR [rax+0x10],rsi
001A0C16: mov       QWORD PTR [rax+0x18],0xf
001A0C1E: mov       rdx,QWORD PTR [rbp+0x18]
001A0C22: cmp       rdx,0xf
001A0C26: jbe       0x1801a0c71
001A0C28: inc       rdx
001A0C2B: mov       rcx,QWORD PTR [rbp+0x0]
001A0C2F: mov       rax,rcx
001A0C32: cmp       rdx,0x1000
001A0C39: jb        0x1801a0c69
001A0C3B: add       rdx,0x27
001A0C3F: mov       rcx,QWORD PTR [rcx-0x8]
001A0C43: sub       rax,rcx
001A0C46: sub       rax,0x8
001A0C4A: cmp       rax,0x1f
001A0C4E: jbe       0x1801a0c69
001A0C50: mov       QWORD PTR [rsp+0x20],rsi
001A0C55: xor       r9d,r9d
001A0C58: xor       r8d,r8d
001A0C5B: xor       edx,edx
001A0C5D: xor       ecx,ecx
001A0C5F: vzeroupper 
001A0C62: call      QWORD PTR [rip+0x123120]        # 0x1802c3d88
001A0C68: int3      
001A0C69: vzeroupper 
001A0C6C: call      0x18023927c
001A0C71: vmovdqu   xmm6,XMMWORD PTR [rip+0x26b817]        # 0x18040c490
001A0C79: vmovdqu   XMMWORD PTR [rbp+0x10],xmm6
001A0C7E: mov       BYTE PTR [rbp+0x0],sil
001A0C82: lea       rcx,[rbp-0x20]
001A0C86: cmp       QWORD PTR [rbp-0x8],0xf
001A0C8B: cmova     rcx,QWORD PTR [rbp-0x20]
001A0C90: vzeroupper 
001A0C93: call      QWORD PTR [rip+0x1225cf]        # 0x1802c3268
001A0C99: test      rax,rax
001A0C9C: je        0x1801a2540
001A0CA2: lea       rdx,[rip+0x26a39f]        # 0x18040b048
001A0CA9: mov       rcx,rax
001A0CAC: call      QWORD PTR [rip+0x1224a6]        # 0x1802c3158
001A0CB2: mov       QWORD PTR [rip+0x2dc7df],rax        # 0x18047d498
001A0CB9: mov       r8,r12
001A0CBC: lea       rcx,[rsp+0x70]
001A0CC1: call      0x18019d3f0
001A0CC6: vmovups   xmm7,XMMWORD PTR [rax]
001A0CCA: vmovups   XMMWORD PTR [rbp-0x40],xmm7
001A0CCF: vmovsd    xmm0,QWORD PTR [rax+0x10]
001A0CD4: vmovsd    QWORD PTR [rbp-0x30],xmm0
001A0CD9: vmovq     rcx,xmm7
001A0CDE: test      rcx,rcx
001A0CE1: jne       0x1801a0d54
001A0CE3: call      0x180222050
001A0CE8: mov       QWORD PTR [rsp+0x50],r15
001A0CED: mov       DWORD PTR [rsp+0x58],0x991
001A0CF5: mov       ecx,DWORD PTR [rsp+0x7c]
001A0CF9: mov       DWORD PTR [rsp+0x5c],ecx
001A0CFD: mov       QWORD PTR [rsp+0x60],rdi
001A0D02: lea       rcx,[rip+0x26a3af]        # 0x18040b0b8
001A0D09: mov       QWORD PTR [rsp+0x40],rcx
001A0D0E: mov       QWORD PTR [rsp+0x48],0x2f
001A0D17: vmovups   xmm0,XMMWORD PTR [rsp+0x50]
001A0D1D: vmovups   XMMWORD PTR [rsp+0x70],xmm0
001A0D23: vmovsd    xmm1,QWORD PTR [rsp+0x60]
001A0D29: vmovsd    QWORD PTR [rbp-0x80],xmm1
001A0D2E: lea       rcx,[rbp-0x30]
001A0D32: mov       QWORD PTR [rsp+0x20],rcx
001A0D37: lea       r9,[rsp+0x40]
001A0D3C: mov       r8d,0x4
001A0D42: lea       rdx,[rsp+0x70]
001A0D47: mov       rcx,rax
001A0D4A: call      0x180151d80
001A0D4F: jmp       0x1801a0ea1
001A0D54: mov       QWORD PTR [rbp+0x1b0],rsi
001A0D5B: lea       r8,[rbp+0x1b0]
001A0D62: lea       rdx,[rip+0x1051d7]        # 0x1802a5f40
001A0D69: call      0x1801b20b0
001A0D6E: test      eax,eax
001A0D70: jne       0x1801a0e2c
001A0D76: mov       rax,QWORD PTR [rbp+0x1b0]
001A0D7D: mov       QWORD PTR [rip+0x2dc714],rax        # 0x18047d498
001A0D84: mov       QWORD PTR [rbp+0x1a0],rax
001A0D8B: vpextrb   ecx,xmm7,0x8
001A0D91: lea       rdx,[rip+0x26a308]        # 0x18040b0a0
001A0D98: lea       rax,[rip+0x26a389]        # 0x18040b128
001A0D9F: test      cl,cl
001A0DA1: cmovne    rax,rdx
001A0DA5: mov       QWORD PTR [rbp+0x1b8],rax
001A0DAC: call      0x180222050
001A0DB1: mov       QWORD PTR [rsp+0x50],r15
001A0DB6: mov       DWORD PTR [rsp+0x58],0x999
001A0DBE: mov       ecx,DWORD PTR [rsp+0x7c]
001A0DC2: mov       DWORD PTR [rsp+0x5c],ecx
001A0DC6: mov       QWORD PTR [rsp+0x60],rdi
001A0DCB: lea       rcx,[rip+0x26a316]        # 0x18040b0e8
001A0DD2: mov       QWORD PTR [rsp+0x40],rcx
001A0DD7: mov       QWORD PTR [rsp+0x48],0x3d
001A0DE0: vmovups   xmm0,XMMWORD PTR [rsp+0x50]
001A0DE6: vmovups   XMMWORD PTR [rsp+0x70],xmm0
001A0DEC: vmovsd    xmm1,QWORD PTR [rsp+0x60]
001A0DF2: vmovsd    QWORD PTR [rbp-0x80],xmm1
001A0DF7: lea       rcx,[rbp+0x1a0]
001A0DFE: mov       QWORD PTR [rsp+0x30],rcx
001A0E03: lea       rcx,[rbp+0x1b8]
001A0E0A: mov       QWORD PTR [rsp+0x28],rcx
001A0E0F: lea       rcx,[rbp-0x40]
001A0E13: mov       QWORD PTR [rsp+0x20],rcx
001A0E18: lea       r9,[rsp+0x40]
001A0E1D: lea       rdx,[rsp+0x70]
001A0E22: mov       rcx,rax
001A0E25: call      0x1801a56e0
001A0E2A: jmp       0x1801a0ea1
001A0E2C: mov       DWORD PTR [rbp+0x1a0],eax
001A0E32: call      0x180222050
001A0E37: mov       QWORD PTR [rsp+0x50],r15
001A0E3C: mov       DWORD PTR [rsp+0x58],0x99d
001A0E44: mov       ecx,DWORD PTR [rsp+0x7c]
001A0E48: mov       DWORD PTR [rsp+0x5c],ecx
001A0E4C: mov       QWORD PTR [rsp+0x60],rdi
001A0E51: lea       rcx,[rip+0x26a2f0]        # 0x18040b148
001A0E58: mov       QWORD PTR [rsp+0x40],rcx
001A0E5D: mov       QWORD PTR [rsp+0x48],0x2d
001A0E66: vmovups   xmm0,XMMWORD PTR [rsp+0x50]
001A0E6C: vmovups   XMMWORD PTR [rsp+0x70],xmm0
001A0E72: vmovsd    xmm1,QWORD PTR [rsp+0x60]
001A0E78: vmovsd    QWORD PTR [rbp-0x80],xmm1
001A0E7D: lea       rcx,[rbp+0x1a0]
001A0E84: mov       QWORD PTR [rsp+0x20],rcx
001A0E89: lea       r9,[rsp+0x40]
001A0E8E: mov       r8d,0x4
001A0E94: lea       rdx,[rsp+0x70]
001A0E99: mov       rcx,rax
001A0E9C: call      0x180169990
001A0EA1: vpxor     xmm0,xmm0,xmm0
001A0EA5: vmovups   XMMWORD PTR [rbp-0x40],xmm0
001A0EAA: vpxor     xmm1,xmm1,xmm1
001A0EAE: vmovdqu   XMMWORD PTR [rbp-0x30],xmm1
001A0EB3: lea       rax,[rbp+0x20]
001A0EB7: inc       rbx
001A0EBA: cmp       BYTE PTR [rax+rbx*1],sil
001A0EBE: jne       0x1801a0eb7
001A0EC0: cmp       rbx,r14
001A0EC3: ja        0x1801a2563
001A0EC9: cmp       rbx,0xf
001A0ECD: ja        0x1801a0ef2
001A0ECF: mov       QWORD PTR [rbp-0x30],rbx
001A0ED3: mov       QWORD PTR [rbp-0x28],0xf
001A0EDB: mov       r8,rbx
001A0EDE: lea       rdx,[rbp+0x20]
001A0EE2: lea       rcx,[rbp-0x40]
001A0EE6: call      0x18023a826
001A0EEB: mov       BYTE PTR [rbp+rbx*1-0x40],sil
001A0EF0: jmp       0x1801a0f40
001A0EF2: mov       rax,rbx
001A0EF5: or        rax,0xf
001A0EF9: cmp       rax,r14
001A0EFC: ja        0x1801a0f0e
001A0EFE: mov       r14,rax
001A0F01: cmp       rax,0x16
001A0F05: mov       eax,0x16
001A0F0A: cmovb     r14,rax
001A0F0E: lea       rcx,[r14+0x1]
001A0F12: call      0x18013dee0
001A0F17: mov       rdi,rax
001A0F1A: mov       QWORD PTR [rbp-0x40],rax
001A0F1E: mov       QWORD PTR [rbp-0x30],rbx
001A0F22: mov       QWORD PTR [rbp-0x28],r14
001A0F26: mov       r8,rbx
001A0F29: lea       rdx,[rbp+0x20]
001A0F2D: mov       rcx,rax
001A0F30: call      0x18023a826
001A0F35: mov       BYTE PTR [rdi+rbx*1],sil
001A0F39: lea       rdi,[rip+0x26a008]        # 0x18040af48
001A0F40: mov       r8d,0x9
001A0F46: lea       rdx,[rip+0x26a1eb]        # 0x18040b138
001A0F4D: lea       rcx,[rbp-0x40]
001A0F51: call      0x180139150
001A0F56: vpxor     xmm0,xmm0,xmm0
001A0F5A: vmovups   XMMWORD PTR [rbp-0x60],xmm0
001A0F5F: vpxor     xmm1,xmm1,xmm1
001A0F63: vmovdqu   XMMWORD PTR [rbp-0x50],xmm1
001A0F68: vmovups   ymm0,YMMWORD PTR [rax]
001A0F6C: vmovups   YMMWORD PTR [rbp-0x60],ymm0
001A0F71: mov       BYTE PTR [rax],0x0
001A0F74: mov       QWORD PTR [rax+0x10],rsi
001A0F78: mov       QWORD PTR [rax+0x18],0xf
001A0F80: mov       rdx,QWORD PTR [rbp-0x28]
001A0F84: cmp       rdx,0xf
001A0F88: jbe       0x1801a0fd3
001A0F8A: inc       rdx
001A0F8D: mov       rcx,QWORD PTR [rbp-0x40]
001A0F91: mov       rax,rcx
001A0F94: cmp       rdx,0x1000
001A0F9B: jb        0x1801a0fcb
001A0F9D: add       rdx,0x27
001A0FA1: mov       rcx,QWORD PTR [rcx-0x8]
001A0FA5: sub       rax,rcx
001A0FA8: sub       rax,0x8
001A0FAC: cmp       rax,0x1f
001A0FB0: jbe       0x1801a0fcb
001A0FB2: mov       QWORD PTR [rsp+0x20],rsi
001A0FB7: xor       r9d,r9d
001A0FBA: xor       r8d,r8d
001A0FBD: xor       edx,edx
001A0FBF: xor       ecx,ecx
001A0FC1: vzeroupper 
001A0FC4: call      QWORD PTR [rip+0x122dbe]        # 0x1802c3d88
001A0FCA: int3      
001A0FCB: vzeroupper 
001A0FCE: call      0x18023927c
001A0FD3: vmovdqu   XMMWORD PTR [rbp-0x30],xmm6
001A0FD8: mov       BYTE PTR [rbp-0x40],0x0
001A0FDC: mov       BYTE PTR [rip+0xcdff46],0x1        # 0x180e80f29
001A0FE3: lea       rcx,[rbp-0x60]
001A0FE7: cmp       QWORD PTR [rbp-0x48],0xf
001A0FEC: cmova     rcx,QWORD PTR [rbp-0x60]
001A0FF1: vzeroupper 
001A0FF4: call      QWORD PTR [rip+0x12226e]        # 0x1802c3268
001A0FFA: mov       BYTE PTR [rip+0xcdff28],0x0        # 0x180e80f29
001A1001: test      rax,rax
001A1004: je        0x1801a2569
001A100A: lea       rdx,[rip+0x26a167]        # 0x18040b178
001A1011: mov       rcx,rax
001A1014: call      QWORD PTR [rip+0x12213e]        # 0x1802c3158
001A101A: mov       QWORD PTR [rip+0x2dc457],rax        # 0x18047d478
001A1021: lea       r8,[rip+0x2dc450]        # 0x18047d478
001A1028: lea       rdx,[rip+0x106891]        # 0x1802a78c0
001A102F: mov       rcx,rax
001A1032: call      0x1801b20b0
001A1037: test      eax,eax
001A1039: je        0x1801a115a
001A103F: mov       DWORD PTR [rbp+0x1a0],eax
001A1045: call      0x180222050
001A104A: mov       QWORD PTR [rsp+0x50],r15
001A104F: mov       DWORD PTR [rsp+0x58],0x9ab
001A1057: mov       ecx,DWORD PTR [rsp+0x7c]
001A105B: mov       DWORD PTR [rsp+0x5c],ecx
001A105F: mov       QWORD PTR [rsp+0x60],rdi
001A1064: lea       rcx,[rip+0x26a15d]        # 0x18040b1c8
001A106B: mov       QWORD PTR [rsp+0x40],rcx
001A1070: mov       QWORD PTR [rsp+0x48],0x27
001A1079: vmovups   xmm0,XMMWORD PTR [rsp+0x50]
001A107F: vmovups   XMMWORD PTR [rsp+0x70],xmm0
001A1085: vmovsd    xmm1,QWORD PTR [rsp+0x60]
001A108B: vmovsd    QWORD PTR [rbp-0x80],xmm1
001A1090: lea       rcx,[rbp+0x1a0]
001A1097: mov       QWORD PTR [rsp+0x20],rcx
001A109C: lea       r9,[rsp+0x40]
001A10A1: mov       r8d,0x4
001A10A7: lea       rdx,[rsp+0x70]
001A10AC: mov       rcx,rax
001A10AF: call      0x180169990
001A10B4: nop       
001A10B5: mov       rdx,QWORD PTR [rbp-0x48]
001A10B9: cmp       rdx,0xf
001A10BD: jbe       0x1801a10f0
001A10BF: inc       rdx
001A10C2: mov       rcx,QWORD PTR [rbp-0x60]
001A10C6: mov       rax,rcx
001A10C9: cmp       rdx,0x1000
001A10D0: jb        0x1801a10eb
001A10D2: add       rdx,0x27
001A10D6: mov       rcx,QWORD PTR [rcx-0x8]
001A10DA: sub       rax,rcx
001A10DD: sub       rax,0x8
001A10E1: cmp       rax,0x1f
001A10E5: ja        0x1801a1405
001A10EB: call      0x18023927c
001A10F0: vmovdqu   XMMWORD PTR [rbp-0x50],xmm6
001A10F5: mov       BYTE PTR [rbp-0x60],0x0
001A10F9: mov       rdx,QWORD PTR [rbp-0x8]
001A10FD: cmp       rdx,0xf
001A1101: jbe       0x1801a1134
001A1103: inc       rdx
001A1106: mov       rcx,QWORD PTR [rbp-0x20]
001A110A: mov       rax,rcx
001A110D: cmp       rdx,0x1000
001A1114: jb        0x1801a112f
001A1116: add       rdx,0x27
001A111A: mov       rcx,QWORD PTR [rcx-0x8]
001A111E: sub       rax,rcx
001A1121: sub       rax,0x8
001A1125: cmp       rax,0x1f
001A1129: ja        0x1801a1467
001A112F: call      0x18023927c
001A1134: vmovaps   xmm6,XMMWORD PTR [rsp+0x240]
001A113D: vmovaps   xmm7,XMMWORD PTR [rsp+0x230]
001A1146: add       rsp,0x258
001A114D: pop       r15
001A114F: pop       r14
001A1151: pop       r13
001A1153: pop       r12
001A1155: pop       rdi
001A1156: pop       rsi
001A1157: pop       rbx
001A1158: pop       rbp
001A1159: ret       
001A115A: xor       ecx,ecx
001A115C: call      0x1801b2340
001A1161: test      eax,eax
001A1163: jne       0x1801a258c
001A1169: test      r12,r12
001A116C: jne       0x1801a11a6
001A116E: lea       rdx,[rip+0x26a04b]        # 0x18040b1c0
001A1175: lea       rcx,[rip+0x26a0b4]        # 0x18040b230
001A117C: call      QWORD PTR [rip+0x1220f6]        # 0x1802c3278
001A1182: lea       rdx,[rip+0x26a037]        # 0x18040b1c0
001A1189: lea       rcx,[rip+0x26a060]        # 0x18040b1f0
001A1190: call      QWORD PTR [rip+0x1220e2]        # 0x1802c3278
001A1196: lea       rcx,[rip+0x26a103]        # 0x18040b2a0
001A119D: call      QWORD PTR [rip+0x1220c5]        # 0x1802c3268
001A11A3: mov       r12,rsi
001A11A6: lea       rcx,[rip+0x26a0f3]        # 0x18040b2a0
001A11AD: call      QWORD PTR [rip+0x121f8d]        # 0x1802c3140
001A11B3: mov       rbx,rax
001A11B6: test      rax,rax
001A11B9: je        0x1801a131f
001A11BF: mov       rcx,QWORD PTR [rip+0xcdfc1a]        # 0x180e80de0
001A11C6: mov       WORD PTR [rcx+0x1689],0x0
001A11CF: lea       rdx,[rip+0x268842]        # 0x180409a18
001A11D6: mov       rcx,rax
001A11D9: call      QWORD PTR [rip+0x121f79]        # 0x1802c3158
001A11DF: mov       QWORD PTR [rip+0x2dc232],rax        # 0x18047d418
001A11E6: lea       r8,[rip+0x2dc22b]        # 0x18047d418
001A11ED: lea       rdx,[rip+0x1056cc]        # 0x1802a68c0
001A11F4: mov       rcx,rax
001A11F7: call      0x1801b20b0
001A11FC: test      eax,eax
001A11FE: je        0x1801a12ba
001A1204: mov       DWORD PTR [rbp+0x1a0],eax
001A120A: call      0x180222050
001A120F: mov       DWORD PTR [rsp+0x58],0x9d6
001A1217: mov       ecx,DWORD PTR [rsp+0x7c]
001A121B: mov       DWORD PTR [rsp+0x5c],ecx
001A121F: lea       rcx,[rip+0x26a0c2]        # 0x18040b2e8
001A1226: mov       QWORD PTR [rsp+0x48],0x2a
001A122F: mov       QWORD PTR [rsp+0x40],rcx
001A1234: mov       QWORD PTR [rsp+0x50],r15
001A1239: mov       QWORD PTR [rsp+0x60],rdi
001A123E: lea       rcx,[rbp+0x1a0]
001A1245: vmovups   xmm0,XMMWORD PTR [rsp+0x50]
001A124B: vmovsd    xmm1,QWORD PTR [rsp+0x60]
001A1251: mov       QWORD PTR [rsp+0x20],rcx
001A1256: vmovups   XMMWORD PTR [rsp+0x70],xmm0
001A125C: vmovsd    QWORD PTR [rbp-0x80],xmm1
001A1261: lea       r9,[rsp+0x40]
001A1266: mov       r8d,0x4
001A126C: lea       rdx,[rsp+0x70]
001A1271: mov       rcx,rax
001A1274: call      0x180169990
001A1279: nop       
001A127A: mov       r8,QWORD PTR [rbp-0x48]
001A127E: cmp       r8,0xf
001A1282: jbe       0x1801a1291
001A1284: mov       rdx,QWORD PTR [rbp-0x60]
001A1288: lea       rcx,[rbp-0x60]
001A128C: call      0x18013cc30
001A1291: vmovdqu   XMMWORD PTR [rbp-0x50],xmm6
001A1296: mov       BYTE PTR [rbp-0x60],0x0
001A129A: mov       r8,QWORD PTR [rbp-0x8]
001A129E: cmp       r8,0xf
001A12A2: jbe       0x1801a1134
001A12A8: mov       rdx,QWORD PTR [rbp-0x20]
001A12AC: lea       rcx,[rbp-0x20]
001A12B0: call      0x18013cc30
001A12B5: jmp       0x1801a1134
001A12BA: lea       rdx,[rip+0x268777]        # 0x180409a38
001A12C1: mov       rcx,rbx
001A12C4: call      QWORD PTR [rip+0x121e8e]        # 0x1802c3158
001A12CA: mov       QWORD PTR [rip+0x2dc14f],rax        # 0x18047d420
001A12D1: lea       r8,[rip+0x2dc148]        # 0x18047d420
001A12D8: lea       rdx,[rip+0x105791]        # 0x1802a6a70
001A12DF: mov       rcx,rax
001A12E2: call      0x1801b20b0
001A12E7: test      eax,eax
001A12E9: je        0x1801a147d
001A12EF: mov       DWORD PTR [rbp+0x1a0],eax
001A12F5: call      0x180222050
001A12FA: mov       DWORD PTR [rsp+0x58],0x9db
001A1302: mov       ecx,DWORD PTR [rsp+0x7c]
001A1306: mov       DWORD PTR [rsp+0x5c],ecx
001A130A: lea       rcx,[rip+0x269f9f]        # 0x18040b2b0
001A1311: mov       QWORD PTR [rsp+0x48],0x34
001A131A: jmp       0x1801a122f
001A131F: test      r12,r12
001A1322: je        0x1801a147d
001A1328: lea       rdx,[rip+0x269e49]        # 0x18040b178
001A132F: mov       rcx,r12
001A1332: call      QWORD PTR [rip+0x121e20]        # 0x1802c3158
001A1338: mov       QWORD PTR [rip+0x2dc151],rax        # 0x18047d490
001A133F: lea       r8,[rip+0x2dc14a]        # 0x18047d490
001A1346: lea       rdx,[rip+0x106783]        # 0x1802a7ad0
001A134D: mov       rcx,rax
001A1350: call      0x1801b20b0
001A1355: test      eax,eax
001A1357: je        0x1801a147d
001A135D: mov       DWORD PTR [rbp+0x1a0],eax
001A1363: call      0x180222050
001A1368: mov       QWORD PTR [rsp+0x50],r15
001A136D: mov       DWORD PTR [rsp+0x58],0x9d0
001A1375: mov       ecx,DWORD PTR [rsp+0x7c]
001A1379: mov       DWORD PTR [rsp+0x5c],ecx
001A137D: mov       QWORD PTR [rsp+0x60],rdi
001A1382: lea       rcx,[rip+0x269ee7]        # 0x18040b270
001A1389: mov       QWORD PTR [rsp+0x40],rcx
001A138E: mov       QWORD PTR [rsp+0x48],0x2a
001A1397: vmovups   xmm0,XMMWORD PTR [rsp+0x50]
001A139D: vmovups   XMMWORD PTR [rsp+0x70],xmm0
001A13A3: vmovsd    xmm1,QWORD PTR [rsp+0x60]
001A13A9: vmovsd    QWORD PTR [rbp-0x80],xmm1
001A13AE: lea       rcx,[rbp+0x1a0]
001A13B5: mov       QWORD PTR [rsp+0x20],rcx
001A13BA: lea       r9,[rsp+0x40]
001A13BF: mov       r8d,0x4
001A13C5: lea       rdx,[rsp+0x70]
001A13CA: mov       rcx,rax
001A13CD: call      0x180169990
001A13D2: nop       
001A13D3: mov       rdx,QWORD PTR [rbp-0x48]
001A13D7: cmp       rdx,0xf
001A13DB: jbe       0x1801a1420
001A13DD: inc       rdx
001A13E0: mov       rcx,QWORD PTR [rbp-0x60]
001A13E4: mov       rax,rcx
001A13E7: cmp       rdx,0x1000
001A13EE: jb        0x1801a141b
001A13F0: add       rdx,0x27
001A13F4: mov       rcx,QWORD PTR [rcx-0x8]
001A13F8: sub       rax,rcx
001A13FB: sub       rax,0x8
001A13FF: cmp       rax,0x1f
001A1403: jbe       0x1801a141b
001A1405: mov       QWORD PTR [rsp+0x20],rsi
001A140A: xor       r9d,r9d
001A140D: xor       r8d,r8d
001A1410: xor       edx,edx
001A1412: xor       ecx,ecx
001A1414: call      QWORD PTR [rip+0x12296e]        # 0x1802c3d88
001A141A: int3      
001A141B: call      0x18023927c
001A1420: vmovdqu   XMMWORD PTR [rbp-0x50],xmm6
001A1425: mov       BYTE PTR [rbp-0x60],0x0
001A1429: mov       rdx,QWORD PTR [rbp-0x8]
001A142D: cmp       rdx,0xf
001A1431: jbe       0x1801a1134
001A1437: inc       rdx
001A143A: mov       rcx,QWORD PTR [rbp-0x20]
001A143E: mov       rax,rcx
001A1441: cmp       rdx,0x1000
001A1448: jb        0x1801a112f
001A144E: add       rdx,0x27
001A1452: mov       rcx,QWORD PTR [rcx-0x8]
001A1456: sub       rax,rcx
001A1459: sub       rax,0x8
001A145D: cmp       rax,0x1f
001A1461: jbe       0x1801a112f
001A1467: mov       QWORD PTR [rsp+0x20],rsi
001A146C: xor       r9d,r9d
001A146F: xor       r8d,r8d
001A1472: xor       edx,edx
001A1474: xor       ecx,ecx
001A1476: call      QWORD PTR [rip+0x12290c]        # 0x1802c3d88
001A147C: nop       
001A147D: xor       ecx,ecx
001A147F: call      0x1801b2340
001A1484: test      eax,eax
001A1486: jne       0x1801a24ae
001A148C: call      0x180135920
001A1491: mov       rcx,QWORD PTR [rax+0x108]
001A1498: mov       QWORD PTR [rbp+0x1b0],rcx
001A149F: call      0x180135920
001A14A4: movzx     ebx,BYTE PTR [rax+0x118]
001A14AB: mov       QWORD PTR [rsp+0x40],0x12745
001A14B4: mov       QWORD PTR [rsp+0x48],0x12e54
001A14BD: lea       rcx,[rsp+0x40]
001A14C2: call      0x1801365e0
001A14C7: mov       esi,0x2c0
001A14CC: mov       edi,esi
001A14CE: mov       r14d,0x2e3
001A14D4: cmp       bl,0x1
001A14D7: cmovne    edi,r14d
001A14DB: add       rdi,rax
001A14DE: mov       dl,0x1
001A14E0: mov       ecx,0xe
001A14E5: call      0x1801af520
001A14EA: call      0x1801aacc0
001A14EF: movsxd    rbx,DWORD PTR [rdi+0x1]
001A14F3: add       rbx,0x5
001A14F7: add       rbx,rdi
001A14FA: mov       r9b,0xe8
001A14FD: lea       r8,[rip+0xffffffffffffd0dc]        # 0x18019e5e0
001A1504: mov       rdx,rdi
001A1507: mov       rcx,rax
001A150A: call      0x1801ab720
001A150F: mov       QWORD PTR [rip+0x2dbec2],rbx        # 0x18047d3d8
001A1516: call      0x180135920
001A151B: movzx     ebx,BYTE PTR [rax+0x118]
001A1522: mov       QWORD PTR [rsp+0x40],0x12745
001A152B: mov       QWORD PTR [rsp+0x48],0x12e54
001A1534: lea       rcx,[rsp+0x40]
001A1539: call      0x1801365e0
001A153E: cmp       bl,0x1
001A1541: cmovne    esi,r14d
001A1545: add       rax,rsi
001A1548: mov       QWORD PTR [rbp+0x1a0],rax
001A154F: call      0x180222050
001A1554: mov       QWORD PTR [rsp+0x50],r15
001A1559: mov       DWORD PTR [rsp+0x58],0x9e8
001A1561: mov       ecx,DWORD PTR [rsp+0x7c]
001A1565: mov       DWORD PTR [rsp+0x5c],ecx
001A1569: lea       r12,[rip+0x2699d8]        # 0x18040af48
001A1570: mov       QWORD PTR [rsp+0x60],r12
001A1575: lea       rcx,[rip+0x269dc4]        # 0x18040b340
001A157C: mov       QWORD PTR [rsp+0x40],rcx
001A1581: mov       QWORD PTR [rsp+0x48],0x25
001A158A: vmovups   xmm0,XMMWORD PTR [rsp+0x50]
001A1590: vmovups   XMMWORD PTR [rsp+0x70],xmm0
001A1596: vmovsd    xmm1,QWORD PTR [rsp+0x60]
001A159C: vmovsd    QWORD PTR [rbp-0x80],xmm1
001A15A1: lea       rcx,[rbp+0x1a0]
001A15A8: mov       QWORD PTR [rsp+0x20],rcx
001A15AD: lea       r9,[rsp+0x40]
001A15B2: mov       r8d,0x1
001A15B8: lea       rdx,[rsp+0x70]
001A15BD: mov       rcx,rax
001A15C0: call      0x18017d110
001A15C5: nop       
001A15C6: call      0x180135920
001A15CB: movzx     ebx,BYTE PTR [rax+0x118]
001A15D2: mov       QWORD PTR [rsp+0x40],0x1274b
001A15DB: mov       QWORD PTR [rsp+0x48],0x12daa
001A15E4: lea       rcx,[rsp+0x40]
001A15E9: call      0x1801365e0
001A15EE: mov       esi,0x2bc
001A15F3: mov       edi,esi
001A15F5: mov       r14d,0x50
001A15FB: cmp       bl,0x1
001A15FE: cmovne    edi,r14d
001A1602: add       rdi,rax
001A1605: mov       dl,0x1
001A1607: mov       ecx,0xe
001A160C: call      0x1801af520
001A1611: call      0x1801aacc0
001A1616: movsxd    rbx,DWORD PTR [rdi+0x1]
001A161A: add       rbx,0x5
001A161E: add       rbx,rdi
001A1621: mov       r9b,0xe8
001A1624: lea       r8,[rip+0xffffffffffffd8f5]        # 0x18019ef20
001A162B: mov       rdx,rdi
001A162E: mov       rcx,rax
001A1631: call      0x1801ab720
001A1636: mov       QWORD PTR [rip+0x2dbd93],rbx        # 0x18047d3d0
001A163D: call      0x180135920
001A1642: movzx     ebx,BYTE PTR [rax+0x118]
001A1649: mov       QWORD PTR [rsp+0x40],0x1274b
001A1652: mov       QWORD PTR [rsp+0x48],0x12daa
001A165B: lea       rcx,[rsp+0x40]
001A1660: call      0x1801365e0
001A1665: cmp       bl,0x1
001A1668: cmovne    esi,r14d
001A166C: add       rax,rsi
001A166F: mov       QWORD PTR [rbp+0x1a0],rax
001A1676: call      0x180222050
001A167B: mov       QWORD PTR [rsp+0x50],r15
001A1680: mov       DWORD PTR [rsp+0x58],0x9eb
001A1688: mov       ecx,DWORD PTR [rsp+0x7c]
001A168C: mov       DWORD PTR [rsp+0x5c],ecx
001A1690: mov       QWORD PTR [rsp+0x60],r12
001A1695: lea       rcx,[rip+0x269c7c]        # 0x18040b318
001A169C: mov       QWORD PTR [rsp+0x40],rcx
001A16A1: mov       QWORD PTR [rsp+0x48],0x23
001A16AA: vmovups   xmm0,XMMWORD PTR [rsp+0x50]
001A16B0: vmovups   XMMWORD PTR [rsp+0x70],xmm0
001A16B6: vmovsd    xmm1,QWORD PTR [rsp+0x60]
001A16BC: vmovsd    QWORD PTR [rbp-0x80],xmm1
001A16C1: lea       rcx,[rbp+0x1a0]
001A16C8: mov       QWORD PTR [rsp+0x20],rcx
001A16CD: lea       r9,[rsp+0x40]
001A16D2: mov       r8d,0x1
001A16D8: lea       rdx,[rsp+0x70]
001A16DD: mov       rcx,rax
001A16E0: call      0x18017d110
001A16E5: nop       
001A16E6: call      0x180135920
001A16EB: movzx     ebx,BYTE PTR [rax+0x118]
001A16F2: mov       QWORD PTR [rsp+0x40],0x1384b
001A16FB: mov       QWORD PTR [rsp+0x48],0x140a4
001A1704: lea       rcx,[rsp+0x40]
001A1709: call      0x1801365e0
001A170E: mov       esi,0x17a
001A1713: mov       edi,esi
001A1715: mov       r14d,0x16f
001A171B: cmp       bl,0x1
001A171E: cmovne    edi,r14d
001A1722: add       rdi,rax
001A1725: mov       dl,0x1
001A1727: mov       ecx,0xe
001A172C: call      0x1801af520
001A1731: call      0x1801aacc0
001A1736: movsxd    rbx,DWORD PTR [rdi+0x1]
001A173A: add       rbx,0x5
001A173E: add       rbx,rdi
001A1741: mov       r9b,0xe8
001A1744: lea       r8,[rip+0xffffffffffffdcd5]        # 0x18019f420
001A174B: mov       rdx,rdi
001A174E: mov       rcx,rax
001A1751: call      0x1801ab720
001A1756: mov       QWORD PTR [rip+0x2dbc53],rbx        # 0x18047d3b0
001A175D: call      0x180135920
001A1762: movzx     ebx,BYTE PTR [rax+0x118]
001A1769: mov       QWORD PTR [rsp+0x40],0x1384b
001A1772: mov       QWORD PTR [rsp+0x48],0x140a4
001A177B: lea       rcx,[rsp+0x40]
001A1780: call      0x1801365e0
001A1785: cmp       bl,0x1
001A1788: cmovne    esi,r14d
001A178C: add       rax,rsi
001A178F: mov       QWORD PTR [rbp+0x1a0],rax
001A1796: call      0x180222050
001A179B: mov       QWORD PTR [rsp+0x50],r15
001A17A0: mov       DWORD PTR [rsp+0x58],0x9ef
001A17A8: mov       ecx,DWORD PTR [rsp+0x7c]
001A17AC: mov       DWORD PTR [rsp+0x5c],ecx
001A17B0: mov       QWORD PTR [rsp+0x60],r12
001A17B5: lea       rcx,[rip+0x269bdc]        # 0x18040b398
001A17BC: mov       QWORD PTR [rsp+0x40],rcx
001A17C1: mov       QWORD PTR [rsp+0x48],0x17
001A17CA: vmovups   xmm0,XMMWORD PTR [rsp+0x50]
001A17D0: vmovups   XMMWORD PTR [rsp+0x70],xmm0
001A17D6: vmovsd    xmm1,QWORD PTR [rsp+0x60]
001A17DC: vmovsd    QWORD PTR [rbp-0x80],xmm1
001A17E1: lea       rcx,[rbp+0x1a0]
001A17E8: mov       QWORD PTR [rsp+0x20],rcx
001A17ED: lea       r9,[rsp+0x40]
001A17F2: mov       r8d,0x1
001A17F8: lea       rdx,[rsp+0x70]
001A17FD: mov       rcx,rax
001A1800: call      0x18017d110
001A1805: call      0x180135920
001A180A: mov       rcx,QWORD PTR [rax+0x108]
001A1811: mov       QWORD PTR [rbp+0x1a0],rcx
001A1818: lea       rcx,[rbp+0x1a0]
001A181F: call      0x1801a72c0
001A1824: test      al,al
001A1826: jg        0x1801a18d1
001A182C: call      0x180135920
001A1831: movzx     ebx,BYTE PTR [rax+0x118]
001A1838: mov       QWORD PTR [rsp+0x40],0x126c4
001A1841: mov       QWORD PTR [rsp+0x48],0x12dbd
001A184A: lea       rcx,[rsp+0x40]
001A184F: call      0x1801365e0
001A1854: mov       edi,0xe2
001A1859: mov       ecx,edi
001A185B: mov       esi,0xe5
001A1860: cmp       bl,0x1
001A1863: cmovne    ecx,esi
001A1866: add       rcx,rax
001A1869: call      0x1801a4680
001A186E: nop       
001A186F: call      0x180135920
001A1874: movzx     ebx,BYTE PTR [rax+0x118]
001A187B: mov       QWORD PTR [rsp+0x40],0x126c4
001A1884: mov       QWORD PTR [rsp+0x48],0x12dbd
001A188D: lea       rcx,[rsp+0x40]
001A1892: call      0x1801365e0
001A1897: cmp       bl,0x1
001A189A: cmovne    edi,esi
001A189D: add       rax,rdi
001A18A0: mov       QWORD PTR [rbp+0x1a0],rax
001A18A7: call      0x180222050
001A18AC: mov       DWORD PTR [rsp+0x58],0x9f4
001A18B4: mov       ecx,DWORD PTR [rsp+0x7c]
001A18B8: mov       DWORD PTR [rsp+0x5c],ecx
001A18BC: lea       rcx,[rip+0x269aa5]        # 0x18040b368
001A18C3: mov       QWORD PTR [rsp+0x48],0x29
001A18CC: jmp       0x1801a1971
001A18D1: call      0x180135920
001A18D6: movzx     ebx,BYTE PTR [rax+0x118]
001A18DD: mov       QWORD PTR [rsp+0x40],0x126c4
001A18E6: mov       QWORD PTR [rsp+0x48],0x12dbd
001A18EF: lea       rcx,[rsp+0x40]
001A18F4: call      0x1801365e0
001A18F9: mov       edi,0x133
001A18FE: mov       ecx,edi
001A1900: mov       esi,0xe5
001A1905: cmp       bl,0x1
001A1908: cmovne    ecx,esi
001A190B: add       rcx,rax
001A190E: call      0x1801a4680
001A1913: nop       
001A1914: call      0x180135920
001A1919: movzx     ebx,BYTE PTR [rax+0x118]
001A1920: mov       QWORD PTR [rsp+0x40],0x126c4
001A1929: mov       QWORD PTR [rsp+0x48],0x12dbd
001A1932: lea       rcx,[rsp+0x40]
001A1937: call      0x1801365e0
001A193C: cmp       bl,0x1
001A193F: cmovne    edi,esi
001A1942: add       rax,rdi
001A1945: mov       QWORD PTR [rbp+0x1a0],rax
001A194C: call      0x180222050
001A1951: mov       DWORD PTR [rsp+0x58],0x9f7
001A1959: mov       ecx,DWORD PTR [rsp+0x7c]
001A195D: mov       DWORD PTR [rsp+0x5c],ecx
001A1961: lea       rcx,[rip+0x269a60]        # 0x18040b3c8
001A1968: mov       QWORD PTR [rsp+0x48],0x31
001A1971: mov       QWORD PTR [rsp+0x40],rcx
001A1976: mov       QWORD PTR [rsp+0x50],r15
001A197B: mov       QWORD PTR [rsp+0x60],r12
001A1980: lea       rcx,[rbp+0x1a0]
001A1987: vmovups   xmm0,XMMWORD PTR [rsp+0x50]
001A198D: vmovsd    xmm1,QWORD PTR [rsp+0x60]
001A1993: mov       QWORD PTR [rsp+0x20],rcx
001A1998: vmovups   XMMWORD PTR [rsp+0x70],xmm0
001A199E: vmovsd    QWORD PTR [rbp-0x80],xmm1
001A19A3: lea       r9,[rsp+0x40]
001A19A8: mov       r8d,0x1
001A19AE: lea       rdx,[rsp+0x70]
001A19B3: mov       rcx,rax
001A19B6: call      0x18017d110
001A19BB: call      0x180135920
001A19C0: mov       rcx,QWORD PTR [rax+0x108]
001A19C7: mov       QWORD PTR [rbp+0x1a0],rcx
001A19CE: lea       rcx,[rbp+0x1a0]
001A19D5: call      0x1801a72c0
001A19DA: test      al,al
001A19DC: jg        0x1801a1a87
001A19E2: call      0x180135920
001A19E7: movzx     ebx,BYTE PTR [rax+0x118]
001A19EE: mov       QWORD PTR [rsp+0x40],0x126c4
001A19F7: mov       QWORD PTR [rsp+0x48],0x12dbd
001A1A00: lea       rcx,[rsp+0x40]
001A1A05: call      0x1801365e0
001A1A0A: mov       edi,0x18b
001A1A0F: mov       ecx,edi
001A1A11: mov       esi,0x192
001A1A16: cmp       bl,0x1
001A1A19: cmovne    ecx,esi
001A1A1C: add       rcx,rax
001A1A1F: call      0x1801a46d0
001A1A24: nop       
001A1A25: call      0x180135920
001A1A2A: movzx     ebx,BYTE PTR [rax+0x118]
001A1A31: mov       QWORD PTR [rsp+0x40],0x126c4
001A1A3A: mov       QWORD PTR [rsp+0x48],0x12dbd
001A1A43: lea       rcx,[rsp+0x40]
001A1A48: call      0x1801365e0
001A1A4D: cmp       bl,0x1
001A1A50: cmovne    edi,esi
001A1A53: add       rax,rdi
001A1A56: mov       QWORD PTR [rbp+0x1a0],rax
001A1A5D: call      0x180222050
001A1A62: mov       DWORD PTR [rsp+0x58],0x9fd
001A1A6A: mov       ecx,DWORD PTR [rsp+0x7c]
001A1A6E: mov       DWORD PTR [rsp+0x5c],ecx
001A1A72: lea       rcx,[rip+0x269937]        # 0x18040b3b0
001A1A79: mov       QWORD PTR [rsp+0x48],0x14
001A1A82: jmp       0x1801a1b27
001A1A87: mov       QWORD PTR [rsp+0x40],0x126c4
001A1A90: mov       QWORD PTR [rsp+0x48],0x12dbd
001A1A99: call      0x180135920
001A1A9E: movzx     ebx,BYTE PTR [rax+0x118]
001A1AA5: lea       rcx,[rsp+0x40]
001A1AAA: call      0x1801365e0
001A1AAF: mov       edi,0x1dc
001A1AB4: mov       ecx,edi
001A1AB6: mov       esi,0x192
001A1ABB: cmp       bl,0x1
001A1ABE: cmovne    ecx,esi
001A1AC1: add       rcx,rax
001A1AC4: call      0x1801a46d0
001A1AC9: nop       
001A1ACA: call      0x180135920
001A1ACF: movzx     ebx,BYTE PTR [rax+0x118]
001A1AD6: mov       QWORD PTR [rsp+0x40],0x126c4
001A1ADF: mov       QWORD PTR [rsp+0x48],0x12dbd
001A1AE8: lea       rcx,[rsp+0x40]
001A1AED: call      0x1801365e0
001A1AF2: cmp       bl,0x1
001A1AF5: cmovne    edi,esi
001A1AF8: add       rax,rdi
001A1AFB: mov       QWORD PTR [rbp+0x1a0],rax
001A1B02: call      0x180222050
001A1B07: mov       DWORD PTR [rsp+0x58],0xa00
001A1B0F: mov       ecx,DWORD PTR [rsp+0x7c]
001A1B13: mov       DWORD PTR [rsp+0x5c],ecx
001A1B17: lea       rcx,[rip+0x2698fa]        # 0x18040b418
001A1B1E: mov       QWORD PTR [rsp+0x48],0x1c
001A1B27: mov       QWORD PTR [rsp+0x40],rcx
001A1B2C: mov       QWORD PTR [rsp+0x50],r15
001A1B31: mov       QWORD PTR [rsp+0x60],r12
001A1B36: lea       rcx,[rbp+0x1a0]
001A1B3D: vmovups   xmm0,XMMWORD PTR [rsp+0x50]
001A1B43: vmovsd    xmm1,QWORD PTR [rsp+0x60]
001A1B49: mov       QWORD PTR [rsp+0x20],rcx
001A1B4E: vmovups   XMMWORD PTR [rsp+0x70],xmm0
001A1B54: vmovsd    QWORD PTR [rbp-0x80],xmm1
001A1B59: lea       r9,[rsp+0x40]
001A1B5E: mov       r8d,0x1
001A1B64: lea       rdx,[rsp+0x70]
001A1B69: mov       rcx,rax
001A1B6C: call      0x18017d110
001A1B71: mov       QWORD PTR [rsp+0x40],0xc9b7
001A1B7A: mov       QWORD PTR [rsp+0x48],0xcd1f
001A1B83: lea       rcx,[rsp+0x40]
001A1B88: call      0x1801365e0
001A1B8D: lea       rdi,[rax+0x1b]
001A1B91: movsxd    rbx,DWORD PTR [rdi+0x1]
001A1B95: cmp       BYTE PTR [rdi],0xe8
001A1B98: jne       0x1801a1c03
001A1B9A: mov       QWORD PTR [rsp+0x40],0xca8f
001A1BA3: mov       QWORD PTR [rsp+0x48],0xcdf7
001A1BAC: lea       rcx,[rsp+0x40]
001A1BB1: call      0x1801365e0
001A1BB6: lea       r8,[rbx+0x5]
001A1BBA: add       r8,rdi
001A1BBD: cmp       r8,rax
001A1BC0: jne       0x1801a1c03
001A1BC2: mov       dl,0x1
001A1BC4: mov       ecx,0xe
001A1BC9: call      0x1801af520
001A1BCE: call      0x1801aacc0
001A1BD3: movsxd    rbx,DWORD PTR [rdi+0x1]
001A1BD7: add       rbx,0x5
001A1BDB: add       rbx,rdi
001A1BDE: mov       r9b,0xe8
001A1BE1: lea       r8,[rip+0xffffffffffffec08]        # 0x1801a07f0
001A1BE8: mov       rdx,rdi
001A1BEB: mov       rcx,rax
001A1BEE: call      0x1801ab720
001A1BF3: mov       QWORD PTR [rip+0x2db7a6],rbx        # 0x18047d3a0
001A1BFA: mov       BYTE PTR [rip+0xcdf421],0x1        # 0x180e81022
001A1C01: jmp       0x1801a1c6e
001A1C03: call      0x180222050
001A1C08: mov       QWORD PTR [rsp+0x50],r15
001A1C0D: mov       DWORD PTR [rsp+0x58],0x91f
001A1C15: mov       ecx,DWORD PTR [rsp+0x7c]
001A1C19: mov       DWORD PTR [rsp+0x5c],ecx
001A1C1D: lea       rcx,[rip+0x26926c]        # 0x18040ae90
001A1C24: mov       QWORD PTR [rsp+0x60],rcx
001A1C29: lea       rcx,[rip+0x269220]        # 0x18040ae50
001A1C30: mov       QWORD PTR [rsp+0x40],rcx
001A1C35: mov       QWORD PTR [rsp+0x48],0x3e
001A1C3E: vmovups   xmm0,XMMWORD PTR [rsp+0x50]
001A1C44: vmovups   XMMWORD PTR [rsp+0x70],xmm0
001A1C4A: vmovsd    xmm1,QWORD PTR [rsp+0x60]
001A1C50: vmovsd    QWORD PTR [rbp-0x80],xmm1
001A1C55: lea       r9,[rsp+0x40]
001A1C5A: mov       r8d,0x3
001A1C60: lea       rdx,[rsp+0x70]
001A1C65: mov       rcx,rax
001A1C68: call      0x180151990
001A1C6D: nop       
001A1C6E: call      0x180135920
001A1C73: movzx     ebx,BYTE PTR [rax+0x118]
001A1C7A: mov       QWORD PTR [rsp+0x40],0xca8f
001A1C83: mov       QWORD PTR [rsp+0x48],0xcdf7
001A1C8C: lea       rcx,[rsp+0x40]
001A1C91: call      0x1801365e0
001A1C96: mov       esi,0x7a4
001A1C9B: mov       edi,esi
001A1C9D: mov       r14d,0x7a1
001A1CA3: cmp       bl,0x1
001A1CA6: cmovne    edi,r14d
001A1CAA: add       rdi,rax
001A1CAD: mov       dl,0x1
001A1CAF: mov       ecx,0xe
001A1CB4: call      0x1801af520
001A1CB9: call      0x1801aacc0
001A1CBE: movsxd    rbx,DWORD PTR [rdi+0x1]
001A1CC2: add       rbx,0x5
001A1CC6: add       rbx,rdi
001A1CC9: mov       r9b,0xe8
001A1CCC: lea       r8,[rip+0xffffffffffffe2ed]        # 0x18019ffc0
001A1CD3: mov       rdx,rdi
001A1CD6: mov       rcx,rax
001A1CD9: call      0x1801ab720
001A1CDE: mov       QWORD PTR [rip+0x2db6c3],rbx        # 0x18047d3a8
001A1CE5: call      0x180135920
001A1CEA: movzx     ebx,BYTE PTR [rax+0x118]
001A1CF1: mov       QWORD PTR [rsp+0x40],0xca8f
001A1CFA: mov       QWORD PTR [rsp+0x48],0xcdf7
001A1D03: lea       rcx,[rsp+0x40]
001A1D08: call      0x1801365e0
001A1D0D: cmp       bl,0x1
001A1D10: cmovne    esi,r14d
001A1D14: add       rax,rsi
001A1D17: mov       QWORD PTR [rbp+0x1a0],rax
001A1D1E: call      0x180222050
001A1D23: mov       QWORD PTR [rsp+0x50],r15
001A1D28: mov       DWORD PTR [rsp+0x58],0xa06
001A1D30: mov       ecx,DWORD PTR [rsp+0x7c]
001A1D34: mov       DWORD PTR [rsp+0x5c],ecx
001A1D38: mov       QWORD PTR [rsp+0x60],r12
001A1D3D: lea       rcx,[rip+0x2696bc]        # 0x18040b400
001A1D44: mov       QWORD PTR [rsp+0x40],rcx
001A1D49: mov       QWORD PTR [rsp+0x48],0x17
001A1D52: vmovups   xmm0,XMMWORD PTR [rsp+0x50]
001A1D58: vmovups   XMMWORD PTR [rsp+0x70],xmm0
001A1D5E: vmovsd    xmm1,QWORD PTR [rsp+0x60]
001A1D64: vmovsd    QWORD PTR [rbp-0x80],xmm1
001A1D69: lea       rcx,[rbp+0x1a0]
001A1D70: mov       QWORD PTR [rsp+0x20],rcx
001A1D75: lea       r9,[rsp+0x40]
001A1D7A: mov       r8d,0x1
001A1D80: lea       rdx,[rsp+0x70]
001A1D85: mov       rcx,rax
001A1D88: call      0x18017d110
001A1D8D: nop       
001A1D8E: call      0x180135920
001A1D93: nop       
001A1D94: mov       QWORD PTR [rsp+0x40],0xc5ac
001A1D9D: mov       QWORD PTR [rsp+0x48],0xc92a
001A1DA6: lea       rcx,[rsp+0x40]
001A1DAB: call      0x1801365e0
001A1DB0: lea       rdi,[rax+0xb]
001A1DB4: mov       dl,0x1
001A1DB6: mov       esi,0xe
001A1DBB: mov       ecx,esi
001A1DBD: call      0x1801af520
001A1DC2: call      0x1801aacc0
001A1DC7: movsxd    rbx,DWORD PTR [rdi+0x1]
001A1DCB: add       rbx,0x5
001A1DCF: add       rbx,rdi
001A1DD2: mov       r9b,0xe8
001A1DD5: lea       r8,[rip+0xffffffffffffeac4]        # 0x1801a08a0
001A1DDC: mov       rdx,rdi
001A1DDF: mov       rcx,rax
001A1DE2: call      0x1801ab720
001A1DE7: mov       QWORD PTR [rip+0x2db5a2],rbx        # 0x18047d390
001A1DEE: call      0x180135920
001A1DF3: nop       
001A1DF4: mov       QWORD PTR [rsp+0x40],0xc5ac
001A1DFD: mov       QWORD PTR [rsp+0x48],0xc92a
001A1E06: lea       rcx,[rsp+0x40]
001A1E0B: call      0x1801365e0
001A1E10: add       rax,0xb
001A1E14: mov       QWORD PTR [rbp+0x1a0],rax
001A1E1B: call      0x180222050
001A1E20: mov       QWORD PTR [rsp+0x50],r15
001A1E25: mov       DWORD PTR [rsp+0x58],0xa0a
001A1E2D: mov       ecx,DWORD PTR [rsp+0x7c]
001A1E31: mov       DWORD PTR [rsp+0x5c],ecx
001A1E35: mov       QWORD PTR [rsp+0x60],r12
001A1E3A: lea       rcx,[rip+0x26960f]        # 0x18040b450
001A1E41: mov       QWORD PTR [rsp+0x40],rcx
001A1E46: mov       QWORD PTR [rsp+0x48],0x24
001A1E4F: vmovups   xmm0,XMMWORD PTR [rsp+0x50]
001A1E55: vmovups   XMMWORD PTR [rsp+0x70],xmm0
001A1E5B: vmovsd    xmm1,QWORD PTR [rsp+0x60]
001A1E61: vmovsd    QWORD PTR [rbp-0x80],xmm1
001A1E66: lea       rcx,[rbp+0x1a0]
001A1E6D: mov       QWORD PTR [rsp+0x20],rcx
001A1E72: lea       r9,[rsp+0x40]
001A1E77: mov       r8d,0x1
001A1E7D: lea       rdx,[rsp+0x70]
001A1E82: mov       rcx,rax
001A1E85: call      0x18017d110
001A1E8A: nop       
001A1E8B: call      0x180135920
001A1E90: nop       
001A1E91: mov       QWORD PTR [rsp+0x40],0x12746
001A1E9A: mov       QWORD PTR [rsp+0x48],0x12e55
001A1EA3: lea       rcx,[rsp+0x40]
001A1EA8: call      0x1801365e0
001A1EAD: lea       rdi,[rax+0x102]
001A1EB4: mov       dl,0x1
001A1EB6: mov       ecx,esi
001A1EB8: call      0x1801af520
001A1EBD: call      0x1801aacc0
001A1EC2: movsxd    rbx,DWORD PTR [rdi+0x1]
001A1EC6: add       rbx,0x5
001A1ECA: add       rbx,rdi
001A1ECD: mov       r9b,0xe8
001A1ED0: lea       r8,[rip+0xffffffffffffe9f9]        # 0x1801a08d0
001A1ED7: mov       rdx,rdi
001A1EDA: mov       rcx,rax
001A1EDD: call      0x1801ab720
001A1EE2: mov       QWORD PTR [rip+0x2db4af],rbx        # 0x18047d398
001A1EE9: call      0x180135920
001A1EEE: nop       
001A1EEF: mov       QWORD PTR [rsp+0x40],0x12746
001A1EF8: mov       QWORD PTR [rsp+0x48],0x12e55
001A1F01: lea       rcx,[rsp+0x40]
001A1F06: call      0x1801365e0
001A1F0B: add       rax,0x102
001A1F11: mov       QWORD PTR [rbp+0x1a0],rax
001A1F18: call      0x180222050
001A1F1D: mov       QWORD PTR [rsp+0x50],r15
001A1F22: mov       DWORD PTR [rsp+0x58],0xa0d
001A1F2A: mov       ecx,DWORD PTR [rsp+0x7c]
001A1F2E: mov       DWORD PTR [rsp+0x5c],ecx
001A1F32: mov       QWORD PTR [rsp+0x60],r12
001A1F37: lea       rcx,[rip+0x2694fa]        # 0x18040b438
001A1F3E: mov       QWORD PTR [rsp+0x40],rcx
001A1F43: mov       QWORD PTR [rsp+0x48],0x10
001A1F4C: vmovups   xmm0,XMMWORD PTR [rsp+0x50]
001A1F52: vmovups   XMMWORD PTR [rsp+0x70],xmm0
001A1F58: vmovsd    xmm1,QWORD PTR [rsp+0x60]
001A1F5E: vmovsd    QWORD PTR [rbp-0x80],xmm1
001A1F63: lea       rcx,[rbp+0x1a0]
001A1F6A: mov       QWORD PTR [rsp+0x20],rcx
001A1F6F: lea       r9,[rsp+0x40]
001A1F74: mov       r8d,0x1
001A1F7A: lea       rdx,[rsp+0x70]
001A1F7F: mov       rcx,rax
001A1F82: call      0x18017d110
001A1F87: mov       QWORD PTR [rsp+0x40],0x127bd
001A1F90: mov       QWORD PTR [rsp+0x48],0x12ece
001A1F99: lea       rcx,[rsp+0x40]
001A1F9E: call      0x1801365e0
001A1FA3: mov       rbx,rax
001A1FA6: mov       QWORD PTR [rsp+0x40],0x127bf
001A1FAF: mov       QWORD PTR [rsp+0x48],0x12ed0
001A1FB8: lea       rcx,[rsp+0x40]
001A1FBD: call      0x1801365e0
001A1FC2: mov       rdi,rax
001A1FC5: mov       DWORD PTR [rbp+0x1a8],0x90909090
001A1FCF: mov       WORD PTR [rbp+0x1ac],0x9090
001A1FD8: mov       DWORD PTR [rbp-0x70],0x90909090
001A1FDF: mov       DWORD PTR [rbp-0x6c],0x90909090
001A1FE6: mov       WORD PTR [rbp-0x68],0x9090
001A1FEC: call      0x180135920
001A1FF1: nop       
001A1FF2: mov       rcx,r13
001A1FF5: cmp       BYTE PTR [rax+0x118],0x1
001A1FFC: cmovne    rcx,rsi
001A2000: add       rcx,rbx
001A2003: mov       r8d,0x6
001A2009: lea       rdx,[rbp+0x1a8]
001A2010: call      0x1801ab9e0
001A2015: nop       
001A2016: call      0x180135920
001A201B: nop       
001A201C: lea       rcx,[rdi+0x1d5]
001A2023: mov       r8d,0xa
001A2029: lea       rdx,[rbp-0x70]
001A202D: call      0x1801ab9e0
001A2032: nop       
001A2033: call      0x180135920
001A2038: movzx     ebx,BYTE PTR [rax+0x118]
001A203F: mov       QWORD PTR [rsp+0x40],0x127bd
001A2048: mov       QWORD PTR [rsp+0x48],0x12ece
001A2051: lea       rcx,[rsp+0x40]
001A2056: call      0x1801365e0
001A205B: cmp       bl,0x1
001A205E: cmovne    r13,rsi
001A2062: add       rax,r13
001A2065: mov       QWORD PTR [rbp+0x1a0],rax
001A206C: call      0x180222050
001A2071: mov       QWORD PTR [rsp+0x50],r15
001A2076: mov       DWORD PTR [rsp+0x58],0xa18
001A207E: mov       ecx,DWORD PTR [rsp+0x7c]
001A2082: mov       DWORD PTR [rsp+0x5c],ecx
001A2086: mov       QWORD PTR [rsp+0x60],r12
001A208B: lea       rcx,[rip+0x269406]        # 0x18040b498
001A2092: mov       QWORD PTR [rsp+0x40],rcx
001A2097: mov       QWORD PTR [rsp+0x48],0x13
001A20A0: vmovups   xmm0,XMMWORD PTR [rsp+0x50]
001A20A6: vmovups   XMMWORD PTR [rsp+0x70],xmm0
001A20AC: vmovsd    xmm1,QWORD PTR [rsp+0x60]
001A20B2: vmovsd    QWORD PTR [rbp-0x80],xmm1
001A20B7: lea       rcx,[rbp+0x1a0]
001A20BE: mov       QWORD PTR [rsp+0x20],rcx
001A20C3: lea       r9,[rsp+0x40]
001A20C8: mov       r8d,0x1
001A20CE: lea       rdx,[rsp+0x70]
001A20D3: mov       rcx,rax
001A20D6: call      0x18017d110
001A20DB: mov       QWORD PTR [rsp+0x40],0x127bd
001A20E4: mov       QWORD PTR [rsp+0x48],0x12ece
001A20ED: call      0x180135920
001A20F2: nop       
001A20F3: lea       rcx,[rsp+0x40]
001A20F8: call      0x1801365e0
001A20FD: add       rax,0x1d5
001A2103: mov       QWORD PTR [rbp+0x1a0],rax
001A210A: call      0x180222050
001A210F: mov       QWORD PTR [rsp+0x50],r15
001A2114: mov       DWORD PTR [rsp+0x58],0xa19
001A211C: mov       ecx,DWORD PTR [rsp+0x7c]
001A2120: mov       DWORD PTR [rsp+0x5c],ecx
001A2124: mov       QWORD PTR [rsp+0x60],r12
001A2129: lea       rcx,[rip+0x269348]        # 0x18040b478
001A2130: mov       QWORD PTR [rsp+0x40],rcx
001A2135: mov       QWORD PTR [rsp+0x48],0x1b
001A213E: vmovups   xmm0,XMMWORD PTR [rsp+0x50]
001A2144: vmovups   XMMWORD PTR [rsp+0x70],xmm0
001A214A: vmovsd    xmm1,QWORD PTR [rsp+0x60]
001A2150: vmovsd    QWORD PTR [rbp-0x80],xmm1
001A2155: lea       rcx,[rbp+0x1a0]
001A215C: mov       QWORD PTR [rsp+0x20],rcx
001A2161: lea       r9,[rsp+0x40]
001A2166: mov       r8d,0x1
001A216C: lea       rdx,[rsp+0x70]
001A2171: mov       rcx,rax
001A2174: call      0x18017d110
001A2179: nop       
001A217A: call      0x180135920
001A217F: movzx     ebx,BYTE PTR [rax+0x118]
001A2186: mov       QWORD PTR [rsp+0x40],0x18662
001A218F: mov       QWORD PTR [rsp+0x48],0x1a057
001A2198: lea       rcx,[rsp+0x40]
001A219D: call      0x1801365e0
001A21A2: mov       edi,0x84
001A21A7: mov       r8d,0x8e
001A21AD: cmp       bl,0x1
001A21B0: cmovne    edi,r8d
001A21B4: add       rdi,rax
001A21B7: mov       dl,0x1
001A21B9: mov       ecx,esi
001A21BB: call      0x1801af520
001A21C0: call      0x1801aacc0
001A21C5: movsxd    rbx,DWORD PTR [rdi+0x1]
001A21C9: add       rbx,0x5
001A21CD: add       rbx,rdi
001A21D0: mov       r9b,0xe8
001A21D3: lea       r8,[rip+0xffffffffffffcf46]        # 0x18019f120
001A21DA: mov       rdx,rdi
001A21DD: mov       rcx,rax
001A21E0: call      0x1801ab720
001A21E5: mov       QWORD PTR [rip+0x2db1d4],rbx        # 0x18047d3c0
001A21EC: mov       QWORD PTR [rsp+0x40],0x801e0
001A21F5: mov       QWORD PTR [rsp+0x48],0x646f8
001A21FE: lea       rcx,[rsp+0x40]
001A2203: call      0x1801365e0
001A2208: mov       QWORD PTR [rip+0xcdee79],rax        # 0x180e81088
001A220F: mov       QWORD PTR [rsp+0x40],0x802ca
001A2218: mov       QWORD PTR [rsp+0x48],0x6475b
001A2221: lea       rcx,[rsp+0x40]
001A2226: call      0x1801365e0
001A222B: mov       rcx,QWORD PTR [rip+0xcdebae]        # 0x180e80de0
001A2232: mov       QWORD PTR [rcx+0x16a8],rax
001A2239: mov       QWORD PTR [rsp+0x40],0x802cb
001A2242: mov       QWORD PTR [rsp+0x48],0x6475c
001A224B: lea       rcx,[rsp+0x40]
001A2250: call      0x1801365e0
001A2255: mov       rcx,QWORD PTR [rip+0xcdeb84]        # 0x180e80de0
001A225C: mov       QWORD PTR [rcx+0x16b0],rax
001A2263: mov       QWORD PTR [rsp+0x40],0x802cc
001A226C: mov       QWORD PTR [rsp+0x48],0x6475d
001A2275: lea       rcx,[rsp+0x40]
001A227A: call      0x1801365e0
001A227F: mov       rcx,QWORD PTR [rip+0xcdeb5a]        # 0x180e80de0
001A2286: mov       QWORD PTR [rcx+0x16b8],rax
001A228D: mov       QWORD PTR [rsp+0x40],0x802cd
001A2296: mov       QWORD PTR [rsp+0x48],0x6475e
001A229F: lea       rcx,[rsp+0x40]
001A22A4: call      0x1801365e0
001A22A9: mov       rcx,QWORD PTR [rip+0xcdeb30]        # 0x180e80de0
001A22B0: mov       QWORD PTR [rcx+0x16c0],rax
001A22B7: call      0x180222050
001A22BC: mov       QWORD PTR [rsp+0x50],r15
001A22C1: mov       DWORD PTR [rsp+0x58],0xa2c
001A22C9: mov       ecx,DWORD PTR [rsp+0x7c]
001A22CD: mov       DWORD PTR [rsp+0x5c],ecx
001A22D1: mov       QWORD PTR [rsp+0x60],r12
001A22D6: lea       rcx,[rip+0x2691e3]        # 0x18040b4c0
001A22DD: mov       QWORD PTR [rsp+0x40],rcx
001A22E2: mov       QWORD PTR [rsp+0x48],0x18
001A22EB: vmovups   xmm0,XMMWORD PTR [rsp+0x50]
001A22F1: vmovups   XMMWORD PTR [rsp+0x70],xmm0
001A22F7: vmovsd    xmm1,QWORD PTR [rsp+0x60]
001A22FD: vmovsd    QWORD PTR [rbp-0x80],xmm1
001A2302: lea       r9,[rsp+0x40]
001A2307: mov       r8d,0x2
001A230D: lea       rdx,[rsp+0x70]
001A2312: mov       rcx,rax
001A2315: call      0x180151990
001A231A: lea       rax,[rip+0x23fc13]        # 0x1803e1f34
001A2321: mov       QWORD PTR [rsp+0x40],rax
001A2326: mov       QWORD PTR [rsp+0x48],0x1
001A232F: lea       r8,[rsp+0x40]
001A2334: lea       rdx,[rbp+0x0]
001A2338: lea       rcx,[rbp+0x1b0]
001A233F: call      0x1801355f0
001A2344: mov       rbx,rax
001A2347: call      0x180222050
001A234C: mov       QWORD PTR [rsp+0x50],r15
001A2351: mov       DWORD PTR [rsp+0x58],0xa2d
001A2359: mov       ecx,DWORD PTR [rsp+0x7c]
001A235D: mov       DWORD PTR [rsp+0x5c],ecx
001A2361: mov       QWORD PTR [rsp+0x60],r12
001A2366: lea       rcx,[rip+0x269143]        # 0x18040b4b0
001A236D: mov       QWORD PTR [rsp+0x40],rcx
001A2372: mov       QWORD PTR [rsp+0x48],0xf
001A237B: vmovups   xmm0,XMMWORD PTR [rsp+0x50]
001A2381: vmovups   XMMWORD PTR [rsp+0x70],xmm0
001A2387: vmovsd    xmm1,QWORD PTR [rsp+0x60]
001A238D: vmovsd    QWORD PTR [rbp-0x80],xmm1
001A2392: mov       QWORD PTR [rsp+0x20],rbx
001A2397: lea       r9,[rsp+0x40]
001A239C: lea       rdx,[rsp+0x70]
001A23A1: mov       rcx,rax
001A23A4: call      0x1801a54f0
001A23A9: nop       
001A23AA: mov       rdx,QWORD PTR [rbp+0x18]
001A23AE: cmp       rdx,0xf
001A23B2: jbe       0x1801a23fc
001A23B4: inc       rdx
001A23B7: mov       rcx,QWORD PTR [rbp+0x0]
001A23BB: mov       rax,rcx
001A23BE: cmp       rdx,0x1000
001A23C5: jb        0x1801a23f6
001A23C7: add       rdx,0x27
001A23CB: mov       rcx,QWORD PTR [rcx-0x8]
001A23CF: sub       rax,rcx
001A23D2: sub       rax,0x8
001A23D6: cmp       rax,0x1f
001A23DA: jbe       0x1801a23f6
001A23DC: mov       QWORD PTR [rsp+0x20],0x0
001A23E5: xor       r9d,r9d
001A23E8: xor       r8d,r8d
001A23EB: xor       edx,edx
001A23ED: xor       ecx,ecx
001A23EF: call      QWORD PTR [rip+0x121993]        # 0x1802c3d88
001A23F5: int3      
001A23F6: call      0x18023927c
001A23FB: nop       
001A23FC: mov       rdx,QWORD PTR [rbp-0x48]
001A2400: cmp       rdx,0xf
001A2404: jbe       0x1801a244d
001A2406: inc       rdx
001A2409: mov       rcx,QWORD PTR [rbp-0x60]
001A240D: mov       rax,rcx
001A2410: cmp       rdx,0x1000
001A2417: jb        0x1801a2448
001A2419: add       rdx,0x27
001A241D: mov       rcx,QWORD PTR [rcx-0x8]
001A2421: sub       rax,rcx
001A2424: sub       rax,0x8
001A2428: cmp       rax,0x1f
001A242C: jbe       0x1801a2448
001A242E: mov       QWORD PTR [rsp+0x20],0x0
001A2437: xor       r9d,r9d
001A243A: xor       r8d,r8d
001A243D: xor       edx,edx
001A243F: xor       ecx,ecx
001A2441: call      QWORD PTR [rip+0x121941]        # 0x1802c3d88
001A2447: int3      
001A2448: call      0x18023927c
001A244D: vmovdqu   XMMWORD PTR [rbp-0x50],xmm6
001A2452: mov       BYTE PTR [rbp-0x60],0x0
001A2456: mov       rdx,QWORD PTR [rbp-0x8]
001A245A: cmp       rdx,0xf
001A245E: jbe       0x1801a1134
001A2464: inc       rdx
001A2467: mov       rcx,QWORD PTR [rbp-0x20]
001A246B: mov       rax,rcx
001A246E: cmp       rdx,0x1000
001A2475: jb        0x1801a112f
001A247B: add       rdx,0x27
001A247F: mov       rcx,QWORD PTR [rcx-0x8]
001A2483: sub       rax,rcx
001A2486: sub       rax,0x8
001A248A: cmp       rax,0x1f
001A248E: jbe       0x1801a112f
001A2494: mov       QWORD PTR [rsp+0x20],0x0
001A249D: xor       r9d,r9d
001A24A0: xor       r8d,r8d
001A24A3: xor       edx,edx
001A24A5: xor       ecx,ecx
001A24A7: call      QWORD PTR [rip+0x1218db]        # 0x1802c3d88
001A24AD: nop       
001A24AE: lea       rdx,[rip+0x268af3]        # 0x18040afa8
001A24B5: lea       rcx,[rsp+0x70]
001A24BA: call      0x180131680
001A24BF: lea       rdx,[rip+0x2b50da]        # 0x1804575a0
001A24C6: lea       rcx,[rsp+0x70]
001A24CB: call      0x18023a820
001A24D0: nop       
001A24D1: lea       rdx,[rip+0x268a18]        # 0x18040aef0
001A24D8: lea       rcx,[rsp+0x70]
001A24DD: call      0x180131680
001A24E2: lea       rdx,[rip+0x2b50b7]        # 0x1804575a0
001A24E9: lea       rcx,[rsp+0x70]
001A24EE: call      0x18023a820
001A24F3: int3      
001A24F4: lea       rdx,[rip+0x268aad]        # 0x18040afa8
001A24FB: lea       rcx,[rsp+0x70]
001A2500: call      0x180131680
001A2505: lea       rdx,[rip+0x2b5094]        # 0x1804575a0
001A250C: lea       rcx,[rsp+0x70]
001A2511: call      0x18023a820
001A2516: int3      
001A2517: lea       rdx,[rip+0x268afa]        # 0x18040b018
001A251E: lea       rcx,[rsp+0x70]
001A2523: call      0x180131680
001A2528: lea       rdx,[rip+0x2b5071]        # 0x1804575a0
001A252F: lea       rcx,[rsp+0x70]
001A2534: call      0x18023a820
001A2539: int3      
001A253A: call      0x180131520
001A253F: nop       
001A2540: lea       rdx,[rip+0x268b21]        # 0x18040b068
001A2547: lea       rcx,[rsp+0x70]
001A254C: call      0x180131680
001A2551: lea       rdx,[rip+0x2b5048]        # 0x1804575a0
001A2558: lea       rcx,[rsp+0x70]
001A255D: call      0x18023a820
001A2562: int3      
001A2563: call      0x180131520
001A2568: nop       
001A2569: lea       rdx,[rip+0x268c20]        # 0x18040b190
001A2570: lea       rcx,[rsp+0x70]
001A2575: call      0x180131680
001A257A: lea       rdx,[rip+0x2b501f]        # 0x1804575a0
001A2581: lea       rcx,[rsp+0x70]
001A2586: call      0x18023a820
001A258B: int3      
001A258C: lea       rdx,[rip+0x268a15]        # 0x18040afa8
001A2593: lea       rcx,[rsp+0x70]
001A2598: call      0x180131680
001A259D: lea       rdx,[rip+0x2b4ffc]        # 0x1804575a0
001A25A4: lea       rcx,[rsp+0x70]
001A25A9: call      0x18023a820
001A25AE: int3      
