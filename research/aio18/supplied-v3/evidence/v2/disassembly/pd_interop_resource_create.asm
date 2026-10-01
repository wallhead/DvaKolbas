; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0x74C60..0x75627; unnamed
00074C60: rex       push rbp
00074C62: push      rbx
00074C63: push      rsi
00074C64: push      rdi
00074C65: push      r12
00074C67: push      r13
00074C69: push      r14
00074C6B: push      r15
00074C6D: lea       rbp,[rsp-0xf8]
00074C75: sub       rsp,0x1f8
00074C7C: mov       rax,QWORD PTR [rip+0x1155d3d]        # 0x1811ca9c0
00074C83: xor       rax,rsp
00074C86: mov       QWORD PTR [rbp+0xe8],rax
00074C8D: mov       r12d,r9d
00074C90: mov       r14,r8
00074C93: mov       rsi,rdx
00074C96: mov       rbx,rcx
00074C99: mov       r13d,DWORD PTR [rbp+0x160]
00074CA0: mov       r15d,DWORD PTR [rbp+0x168]
00074CA7: mov       DWORD PTR [rsp+0x68],r15d
00074CAC: mov       eax,DWORD PTR [rbp+0x178]
00074CB2: mov       DWORD PTR [rsp+0x60],eax
00074CB6: test      rdx,rdx
00074CB9: je        0x180074e96
00074CBF: test      r8,r8
00074CC2: je        0x180074e96
00074CC8: cmp       DWORD PTR [rbp+0x188],0x0
00074CCF: jne       0x180074cde
00074CD1: mov       ecx,eax
00074CD3: call      0x18006ecf0
00074CD8: mov       DWORD PTR [rbp+0x188],eax
00074CDE: cmp       BYTE PTR [rbp+0x180],0x0
00074CE5: je        0x180074dd7
00074CEB: mov       rcx,QWORD PTR [rsi]
00074CEE: xor       edi,edi
00074CF0: test      rcx,rcx
00074CF3: je        0x180074cfe
00074CF5: mov       rax,QWORD PTR [rcx]
00074CF8: call      QWORD PTR [rax+0x10]
00074CFB: mov       QWORD PTR [rsi],rdi
00074CFE: mov       rdx,QWORD PTR [r14]
00074D01: test      rdx,rdx
00074D04: je        0x180074e96
00074D0A: mov       rcx,QWORD PTR [rbx+0x98]
00074D11: mov       rax,QWORD PTR [rcx]
00074D14: lea       r8,[rsp+0x70]
00074D19: mov       QWORD PTR [rsp+0x28],r8
00074D1E: mov       QWORD PTR [rsp+0x20],rdi
00074D23: mov       r9d,0x10000000
00074D29: xor       r8d,r8d
00074D2C: call      QWORD PTR [rax+0xf8]
00074D32: test      eax,eax
00074D34: jns       0x180074d49
00074D36: mov       edx,eax
00074D38: lea       rcx,[rip+0x1128421]        # 0x18119d160 ; 'Create Shared ID3D12Resource Handle Failed! HRESULT: 0x%08x'
00074D3F: call      0x1800fbb40
00074D44: mov       r15,rdi
00074D47: jmp       0x180074d4e
00074D49: mov       r15,QWORD PTR [rsp+0x70]
00074D4E: mov       QWORD PTR [rsp+0x50],rdi
00074D53: mov       r10,QWORD PTR [rbx+0x8]
00074D57: lea       rcx,[rsp+0x50]
00074D5C: call      0x180008420
00074D61: mov       r9,rax
00074D64: mov       rax,QWORD PTR [r10]
00074D67: lea       r8,[rip+0x11274e2]        # 0x18119c250
00074D6E: mov       rdx,r15
00074D71: mov       rcx,r10
00074D74: call      QWORD PTR [rax+0x180]
00074D7A: lea       rdx,[rip+0x1128447]        # 0x18119d1c8 ; 'DX11 OpenSharedHandle Success : 0x%08x'
00074D81: lea       rcx,[rip+0x1128418]        # 0x18119d1a0 ; 'DX11 OpenSharedHandle Failed : 0x%08x'
00074D88: test      eax,eax
00074D8A: cmovns    rcx,rdx
00074D8E: mov       edx,eax
00074D90: call      0x1800fbb40
00074D95: mov       rax,QWORD PTR [rsp+0x50]
00074D9A: mov       QWORD PTR [rsi],rax
00074D9D: test      r15,r15
00074DA0: je        0x180074dab
00074DA2: mov       rcx,r15
00074DA5: call      QWORD PTR [rip+0x9e315]        # 0x1801130c0 ; KERNEL32.dll!CloseHandle
00074DAB: mov       rdx,QWORD PTR [rsi]
00074DAE: test      rdx,rdx
00074DB1: je        0x180074dbf
00074DB3: mov       r8,QWORD PTR [r14]
00074DB6: test      r8,r8
00074DB9: jne       0x18007560d
00074DBF: movzx     edx,BYTE PTR [rbx+0x924]
00074DC6: lea       rcx,[rip+0x1129f13]        # 0x18119ece0 ; 'CreateShareTexture: opening D3D12 resource on D3D11 failed (crossAdapter=%d)'
00074DCD: call      0x1800fbb40
00074DD2: jmp       0x180074e96
00074DD7: mov       r8,QWORD PTR [r8]
00074DDA: mov       rdx,QWORD PTR [rsi]
00074DDD: mov       rcx,rbx
00074DE0: call      0x180073cb0
00074DE5: mov       rcx,QWORD PTR [rsi]
00074DE8: xor       edi,edi
00074DEA: test      rcx,rcx
00074DED: je        0x180074df8
00074DEF: mov       rax,QWORD PTR [rcx]
00074DF2: call      QWORD PTR [rax+0x10]
00074DF5: mov       QWORD PTR [rsi],rdi
00074DF8: mov       rdx,QWORD PTR [r14]
00074DFB: mov       rcx,rbx
00074DFE: call      0x180074a60
00074E03: mov       rcx,QWORD PTR [r14]
00074E06: test      rcx,rcx
00074E09: je        0x180074e14
00074E0B: mov       rax,QWORD PTR [rcx]
00074E0E: call      QWORD PTR [rax+0x10]
00074E11: mov       QWORD PTR [r14],rdi
00074E14: cmp       BYTE PTR [rbx+0x925],dil
00074E1B: je        0x180074ffd
00074E21: cmp       QWORD PTR [rbx+0xb18],rdi
00074E28: je        0x180074ffd
00074E2E: mov       eax,DWORD PTR [rbp+0x188]
00074E34: test      al,0xc
00074E36: je        0x180074ffd
00074E3C: test      al,0x3
00074E3E: jne       0x180074ffd
00074E44: cmp       DWORD PTR [rsp+0x60],0x1
00074E49: sete      al
00074E4C: mov       BYTE PTR [rsp+0x38],al
00074E50: mov       BYTE PTR [rsp+0x30],0x1
00074E55: mov       DWORD PTR [rsp+0x28],r15d
00074E5A: movzx     eax,BYTE PTR [rbp+0x170]
00074E61: mov       BYTE PTR [rsp+0x20],al
00074E65: mov       r9b,0x1
00074E68: mov       r8d,r13d
00074E6B: mov       edx,r12d
00074E6E: mov       rcx,rbx
00074E71: call      0x18006d4b0
00074E76: mov       rcx,rax
00074E79: mov       QWORD PTR [rsi],rax
00074E7C: test      rax,rax
00074E7F: jne       0x180074ebb
00074E81: mov       r9d,r15d
00074E84: mov       r8d,r13d
00074E87: mov       edx,r12d
00074E8A: lea       rcx,[rip+0x1129e9f]        # 0x18119ed30 ; 'CreateShareTexture: failed to create local NR D3D11 staging %dx%d fmt=%d'
00074E91: call      0x1800fbb40
00074E96: xor       al,al
00074E98: mov       rcx,QWORD PTR [rbp+0xe8]
00074E9F: xor       rcx,rsp
00074EA2: call      0x18010c270
00074EA7: add       rsp,0x1f8
00074EAE: pop       r15
00074EB0: pop       r14
00074EB2: pop       r13
00074EB4: pop       r12
00074EB6: pop       rdi
00074EB7: pop       rsi
00074EB8: pop       rbx
00074EB9: pop       rbp
00074EBA: ret       
00074EBB: mov       QWORD PTR [rsp+0x58],rdi
00074EC0: mov       QWORD PTR [rsp+0x50],rdi
00074EC5: mov       rax,QWORD PTR [rax]
00074EC8: lea       r8,[rsp+0x50]
00074ECD: lea       rdx,[rip+0x112a484]        # 0x18119f358
00074ED4: call      QWORD PTR [rax]
00074ED6: test      eax,eax
00074ED8: jns       0x180074ef8
00074EDA: mov       edx,eax
00074EDC: lea       rcx,[rip+0x11281fd]        # 0x18119d0e0 ; 'QueryInterface failed : 0x%08x'
00074EE3: call      0x1800fbb40
00074EE8: xor       edx,edx
00074EEA: mov       rcx,QWORD PTR [rbx+0xb18]
00074EF1: call      0x18006e850
00074EF6: jmp       0x180074f74
00074EF8: mov       rcx,QWORD PTR [rsp+0x50]
00074EFD: mov       rax,QWORD PTR [rcx]
00074F00: lea       rdx,[rsp+0x58]
00074F05: mov       QWORD PTR [rsp+0x20],rdx
00074F0A: xor       r9d,r9d
00074F0D: xor       edx,edx
00074F0F: mov       r8d,0x80000001
00074F15: call      QWORD PTR [rax+0x68]
00074F18: mov       rcx,QWORD PTR [rsp+0x50]
00074F1D: mov       rax,QWORD PTR [rcx]
00074F20: call      QWORD PTR [rax+0x10]
00074F23: mov       rax,QWORD PTR [rsp+0x58]
00074F28: mov       QWORD PTR [rsp+0x68],rax
00074F2D: test      rax,rax
00074F30: jne       0x180074f48
00074F32: lea       rcx,[rip+0x11281f7]        # 0x18119d130 ; 'CreateShareHandle failed with null handle'
00074F39: call      0x1800fbb40
00074F3E: mov       rax,QWORD PTR [rsp+0x58]
00074F43: mov       QWORD PTR [rsp+0x68],rax
00074F48: mov       rdx,rax
00074F4B: mov       rcx,QWORD PTR [rbx+0xb18]
00074F52: call      0x18006e850
00074F57: mov       QWORD PTR [rsp+0x70],rax
00074F5C: mov       rdx,rax
00074F5F: mov       rcx,QWORD PTR [rsp+0x68]
00074F64: test      rcx,rcx
00074F67: je        0x180074f77
00074F69: call      QWORD PTR [rip+0x9e151]        # 0x1801130c0 ; KERNEL32.dll!CloseHandle
00074F6F: mov       rax,QWORD PTR [rsp+0x70]
00074F74: mov       rdx,rax
00074F77: mov       QWORD PTR [r14],rax
00074F7A: cmp       QWORD PTR [rsi],rdi
00074F7D: je        0x180074fc6
00074F7F: test      rax,rax
00074F82: je        0x180074fc6
00074F84: xor       r8d,r8d
00074F87: mov       rdx,rax
00074F8A: mov       rcx,rbx
00074F8D: call      0x180075700
00074F92: mov       r8,QWORD PTR [r14]
00074F95: mov       rdx,QWORD PTR [rsi]
00074F98: mov       rcx,rbx
00074F9B: call      0x180073c40
00074FA0: mov       eax,DWORD PTR [rbp+0x188]
00074FA6: mov       DWORD PTR [rsp+0x20],eax
00074FAA: mov       r9d,r15d
00074FAD: mov       r8d,r13d
00074FB0: mov       edx,r12d
00074FB3: lea       rcx,[rip+0x1129dc6]        # 0x18119ed80 ; 'CreateShareTexture: local-only NR share %dx%d fmt=%d kind=0x%x'
00074FBA: call      0x1800fbb40
00074FBF: mov       al,0x1
00074FC1: jmp       0x180074e98
00074FC6: test      rdx,rdx
00074FC9: je        0x180074fd7
00074FCB: mov       rax,QWORD PTR [rdx]
00074FCE: mov       rcx,rdx
00074FD1: call      QWORD PTR [rax+0x10]
00074FD4: mov       QWORD PTR [r14],rdi
00074FD7: mov       rcx,QWORD PTR [rsi]
00074FDA: test      rcx,rcx
00074FDD: je        0x180074fe8
00074FDF: mov       rax,QWORD PTR [rcx]
00074FE2: call      QWORD PTR [rax+0x10]
00074FE5: mov       QWORD PTR [rsi],rdi
00074FE8: mov       r9d,r15d
00074FEB: mov       r8d,r13d
00074FEE: mov       edx,r12d
00074FF1: lea       rcx,[rip+0x1129dc8]        # 0x18119edc0 ; 'CreateShareTexture: local-only NR share failed %dx%d fmt=%d'
00074FF8: jmp       0x180074e91
00074FFD: cmp       BYTE PTR [rbx+0x924],dil
00075004: je        0x1800754d7
0007500A: lea       rax,[rbx+0xa28]
00075011: mov       QWORD PTR [rbp-0x80],rax
00075015: mov       BYTE PTR [rbp-0x78],dil
00075019: test      rax,rax
0007501C: je        0x18007561c
00075022: mov       rcx,rax
00075025: call      QWORD PTR [rip+0x9e50d]        # 0x180113538 ; MSVCP140.dll!_Mtx_lock
0007502B: test      eax,eax
0007502D: je        0x18007503b
0007502F: mov       ecx,0x5
00075034: call      QWORD PTR [rip+0x9e506]        # 0x180113540 ; MSVCP140.dll!?_Throw_Cpp_error@std@@YAXH@Z
0007503A: int3      
0007503B: cmp       DWORD PTR [rbx+0xa74],0x7fffffff
00075045: jne       0x18007505d
00075047: mov       DWORD PTR [rbx+0xa74],0x7ffffffe
00075051: mov       ecx,0x6
00075056: call      QWORD PTR [rip+0x9e4e4]        # 0x180113540 ; MSVCP140.dll!?_Throw_Cpp_error@std@@YAXH@Z
0007505C: int3      
0007505D: mov       BYTE PTR [rbp-0x78],0x1
00075061: cmp       QWORD PTR [rbx+0xb18],0x0
00075069: je        0x1800754b7
0007506F: cmp       QWORD PTR [rbx+0xb20],0x0
00075077: je        0x1800754b7
0007507D: cmp       DWORD PTR [rsp+0x60],0x1
00075082: sete      al
00075085: mov       BYTE PTR [rsp+0x38],al
00075089: mov       BYTE PTR [rsp+0x30],0x1
0007508E: mov       DWORD PTR [rsp+0x28],r15d
00075093: movzx     eax,BYTE PTR [rbp+0x170]
0007509A: mov       BYTE PTR [rsp+0x20],al
0007509E: mov       r9b,0x1
000750A1: mov       r8d,r13d
000750A4: mov       edx,r12d
000750A7: mov       rcx,rbx
000750AA: call      0x18006d4b0
000750AF: mov       rcx,rax
000750B2: mov       QWORD PTR [rsi],rax
000750B5: test      rax,rax
000750B8: jne       0x1800750d4
000750BA: mov       r9d,r15d
000750BD: mov       r8d,r13d
000750C0: mov       edx,r12d
000750C3: lea       rcx,[rip+0x1129d86]        # 0x18119ee50 ; 'CreateShareTexture: failed to create GPU1 D3D11 NT-shared staging %dx%d fmt=%d'
000750CA: call      0x1800fbb40
000750CF: jmp       0x1800754c3
000750D4: mov       QWORD PTR [rsp+0x60],rdi
000750D9: mov       QWORD PTR [rsp+0x58],rdi
000750DE: mov       rax,QWORD PTR [rax]
000750E1: lea       r8,[rsp+0x58]
000750E6: lea       rdx,[rip+0x112a26b]        # 0x18119f358
000750ED: call      QWORD PTR [rax]
000750EF: test      eax,eax
000750F1: jns       0x180075116
000750F3: mov       edx,eax
000750F5: lea       rcx,[rip+0x1127fe4]        # 0x18119d0e0 ; 'QueryInterface failed : 0x%08x'
000750FC: call      0x1800fbb40
00075101: xor       edx,edx
00075103: mov       rcx,QWORD PTR [rbx+0xb18]
0007510A: call      0x18006e850
0007510F: mov       QWORD PTR [rsp+0x50],rax
00075114: jmp       0x18007518a
00075116: mov       rcx,QWORD PTR [rsp+0x58]
0007511B: mov       rax,QWORD PTR [rcx]
0007511E: lea       rdx,[rsp+0x60]
00075123: mov       QWORD PTR [rsp+0x20],rdx
00075128: xor       r9d,r9d
0007512B: xor       edx,edx
0007512D: mov       r8d,0x80000001
00075133: call      QWORD PTR [rax+0x68]
00075136: mov       rcx,QWORD PTR [rsp+0x58]
0007513B: mov       rax,QWORD PTR [rcx]
0007513E: call      QWORD PTR [rax+0x10]
00075141: mov       rax,QWORD PTR [rsp+0x60]
00075146: mov       QWORD PTR [rsp+0x70],rax
0007514B: test      rax,rax
0007514E: jne       0x180075166
00075150: lea       rcx,[rip+0x1127fd9]        # 0x18119d130 ; 'CreateShareHandle failed with null handle'
00075157: call      0x1800fbb40
0007515C: mov       rax,QWORD PTR [rsp+0x60]
00075161: mov       QWORD PTR [rsp+0x70],rax
00075166: mov       rdx,rax
00075169: mov       rcx,QWORD PTR [rbx+0xb18]
00075170: call      0x18006e850
00075175: mov       QWORD PTR [rsp+0x50],rax
0007517A: mov       rcx,QWORD PTR [rsp+0x70]
0007517F: test      rcx,rcx
00075182: je        0x18007518a
00075184: call      QWORD PTR [rip+0x9df36]        # 0x1801130c0 ; KERNEL32.dll!CloseHandle
0007518A: mov       BYTE PTR [rsp+0x28],0x0
0007518F: movzx     eax,BYTE PTR [rbp+0x170]
00075196: mov       BYTE PTR [rsp+0x20],al
0007519A: mov       r9d,r15d
0007519D: mov       r8d,r13d
000751A0: mov       edx,r12d
000751A3: mov       rcx,rbx
000751A6: call      0x18006d390
000751AB: mov       QWORD PTR [r14],rax
000751AE: mov       rax,QWORD PTR [rsp+0x50]
000751B3: test      rax,rax
000751B6: je        0x18007547e
000751BC: mov       rdx,rax
000751BF: mov       rcx,QWORD PTR [rbx+0xb18]
000751C6: call      0x18006e910
000751CB: mov       QWORD PTR [rsp+0x78],rax
000751D0: test      rax,rax
000751D3: je        0x180075473
000751D9: mov       r8,QWORD PTR [rax]
000751DC: mov       rcx,rax
000751DF: cmp       QWORD PTR [r14],0x0
000751E3: je        0x18007546f
000751E9: mov       QWORD PTR [rsp+0x58],rdi
000751EE: xorps     xmm0,xmm0
000751F1: movups    XMMWORD PTR [rbp+0x90],xmm0
000751F8: movups    XMMWORD PTR [rbp+0xa0],xmm0
000751FF: mov       DWORD PTR [rsp+0x60],edi
00075203: mov       QWORD PTR [rsp+0x70],rdi
00075208: lea       rdx,[rbp+0xb0]
0007520F: call      QWORD PTR [r8+0x50]
00075213: mov       rcx,QWORD PTR [rbx+0xb18]
0007521A: mov       rax,QWORD PTR [rcx]
0007521D: lea       rdx,[rsp+0x58]
00075222: mov       QWORD PTR [rsp+0x40],rdx
00075227: lea       rdx,[rsp+0x70]
0007522C: mov       QWORD PTR [rsp+0x38],rdx
00075231: lea       rdx,[rsp+0x60]
00075236: mov       QWORD PTR [rsp+0x30],rdx
0007523B: lea       rdx,[rbp+0x90]
00075242: mov       QWORD PTR [rsp+0x28],rdx
00075247: mov       QWORD PTR [rsp+0x20],rdi
0007524C: mov       r9d,0x1
00075252: xor       r8d,r8d
00075255: lea       rdx,[rbp+0xb0]
0007525C: call      QWORD PTR [rax+0x130]
00075262: xorps     xmm0,xmm0
00075265: xor       eax,eax
00075267: movups    XMMWORD PTR [rbp-0x58],xmm0
0007526B: mov       QWORD PTR [rbp-0x48],rax
0007526F: xorps     xmm1,xmm1
00075272: movups    XMMWORD PTR [rbp-0x40],xmm1
00075276: mov       QWORD PTR [rbp-0x30],rax
0007527A: movups    XMMWORD PTR [rbp-0x28],xmm0
0007527E: mov       QWORD PTR [rbp-0x18],rax
00075282: movups    XMMWORD PTR [rbp-0x10],xmm1
00075286: mov       QWORD PTR [rbp+0x0],rax
0007528A: movups    XMMWORD PTR [rbp+0x8],xmm0
0007528E: movups    XMMWORD PTR [rbp+0x18],xmm0
00075292: mov       QWORD PTR [rbp+0x38],rdi
00075296: mov       DWORD PTR [rbp+0x40],edi
00075299: mov       BYTE PTR [rbp+0x44],al
0007529C: mov       QWORD PTR [rbp+0x48],rdi
000752A0: movups    XMMWORD PTR [rbp+0x50],xmm0
000752A4: mov       QWORD PTR [rbp+0x60],rax
000752A8: mov       QWORD PTR [rbp+0x68],rax
000752AC: mov       QWORD PTR [rbp+0x70],rax
000752B0: movups    XMMWORD PTR [rbp+0x78],xmm0
000752B4: mov       QWORD PTR [rbp+0x88],rax
000752BB: mov       rcx,QWORD PTR [rsp+0x50]
000752C0: mov       QWORD PTR [rbp-0x70],rcx
000752C4: mov       rax,QWORD PTR [rsp+0x78]
000752C9: mov       QWORD PTR [rbp-0x68],rax
000752CD: mov       rax,QWORD PTR [r14]
000752D0: mov       QWORD PTR [rbp-0x60],rax
000752D4: movups    xmm0,XMMWORD PTR [rbp+0x90]
000752DB: movups    XMMWORD PTR [rbp+0x8],xmm0
000752DF: movups    xmm1,XMMWORD PTR [rbp+0xa0]
000752E6: movups    XMMWORD PTR [rbp+0x18],xmm1
000752EA: mov       r8,QWORD PTR [rsp+0x58]
000752EF: mov       QWORD PTR [rbp+0x28],r8
000752F3: mov       eax,DWORD PTR [rbp+0x188]
000752F9: mov       DWORD PTR [rbp+0x30],eax
000752FC: mov       eax,DWORD PTR [rbp+0x190]
00075302: mov       DWORD PTR [rbp+0x34],eax
00075305: mov       QWORD PTR [rbp+0x68],0xffffffffffffffff
0007530D: mov       DWORD PTR [rbp+0x70],0xffffffff
00075314: mov       r15d,edi
00075317: nop       WORD PTR [rax+rax*1+0x0]
00075320: mov       eax,r15d
00075323: lea       rcx,[rbp-0x40]
00075327: lea       rcx,[rcx+rax*8]
0007532B: lea       rdx,[rbp-0x58]
0007532F: lea       rdx,[rdx+rax*8]
00075333: lea       r10,[rbp-0x10]
00075337: lea       r10,[r10+rax*8]
0007533B: lea       r9,[rbp-0x28]
0007533F: lea       r9,[r9+rax*8]
00075343: mov       QWORD PTR [rsp+0x30],rcx
00075348: mov       QWORD PTR [rsp+0x28],rdx
0007534D: mov       QWORD PTR [rsp+0x20],r10
00075352: mov       rdx,QWORD PTR [rbx+0x98]
00075359: mov       rcx,QWORD PTR [rbx+0xb18]
00075360: call      0x18006ea30 ; '@USVWATAUAVAWH'
00075365: test      al,al
00075367: mov       eax,DWORD PTR [rbp+0x38]
0007536A: je        0x180075381
0007536C: inc       eax
0007536E: mov       DWORD PTR [rbp+0x38],eax
00075371: inc       r15d
00075374: cmp       r15d,0x3
00075378: jge       0x180075381
0007537A: mov       r8,QWORD PTR [rsp+0x58]
0007537F: jmp       0x180075320
00075381: test      eax,eax
00075383: jle       0x18007545f
00075389: mov       rdx,QWORD PTR [rbx+0xc98]
00075390: cmp       rdx,QWORD PTR [rbx+0xca0]
00075397: je        0x180075401
00075399: lea       rax,[rbp-0x70]
0007539D: mov       ecx,0x2
000753A2: movups    xmm0,XMMWORD PTR [rax]
000753A5: movups    XMMWORD PTR [rdx],xmm0
000753A8: movups    xmm1,XMMWORD PTR [rax+0x10]
000753AC: movups    XMMWORD PTR [rdx+0x10],xmm1
000753B0: movups    xmm0,XMMWORD PTR [rax+0x20]
000753B4: movups    XMMWORD PTR [rdx+0x20],xmm0
000753B8: movups    xmm1,XMMWORD PTR [rax+0x30]
000753BC: movups    XMMWORD PTR [rdx+0x30],xmm1
000753C0: movups    xmm0,XMMWORD PTR [rax+0x40]
000753C4: movups    XMMWORD PTR [rdx+0x40],xmm0
000753C8: movups    xmm1,XMMWORD PTR [rax+0x50]
000753CC: movups    XMMWORD PTR [rdx+0x50],xmm1
000753D0: movups    xmm0,XMMWORD PTR [rax+0x60]
000753D4: movups    XMMWORD PTR [rdx+0x60],xmm0
000753D8: lea       rdx,[rdx+0x80]
000753DF: movups    xmm1,XMMWORD PTR [rax+0x70]
000753E3: movups    XMMWORD PTR [rdx-0x10],xmm1
000753E7: lea       rax,[rax+0x80]
000753EE: sub       rcx,0x1
000753F2: jne       0x1800753a2
000753F4: add       QWORD PTR [rbx+0xc98],0x100
000753FF: jmp       0x180075411
00075401: lea       r8,[rbp-0x70]
00075405: lea       rcx,[rbx+0xc90]
0007540C: call      0x180078520
00075411: mov       r8,QWORD PTR [r14]
00075414: mov       rdx,QWORD PTR [rsi]
00075417: mov       rcx,rbx
0007541A: call      0x180073c40
0007541F: mov       eax,DWORD PTR [rbp+0x38]
00075422: mov       DWORD PTR [rsp+0x38],eax
00075426: mov       eax,DWORD PTR [rbp+0x190]
0007542C: mov       DWORD PTR [rsp+0x30],eax
00075430: mov       eax,DWORD PTR [rbp+0x188]
00075436: mov       DWORD PTR [rsp+0x28],eax
0007543A: mov       rax,QWORD PTR [rsp+0x58]
0007543F: mov       QWORD PTR [rsp+0x20],rax
00075444: mov       r9d,DWORD PTR [rsp+0x68]
00075449: mov       r8d,r13d
0007544C: mov       edx,r12d
0007544F: lea       rcx,[rip+0x1129a4a]        # 0x18119eea0 ; 'CreateShareTexture: cross-adapter buffer bounce %dx%d fmt=%d size=%llu kind=0x%x bb=%d ring=%d'
00075456: call      0x1800fbb40
0007545B: mov       bl,0x1
0007545D: jmp       0x1800754c5
0007545F: lea       rcx,[rbp-0x70]
00075463: call      0x18006ed50
00075468: mov       r15d,DWORD PTR [rsp+0x68]
0007546D: jmp       0x18007547e
0007546F: call      QWORD PTR [r8+0x10]
00075473: mov       rcx,QWORD PTR [rsp+0x50]
00075478: mov       rax,QWORD PTR [rcx]
0007547B: call      QWORD PTR [rax+0x10]
0007547E: mov       rcx,QWORD PTR [r14]
00075481: test      rcx,rcx
00075484: je        0x18007548f
00075486: mov       rax,QWORD PTR [rcx]
00075489: call      QWORD PTR [rax+0x10]
0007548C: mov       QWORD PTR [r14],rdi
0007548F: mov       rcx,QWORD PTR [rsi]
00075492: test      rcx,rcx
00075495: je        0x1800754a0
00075497: mov       rax,QWORD PTR [rcx]
0007549A: call      QWORD PTR [rax+0x10]
0007549D: mov       QWORD PTR [rsi],rdi
000754A0: mov       r9d,r15d
000754A3: mov       r8d,r13d
000754A6: mov       edx,r12d
000754A9: lea       rcx,[rip+0x1129a50]        # 0x18119ef00 ; 'CreateShareTexture: cross-adapter buffer bounce failed %dx%d fmt=%d'
000754B0: call      0x1800fbb40
000754B5: jmp       0x1800754c3
000754B7: lea       rcx,[rip+0x1129942]        # 0x18119ee00 ; 'CreateShareTexture: cross-adapter bounce requires GPU1 companion D3D12 device.'
000754BE: call      0x1800fbb40
000754C3: xor       bl,bl
000754C5: mov       rcx,QWORD PTR [rbp-0x80]
000754C9: call      QWORD PTR [rip+0x9e059]        # 0x180113528 ; MSVCP140.dll!_Mtx_unlock
000754CF: movzx     eax,bl
000754D2: jmp       0x180074e98
000754D7: cmp       DWORD PTR [rsp+0x60],0x1
000754DC: sete      al
000754DF: mov       BYTE PTR [rsp+0x38],al
000754E3: mov       BYTE PTR [rsp+0x30],0x1
000754E8: mov       DWORD PTR [rsp+0x28],r15d
000754ED: movzx     eax,BYTE PTR [rbp+0x170]
000754F4: mov       BYTE PTR [rsp+0x20],al
000754F8: xor       r9d,r9d
000754FB: mov       r8d,r13d
000754FE: mov       edx,r12d
00075501: mov       rcx,rbx
00075504: call      0x18006d4b0
00075509: mov       rcx,rax
0007550C: mov       QWORD PTR [rsi],rax
0007550F: test      rax,rax
00075512: jne       0x180075528
00075514: lea       rcx,[rip+0x1127b95]        # 0x18119d0b0 ; 'CreateShareHandle failed with null texture'
0007551B: call      0x1800fbb40
00075520: mov       rdx,rdi
00075523: jmp       0x1800755c3
00075528: mov       QWORD PTR [rsp+0x68],rdi
0007552D: mov       QWORD PTR [rsp+0x50],rdi
00075532: mov       rax,QWORD PTR [rax]
00075535: lea       r8,[rsp+0x50]
0007553A: lea       rdx,[rip+0x1129e17]        # 0x18119f358
00075541: call      QWORD PTR [rax]
00075543: test      eax,eax
00075545: jns       0x18007555a
00075547: mov       edx,eax
00075549: lea       rcx,[rip+0x1127b90]        # 0x18119d0e0 ; 'QueryInterface failed : 0x%08x'
00075550: call      0x1800fbb40
00075555: mov       rdx,rdi
00075558: jmp       0x1800755c3
0007555A: mov       rcx,QWORD PTR [rsp+0x50]
0007555F: mov       rax,QWORD PTR [rcx]
00075562: lea       rdx,[rsp+0x68]
00075567: call      QWORD PTR [rax+0x40]
0007556A: cmp       QWORD PTR [rsp+0x68],rdi
0007556F: jne       0x18007559d
00075571: lea       rcx,[rip+0x1127b88]        # 0x18119d100 ; 'CreateShareHandle failed, trying NTHandle'
00075578: call      0x1800fbb40
0007557D: mov       rcx,QWORD PTR [rsp+0x50]
00075582: mov       rax,QWORD PTR [rcx]
00075585: lea       rdx,[rsp+0x68]
0007558A: mov       QWORD PTR [rsp+0x20],rdx
0007558F: xor       r9d,r9d
00075592: xor       edx,edx
00075594: mov       r8d,0x80000001
0007559A: call      QWORD PTR [rax+0x68]
0007559D: mov       rcx,QWORD PTR [rsp+0x50]
000755A2: mov       rax,QWORD PTR [rcx]
000755A5: call      QWORD PTR [rax+0x10]
000755A8: mov       rdx,QWORD PTR [rsp+0x68]
000755AD: test      rdx,rdx
000755B0: jne       0x1800755c3
000755B2: lea       rcx,[rip+0x1127b77]        # 0x18119d130 ; 'CreateShareHandle failed with null handle'
000755B9: call      0x1800fbb40
000755BE: mov       rdx,QWORD PTR [rsp+0x68]
000755C3: mov       rcx,QWORD PTR [rbx+0x98]
000755CA: test      rcx,rcx
000755CD: jne       0x1800755db
000755CF: mov       rcx,QWORD PTR [rbx+0xb18]
000755D6: test      rcx,rcx
000755D9: je        0x1800755e3
000755DB: call      0x18006e850
000755E0: mov       rdi,rax
000755E3: mov       QWORD PTR [r14],rdi
000755E6: cmp       QWORD PTR [rsi],0x0
000755EA: je        0x180074e96
000755F0: test      rdi,rdi
000755F3: je        0x180074e96
000755F9: xor       r8d,r8d
000755FC: mov       rdx,rdi
000755FF: mov       rcx,rbx
00075602: call      0x180075700
00075607: mov       r8,QWORD PTR [r14]
0007560A: mov       rdx,QWORD PTR [rsi]
0007560D: mov       rcx,rbx
00075610: call      0x180073c40
00075615: mov       al,0x1
00075617: jmp       0x180074e98
0007561C: mov       ecx,0x1
00075621: call      0x18006cfd0
00075626: int3      
