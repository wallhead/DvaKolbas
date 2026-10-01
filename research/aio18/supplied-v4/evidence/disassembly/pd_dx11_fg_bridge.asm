; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xEA650..0xEAD87; unnamed
000EA650: mov       QWORD PTR [rsp+0x18],rbx
000EA655: mov       QWORD PTR [rsp+0x20],rsi
000EA65A: push      rbp
000EA65B: push      rdi
000EA65C: push      r12
000EA65E: push      r14
000EA660: push      r15
000EA662: lea       rbp,[rsp-0x140]
000EA66A: sub       rsp,0x240
000EA671: mov       rax,QWORD PTR [rip+0x10e0348]        # 0x1811ca9c0
000EA678: xor       rax,rsp
000EA67B: mov       QWORD PTR [rbp+0x130],rax
000EA682: mov       rbx,rdx
000EA685: mov       rdi,rcx
000EA688: xor       ecx,ecx
000EA68A: call      0x1800d35a0
000EA68F: call      0x1800899c0
000EA694: mov       rdx,rax
000EA697: call      0x1800badc0
000EA69C: xor       r12d,r12d
000EA69F: test      al,al
000EA6A1: je        0x1800ea6c3
000EA6A3: cmp       DWORD PTR [rdx+0x1dcc],r12d
000EA6AA: je        0x1800ea6c3
000EA6AC: mov       ecx,DWORD PTR [rbx+0x98]
000EA6B2: cmp       DWORD PTR [rdx+0x1dc8],ecx
000EA6B8: jne       0x1800ea6c3
000EA6BA: mov       rsi,QWORD PTR [rdx+0x1db0]
000EA6C1: jmp       0x1800ea6c6
000EA6C3: mov       rsi,r12
000EA6C6: mov       QWORD PTR [rbx+0x20],rsi
000EA6CA: cmp       DWORD PTR [rdi+0x118],r12d
000EA6D1: jne       0x1800ea758
000EA6D7: call      0x1800badc0
000EA6DC: test      al,al
000EA6DE: je        0x1800ead5c
000EA6E4: cmp       QWORD PTR [rdi+0x198],r12
000EA6EB: jne       0x1800ea744
000EA6ED: lea       rcx,[rsp+0x30]
000EA6F2: call      0x1800ed710
000EA6F7: mov       rcx,QWORD PTR [rax]
000EA6FA: mov       QWORD PTR [rax],r12
000EA6FD: mov       rsi,QWORD PTR [rdi+0x198]
000EA704: mov       QWORD PTR [rdi+0x198],rcx
000EA70B: test      rsi,rsi
000EA70E: je        0x1800ea725
000EA710: mov       rcx,rsi
000EA713: call      0x1800ed980
000EA718: mov       edx,0x930
000EA71D: mov       rcx,rsi
000EA720: call      0x18010c39c
000EA725: mov       rsi,QWORD PTR [rsp+0x30]
000EA72A: test      rsi,rsi
000EA72D: je        0x1800ea744
000EA72F: mov       rcx,rsi
000EA732: call      0x1800ed980
000EA737: mov       edx,0x930
000EA73C: mov       rcx,rsi
000EA73F: call      0x18010c39c
000EA744: mov       rdx,rbx
000EA747: mov       rcx,QWORD PTR [rdi+0x198]
000EA74E: call      0x1800e9210
000EA753: jmp       0x1800ead5c
000EA758: mov       rcx,QWORD PTR [rip+0x112bc89]        # 0x1812163e8
000EA75F: call      0x18006fcc0
000EA764: test      al,al
000EA766: jne       0x1800ead5c
000EA76C: mov       rcx,QWORD PTR [rip+0x1135245]        # 0x18121f9b8
000EA773: test      rcx,rcx
000EA776: je        0x1800ea80a
000EA77C: cmp       BYTE PTR [rcx+0x428],r12b
000EA783: je        0x1800ea7ab
000EA785: mov       rax,QWORD PTR [rip+0x112bc5c]        # 0x1812163e8
000EA78C: cmp       BYTE PTR [rax+0x924],r12b
000EA793: jne       0x1800ea79f
000EA795: call      0x18009beb0
000EA79A: test      rax,rax
000EA79D: je        0x1800ea7ab
000EA79F: call      0x1800d3780
000EA7A4: mov       rcx,QWORD PTR [rip+0x113520d]        # 0x18121f9b8
000EA7AB: test      rcx,rcx
000EA7AE: je        0x1800ea80a
000EA7B0: lea       rdx,[rcx+0x4f0]
000EA7B7: cmp       QWORD PTR [rdx+0x38],r12
000EA7BB: je        0x1800ea80a
000EA7BD: mov       QWORD PTR [rbp+0xe8],r12
000EA7C4: lea       rcx,[rbp+0xb0]
000EA7CB: call      0x18000f630
000EA7D0: nop       
000EA7D1: mov       rcx,QWORD PTR [rbp+0xe8]
000EA7D8: test      rcx,rcx
000EA7DB: jne       0x1800ea7e4
000EA7DD: call      QWORD PTR [rip+0x28d6d]        # 0x180113550 ; MSVCP140.dll!?_Xbad_function_call@std@@YAXXZ
000EA7E3: int3      
000EA7E4: mov       rax,QWORD PTR [rcx]
000EA7E7: call      QWORD PTR [rax+0x10]
000EA7EA: nop       
000EA7EB: mov       rcx,QWORD PTR [rbp+0xe8]
000EA7F2: test      rcx,rcx
000EA7F5: je        0x1800ea80a
000EA7F7: lea       rax,[rbp+0xb0]
000EA7FE: cmp       rcx,rax
000EA801: setne     dl
000EA804: mov       rax,QWORD PTR [rcx]
000EA807: call      QWORD PTR [rax+0x20]
000EA80A: call      0x1800badc0
000EA80F: test      al,al
000EA811: jne       0x1800ea83b
000EA813: cmp       BYTE PTR [rdi+0x171],al
000EA819: jne       0x1800ea83b
000EA81B: mov       rcx,QWORD PTR [rdi+0x148]
000EA822: test      rcx,rcx
000EA825: je        0x1800ea83b
000EA827: mov       rax,QWORD PTR [rcx]
000EA82A: mov       edx,DWORD PTR [rbx+0x98]
000EA830: call      QWORD PTR [rax+0x40]
000EA833: test      al,al
000EA835: jne       0x1800ead5c
000EA83B: movss     xmm2,DWORD PTR [rbx+0x38]
000EA840: movd      xmm0,DWORD PTR [rdi+0x15c]
000EA848: cvtdq2ps  xmm0,xmm0
000EA84B: ucomiss   xmm2,xmm0
000EA84E: jp        0x1800ea869
000EA850: jne       0x1800ea869
000EA852: movd      xmm1,DWORD PTR [rdi+0x160]
000EA85A: cvtdq2ps  xmm1,xmm1
000EA85D: movss     xmm0,DWORD PTR [rbx+0x3c]
000EA862: ucomiss   xmm0,xmm1
000EA865: jp        0x1800ea869
000EA867: je        0x1800ea885
000EA869: mov       BYTE PTR [rdi+0x158],0x0
000EA870: cvttss2si eax,xmm2
000EA874: mov       DWORD PTR [rdi+0x15c],eax
000EA87A: cvttss2si eax,DWORD PTR [rbx+0x3c]
000EA87F: mov       DWORD PTR [rdi+0x160],eax
000EA885: cmp       BYTE PTR [rdi+0x158],0x0
000EA88C: jne       0x1800ea923
000EA892: cmp       QWORD PTR [rbx+0x18],0x0
000EA897: je        0x1800ea923
000EA89D: cmp       QWORD PTR [rbx+0x10],0x0
000EA8A2: je        0x1800ea923
000EA8A4: lea       rcx,[rbp+0x0]
000EA8A8: movups    xmm0,XMMWORD PTR [rbx]
000EA8AB: movups    XMMWORD PTR [rcx],xmm0
000EA8AE: movups    xmm1,XMMWORD PTR [rbx+0x10]
000EA8B2: movups    XMMWORD PTR [rcx+0x10],xmm1
000EA8B6: movups    xmm0,XMMWORD PTR [rbx+0x20]
000EA8BA: movups    XMMWORD PTR [rcx+0x20],xmm0
000EA8BE: movups    xmm1,XMMWORD PTR [rbx+0x30]
000EA8C2: movups    XMMWORD PTR [rcx+0x30],xmm1
000EA8C6: movups    xmm0,XMMWORD PTR [rbx+0x40]
000EA8CA: movups    XMMWORD PTR [rcx+0x40],xmm0
000EA8CE: movups    xmm1,XMMWORD PTR [rbx+0x50]
000EA8D2: movups    XMMWORD PTR [rcx+0x50],xmm1
000EA8D6: movups    xmm0,XMMWORD PTR [rbx+0x60]
000EA8DA: movups    XMMWORD PTR [rcx+0x60],xmm0
000EA8DE: movups    xmm1,XMMWORD PTR [rbx+0x70]
000EA8E2: movups    XMMWORD PTR [rcx+0x70],xmm1
000EA8E6: movups    xmm0,XMMWORD PTR [rbx+0x80]
000EA8ED: movups    XMMWORD PTR [rcx+0x80],xmm0
000EA8F4: movups    xmm1,XMMWORD PTR [rbx+0x90]
000EA8FB: movups    XMMWORD PTR [rcx+0x90],xmm1
000EA902: movups    xmm0,XMMWORD PTR [rbx+0xa0]
000EA909: movups    XMMWORD PTR [rcx+0xa0],xmm0
000EA910: lea       rdx,[rbp+0x0]
000EA914: mov       rcx,rdi
000EA917: call      0x1800e9f80
000EA91C: mov       BYTE PTR [rdi+0x158],0x1
000EA923: test      rsi,rsi
000EA926: je        0x1800ea9b5
000EA92C: mov       rax,QWORD PTR [rip+0x112bab5]        # 0x1812163e8
000EA933: cmp       QWORD PTR [rax+0x598],0x0
000EA93B: jne       0x1800ea9b5
000EA93D: lea       rcx,[rbp+0x0]
000EA941: movups    xmm0,XMMWORD PTR [rbx]
000EA944: movups    XMMWORD PTR [rcx],xmm0
000EA947: movups    xmm1,XMMWORD PTR [rbx+0x10]
000EA94B: movups    XMMWORD PTR [rcx+0x10],xmm1
000EA94F: movups    xmm0,XMMWORD PTR [rbx+0x20]
000EA953: movups    XMMWORD PTR [rcx+0x20],xmm0
000EA957: movups    xmm1,XMMWORD PTR [rbx+0x30]
000EA95B: movups    XMMWORD PTR [rcx+0x30],xmm1
000EA95F: movups    xmm0,XMMWORD PTR [rbx+0x40]
000EA963: movups    XMMWORD PTR [rcx+0x40],xmm0
000EA967: movups    xmm1,XMMWORD PTR [rbx+0x50]
000EA96B: movups    XMMWORD PTR [rcx+0x50],xmm1
000EA96F: movups    xmm0,XMMWORD PTR [rbx+0x60]
000EA973: movups    XMMWORD PTR [rcx+0x60],xmm0
000EA977: movups    xmm1,XMMWORD PTR [rbx+0x70]
000EA97B: movups    XMMWORD PTR [rcx+0x70],xmm1
000EA97F: movups    xmm0,XMMWORD PTR [rbx+0x80]
000EA986: movups    XMMWORD PTR [rcx+0x80],xmm0
000EA98D: movups    xmm1,XMMWORD PTR [rbx+0x90]
000EA994: movups    XMMWORD PTR [rcx+0x90],xmm1
000EA99B: movups    xmm0,XMMWORD PTR [rbx+0xa0]
000EA9A2: movups    XMMWORD PTR [rcx+0xa0],xmm0
000EA9A9: lea       rdx,[rbp+0x0]
000EA9AD: mov       rcx,rdi
000EA9B0: call      0x1800e9f80
000EA9B5: lea       rcx,[rbp+0x0]
000EA9B9: movups    xmm0,XMMWORD PTR [rbx]
000EA9BC: movups    XMMWORD PTR [rcx],xmm0
000EA9BF: movups    xmm1,XMMWORD PTR [rbx+0x10]
000EA9C3: movups    XMMWORD PTR [rcx+0x10],xmm1
000EA9C7: movups    xmm0,XMMWORD PTR [rbx+0x20]
000EA9CB: movups    XMMWORD PTR [rcx+0x20],xmm0
000EA9CF: movups    xmm1,XMMWORD PTR [rbx+0x30]
000EA9D3: movups    XMMWORD PTR [rcx+0x30],xmm1
000EA9D7: movups    xmm0,XMMWORD PTR [rbx+0x40]
000EA9DB: movups    XMMWORD PTR [rcx+0x40],xmm0
000EA9DF: movups    xmm1,XMMWORD PTR [rbx+0x50]
000EA9E3: movups    XMMWORD PTR [rcx+0x50],xmm1
000EA9E7: movups    xmm0,XMMWORD PTR [rbx+0x60]
000EA9EB: movups    XMMWORD PTR [rcx+0x60],xmm0
000EA9EF: movups    xmm1,XMMWORD PTR [rbx+0x70]
000EA9F3: movups    XMMWORD PTR [rcx+0x70],xmm1
000EA9F7: movups    xmm0,XMMWORD PTR [rbx+0x80]
000EA9FE: movups    XMMWORD PTR [rcx+0x80],xmm0
000EAA05: movups    xmm1,XMMWORD PTR [rbx+0x90]
000EAA0C: movups    XMMWORD PTR [rcx+0x90],xmm1
000EAA13: movups    xmm0,XMMWORD PTR [rbx+0xa0]
000EAA1A: movups    XMMWORD PTR [rcx+0xa0],xmm0
000EAA21: lea       rdx,[rbp+0x0]
000EAA25: call      0x1800e9fd0
000EAA2A: mov       r14,r12
000EAA2D: call      0x1800899c0
000EAA32: mov       rdx,rax
000EAA35: call      0x1800badc0
000EAA3A: test      al,al
000EAA3C: je        0x1800eaa60
000EAA3E: mov       ecx,DWORD PTR [rbx+0x98]
000EAA44: cmp       DWORD PTR [rdx+0x1d28],ecx
000EAA4A: jne       0x1800eaa60
000EAA4C: mov       r14,QWORD PTR [rdx+0x1d20]
000EAA53: test      r14,r14
000EAA56: je        0x1800eaa60
000EAA58: mov       rdx,r14
000EAA5B: call      0x1800772a0
000EAA60: mov       r9,QWORD PTR [rip+0x112b981]        # 0x1812163e8
000EAA67: cmp       QWORD PTR [r9+0x6e8],0x0
000EAA6F: je        0x1800eaa7b
000EAA71: cmp       QWORD PTR [r9+0x490],0x0
000EAA79: jne       0x1800eaa7e
000EAA7B: mov       r14,r12
000EAA7E: movzx     eax,BYTE PTR [rbx+0x9c]
000EAA85: mov       BYTE PTR [rdi+0x13c],al
000EAA8B: mov       rsi,QWORD PTR [r9+0x10]
000EAA8F: cmp       BYTE PTR [r9+0x924],0x0
000EAA97: jne       0x1800eaadf
000EAA99: mov       rdx,QWORD PTR [r9+0x28]
000EAA9D: test      rdx,rdx
000EAAA0: je        0x1800eaabc
000EAAA2: mov       rax,QWORD PTR [rsi]
000EAAA5: mov       r8,QWORD PTR [r9+0x210]
000EAAAC: mov       rcx,rsi
000EAAAF: call      QWORD PTR [rax+0x4a0]
000EAAB5: mov       r9,QWORD PTR [rip+0x112b92c]        # 0x1812163e8
000EAABC: mov       rdx,QWORD PTR [r9+0x20]
000EAAC0: test      rdx,rdx
000EAAC3: je        0x1800eaadf
000EAAC5: mov       rax,QWORD PTR [rsi]
000EAAC8: mov       r8,QWORD PTR [r9+0x208]
000EAACF: mov       rcx,rsi
000EAAD2: call      QWORD PTR [rax+0x4a0]
000EAAD8: mov       r9,QWORD PTR [rip+0x112b909]        # 0x1812163e8
000EAADF: mov       QWORD PTR [rbp+0x8],r12
000EAAE3: xorps     xmm0,xmm0
000EAAE6: movdqa    XMMWORD PTR [rbp+0x10],xmm0
000EAAEB: xorps     xmm1,xmm1
000EAAEE: movups    XMMWORD PTR [rbp+0x20],xmm1
000EAAF2: movups    XMMWORD PTR [rbp+0x30],xmm1
000EAAF6: movups    XMMWORD PTR [rbp+0x40],xmm1
000EAAFA: movups    XMMWORD PTR [rbp+0x50],xmm0
000EAAFE: movdqa    xmm1,XMMWORD PTR [rip+0x10c218a]        # 0x1811acc90
000EAB06: movdqa    XMMWORD PTR [rbp+0x60],xmm1
000EAB0B: mov       BYTE PTR [rbp+0x50],0x0
000EAB0F: mov       rax,QWORD PTR [rbx+0x10]
000EAB13: mov       QWORD PTR [rbp+0x0],rax
000EAB17: lea       rax,[r9+0x528]
000EAB1E: add       r9,0x4b8
000EAB25: mov       QWORD PTR [rsp+0x20],rax
000EAB2A: mov       r8,rbx
000EAB2D: mov       rdx,rsi
000EAB30: call      0x1800ea200
000EAB35: test      r14,r14
000EAB38: je        0x1800eab57
000EAB3A: mov       rax,QWORD PTR [rsi]
000EAB3D: mov       r8,r14
000EAB40: mov       rdx,QWORD PTR [rip+0x112b8a1]        # 0x1812163e8
000EAB47: mov       rdx,QWORD PTR [rdx+0x6e8]
000EAB4E: mov       rcx,rsi
000EAB51: call      QWORD PTR [rax+0x178]
000EAB57: mov       r15,r12
000EAB5A: mov       r8,QWORD PTR [rbx+0x20]
000EAB5E: mov       rdx,QWORD PTR [rip+0x112b883]        # 0x1812163e8
000EAB65: test      r8,r8
000EAB68: je        0x1800eaba3
000EAB6A: mov       rcx,QWORD PTR [rdx+0x598]
000EAB71: test      rcx,rcx
000EAB74: je        0x1800eaba3
000EAB76: cmp       BYTE PTR [rdx+0x91b],r15b
000EAB7D: jne       0x1800eab9c
000EAB7F: mov       rax,QWORD PTR [rsi]
000EAB82: mov       rdx,rcx
000EAB85: mov       rcx,rsi
000EAB88: call      QWORD PTR [rax+0x178]
000EAB8E: mov       rdx,QWORD PTR [rip+0x112b853]        # 0x1812163e8
000EAB95: mov       BYTE PTR [rdx+0x91b],0x1
000EAB9C: mov       r15,QWORD PTR [rdx+0x478]
000EABA3: mov       rax,QWORD PTR [rsi]
000EABA6: inc       QWORD PTR [rdx+0x210]
000EABAD: mov       r8,QWORD PTR [rdx+0x210]
000EABB4: mov       rdx,QWORD PTR [rdx+0x28]
000EABB8: mov       rcx,rsi
000EABBB: call      QWORD PTR [rax+0x498]
000EABC1: mov       rax,QWORD PTR [rsi]
000EABC4: mov       rcx,rsi
000EABC7: call      QWORD PTR [rax+0x378]
000EABCD: mov       QWORD PTR [rbx+0xa8],r12
000EABD4: mov       QWORD PTR [rbx+0x30],r12
000EABD8: mov       QWORD PTR [rbx+0x28],r12
000EABDC: mov       QWORD PTR [rbx+0x68],r12
000EABE0: mov       QWORD PTR [rbx+0x8],r14
000EABE4: test      r15,r15
000EABE7: jne       0x1800eabed
000EABE9: mov       QWORD PTR [rbx+0x20],r12
000EABED: mov       rcx,QWORD PTR [rip+0x1134dc4]        # 0x18121f9b8
000EABF4: test      rcx,rcx
000EABF7: je        0x1800eacd8
000EABFD: cmp       BYTE PTR [rcx+0x428],0x0
000EAC04: je        0x1800eacd8
000EAC0A: mov       QWORD PTR [rsp+0x40],rdi
000EAC0F: lea       rax,[rsp+0x48]
000EAC14: movups    xmm0,XMMWORD PTR [rbx]
000EAC17: movups    XMMWORD PTR [rax],xmm0
000EAC1A: movups    xmm1,XMMWORD PTR [rbx+0x10]
000EAC1E: movups    XMMWORD PTR [rax+0x10],xmm1
000EAC22: movups    xmm0,XMMWORD PTR [rbx+0x20]
000EAC26: movups    XMMWORD PTR [rax+0x20],xmm0
000EAC2A: movups    xmm1,XMMWORD PTR [rbx+0x30]
000EAC2E: movups    XMMWORD PTR [rax+0x30],xmm1
000EAC32: movups    xmm0,XMMWORD PTR [rbx+0x40]
000EAC36: movups    XMMWORD PTR [rax+0x40],xmm0
000EAC3A: movups    xmm1,XMMWORD PTR [rbx+0x50]
000EAC3E: movups    XMMWORD PTR [rax+0x50],xmm1
000EAC42: movups    xmm0,XMMWORD PTR [rbx+0x60]
000EAC46: movups    XMMWORD PTR [rax+0x60],xmm0
000EAC4A: movups    xmm1,XMMWORD PTR [rbx+0x70]
000EAC4E: movups    XMMWORD PTR [rax+0x70],xmm1
000EAC52: movups    xmm0,XMMWORD PTR [rbx+0x80]
000EAC59: movups    XMMWORD PTR [rax+0x80],xmm0
000EAC60: movups    xmm1,XMMWORD PTR [rbx+0x90]
000EAC67: movups    XMMWORD PTR [rax+0x90],xmm1
000EAC6E: movups    xmm0,XMMWORD PTR [rbx+0xa0]
000EAC75: movups    XMMWORD PTR [rax+0xa0],xmm0
000EAC7C: lea       rbx,[rcx+0x4f0]
000EAC83: mov       QWORD PTR [rbp+0x128],r12
000EAC8A: call      0x18004bb40
000EAC8F: test      al,al
000EAC91: je        0x1800eaca4
000EAC93: lea       rcx,[rsp+0x40]
000EAC98: call      0x1800edb20
000EAC9D: mov       QWORD PTR [rbp+0x128],rax
000EACA4: mov       rdx,rbx
000EACA7: lea       rcx,[rbp+0xf0]
000EACAE: call      0x1800e8640
000EACB3: mov       rcx,QWORD PTR [rbp+0x128]
000EACBA: test      rcx,rcx
000EACBD: je        0x1800ead53
000EACC3: lea       rax,[rbp+0xf0]
000EACCA: cmp       rcx,rax
000EACCD: setne     dl
000EACD0: mov       rax,QWORD PTR [rcx]
000EACD3: call      QWORD PTR [rax+0x20]
000EACD6: jmp       0x1800ead53
000EACD8: lea       rax,[rsp+0x40]
000EACDD: movups    xmm0,XMMWORD PTR [rbx]
000EACE0: movups    XMMWORD PTR [rax],xmm0
000EACE3: movups    xmm1,XMMWORD PTR [rbx+0x10]
000EACE7: movups    XMMWORD PTR [rax+0x10],xmm1
000EACEB: movups    xmm0,XMMWORD PTR [rbx+0x20]
000EACEF: movups    XMMWORD PTR [rax+0x20],xmm0
000EACF3: movups    xmm1,XMMWORD PTR [rbx+0x30]
000EACF7: movups    XMMWORD PTR [rax+0x30],xmm1
000EACFB: movups    xmm0,XMMWORD PTR [rbx+0x40]
000EACFF: movups    XMMWORD PTR [rax+0x40],xmm0
000EAD03: movups    xmm1,XMMWORD PTR [rbx+0x50]
000EAD07: movups    XMMWORD PTR [rax+0x50],xmm1
000EAD0B: movups    xmm0,XMMWORD PTR [rbx+0x60]
000EAD0F: movups    XMMWORD PTR [rax+0x60],xmm0
000EAD13: movups    xmm1,XMMWORD PTR [rbx+0x70]
000EAD17: movups    XMMWORD PTR [rax+0x70],xmm1
000EAD1B: movups    xmm0,XMMWORD PTR [rbx+0x80]
000EAD22: movups    XMMWORD PTR [rax+0x80],xmm0
000EAD29: movups    xmm1,XMMWORD PTR [rbx+0x90]
000EAD30: movups    XMMWORD PTR [rax+0x90],xmm1
000EAD37: movups    xmm0,XMMWORD PTR [rbx+0xa0]
000EAD3E: movups    XMMWORD PTR [rax+0xa0],xmm0
000EAD45: lea       rdx,[rsp+0x40]
000EAD4A: mov       rcx,rdi
000EAD4D: call      0x1800ead90
000EAD52: nop       
000EAD53: lea       rcx,[rbp+0x50]
000EAD57: call      0x1800784a0
000EAD5C: mov       rcx,QWORD PTR [rbp+0x130]
000EAD63: xor       rcx,rsp
000EAD66: call      0x18010c270
000EAD6B: lea       r11,[rsp+0x240]
000EAD73: mov       rbx,QWORD PTR [r11+0x40]
000EAD77: mov       rsi,QWORD PTR [r11+0x48]
000EAD7B: mov       rsp,r11
000EAD7E: pop       r15
000EAD80: pop       r14
000EAD82: pop       r12
000EAD84: pop       rdi
000EAD85: pop       rbp
000EAD86: ret       
