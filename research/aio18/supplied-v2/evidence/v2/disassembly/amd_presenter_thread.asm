; amd_fidelityfx_framegeneration_dx12.dll SHA256=3f5e674a59b400756e98ef31fd583a4eb08ad567ec8dee362886bc96e55ed347
; ImageBase=0x180000000, RVAs in left column
; range 0xCBD0..0xCF5F; presenterThread<FrameinterpolationPresentInfoExt,FrameInterpolationPacingDataExt,ffxCallbackDescFrameGenerationPresent>
0000CBD0: mov       r11,rsp
0000CBD3: push      rbp
0000CBD4: push      rbx
0000CBD5: lea       rbp,[r11-0xb8]
0000CBDC: sub       rsp,0x1a8
0000CBE3: mov       rax,QWORD PTR [rip+0x2461416]        # 0x18246e000
0000CBEA: xor       rax,rsp
0000CBED: mov       QWORD PTR [rbp+0x88],rax
0000CBF4: mov       rbx,rcx
0000CBF7: test      rcx,rcx
0000CBFA: je        0x18000cf44
0000CC00: mov       QWORD PTR [r11+0x18],rdi
0000CC04: lea       rcx,[rbp+0x78]
0000CC08: mov       QWORD PTR [r11-0x18],r13
0000CC0C: xor       r13d,r13d
0000CC0F: mov       edi,r13d
0000CC12: mov       QWORD PTR [r11-0x28],r15
0000CC16: call      QWORD PTR [rip+0xf94b4]        # 0x1801060d0 ; KERNEL32.dll!QueryPerformanceFrequency
0000CC1C: mov       r15,QWORD PTR [rbp+0x78]
0000CC20: mov       DWORD PTR [rbp+0x68],r13d
0000CC24: mov       QWORD PTR [rbx+0x1750],r13
0000CC2B: movzx     eax,BYTE PTR [rbx+0x1721]
0000CC32: mov       QWORD PTR [rsp+0x30],r15
0000CC37: test      al,al
0000CC39: jne       0x18000cf15
0000CC3F: mov       QWORD PTR [rsp+0x1c8],rsi
0000CC47: mov       QWORD PTR [rsp+0x1d8],r12
0000CC4F: mov       r12d,0x1
0000CC55: mov       QWORD PTR [rsp+0x198],r14
0000CC5D: nop       DWORD PTR [rax]
0000CC60: mov       rcx,QWORD PTR [rbx+0x1718]
0000CC67: mov       edx,0xffffffff
0000CC6C: call      QWORD PTR [rip+0xf9476]        # 0x1801060e8 ; KERNEL32.dll!WaitForSingleObject
0000CC72: movzx     eax,BYTE PTR [rbx+0x1721]
0000CC79: test      al,al
0000CC7B: jne       0x18000ceee
0000CC81: mov       rcx,rbx
0000CC84: call      QWORD PTR [rip+0xf9486]        # 0x180106110 ; KERNEL32.dll!EnterCriticalSection
0000CC8A: lea       rcx,[rbx+0x1570]
0000CC91: mov       edx,0x2
0000CC96: lea       rax,[rsp+0x40]
0000CC9B: nop       DWORD PTR [rax+rax*1+0x0]
0000CCA0: lea       rax,[rax+0x80]
0000CCA7: movups    xmm0,XMMWORD PTR [rcx]
0000CCAA: movups    xmm1,XMMWORD PTR [rcx+0x10]
0000CCAE: lea       rcx,[rcx+0x80]
0000CCB5: movups    XMMWORD PTR [rax-0x80],xmm0
0000CCB9: movups    xmm0,XMMWORD PTR [rcx-0x60]
0000CCBD: movups    XMMWORD PTR [rax-0x70],xmm1
0000CCC1: movups    xmm1,XMMWORD PTR [rcx-0x50]
0000CCC5: movups    XMMWORD PTR [rax-0x60],xmm0
0000CCC9: movups    xmm0,XMMWORD PTR [rcx-0x40]
0000CCCD: movups    XMMWORD PTR [rax-0x50],xmm1
0000CCD1: movups    xmm1,XMMWORD PTR [rcx-0x30]
0000CCD5: movups    XMMWORD PTR [rax-0x40],xmm0
0000CCD9: movups    xmm0,XMMWORD PTR [rcx-0x20]
0000CCDD: movups    XMMWORD PTR [rax-0x30],xmm1
0000CCE1: movups    xmm1,XMMWORD PTR [rcx-0x10]
0000CCE5: movups    XMMWORD PTR [rax-0x20],xmm0
0000CCE9: movups    XMMWORD PTR [rax-0x10],xmm1
0000CCED: sub       rdx,0x1
0000CCF1: jne       0x18000cca0
0000CCF3: movups    xmm0,XMMWORD PTR [rcx]
0000CCF6: mov       r8d,0x110
0000CCFC: lea       rcx,[rbx+0x1570]
0000CD03: movups    XMMWORD PTR [rax],xmm0
0000CD06: call      0x1801056da
0000CD0B: mov       rcx,rbx
0000CD0E: call      QWORD PTR [rip+0xf93e4]        # 0x1801060f8 ; KERNEL32.dll!LeaveCriticalSection
0000CD14: cmp       DWORD PTR [rbp-0x60],0x0
0000CD18: jbe       0x18000ceee
0000CD1E: mov       rcx,QWORD PTR [rbx+0x16d0]
0000CD25: mov       r8,QWORD PTR [rbp-0x68]
0000CD29: mov       rdx,QWORD PTR [rbx+0x16e8]
0000CD30: mov       rax,QWORD PTR [rcx]
0000CD33: call      QWORD PTR [rax+0x70]
0000CD36: mov       rcx,QWORD PTR [rbx+0x16d0]
0000CD3D: mov       r8,QWORD PTR [rbp-0x78]
0000CD41: mov       rdx,QWORD PTR [rbx+0x16e0]
0000CD48: mov       rax,QWORD PTR [rcx]
0000CD4B: call      QWORD PTR [rax+0x78]
0000CD4E: mov       edi,r13d
0000CD51: lea       rsi,[rbp-0x8]
0000CD55: cmp       BYTE PTR [rsi-0x48],0x0
0000CD59: je        0x18000ced8
0000CD5F: mov       r8d,edi
0000CD62: lea       rdx,[rsp+0x40]
0000CD67: mov       rcx,rbx
0000CD6A: call      0x18000e4c0 ; compositeSwapChainFrame<FrameinterpolationPresentInfoExt,FrameInterpolationPacingDataExt,ffxCallbackDescFrameGenerationPresent>
0000CD6F: mov       r8,QWORD PTR [rbp-0x70]
0000CD73: mov       r14,QWORD PTR [rsi-0x8]
0000CD77: cmp       r14,r8
0000CD7A: jne       0x18000cd90
0000CD7C: mov       rcx,QWORD PTR [rbx+0x16d0]
0000CD83: mov       rdx,QWORD PTR [rbx+0x16f0]
0000CD8A: mov       rax,QWORD PTR [rcx]
0000CD8D: call      QWORD PTR [rax+0x70]
0000CD90: mov       edx,0x8
0000CD95: lea       rcx,[rbp+0x68]
0000CD99: call      QWORD PTR [rip+0xf9481]        # 0x180106220 ; WINMM.dll!timeGetDevCaps
0000CD9F: test      eax,eax
0000CDA1: jne       0x18000cdbd
0000CDA3: movzx     eax,BYTE PTR [rbx+0x1738]
0000CDAA: test      al,al
0000CDAC: je        0x18000cdbd
0000CDAE: mov       eax,DWORD PTR [rbp+0x68]
0000CDB1: cmp       eax,0x1
0000CDB4: cmovb     eax,r12d
0000CDB8: mov       DWORD PTR [rbp+0x68],eax
0000CDBB: jmp       0x18000cdc1
0000CDBD: mov       DWORD PTR [rbp+0x68],r13d
0000CDC1: mov       rcx,QWORD PTR [rbx+0x1700]
0000CDC8: xor       r9d,r9d
0000CDCB: mov       rdx,r14
0000CDCE: mov       BYTE PTR [rsp+0x20],0x0
0000CDD3: call      0x1800f4c30
0000CDD8: mov       rcx,QWORD PTR [rbx+0x1750]
0000CDDF: mov       rdx,r15
0000CDE2: mov       r9d,DWORD PTR [rbx+0x173c]
0000CDE9: add       rcx,QWORD PTR [rsi]
0000CDEC: mov       r8d,DWORD PTR [rbp+0x68]
0000CDF0: call      0x1800f4b20 ; '@SUVWH'
0000CDF5: lea       rcx,[rbp+0x80]
0000CDFC: call      QWORD PTR [rip+0xf92ae]        # 0x1801060b0 ; KERNEL32.dll!QueryPerformanceCounter
0000CE02: mov       rax,QWORD PTR [rbp+0x80]
0000CE09: lea       rdx,[rbp+0x60]
0000CE0D: mov       rcx,QWORD PTR [rbx+0x30]
0000CE11: movzx     r15d,BYTE PTR [rbp-0x80]
0000CE16: mov       QWORD PTR [rbx+0x1750],rax
0000CE1D: movsxd    rax,edi
0000CE20: mov       DWORD PTR [rbp+0x60],0x0
0000CE27: lea       r12,[rax+rax*4]
0000CE2B: mov       rax,QWORD PTR [rcx]
0000CE2E: add       r12,r12
0000CE31: test      r15d,r15d
0000CE34: setne     r13b
0000CE38: xor       r8d,r8d
0000CE3B: xor       r14b,r14b
0000CE3E: call      QWORD PTR [rax+0x58]
0000CE41: test      eax,eax
0000CE43: js        0x18000ce4d
0000CE45: cmp       DWORD PTR [rbp+0x60],0x1
0000CE49: sete      r14b
0000CE4D: cmp       BYTE PTR [rbp-0x7f],0x0
0000CE51: je        0x18000ce65
0000CE53: test      r14b,r14b
0000CE56: jne       0x18000ce65
0000CE58: test      r15d,r15d
0000CE5B: jne       0x18000ce65
0000CE5D: mov       r14d,0x200
0000CE63: jmp       0x18000ce68
0000CE65: xor       r14d,r14d
0000CE68: mov       rcx,QWORD PTR [rbx+0x30]
0000CE6C: lea       r9,[rbp+0x50]
0000CE70: mov       DWORD PTR [rbp+0x70],0x10
0000CE77: lea       r8,[rbp+0x70]
0000CE7B: lea       rdx,[rip+0xfa04e]        # 0x180106ed0
0000CE82: mov       rax,QWORD PTR [rcx]
0000CE85: call      QWORD PTR [rax+0x28]
0000CE88: test      eax,eax
0000CE8A: js        0x18000cea1
0000CE8C: cmp       BYTE PTR [rbp+0x58],0x0
0000CE90: je        0x18000cea1
0000CE92: mov       rcx,QWORD PTR [rbp+0x50]
0000CE96: cmp       edi,0x1
0000CE99: setne     dl
0000CE9C: call      0x1800075c0
0000CEA1: mov       rcx,QWORD PTR [rbx+0x30]
0000CEA5: mov       r8d,r14d
0000CEA8: mov       edx,r13d
0000CEAB: mov       rax,QWORD PTR [rcx]
0000CEAE: call      QWORD PTR [rax+0x40]
0000CEB1: mov       rcx,QWORD PTR [rbx+0x16d0]
0000CEB8: mov       r8,QWORD PTR [rbp+r12*8-0x10]
0000CEBD: mov       rdx,QWORD PTR [rbx+0x16e8]
0000CEC4: mov       rax,QWORD PTR [rcx]
0000CEC7: call      QWORD PTR [rax+0x70]
0000CECA: mov       r15,QWORD PTR [rsp+0x30]
0000CECF: xor       r13d,r13d
0000CED2: mov       r12d,0x1
0000CED8: inc       edi
0000CEDA: add       rsi,0x50
0000CEDE: cmp       edi,0x2
0000CEE1: jb        0x18000cd55
0000CEE7: mov       edi,DWORD PTR [rbp-0x60]
0000CEEA: add       rdi,QWORD PTR [rbp-0x68]
0000CEEE: movzx     eax,BYTE PTR [rbx+0x1721]
0000CEF5: test      al,al
0000CEF7: je        0x18000cc60
0000CEFD: mov       r14,QWORD PTR [rsp+0x198]
0000CF05: mov       r12,QWORD PTR [rsp+0x1d8]
0000CF0D: mov       rsi,QWORD PTR [rsp+0x1c8]
0000CF15: mov       rcx,QWORD PTR [rbx+0x16e8]
0000CF1C: xor       r9d,r9d
0000CF1F: mov       rdx,rdi
0000CF22: mov       BYTE PTR [rsp+0x20],0x0
0000CF27: call      0x1800f4c30
0000CF2C: mov       r15,QWORD PTR [rsp+0x190]
0000CF34: mov       r13,QWORD PTR [rsp+0x1a0]
0000CF3C: mov       rdi,QWORD PTR [rsp+0x1d0]
0000CF44: xor       eax,eax
0000CF46: mov       rcx,QWORD PTR [rbp+0x88]
0000CF4D: xor       rcx,rsp
0000CF50: call      0x180104650
0000CF55: add       rsp,0x1a8
0000CF5C: pop       rbx
0000CF5D: pop       rbp
0000CF5E: ret       
