; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xF5CF0..0xF6105; unnamed
000F5CF0: mov       QWORD PTR [rsp+0x18],rbx
000F5CF5: push      rbp
000F5CF6: push      rsi
000F5CF7: push      rdi
000F5CF8: push      r14
000F5CFA: push      r15
000F5CFC: lea       rbp,[rsp-0xc0]
000F5D04: sub       rsp,0x1c0
000F5D0B: mov       rax,QWORD PTR [rip+0x10d4cae]        # 0x1811ca9c0
000F5D12: xor       rax,rsp
000F5D15: mov       QWORD PTR [rbp+0xb0],rax
000F5D1C: mov       rdi,rdx
000F5D1F: mov       rsi,rcx
000F5D22: xor       ecx,ecx
000F5D24: call      0x1800d35a0
000F5D29: mov       rcx,QWORD PTR [rsi+0x48]
000F5D2D: test      rcx,rcx
000F5D30: je        0x1800f60df
000F5D36: mov       rax,QWORD PTR [rcx]
000F5D39: mov       edx,DWORD PTR [rdi]
000F5D3B: call      QWORD PTR [rax+0x28]
000F5D3E: test      al,al
000F5D40: je        0x1800f60df
000F5D46: movss     xmm1,DWORD PTR [rdi+0x40]
000F5D4B: comiss    xmm1,DWORD PTR [rip+0x10b6e0e]        # 0x1811acb60
000F5D52: ja        0x1800f5d61
000F5D54: movss     xmm0,DWORD PTR [rip+0x10b6e58]        # 0x1811acbb4
000F5D5C: comiss    xmm0,xmm1
000F5D5F: jbe       0x1800f5d72
000F5D61: lea       rcx,[rsi+0x50]
000F5D65: mov       rdx,rdi
000F5D68: call      0x1800cdfe0
000F5D6D: mov       ecx,DWORD PTR [rax]
000F5D6F: mov       DWORD PTR [rdi+0x40],ecx
000F5D72: cmp       QWORD PTR [rdi+0x68],0x0
000F5D77: jne       0x1800f5d84
000F5D79: mov       rax,QWORD PTR [rsi+0x88]
000F5D80: mov       QWORD PTR [rdi+0x68],rax
000F5D84: lea       rcx,[rsi+0x90]
000F5D8B: mov       rdx,rdi
000F5D8E: call      0x180091e70
000F5D93: mov       rax,QWORD PTR [rax]
000F5D96: mov       QWORD PTR [rdi+0x28],rax
000F5D9A: cmp       BYTE PTR [rsi+0x6c],0x0
000F5D9E: je        0x1800f5fbd
000F5DA4: call      0x1800899c0
000F5DA9: mov       rcx,rax
000F5DAC: call      0x180091390
000F5DB1: mov       rbx,rax
000F5DB4: test      rax,rax
000F5DB7: je        0x1800f5fbd
000F5DBD: cmp       BYTE PTR [rax+0x15e],0x0
000F5DC4: je        0x1800f5fbd
000F5DCA: mov       rcx,QWORD PTR [rdi+0x8]
000F5DCE: mov       QWORD PTR [rbp+0x70],rcx
000F5DD2: mov       rcx,QWORD PTR [rdi+0x10]
000F5DD6: mov       QWORD PTR [rbp+0x78],rcx
000F5DDA: mov       rcx,QWORD PTR [rdi+0x18]
000F5DDE: mov       QWORD PTR [rbp+0x80],rcx
000F5DE5: mov       rax,QWORD PTR [rdi+0x20]
000F5DE9: mov       QWORD PTR [rbp+0x88],rax
000F5DF0: mov       rax,QWORD PTR [rdi+0x28]
000F5DF4: mov       QWORD PTR [rbp+0x90],rax
000F5DFB: mov       rax,QWORD PTR [rdi+0x30]
000F5DFF: mov       QWORD PTR [rbp+0x98],rax
000F5E06: mov       rax,QWORD PTR [rdi+0x88]
000F5E0D: mov       QWORD PTR [rbp+0xa0],rax
000F5E14: mov       rax,QWORD PTR [rdi+0x90]
000F5E1B: mov       QWORD PTR [rbp+0xa8],rax
000F5E22: lea       rax,[rbp+0x70]
000F5E26: mov       QWORD PTR [rsp+0x50],rax
000F5E2B: lea       rax,[rbp+0xb0]
000F5E32: mov       QWORD PTR [rsp+0x58],rax
000F5E37: lea       rdx,[rsp+0x50]
000F5E3C: lea       rcx,[rsp+0x30]
000F5E41: call      0x1800f4300
000F5E46: nop       
000F5E47: lea       rax,[rsp+0x60]
000F5E4C: mov       QWORD PTR [rsp+0x50],rax
000F5E51: mov       QWORD PTR [rbp-0x60],rsi
000F5E55: lea       rcx,[rbp-0x58]
000F5E59: movups    xmm0,XMMWORD PTR [rdi]
000F5E5C: movups    XMMWORD PTR [rcx],xmm0
000F5E5F: movups    xmm1,XMMWORD PTR [rdi+0x10]
000F5E63: movups    XMMWORD PTR [rcx+0x10],xmm1
000F5E67: movups    xmm0,XMMWORD PTR [rdi+0x20]
000F5E6B: movups    XMMWORD PTR [rcx+0x20],xmm0
000F5E6F: movups    xmm1,XMMWORD PTR [rdi+0x30]
000F5E73: movups    XMMWORD PTR [rcx+0x30],xmm1
000F5E77: movups    xmm0,XMMWORD PTR [rdi+0x40]
000F5E7B: movups    XMMWORD PTR [rcx+0x40],xmm0
000F5E7F: movups    xmm1,XMMWORD PTR [rdi+0x50]
000F5E83: movups    XMMWORD PTR [rcx+0x50],xmm1
000F5E87: movups    xmm0,XMMWORD PTR [rdi+0x60]
000F5E8B: movups    XMMWORD PTR [rcx+0x60],xmm0
000F5E8F: movups    xmm0,XMMWORD PTR [rdi+0x70]
000F5E93: movups    XMMWORD PTR [rcx+0x70],xmm0
000F5E97: movups    xmm1,XMMWORD PTR [rdi+0x80]
000F5E9E: movups    XMMWORD PTR [rcx+0x80],xmm1
000F5EA5: movups    xmm0,XMMWORD PTR [rdi+0x90]
000F5EAC: movups    XMMWORD PTR [rcx+0x90],xmm0
000F5EB3: movups    xmm1,XMMWORD PTR [rdi+0xa0]
000F5EBA: movups    XMMWORD PTR [rcx+0xa0],xmm1
000F5EC1: mov       rdx,QWORD PTR [rsp+0x40]
000F5EC6: xor       r15d,r15d
000F5EC9: mov       QWORD PTR [rsp+0x40],r15
000F5ECE: mov       rcx,QWORD PTR [rsp+0x38]
000F5ED3: mov       QWORD PTR [rsp+0x38],r15
000F5ED8: mov       rax,QWORD PTR [rsp+0x30]
000F5EDD: mov       QWORD PTR [rsp+0x30],r15
000F5EE2: mov       QWORD PTR [rbp+0x58],rax
000F5EE6: mov       QWORD PTR [rbp+0x60],rcx
000F5EEA: mov       QWORD PTR [rbp+0x68],rdx
000F5EEE: mov       QWORD PTR [rbp-0x68],r15
000F5EF2: call      0x18004bb40
000F5EF7: test      al,al
000F5EF9: je        0x1800f5f08
000F5EFB: lea       rcx,[rbp-0x60]
000F5EFF: call      0x1800f6330
000F5F04: mov       QWORD PTR [rbp-0x68],rax
000F5F08: lea       rdx,[rsp+0x60]
000F5F0D: mov       rcx,rbx
000F5F10: call      0x180008510
000F5F15: mov       ebx,eax
000F5F17: lea       rcx,[rbp+0x58]
000F5F1B: call      0x18000f6a0
000F5F20: test      ebx,ebx
000F5F22: jns       0x1800f5f33
000F5F24: mov       edx,ebx
000F5F26: lea       rcx,[rip+0x10b581b]        # 0x1811ab748
000F5F2D: call      0x1800fbb40
000F5F32: nop       
000F5F33: mov       rbx,QWORD PTR [rsp+0x30]
000F5F38: test      rbx,rbx
000F5F3B: je        0x1800f6037
000F5F41: mov       r14,QWORD PTR [rsp+0x38]
000F5F46: cmp       rbx,r14
000F5F49: je        0x1800f5f70
000F5F4B: nop       DWORD PTR [rax+rax*1+0x0]
000F5F50: mov       rcx,QWORD PTR [rbx]
000F5F53: test      rcx,rcx
000F5F56: je        0x1800f5f62
000F5F58: mov       QWORD PTR [rbx],r15
000F5F5B: mov       rax,QWORD PTR [rcx]
000F5F5E: call      QWORD PTR [rax+0x10]
000F5F61: nop       
000F5F62: add       rbx,0x8
000F5F66: cmp       rbx,r14
000F5F69: jne       0x1800f5f50
000F5F6B: mov       rbx,QWORD PTR [rsp+0x30]
000F5F70: mov       rdx,QWORD PTR [rsp+0x40]
000F5F75: sub       rdx,rbx
000F5F78: and       rdx,0xfffffffffffffff8
000F5F7C: mov       rax,rbx
000F5F7F: cmp       rdx,0x1000
000F5F86: jb        0x1800f5fb3
000F5F88: add       rdx,0x27
000F5F8C: mov       rbx,QWORD PTR [rbx-0x8]
000F5F90: sub       rax,rbx
000F5F93: sub       rax,0x8
000F5F97: cmp       rax,0x1f
000F5F9B: jbe       0x1800f5fb3
000F5F9D: mov       QWORD PTR [rsp+0x20],r15
000F5FA2: xor       r9d,r9d
000F5FA5: xor       r8d,r8d
000F5FA8: xor       edx,edx
000F5FAA: xor       ecx,ecx
000F5FAC: call      QWORD PTR [rip+0x1d806]        # 0x1801137b8 ; api-ms-win-crt-runtime-l1-1-0.dll!_invoke_watson
000F5FB2: int3      
000F5FB3: mov       rcx,rbx
000F5FB6: call      0x18010c39c
000F5FBB: jmp       0x1800f6037
000F5FBD: mov       rcx,QWORD PTR [rsi+0x48]
000F5FC1: lea       rdx,[rbp-0x60]
000F5FC5: movups    xmm0,XMMWORD PTR [rdi]
000F5FC8: movups    XMMWORD PTR [rdx],xmm0
000F5FCB: movups    xmm1,XMMWORD PTR [rdi+0x10]
000F5FCF: movups    XMMWORD PTR [rdx+0x10],xmm1
000F5FD3: movups    xmm0,XMMWORD PTR [rdi+0x20]
000F5FD7: movups    XMMWORD PTR [rdx+0x20],xmm0
000F5FDB: movups    xmm1,XMMWORD PTR [rdi+0x30]
000F5FDF: movups    XMMWORD PTR [rdx+0x30],xmm1
000F5FE3: movups    xmm0,XMMWORD PTR [rdi+0x40]
000F5FE7: movups    XMMWORD PTR [rdx+0x40],xmm0
000F5FEB: movups    xmm1,XMMWORD PTR [rdi+0x50]
000F5FEF: movups    XMMWORD PTR [rdx+0x50],xmm1
000F5FF3: movups    xmm0,XMMWORD PTR [rdi+0x60]
000F5FF7: movups    XMMWORD PTR [rdx+0x60],xmm0
000F5FFB: movups    xmm0,XMMWORD PTR [rdi+0x70]
000F5FFF: movups    XMMWORD PTR [rdx+0x70],xmm0
000F6003: movups    xmm1,XMMWORD PTR [rdi+0x80]
000F600A: movups    XMMWORD PTR [rdx+0x80],xmm1
000F6011: movups    xmm0,XMMWORD PTR [rdi+0x90]
000F6018: movups    XMMWORD PTR [rdx+0x90],xmm0
000F601F: movups    xmm1,XMMWORD PTR [rdi+0xa0]
000F6026: movups    XMMWORD PTR [rdx+0xa0],xmm1
000F602D: mov       rax,QWORD PTR [rcx]
000F6030: lea       rdx,[rbp-0x60]
000F6034: call      QWORD PTR [rax+0x8]
000F6037: mov       rcx,QWORD PTR [rsi+0x70]
000F603B: test      rcx,rcx
000F603E: je        0x1800f60df
000F6044: mov       rax,QWORD PTR [rcx]
000F6047: call      QWORD PTR [rax+0x50]
000F604A: test      al,al
000F604C: je        0x1800f60df
000F6052: cmp       DWORD PTR [rsi+0x68],0x2
000F6056: je        0x1800f60df
000F605C: call      0x1800badc0
000F6061: test      al,al
000F6063: jne       0x1800f60df
000F6065: mov       rcx,QWORD PTR [rsi+0x70]
000F6069: lea       rax,[rbp-0x60]
000F606D: movups    xmm0,XMMWORD PTR [rdi]
000F6070: movups    XMMWORD PTR [rax],xmm0
000F6073: movups    xmm1,XMMWORD PTR [rdi+0x10]
000F6077: movups    XMMWORD PTR [rax+0x10],xmm1
000F607B: movups    xmm0,XMMWORD PTR [rdi+0x20]
000F607F: movups    XMMWORD PTR [rax+0x20],xmm0
000F6083: movups    xmm1,XMMWORD PTR [rdi+0x30]
000F6087: movups    XMMWORD PTR [rax+0x30],xmm1
000F608B: movups    xmm0,XMMWORD PTR [rdi+0x40]
000F608F: movups    XMMWORD PTR [rax+0x40],xmm0
000F6093: movups    xmm1,XMMWORD PTR [rdi+0x50]
000F6097: movups    XMMWORD PTR [rax+0x50],xmm1
000F609B: movups    xmm0,XMMWORD PTR [rdi+0x60]
000F609F: movups    XMMWORD PTR [rax+0x60],xmm0
000F60A3: movups    xmm1,XMMWORD PTR [rdi+0x70]
000F60A7: movups    XMMWORD PTR [rax+0x70],xmm1
000F60AB: movups    xmm0,XMMWORD PTR [rdi+0x80]
000F60B2: movups    XMMWORD PTR [rax+0x80],xmm0
000F60B9: movups    xmm1,XMMWORD PTR [rdi+0x90]
000F60C0: movups    XMMWORD PTR [rax+0x90],xmm1
000F60C7: movups    xmm0,XMMWORD PTR [rdi+0xa0]
000F60CE: movups    XMMWORD PTR [rax+0xa0],xmm0
000F60D5: mov       rax,QWORD PTR [rcx]
000F60D8: lea       rdx,[rbp-0x60]
000F60DC: call      QWORD PTR [rax+0x8]
000F60DF: mov       rcx,QWORD PTR [rbp+0xb0]
000F60E6: xor       rcx,rsp
000F60E9: call      0x18010c270
000F60EE: mov       rbx,QWORD PTR [rsp+0x200]
000F60F6: add       rsp,0x1c0
000F60FD: pop       r15
000F60FF: pop       r14
000F6101: pop       rdi
000F6102: pop       rsi
000F6103: pop       rbp
000F6104: ret       
