; amd_fidelityfx_framegeneration_dx12.dll SHA256=3f5e674a59b400756e98ef31fd583a4eb08ad567ec8dee362886bc96e55ed347
; ImageBase=0x180000000, RVAs in left column
; range 0x98E0..0x9C55; TFrameInterpolationSwapChainDX12<IFrameInterpolationSwapChainDX12,FrameinterpolationPresentInfoExt,FfxFrameGenerationConfig,ffxCallbackDescFrameGenerationPresent,16>::presentInterpolated
000098E0: mov       QWORD PTR [rsp+0x18],rbx
000098E5: mov       QWORD PTR [rsp+0x20],rsi
000098EA: push      rbp
000098EB: push      rdi
000098EC: push      r14
000098EE: lea       rbp,[rsp-0xf0]
000098F6: sub       rsp,0x1f0
000098FD: mov       rax,QWORD PTR [rip+0x24646fc]        # 0x18246e000 ; __security_cookie
00009904: xor       rax,rsp
00009907: mov       QWORD PTR [rbp+0xe0],rax
0000990E: mov       rbx,rcx
00009911: test      edx,edx
00009913: mov       rcx,QWORD PTR [rcx+0x16d0]
0000991A: setne     dil
0000991E: mov       rdx,QWORD PTR [rbx+0x16e0]
00009925: mov       rax,QWORD PTR [rcx]
00009928: inc       QWORD PTR [rbx+0x1870]
0000992F: mov       r8,QWORD PTR [rbx+0x1870]
00009936: call      QWORD PTR [rax+0x70]
00009939: mov       rcx,QWORD PTR [rbx+0x16c0]
00009940: mov       r8,QWORD PTR [rbx+0x1870]
00009947: mov       rdx,QWORD PTR [rbx+0x16e0]
0000994E: mov       rax,QWORD PTR [rcx]
00009951: call      QWORD PTR [rax+0x78]
00009954: mov       rax,QWORD PTR [rbx]
00009957: lea       r8,[rbp+0xb0]
0000995E: xorps     xmm0,xmm0
00009961: lea       rdx,[rbp+0x80]
00009968: xorps     xmm1,xmm1
0000996B: mov       rcx,rbx
0000996E: movups    XMMWORD PTR [rbp+0x80],xmm0
00009975: movups    XMMWORD PTR [rbp+0x90],xmm0
0000997C: movups    XMMWORD PTR [rbp+0xa0],xmm0
00009983: movups    XMMWORD PTR [rbp+0xb0],xmm1
0000998A: movups    XMMWORD PTR [rbp+0xc0],xmm1
00009991: movups    XMMWORD PTR [rbp+0xd0],xmm1
00009998: call      QWORD PTR [rax+0x160]
0000999E: lea       rcx,[rbx+0x8]
000099A2: call      QWORD PTR [rip+0xfc768]        # 0x180106110 ; __imp_EnterCriticalSection | KERNEL32.dll!EnterCriticalSection
000099A8: xorps     xmm0,xmm0
000099AB: lea       rcx,[rbp-0x40]
000099AF: xor       edx,edx
000099B1: mov       r8d,0xa0
000099B7: movups    XMMWORD PTR [rsp+0x60],xmm0
000099BC: movups    XMMWORD PTR [rsp+0x70],xmm0
000099C1: movups    XMMWORD PTR [rbp-0x80],xmm0
000099C5: movups    XMMWORD PTR [rbp-0x70],xmm0
000099C9: movups    XMMWORD PTR [rbp-0x60],xmm0
000099CD: movups    XMMWORD PTR [rbp-0x50],xmm0
000099D1: call      0x1801056da ; memset
000099D6: mov       rax,QWORD PTR [rbx+0x1a90]
000099DD: xor       esi,esi
000099DF: test      BYTE PTR [rbx+0x16b8],0x2
000099E6: mov       QWORD PTR [rsp+0x50],rax
000099EB: mov       rax,QWORD PTR [rbx+0x1a98]
000099F2: mov       QWORD PTR [rsp+0x58],rax
000099F7: movzx     eax,BYTE PTR [rbx+0x1a58]
000099FE: mov       BYTE PTR [rbp-0x6d],al
00009A01: je        0x180009a54
00009A03: mov       rdx,QWORD PTR [rbx+0x1a30]
00009A0A: lea       rcx,[rbp+0x60]
00009A0E: call      0x180001f50 ; ffxGetResourceDescriptionDX12
00009A13: mov       rcx,QWORD PTR [rbx+0x1a30]
00009A1A: mov       QWORD PTR [rsp+0x20],rcx
00009A1F: mov       ecx,DWORD PTR [rbx+0x16b0]
00009A25: movups    xmm0,XMMWORD PTR [rax]
00009A28: mov       DWORD PTR [rsp+0x4c],esi
00009A2C: movups    xmm1,XMMWORD PTR [rax+0x10]
00009A30: mov       DWORD PTR [rsp+0x48],ecx
00009A34: movups    XMMWORD PTR [rsp+0x28],xmm0
00009A39: movups    xmm0,XMMWORD PTR [rsp+0x20]
00009A3E: movups    XMMWORD PTR [rsp+0x38],xmm1
00009A43: movups    xmm1,XMMWORD PTR [rsp+0x30]
00009A48: movaps    XMMWORD PTR [rsp+0x60],xmm0
00009A4D: movups    xmm0,XMMWORD PTR [rsp+0x40]
00009A52: jmp       0x180009a6e
00009A54: movups    xmm0,XMMWORD PTR [rbx+0x1688]
00009A5B: movups    xmm1,XMMWORD PTR [rbx+0x1698]
00009A62: movaps    XMMWORD PTR [rsp+0x60],xmm0
00009A67: movups    xmm0,XMMWORD PTR [rbx+0x16a8]
00009A6E: movzx     eax,BYTE PTR [rbx+0x1a54]
00009A75: mov       r8,QWORD PTR [rbx+0x1868]
00009A7C: mov       BYTE PTR [rbp-0x6f],al
00009A7F: movzx     eax,BYTE PTR [rbx+0x16b8]
00009A86: and       al,0x1
00009A88: mov       BYTE PTR [rbp-0x70],dil
00009A8C: mov       rdi,QWORD PTR [rbx+0x1a68]
00009A93: mov       rcx,rdi
00009A96: mov       BYTE PTR [rbp-0x6e],al
00009A99: mov       rax,QWORD PTR [rbx+0x1a60]
00009AA0: mov       QWORD PTR [rbp-0x48],rax
00009AA4: movaps    XMMWORD PTR [rbp-0x80],xmm0
00009AA8: movaps    XMMWORD PTR [rsp+0x70],xmm1
00009AAD: mov       QWORD PTR [rbp-0x58],rdi
00009AB1: mov       QWORD PTR [rbp-0x68],r8
00009AB5: cmp       QWORD PTR [rbp+0x80],rsi
00009ABC: je        0x180009af9
00009ABE: movups    xmm0,XMMWORD PTR [rbp+0x80]
00009AC5: lea       rcx,[rdi+0x1]
00009AC9: mov       dl,0x1
00009ACB: movups    xmm1,XMMWORD PTR [rbp+0x90]
00009AD2: mov       BYTE PTR [rbp-0x40],dl
00009AD5: movups    XMMWORD PTR [rbp-0x38],xmm0
00009AD9: mov       QWORD PTR [rbp-0x8],r8
00009ADD: movups    xmm0,XMMWORD PTR [rbp+0xa0]
00009AE4: mov       QWORD PTR [rbx+0x1a68],rcx
00009AEB: movups    XMMWORD PTR [rbp-0x28],xmm1
00009AEF: mov       QWORD PTR [rbp+0x0],rcx
00009AF3: movups    XMMWORD PTR [rbp-0x18],xmm0
00009AF7: jmp       0x180009afd
00009AF9: movzx     edx,BYTE PTR [rbp-0x40]
00009AFD: mov       rax,rcx
00009B00: cmp       BYTE PTR [rbx+0x1a56],sil
00009B07: jne       0x180009b46
00009B09: cmp       QWORD PTR [rbp+0xb0],rsi
00009B10: je        0x180009b46
00009B12: movups    xmm0,XMMWORD PTR [rbp+0xb0]
00009B19: lea       rax,[rcx+0x1]
00009B1D: mov       BYTE PTR [rbp+0x10],0x1
00009B21: movups    xmm1,XMMWORD PTR [rbp+0xc0]
00009B28: mov       QWORD PTR [rbx+0x1a68],rax
00009B2F: movups    XMMWORD PTR [rbp+0x18],xmm0
00009B33: mov       QWORD PTR [rbp+0x50],rax
00009B37: movups    xmm0,XMMWORD PTR [rbp+0xd0]
00009B3E: movups    XMMWORD PTR [rbp+0x28],xmm1
00009B42: movups    XMMWORD PTR [rbp+0x38],xmm0
00009B46: mov       QWORD PTR [rbp-0x60],rax
00009B4A: mov       r8d,0x2
00009B50: sub       eax,edi
00009B52: movzx     ecx,dl
00009B55: mov       DWORD PTR [rbp-0x50],eax
00009B58: lea       rdx,[rbx+0x1468]
00009B5F: movsxd    rax,DWORD PTR [rbx+0x1a44]
00009B66: add       rcx,rdi
00009B69: add       rax,rax
00009B6C: mov       QWORD PTR [rbx+rax*8+0x1a18],rcx
00009B74: movzx     eax,BYTE PTR [rbx+0x1878]
00009B7B: mov       BYTE PTR [rbx+0x1728],al
00009B81: lea       rax,[rsp+0x50]
00009B86: data16    nop WORD PTR [rax+rax*1+0x0]
00009B90: lea       rdx,[rdx+0x80]
00009B97: movups    xmm0,XMMWORD PTR [rax]
00009B9A: movups    xmm1,XMMWORD PTR [rax+0x10]
00009B9E: lea       rax,[rax+0x80]
00009BA5: movups    XMMWORD PTR [rdx-0x80],xmm0
00009BA9: movups    xmm0,XMMWORD PTR [rax-0x60]
00009BAD: movups    XMMWORD PTR [rdx-0x70],xmm1
00009BB1: movups    xmm1,XMMWORD PTR [rax-0x50]
00009BB5: movups    XMMWORD PTR [rdx-0x60],xmm0
00009BB9: movups    xmm0,XMMWORD PTR [rax-0x40]
00009BBD: movups    XMMWORD PTR [rdx-0x50],xmm1
00009BC1: movups    xmm1,XMMWORD PTR [rax-0x30]
00009BC5: movups    XMMWORD PTR [rdx-0x40],xmm0
00009BC9: movups    xmm0,XMMWORD PTR [rax-0x20]
00009BCD: movups    XMMWORD PTR [rdx-0x30],xmm1
00009BD1: movups    xmm1,XMMWORD PTR [rax-0x10]
00009BD5: movups    XMMWORD PTR [rdx-0x20],xmm0
00009BD9: movups    XMMWORD PTR [rdx-0x10],xmm1
00009BDD: sub       r8,0x1
00009BE1: jne       0x180009b90
00009BE3: movups    xmm0,XMMWORD PTR [rax]
00009BE6: lea       rcx,[rbx+0x8]
00009BEA: movups    XMMWORD PTR [rdx],xmm0
00009BED: call      QWORD PTR [rip+0xfc505]        # 0x1801060f8 ; __imp_LeaveCriticalSection | KERNEL32.dll!LeaveCriticalSection
00009BF3: mov       rcx,QWORD PTR [rbx+0x1710]
00009BFA: call      QWORD PTR [rip+0xfc4d8]        # 0x1801060d8 ; __imp_SetEvent | KERNEL32.dll!SetEvent
00009C00: mov       r8,QWORD PTR [rbx+0x1860]
00009C07: mov       QWORD PTR [rbx+0x1a70],rdi
00009C0E: test      r8,r8
00009C11: je        0x180009c2e
00009C13: mov       rcx,QWORD PTR [rbx+0x16f0]
00009C1A: lea       rax,[rdi-0x1]
00009C1E: test      rdi,rdi
00009C21: cmovne    rsi,rax
00009C25: mov       rax,QWORD PTR [rcx]
00009C28: mov       rdx,rsi
00009C2B: call      QWORD PTR [rax+0x48]
00009C2E: mov       rcx,QWORD PTR [rbp+0xe0]
00009C35: xor       rcx,rsp
00009C38: call      0x180104650 ; __security_check_cookie
00009C3D: lea       r11,[rsp+0x1f0]
00009C45: mov       rbx,QWORD PTR [r11+0x30]
00009C49: mov       rsi,QWORD PTR [r11+0x38]
00009C4D: mov       rsp,r11
00009C50: pop       r14
00009C52: pop       rdi
00009C53: pop       rbp
00009C54: ret       
