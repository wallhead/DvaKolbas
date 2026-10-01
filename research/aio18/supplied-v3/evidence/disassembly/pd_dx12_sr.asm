; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0x100B70..0x100EC3; unnamed
00100B70: mov       QWORD PTR [rsp+0x18],rbx
00100B75: push      rbp
00100B76: push      rsi
00100B77: push      rdi
00100B78: lea       rbp,[rsp-0x130]
00100B80: sub       rsp,0x230
00100B87: movaps    XMMWORD PTR [rsp+0x220],xmm6
00100B8F: mov       rax,QWORD PTR [rip+0x10c9e2a]        # 0x1811ca9c0
00100B96: xor       rax,rsp
00100B99: mov       QWORD PTR [rbp+0x110],rax
00100BA0: mov       rbx,rdx
00100BA3: mov       rdi,rcx
00100BA6: call      0x1800cc790
00100BAB: mov       r8d,DWORD PTR [rdi+0x8]
00100BAF: mov       rcx,rax
00100BB2: mov       edx,DWORD PTR [rdi+0x110]
00100BB8: call      0x1800ccc80
00100BBD: xor       edx,edx
00100BBF: lea       rcx,[rsp+0x78]
00100BC4: mov       r8d,0x150
00100BCA: call      0x18010d61a
00100BCF: mov       rsi,QWORD PTR [rbx+0x30]
00100BD3: xor       eax,eax
00100BD5: mov       WORD PTR [rbp+0xe9],ax
00100BDC: mov       BYTE PTR [rbp+0xeb],al
00100BE2: mov       WORD PTR [rbp+0xf9],ax
00100BE9: mov       BYTE PTR [rbp+0xfb],al
00100BEF: mov       QWORD PTR [rsp+0x68],rax
00100BF4: mov       rax,QWORD PTR [rbx+0x68]
00100BF8: mov       QWORD PTR [rsp+0x70],rax
00100BFD: mov       QWORD PTR [rsp+0x60],0x10001
00100C06: test      rsi,rsi
00100C09: jne       0x180100c0f
00100C0B: mov       rsi,QWORD PTR [rbx+0x28]
00100C0F: mov       r8,QWORD PTR [rbx+0x8]
00100C13: lea       rdx,[rsp+0x30]
00100C18: mov       rcx,rdi
00100C1B: call      0x180100a00
00100C20: mov       r8,QWORD PTR [rbx+0x18]
00100C24: lea       rdx,[rsp+0x30]
00100C29: mov       rcx,rdi
00100C2C: movups    xmm0,XMMWORD PTR [rax]
00100C2F: movups    XMMWORD PTR [rsp+0x78],xmm0
00100C34: movups    xmm1,XMMWORD PTR [rax+0x10]
00100C38: movups    XMMWORD PTR [rbp-0x78],xmm1
00100C3C: movups    xmm0,XMMWORD PTR [rax+0x20]
00100C40: movups    XMMWORD PTR [rbp-0x68],xmm0
00100C44: call      0x180100a00
00100C49: mov       r8,QWORD PTR [rbx+0x10]
00100C4D: lea       rdx,[rsp+0x30]
00100C52: mov       rcx,rdi
00100C55: movups    xmm0,XMMWORD PTR [rax]
00100C58: movups    XMMWORD PTR [rbp-0x58],xmm0
00100C5C: movups    xmm1,XMMWORD PTR [rax+0x10]
00100C60: movups    XMMWORD PTR [rbp-0x48],xmm1
00100C64: movups    xmm0,XMMWORD PTR [rax+0x20]
00100C68: movups    XMMWORD PTR [rbp-0x38],xmm0
00100C6C: call      0x180100a00
00100C71: mov       r8,rsi
00100C74: lea       rdx,[rsp+0x30]
00100C79: mov       rcx,rdi
00100C7C: movups    xmm0,XMMWORD PTR [rax]
00100C7F: movups    XMMWORD PTR [rbp-0x28],xmm0
00100C83: movups    xmm1,XMMWORD PTR [rax+0x10]
00100C87: movups    XMMWORD PTR [rbp-0x18],xmm1
00100C8B: movups    xmm0,XMMWORD PTR [rax+0x20]
00100C8F: movups    XMMWORD PTR [rbp-0x8],xmm0
00100C93: call      0x180100a00
00100C98: mov       r8,QWORD PTR [rbx+0x20]
00100C9C: lea       rdx,[rsp+0x30]
00100CA1: mov       rcx,rdi
00100CA4: movups    xmm0,XMMWORD PTR [rax]
00100CA7: movups    XMMWORD PTR [rbp+0x98],xmm0
00100CAE: movups    xmm1,XMMWORD PTR [rax+0x10]
00100CB2: movups    XMMWORD PTR [rbp+0xa8],xmm1
00100CB9: movups    xmm0,XMMWORD PTR [rax+0x20]
00100CBD: movups    XMMWORD PTR [rbp+0xb8],xmm0
00100CC4: call      0x180100a00
00100CC9: xor       r8d,r8d
00100CCC: lea       rdx,[rsp+0x30]
00100CD1: mov       rcx,rdi
00100CD4: movups    xmm0,XMMWORD PTR [rax]
00100CD7: movups    XMMWORD PTR [rbp+0x38],xmm0
00100CDB: movups    xmm1,XMMWORD PTR [rax+0x10]
00100CDF: movups    XMMWORD PTR [rbp+0x48],xmm1
00100CE3: movups    xmm0,XMMWORD PTR [rax+0x20]
00100CE7: movups    XMMWORD PTR [rbp+0x58],xmm0
00100CEB: call      0x180100a00
00100CF0: xor       r8d,r8d
00100CF3: lea       rdx,[rsp+0x30]
00100CF8: mov       rcx,rdi
00100CFB: movups    xmm0,XMMWORD PTR [rax]
00100CFE: movups    XMMWORD PTR [rbp+0x68],xmm0
00100D02: movups    xmm1,XMMWORD PTR [rax+0x10]
00100D06: movups    XMMWORD PTR [rbp+0x78],xmm1
00100D0A: movups    xmm0,XMMWORD PTR [rax+0x20]
00100D0E: movups    XMMWORD PTR [rbp+0x88],xmm0
00100D15: call      0x180100a00
00100D1A: cmp       BYTE PTR [rdi+0x2c],0x0
00100D1E: movups    xmm0,XMMWORD PTR [rax]
00100D21: movups    XMMWORD PTR [rbp+0x8],xmm0
00100D25: movups    xmm1,XMMWORD PTR [rax+0x10]
00100D29: movups    XMMWORD PTR [rbp+0x18],xmm1
00100D2D: movups    xmm0,XMMWORD PTR [rax+0x20]
00100D31: movzx     eax,BYTE PTR [rbx+0x54]
00100D35: movups    xmm1,XMMWORD PTR [rbx+0x44]
00100D39: mov       BYTE PTR [rbp+0xf8],al
00100D3F: movups    XMMWORD PTR [rbp+0x28],xmm0
00100D43: mov       DWORD PTR [rbp+0x108],0x3f800000
00100D4D: movups    XMMWORD PTR [rbp+0xc8],xmm1
00100D54: movss     xmm1,DWORD PTR [rbx+0x40]
00100D59: jne       0x180100d6c
00100D5B: xorps     xmm0,xmm0
00100D5E: ucomiss   xmm1,xmm0
00100D61: jp        0x180100d6c
00100D63: mov       BYTE PTR [rbp+0xe8],0x0
00100D6A: je        0x180100d73
00100D6C: mov       BYTE PTR [rbp+0xe8],0x1
00100D73: movss     DWORD PTR [rbp+0xec],xmm1
00100D7B: call      0x1800ffe70
00100D80: cmp       BYTE PTR [rdi+0x29],0x0
00100D84: movaps    xmm1,xmm0
00100D87: subsd     xmm1,QWORD PTR [rdi+0x140]
00100D8F: cvttss2si rax,DWORD PTR [rbx+0x38]
00100D95: mov       DWORD PTR [rbp+0xf4],0x3f800000
00100D9F: movss     xmm6,DWORD PTR [rbx+0x58]
00100DA4: movsd     QWORD PTR [rdi+0x140],xmm0
00100DAC: xorps     xmm0,xmm0
00100DAF: cvtsd2ss  xmm0,xmm1
00100DB3: mov       DWORD PTR [rbp+0xd8],eax
00100DB9: cvttss2si rax,DWORD PTR [rbx+0x3c]
00100DBF: movss     xmm1,DWORD PTR [rbx+0x5c]
00100DC4: mov       DWORD PTR [rbp+0xdc],eax
00100DCA: mov       eax,DWORD PTR [rdi+0x1c]
00100DCD: movss     DWORD PTR [rbp+0xf0],xmm0
00100DD5: movss     xmm0,DWORD PTR [rbx+0x60]
00100DDA: mov       DWORD PTR [rbp+0xe0],eax
00100DE0: mov       eax,DWORD PTR [rdi+0x20]
00100DE3: movss     DWORD PTR [rbp+0x104],xmm0
00100DEB: movaps    xmm0,xmm6
00100DEE: mov       DWORD PTR [rbp+0xe4],eax
00100DF4: je        0x180100e0d
00100DF6: movss     DWORD PTR [rbp+0x100],xmm6
00100DFE: call      0x1800ee440
00100E03: movss     DWORD PTR [rbp+0xfc],xmm0
00100E0B: jmp       0x180100e22
00100E0D: call      0x1800ee440
00100E12: movss     DWORD PTR [rbp+0x100],xmm0
00100E1A: movss     DWORD PTR [rbp+0xfc],xmm6
00100E22: cmp       BYTE PTR [rdi+0x48],0x0
00100E26: je        0x180100e40
00100E28: cmp       BYTE PTR [rdi+0x49],0x0
00100E2C: je        0x180100e34
00100E2E: cmp       DWORD PTR [rdi+0x4c],0x1
00100E32: je        0x180100e40
00100E34: mov       DWORD PTR [rbp+0x10c],0x1
00100E3E: jmp       0x180100e4a
00100E40: mov       DWORD PTR [rbp+0x10c],0x0
00100E4A: mov       rdx,rbx
00100E4D: call      0x180100fb0
00100E52: lea       rcx,[rsp+0x60]
00100E57: mov       rdi,rax
00100E5A: call      0x1800f0b60
00100E5F: mov       rbx,QWORD PTR [rip+0x11155fa]        # 0x181216460
00100E66: mov       rsi,rax
00100E69: cmp       BYTE PTR [rbx+0xca],0x0
00100E70: jne       0x180100e7a
00100E72: mov       rcx,rbx
00100E75: call      0x1800cc8c0
00100E7A: mov       r8,QWORD PTR [rbx+0x18]
00100E7E: mov       rdx,rsi
00100E81: mov       rcx,rdi
00100E84: call      r8
00100E87: test      eax,eax
00100E89: je        0x180100e99
00100E8B: mov       edx,eax
00100E8D: lea       rcx,[rip+0x10ab42c]        # 0x1811ac2c0 ; 'ffxFsr3ContextDispatchUpscale Failed! ErrorCode: %d'
00100E94: call      0x1800fbb40
00100E99: mov       rcx,QWORD PTR [rbp+0x110]
00100EA0: xor       rcx,rsp
00100EA3: call      0x18010c270
00100EA8: mov       rbx,QWORD PTR [rsp+0x260]
00100EB0: movaps    xmm6,XMMWORD PTR [rsp+0x220]
00100EB8: add       rsp,0x230
00100EBF: pop       rdi
00100EC0: pop       rsi
00100EC1: pop       rbp
00100EC2: ret       
