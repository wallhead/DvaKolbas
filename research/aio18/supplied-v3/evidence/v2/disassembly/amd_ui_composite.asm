; amd_fidelityfx_framegeneration_dx12.dll SHA256=3f5e674a59b400756e98ef31fd583a4eb08ad567ec8dee362886bc96e55ed347
; ImageBase=0x180000000, RVAs in left column
; range 0xE4C0..0xEC0E; compositeSwapChainFrame<FrameinterpolationPresentInfoExt,FrameInterpolationPacingDataExt,ffxCallbackDescFrameGenerationPresent>
0000E4C0: mov       rax,rsp
0000E4C3: mov       QWORD PTR [rax+0x20],rbx
0000E4C7: push      rbp
0000E4C8: push      rsi
0000E4C9: push      rdi
0000E4CA: push      r12
0000E4CC: push      r13
0000E4CE: push      r14
0000E4D0: push      r15
0000E4D2: lea       rbp,[rax-0x338]
0000E4D9: sub       rsp,0x400
0000E4E0: movaps    XMMWORD PTR [rax-0x48],xmm6
0000E4E4: movaps    XMMWORD PTR [rax-0x58],xmm7
0000E4E8: movaps    XMMWORD PTR [rax-0x68],xmm8
0000E4ED: movaps    XMMWORD PTR [rax-0x78],xmm9
0000E4F2: movaps    XMMWORD PTR [rax-0x88],xmm10
0000E4FA: movaps    XMMWORD PTR [rax-0x98],xmm11
0000E502: mov       rax,QWORD PTR [rip+0x245faf7]        # 0x18246e000
0000E509: xor       rax,rsp
0000E50C: mov       QWORD PTR [rbp+0x298],rax
0000E513: mov       eax,r8d
0000E516: mov       rdi,rdx
0000E519: mov       DWORD PTR [rsp+0x20],r8d
0000E51E: mov       r14,rcx
0000E521: mov       QWORD PTR [rsp+0x28],rdx
0000E526: lea       r15,[rax+rax*4]
0000E52A: shl       r15,0x4
0000E52E: add       r15,rdx
0000E531: cmp       r8d,0x1
0000E535: je        0x18000e552
0000E537: mov       rcx,QWORD PTR [rcx+0x16d0]
0000E53E: mov       r8,QWORD PTR [r15+0xa8]
0000E545: mov       rdx,QWORD PTR [r14+0x16e0]
0000E54C: mov       rax,QWORD PTR [rcx]
0000E54F: call      QWORD PTR [rax+0x78]
0000E552: cmp       BYTE PTR [rdi+0x43],0x0
0000E556: je        0x18000e85a
0000E55C: mov       r12,QWORD PTR [r14+0x16d0]
0000E563: lea       rdx,[rbp+0x190]
0000E56A: mov       rcx,r12
0000E56D: lea       r13,[r14+0x38]
0000E571: mov       rax,QWORD PTR [r12]
0000E575: call      QWORD PTR [rax+0x90]
0000E57B: mov       rcx,r13
0000E57E: call      QWORD PTR [rip+0xf7b8c]        # 0x180106110 ; KERNEL32.dll!EnterCriticalSection
0000E584: xor       ebx,ebx
0000E586: xor       esi,esi
0000E588: test      rbx,rbx
0000E58B: jne       0x18000e5d7
0000E58D: movsxd    rax,DWORD PTR [rbp+0x190]
0000E594: mov       rdx,r12
0000E597: shl       rax,0x5
0000E59B: inc       rax
0000E59E: add       rax,rsi
0000E5A1: lea       rcx,[rax+rax*4]
0000E5A5: lea       rdi,[rcx*8+0x0]
0000E5AD: add       rdi,r13
0000E5B0: mov       rcx,rdi
0000E5B3: call      0x180007320
0000E5B8: test      al,al
0000E5BA: je        0x18000e5ce
0000E5BC: mov       rcx,QWORD PTR [rdi+0x18]
0000E5C0: mov       rax,QWORD PTR [rcx]
0000E5C3: call      QWORD PTR [rax+0x40]
0000E5C6: cmp       rax,QWORD PTR [rdi+0x20]
0000E5CA: cmovae    rbx,rdi
0000E5CE: inc       rsi
0000E5D1: cmp       rsi,0x20
0000E5D5: jb        0x18000e588
0000E5D7: mov       rcx,QWORD PTR [rbx+0x8]
0000E5DB: lea       rdx,[rip+0xf889e]        # 0x180106e80 ; 'compositeSwapChainFrame'
0000E5E2: inc       QWORD PTR [rbx+0x20]
0000E5E6: mov       QWORD PTR [rbx],r12
0000E5E9: mov       rax,QWORD PTR [rcx]
0000E5EC: call      QWORD PTR [rax+0x30]
0000E5EF: mov       rcx,QWORD PTR [rbx+0x10]
0000E5F3: lea       rdx,[rip+0xf8886]        # 0x180106e80 ; 'compositeSwapChainFrame'
0000E5FA: mov       rax,QWORD PTR [rcx]
0000E5FD: call      QWORD PTR [rax+0x30]
0000E600: mov       rcx,QWORD PTR [rbx+0x18]
0000E604: lea       rdx,[rip+0xf8875]        # 0x180106e80 ; 'compositeSwapChainFrame'
0000E60B: mov       rax,QWORD PTR [rcx]
0000E60E: call      QWORD PTR [rax+0x30]
0000E611: mov       rcx,r13
0000E614: call      QWORD PTR [rip+0xf7ade]        # 0x1801060f8 ; KERNEL32.dll!LeaveCriticalSection
0000E61A: mov       rcx,QWORD PTR [r14+0x30]
0000E61E: mov       rax,QWORD PTR [rcx]
0000E621: call      QWORD PTR [rax+0x120]
0000E627: mov       rcx,QWORD PTR [r14+0x30]
0000E62B: lea       r9,[rbp+0x188]
0000E632: xor       edi,edi
0000E634: lea       r8,[rip+0xf81d5]        # 0x180106810
0000E63B: mov       QWORD PTR [rbp+0x188],rdi
0000E642: mov       rdx,QWORD PTR [rcx]
0000E645: mov       r10,QWORD PTR [rdx+0x48]
0000E649: mov       edx,eax
0000E64B: call      r10
0000E64E: mov       rdi,QWORD PTR [rsp+0x28]
0000E653: lea       rcx,[rbp+0xa0]
0000E65A: mov       rdx,QWORD PTR [rbp+0x188]
0000E661: movups    xmm9,XMMWORD PTR [r15+0x78]
0000E666: mov       rsi,QWORD PTR [rdi+0x68]
0000E66A: movzx     r12d,BYTE PTR [rdi+0x42]
0000E66F: movups    xmm6,XMMWORD PTR [rdi+0x10]
0000E673: movups    xmm7,XMMWORD PTR [rdi+0x20]
0000E677: movups    xmm8,XMMWORD PTR [rdi+0x30]
0000E67C: movups    xmm10,XMMWORD PTR [r15+0x88]
0000E684: movups    xmm11,XMMWORD PTR [r15+0x98]
0000E68C: call      0x180001f50
0000E691: mov       rcx,rax
0000E694: mov       QWORD PTR [rsp+0x58],0x80
0000E69D: mov       rax,QWORD PTR [rbp+0x188]
0000E6A4: xorps     xmm0,xmm0
0000E6A7: movdqa    XMMWORD PTR [rsp+0x40],xmm0
0000E6AD: xor       edi,edi
0000E6AF: mov       QWORD PTR [rsp+0x30],rax
0000E6B4: movups    xmm0,XMMWORD PTR [rcx]
0000E6B7: movups    xmm1,XMMWORD PTR [rcx+0x10]
0000E6BB: mov       rcx,QWORD PTR [rbx+0x8]
0000E6BF: movups    XMMWORD PTR [rsp+0x38],xmm0
0000E6C4: movups    XMMWORD PTR [rsp+0x48],xmm1
0000E6C9: mov       rax,QWORD PTR [rcx]
0000E6CC: call      QWORD PTR [rax+0x40]
0000E6CF: test      eax,eax
0000E6D1: js        0x18000e6e4
0000E6D3: mov       rcx,QWORD PTR [rbx+0x10]
0000E6D7: xor       r8d,r8d
0000E6DA: mov       rdx,QWORD PTR [rbx+0x8]
0000E6DE: mov       rax,QWORD PTR [rcx]
0000E6E1: call      QWORD PTR [rax+0x50]
0000E6E4: xor       edx,edx
0000E6E6: lea       rcx,[rbp+0xc0]
0000E6ED: mov       r8d,0xc0
0000E6F3: call      0x1801056da
0000E6F8: cmp       DWORD PTR [rsp+0x20],0x1
0000E6FD: lea       rax,[rbp+0xc0]
0000E704: movups    xmm0,XMMWORD PTR [rax]
0000E707: lea       rcx,[rbp-0x20]
0000E70B: movups    xmm1,XMMWORD PTR [rax+0x10]
0000E70F: movups    XMMWORD PTR [rcx],xmm0
0000E712: movups    xmm0,XMMWORD PTR [rax+0x20]
0000E716: movups    XMMWORD PTR [rcx+0x10],xmm1
0000E71A: movups    xmm1,XMMWORD PTR [rax+0x30]
0000E71E: movups    XMMWORD PTR [rcx+0x20],xmm0
0000E722: movups    xmm0,XMMWORD PTR [rax+0x40]
0000E726: movups    XMMWORD PTR [rcx+0x30],xmm1
0000E72A: movups    xmm1,XMMWORD PTR [rax+0x50]
0000E72E: movups    XMMWORD PTR [rcx+0x40],xmm0
0000E732: movups    xmm0,XMMWORD PTR [rax+0x60]
0000E736: movups    XMMWORD PTR [rcx+0x50],xmm1
0000E73A: movups    xmm1,XMMWORD PTR [rax+0x70]
0000E73E: movups    XMMWORD PTR [rcx+0x60],xmm0
0000E742: movups    xmm0,XMMWORD PTR [rax+0x80]
0000E749: movups    XMMWORD PTR [rcx+0x70],xmm1
0000E74D: movups    xmm1,XMMWORD PTR [rax+0x90]
0000E754: movups    XMMWORD PTR [rcx+0x80],xmm0
0000E75B: movups    xmm0,XMMWORD PTR [rax+0xa0]
0000E762: movups    XMMWORD PTR [rcx+0x90],xmm1
0000E769: movups    xmm1,XMMWORD PTR [rax+0xb0]
0000E770: mov       rax,QWORD PTR [rbx+0x10]
0000E774: movups    XMMWORD PTR [rcx+0xa0],xmm0
0000E77B: mov       BYTE PTR [rbp+0x1b0],r12b
0000E782: movaps    xmm0,XMMWORD PTR [rsp+0x30]
0000E787: movups    XMMWORD PTR [rcx+0xb0],xmm1
0000E78E: lea       rcx,[rbp-0x20]
0000E792: mov       QWORD PTR [rbp-0x8],rax
0000E796: movaps    xmm1,XMMWORD PTR [rsp+0x40]
0000E79B: setne     BYTE PTR [rbp+0x90]
0000E7A2: mov       rax,QWORD PTR [r14+0x28]
0000E7A6: mov       QWORD PTR [rbp-0x10],rax
0000E7AA: lea       rax,[rbp+0x1a0]
0000E7B1: movaps    XMMWORD PTR [rbp+0x60],xmm0
0000E7B5: movaps    xmm0,XMMWORD PTR [rsp+0x50]
0000E7BA: mov       QWORD PTR [rbp-0x18],rax
0000E7BE: mov       QWORD PTR [rbp-0x20],0x20005
0000E7C6: movaps    XMMWORD PTR [rbp+0x70],xmm1
0000E7CA: movaps    XMMWORD PTR [rbp+0x80],xmm0
0000E7D1: movaps    XMMWORD PTR [rbp+0x0],xmm9
0000E7D6: movaps    XMMWORD PTR [rbp+0x10],xmm10
0000E7DB: movaps    XMMWORD PTR [rbp+0x20],xmm11
0000E7E0: movaps    XMMWORD PTR [rbp+0x30],xmm6
0000E7E4: movaps    XMMWORD PTR [rbp+0x40],xmm7
0000E7E8: movaps    XMMWORD PTR [rbp+0x50],xmm8
0000E7ED: mov       QWORD PTR [rbp+0x98],rsi
0000E7F4: mov       QWORD PTR [rbp+0x1a8],rdi
0000E7FB: mov       QWORD PTR [rbp+0x1a0],0x2000d
0000E806: call      0x1800f6210 ; ffxFrameInterpolationDebugPacing
0000E80B: mov       rcx,QWORD PTR [rbx+0x10]
0000E80F: mov       rax,QWORD PTR [rcx]
0000E812: call      QWORD PTR [rax+0x48]
0000E815: mov       rax,QWORD PTR [rbx+0x10]
0000E819: lea       r8,[rbp+0x290]
0000E820: mov       rcx,QWORD PTR [rbx]
0000E823: mov       edx,0x1
0000E828: mov       QWORD PTR [rbp+0x290],rax
0000E82F: mov       rax,QWORD PTR [rcx]
0000E832: call      QWORD PTR [rax+0x50]
0000E835: mov       rcx,QWORD PTR [rbx]
0000E838: mov       r8,QWORD PTR [rbx+0x20]
0000E83C: mov       rdx,QWORD PTR [rbx+0x18]
0000E840: mov       rax,QWORD PTR [rcx]
0000E843: call      QWORD PTR [rax+0x70]
0000E846: mov       rcx,QWORD PTR [rbp+0x188]
0000E84D: test      rcx,rcx
0000E850: je        0x18000e85c
0000E852: mov       rax,QWORD PTR [rcx]
0000E855: call      QWORD PTR [rax+0x10]
0000E858: jmp       0x18000e85c
0000E85A: xor       edi,edi
0000E85C: mov       r12,QWORD PTR [r14+0x16d0]
0000E863: lea       rdx,[rbp+0x190]
0000E86A: mov       rcx,r12
0000E86D: lea       r13,[r14+0x38]
0000E871: mov       rax,QWORD PTR [r12]
0000E875: call      QWORD PTR [rax+0x90]
0000E87B: mov       rcx,r13
0000E87E: call      QWORD PTR [rip+0xf788c]        # 0x180106110 ; KERNEL32.dll!EnterCriticalSection
0000E884: mov       rbx,rdi
0000E887: mov       rsi,rdi
0000E88A: nop       WORD PTR [rax+rax*1+0x0]
0000E890: test      rbx,rbx
0000E893: jne       0x18000e8df
0000E895: movsxd    rax,DWORD PTR [rbp+0x190]
0000E89C: mov       rdx,r12
0000E89F: shl       rax,0x5
0000E8A3: inc       rax
0000E8A6: add       rax,rsi
0000E8A9: lea       rcx,[rax+rax*4]
0000E8AD: lea       rdi,[rcx*8+0x0]
0000E8B5: add       rdi,r13
0000E8B8: mov       rcx,rdi
0000E8BB: call      0x180007320
0000E8C0: test      al,al
0000E8C2: je        0x18000e8d6
0000E8C4: mov       rcx,QWORD PTR [rdi+0x18]
0000E8C8: mov       rax,QWORD PTR [rcx]
0000E8CB: call      QWORD PTR [rax+0x40]
0000E8CE: cmp       rax,QWORD PTR [rdi+0x20]
0000E8D2: cmovae    rbx,rdi
0000E8D6: inc       rsi
0000E8D9: cmp       rsi,0x20
0000E8DD: jb        0x18000e890
0000E8DF: mov       rcx,QWORD PTR [rbx+0x8]
0000E8E3: lea       rdx,[rip+0xf8596]        # 0x180106e80 ; 'compositeSwapChainFrame'
0000E8EA: inc       QWORD PTR [rbx+0x20]
0000E8EE: mov       QWORD PTR [rbx],r12
0000E8F1: mov       rax,QWORD PTR [rcx]
0000E8F4: call      QWORD PTR [rax+0x30]
0000E8F7: mov       rcx,QWORD PTR [rbx+0x10]
0000E8FB: lea       rdx,[rip+0xf857e]        # 0x180106e80 ; 'compositeSwapChainFrame'
0000E902: mov       rax,QWORD PTR [rcx]
0000E905: call      QWORD PTR [rax+0x30]
0000E908: mov       rcx,QWORD PTR [rbx+0x18]
0000E90C: lea       rdx,[rip+0xf856d]        # 0x180106e80 ; 'compositeSwapChainFrame'
0000E913: mov       rax,QWORD PTR [rcx]
0000E916: call      QWORD PTR [rax+0x30]
0000E919: mov       rcx,r13
0000E91C: call      QWORD PTR [rip+0xf77d6]        # 0x1801060f8 ; KERNEL32.dll!LeaveCriticalSection
0000E922: mov       rcx,QWORD PTR [r14+0x30]
0000E926: mov       rax,QWORD PTR [rcx]
0000E929: call      QWORD PTR [rax+0x120]
0000E92F: mov       rcx,QWORD PTR [r14+0x30]
0000E933: lea       r9,[rbp+0x180]
0000E93A: xor       edi,edi
0000E93C: lea       r8,[rip+0xf7ecd]        # 0x180106810
0000E943: mov       QWORD PTR [rbp+0x180],rdi
0000E94A: mov       rdx,QWORD PTR [rcx]
0000E94D: mov       r10,QWORD PTR [rdx+0x48]
0000E951: mov       edx,eax
0000E953: call      r10
0000E956: mov       rsi,QWORD PTR [rsp+0x28]
0000E95B: lea       rcx,[rbp+0xa0]
0000E962: mov       rdx,QWORD PTR [rbp+0x180]
0000E969: movups    xmm9,XMMWORD PTR [r15+0x78]
0000E96E: mov       r12,QWORD PTR [rsi+0x68]
0000E972: movzx     r13d,BYTE PTR [rsi+0x42]
0000E977: movups    xmm6,XMMWORD PTR [rsi+0x10]
0000E97B: movups    xmm7,XMMWORD PTR [rsi+0x20]
0000E97F: movups    xmm8,XMMWORD PTR [rsi+0x30]
0000E984: movups    xmm10,XMMWORD PTR [r15+0x88]
0000E98C: movups    xmm11,XMMWORD PTR [r15+0x98]
0000E994: call      0x180001f50
0000E999: mov       rcx,rax
0000E99C: mov       QWORD PTR [rsp+0x58],0x80
0000E9A5: mov       rax,QWORD PTR [rbp+0x180]
0000E9AC: xorps     xmm0,xmm0
0000E9AF: movdqa    XMMWORD PTR [rsp+0x40],xmm0
0000E9B5: mov       QWORD PTR [rsp+0x30],rax
0000E9BA: movups    xmm0,XMMWORD PTR [rcx]
0000E9BD: movups    xmm1,XMMWORD PTR [rcx+0x10]
0000E9C1: mov       rcx,QWORD PTR [rbx+0x8]
0000E9C5: movups    XMMWORD PTR [rsp+0x38],xmm0
0000E9CA: movups    XMMWORD PTR [rsp+0x48],xmm1
0000E9CF: mov       rax,QWORD PTR [rcx]
0000E9D2: call      QWORD PTR [rax+0x40]
0000E9D5: test      eax,eax
0000E9D7: js        0x18000e9ea
0000E9D9: mov       rcx,QWORD PTR [rbx+0x10]
0000E9DD: xor       r8d,r8d
0000E9E0: mov       rdx,QWORD PTR [rbx+0x8]
0000E9E4: mov       rax,QWORD PTR [rcx]
0000E9E7: call      QWORD PTR [rax+0x50]
0000E9EA: mov       rdi,QWORD PTR [rbx+0x10]
0000E9EE: lea       rcx,[rbp+0xc0]
0000E9F5: xor       edx,edx
0000E9F7: mov       r8d,0xc0
0000E9FD: call      0x1801056da
0000EA02: cmp       DWORD PTR [rsp+0x20],0x1
0000EA07: lea       rax,[rbp+0xc0]
0000EA0E: movups    xmm0,XMMWORD PTR [rax]
0000EA11: mov       rdx,QWORD PTR [rsi+0x8]
0000EA15: lea       rcx,[rbp+0x1d0]
0000EA1C: movups    xmm1,XMMWORD PTR [rax+0x10]
0000EA20: mov       BYTE PTR [rbp+0x1c8],r13b
0000EA27: movups    XMMWORD PTR [rcx],xmm0
0000EA2A: mov       QWORD PTR [rbp+0x1b8],0x2000d
0000EA35: movups    xmm0,XMMWORD PTR [rax+0x20]
0000EA39: movups    XMMWORD PTR [rcx+0x10],xmm1
0000EA3D: movups    xmm1,XMMWORD PTR [rax+0x30]
0000EA41: movups    XMMWORD PTR [rcx+0x20],xmm0
0000EA45: movups    xmm0,XMMWORD PTR [rax+0x40]
0000EA49: movups    XMMWORD PTR [rcx+0x30],xmm1
0000EA4D: movups    xmm1,XMMWORD PTR [rax+0x50]
0000EA51: movups    XMMWORD PTR [rcx+0x40],xmm0
0000EA55: movups    xmm0,XMMWORD PTR [rax+0x60]
0000EA59: movups    XMMWORD PTR [rcx+0x50],xmm1
0000EA5D: movups    xmm1,XMMWORD PTR [rax+0x70]
0000EA61: movups    XMMWORD PTR [rcx+0x60],xmm0
0000EA65: movups    xmm0,XMMWORD PTR [rax+0x80]
0000EA6C: movups    XMMWORD PTR [rcx+0x70],xmm1
0000EA70: movups    xmm1,XMMWORD PTR [rax+0x90]
0000EA77: movups    XMMWORD PTR [rcx+0x80],xmm0
0000EA7E: movups    xmm0,XMMWORD PTR [rax+0xa0]
0000EA85: movups    XMMWORD PTR [rcx+0x90],xmm1
0000EA8C: movups    xmm1,XMMWORD PTR [rax+0xb0]
0000EA93: mov       rax,QWORD PTR [r14+0x28]
0000EA97: movups    XMMWORD PTR [rcx+0xa0],xmm0
0000EA9E: movaps    xmm0,XMMWORD PTR [rsp+0x30]
0000EAA3: movups    XMMWORD PTR [rcx+0xb0],xmm1
0000EAAA: lea       rcx,[rbp+0x1d0]
0000EAB1: mov       QWORD PTR [rbp+0x1e0],rax
0000EAB8: movaps    xmm1,XMMWORD PTR [rsp+0x40]
0000EABD: lea       rax,[rbp+0x1b8]
0000EAC4: mov       QWORD PTR [rbp+0x1e8],rdi
0000EACB: setne     BYTE PTR [rbp+0x280]
0000EAD2: movaps    XMMWORD PTR [rbp+0x250],xmm0
0000EAD9: xor       edi,edi
0000EADB: movaps    xmm0,XMMWORD PTR [rsp+0x50]
0000EAE0: mov       QWORD PTR [rbp+0x1d8],rax
0000EAE7: mov       rax,QWORD PTR [rsi]
0000EAEA: mov       QWORD PTR [rbp+0x1d0],0x20005
0000EAF5: movaps    XMMWORD PTR [rbp+0x260],xmm1
0000EAFC: movaps    XMMWORD PTR [rbp+0x270],xmm0
0000EB03: movaps    XMMWORD PTR [rbp+0x1f0],xmm9
0000EB0B: movaps    XMMWORD PTR [rbp+0x200],xmm10
0000EB13: movaps    XMMWORD PTR [rbp+0x210],xmm11
0000EB1B: movaps    XMMWORD PTR [rbp+0x220],xmm6
0000EB22: movaps    XMMWORD PTR [rbp+0x230],xmm7
0000EB29: movaps    XMMWORD PTR [rbp+0x240],xmm8
0000EB31: mov       QWORD PTR [rbp+0x288],r12
0000EB38: mov       QWORD PTR [rbp+0x1c0],rdi
0000EB3F: call      rax
0000EB41: mov       rcx,QWORD PTR [rbx+0x10]
0000EB45: mov       rax,QWORD PTR [rcx]
0000EB48: call      QWORD PTR [rax+0x48]
0000EB4B: mov       rax,QWORD PTR [rbx+0x10]
0000EB4F: lea       r8,[rbp+0x290]
0000EB56: mov       rcx,QWORD PTR [rbx]
0000EB59: mov       edx,0x1
0000EB5E: mov       QWORD PTR [rbp+0x290],rax
0000EB65: mov       rax,QWORD PTR [rcx]
0000EB68: call      QWORD PTR [rax+0x50]
0000EB6B: mov       rcx,QWORD PTR [rbx]
0000EB6E: mov       r8,QWORD PTR [rbx+0x20]
0000EB72: mov       rdx,QWORD PTR [rbx+0x18]
0000EB76: mov       rax,QWORD PTR [rcx]
0000EB79: call      QWORD PTR [rax+0x70]
0000EB7C: mov       rcx,QWORD PTR [rbp+0x180]
0000EB83: test      rcx,rcx
0000EB86: je        0x18000eb95
0000EB88: mov       rax,QWORD PTR [rcx]
0000EB8B: call      QWORD PTR [rax+0x10]
0000EB8E: mov       QWORD PTR [rbp+0x180],rdi
0000EB95: mov       rcx,QWORD PTR [r14+0x16d0]
0000EB9C: mov       r8,QWORD PTR [r15+0xb0]
0000EBA3: mov       rdx,QWORD PTR [r14+0x1700]
0000EBAA: mov       rax,QWORD PTR [rcx]
0000EBAD: call      QWORD PTR [rax+0x70]
0000EBB0: mov       rcx,QWORD PTR [r14+0x16f8]
0000EBB7: mov       rdx,QWORD PTR [r15+0xb0]
0000EBBE: mov       rax,QWORD PTR [rcx]
0000EBC1: call      QWORD PTR [rax+0x50]
0000EBC4: xor       eax,eax
0000EBC6: mov       rcx,QWORD PTR [rbp+0x298]
0000EBCD: xor       rcx,rsp
0000EBD0: call      0x180104650
0000EBD5: lea       r11,[rsp+0x400]
0000EBDD: mov       rbx,QWORD PTR [r11+0x58]
0000EBE1: movaps    xmm6,XMMWORD PTR [r11-0x10]
0000EBE6: movaps    xmm7,XMMWORD PTR [r11-0x20]
0000EBEB: movaps    xmm8,XMMWORD PTR [r11-0x30]
0000EBF0: movaps    xmm9,XMMWORD PTR [r11-0x40]
0000EBF5: movaps    xmm10,XMMWORD PTR [r11-0x50]
0000EBFA: movaps    xmm11,XMMWORD PTR [r11-0x60]
0000EBFF: mov       rsp,r11
0000EC02: pop       r15
0000EC04: pop       r14
0000EC06: pop       r13
0000EC08: pop       r12
0000EC0A: pop       rdi
0000EC0B: pop       rsi
0000EC0C: pop       rbp
0000EC0D: ret       
