; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x284D30..0x284F47; unnamed
00284D30: mov       r11,rsp
00284D33: push      rdi
00284D34: sub       rsp,0x80
00284D3B: mov       rax,QWORD PTR [rip+0xbfc09e]        # 0x180e80de0
00284D42: mov       rdi,rcx
00284D45: cmp       rcx,QWORD PTR [rax+0x1690]
00284D4C: jne       0x180284e59
00284D52: cmp       BYTE PTR [rip+0xbfc246],0x0        # 0x180e80f9f
00284D59: je        0x180284e59
00284D5F: mov       rcx,QWORD PTR [rip+0xbfc1ba]        # 0x180e80f20
00284D66: test      rcx,rcx
00284D69: je        0x180284e59
00284D6F: mov       rax,QWORD PTR [rcx]
00284D72: mov       QWORD PTR [r11+0x10],rbx
00284D76: lea       rbx,[rcx+0x40]
00284D7A: mov       QWORD PTR [r11+0x18],rsi
00284D7E: movzx     esi,BYTE PTR [rip+0xbfc30c]        # 0x180e81091
00284D85: mov       BYTE PTR [rip+0xbfc213],0x0        # 0x180e80f9f
00284D8C: mov       BYTE PTR [rip+0xbfc2fe],0x0        # 0x180e81091
00284D93: call      QWORD PTR [rax+0x120]
00284D99: mov       eax,eax
00284D9B: imul      rdx,rax,0x58
00284D9F: add       rdx,rbx
00284DA2: call      0x1802941a0
00284DA7: cmp       BYTE PTR [rip+0xbfc1ed],0x0        # 0x180e80f9b
00284DAE: mov       rbx,QWORD PTR [rsp+0x98]
00284DB6: jne       0x180284e4a
00284DBC: mov       QWORD PTR [rsp+0x90],rdi
00284DC4: call      0x180222050
00284DC9: lea       rcx,[rip+0x184e48]        # 0x180409c18 ; 'F:\\GithubMods\\DLSS\\SkyrimUpscaler\\src\\UpscalerHooks.cpp'
00284DD0: mov       DWORD PTR [rsp+0x48],0x139
00284DD8: mov       QWORD PTR [rsp+0x40],rcx
00284DDD: lea       r9,[rsp+0x30]
00284DE2: mov       ecx,DWORD PTR [rsp+0x6c]
00284DE6: lea       rdx,[rsp+0x60]
00284DEB: mov       DWORD PTR [rsp+0x4c],ecx
00284DEF: mov       r8d,0x2
00284DF5: vmovups   xmm0,XMMWORD PTR [rsp+0x40]
00284DFB: lea       rcx,[rip+0x184fce]        # 0x180409dd0 ; 'void __cdecl ReShadeFrameComplete(struct reshade::api::effect_runtime *)'
00284E02: mov       QWORD PTR [rsp+0x38],0x45
00284E0B: mov       QWORD PTR [rsp+0x50],rcx
00284E10: lea       rcx,[rip+0x184f69]        # 0x180409d80 ; 'ReShade final overlay included before FG/Direct submission runtime={}'
00284E17: vmovsd    xmm1,QWORD PTR [rsp+0x50]
00284E1D: mov       QWORD PTR [rsp+0x30],rcx
00284E22: lea       rcx,[rsp+0x90]
00284E2A: mov       QWORD PTR [rsp+0x20],rcx
00284E2F: mov       rcx,rax
00284E32: vmovups   XMMWORD PTR [rsp+0x60],xmm0
00284E38: vmovsd    QWORD PTR [rsp+0x70],xmm1
00284E3E: call      0x18017d110
00284E43: mov       BYTE PTR [rip+0xbfc151],0x1        # 0x180e80f9b
00284E4A: mov       BYTE PTR [rip+0xbfc240],sil        # 0x180e81091
00284E51: mov       rsi,QWORD PTR [rsp+0xa0]
00284E59: add       rsp,0x80
00284E60: pop       rdi
00284E61: ret       
00284E62: int3      
00284E63: int3      
00284E64: int3      
00284E65: int3      
00284E66: int3      
00284E67: int3      
00284E68: int3      
00284E69: int3      
00284E6A: int3      
00284E6B: int3      
00284E6C: int3      
00284E6D: int3      
00284E6E: int3      
00284E6F: int3      
00284E70: mov       rax,QWORD PTR [rip+0xbfbf69]        # 0x180e80de0
00284E77: cmp       rcx,QWORD PTR [rax+0x1690]
00284E7E: jne       0x180284eaa
00284E80: cmp       BYTE PTR [rip+0xbfc117],0x0        # 0x180e80f9e
00284E87: je        0x180284eaa
00284E89: movzx     eax,BYTE PTR [rip+0xbfc10c]        # 0x180e80f9c
00284E90: mov       BYTE PTR [rip+0xbfc1fb],al        # 0x180e81091
00284E96: movzx     eax,BYTE PTR [rip+0xbfc100]        # 0x180e80f9d
00284E9D: mov       BYTE PTR [rip+0xbfc1ef],al        # 0x180e81092
00284EA3: mov       BYTE PTR [rip+0xbfc0f4],0x0        # 0x180e80f9e
00284EAA: ret       
00284EAB: int3      
00284EAC: int3      
00284EAD: int3      
00284EAE: int3      
00284EAF: int3      
00284EB0: int3      
00284EB1: int3      
00284EB2: int3      
00284EB3: int3      
00284EB4: int3      
00284EB5: int3      
00284EB6: int3      
00284EB7: int3      
00284EB8: int3      
00284EB9: int3      
00284EBA: int3      
00284EBB: int3      
00284EBC: int3      
00284EBD: int3      
00284EBE: int3      
00284EBF: int3      
00284EC0: mov       rax,QWORD PTR [rip+0xbfbf19]        # 0x180e80de0
00284EC7: cmp       rcx,QWORD PTR [rax+0x1690]
00284ECE: jne       0x180284eff
00284ED0: movzx     eax,BYTE PTR [rip+0xbfc1ba]        # 0x180e81091
00284ED7: mov       BYTE PTR [rip+0xbfc0bf],al        # 0x180e80f9c
00284EDD: movzx     eax,BYTE PTR [rip+0xbfc1ae]        # 0x180e81092
00284EE4: mov       BYTE PTR [rip+0xbfc0b3],al        # 0x180e80f9d
00284EEA: mov       BYTE PTR [rip+0xbfc0ad],0x1        # 0x180e80f9e
00284EF1: mov       BYTE PTR [rip+0xbfc19a],0x0        # 0x180e81092
00284EF8: mov       BYTE PTR [rip+0xbfc192],0x0        # 0x180e81091
00284EFF: ret       
00284F00: int3      
00284F01: int3      
00284F02: int3      
00284F03: int3      
00284F04: int3      
00284F05: int3      
00284F06: int3      
00284F07: int3      
00284F08: int3      
00284F09: int3      
00284F0A: int3      
00284F0B: int3      
00284F0C: int3      
00284F0D: int3      
00284F0E: int3      
00284F0F: int3      
00284F10: mov       QWORD PTR [rsp+0x8],rbx
00284F15: push      rdi
00284F16: sub       rsp,0x20
00284F1A: movzx     edi,dl
00284F1D: mov       rbx,rcx
00284F20: call      0x1801503a0
00284F25: cmp       QWORD PTR [rax+0x1690],rbx
00284F2C: jne       0x180284f3a
00284F2E: call      0x1801503a0
00284F33: mov       BYTE PTR [rax+0x168b],dil
00284F3A: mov       rbx,QWORD PTR [rsp+0x30]
00284F3F: xor       al,al
00284F41: add       rsp,0x20
00284F45: pop       rdi
00284F46: ret       
