; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x2A9B30..0x2A9FB5; unnamed
002A9B30: mov       QWORD PTR [rsp+0x8],rbx
002A9B35: mov       QWORD PTR [rsp+0x10],rsi
002A9B3A: mov       QWORD PTR [rsp+0x18],rdi
002A9B3F: mov       QWORD PTR [rsp+0x20],r9
002A9B44: push      rbp
002A9B45: push      r12
002A9B47: push      r13
002A9B49: push      r14
002A9B4B: push      r15
002A9B4D: lea       rbp,[rsp-0x37]
002A9B52: sub       rsp,0xa0
002A9B59: mov       rbx,r9
002A9B5C: mov       rsi,r8
002A9B5F: mov       edi,edx
002A9B61: mov       r14,rcx
002A9B64: xor       r12d,r12d
002A9B67: cmp       BYTE PTR [rip+0xbd7524],r12b        # 0x180e81092
002A9B6E: jne       0x1802a9d51
002A9B74: mov       rax,QWORD PTR [rip+0xbd7265]        # 0x180e80de0
002A9B7B: cmp       BYTE PTR [rax+0x343],r12b
002A9B82: je        0x1802a9e21
002A9B88: test      rbx,rbx
002A9B8B: je        0x1802a9e21
002A9B91: test      edx,edx
002A9B93: je        0x1802a9e21
002A9B99: test      r8,r8
002A9B9C: je        0x1802a9e21
002A9BA2: mov       rcx,QWORD PTR [r8]
002A9BA5: test      rcx,rcx
002A9BA8: je        0x1802a9e21
002A9BAE: cmp       QWORD PTR [rax+0x8d8],r12
002A9BB5: je        0x1802a9e21
002A9BBB: mov       QWORD PTR [rbp-0x49],r12
002A9BBF: mov       rax,QWORD PTR [rcx]
002A9BC2: lea       rdx,[rbp-0x49]
002A9BC6: call      QWORD PTR [rax+0x38]
002A9BC9: mov       r8,QWORD PTR [rbp-0x49]
002A9BCD: mov       rbx,QWORD PTR [rip+0xbd734c]        # 0x180e80f20
002A9BD4: test      rbx,rbx
002A9BD7: je        0x1802a9be7
002A9BD9: cmp       r8,QWORD PTR [rbx+0x148]
002A9BE0: jne       0x1802a9be7
002A9BE2: mov       r13b,0x1
002A9BE5: jmp       0x1802a9bea
002A9BE7: xor       r13b,r13b
002A9BEA: test      rbx,rbx
002A9BED: je        0x1802a9c16
002A9BEF: mov       rax,QWORD PTR [rbx]
002A9BF2: mov       rcx,rbx
002A9BF5: call      QWORD PTR [rax+0x120]
002A9BFB: mov       eax,eax
002A9BFD: imul      rcx,rax,0x58
002A9C01: mov       r8,QWORD PTR [rbp-0x49]
002A9C05: cmp       r8,QWORD PTR [rcx+rbx*1+0x40]
002A9C0A: mov       rbx,QWORD PTR [rip+0xbd730f]        # 0x180e80f20
002A9C11: jne       0x1802a9c16
002A9C13: mov       r12b,0x1
002A9C16: mov       rcx,QWORD PTR [rip+0xbd71c3]        # 0x180e80de0
002A9C1D: mov       rax,QWORD PTR [rcx+0x1570]
002A9C24: test      rax,rax
002A9C27: je        0x1802a9c33
002A9C29: cmp       r8,rax
002A9C2C: jne       0x1802a9c33
002A9C2E: mov       r15b,0x1
002A9C31: jmp       0x1802a9c36
002A9C33: xor       r15b,r15b
002A9C36: vpxor     xmm0,xmm0,xmm0
002A9C3A: xor       eax,eax
002A9C3C: vmovups   YMMWORD PTR [rbp+0x7],ymm0
002A9C41: mov       QWORD PTR [rbp+0x27],rax
002A9C45: mov       DWORD PTR [rbp+0x2f],eax
002A9C48: test      r8,r8
002A9C4B: je        0x1802a9c75
002A9C4D: mov       rax,QWORD PTR [r8]
002A9C50: lea       rdx,[rbp+0x7]
002A9C54: mov       rcx,r8
002A9C57: vzeroupper 
002A9C5A: call      QWORD PTR [rax+0x50]
002A9C5D: mov       rcx,QWORD PTR [rbp-0x49]
002A9C61: mov       rax,QWORD PTR [rcx]
002A9C64: call      QWORD PTR [rax+0x10]
002A9C67: mov       rcx,QWORD PTR [rip+0xbd7172]        # 0x180e80de0
002A9C6E: mov       rbx,QWORD PTR [rip+0xbd72ab]        # 0x180e80f20
002A9C75: test      r13b,r13b
002A9C78: jne       0x1802a9c88
002A9C7A: test      r12b,r12b
002A9C7D: jne       0x1802a9c88
002A9C7F: test      r15b,r15b
002A9C82: je        0x1802a9d3f
002A9C88: lea       r15,[rcx+0x8d8]
002A9C8F: mov       r8,QWORD PTR [r15]
002A9C92: test      r8,r8
002A9C95: je        0x1802a9cb5
002A9C97: lea       rdx,[r15+0x28]
002A9C9B: mov       rax,QWORD PTR [r8]
002A9C9E: mov       rcx,r8
002A9CA1: vzeroupper 
002A9CA4: call      QWORD PTR [rax+0x50]
002A9CA7: mov       rcx,QWORD PTR [rip+0xbd7132]        # 0x180e80de0
002A9CAE: mov       rbx,QWORD PTR [rip+0xbd726b]        # 0x180e80f20
002A9CB5: mov       eax,0x28
002A9CBA: vmovups   ymm1,YMMWORD PTR [r15+rax*1]
002A9CC0: vmovsd    xmm0,QWORD PTR [r15+rax*1+0x20]
002A9CC7: vmovsd    QWORD PTR [rbp-0x19],xmm0
002A9CCC: mov       eax,DWORD PTR [r15+rax*1+0x28]
002A9CD1: mov       DWORD PTR [rbp-0x11],eax
002A9CD4: vmovd     eax,xmm1
002A9CD8: cmp       DWORD PTR [rbp+0x7],eax
002A9CDB: jne       0x1802a9d3f
002A9CDD: vmovq     rax,xmm1
002A9CE2: shr       rax,0x20
002A9CE6: cmp       DWORD PTR [rbp+0xb],eax
002A9CE9: jne       0x1802a9d3f
002A9CEB: add       rcx,0x8d8
002A9CF2: vzeroupper 
002A9CF5: call      0x18016c450
002A9CFA: mov       r9,rax
002A9CFD: mov       edx,edi
002A9CFF: cmp       r14,QWORD PTR [rip+0xbd729a]        # 0x180e80fa0
002A9D06: jne       0x1802a9d34
002A9D08: mov       r8d,DWORD PTR [rip+0x1c97a5]        # 0x1804734b4
002A9D0F: mov       rcx,QWORD PTR gs:0x58
002A9D18: mov       r10d,0x3f28
002A9D1E: mov       rcx,QWORD PTR [rcx+r8*8]
002A9D22: mov       r10,QWORD PTR [r10+rcx*1]
002A9D26: mov       r8,rsi
002A9D29: mov       rcx,r14
002A9D2C: call      r10
002A9D2F: jmp       0x1802a9f94
002A9D34: mov       r8,rsi
002A9D37: mov       rcx,r14
002A9D3A: jmp       0x1802a9f8e
002A9D3F: cmp       BYTE PTR [rip+0xbd734c],0x0        # 0x180e81092
002A9D46: je        0x1802a9e1d
002A9D4C: xor       r12d,r12d
002A9D4F: jmp       0x1802a9d58
002A9D51: mov       rbx,QWORD PTR [rip+0xbd71c8]        # 0x180e80f20
002A9D58: test      rbx,rbx
002A9D5B: je        0x1802a9e1d
002A9D61: test      edi,edi
002A9D63: je        0x1802a9e1d
002A9D69: test      rsi,rsi
002A9D6C: je        0x1802a9e1d
002A9D72: mov       rcx,QWORD PTR [rsi]
002A9D75: test      rcx,rcx
002A9D78: je        0x1802a9e1d
002A9D7E: mov       QWORD PTR [rbp-0x49],r12
002A9D82: mov       rax,QWORD PTR [rcx]
002A9D85: lea       rdx,[rbp-0x49]
002A9D89: vzeroupper 
002A9D8C: call      QWORD PTR [rax+0x38]
002A9D8F: mov       rdx,QWORD PTR [rbp-0x49]
002A9D93: mov       rcx,QWORD PTR [rip+0xbd7046]        # 0x180e80de0
002A9D9A: cmp       BYTE PTR [rcx+0x3d0],0x0
002A9DA1: je        0x1802a9db1
002A9DA3: cmp       rdx,QWORD PTR [rcx+0x1570]
002A9DAA: jne       0x1802a9db1
002A9DAC: mov       r15b,0x1
002A9DAF: jmp       0x1802a9db4
002A9DB1: xor       r15b,r15b
002A9DB4: cmp       BYTE PTR [rcx+0x343],0x0
002A9DBB: je        0x1802a9ded
002A9DBD: mov       rcx,QWORD PTR [rip+0xbd715c]        # 0x180e80f20
002A9DC4: lea       rbx,[rcx+0x40]
002A9DC8: mov       rax,QWORD PTR [rcx]
002A9DCB: call      QWORD PTR [rax+0x120]
002A9DD1: mov       eax,eax
002A9DD3: imul      rcx,rax,0x58
002A9DD7: mov       rdx,QWORD PTR [rbp-0x49]
002A9DDB: cmp       rdx,QWORD PTR [rcx+rbx*1]
002A9DDF: mov       rcx,QWORD PTR [rip+0xbd6ffa]        # 0x180e80de0
002A9DE6: jne       0x1802a9ded
002A9DE8: mov       r8b,0x1
002A9DEB: jmp       0x1802a9df0
002A9DED: xor       r8b,r8b
002A9DF0: mov       rax,QWORD PTR [rip+0xbd7129]        # 0x180e80f20
002A9DF7: cmp       rdx,QWORD PTR [rax+0x148]
002A9DFE: je        0x1802a9e61
002A9E00: test      r15b,r15b
002A9E03: jne       0x1802a9e61
002A9E05: test      r8b,r8b
002A9E08: jne       0x1802a9e61
002A9E0A: test      rdx,rdx
002A9E0D: je        0x1802a9e1d
002A9E0F: mov       QWORD PTR [rbp-0x49],r12
002A9E13: mov       rax,QWORD PTR [rdx]
002A9E16: mov       rcx,rdx
002A9E19: call      QWORD PTR [rax+0x10]
002A9E1C: nop       
002A9E1D: mov       rbx,QWORD PTR [rbp+0x7f]
002A9E21: mov       r9,rbx
002A9E24: mov       r8,rsi
002A9E27: mov       rcx,r14
002A9E2A: cmp       r14,QWORD PTR [rip+0xbd716f]        # 0x180e80fa0
002A9E31: jne       0x1802a9f89
002A9E37: mov       edx,DWORD PTR [rip+0x1c9677]        # 0x1804734b4
002A9E3D: mov       rax,QWORD PTR gs:0x58
002A9E46: mov       r10d,0x3f28
002A9E4C: mov       rax,QWORD PTR [rax+rdx*8]
002A9E50: mov       r10,QWORD PTR [r10+rax*1]
002A9E54: mov       edx,edi
002A9E56: vzeroupper 
002A9E59: call      r10
002A9E5C: jmp       0x1802a9f94
002A9E61: cmp       BYTE PTR [rcx+0x3d3],0x0
002A9E68: jne       0x1802a9e9e
002A9E6A: call      0x180293f20
002A9E6F: test      al,al
002A9E71: jne       0x1802a9e97
002A9E73: test      r15b,r15b
002A9E76: jne       0x1802a9e97
002A9E78: mov       rcx,QWORD PTR [rip+0xbd70a1]        # 0x180e80f20
002A9E7F: lea       rbx,[rcx+0x40]
002A9E83: mov       rax,QWORD PTR [rcx]
002A9E86: call      QWORD PTR [rax+0x120]
002A9E8C: mov       eax,eax
002A9E8E: imul      rcx,rax,0x58
002A9E92: add       rcx,rbx
002A9E95: jmp       0x1802a9ea5
002A9E97: mov       rcx,QWORD PTR [rip+0xbd6f42]        # 0x180e80de0
002A9E9E: add       rcx,0x9e0
002A9EA5: call      0x18016c140
002A9EAA: mov       rbx,rax
002A9EAD: mov       QWORD PTR [rbp-0x31],r12
002A9EB1: vpxor     xmm0,xmm0,xmm0
002A9EB5: vmovdqu   XMMWORD PTR [rbp-0x29],xmm0
002A9EBA: vpxor     xmm1,xmm1,xmm1
002A9EBE: vmovdqu   XMMWORD PTR [rbp-0x19],xmm1
002A9EC3: vmovdqu   XMMWORD PTR [rbp-0x9],xmm0
002A9EC8: mov       ecx,edi
002A9ECA: mov       eax,0x8
002A9ECF: cmp       edi,eax
002A9ED1: cmova     ecx,eax
002A9ED4: test      ecx,ecx
002A9ED6: je        0x1802a9eeb
002A9ED8: mov       r8d,ecx
002A9EDB: shl       r8,0x3
002A9EDF: mov       rdx,rsi
002A9EE2: lea       rcx,[rbp-0x39]
002A9EE6: call      0x18023a826
002A9EEB: mov       QWORD PTR [rbp-0x39],rbx
002A9EEF: mov       rcx,QWORD PTR [rip+0xbd6eea]        # 0x180e80de0
002A9EF6: cmp       edi,0x1
002A9EF9: jbe       0x1802a9f1b
002A9EFB: cmp       BYTE PTR [rcx+0x343],0x0
002A9F02: jne       0x1802a9f1b
002A9F04: add       rcx,0x880
002A9F0B: call      0x18016c140
002A9F10: mov       QWORD PTR [rbp-0x31],rax
002A9F14: mov       rcx,QWORD PTR [rip+0xbd6ec5]        # 0x180e80de0
002A9F1B: cmp       QWORD PTR [rbp+0x7f],0x0
002A9F20: je        0x1802a9f33
002A9F22: add       rcx,0x8d8
002A9F29: call      0x18016c450
002A9F2E: mov       r9,rax
002A9F31: jmp       0x1802a9f36
002A9F33: mov       r9,r12
002A9F36: lea       r8,[rbp-0x39]
002A9F3A: mov       rcx,r14
002A9F3D: cmp       r14,QWORD PTR [rip+0xbd705c]        # 0x180e80fa0
002A9F44: jne       0x1802a9f6a
002A9F46: mov       edx,DWORD PTR [rip+0x1c9568]        # 0x1804734b4
002A9F4C: mov       rax,QWORD PTR gs:0x58
002A9F55: mov       r10d,0x3f28
002A9F5B: mov       rax,QWORD PTR [rax+rdx*8]
002A9F5F: mov       r10,QWORD PTR [r10+rax*1]
002A9F63: mov       edx,edi
002A9F65: call      r10
002A9F68: jmp       0x1802a9f73
002A9F6A: mov       edx,edi
002A9F6C: call      QWORD PTR [rip+0x1d34ce]        # 0x18047d440
002A9F72: nop       
002A9F73: mov       rcx,QWORD PTR [rbp-0x49]
002A9F77: test      rcx,rcx
002A9F7A: je        0x1802a9f87
002A9F7C: mov       QWORD PTR [rbp-0x49],r12
002A9F80: mov       rax,QWORD PTR [rcx]
002A9F83: call      QWORD PTR [rax+0x10]
002A9F86: nop       
002A9F87: jmp       0x1802a9f94
002A9F89: mov       edx,edi
002A9F8B: vzeroupper 
002A9F8E: call      QWORD PTR [rip+0x1d34ac]        # 0x18047d440
002A9F94: lea       r11,[rsp+0xa0]
002A9F9C: mov       rbx,QWORD PTR [r11+0x30]
002A9FA0: mov       rsi,QWORD PTR [r11+0x38]
002A9FA4: mov       rdi,QWORD PTR [r11+0x40]
002A9FA8: mov       rsp,r11
002A9FAB: pop       r15
002A9FAD: pop       r14
002A9FAF: pop       r13
002A9FB1: pop       r12
002A9FB3: pop       rbp
002A9FB4: ret       
