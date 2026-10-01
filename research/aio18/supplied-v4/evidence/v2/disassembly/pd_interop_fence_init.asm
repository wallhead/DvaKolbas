; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0x72810..0x72DCD; unnamed
00072810: rex       push rsi
00072812: push      rdi
00072813: push      r13
00072815: push      r15
00072817: sub       rsp,0x78
0007281B: mov       rax,QWORD PTR [rip+0x115819e]        # 0x1811ca9c0
00072822: xor       rax,rsp
00072825: mov       QWORD PTR [rsp+0x58],rax
0007282A: movsxd    r15,edx
0007282D: mov       rdi,rcx
00072830: mov       BYTE PTR [rsp+0x40],r8b
00072835: lea       rax,[r15+r15*2]
00072839: lea       r13,[rcx+rax*8]
0007283D: cmp       QWORD PTR [r13+0x118],0x0
00072845: je        0x180072860
00072847: cmp       QWORD PTR [r13+0xb8],0x0
0007284F: je        0x180072860
00072851: cmp       QWORD PTR [rcx+r15*8+0x178],0x0
0007285A: jne       0x180072db5
00072860: mov       QWORD PTR [rsp+0xb0],rbx
00072868: mov       edx,r15d
0007286B: mov       QWORD PTR [rsp+0x70],rbp
00072870: mov       QWORD PTR [rsp+0x68],r12
00072875: mov       QWORD PTR [rsp+0x60],r14
0007287A: call      0x180072dd0
0007287F: xor       r9d,r9d
00072882: xor       r8d,r8d
00072885: xor       edx,edx
00072887: xor       ecx,ecx
00072889: call      QWORD PTR [rip+0xa0931]        # 0x1801131c0 ; KERNEL32.dll!CreateEventW
0007288F: lea       r12,[r15+r15*2]
00072893: mov       QWORD PTR [rdi+r15*8+0x40],rax
00072898: lea       r14,[rdi+0x118]
0007289F: xor       ebp,ebp
000728A1: lea       r14,[r14+r12*8]
000728A5: data16    data16 nop WORD PTR [rax+rax*1+0x0]
000728B0: mov       r10,QWORD PTR [rdi+0x98]
000728B7: lea       rcx,[rdi+0xb8]
000728BE: mov       ebx,ebp
000728C0: add       rbx,r12
000728C3: lea       rcx,[rcx+rbx*8]
000728C7: call      0x180008420
000728CC: mov       r9,rax
000728CF: lea       r8,[rip+0x112c8aa]        # 0x18119f180
000728D6: mov       rax,QWORD PTR [r10]
000728D9: xor       edx,edx
000728DB: mov       rcx,r10
000728DE: call      QWORD PTR [rax+0x48]
000728E1: test      eax,eax
000728E3: js        0x180072d89
000728E9: mov       r9,QWORD PTR [r14-0x60]
000728ED: test      r9,r9
000728F0: je        0x180072d89
000728F6: mov       r11,QWORD PTR [rdi+0x98]
000728FD: lea       rcx,[rdi+0x118]
00072904: lea       rcx,[rcx+rbx*8]
00072908: call      0x180008420
0007290D: mov       rdx,QWORD PTR [r11]
00072910: xor       r8d,r8d
00072913: mov       QWORD PTR [rsp+0x30],rax
00072918: mov       rcx,r11
0007291B: lea       rax,[rip+0x112c81e]        # 0x18119f140
00072922: mov       QWORD PTR [rsp+0x28],rax
00072927: mov       r10,QWORD PTR [rdx+0x60]
0007292B: xor       edx,edx
0007292D: mov       QWORD PTR [rsp+0x20],0x0
00072936: call      r10
00072939: test      eax,eax
0007293B: js        0x180072d80
00072941: cmp       QWORD PTR [r14],0x0
00072945: je        0x180072d80
0007294B: mov       ecx,r15d
0007294E: call      0x1800727d0
00072953: mov       rcx,QWORD PTR [r14-0x60]
00072957: mov       rbx,rax
0007295A: mov       rdx,QWORD PTR [rcx]
0007295D: mov       r8,QWORD PTR [rdx+0x30]
00072961: mov       rdx,rax
00072964: call      r8
00072967: mov       rcx,QWORD PTR [r14]
0007296A: mov       rdx,QWORD PTR [rcx]
0007296D: mov       r8,QWORD PTR [rdx+0x30]
00072971: mov       rdx,rbx
00072974: call      r8
00072977: mov       rcx,QWORD PTR [r14]
0007297A: mov       rax,QWORD PTR [rcx]
0007297D: call      QWORD PTR [rax+0x48]
00072980: inc       ebp
00072982: add       r14,0x8
00072986: cmp       ebp,0x3
00072989: jl        0x1800728b0
0007298F: mov       BYTE PTR [r15+rdi*1+0x1f8],0x0
00072998: lea       rax,[r15+r15*2]
0007299C: xor       r12d,r12d
0007299F: xor       r9d,r9d
000729A2: xor       r8d,r8d
000729A5: mov       QWORD PTR [rdi+rax*8+0x258],r12
000729AD: xor       edx,edx
000729AF: mov       QWORD PTR [r13+0x250],r12
000729B6: xor       ecx,ecx
000729B8: mov       QWORD PTR [r13+0x248],r12
000729BF: call      QWORD PTR [rip+0xa07fb]        # 0x1801131c0 ; KERNEL32.dll!CreateEventW
000729C5: movzx     r14d,BYTE PTR [rsp+0x40]
000729CB: mov       r13d,0x3
000729D1: mov       QWORD PTR [rdi+r15*8+0x198],rax
000729D9: mov       eax,0x1
000729DE: test      r14b,r14b
000729E1: je        0x180072b21
000729E7: cmp       QWORD PTR [rdi+0x8],r12
000729EB: je        0x180072b21
000729F1: cmp       BYTE PTR [rdi+0x924],r12b
000729F8: lea       rbp,[rdi+r15*8]
000729FC: mov       r11,QWORD PTR [rdi+0x98]
00072A03: lea       rcx,[rbp+0x178]
00072A0A: mov       ebx,r13d
00072A0D: cmove     ebx,eax
00072A10: call      0x180008420
00072A15: mov       rdx,QWORD PTR [r11]
00072A18: lea       r9,[rip+0x112c6b9]        # 0x18119f0d8
00072A1F: mov       r8d,ebx
00072A22: mov       QWORD PTR [rsp+0x20],rax
00072A27: mov       rcx,r11
00072A2A: mov       r10,QWORD PTR [rdx+0x120]
00072A31: xor       edx,edx
00072A33: call      r10
00072A36: test      eax,eax
00072A38: jns       0x180072a4b
00072A3A: mov       r8d,eax
00072A3D: lea       rcx,[rip+0x112c054]        # 0x18119ea98 ; 'Failed to CreateFence shared (flags=0x%x) 0x%08x'
00072A44: mov       edx,ebx
00072A46: call      0x1800fbb40
00072A4B: mov       rcx,QWORD PTR [rdi+0x98]
00072A52: lea       rdx,[rsp+0x48]
00072A57: mov       QWORD PTR [rsp+0x28],rdx
00072A5C: mov       r9d,0x10000000
00072A62: mov       rdx,QWORD PTR [rdi+r15*8+0x178]
00072A6A: xor       r8d,r8d
00072A6D: mov       QWORD PTR [rsp+0x20],r12
00072A72: mov       rax,QWORD PTR [rcx]
00072A75: call      QWORD PTR [rax+0xf8]
00072A7B: test      eax,eax
00072A7D: jns       0x180072a8d
00072A7F: mov       edx,eax
00072A81: lea       rcx,[rip+0x112c048]        # 0x18119ead0 ; 's_D3D12Device->CreateSharedHandle Failed ErrorCode: 0x%08x'
00072A88: call      0x1800fbb40
00072A8D: mov       r10,QWORD PTR [rdi+0x8]
00072A91: lea       rcx,[rbp+0x20]
00072A95: call      0x180008420
00072A9A: mov       rdx,QWORD PTR [rsp+0x48]
00072A9F: lea       r8,[rip+0x112c652]        # 0x18119f0f8
00072AA6: mov       r9,rax
00072AA9: mov       rcx,r10
00072AAC: mov       rax,QWORD PTR [r10]
00072AAF: call      QWORD PTR [rax+0x218]
00072AB5: test      eax,eax
00072AB7: jns       0x180072ac7
00072AB9: mov       edx,eax
00072ABB: lea       rcx,[rip+0x112c04e]        # 0x18119eb10 ; 'Failed to OpenSharedFence ErrorCode: 0x%08x'
00072AC2: call      0x1800fbb40
00072AC7: mov       r10,QWORD PTR [rdi+0xb18]
00072ACE: test      r10,r10
00072AD1: je        0x180072b11
00072AD3: mov       rdx,QWORD PTR [rsp+0x48]
00072AD8: test      rdx,rdx
00072ADB: je        0x180072b16
00072ADD: lea       rcx,[rbp+0xbb0]
00072AE4: call      0x180008420
00072AE9: mov       r9,rax
00072AEC: lea       r8,[rip+0x112c5e5]        # 0x18119f0d8
00072AF3: mov       rax,QWORD PTR [r10]
00072AF6: mov       rcx,r10
00072AF9: call      QWORD PTR [rax+0x100]
00072AFF: test      eax,eax
00072B01: jns       0x180072b11
00072B03: mov       edx,eax
00072B05: lea       rcx,[rip+0x112c034]        # 0x18119eb40 ; 'Failed to open GPU2 fence on GPU1 device: 0x%08x'
00072B0C: call      0x1800fbb40
00072B11: mov       rdx,QWORD PTR [rsp+0x48]
00072B16: mov       rcx,rdx
00072B19: call      QWORD PTR [rip+0xa05a1]        # 0x1801130c0 ; KERNEL32.dll!CloseHandle
00072B1F: jmp       0x180072b69
00072B21: mov       r11,QWORD PTR [rdi+0x98]
00072B28: lea       rbp,[rdi+r15*8]
00072B2C: lea       rcx,[rbp+0x178]
00072B33: call      0x180008420
00072B38: mov       rdx,QWORD PTR [r11]
00072B3B: lea       r9,[rip+0x112c596]        # 0x18119f0d8
00072B42: xor       r8d,r8d
00072B45: mov       QWORD PTR [rsp+0x20],rax
00072B4A: mov       rcx,r11
00072B4D: mov       r10,QWORD PTR [rdx+0x120]
00072B54: xor       edx,edx
00072B56: call      r10
00072B59: test      eax,eax
00072B5B: jns       0x180072b69
00072B5D: lea       rcx,[rip+0x112c014]        # 0x18119eb78 ; 'Failed to CreateFence.'
00072B64: call      0x1800fbb40
00072B69: mov       rcx,QWORD PTR [rdi+r15*8+0x178]
00072B71: mov       rax,QWORD PTR [rcx]
00072B74: call      QWORD PTR [rax+0x40]
00072B77: xor       r9d,r9d
00072B7A: xor       r8d,r8d
00072B7D: xor       edx,edx
00072B7F: mov       QWORD PTR [rdi+r15*8+0x208],rax
00072B87: xor       ecx,ecx
00072B89: call      QWORD PTR [rip+0xa0631]        # 0x1801131c0 ; KERNEL32.dll!CreateEventW
00072B8F: mov       QWORD PTR [rdi+r15*8+0x1d8],rax
00072B97: mov       ebx,r12d
00072B9A: test      r14b,r14b
00072B9D: je        0x180072ba9
00072B9F: cmp       BYTE PTR [rdi+0x924],bl
00072BA5: cmovne    ebx,r13d
00072BA9: mov       r11,QWORD PTR [rdi+0x98]
00072BB0: lea       rcx,[rbp+0x1b8]
00072BB7: call      0x180008420
00072BBC: mov       rdx,QWORD PTR [r11]
00072BBF: lea       r9,[rip+0x112c512]        # 0x18119f0d8
00072BC6: mov       r8d,ebx
00072BC9: mov       QWORD PTR [rsp+0x20],rax
00072BCE: mov       rcx,r11
00072BD1: mov       r10,QWORD PTR [rdx+0x120]
00072BD8: xor       edx,edx
00072BDA: call      r10
00072BDD: test      eax,eax
00072BDF: jns       0x180072bff
00072BE1: mov       r8d,eax
00072BE4: lea       rcx,[rip+0x112bfa5]        # 0x18119eb90 ; 'Failed to CreateFence GPU1-only (flags=0x%x) 0x%08x'
00072BEB: mov       edx,ebx
00072BED: call      0x1800fbb40
00072BF2: mov       QWORD PTR [rdi+r15*8+0x228],r12
00072BFA: jmp       0x180072cb1
00072BFF: mov       QWORD PTR [rdi+r15*8+0x228],r12
00072C07: test      bl,0x1
00072C0A: je        0x180072cb1
00072C10: cmp       QWORD PTR [rdi+0xb18],r12
00072C17: je        0x180072cb1
00072C1D: mov       rcx,QWORD PTR [rdi+0x98]
00072C24: lea       rdx,[rsp+0x48]
00072C29: mov       QWORD PTR [rsp+0x28],rdx
00072C2E: mov       r9d,0x10000000
00072C34: mov       rdx,QWORD PTR [rdi+r15*8+0x1b8]
00072C3C: xor       r8d,r8d
00072C3F: mov       QWORD PTR [rsp+0x48],r12
00072C44: mov       rax,QWORD PTR [rcx]
00072C47: mov       QWORD PTR [rsp+0x20],r12
00072C4C: call      QWORD PTR [rax+0xf8]
00072C52: test      eax,eax
00072C54: jns       0x180072c66
00072C56: mov       edx,eax
00072C58: lea       rcx,[rip+0x112bf69]        # 0x18119ebc8 ; 'CreateSharedHandle GPU1-only fence failed: 0x%08x'
00072C5F: call      0x1800fbb40
00072C64: jmp       0x180072cb1
00072C66: mov       r10,QWORD PTR [rdi+0xb18]
00072C6D: lea       rcx,[rbp+0xbd0]
00072C74: call      0x180008420
00072C79: mov       rdx,QWORD PTR [rsp+0x48]
00072C7E: lea       r8,[rip+0x112c453]        # 0x18119f0d8
00072C85: mov       r9,rax
00072C88: mov       rcx,r10
00072C8B: mov       rax,QWORD PTR [r10]
00072C8E: call      QWORD PTR [rax+0x100]
00072C94: test      eax,eax
00072C96: jns       0x180072ca6
00072C98: mov       edx,eax
00072C9A: lea       rcx,[rip+0x112bf5f]        # 0x18119ec00 ; 'OpenSharedHandle GPU1-only fence on local device failed: 0x%08x'
00072CA1: call      0x1800fbb40
00072CA6: mov       rcx,QWORD PTR [rsp+0x48]
00072CAB: call      QWORD PTR [rip+0xa040f]        # 0x1801130c0 ; KERNEL32.dll!CloseHandle
00072CB1: lea       rcx,[rdi+0x368]
00072CB8: cmp       QWORD PTR [rcx],r12
00072CBB: jne       0x180072d11
00072CBD: mov       r10,QWORD PTR [rdi+0x98]
00072CC4: mov       QWORD PTR [rsp+0x50],r12
00072CC9: mov       DWORD PTR [rsp+0x4c],0x14
00072CD1: mov       DWORD PTR [rsp+0x48],0x2
00072CD9: call      0x180008420
00072CDE: mov       r9,rax
00072CE1: lea       r8,[rip+0x112c3e0]        # 0x18119f0c8
00072CE8: mov       rax,QWORD PTR [r10]
00072CEB: lea       rdx,[rsp+0x48]
00072CF0: mov       rcx,r10
00072CF3: call      QWORD PTR [rax+0x70]
00072CF6: mov       rcx,QWORD PTR [rdi+0x98]
00072CFD: mov       edx,0x2
00072D02: mov       rax,QWORD PTR [rcx]
00072D05: call      QWORD PTR [rax+0x78]
00072D08: mov       eax,eax
00072D0A: mov       QWORD PTR [rdi+0x388],rax
00072D11: lea       rcx,[rdi+0x340]
00072D18: cmp       QWORD PTR [rcx],r12
00072D1B: jne       0x180072d6f
00072D1D: mov       r10,QWORD PTR [rdi+0x98]
00072D24: mov       QWORD PTR [rsp+0x50],0x1
00072D2D: mov       DWORD PTR [rsp+0x4c],0x4000
00072D35: mov       DWORD PTR [rsp+0x48],r12d
00072D3A: call      0x180008420
00072D3F: mov       r9,rax
00072D42: lea       r8,[rip+0x112c37f]        # 0x18119f0c8
00072D49: mov       rax,QWORD PTR [r10]
00072D4C: lea       rdx,[rsp+0x48]
00072D51: mov       rcx,r10
00072D54: call      QWORD PTR [rax+0x70]
00072D57: mov       rcx,QWORD PTR [rdi+0x98]
00072D5E: xor       edx,edx
00072D60: mov       rax,QWORD PTR [rcx]
00072D63: call      QWORD PTR [rax+0x78]
00072D66: mov       eax,eax
00072D68: mov       QWORD PTR [rdi+0x360],rax
00072D6F: mov       edx,r15d
00072D72: lea       rcx,[rip+0x112bec7]        # 0x18119ec40 ; 'DXGISwapChainDX11Wrapper resources for type %d created!'
00072D79: call      0x1800fbb40
00072D7E: jmp       0x180072d9e
00072D80: lea       rcx,[rip+0x112bcd9]        # 0x18119ea60 ; 'Failed to CreateCommandList set=%d i=%d hr=0x%08x'
00072D87: jmp       0x180072d90
00072D89: lea       rcx,[rip+0x112bc98]        # 0x18119ea28 ; 'Failed to CreateCommandAllocator set=%d i=%d hr=0x%08x'
00072D90: mov       r9d,eax
00072D93: mov       r8d,ebp
00072D96: mov       edx,r15d
00072D99: call      0x1800fbb40
00072D9E: mov       r12,QWORD PTR [rsp+0x68]
00072DA3: mov       rbp,QWORD PTR [rsp+0x70]
00072DA8: mov       rbx,QWORD PTR [rsp+0xb0]
00072DB0: mov       r14,QWORD PTR [rsp+0x60]
00072DB5: mov       rcx,QWORD PTR [rsp+0x58]
00072DBA: xor       rcx,rsp
00072DBD: call      0x18010c270
00072DC2: add       rsp,0x78
00072DC6: pop       r15
00072DC8: pop       r13
00072DCA: pop       rdi
00072DCB: pop       rsi
00072DCC: ret       
