; amd_fidelityfx_framegeneration_dx12.dll SHA256=3f5e674a59b400756e98ef31fd583a4eb08ad567ec8dee362886bc96e55ed347
; ImageBase=0x180000000, RVAs in left column
; range 0x6D90..0x6EF9; GetProvider
00006D90: rex       push rbx
00006D92: push      rbp
00006D93: push      rsi
00006D94: push      rdi
00006D95: push      r12
00006D97: push      r14
00006D99: push      r15
00006D9B: sub       rsp,0x20
00006D9F: mov       rdi,r9
00006DA2: mov       r12,r8
00006DA5: mov       r14,rdx
00006DA8: mov       rbp,rcx
00006DAB: mov       rsi,QWORD PTR [rsp+0x80]
00006DB3: cmp       BYTE PTR [r9+0x58],0x0
00006DB8: je        0x180006dd8
00006DBA: lea       rax,[rip+0xff67f]        # 0x180106440 ; ffxProviderExternal::`vftable'
00006DC1: mov       QWORD PTR [r9],rax
00006DC4: mov       rcx,QWORD PTR [r9+0x28]
00006DC8: test      rcx,rcx
00006DCB: je        0x180006dd4
00006DCD: mov       rax,QWORD PTR [rcx]
00006DD0: call      QWORD PTR [rax+0x10]
00006DD3: nop       
00006DD4: mov       BYTE PTR [rdi+0x58],0x0
00006DD8: mov       r8,rbp
00006DDB: mov       rdx,r12
00006DDE: mov       rcx,rdi
00006DE1: call      0x180006ac0 ; ffxProviderExternal::ffxProviderExternal | '@SUVWAVH'
00006DE6: mov       BYTE PTR [rdi+0x58],0x1
00006DEA: test      r14,r14
00006DED: je        0x180006e65
00006DEF: cmp       QWORD PTR [rdi+0x30],0x0
00006DF4: je        0x180006e0a
00006DF6: cmp       QWORD PTR [rdi+0x10],r14
00006DFA: jne       0x180006e0a
00006DFC: mov       rax,rbp
00006DFF: and       eax,0xff0000
00006E04: cmp       rax,QWORD PTR [rdi+0x18]
00006E08: je        0x180006e53
00006E0A: mov       rbx,QWORD PTR [rsi]
00006E0D: mov       rax,QWORD PTR [rsi+0x8]
00006E11: lea       rsi,[rbx+rax*8]
00006E15: cmp       rbx,rsi
00006E18: je        0x180006e42
00006E1A: nop       WORD PTR [rax+rax*1+0x0]
00006E20: mov       rdi,QWORD PTR [rbx]
00006E23: cmp       QWORD PTR [rdi+0x10],r14
00006E27: jne       0x180006e39
00006E29: mov       rax,QWORD PTR [rdi]
00006E2C: mov       rdx,rbp
00006E2F: mov       rcx,rdi
00006E32: call      QWORD PTR [rax+0x8]
00006E35: test      al,al
00006E37: jne       0x180006e53
00006E39: add       rbx,0x8
00006E3D: cmp       rbx,rsi
00006E40: jne       0x180006e20
00006E42: xor       eax,eax
00006E44: add       rsp,0x20
00006E48: pop       r15
00006E4A: pop       r14
00006E4C: pop       r12
00006E4E: pop       rdi
00006E4F: pop       rsi
00006E50: pop       rbp
00006E51: pop       rbx
00006E52: ret       
00006E53: mov       rax,rdi
00006E56: add       rsp,0x20
00006E5A: pop       r15
00006E5C: pop       r14
00006E5E: pop       r12
00006E60: pop       rdi
00006E61: pop       rsi
00006E62: pop       rbp
00006E63: pop       rbx
00006E64: ret       
00006E65: xor       r15d,r15d
00006E68: mov       rbx,QWORD PTR [rsi]
00006E6B: mov       rax,QWORD PTR [rsi+0x8]
00006E6F: lea       r14,[rbx+rax*8]
00006E73: cmp       rbx,r14
00006E76: je        0x180006eb1
00006E78: nop       DWORD PTR [rax+rax*1+0x0]
00006E80: mov       rsi,QWORD PTR [rbx]
00006E83: mov       rax,QWORD PTR [rsi]
00006E86: mov       rdx,rbp
00006E89: mov       rcx,rsi
00006E8C: call      QWORD PTR [rax+0x8]
00006E8F: test      al,al
00006E91: je        0x180006ea3
00006E93: mov       rax,QWORD PTR [rsi]
00006E96: mov       rdx,r12
00006E99: mov       rcx,rsi
00006E9C: call      QWORD PTR [rax+0x10]
00006E9F: test      al,al
00006EA1: jne       0x180006eae
00006EA3: add       rbx,0x8
00006EA7: cmp       rbx,r14
00006EAA: jne       0x180006e80
00006EAC: jmp       0x180006eb1
00006EAE: mov       r15,rsi
00006EB1: cmp       BYTE PTR [rdi+0x58],0x0
00006EB5: je        0x180006ee7
00006EB7: cmp       QWORD PTR [rdi+0x30],0x0
00006EBC: je        0x180006ee7
00006EBE: and       ebp,0xff0000
00006EC4: cmp       rbp,QWORD PTR [rdi+0x18]
00006EC8: jne       0x180006ee7
00006ECA: test      r15,r15
00006ECD: je        0x180006e53
00006ECF: mov       edx,DWORD PTR [rdi+0x10]
00006ED2: btr       edx,0x1f
00006ED6: mov       ecx,DWORD PTR [r15+0x10]
00006EDA: btr       ecx,0x1f
00006EDE: cmp       rdx,rcx
00006EE1: ja        0x180006e53
00006EE7: mov       rax,r15
00006EEA: add       rsp,0x20
00006EEE: pop       r15
00006EF0: pop       r14
00006EF2: pop       r12
00006EF4: pop       rdi
00006EF5: pop       rsi
00006EF6: pop       rbp
00006EF7: pop       rbx
00006EF8: ret       
