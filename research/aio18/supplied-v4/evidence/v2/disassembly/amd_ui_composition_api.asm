; amd_fidelityfx_framegeneration_dx12.dll SHA256=3f5e674a59b400756e98ef31fd583a4eb08ad567ec8dee362886bc96e55ed347
; ImageBase=0x180000000, RVAs in left column
; range 0xF5600..0xF5D68; ffxFrameInterpolationUiComposition
000F5600: rex       push rbp
000F5602: push      rbx
000F5603: push      rsi
000F5604: push      r14
000F5606: push      r15
000F5608: lea       rbp,[rsp-0x130]
000F5610: sub       rsp,0x230
000F5617: mov       rax,QWORD PTR [rip+0x23789e2]        # 0x18246e000
000F561E: xor       rax,rsp
000F5621: mov       QWORD PTR [rbp+0x120],rax
000F5628: mov       r15,QWORD PTR [rcx+0x80]
000F562F: lea       rdx,[rbp-0x30]
000F5633: mov       rsi,QWORD PTR [rcx+0x10]
000F5637: mov       r14,rcx
000F563A: mov       rcx,r15
000F563D: mov       rax,QWORD PTR [r15]
000F5640: call      QWORD PTR [rax+0x50]
000F5643: cmp       QWORD PTR [rip+0x237cbad],0x0        # 0x1824721f8
000F564B: je        0x1800f5657
000F564D: cmp       QWORD PTR [rip+0x237cbd3],0x0        # 0x182472228
000F5655: jne       0x1800f566a
000F5657: mov       edx,DWORD PTR [rbp-0x10]
000F565A: mov       rcx,rsi
000F565D: call      0x1800f5100 ; '@UVATAVAWH'
000F5662: test      eax,eax
000F5664: jne       0x1800f5ca9
000F566A: xor       ebx,ebx
000F566C: cmp       QWORD PTR [rip+0x237cb8d],rbx        # 0x182472200
000F5673: jne       0x1800f56b0
000F5675: mov       rax,QWORD PTR [rsi]
000F5678: lea       r9,[rip+0x237cb81]        # 0x182472200
000F567F: lea       r8,[rip+0x1116a]        # 0x1801067f0
000F5686: mov       DWORD PTR [rsp+0x4c],0x40
000F568E: lea       rdx,[rsp+0x48]
000F5693: mov       DWORD PTR [rsp+0x48],ebx
000F5697: mov       rcx,rsi
000F569A: mov       QWORD PTR [rsp+0x50],0x1
000F56A3: mov       DWORD PTR [rip+0x237cb77],ebx        # 0x182472220
000F56A9: call      QWORD PTR [rax+0x70]
000F56AC: test      eax,eax
000F56AE: js        0x1800f56ee
000F56B0: cmp       QWORD PTR [rip+0x237cb59],rbx        # 0x182472210
000F56B7: jne       0x1800f56f8
000F56B9: mov       rax,QWORD PTR [rsi]
000F56BC: lea       r9,[rip+0x237cb4d]        # 0x182472210
000F56C3: lea       r8,[rip+0x11126]        # 0x1801067f0
000F56CA: mov       DWORD PTR [rsp+0x4c],0x20
000F56D2: lea       rdx,[rsp+0x48]
000F56D7: mov       DWORD PTR [rsp+0x48],0x2
000F56DF: mov       rcx,rsi
000F56E2: mov       QWORD PTR [rsp+0x50],rbx
000F56E7: call      QWORD PTR [rax+0x70]
000F56EA: test      eax,eax
000F56EC: jns       0x1800f56f8
000F56EE: mov       eax,0x8000000d
000F56F3: jmp       0x1800f5ca9
000F56F8: mov       QWORD PTR [rsp+0x268],rdi
000F5700: mov       rax,r14
000F5703: mov       rdi,QWORD PTR [r14+0x18]
000F5707: mov       QWORD PTR [rsp+0x270],r12
000F570F: mov       QWORD PTR [rsp+0x278],r13
000F5717: mov       r13,QWORD PTR [rip+0x237cada]        # 0x1824721f8
000F571E: xchg      ax,ax
000F5720: cmp       QWORD PTR [rax],0x2000d
000F5727: je        0x1800f5734
000F5729: mov       rax,QWORD PTR [rax+0x8]
000F572D: test      rax,rax
000F5730: jne       0x1800f5720
000F5732: jmp       0x1800f5746
000F5734: cmp       BYTE PTR [rax+0x10],bl
000F5737: mov       r13,QWORD PTR [rip+0x237caba]        # 0x1824721f8
000F573E: cmovne    r13,QWORD PTR [rip+0x237cae2]        # 0x182472228
000F5746: mov       r12,QWORD PTR [r14+0x20]
000F574A: mov       r10,QWORD PTR [r14+0x50]
000F574E: mov       ecx,DWORD PTR [r14+0x48]
000F5752: mov       QWORD PTR [rsp+0x30],r12
000F5757: mov       QWORD PTR [rsp+0x48],r10
000F575C: call      0x180001aa0
000F5761: mov       r8d,eax
000F5764: test      r10,r10
000F5767: jne       0x1800f5832
000F576D: mov       ecx,DWORD PTR [r14+0xa8]
000F5774: mov       QWORD PTR [rbp+0x80],rbx
000F577B: mov       DWORD PTR [rbp+0x90],ebx
000F5781: mov       QWORD PTR [rbp+0x98],0x800
000F578C: mov       QWORD PTR [rbp+0xa0],rbx
000F5793: mov       DWORD PTR [rbp+0xb0],ebx
000F5799: mov       QWORD PTR [rbp+0xb8],0x400
000F57A4: mov       QWORD PTR [rbp+0x88],r12
000F57AB: mov       DWORD PTR [rbp+0x94],eax
000F57B1: mov       QWORD PTR [rbp+0xa8],r15
000F57B8: call      0x180001aa0
000F57BD: mov       DWORD PTR [rbp+0xb4],eax
000F57C3: lea       r8,[rbp+0x80]
000F57CA: mov       rax,QWORD PTR [rdi]
000F57CD: mov       edx,0x2
000F57D2: mov       rcx,rdi
000F57D5: call      QWORD PTR [rax+0xd0]
000F57DB: mov       rax,QWORD PTR [rdi]
000F57DE: mov       r8,r12
000F57E1: mov       rdx,r15
000F57E4: mov       rcx,rdi
000F57E7: call      QWORD PTR [rax+0x88]
000F57ED: mov       r8d,DWORD PTR [rbp+0x94]
000F57F4: mov       edx,0x2
000F57F9: mov       eax,DWORD PTR [rbp+0x98]
000F57FF: mov       DWORD PTR [rbp+0x98],r8d
000F5806: mov       r8d,DWORD PTR [rbp+0xb4]
000F580D: mov       DWORD PTR [rbp+0x94],eax
000F5813: mov       eax,DWORD PTR [rbp+0xb8]
000F5819: mov       DWORD PTR [rbp+0xb8],r8d
000F5820: lea       r8,[rbp+0x80]
000F5827: mov       DWORD PTR [rbp+0xb4],eax
000F582D: jmp       0x1800f5c83
000F5832: mov       ecx,DWORD PTR [r14+0x78]
000F5836: xorps     xmm0,xmm0
000F5839: mov       eax,ebx
000F583B: cmp       r8d,0x80
000F5842: movups    XMMWORD PTR [rbp+0xd0],xmm0
000F5849: setne     al
000F584C: mov       DWORD PTR [rbp+0xd4],r8d
000F5853: movups    XMMWORD PTR [rbp+0xc0],xmm0
000F585A: mov       r8d,eax
000F585D: shl       r8,0x5
000F5861: movups    XMMWORD PTR [rbp+0xe0],xmm0
000F5868: mov       QWORD PTR [rbp+0xc8],r12
000F586F: movups    XMMWORD PTR [rbp+0xf0],xmm0
000F5876: mov       DWORD PTR [rbp+0xd8],0x80
000F5880: movups    XMMWORD PTR [rbp+0x100],xmm0
000F5887: mov       r9d,eax
000F588A: movups    XMMWORD PTR [rbp+0x110],xmm0
000F5891: mov       DWORD PTR [rbp+r8*1+0xc0],ebx
000F5899: mov       QWORD PTR [rbp+r8*1+0xc8],r10
000F58A1: call      0x180001aa0
000F58A6: mov       DWORD PTR [rbp+r8*1+0xd4],eax
000F58AE: lea       ecx,[r9+0x1]
000F58B2: mov       DWORD PTR [rbp+r8*1+0xd8],0x80
000F58BE: cmp       eax,0x80
000F58C3: lea       r8,[rbp+0xc0]
000F58CA: cmove     ecx,r9d
000F58CE: mov       eax,ecx
000F58D0: shl       rax,0x5
000F58D4: lea       r12d,[rcx+0x1]
000F58D8: mov       rcx,rdi
000F58DB: mov       edx,r12d
000F58DE: mov       DWORD PTR [rbp+rax*1+0xc0],ebx
000F58E5: mov       QWORD PTR [rbp+rax*1+0xc8],r15
000F58ED: mov       DWORD PTR [rbp+rax*1+0xd4],ebx
000F58F4: mov       DWORD PTR [rbp+rax*1+0xd8],0x4
000F58FF: mov       rax,QWORD PTR [rdi]
000F5902: call      QWORD PTR [rax+0xd0]
000F5908: mov       rax,QWORD PTR [rdi]
000F590B: mov       rcx,rdi
000F590E: mov       rdx,QWORD PTR [rip+0x237c903]        # 0x182472218
000F5915: call      QWORD PTR [rax+0xf0]
000F591B: mov       rax,QWORD PTR [rip+0x237c8de]        # 0x182472200
000F5922: lea       r8,[rsp+0x38]
000F5927: mov       QWORD PTR [rsp+0x38],rax
000F592C: mov       edx,0x1
000F5931: mov       rax,QWORD PTR [rdi]
000F5934: mov       rcx,rdi
000F5937: call      QWORD PTR [rax+0xe0]
000F593D: mov       eax,DWORD PTR [rbp-0x10]
000F5940: lea       rdx,[rsp+0x58]
000F5945: mov       rcx,QWORD PTR [rsp+0x38]
000F594A: mov       QWORD PTR [rbp-0x60],rbx
000F594E: mov       DWORD PTR [rbp-0x70],eax
000F5951: mov       QWORD PTR [rbp-0x6c],0x4
000F5959: mov       DWORD PTR [rbp-0x64],ebx
000F595C: mov       rax,QWORD PTR [rcx]
000F595F: call      QWORD PTR [rax+0x50]
000F5962: mov       rax,QWORD PTR [rsi]
000F5965: xor       edx,edx
000F5967: mov       rcx,rsi
000F596A: call      QWORD PTR [rax+0x78]
000F596D: mov       rcx,QWORD PTR [rsp+0x38]
000F5972: lea       rdx,[rsp+0x40]
000F5977: imul      eax,DWORD PTR [rip+0x237c8a2]        # 0x182472220
000F597E: add       QWORD PTR [rsp+0x58],rax
000F5983: mov       rax,QWORD PTR [rcx]
000F5986: call      QWORD PTR [rax+0x48]
000F5989: mov       rax,QWORD PTR [rsi]
000F598C: xor       edx,edx
000F598E: mov       rcx,rsi
000F5991: call      QWORD PTR [rax+0x78]
000F5994: mov       ecx,DWORD PTR [r14+0x2c]
000F5998: xorps     xmm0,xmm0
000F599B: imul      eax,DWORD PTR [rip+0x237c87e]        # 0x182472220
000F59A2: mov       QWORD PTR [rbp-0x4c],rbx
000F59A6: movdqu    XMMWORD PTR [rbp-0x40],xmm0
000F59AB: mov       DWORD PTR [rbp-0x50],0x1688
000F59B2: add       QWORD PTR [rsp+0x40],rax
000F59B7: call      0x180001bd0
000F59BC: mov       DWORD PTR [rbp-0x58],eax
000F59BF: mov       DWORD PTR [rbp-0x54],0x4
000F59C6: mov       rcx,QWORD PTR [rsp+0x30]
000F59CB: lea       rdx,[rbp+0x80]
000F59D2: mov       rax,QWORD PTR [rcx]
000F59D5: call      QWORD PTR [rax+0x50]
000F59D8: mov       r9,QWORD PTR [rsp+0x40]
000F59DD: lea       r8,[rbp-0x58]
000F59E1: mov       rdx,QWORD PTR [rsp+0x30]
000F59E6: movzx     ecx,WORD PTR [rax+0x1e]
000F59EA: mov       rax,QWORD PTR [rsi]
000F59ED: mov       DWORD PTR [rbp-0x44],ecx
000F59F0: mov       rcx,rsi
000F59F3: call      QWORD PTR [rax+0x90]
000F59F9: mov       rax,QWORD PTR [rsi]
000F59FC: xor       edx,edx
000F59FE: mov       rcx,rsi
000F5A01: call      QWORD PTR [rax+0x78]
000F5A04: mov       rcx,QWORD PTR [rsp+0x48]
000F5A09: lea       rdx,[rbp+0x40]
000F5A0D: mov       eax,eax
000F5A0F: add       QWORD PTR [rsp+0x40],rax
000F5A14: mov       rax,QWORD PTR [rcx]
000F5A17: call      QWORD PTR [rax+0x50]
000F5A1A: mov       ecx,DWORD PTR [r14+0x5c]
000F5A1E: call      0x180001bd0
000F5A23: lea       ecx,[rax-0x1]
000F5A26: cmp       ecx,0x5b
000F5A29: ja        0x1800f5abd
000F5A2F: lea       r8,[rip+0xfffffffffff0a5ca]        # 0x180000000
000F5A36: movsxd    rcx,ecx
000F5A39: movzx     ecx,BYTE PTR [r8+rcx*1+0xf5d0c]
000F5A42: mov       edx,DWORD PTR [r8+rcx*4+0xf5cc8]
000F5A4A: add       rdx,r8
000F5A4D: jmp       rdx
000F5A4F: mov       eax,0x15
000F5A54: jmp       0x1800f5abd
000F5A56: mov       eax,0x29
000F5A5B: jmp       0x1800f5abd
000F5A5D: mov       eax,0x2e
000F5A62: jmp       0x1800f5abd
000F5A64: mov       eax,0x38
000F5A69: jmp       0x1800f5abd
000F5A6B: mov       eax,0x2
000F5A70: jmp       0x1800f5abd
000F5A72: mov       eax,0x6
000F5A77: jmp       0x1800f5abd
000F5A79: mov       eax,0xa
000F5A7E: jmp       0x1800f5abd
000F5A80: mov       eax,0x1c
000F5A85: jmp       0x1800f5abd
000F5A87: mov       eax,0x10
000F5A8C: jmp       0x1800f5abd
000F5A8E: mov       eax,0x22
000F5A93: jmp       0x1800f5abd
000F5A95: mov       eax,0x18
000F5A9A: jmp       0x1800f5abd
000F5A9C: mov       eax,0x57
000F5AA1: jmp       0x1800f5abd
000F5AA3: mov       eax,0x5d
000F5AA8: jmp       0x1800f5abd
000F5AAA: mov       eax,0x31
000F5AAF: jmp       0x1800f5abd
000F5AB1: mov       eax,0x36
000F5AB6: jmp       0x1800f5abd
000F5AB8: mov       eax,0x3d
000F5ABD: mov       r9,QWORD PTR [rsp+0x40]
000F5AC2: lea       r8,[rbp-0x58]
000F5AC6: mov       rdx,QWORD PTR [rsp+0x48]
000F5ACB: mov       rcx,rsi
000F5ACE: mov       DWORD PTR [rbp-0x58],eax
000F5AD1: movzx     eax,WORD PTR [rbp+0x5e]
000F5AD5: mov       DWORD PTR [rbp-0x44],eax
000F5AD8: mov       rax,QWORD PTR [rsi]
000F5ADB: call      QWORD PTR [rax+0x90]
000F5AE1: mov       eax,DWORD PTR [rip+0x237c739]        # 0x182472220
000F5AE7: xor       edx,edx
000F5AE9: mov       r8,QWORD PTR [rsp+0x58]
000F5AEE: add       eax,0x2
000F5AF1: and       eax,0x3f
000F5AF4: mov       rcx,rdi
000F5AF7: mov       DWORD PTR [rip+0x237c723],eax        # 0x182472220
000F5AFD: mov       rax,QWORD PTR [rdi]
000F5B00: call      QWORD PTR [rax+0x100]
000F5B06: mov       rcx,QWORD PTR [rip+0x237c703]        # 0x182472210
000F5B0D: lea       rdx,[rsp+0x60]
000F5B12: mov       rax,QWORD PTR [rcx]
000F5B15: call      QWORD PTR [rax+0x48]
000F5B18: mov       rax,QWORD PTR [rsi]
000F5B1B: mov       edx,0x2
000F5B20: mov       rcx,rsi
000F5B23: call      QWORD PTR [rax+0x78]
000F5B26: mov       edx,DWORD PTR [rip+0x237c6dc]        # 0x182472208
000F5B2C: lea       r8,[rbp-0x70]
000F5B30: mov       r9,QWORD PTR [rsp+0x60]
000F5B35: mov       rcx,rsi
000F5B38: imul      eax,edx
000F5B3B: add       r9,rax
000F5B3E: lea       eax,[rdx+0x1]
000F5B41: and       eax,0x1f
000F5B44: mov       QWORD PTR [rsp+0x60],r9
000F5B49: mov       DWORD PTR [rip+0x237c6b9],eax        # 0x182472208
000F5B4F: mov       rdx,r15
000F5B52: mov       rax,QWORD PTR [rsi]
000F5B55: call      QWORD PTR [rax+0xa0]
000F5B5B: mov       rax,QWORD PTR [r15]
000F5B5E: lea       rdx,[rbp+0x8]
000F5B62: mov       rcx,r15
000F5B65: call      QWORD PTR [rax+0x50]
000F5B68: mov       rdx,QWORD PTR [rbp+0x18]
000F5B6C: xorps     xmm0,xmm0
000F5B6F: mov       QWORD PTR [rsp+0x78],rbx
000F5B74: test      rdx,rdx
000F5B77: js        0x1800f5b80
000F5B79: cvtsi2ss  xmm0,rdx
000F5B7E: jmp       0x1800f5b98
000F5B80: mov       rax,rdx
000F5B83: mov       rcx,rdx
000F5B86: shr       rcx,1
000F5B89: and       eax,0x1
000F5B8C: or        rcx,rax
000F5B8F: cvtsi2ss  xmm0,rcx
000F5B94: addss     xmm0,xmm0
000F5B98: mov       ecx,DWORD PTR [rbp+0x20]
000F5B9B: lea       r8,[rsp+0x60]
000F5BA0: mov       rax,QWORD PTR [rdi]
000F5BA3: mov       r9d,0x1
000F5BA9: movss     DWORD PTR [rbp-0x80],xmm0
000F5BAE: xorps     xmm0,xmm0
000F5BB1: mov       DWORD PTR [rsp+0x70],edx
000F5BB5: mov       edx,r9d
000F5BB8: cvtsi2ss  xmm0,rcx
000F5BBD: mov       DWORD PTR [rsp+0x74],ecx
000F5BC1: mov       rcx,rdi
000F5BC4: mov       DWORD PTR [rbp-0x78],ebx
000F5BC7: mov       DWORD PTR [rbp-0x74],0x3f800000
000F5BCE: mov       QWORD PTR [rsp+0x68],rbx
000F5BD3: mov       QWORD PTR [rsp+0x20],rbx
000F5BD8: movss     DWORD PTR [rbp-0x7c],xmm0
000F5BDD: call      QWORD PTR [rax+0x170]
000F5BE3: mov       rax,QWORD PTR [rdi]
000F5BE6: mov       edx,0x4
000F5BEB: mov       rcx,rdi
000F5BEE: call      QWORD PTR [rax+0xa0]
000F5BF4: mov       rax,QWORD PTR [rdi]
000F5BF7: mov       rdx,r13
000F5BFA: mov       rcx,rdi
000F5BFD: call      QWORD PTR [rax+0xc8]
000F5C03: mov       rax,QWORD PTR [rdi]
000F5C06: lea       r8,[rsp+0x78]
000F5C0B: mov       edx,0x1
000F5C10: mov       rcx,rdi
000F5C13: call      QWORD PTR [rax+0xa8]
000F5C19: mov       rax,QWORD PTR [rdi]
000F5C1C: lea       r8,[rsp+0x68]
000F5C21: mov       edx,0x1
000F5C26: mov       rcx,rdi
000F5C29: call      QWORD PTR [rax+0xb0]
000F5C2F: mov       rax,QWORD PTR [rdi]
000F5C32: xor       r9d,r9d
000F5C35: mov       edx,0x3
000F5C3A: mov       DWORD PTR [rsp+0x20],ebx
000F5C3E: mov       r8d,0x1
000F5C44: mov       rcx,rdi
000F5C47: call      QWORD PTR [rax+0x60]
000F5C4A: lea       r8,[rbp+0xd8]
000F5C51: nop       DWORD PTR [rax+0x0]
000F5C55: data16    data16 nop WORD PTR [rax+rax*1+0x0]
000F5C60: mov       ecx,DWORD PTR [r8-0x4]
000F5C64: inc       ebx
000F5C66: mov       eax,DWORD PTR [r8]
000F5C69: mov       DWORD PTR [r8-0x4],eax
000F5C6D: mov       DWORD PTR [r8],ecx
000F5C70: lea       r8,[r8+0x20]
000F5C74: cmp       ebx,r12d
000F5C77: jl        0x1800f5c60
000F5C79: lea       r8,[rbp+0xc0]
000F5C80: mov       edx,r12d
000F5C83: mov       rax,QWORD PTR [rdi]
000F5C86: mov       rcx,rdi
000F5C89: call      QWORD PTR [rax+0xd0]
000F5C8F: mov       r13,QWORD PTR [rsp+0x278]
000F5C97: xor       eax,eax
000F5C99: mov       r12,QWORD PTR [rsp+0x270]
000F5CA1: mov       rdi,QWORD PTR [rsp+0x268]
000F5CA9: mov       rcx,QWORD PTR [rbp+0x120]
000F5CB0: xor       rcx,rsp
000F5CB3: call      0x180104650
000F5CB8: add       rsp,0x230
000F5CBF: pop       r15
000F5CC1: pop       r14
000F5CC3: pop       rsi
000F5CC4: pop       rbx
000F5CC5: pop       rbp
000F5CC6: ret       
000F5CC7: nop       
000F5CC8: imul      ebx,DWORD PTR [rdx+0xf],0x0
000F5CCC: jb        0x1800f5d28
000F5CCE: (bad)     
000F5CD0: jns       0x1800f5d2c
000F5CD2: sldt      WORD PTR [rdi+0x4f000f5a]
000F5CD9: pop       rdx
000F5CDA: lldt      WORD PTR [rbp-0x7ffff0a6]
000F5CE1: pop       rdx
000F5CE2: str       WORD PTR [rsi+0x56000f5a]
000F5CE9: pop       rdx
000F5CEA: ltr       WORD PTR [rbp+0x5a]
000F5CEE: verw      WORD PTR [rdx-0x4efff0a6]
000F5CF5: pop       rdx
000F5CF6: verr      WORD PTR [rdx+rbx*2+0xf]
000F5CFB: add       BYTE PTR [rax-0x63fff0a6],bh
000F5D01: pop       rdx
000F5D02: verr      WORD PTR [rbx-0x42fff0a6]
000F5D09: pop       rdx
000F5D0A: sldt      WORD PTR [rax]
000F5D0D: adc       BYTE PTR [rax],dl
000F5D0F: adc       BYTE PTR [rcx],al
000F5D11: adc       BYTE PTR [rax],dl
000F5D13: adc       BYTE PTR [rdx],al
000F5D15: adc       BYTE PTR [rax],dl
000F5D17: adc       BYTE PTR [rax],dl
000F5D19: adc       BYTE PTR [rbx],al
000F5D1B: adc       BYTE PTR [rax],dl
000F5D1D: adc       BYTE PTR [rsp+rax*1],al
000F5D20: adc       BYTE PTR [rax],dl
000F5D22: add       eax,0x6101010
000F5D27: adc       BYTE PTR [rax],dl
000F5D29: adc       BYTE PTR [rax],dl
000F5D2B: adc       BYTE PTR [rdi],al
000F5D2D: adc       BYTE PTR [rax],dl
000F5D2F: adc       BYTE PTR [rax],dl
000F5D31: adc       BYTE PTR [rax],cl
000F5D33: or        BYTE PTR [rax],dl
000F5D35: adc       BYTE PTR [rax],dl
000F5D37: or        DWORD PTR [rcx],ecx
000F5D39: adc       BYTE PTR [rcx],cl
000F5D3B: or        dl,BYTE PTR [rax]
000F5D3D: adc       BYTE PTR [rax],dl
000F5D3F: adc       BYTE PTR [rbx],cl
000F5D41: adc       BYTE PTR [rax+rdx*1],cl
000F5D44: adc       BYTE PTR [rax],dl
000F5D46: adc       BYTE PTR [rip+0x10101010],cl        # 0x1901f6d5c
000F5D4C: adc       BYTE PTR [rax],dl
000F5D4E: adc       BYTE PTR [rax],dl
000F5D50: adc       BYTE PTR [rax],dl
000F5D52: adc       BYTE PTR [rax],dl
000F5D54: adc       BYTE PTR [rax],dl
000F5D56: adc       BYTE PTR [rax],dl
000F5D58: adc       BYTE PTR [rax],dl
000F5D5A: adc       BYTE PTR [rax],dl
000F5D5C: adc       BYTE PTR [rax],dl
000F5D5E: adc       BYTE PTR [rax],dl
000F5D60: adc       BYTE PTR [rax],dl
000F5D62: adc       BYTE PTR [rax],dl
000F5D64: adc       BYTE PTR [rsi],cl
000F5D66: adc       BYTE PTR [rdi],cl
