; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x2AA8A0..0x2AC041; unnamed
002AA8A0: mov       rax,rsp
002AA8A3: push      rbp
002AA8A4: push      rbx
002AA8A5: push      rsi
002AA8A6: push      r13
002AA8A8: push      r15
002AA8AA: lea       rbp,[rax-0x368]
002AA8B1: sub       rsp,0x440
002AA8B8: mov       QWORD PTR [rax-0x30],rdi
002AA8BC: mov       QWORD PTR [rax-0x38],r12
002AA8C0: vmovaps   XMMWORD PTR [rax-0x58],xmm6
002AA8C5: vmovaps   XMMWORD PTR [rax-0x68],xmm7
002AA8CA: mov       rax,QWORD PTR [rip+0xbd650f]        # 0x180e80de0
002AA8D1: cmp       QWORD PTR [rax+0x1690],0x0
002AA8D9: jne       0x1802aa8e0
002AA8DB: call      0x1802851d0
002AA8E0: call      0x18028d400
002AA8E5: mov       rax,QWORD PTR [rip+0xbd64f4]        # 0x180e80de0
002AA8EC: cmp       BYTE PTR [rax+0x4e4],0x0
002AA8F3: jne       0x1802aaaca
002AA8F9: mov       edi,DWORD PTR [rax+0x3dc]
002AA8FF: mov       ecx,DWORD PTR [rax+0x3e0]
002AA905: cmp       ecx,edi
002AA907: je        0x1802aaaca
002AA90D: vmovups   xmm0,XMMWORD PTR [rax+0x464]
002AA915: vmovsd    xmm1,QWORD PTR [rax+0x474]
002AA91D: mov       eax,DWORD PTR [rax+0x3e8]
002AA923: vmovups   XMMWORD PTR [rbp-0x70],xmm0
002AA928: vmovsd    QWORD PTR [rbp-0x60],xmm1
002AA92D: mov       DWORD PTR [rbp-0x70],ecx
002AA930: lea       rcx,[rsp+0x70]
002AA935: vmovups   xmm6,XMMWORD PTR [rbp-0x70]
002AA93A: mov       DWORD PTR [rbp-0x5c],eax
002AA93D: vmovsd    xmm7,QWORD PTR [rbp-0x60]
002AA942: vmovups   XMMWORD PTR [rsp+0x70],xmm6
002AA948: vmovsd    QWORD PTR [rbp-0x80],xmm7
002AA94D: mov       DWORD PTR [rbp+0x370],edi
002AA953: call      QWORD PTR [rip+0x1c8297]        # 0x180472bf0 ; PDPerfPlugin.dll!SetFrameGenParams
002AA959: test      al,al
002AA95B: je        0x1802aaa31
002AA961: mov       rcx,QWORD PTR [rip+0xbd6478]        # 0x180e80de0
002AA968: mov       eax,DWORD PTR [rcx+0x3e0]
002AA96E: vmovups   XMMWORD PTR [rcx+0x464],xmm6
002AA976: vmovsd    QWORD PTR [rcx+0x474],xmm7
002AA97E: mov       DWORD PTR [rcx+0x3dc],eax
002AA984: call      0x1802a4730
002AA989: mov       rcx,QWORD PTR [rip+0xbd6450]        # 0x180e80de0
002AA990: call      0x180294160
002AA995: movzx     ecx,al
002AA998: call      QWORD PTR [rip+0x1c82ca]        # 0x180472c68 ; PDPerfPlugin.dll!SetFrameGeneration
002AA99E: mov       rbx,QWORD PTR [rip+0xbd643b]        # 0x180e80de0
002AA9A5: add       rbx,0x3dc
002AA9AC: call      0x180222050
002AA9B1: lea       rcx,[rip+0x15f260]        # 0x180409c18 ; 'F:\\GithubMods\\DLSS\\SkyrimUpscaler\\src\\UpscalerHooks.cpp'
002AA9B8: mov       DWORD PTR [rbp-0x68],0x1f7
002AA9BF: mov       QWORD PTR [rbp-0x70],rcx
002AA9C3: lea       r9,[rsp+0x70]
002AA9C8: mov       ecx,DWORD PTR [rbp+0x1c]
002AA9CB: lea       rdx,[rbp+0x10]
002AA9CF: mov       DWORD PTR [rbp-0x64],ecx
002AA9D2: mov       r8d,0x2
002AA9D8: vmovups   xmm0,XMMWORD PTR [rbp-0x70]
002AA9DD: lea       rcx,[rip+0x15f47c]        # 0x180409e60 ; 'void __cdecl BeforePresent(void)'
002AA9E4: mov       QWORD PTR [rsp+0x28],rbx
002AA9E9: mov       QWORD PTR [rbp-0x60],rcx
002AA9ED: lea       rcx,[rip+0x15f4f4]        # 0x180409ee8 ; 'FrameGen backend switched {} -> {}'
002AA9F4: vmovsd    xmm1,QWORD PTR [rbp-0x60]
002AA9F9: mov       QWORD PTR [rsp+0x70],rcx
002AA9FE: lea       rcx,[rbp+0x370]
002AAA05: mov       QWORD PTR [rsp+0x20],rcx
002AAA0A: mov       rcx,rax
002AAA0D: mov       QWORD PTR [rsp+0x78],0x22
002AAA16: vmovups   XMMWORD PTR [rbp+0x10],xmm0
002AAA1B: vmovsd    QWORD PTR [rbp+0x20],xmm1
002AAA20: call      0x1801a66e0
002AAA25: mov       rcx,QWORD PTR [rip+0xbd63b4]        # 0x180e80de0
002AAA2C: jmp       0x1802aaac5
002AAA31: mov       rbx,QWORD PTR [rip+0xbd63a8]        # 0x180e80de0
002AAA38: add       rbx,0x3e0
002AAA3F: call      0x180222050
002AAA44: lea       rcx,[rip+0x15f1cd]        # 0x180409c18 ; 'F:\\GithubMods\\DLSS\\SkyrimUpscaler\\src\\UpscalerHooks.cpp'
002AAA4B: mov       DWORD PTR [rbp-0x68],0x1f9
002AAA52: mov       QWORD PTR [rbp-0x70],rcx
002AAA56: lea       r9,[rsp+0x70]
002AAA5B: mov       ecx,DWORD PTR [rbp+0x1c]
002AAA5E: lea       rdx,[rbp+0x10]
002AAA62: mov       DWORD PTR [rbp-0x64],ecx
002AAA65: mov       r8d,0x4
002AAA6B: vmovups   xmm0,XMMWORD PTR [rbp-0x70]
002AAA70: lea       rcx,[rip+0x15f3e9]        # 0x180409e60 ; 'void __cdecl BeforePresent(void)'
002AAA77: mov       QWORD PTR [rsp+0x28],rbx
002AAA7C: mov       QWORD PTR [rbp-0x60],rcx
002AAA80: lea       rcx,[rip+0x15f419]        # 0x180409ea0 ; 'FrameGen backend switch {} -> {} failed; previous backend retained'
002AAA87: vmovsd    xmm1,QWORD PTR [rbp-0x60]
002AAA8C: mov       QWORD PTR [rsp+0x70],rcx
002AAA91: lea       rcx,[rbp+0x370]
002AAA98: mov       QWORD PTR [rsp+0x20],rcx
002AAA9D: mov       rcx,rax
002AAAA0: mov       QWORD PTR [rsp+0x78],0x42
002AAAA9: vmovups   XMMWORD PTR [rbp+0x10],xmm0
002AAAAE: vmovsd    QWORD PTR [rbp+0x20],xmm1
002AAAB3: call      0x1801a66e0
002AAAB8: mov       rcx,QWORD PTR [rip+0xbd6321]        # 0x180e80de0
002AAABF: mov       DWORD PTR [rcx+0x3e0],edi
002AAAC5: call      0x1802a2650
002AAACA: call      0x1802a8fb0
002AAACF: mov       rcx,QWORD PTR [rip+0xbd630a]        # 0x180e80de0
002AAAD6: call      0x180258fc0
002AAADB: mov       rcx,QWORD PTR [rip+0xbd62fe]        # 0x180e80de0
002AAAE2: mov       BYTE PTR [rip+0xbd65a9],0x0        # 0x180e81092
002AAAE9: mov       BYTE PTR [rip+0xbd6530],0x0        # 0x180e81020
002AAAF0: call      0x180298370 ; '@USVWAVH'
002AAAF5: mov       rcx,QWORD PTR [rip+0xbd6424]        # 0x180e80f20
002AAAFC: mov       rax,QWORD PTR [rcx]
002AAAFF: call      QWORD PTR [rax+0x120]
002AAB05: mov       rdx,QWORD PTR [rip+0xbd6414]        # 0x180e80f20
002AAB0C: xor       r12b,r12b
002AAB0F: mov       ecx,eax
002AAB11: xor       r13d,r13d
002AAB14: imul      rax,rcx,0x58
002AAB18: mov       rcx,QWORD PTR [rip+0xbd62c1]        # 0x180e80de0
002AAB1F: vmovups   ymm2,YMMWORD PTR [rax+rdx*1+0x40]
002AAB25: vmovups   YMMWORD PTR [rbp-0x50],ymm2
002AAB2A: vmovups   ymm0,YMMWORD PTR [rax+rdx*1+0x60]
002AAB30: vmovups   YMMWORD PTR [rbp-0x30],ymm0
002AAB35: vmovups   xmm1,XMMWORD PTR [rax+rdx*1+0x80]
002AAB3E: vmovups   XMMWORD PTR [rbp-0x10],xmm1
002AAB43: vmovsd    xmm0,QWORD PTR [rax+rdx*1+0x90]
002AAB4C: vmovsd    QWORD PTR [rbp+0x0],xmm0
002AAB51: cmp       BYTE PTR [rcx+0x343],r12b
002AAB58: jne       0x1802aafb4
002AAB5E: cmp       BYTE PTR [rcx+0x48b],r12b
002AAB65: jne       0x1802ab34e
002AAB6B: mov       BYTE PTR [rcx+0x489],0x1
002AAB72: cmp       QWORD PTR [rcx+0x1690],r13
002AAB79: je        0x1802aae0a
002AAB7F: cmp       BYTE PTR [rcx+0x487],r12b
002AAB86: je        0x1802aae0a
002AAB8C: lea       rcx,[rdx+0x148]
002AAB93: vzeroupper 
002AAB96: call      0x18016c290
002AAB9B: mov       QWORD PTR [rbp+0x370],rax
002AABA2: lea       rcx,[rbp-0x50]
002AABA6: mov       rax,QWORD PTR [rip+0xbd6233]        # 0x180e80de0
002AABAD: mov       ebx,DWORD PTR [rax+0x27c]
002AABB3: mov       edi,DWORD PTR [rax+0x278]
002AABB9: call      0x18016c140
002AABBE: mov       rcx,QWORD PTR [rip+0xbd621b]        # 0x180e80de0
002AABC5: xor       r9d,r9d
002AABC8: mov       QWORD PTR [rsp+0x68],r13
002AABCD: xor       r8d,r8d
002AABD0: mov       BYTE PTR [rsp+0x60],r12b
002AABD5: mov       edx,0x1
002AABDA: mov       DWORD PTR [rsp+0x58],r13d
002AABDF: mov       DWORD PTR [rsp+0x50],r13d
002AABE4: mov       DWORD PTR [rsp+0x48],ebx
002AABE8: mov       DWORD PTR [rsp+0x40],edi
002AABEC: mov       QWORD PTR [rsp+0x38],rax
002AABF1: lea       rax,[rbp+0x370]
002AABF8: mov       QWORD PTR [rsp+0x30],r13
002AABFD: mov       QWORD PTR [rsp+0x28],rax
002AAC02: mov       DWORD PTR [rsp+0x20],0x1
002AAC0A: call      0x1802a1240
002AAC0F: mov       rcx,QWORD PTR [rip+0xbd61ca]        # 0x180e80de0
002AAC16: add       rcx,0x7d0
002AAC1D: call      0x18016c290
002AAC22: mov       rcx,QWORD PTR [rip+0xbd61b7]        # 0x180e80de0
002AAC29: mov       QWORD PTR [rbp+0x370],rax
002AAC30: mov       ebx,DWORD PTR [rcx+0x27c]
002AAC36: mov       edi,DWORD PTR [rcx+0x278]
002AAC3C: add       rcx,0x8d8
002AAC43: call      0x18016c450
002AAC48: mov       rcx,QWORD PTR [rip+0xbd6191]        # 0x180e80de0
002AAC4F: xor       r9d,r9d
002AAC52: mov       QWORD PTR [rsp+0x68],r13
002AAC57: xor       r8d,r8d
002AAC5A: mov       BYTE PTR [rsp+0x60],r12b
002AAC5F: mov       edx,0x2
002AAC64: mov       DWORD PTR [rsp+0x58],r13d
002AAC69: mov       DWORD PTR [rsp+0x50],r13d
002AAC6E: mov       DWORD PTR [rsp+0x48],ebx
002AAC72: mov       DWORD PTR [rsp+0x40],edi
002AAC76: mov       QWORD PTR [rsp+0x38],r13
002AAC7B: mov       QWORD PTR [rsp+0x30],rax
002AAC80: lea       rax,[rbp+0x370]
002AAC87: mov       QWORD PTR [rsp+0x28],rax
002AAC8C: mov       DWORD PTR [rsp+0x20],0x1
002AAC94: call      0x1802a1240
002AAC99: mov       rcx,QWORD PTR [rip+0xbd6140]        # 0x180e80de0
002AACA0: add       rcx,0x8d8
002AACA7: call      0x18016c290
002AACAC: mov       rcx,QWORD PTR [rip+0xbd612d]        # 0x180e80de0
002AACB3: mov       r9,rax
002AACB6: mov       r8,rax
002AACB9: mov       rcx,QWORD PTR [rcx+0x1690]
002AACC0: mov       rdx,QWORD PTR [rcx]
002AACC3: mov       r10,QWORD PTR [rdx+0x170]
002AACCA: lea       rdx,[rip+0x13e0df]        # 0x1803e8db0
002AACD1: call      r10
002AACD4: lea       rcx,[rbp-0x50]
002AACD8: call      0x18016c140
002AACDD: lea       rcx,[rbp-0x50]
002AACE1: mov       QWORD PTR [rbp+0x378],rax
002AACE8: call      0x18016c010
002AACED: mov       QWORD PTR [rbp+0x380],rax
002AACF4: lea       rcx,[rsp+0x70]
002AACF9: lea       rax,[rbp+0x378]
002AAD00: mov       QWORD PTR [rsp+0x70],rax
002AAD05: lea       rax,[rbp+0x380]
002AAD0C: mov       QWORD PTR [rsp+0x78],rax
002AAD11: call      0x1802a8f40
002AAD16: mov       rax,QWORD PTR [rip+0xbd60c3]        # 0x180e80de0
002AAD1D: vxorps    xmm3,xmm3,xmm3
002AAD21: vxorps    xmm0,xmm0,xmm0
002AAD25: vxorps    xmm2,xmm2,xmm2
002AAD29: vcvtsi2ss xmm3,xmm3,DWORD PTR [rax+0x270]
002AAD31: vcvtsi2ss xmm0,xmm0,DWORD PTR [rax+0x278]
002AAD39: vdivss    xmm1,xmm0,xmm3
002AAD3D: vmovss    DWORD PTR [rax+0x1618],xmm1
002AAD45: vcvtsi2ss xmm2,xmm2,DWORD PTR [rax+0x274]
002AAD4D: vxorps    xmm0,xmm0,xmm0
002AAD51: vcvtsi2ss xmm0,xmm0,DWORD PTR [rax+0x27c]
002AAD59: vdivss    xmm1,xmm0,xmm2
002AAD5D: lea       rcx,[rbp-0x50]
002AAD61: vmovss    DWORD PTR [rax+0x161c],xmm1
002AAD69: vmovss    DWORD PTR [rax+0x1620],xmm3
002AAD71: vmovss    DWORD PTR [rax+0x1624],xmm2
002AAD79: call      0x18016c290
002AAD7E: mov       rcx,QWORD PTR [rip+0xbd619b]        # 0x180e80f20
002AAD85: mov       QWORD PTR [rbp+0x370],rax
002AAD8C: add       rcx,0x148
002AAD93: mov       rax,QWORD PTR [rip+0xbd6046]        # 0x180e80de0
002AAD9A: mov       ebx,DWORD PTR [rax+0x27c]
002AADA0: mov       edi,DWORD PTR [rax+0x278]
002AADA6: call      0x18016c140
002AADAB: mov       rcx,QWORD PTR [rip+0xbd602e]        # 0x180e80de0
002AADB2: xor       r9d,r9d
002AADB5: mov       QWORD PTR [rsp+0x68],r13
002AADBA: xor       r8d,r8d
002AADBD: mov       BYTE PTR [rsp+0x60],r12b
002AADC2: mov       edx,0x3
002AADC7: mov       DWORD PTR [rsp+0x58],r13d
002AADCC: mov       DWORD PTR [rsp+0x50],r13d
002AADD1: mov       DWORD PTR [rsp+0x48],ebx
002AADD5: mov       DWORD PTR [rsp+0x40],edi
002AADD9: mov       QWORD PTR [rsp+0x38],rax
002AADDE: lea       rax,[rbp+0x370]
002AADE5: mov       QWORD PTR [rsp+0x30],r13
002AADEA: mov       QWORD PTR [rsp+0x28],rax
002AADEF: mov       DWORD PTR [rsp+0x20],0x1
002AADF7: call      0x1802a1240
002AADFC: mov       rcx,QWORD PTR [rip+0xbd5fdd]        # 0x180e80de0
002AAE03: mov       rdx,QWORD PTR [rip+0xbd6116]        # 0x180e80f20
002AAE0A: lea       r8,[rdx+0x148]
002AAE11: lea       rdx,[rbp-0x50]
002AAE15: vzeroupper 
002AAE18: call      0x1802949c0 ; '@UVATAVH'
002AAE1D: mov       rcx,QWORD PTR [rip+0xbd5fbc]        # 0x180e80de0
002AAE24: cmp       QWORD PTR [rcx+0x1690],r13
002AAE2B: je        0x1802aaedd
002AAE31: cmp       BYTE PTR [rcx+0x487],r12b
002AAE38: jne       0x1802aaedd
002AAE3E: add       rcx,0x7d0
002AAE45: call      0x18016c290
002AAE4A: mov       rcx,QWORD PTR [rip+0xbd5f8f]        # 0x180e80de0
002AAE51: mov       r9,rax
002AAE54: mov       r8,rax
002AAE57: mov       rcx,QWORD PTR [rcx+0x1690]
002AAE5E: mov       rdx,QWORD PTR [rcx]
002AAE61: mov       r10,QWORD PTR [rdx+0x170]
002AAE68: lea       rdx,[rip+0x13df41]        # 0x1803e8db0
002AAE6F: call      r10
002AAE72: lea       rcx,[rbp-0x50]
002AAE76: call      0x18016c140
002AAE7B: lea       rcx,[rbp-0x50]
002AAE7F: mov       QWORD PTR [rbp+0x370],rax
002AAE86: call      0x18016c010
002AAE8B: mov       QWORD PTR [rbp+0x378],rax
002AAE92: lea       rcx,[rsp+0x70]
002AAE97: lea       rax,[rbp+0x370]
002AAE9E: mov       QWORD PTR [rsp+0x70],rax
002AAEA3: lea       rax,[rbp+0x378]
002AAEAA: mov       QWORD PTR [rsp+0x78],rax
002AAEAF: call      0x1802a8eb0
002AAEB4: mov       rdx,QWORD PTR [rip+0xbd5f25]        # 0x180e80de0
002AAEBB: mov       r8,QWORD PTR [rbp-0x50]
002AAEBF: mov       rcx,QWORD PTR [rdx+0x1680]
002AAEC6: mov       rdx,QWORD PTR [rdx+0x778]
002AAECD: mov       rax,QWORD PTR [rcx]
002AAED0: call      QWORD PTR [rax+0x178]
002AAED6: mov       rcx,QWORD PTR [rip+0xbd5f03]        # 0x180e80de0
002AAEDD: mov       BYTE PTR [rcx+0x489],r12b
002AAEE4: cmp       BYTE PTR [rcx+0x48d],r12b
002AAEEB: je        0x1802ab34e
002AAEF1: mov       edi,DWORD PTR [rcx+0x270]
002AAEF7: mov       ebx,DWORD PTR [rcx+0x274]
002AAEFD: add       rcx,0x828
002AAF04: call      0x18016c290
002AAF09: lea       rcx,[rbp-0x50]
002AAF0D: mov       QWORD PTR [rbp+0x380],rax
002AAF14: call      0x18016c140
002AAF19: mov       r10,rax
002AAF1C: mov       QWORD PTR [rsp+0x68],r13
002AAF21: mov       BYTE PTR [rsp+0x60],r12b
002AAF26: mov       eax,ebx
002AAF28: cdq       
002AAF29: and       edx,0x3
002AAF2C: vxorps    xmm0,xmm0,xmm0
002AAF30: vcvtsi2ss xmm0,xmm0,ebx
002AAF34: vmulss    xmm0,xmm0,DWORD PTR [rip+0x160d20]        # 0x18040bc5c
002AAF3C: lea       ecx,[rdx+rax*1]
002AAF3F: mov       eax,edi
002AAF41: cdq       
002AAF42: sar       ecx,0x2
002AAF45: vcvttss2si r9d,xmm0
002AAF49: mov       DWORD PTR [rsp+0x58],r9d
002AAF4E: and       edx,0x3
002AAF51: add       eax,edx
002AAF53: mov       r9d,0x1
002AAF59: sar       eax,0x2
002AAF5C: mov       edx,0x4
002AAF61: vxorps    xmm1,xmm1,xmm1
002AAF65: vcvtsi2ss xmm1,xmm1,edi
002AAF69: vmulss    xmm0,xmm1,DWORD PTR [rip+0x160d87]        # 0x18040bcf8
002AAF71: vcvttss2si r8d,xmm0
002AAF75: mov       DWORD PTR [rsp+0x50],r8d
002AAF7A: xor       r8d,r8d
002AAF7D: mov       DWORD PTR [rsp+0x48],ecx
002AAF81: mov       rcx,QWORD PTR [rip+0xbd5e58]        # 0x180e80de0
002AAF88: mov       DWORD PTR [rsp+0x40],eax
002AAF8C: lea       rax,[rbp+0x380]
002AAF93: mov       QWORD PTR [rsp+0x38],r10
002AAF98: mov       QWORD PTR [rsp+0x30],r13
002AAF9D: mov       QWORD PTR [rsp+0x28],rax
002AAFA2: mov       DWORD PTR [rsp+0x20],0x1
002AAFAA: call      0x1802a1240
002AAFAF: jmp       0x1802ab34e
002AAFB4: add       rdx,0x148
002AAFBB: je        0x1802aafe9
002AAFBD: vmovq     rax,xmm2
002AAFC2: test      rax,rax
002AAFC5: je        0x1802aafe9
002AAFC7: mov       rdx,QWORD PTR [rdx]
002AAFCA: test      rdx,rdx
002AAFCD: je        0x1802aafe9
002AAFCF: cmp       QWORD PTR [rcx+0x1680],r13
002AAFD6: je        0x1802aafe9
002AAFD8: cmp       rax,rdx
002AAFDB: je        0x1802aafe9
002AAFDD: lea       r8,[rbp-0x50]
002AAFE1: vzeroupper 
002AAFE4: call      0x180298770 ; '@USWAVH'
002AAFE9: vzeroupper 
002AAFEC: call      0x180296e90
002AAFF1: mov       rcx,QWORD PTR [rip+0xbd5de8]        # 0x180e80de0
002AAFF8: cmp       DWORD PTR [rcx+0x3dc],0x2
002AAFFF: je        0x1802ab00e
002AB001: cmp       BYTE PTR [rcx+0x4a5],r12b
002AB008: je        0x1802ab1ff
002AB00E: cmp       BYTE PTR [rcx+0x3d2],r12b
002AB015: je        0x1802ab1ff
002AB01B: cmp       BYTE PTR [rcx+0x2aa],r12b
002AB022: jne       0x1802ab1ff
002AB028: cmp       BYTE PTR [rcx+0x2a8],r12b
002AB02F: je        0x1802ab1ff
002AB035: cmp       BYTE PTR [rcx+0x2a9],r12b
002AB03C: je        0x1802ab1ff
002AB042: cmp       QWORD PTR [rcx+0xb50],r13
002AB049: je        0x1802ab1ff
002AB04F: cmp       QWORD PTR [rcx+0xba8],r13
002AB056: je        0x1802ab1ff
002AB05C: add       rcx,0xa38
002AB063: call      0x18016c290
002AB068: test      rax,rax
002AB06B: je        0x1802ab1f8
002AB071: mov       rcx,QWORD PTR [rip+0xbd5d68]        # 0x180e80de0
002AB078: add       rcx,0x988
002AB07F: call      0x18016c290
002AB084: test      rax,rax
002AB087: je        0x1802ab1f8
002AB08D: lea       rcx,[rbp-0x50]
002AB091: call      0x18016c140
002AB096: test      rax,rax
002AB099: je        0x1802ab1f8
002AB09F: mov       rcx,QWORD PTR [rip+0xbd5d3a]        # 0x180e80de0
002AB0A6: mov       eax,DWORD PTR [rip+0x1b6034]        # 0x1804610e0
002AB0AC: cmp       DWORD PTR [rcx+0x460],eax
002AB0B2: jne       0x1802ab0c1
002AB0B4: cmp       BYTE PTR [rip+0x1cac89],r12b        # 0x180475d44
002AB0BB: jne       0x1802ab1ff
002AB0C1: mov       r9b,0x1
002AB0C4: lea       rdx,[rcx+0xa38]
002AB0CB: movzx     r8d,r9b
002AB0CF: call      0x1802a2300
002AB0D4: mov       rcx,QWORD PTR [rip+0xbd5d05]        # 0x180e80de0
002AB0DB: add       rcx,0xa38
002AB0E2: call      0x18016c290
002AB0E7: mov       QWORD PTR [rbp+0x370],rax
002AB0EE: lea       rcx,[rbp-0x50]
002AB0F2: mov       rax,QWORD PTR [rip+0xbd5ce7]        # 0x180e80de0
002AB0F9: mov       ebx,DWORD PTR [rax+0x274]
002AB0FF: mov       edi,DWORD PTR [rax+0x270]
002AB105: call      0x18016c140
002AB10A: mov       rcx,QWORD PTR [rip+0xbd5ccf]        # 0x180e80de0
002AB111: xor       r9d,r9d
002AB114: mov       QWORD PTR [rsp+0x68],r13
002AB119: xor       r8d,r8d
002AB11C: mov       BYTE PTR [rsp+0x60],r12b
002AB121: mov       edx,0x1
002AB126: mov       DWORD PTR [rsp+0x58],r13d
002AB12B: mov       DWORD PTR [rsp+0x50],r13d
002AB130: mov       DWORD PTR [rsp+0x48],ebx
002AB134: mov       DWORD PTR [rsp+0x40],edi
002AB138: mov       QWORD PTR [rsp+0x38],rax
002AB13D: lea       rax,[rbp+0x370]
002AB144: mov       QWORD PTR [rsp+0x30],r13
002AB149: mov       QWORD PTR [rsp+0x28],rax
002AB14E: mov       DWORD PTR [rsp+0x20],0x1
002AB156: call      0x1802a1240
002AB15B: mov       rcx,QWORD PTR [rip+0xbd5c7e]        # 0x180e80de0
002AB162: add       rcx,0x988
002AB169: call      0x18016c290
002AB16E: mov       QWORD PTR [rbp+0x378],rax
002AB175: lea       rcx,[rbp-0x50]
002AB179: mov       rax,QWORD PTR [rip+0xbd5c60]        # 0x180e80de0
002AB180: mov       ebx,DWORD PTR [rax+0x274]
002AB186: mov       edi,DWORD PTR [rax+0x270]
002AB18C: call      0x18016c140
002AB191: mov       rcx,QWORD PTR [rip+0xbd5c48]        # 0x180e80de0
002AB198: mov       r9d,0x2
002AB19E: mov       QWORD PTR [rsp+0x68],r13
002AB1A3: xor       r8d,r8d
002AB1A6: mov       BYTE PTR [rsp+0x60],r12b
002AB1AB: mov       edx,0x1
002AB1B0: mov       DWORD PTR [rsp+0x58],r13d
002AB1B5: mov       DWORD PTR [rsp+0x50],r13d
002AB1BA: mov       DWORD PTR [rsp+0x48],ebx
002AB1BE: mov       DWORD PTR [rsp+0x40],edi
002AB1C2: mov       QWORD PTR [rsp+0x38],rax
002AB1C7: lea       rax,[rbp+0x378]
002AB1CE: mov       QWORD PTR [rsp+0x30],r13
002AB1D3: mov       QWORD PTR [rsp+0x28],rax
002AB1D8: mov       DWORD PTR [rsp+0x20],0x1
002AB1E0: call      0x1802a1240
002AB1E5: mov       rcx,QWORD PTR [rip+0xbd5bf4]        # 0x180e80de0
002AB1EC: mov       r12b,0x1
002AB1EF: mov       BYTE PTR [rcx+0x48c],0x1
002AB1F6: jmp       0x1802ab1ff
002AB1F8: mov       rcx,QWORD PTR [rip+0xbd5be1]        # 0x180e80de0
002AB1FF: cmp       BYTE PTR [rcx+0x3d3],r13b
002AB206: je        0x1802ab2bf
002AB20C: cmp       BYTE PTR [rcx+0x3d2],r13b
002AB213: je        0x1802ab2bf
002AB219: test      r12b,r12b
002AB21C: jne       0x1802ab2bf
002AB222: add       rcx,0x988
002AB229: call      0x18016c290
002AB22E: mov       QWORD PTR [rbp+0x380],rax
002AB235: test      rax,rax
002AB238: je        0x1802ab2bf
002AB23E: lea       rcx,[rbp-0x50]
002AB242: call      0x18016c140
002AB247: test      rax,rax
002AB24A: je        0x1802ab2bf
002AB24C: mov       rax,QWORD PTR [rip+0xbd5b8d]        # 0x180e80de0
002AB253: lea       rcx,[rbp-0x50]
002AB257: mov       ebx,DWORD PTR [rax+0x274]
002AB25D: mov       edi,DWORD PTR [rax+0x270]
002AB263: call      0x18016c140
002AB268: mov       rcx,QWORD PTR [rip+0xbd5b71]        # 0x180e80de0
002AB26F: mov       r9d,0x2
002AB275: mov       QWORD PTR [rsp+0x68],r13
002AB27A: xor       r8d,r8d
002AB27D: mov       BYTE PTR [rsp+0x60],r13b
002AB282: mov       edx,0x1
002AB287: mov       DWORD PTR [rsp+0x58],r13d
002AB28C: mov       DWORD PTR [rsp+0x50],r13d
002AB291: mov       DWORD PTR [rsp+0x48],ebx
002AB295: mov       DWORD PTR [rsp+0x40],edi
002AB299: mov       QWORD PTR [rsp+0x38],rax
002AB29E: lea       rax,[rbp+0x380]
002AB2A5: mov       QWORD PTR [rsp+0x30],r13
002AB2AA: mov       QWORD PTR [rsp+0x28],rax
002AB2AF: mov       DWORD PTR [rsp+0x20],0x1
002AB2B7: call      0x1802a1240
002AB2BC: mov       r12b,0x1
002AB2BF: lea       rcx,[rbp-0x50]
002AB2C3: call      0x18016c140
002AB2C8: mov       QWORD PTR [rbp+0x388],rax
002AB2CF: test      rax,rax
002AB2D2: je        0x1802ab2fa
002AB2D4: mov       rax,QWORD PTR [rip+0xbd5b05]        # 0x180e80de0
002AB2DB: lea       r8,[rbp+0x388]
002AB2E2: xor       r9d,r9d
002AB2E5: mov       edx,0x1
002AB2EA: mov       rcx,QWORD PTR [rax+0x1680]
002AB2F1: mov       rax,QWORD PTR [rcx]
002AB2F4: call      QWORD PTR [rax+0x108]
002AB2FA: call      0x18028b450
002AB2FF: mov       rax,QWORD PTR [rip+0xbd5ada]        # 0x180e80de0
002AB306: cmp       BYTE PTR [rax+0x3d2],r13b
002AB30D: je        0x1802ab32a
002AB30F: lea       rcx,[rax+0x988]
002AB316: call      0x18016c140
002AB31B: mov       rdx,rax
002AB31E: call      0x180285500
002AB323: mov       rax,QWORD PTR [rip+0xbd5ab6]        # 0x180e80de0
002AB32A: cmp       BYTE PTR [rax+0x4e4],r13b
002AB331: jne       0x1802ab34e
002AB333: cmp       DWORD PTR [rax+0x3dc],0x2
002AB33A: je        0x1802ab34e
002AB33C: cmp       BYTE PTR [rax+0x4a5],r13b
002AB343: jne       0x1802ab34e
002AB345: lea       rdx,[rbp-0x50]
002AB349: call      0x1802941a0
002AB34E: vzeroupper 
002AB351: call      0x180152f00
002AB356: cmp       BYTE PTR [rax+0x11],r13b
002AB35A: jne       0x1802ab399
002AB35C: cmp       BYTE PTR [rax+0x12],r13b
002AB360: jne       0x1802ab399
002AB362: call      0x1801ac3d0
002AB367: mov       rbx,rax
002AB36A: test      rax,rax
002AB36D: je        0x1802ab395
002AB36F: lea       rdx,[rip+0x13a3c2]        # 0x1803e5738
002AB376: mov       rcx,rax
002AB379: call      0x1801ac480
002AB37E: test      al,al
002AB380: jne       0x1802ab399
002AB382: lea       rdx,[rip+0x13a38f]        # 0x1803e5718
002AB389: mov       rcx,rbx
002AB38C: call      0x1801ac480
002AB391: test      al,al
002AB393: jne       0x1802ab399
002AB395: xor       bl,bl
002AB397: jmp       0x1802ab39b
002AB399: mov       bl,0x1
002AB39B: mov       rax,QWORD PTR [rip+0xbd5a3e]        # 0x180e80de0
002AB3A2: cmp       BYTE PTR [rax+0x343],r13b
002AB3A9: jne       0x1802ab3ce
002AB3AB: mov       rcx,rax
002AB3AE: call      0x180293ec0
002AB3B3: test      al,al
002AB3B5: je        0x1802ab3c7
002AB3B7: test      bl,bl
002AB3B9: jne       0x1802ab3c7
002AB3BB: mov       rax,QWORD PTR [rip+0xbd5a1e]        # 0x180e80de0
002AB3C2: mov       dil,0x1
002AB3C5: jmp       0x1802ab3d1
002AB3C7: mov       rax,QWORD PTR [rip+0xbd5a12]        # 0x180e80de0
002AB3CE: xor       dil,dil
002AB3D1: mov       QWORD PTR [rsp+0x428],r14
002AB3D9: cmp       BYTE PTR [rax+0x343],r13b
002AB3E0: jne       0x1802ab406
002AB3E2: cmp       BYTE PTR [rax+0x3d3],r13b
002AB3E9: je        0x1802ab406
002AB3EB: cmp       QWORD PTR [rax+0x988],r13
002AB3F2: je        0x1802ab406
002AB3F4: cmp       QWORD PTR [rax+0x9e0],r13
002AB3FB: je        0x1802ab406
002AB3FD: test      bl,bl
002AB3FF: jne       0x1802ab406
002AB401: mov       r14b,0x1
002AB404: jmp       0x1802ab409
002AB406: xor       r14b,r14b
002AB409: test      dil,dil
002AB40C: je        0x1802ab4dc
002AB412: lea       rcx,[rbp-0x50]
002AB416: call      0x18016c140
002AB41B: mov       QWORD PTR [rbp+0x370],rax
002AB422: lea       r8,[rbp+0x370]
002AB429: mov       rax,QWORD PTR [rip+0xbd59b0]        # 0x180e80de0
002AB430: xor       r9d,r9d
002AB433: mov       edx,0x1
002AB438: mov       rcx,QWORD PTR [rax+0x1680]
002AB43F: mov       rax,QWORD PTR [rcx]
002AB442: call      QWORD PTR [rax+0x108]
002AB448: call      0x180296e90
002AB44D: call      0x18028b450
002AB452: mov       rcx,QWORD PTR [rip+0xbd5987]        # 0x180e80de0
002AB459: add       rcx,0x988
002AB460: call      0x18016c140
002AB465: mov       rcx,QWORD PTR [rip+0xbd597c]        # 0x180e80de8
002AB46C: mov       QWORD PTR [rbp+0x378],rax
002AB473: cmp       BYTE PTR [rcx],r13b
002AB476: je        0x1802ab4d0
002AB478: cmp       BYTE PTR [rcx+0xc8],r13b
002AB47F: je        0x1802ab4d0
002AB481: test      rax,rax
002AB484: je        0x1802ab4d0
002AB486: call      0x1801d2c40
002AB48B: mov       rbx,rax
002AB48E: test      rax,rax
002AB491: je        0x1802ab4d0
002AB493: cmp       DWORD PTR [rax+0x4],r13d
002AB497: jle       0x1802ab4d0
002AB499: mov       rcx,QWORD PTR [rip+0xbd5868]        # 0x180e80d08
002AB4A0: mov       rcx,QWORD PTR [rcx+0x1680]
002AB4A7: test      rcx,rcx
002AB4AA: je        0x1802ab4d0
002AB4AC: mov       rdx,QWORD PTR [rcx]
002AB4AF: lea       r8,[rbp+0x378]
002AB4B6: xor       r9d,r9d
002AB4B9: mov       r10,QWORD PTR [rdx+0x108]
002AB4C0: mov       edx,0x1
002AB4C5: call      r10
002AB4C8: mov       rcx,rbx
002AB4CB: call      0x180212eb0
002AB4D0: mov       rax,QWORD PTR [rip+0xbd5909]        # 0x180e80de0
002AB4D7: jmp       0x1802ab56e
002AB4DC: test      r14b,r14b
002AB4DF: je        0x1802ab56e
002AB4E5: lea       rcx,[rbp-0x50]
002AB4E9: call      0x18016c140
002AB4EE: mov       QWORD PTR [rbp+0x380],rax
002AB4F5: lea       r8,[rbp+0x380]
002AB4FC: mov       rax,QWORD PTR [rip+0xbd58dd]        # 0x180e80de0
002AB503: xor       r9d,r9d
002AB506: mov       edx,0x1
002AB50B: mov       rcx,QWORD PTR [rax+0x1680]
002AB512: mov       rax,QWORD PTR [rcx]
002AB515: call      QWORD PTR [rax+0x108]
002AB51B: call      0x18028b450
002AB520: mov       rdx,QWORD PTR [rip+0xbd58b9]        # 0x180e80de0
002AB527: mov       rcx,QWORD PTR [rdx+0x1680]
002AB52E: mov       r8,QWORD PTR [rdx+0x9e0]
002AB535: mov       rdx,QWORD PTR [rdx+0x988]
002AB53C: mov       rax,QWORD PTR [rcx]
002AB53F: call      QWORD PTR [rax+0x178]
002AB545: mov       rcx,QWORD PTR [rip+0xbd5894]        # 0x180e80de0
002AB54C: add       rcx,0x988
002AB553: call      0x18016c140
002AB558: mov       rdx,rax
002AB55B: call      0x180285500
002AB560: mov       rax,QWORD PTR [rip+0xbd5879]        # 0x180e80de0
002AB567: mov       BYTE PTR [rax+0x3d2],0x1
002AB56E: cmp       BYTE PTR [rax+0x2aa],r13b
002AB575: jne       0x1802ab5d8
002AB577: cmp       BYTE PTR [rax+0x4e4],r13b
002AB57E: je        0x1802ab59f
002AB580: cmp       BYTE PTR [rax+0x4a5],r13b
002AB587: je        0x1802ab59f
002AB589: mov       eax,DWORD PTR [rip+0x1b3f61]        # 0x18045f4f0
002AB58F: and       eax,0xb8
002AB594: cmp       al,0xb8
002AB596: mov       rax,QWORD PTR [rip+0xbd5843]        # 0x180e80de0
002AB59D: je        0x1802ab5d8
002AB59F: cmp       DWORD PTR [rax+0x3dc],0x2
002AB5A6: je        0x1802ab5b6
002AB5A8: cmp       BYTE PTR [rax+0x4a5],r13b
002AB5AF: jne       0x1802ab5b6
002AB5B1: xor       r8d,r8d
002AB5B4: jmp       0x1802ab5c2
002AB5B6: cmp       BYTE PTR [rax+0x48c],r13b
002AB5BD: jne       0x1802ab5d8
002AB5BF: mov       r8b,0x1
002AB5C2: xor       r9d,r9d
002AB5C5: lea       rdx,[rbp-0x50]
002AB5C9: mov       rcx,rax
002AB5CC: call      0x1802a2300
002AB5D1: mov       rax,QWORD PTR [rip+0xbd5808]        # 0x180e80de0
002AB5D8: cmp       BYTE PTR [rax+0x343],r13b
002AB5DF: jne       0x1802ab62d
002AB5E1: test      dil,dil
002AB5E4: jne       0x1802ab62d
002AB5E6: test      r14b,r14b
002AB5E9: jne       0x1802ab62d
002AB5EB: lea       rcx,[rbp-0x50]
002AB5EF: mov       BYTE PTR [rax+0x3d2],r13b
002AB5F6: call      0x18016c140
002AB5FB: mov       QWORD PTR [rbp+0x370],rax
002AB602: lea       r8,[rbp+0x370]
002AB609: mov       rax,QWORD PTR [rip+0xbd57d0]        # 0x180e80de0
002AB610: xor       r9d,r9d
002AB613: mov       edx,0x1
002AB618: mov       rcx,QWORD PTR [rax+0x1680]
002AB61F: mov       rax,QWORD PTR [rcx]
002AB622: call      QWORD PTR [rax+0x108]
002AB628: call      0x18028b450
002AB62D: call      0x180152f00
002AB632: mov       r15d,0xac
002AB638: mov       DWORD PTR [rax+0xc],0x3f800000
002AB63F: mov       rax,QWORD PTR gs:0x58
002AB648: mov       ecx,DWORD PTR [rip+0x1c7e66]        # 0x1804734b4
002AB64E: mov       rsi,QWORD PTR [rax+rcx*8]
002AB652: mov       eax,DWORD PTR [rsi+r15*1]
002AB656: cmp       DWORD PTR [rip+0xbd62b4],eax        # 0x180e81910
002AB65C: jg        0x1802ac00b
002AB662: lea       rcx,[rbp+0x370]
002AB669: call      0x180179bf0
002AB66E: mov       rax,QWORD PTR [rbp+0x370]
002AB675: mov       rbx,QWORD PTR [rip+0xbd5764]        # 0x180e80de0
002AB67C: mov       rcx,rax
002AB67F: sub       rcx,QWORD PTR [rip+0xbd6292]        # 0x180e81918
002AB686: vxorps    xmm0,xmm0,xmm0
002AB68A: vcvtsi2sd xmm0,xmm0,rcx
002AB68F: vmulsd    xmm0,xmm0,QWORD PTR [rip+0x160609]        # 0x18040bca0
002AB697: vcvtsd2ss xmm1,xmm0,xmm0
002AB69B: vmovss    DWORD PTR [rbx+0x504],xmm1
002AB6A3: mov       QWORD PTR [rip+0xbd626e],rax        # 0x180e81918
002AB6AA: vmovss    xmm6,DWORD PTR [rbx+0x4c0]
002AB6B2: vmovaps   xmm0,xmm6
002AB6B6: call      0x180130d10
002AB6BB: vxorps    xmm7,xmm7,xmm7
002AB6BF: test      al,al
002AB6C1: je        0x1802ab7e5
002AB6C7: vcomiss   xmm6,DWORD PTR [rip+0x1607b5]        # 0x18040be84
002AB6CF: jb        0x1802ab7e5
002AB6D5: lea       rcx,[rbp+0x370]
002AB6DC: call      0x180179bf0
002AB6E1: vmovss    xmm0,DWORD PTR [rip+0xbd58a7]        # 0x180e80f90
002AB6E9: vucomiss  xmm0,xmm6
002AB6ED: jne       0x1802ab7c6
002AB6F3: mov       rbx,QWORD PTR [rip+0xbd588e]        # 0x180e80f88
002AB6FA: test      rbx,rbx
002AB6FD: je        0x1802ab7c6
002AB703: vmovsd    xmm0,QWORD PTR [rip+0x160845]        # 0x18040bf50
002AB70B: mov       rdi,QWORD PTR [rbp+0x370]
002AB712: vcvtss2sd xmm1,xmm6,xmm6
002AB716: vdivsd    xmm0,xmm0,xmm1
002AB71A: vcvttsd2si rax,xmm0
002AB71F: sub       rbx,rax
002AB722: mov       QWORD PTR [rbp+0x378],rbx
002AB729: cmp       rdi,rbx
002AB72C: jge       0x1802ab7c6
002AB732: mov       eax,DWORD PTR [rsi+r15*1]
002AB736: cmp       DWORD PTR [rip+0xbd61e4],eax        # 0x180e81920
002AB73C: jg        0x1802abfb8
002AB742: mov       rcx,QWORD PTR [rip+0xbd61df]        # 0x180e81928
002AB749: sub       rbx,rdi
002AB74C: movabs    rax,0xa3d70a3d70a3d70b
002AB756: imul      rbx
002AB759: add       rdx,rbx
002AB75C: sar       rdx,0x6
002AB760: mov       rax,rdx
002AB763: shr       rax,0x3f
002AB767: add       rdx,rax
002AB76A: mov       eax,0x1
002AB76F: cmp       rdx,rax
002AB772: cmovg     rax,rdx
002AB776: neg       rax
002AB779: mov       QWORD PTR [rbp+0x370],rax
002AB780: test      rcx,rcx
002AB783: je        0x1802ab7ba
002AB785: mov       DWORD PTR [rsp+0x28],r13d
002AB78A: lea       rdx,[rbp+0x370]
002AB791: xor       r9d,r9d
002AB794: mov       QWORD PTR [rsp+0x20],r13
002AB799: xor       r8d,r8d
002AB79C: call      QWORD PTR [rip+0x17aa6]        # 0x1802c3248
002AB7A2: test      eax,eax
002AB7A4: je        0x1802ab7ba
002AB7A6: mov       rcx,QWORD PTR [rip+0xbd617b]        # 0x180e81928
002AB7AD: mov       edx,0xffffffff
002AB7B2: call      QWORD PTR [rip+0x17a98]        # 0x1802c3250
002AB7B8: jmp       0x1802ab7c6
002AB7BA: lea       rcx,[rbp+0x378]
002AB7C1: call      0x180173ab0
002AB7C6: lea       rcx,[rbp+0x370]
002AB7CD: call      0x180179bf0
002AB7D2: mov       rbx,QWORD PTR [rip+0xbd5607]        # 0x180e80de0
002AB7D9: mov       rcx,QWORD PTR [rax]
002AB7DC: mov       QWORD PTR [rip+0xbd57a5],rcx        # 0x180e80f88
002AB7E3: jmp       0x1802ab7f0
002AB7E5: mov       QWORD PTR [rip+0xbd579c],r13        # 0x180e80f88
002AB7EC: vxorps    xmm6,xmm6,xmm6
002AB7F0: vmovss    DWORD PTR [rip+0xbd5798],xmm6        # 0x180e80f90
002AB7F8: vmovaps   xmm6,XMMWORD PTR [rsp+0x410]
002AB801: cmp       BYTE PTR [rbx+0x3d3],r13b
002AB808: jne       0x1802ab833
002AB80A: cmp       BYTE PTR [rbx+0x343],r13b
002AB811: je        0x1802ab82e
002AB813: cmp       BYTE PTR [rbx+0x3d2],r13b
002AB81A: je        0x1802ab82e
002AB81C: cmp       BYTE PTR [rbx+0x3d0],r13b
002AB823: je        0x1802ab82e
002AB825: cmp       QWORD PTR [rbx+0x1570],r13
002AB82C: jne       0x1802ab833
002AB82E: xor       dil,dil
002AB831: jmp       0x1802ab836
002AB833: mov       dil,0x1
002AB836: mov       rcx,rbx
002AB839: call      0x180294090
002AB83E: mov       rcx,QWORD PTR [rip+0xbd559b]        # 0x180e80de0
002AB845: test      al,al
002AB847: je        0x1802ab883
002AB849: cmp       BYTE PTR [rbx+0x3d0],r13b
002AB850: jne       0x1802ab883
002AB852: cmp       DWORD PTR [rbx+0x4fc],r13d
002AB859: jne       0x1802ab883
002AB85B: cmp       DWORD PTR [rcx+0x3dc],0x2
002AB862: je        0x1802ab883
002AB864: cmp       BYTE PTR [rcx+0x4a5],r13b
002AB86B: jne       0x1802ab883
002AB86D: cmp       BYTE PTR [rcx+0x2aa],r13b
002AB874: jne       0x1802ab883
002AB876: cmp       BYTE PTR [rcx+0x2a9],r13b
002AB87D: je        0x1802ab883
002AB87F: mov       bl,0x1
002AB881: jmp       0x1802ab885
002AB883: xor       bl,bl
002AB885: mov       eax,DWORD PTR [rip+0x1b3c65]        # 0x18045f4f0
002AB88B: and       eax,0xb8
002AB890: cmp       al,0xb8
002AB892: jne       0x1802ab8f6
002AB894: cmp       BYTE PTR [rcx+0x4e4],r13b
002AB89B: je        0x1802ab8f6
002AB89D: cmp       BYTE PTR [rcx+0x4a5],r13b
002AB8A4: je        0x1802ab8f6
002AB8A6: mov       eax,DWORD PTR [rip+0x1b3c44]        # 0x18045f4f0
002AB8AC: and       eax,0xb8
002AB8B1: cmp       al,0xb8
002AB8B3: jne       0x1802ab8f6
002AB8B5: cmp       BYTE PTR [rcx+0x168b],r13b
002AB8BC: jne       0x1802ab8f6
002AB8BE: call      0x180152f00
002AB8C3: mov       rcx,rax
002AB8C6: call      0x18025edd0
002AB8CB: test      al,al
002AB8CD: jne       0x1802ab8f6
002AB8CF: mov       rcx,QWORD PTR [rip+0xbd550a]        # 0x180e80de0
002AB8D6: cmp       BYTE PTR [rcx+0x3d2],r13b
002AB8DD: je        0x1802ab8fd
002AB8DF: cmp       QWORD PTR [rcx+0x988],r13
002AB8E6: je        0x1802ab8fd
002AB8E8: cmp       QWORD PTR [rcx+0xa38],r13
002AB8EF: je        0x1802ab8fd
002AB8F1: mov       r15b,0x1
002AB8F4: jmp       0x1802ab900
002AB8F6: mov       rcx,QWORD PTR [rip+0xbd54e3]        # 0x180e80de0
002AB8FD: xor       r15b,r15b
002AB900: mov       esi,0x5
002AB905: test      dil,dil
002AB908: je        0x1802aba56
002AB90E: mov       rdx,QWORD PTR [rcx+0x988]
002AB915: test      rdx,rdx
002AB918: je        0x1802aba56
002AB91E: mov       r8,QWORD PTR [rcx+0x9e0]
002AB925: test      r8,r8
002AB928: je        0x1802aba56
002AB92E: cmp       BYTE PTR [rcx+0x3d2],r13b
002AB935: jne       0x1802ab94e
002AB937: mov       rcx,QWORD PTR [rcx+0x1680]
002AB93E: mov       rax,QWORD PTR [rcx]
002AB941: call      QWORD PTR [rax+0x178]
002AB947: mov       rcx,QWORD PTR [rip+0xbd5492]        # 0x180e80de0
002AB94E: test      r12b,r12b
002AB951: jne       0x1802abbde
002AB957: test      bl,bl
002AB959: jne       0x1802abbde
002AB95F: test      r15b,r15b
002AB962: jne       0x1802abbde
002AB968: mov       eax,DWORD PTR [rcx+0x3dc]
002AB96E: test      eax,eax
002AB970: je        0x1802ab9a6
002AB972: add       eax,0xfffffffe
002AB975: cmp       eax,0x1
002AB978: jbe       0x1802ab9a6
002AB97A: cmp       BYTE PTR [rcx+0x4a5],r13b
002AB981: jne       0x1802ab9a6
002AB983: call      0x180294110
002AB988: test      al,al
002AB98A: je        0x1802ab999
002AB98C: cmp       DWORD PTR [rcx+0x4fc],0x1
002AB993: jne       0x1802abbde
002AB999: cmp       BYTE PTR [rcx+0x3d0],r13b
002AB9A0: je        0x1802abbde
002AB9A6: test      r14b,r14b
002AB9A9: jne       0x1802ab9d6
002AB9AB: call      0x180293f20
002AB9B0: mov       rcx,QWORD PTR [rip+0xbd5429]        # 0x180e80de0
002AB9B7: test      al,al
002AB9B9: jne       0x1802ab9d6
002AB9BB: cmp       BYTE PTR [rcx+0x3d0],r13b
002AB9C2: je        0x1802ab9cd
002AB9C4: cmp       QWORD PTR [rcx+0x1570],r13
002AB9CB: jne       0x1802ab9d6
002AB9CD: add       rcx,0x988
002AB9D4: jmp       0x1802ab9dd
002AB9D6: add       rcx,0x9e0
002AB9DD: call      0x18016c290
002AB9E2: mov       QWORD PTR [rbp+0x370],rax
002AB9E9: lea       rcx,[rbp-0x50]
002AB9ED: mov       rax,QWORD PTR [rip+0xbd53ec]        # 0x180e80de0
002AB9F4: mov       ebx,DWORD PTR [rax+0x274]
002AB9FA: mov       edi,DWORD PTR [rax+0x270]
002ABA00: call      0x18016c140
002ABA05: mov       rcx,QWORD PTR [rip+0xbd53d4]        # 0x180e80de0
002ABA0C: mov       r9d,0x2
002ABA12: mov       QWORD PTR [rsp+0x68],r13
002ABA17: mov       edx,0x1
002ABA1C: mov       BYTE PTR [rsp+0x60],r13b
002ABA21: mov       DWORD PTR [rsp+0x58],r13d
002ABA26: mov       DWORD PTR [rsp+0x50],r13d
002ABA2B: mov       DWORD PTR [rsp+0x48],ebx
002ABA2F: mov       DWORD PTR [rsp+0x40],edi
002ABA33: mov       QWORD PTR [rsp+0x38],rax
002ABA38: lea       rax,[rbp+0x370]
002ABA3F: mov       QWORD PTR [rsp+0x30],r13
002ABA44: mov       QWORD PTR [rsp+0x28],rax
002ABA49: mov       DWORD PTR [rsp+0x20],0x1
002ABA51: jmp       0x1802abbcf
002ABA56: cmp       BYTE PTR [rcx+0x343],r13b
002ABA5D: jne       0x1802abbde
002ABA63: mov       edx,DWORD PTR [rcx+0x4fc]
002ABA69: test      edx,edx
002ABA6B: jne       0x1802aba7a
002ABA6D: cmp       BYTE PTR [rcx+0x168b],r13b
002ABA74: jne       0x1802abbde
002ABA7A: cmp       BYTE PTR [rcx+0x4fa],r13b
002ABA81: je        0x1802abbde
002ABA87: mov       eax,DWORD PTR [rcx+0x3dc]
002ABA8D: test      eax,eax
002ABA8F: jg        0x1802abaab
002ABA91: cmp       DWORD PTR [rip+0x1c9c68],0x2        # 0x180475700
002ABA98: jne       0x1802abbde
002ABA9E: cmp       BYTE PTR [rcx+0x4a5],r13b
002ABAA5: je        0x1802abbde
002ABAAB: test      edx,edx
002ABAAD: jle       0x1802abbde
002ABAB3: cmp       eax,0x3
002ABAB6: jne       0x1802abbde
002ABABC: vxorps    xmm0,xmm0,xmm0
002ABAC0: vcvtsi2ss xmm0,xmm0,DWORD PTR [rcx+0x278]
002ABAC8: vxorps    xmm3,xmm3,xmm3
002ABACC: vcvtsi2ss xmm3,xmm3,DWORD PTR [rcx+0x270]
002ABAD4: vdivss    xmm1,xmm0,xmm3
002ABAD8: vmovss    DWORD PTR [rcx+0x1618],xmm1
002ABAE0: vxorps    xmm0,xmm0,xmm0
002ABAE4: vcvtsi2ss xmm0,xmm0,DWORD PTR [rcx+0x27c]
002ABAEC: vxorps    xmm2,xmm2,xmm2
002ABAF0: vcvtsi2ss xmm2,xmm2,DWORD PTR [rcx+0x274]
002ABAF8: vdivss    xmm1,xmm0,xmm2
002ABAFC: vmovss    DWORD PTR [rcx+0x161c],xmm1
002ABB04: vmovss    DWORD PTR [rcx+0x1620],xmm3
002ABB0C: vmovss    DWORD PTR [rcx+0x1624],xmm2
002ABB14: lea       rcx,[rbp-0x50]
002ABB18: call      0x18016c290
002ABB1D: mov       rcx,QWORD PTR [rip+0xbd52bc]        # 0x180e80de0
002ABB24: add       rcx,0xa38
002ABB2B: mov       QWORD PTR [rbp-0x70],rax
002ABB2F: call      0x18016c290
002ABB34: mov       rcx,QWORD PTR [rip+0xbd52a5]        # 0x180e80de0
002ABB3B: add       rcx,0x988
002ABB42: mov       QWORD PTR [rbp-0x68],rax
002ABB46: call      0x18016c140
002ABB4B: mov       rbx,rax
002ABB4E: test      rax,rax
002ABB51: je        0x1802abb80
002ABB53: mov       rcx,QWORD PTR [rip+0xbd5286]        # 0x180e80de0
002ABB5A: lea       r8,[rsp+0x70]
002ABB5F: vxorps    xmm0,xmm0,xmm0
002ABB63: vmovups   XMMWORD PTR [rsp+0x70],xmm0
002ABB69: mov       rcx,QWORD PTR [rcx+0x1680]
002ABB70: mov       rdx,QWORD PTR [rcx]
002ABB73: mov       r9,QWORD PTR [rdx+0x190]
002ABB7A: mov       rdx,rax
002ABB7D: call      r9
002ABB80: mov       rcx,QWORD PTR [rip+0xbd5259]        # 0x180e80de0
002ABB87: xor       r9d,r9d
002ABB8A: mov       QWORD PTR [rsp+0x68],r13
002ABB8F: mov       edx,esi
002ABB91: mov       BYTE PTR [rsp+0x60],r13b
002ABB96: mov       DWORD PTR [rsp+0x58],r13d
002ABB9B: mov       eax,DWORD PTR [rcx+0x274]
002ABBA1: mov       DWORD PTR [rsp+0x50],r13d
002ABBA6: mov       DWORD PTR [rsp+0x48],eax
002ABBAA: mov       eax,DWORD PTR [rcx+0x270]
002ABBB0: mov       DWORD PTR [rsp+0x40],eax
002ABBB4: lea       rax,[rbp-0x70]
002ABBB8: mov       QWORD PTR [rsp+0x38],rbx
002ABBBD: mov       QWORD PTR [rsp+0x30],r13
002ABBC2: mov       QWORD PTR [rsp+0x28],rax
002ABBC7: mov       DWORD PTR [rsp+0x20],0x2
002ABBCF: xor       r8d,r8d
002ABBD2: call      0x1802a1240
002ABBD7: mov       rcx,QWORD PTR [rip+0xbd5202]        # 0x180e80de0
002ABBDE: cmp       BYTE PTR [rip+0xbd53fc],r13b        # 0x180e80fe1
002ABBE5: lea       rbx,[rip+0x1c2fe4]        # 0x18046ebd0
002ABBEC: mov       r14,QWORD PTR [rsp+0x428]
002ABBF4: mov       r12,QWORD PTR [rsp+0x430]
002ABBFC: mov       rdi,QWORD PTR [rsp+0x438]
002ABC04: je        0x1802abc84
002ABC06: lea       rax,[rbp+0x30]
002ABC0A: mov       rdx,rbx
002ABC0D: mov       r8,rsi
002ABC10: lea       rax,[rax+0x80]
002ABC17: vmovups   ymm0,YMMWORD PTR [rdx]
002ABC1B: vmovups   xmm1,XMMWORD PTR [rdx+0x70]
002ABC20: lea       rdx,[rdx+0x80]
002ABC27: vmovups   YMMWORD PTR [rax-0x80],ymm0
002ABC2C: vmovups   ymm0,YMMWORD PTR [rdx-0x60]
002ABC31: vmovups   YMMWORD PTR [rax-0x60],ymm0
002ABC36: vmovups   ymm0,YMMWORD PTR [rdx-0x40]
002ABC3B: vmovups   YMMWORD PTR [rax-0x40],ymm0
002ABC40: vmovups   xmm0,XMMWORD PTR [rdx-0x20]
002ABC45: vmovups   XMMWORD PTR [rax-0x20],xmm0
002ABC4A: vmovups   XMMWORD PTR [rax-0x10],xmm1
002ABC4F: sub       r8,0x1
002ABC53: jne       0x1802abc10
002ABC55: vmovups   ymm0,YMMWORD PTR [rdx]
002ABC59: vmovups   YMMWORD PTR [rax],ymm0
002ABC5D: vmovups   ymm0,YMMWORD PTR [rdx+0x20]
002ABC62: vmovups   YMMWORD PTR [rax+0x20],ymm0
002ABC67: vmovups   xmm0,XMMWORD PTR [rdx+0x40]
002ABC6C: vmovups   XMMWORD PTR [rax+0x40],xmm0
002ABC71: lea       rdx,[rbp+0x30]
002ABC75: vzeroupper 
002ABC78: call      0x1802953c0
002ABC7D: mov       rcx,QWORD PTR [rip+0xbd515c]        # 0x180e80de0
002ABC84: cmp       BYTE PTR [rcx+0x4a5],r13b
002ABC8B: je        0x1802abcab
002ABC8D: mov       ecx,DWORD PTR [rcx+0x460]
002ABC93: call      0x1802577e0
002ABC98: mov       rcx,QWORD PTR [rip+0xbd5141]        # 0x180e80de0
002ABC9F: call      0x180256670
002ABCA4: mov       rcx,QWORD PTR [rip+0xbd5135]        # 0x180e80de0
002ABCAB: mov       eax,DWORD PTR [rcx+0x460]
002ABCB1: cmp       eax,DWORD PTR [rip+0x1b5429]        # 0x1804610e0
002ABCB7: mov       BYTE PTR [rip+0xbd52e1],r13b        # 0x180e80f9f
002ABCBE: jne       0x1802abcd1
002ABCC0: cmp       BYTE PTR [rip+0x1ca07d],r13b        # 0x180475d44
002ABCC7: je        0x1802abcd1
002ABCC9: cmp       DWORD PTR [rcx+0xfe8],eax
002ABCCF: jne       0x1802abd0e
002ABCD1: cmp       DWORD PTR [rcx+0x3dc],0x2
002ABCD8: je        0x1802abce3
002ABCDA: cmp       BYTE PTR [rcx+0x4a5],r13b
002ABCE1: je        0x1802abd0e
002ABCE3: cmp       QWORD PTR [rcx+0x1690],r13
002ABCEA: je        0x1802abcfe
002ABCEC: cmp       BYTE PTR [rip+0xbd539d],r13b        # 0x180e81090
002ABCF3: jne       0x1802abcfe
002ABCF5: mov       BYTE PTR [rip+0xbd52a3],0x1        # 0x180e80f9f
002ABCFC: jmp       0x1802abd0e
002ABCFE: lea       rdx,[rbp-0x50]
002ABD02: call      0x1802941a0
002ABD07: mov       rcx,QWORD PTR [rip+0xbd50d2]        # 0x180e80de0
002ABD0E: cmp       BYTE PTR [rcx+0x4e4],r13b
002ABD15: je        0x1802abdbc
002ABD1B: movzx     eax,BYTE PTR [rcx+0x4a5]
002ABD22: mov       BYTE PTR [rsp+0x70],al
002ABD26: movzx     eax,BYTE PTR [rcx+0x4bc]
002ABD2D: mov       BYTE PTR [rsp+0x71],al
002ABD31: mov       eax,DWORD PTR [rcx+0x3dc]
002ABD37: vmovss    DWORD PTR [rsp+0x74],xmm7
002ABD3D: cmp       eax,0x2
002ABD40: jne       0x1802abd6e
002ABD42: cmp       BYTE PTR [rcx+0x4f8],r13b
002ABD49: je        0x1802abd66
002ABD4B: cmp       BYTE PTR [rcx+0x48e],r13b
002ABD52: je        0x1802abd66
002ABD54: cmp       BYTE PTR [rcx],r13b
002ABD57: jne       0x1802abd62
002ABD59: cmp       BYTE PTR [rcx+0x343],r13b
002ABD60: je        0x1802abd66
002ABD62: mov       al,0x1
002ABD64: jmp       0x1802abd68
002ABD66: xor       al,al
002ABD68: test      al,al
002ABD6A: je        0x1802abd7b
002ABD6C: jmp       0x1802abd98
002ABD6E: test      eax,eax
002ABD70: je        0x1802abd7b
002ABD72: cmp       BYTE PTR [rcx+0x4a4],r13b
002ABD79: je        0x1802abd98
002ABD7B: vmovss    xmm1,DWORD PTR [rip+0x16011d]        # 0x18040bea0
002ABD83: vmovss    xmm0,DWORD PTR [rcx+0x4c4]
002ABD8B: vmovaps   xmm2,xmm7
002ABD8F: call      0x18029ecb0
002ABD94: vmovaps   xmm7,xmm0
002ABD98: vmovsd    xmm1,QWORD PTR [rsp+0x70]
002ABD9E: lea       rcx,[rsp+0x70]
002ABDA3: vmovsd    QWORD PTR [rsp+0x70],xmm1
002ABDA9: vmovss    DWORD PTR [rsp+0x78],xmm7
002ABDAF: call      QWORD PTR [rip+0x1c6dfb]        # 0x180472bb0 ; PDPerfPlugin.dll!SetPDFrameWarpOptions
002ABDB5: mov       rcx,QWORD PTR [rip+0xbd5024]        # 0x180e80de0
002ABDBC: vmovaps   xmm7,XMMWORD PTR [rsp+0x400]
002ABDC5: cmp       BYTE PTR [rcx+0x4e4],r13b
002ABDCC: je        0x1802abfa2
002ABDD2: cmp       BYTE PTR [rcx+0x4a5],r13b
002ABDD9: je        0x1802abdf1
002ABDDB: mov       eax,DWORD PTR [rip+0x1b370f]        # 0x18045f4f0
002ABDE1: and       eax,0xb8
002ABDE6: cmp       al,0xb8
002ABDE8: je        0x1802abe1e
002ABDEA: mov       rcx,QWORD PTR [rip+0xbd4fef]        # 0x180e80de0
002ABDF1: cmp       BYTE PTR [rcx+0x4e4],r13b
002ABDF8: je        0x1802abfa2
002ABDFE: cmp       DWORD PTR [rip+0x1c98fb],0x2        # 0x180475700
002ABE05: jne       0x1802abfa2
002ABE0B: mov       eax,DWORD PTR [rip+0x1b36df]        # 0x18045f4f0
002ABE11: and       eax,0xb8
002ABE16: cmp       al,0xb8
002ABE18: jne       0x1802abfa2
002ABE1E: cmp       DWORD PTR [rip+0x1c98db],0x2        # 0x180475700
002ABE25: jne       0x1802abe34
002ABE27: cmp       BYTE PTR [rip+0xbd51b3],r13b        # 0x180e80fe1
002ABE2E: je        0x1802abeb4
002ABE34: lea       rax,[rbp+0x30]
002ABE38: nop       DWORD PTR [rax+rax*1+0x0]
002ABE40: lea       rax,[rax+0x80]
002ABE47: vmovups   ymm0,YMMWORD PTR [rbx]
002ABE4B: vmovups   xmm1,XMMWORD PTR [rbx+0x70]
002ABE50: lea       rbx,[rbx+0x80]
002ABE57: vmovups   YMMWORD PTR [rax-0x80],ymm0
002ABE5C: vmovups   ymm0,YMMWORD PTR [rbx-0x60]
002ABE61: vmovups   YMMWORD PTR [rax-0x60],ymm0
002ABE66: vmovups   ymm0,YMMWORD PTR [rbx-0x40]
002ABE6B: vmovups   YMMWORD PTR [rax-0x40],ymm0
002ABE70: vmovups   xmm0,XMMWORD PTR [rbx-0x20]
002ABE75: vmovups   XMMWORD PTR [rax-0x20],xmm0
002ABE7A: vmovups   XMMWORD PTR [rax-0x10],xmm1
002ABE7F: sub       rsi,0x1
002ABE83: jne       0x1802abe40
002ABE85: vmovups   ymm0,YMMWORD PTR [rbx]
002ABE89: mov       rcx,QWORD PTR [rip+0xbd4f50]        # 0x180e80de0
002ABE90: lea       rdx,[rbp+0x30]
002ABE94: vmovups   YMMWORD PTR [rax],ymm0
002ABE98: vmovups   ymm0,YMMWORD PTR [rbx+0x20]
002ABE9D: vmovups   YMMWORD PTR [rax+0x20],ymm0
002ABEA2: vmovups   xmm0,XMMWORD PTR [rbx+0x40]
002ABEA7: vmovups   XMMWORD PTR [rax+0x40],xmm0
002ABEAC: vzeroupper 
002ABEAF: call      0x1802953c0
002ABEB4: mov       rcx,QWORD PTR [rip+0xbd4f25]        # 0x180e80de0
002ABEBB: mov       ecx,DWORD PTR [rcx+0x460]
002ABEC1: call      0x1802577e0
002ABEC6: mov       rax,QWORD PTR [rip+0xbd4f13]        # 0x180e80de0
002ABECD: cmp       BYTE PTR [rax+0x4e4],r13b
002ABED4: je        0x1802abf88
002ABEDA: test      r15b,r15b
002ABEDD: je        0x1802abf15
002ABEDF: mov       rdx,QWORD PTR [rax+0xa38]
002ABEE6: mov       rcx,QWORD PTR [rbp-0x50]
002ABEEA: cmp       rcx,rdx
002ABEED: je        0x1802abf15
002ABEEF: test      rcx,rcx
002ABEF2: je        0x1802abf15
002ABEF4: test      rdx,rdx
002ABEF7: je        0x1802abf15
002ABEF9: cmp       QWORD PTR [rax+0x1680],r13
002ABF00: je        0x1802abf15
002ABF02: lea       r8,[rbp-0x50]
002ABF06: mov       rcx,rax
002ABF09: call      0x180298770 ; '@USWAVH'
002ABF0E: mov       rax,QWORD PTR [rip+0xbd4ecb]        # 0x180e80de0
002ABF15: cmp       BYTE PTR [rax+0x2aa],r13b
002ABF1C: jne       0x1802abf55
002ABF1E: cmp       BYTE PTR [rax+0x4e4],r13b
002ABF25: je        0x1802abf55
002ABF27: cmp       BYTE PTR [rax+0x4a5],r13b
002ABF2E: je        0x1802abf55
002ABF30: mov       eax,DWORD PTR [rip+0x1b35ba]        # 0x18045f4f0
002ABF36: and       eax,0xb8
002ABF3B: cmp       al,0xb8
002ABF3D: jne       0x1802abf55
002ABF3F: mov       rcx,QWORD PTR [rip+0xbd4e9a]        # 0x180e80de0
002ABF46: lea       rdx,[rbp-0x50]
002ABF4A: xor       r9d,r9d
002ABF4D: mov       r8b,0x1
002ABF50: call      0x1802a2300
002ABF55: call      QWORD PTR [rip+0x1c6d3d]        # 0x180472c98 ; PDPerfPlugin.dll!GetPDFrameWarpRenderFrameCount
002ABF5B: cmp       DWORD PTR [rip+0x1c979e],0x2        # 0x180475700
002ABF62: lea       rdx,[rbp-0x50]
002ABF66: mov       QWORD PTR [rip+0xbd5063],rax        # 0x180e80fd0
002ABF6D: jne       0x1802abf7a
002ABF6F: movzx     r8d,r15b
002ABF73: call      0x180256370
002ABF78: jmp       0x1802abfa2
002ABF7A: mov       rcx,QWORD PTR [rip+0xbd4e5f]        # 0x180e80de0
002ABF81: call      0x1802a4d80
002ABF86: jmp       0x1802abfa2
002ABF88: mov       BYTE PTR [rip+0xbd5051],r13b        # 0x180e80fe0
002ABF8F: call      0x180284420
002ABF94: mov       QWORD PTR [rip+0xbd502d],r13        # 0x180e80fc8
002ABF9B: mov       QWORD PTR [rip+0xbd5016],r13        # 0x180e80fb8
002ABFA2: mov       BYTE PTR [rip+0xbd50e8],0x1        # 0x180e81091
002ABFA9: add       rsp,0x440
002ABFB0: pop       r15
002ABFB2: pop       r13
002ABFB4: pop       rsi
002ABFB5: pop       rbx
002ABFB6: pop       rbp
002ABFB7: ret       
002ABFB8: lea       rcx,[rip+0xbd5961]        # 0x180e81920
002ABFBF: call      0x1802391bc
002ABFC4: cmp       DWORD PTR [rip+0xbd5955],0xffffffff        # 0x180e81920
002ABFCB: jne       0x1802ab742
002ABFD1: xor       edx,edx
002ABFD3: xor       ecx,ecx
002ABFD5: mov       r9d,0x1f0003
002ABFDB: mov       r8d,0x2
002ABFE1: call      QWORD PTR [rip+0x17259]        # 0x1802c3240
002ABFE7: lea       rcx,[rip+0x16072]        # 0x1802c2060
002ABFEE: mov       QWORD PTR [rip+0xbd5933],rax        # 0x180e81928
002ABFF5: call      0x1802396d0
002ABFFA: lea       rcx,[rip+0xbd591f]        # 0x180e81920
002AC001: call      0x180239150
002AC006: jmp       0x1802ab742
002AC00B: lea       rcx,[rip+0xbd58fe]        # 0x180e81910
002AC012: call      0x1802391bc
002AC017: cmp       DWORD PTR [rip+0xbd58f2],0xffffffff        # 0x180e81910
002AC01E: jne       0x1802ab662
002AC024: lea       rcx,[rip+0xbd58ed]        # 0x180e81918
002AC02B: call      0x180179bf0
002AC030: lea       rcx,[rip+0xbd58d9]        # 0x180e81910
002AC037: call      0x180239150
002AC03C: jmp       0x1802ab662
