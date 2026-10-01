; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xEFC80..0xEFF5C; unnamed
000EFC80: rex       push rbp
000EFC82: push      rbx
000EFC83: push      rsi
000EFC84: push      rdi
000EFC85: push      r12
000EFC87: push      r13
000EFC89: push      r14
000EFC8B: push      r15
000EFC8D: lea       rbp,[rsp-0x28]
000EFC92: sub       rsp,0x128
000EFC99: mov       rax,QWORD PTR [rip+0x10dad20]        # 0x1811ca9c0
000EFCA0: xor       rax,rsp
000EFCA3: mov       QWORD PTR [rbp+0x10],rax
000EFCA7: mov       r14,QWORD PTR [rbp+0x90]
000EFCAE: mov       rsi,r9
000EFCB1: mov       rdi,QWORD PTR [rbp+0x98]
000EFCB8: mov       r12,r8
000EFCBB: mov       r13,QWORD PTR [rbp+0xa8]
000EFCC2: mov       r15,rdx
000EFCC5: mov       rbx,rcx
000EFCC8: mov       BYTE PTR [rcx+0x1c8],0x0
000EFCCF: call      0x1800cc790
000EFCD4: mov       r8d,DWORD PTR [rbx+0x8]
000EFCD8: mov       rcx,rax
000EFCDB: mov       edx,DWORD PTR [rbx+0x148]
000EFCE1: call      0x1800ccc80
000EFCE6: lea       rcx,[rip+0x10b9ffb]        # 0x1811a9ce8 ; 'Create FSR3 Frame Interpolation SwapChain'
000EFCED: call      0x1800fbb40
000EFCF2: movups    xmm0,XMMWORD PTR [r14]
000EFCF6: xor       ecx,ecx
000EFCF8: mov       QWORD PTR [rsp+0x50],0x30006
000EFD01: movups    xmm1,XMMWORD PTR [r14+0x10]
000EFD06: mov       QWORD PTR [rsp+0x20],rcx
000EFD0B: movups    XMMWORD PTR [rbp-0x58],xmm0
000EFD0F: mov       QWORD PTR [rsp+0x58],rcx
000EFD14: movups    xmm0,XMMWORD PTR [r14+0x20]
000EFD19: movups    XMMWORD PTR [rbp-0x48],xmm1
000EFD1D: movups    XMMWORD PTR [rbp-0x38],xmm0
000EFD21: psrldq    xmm0,0xc
000EFD26: movd      eax,xmm0
000EFD2A: bt        eax,0xd
000EFD2E: jae       0x1800efd38
000EFD30: add       eax,0xffffe000
000EFD35: mov       DWORD PTR [rbp-0x2c],eax
000EFD38: bt        eax,0xe
000EFD3C: jae       0x1800efd46
000EFD3E: add       eax,0xffffc000
000EFD43: mov       DWORD PTR [rbp-0x2c],eax
000EFD46: mov       QWORD PTR [rsp+0x68],rsi
000EFD4B: lea       rax,[rbp-0x58]
000EFD4F: mov       QWORD PTR [rsp+0x70],rax
000EFD54: mov       QWORD PTR [rbp-0x70],rcx
000EFD58: mov       QWORD PTR [rbp-0x68],rcx
000EFD5C: mov       DWORD PTR [rbp-0x60],0x1
000EFD63: test      rdi,rdi
000EFD66: je        0x1800efd75
000EFD68: movups    xmm0,XMMWORD PTR [rdi]
000EFD6B: mov       eax,DWORD PTR [rdi+0x10]
000EFD6E: mov       DWORD PTR [rbp-0x60],eax
000EFD71: movups    XMMWORD PTR [rbp-0x70],xmm0
000EFD75: lea       rax,[rbp-0x70]
000EFD79: mov       QWORD PTR [rsp+0x38],rcx
000EFD7E: mov       QWORD PTR [rsp+0x78],rax
000EFD83: mov       r8b,0x1
000EFD86: lea       rax,[rsp+0x20]
000EFD8B: mov       QWORD PTR [rbp-0x80],r15
000EFD8F: mov       QWORD PTR [rsp+0x60],rax
000EFD94: mov       rdx,rsi
000EFD97: mov       eax,DWORD PTR [r14]
000EFD9A: mov       rcx,r15
000EFD9D: mov       DWORD PTR [rbx+0x134],eax
000EFDA3: mov       eax,DWORD PTR [r14+0x4]
000EFDA7: mov       DWORD PTR [rbx+0x138],eax
000EFDAD: mov       QWORD PTR [rbp-0x78],r12
000EFDB1: mov       QWORD PTR [rsp+0x40],0xc01006
000EFDBA: mov       QWORD PTR [rsp+0x30],0x3000b
000EFDC3: call      0x1800b97d0
000EFDC8: test      al,al
000EFDCA: je        0x1800efe26
000EFDCC: lea       rdx,[rsp+0x30]
000EFDD1: lea       rcx,[rsp+0x50]
000EFDD6: call      0x1800f0b70
000EFDDB: mov       rdi,QWORD PTR [rip+0x1126616]        # 0x1812163f8
000EFDE2: mov       r12,rax
000EFDE5: cmp       BYTE PTR [rdi+0xca],0x0
000EFDEC: jne       0x1800efdf6
000EFDEE: mov       rcx,rdi
000EFDF1: call      0x1800cc8c0
000EFDF6: mov       r9,QWORD PTR [rdi+0x8]
000EFDFA: lea       rcx,[rip+0x112fbbf]        # 0x18121f9c0
000EFE01: xor       r8d,r8d
000EFE04: mov       rdx,r12
000EFE07: call      r9
000EFE0A: mov       r8d,eax
000EFE0D: call      0x1800b9880
000EFE12: test      r8d,r8d
000EFE15: je        0x1800efe30
000EFE17: mov       edx,r8d
000EFE1A: lea       rcx,[rip+0x10b9e8f]        # 0x1811a9cb0 ; "Couldn't create the FFXAPI FG SwapChain (dx12): %d"
000EFE21: call      0x1800fbb40
000EFE26: mov       eax,0x80004005
000EFE2B: jmp       0x1800eff3c
000EFE30: lea       rcx,[rsp+0x28]
000EFE35: call      0x180008420
000EFE3A: mov       rcx,QWORD PTR [rsp+0x20]
000EFE3F: lea       r8,[rip+0x10af32a]        # 0x18119f170
000EFE46: mov       r9,rax
000EFE49: xor       edx,edx
000EFE4B: mov       rax,QWORD PTR [rcx]
000EFE4E: call      QWORD PTR [rax+0x48]
000EFE51: mov       rcx,QWORD PTR [rsp+0x28]
000EFE56: lea       rdx,[rbp-0x28]
000EFE5A: mov       rax,QWORD PTR [rcx]
000EFE5D: call      QWORD PTR [rax+0x50]
000EFE60: mov       ecx,DWORD PTR [rax+0x20]
000EFE63: mov       DWORD PTR [rbx+0x110],ecx
000EFE69: mov       rcx,QWORD PTR [rsp+0x28]
000EFE6E: mov       rax,QWORD PTR [rcx]
000EFE71: call      QWORD PTR [rax+0x10]
000EFE74: mov       rax,QWORD PTR [rsp+0x20]
000EFE79: mov       QWORD PTR [r13+0x0],rax
000EFE7D: mov       QWORD PTR [rbx+0x1a8],rax
000EFE84: call      0x1800badc0
000EFE89: test      al,al
000EFE8B: je        0x1800efead
000EFE8D: mov       rcx,QWORD PTR [rsp+0x20]
000EFE92: mov       rdx,rsi
000EFE95: call      0x1800badb0
000EFE9A: mov       r8,r14
000EFE9D: mov       rcx,r15
000EFEA0: call      0x1800bb060
000EFEA5: test      eax,eax
000EFEA7: js        0x1800eff3c
000EFEAD: lea       rcx,[rip+0x10b9e94]        # 0x1811a9d48 ; 'Create FSR3 Frame Interpolation SwapChain success'
000EFEB4: call      0x1800fbb40
000EFEB9: mov       rax,QWORD PTR [rbx]
000EFEBC: mov       rcx,rbx
000EFEBF: call      QWORD PTR [rax+0x50]
000EFEC2: test      al,al
000EFEC4: je        0x1800eff3a
000EFEC6: mov       rax,QWORD PTR [rsp+0x20]
000EFECB: lea       rcx,[rip+0x1109efe]        # 0x1811f9dd0
000EFED2: xorps     xmm0,xmm0
000EFED5: mov       QWORD PTR [rip+0x1109f04],rax        # 0x1811f9de0
000EFEDC: movaps    XMMWORD PTR [rip+0x1109f2d],xmm0        # 0x1811f9e10
000EFEE3: movaps    XMMWORD PTR [rip+0x1109f36],xmm0        # 0x1811f9e20
000EFEEA: movaps    XMMWORD PTR [rip+0x1109f3f],xmm0        # 0x1811f9e30
000EFEF1: call      0x1800f0b60
000EFEF6: mov       rbx,QWORD PTR [rip+0x11264fb]        # 0x1812163f8
000EFEFD: mov       rdi,rax
000EFF00: cmp       BYTE PTR [rbx+0xca],0x0
000EFF07: jne       0x1800eff11
000EFF09: mov       rcx,rbx
000EFF0C: call      0x1800cc8c0
000EFF11: mov       r8,QWORD PTR [rbx]
000EFF14: lea       rcx,[rip+0x112faad]        # 0x18121f9c8
000EFF1B: mov       rdx,rdi
000EFF1E: call      r8
000EFF21: test      eax,eax
000EFF23: je        0x1800eff3a
000EFF25: mov       edx,eax
000EFF27: lea       rcx,[rip+0x10b9dea]        # 0x1811a9d18 ; "Couldn't set the FFXAPI FrameGen config: %d"
000EFF2E: call      0x1800fbb40
000EFF33: mov       eax,0x80004005
000EFF38: jmp       0x1800eff3c
000EFF3A: xor       eax,eax
000EFF3C: mov       rcx,QWORD PTR [rbp+0x10]
000EFF40: xor       rcx,rsp
000EFF43: call      0x18010c270
000EFF48: add       rsp,0x128
000EFF4F: pop       r15
000EFF51: pop       r14
000EFF53: pop       r13
000EFF55: pop       r12
000EFF57: pop       rdi
000EFF58: pop       rsi
000EFF59: pop       rbx
000EFF5A: pop       rbp
000EFF5B: ret       
