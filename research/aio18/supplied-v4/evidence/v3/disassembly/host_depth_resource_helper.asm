; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x293A10..0x293C26; unnamed
00293A10: mov       QWORD PTR [rsp+0x8],rbx
00293A15: mov       QWORD PTR [rsp+0x18],rsi
00293A1A: mov       QWORD PTR [rsp+0x20],rdi
00293A1F: push      rbp
00293A20: push      r14
00293A22: push      r15
00293A24: lea       rbp,[rsp-0x3f]
00293A29: sub       rsp,0xd0
00293A30: mov       rsi,rcx
00293A33: mov       r14d,r9d
00293A36: mov       rcx,QWORD PTR [rdx]
00293A39: mov       r15d,r8d
00293A3C: mov       rdi,rdx
00293A3F: test      rcx,rcx
00293A42: je        0x180293ad6
00293A48: mov       rax,QWORD PTR [rcx]
00293A4B: add       rdx,0x28
00293A4F: call      QWORD PTR [rax+0x50]
00293A52: mov       eax,DWORD PTR [rdi+0x50]
00293A55: vmovups   ymm1,YMMWORD PTR [rdi+0x28]
00293A5A: vmovsd    xmm0,QWORD PTR [rdi+0x48]
00293A5F: mov       DWORD PTR [rbp+0x37],eax
00293A62: vmovd     eax,xmm1
00293A66: vmovsd    QWORD PTR [rbp+0x2f],xmm0
00293A6B: cmp       eax,r15d
00293A6E: jne       0x180293ad6
00293A70: vmovq     rax,xmm1
00293A75: shr       rax,0x20
00293A79: cmp       eax,r14d
00293A7C: jne       0x180293ad6
00293A7E: mov       rcx,QWORD PTR [rsi]
00293A81: vextractf128 xmm0,ymm1,0x1
00293A87: vmovd     eax,xmm0
00293A8B: cmp       eax,DWORD PTR [rcx+0x10]
00293A8E: jne       0x180293ad6
00293A90: vpextrd   eax,xmm1,0x2
00293A96: cmp       eax,DWORD PTR [rcx+0x8]
00293A99: jne       0x180293ad6
00293A9B: vpextrq   rax,xmm1,0x1
00293AA1: shr       rax,0x20
00293AA5: cmp       eax,DWORD PTR [rcx+0xc]
00293AA8: jne       0x180293ad6
00293AAA: vextractf128 xmm0,ymm1,0x1
00293AB0: vmovq     rax,xmm0
00293AB5: shr       rax,0x20
00293AB9: cmp       eax,DWORD PTR [rcx+0x14]
00293ABC: jne       0x180293ad6
00293ABE: vextractf128 xmm0,ymm1,0x1
00293AC4: vpextrd   eax,xmm0,0x2
00293ACA: cmp       eax,DWORD PTR [rcx+0x18]
00293ACD: jne       0x180293ad6
00293ACF: xor       al,al
00293AD1: jmp       0x180293c06
00293AD6: mov       rcx,QWORD PTR [rdi+0x10]
00293ADA: xor       ebx,ebx
00293ADC: test      rcx,rcx
00293ADF: je        0x180293aee
00293AE1: mov       rax,QWORD PTR [rcx]
00293AE4: vzeroupper 
00293AE7: call      QWORD PTR [rax+0x10]
00293AEA: mov       QWORD PTR [rdi+0x10],rbx
00293AEE: mov       rcx,QWORD PTR [rdi+0x18]
00293AF2: test      rcx,rcx
00293AF5: je        0x180293b04
00293AF7: mov       rax,QWORD PTR [rcx]
00293AFA: vzeroupper 
00293AFD: call      QWORD PTR [rax+0x10]
00293B00: mov       QWORD PTR [rdi+0x18],rbx
00293B04: mov       rcx,QWORD PTR [rdi+0x20]
00293B08: test      rcx,rcx
00293B0B: je        0x180293b1a
00293B0D: mov       rax,QWORD PTR [rcx]
00293B10: vzeroupper 
00293B13: call      QWORD PTR [rax+0x10]
00293B16: mov       QWORD PTR [rdi+0x20],rbx
00293B1A: mov       rcx,QWORD PTR [rdi]
00293B1D: test      rcx,rcx
00293B20: je        0x180293b2e
00293B22: mov       rax,QWORD PTR [rcx]
00293B25: vzeroupper 
00293B28: call      QWORD PTR [rax+0x10]
00293B2B: mov       QWORD PTR [rdi],rbx
00293B2E: mov       rax,QWORD PTR [rsi]
00293B31: lea       rdx,[rbp-0x21]
00293B35: mov       r9,rdi
00293B38: xor       r8d,r8d
00293B3B: vmovups   ymm0,YMMWORD PTR [rax]
00293B3F: vmovups   YMMWORD PTR [rbp-0x21],ymm0
00293B44: vmovsd    xmm1,QWORD PTR [rax+0x20]
00293B49: vmovsd    QWORD PTR [rbp-0x1],xmm1
00293B4E: mov       eax,DWORD PTR [rax+0x28]
00293B51: mov       DWORD PTR [rbp+0x7],eax
00293B54: mov       rax,QWORD PTR [rsi+0x8]
00293B58: mov       DWORD PTR [rbp-0x21],r15d
00293B5C: mov       DWORD PTR [rbp-0x1d],r14d
00293B60: mov       rcx,QWORD PTR [rax+0x1678]
00293B67: mov       rax,QWORD PTR [rcx]
00293B6A: vzeroupper 
00293B6D: call      QWORD PTR [rax+0x28]
00293B70: mov       ebx,eax
00293B72: test      eax,eax
00293B74: jns       0x180293bfd
00293B7A: mov       DWORD PTR [rbp+0x67],eax
00293B7D: mov       eax,DWORD PTR [rbp-0x11]
00293B80: mov       DWORD PTR [rbp-0x51],eax
00293B83: call      0x180222050
00293B88: lea       rcx,[rip+0x1725a1]        # 0x180406130
00293B8F: mov       DWORD PTR [rbp-0x41],0xb9d
00293B96: mov       QWORD PTR [rbp-0x49],rcx
00293B9A: lea       r9,[rbp-0x31]
00293B9E: mov       ecx,DWORD PTR [rbp+0x1b]
00293BA1: lea       rdx,[rbp+0xf]
00293BA5: mov       DWORD PTR [rbp-0x3d],ecx
00293BA8: lea       rcx,[rip+0x1737d1]        # 0x180407380
00293BAF: vmovups   xmm0,XMMWORD PTR [rbp-0x49]
00293BB4: mov       QWORD PTR [rbp-0x39],rcx
00293BB8: lea       rcx,[rip+0x173879]        # 0x180407438
00293BBF: vmovsd    xmm1,QWORD PTR [rbp-0x39]
00293BC4: mov       QWORD PTR [rbp-0x31],rcx
00293BC8: lea       rcx,[rbp+0x67]
00293BCC: mov       QWORD PTR [rsp+0x30],rcx
00293BD1: lea       rcx,[rbp-0x51]
00293BD5: mov       QWORD PTR [rsp+0x28],rcx
00293BDA: lea       rcx,[rbp+0x7f]
00293BDE: mov       QWORD PTR [rsp+0x20],rcx
00293BE3: mov       rcx,rax
00293BE6: mov       QWORD PTR [rbp-0x29],0x25
00293BEE: vmovups   XMMWORD PTR [rbp+0xf],xmm0
00293BF3: vmovsd    QWORD PTR [rbp+0x1f],xmm1
00293BF8: call      0x180196600
00293BFD: shr       ebx,0x1f
00293C00: xor       bl,0x1
00293C03: movzx     eax,bl
00293C06: vzeroupper 
00293C09: lea       r11,[rsp+0xd0]
00293C11: mov       rbx,QWORD PTR [r11+0x20]
00293C15: mov       rsi,QWORD PTR [r11+0x30]
00293C19: mov       rdi,QWORD PTR [r11+0x38]
00293C1D: mov       rsp,r11
00293C20: pop       r15
00293C22: pop       r14
00293C24: pop       rbp
00293C25: ret       
