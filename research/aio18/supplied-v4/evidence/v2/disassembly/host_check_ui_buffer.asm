; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x2A4730..0x2A4CDA; unnamed
002A4730: mov       QWORD PTR [rsp+0x10],rbx
002A4735: mov       QWORD PTR [rsp+0x18],rsi
002A473A: mov       QWORD PTR [rsp+0x20],rdi
002A473F: push      rbp
002A4740: push      r12
002A4742: push      r13
002A4744: push      r14
002A4746: push      r15
002A4748: lea       rbp,[rsp-0x37]
002A474D: sub       rsp,0xb0
002A4754: movzx     eax,BYTE PTR [rcx+0x4fa]
002A475B: mov       rdi,rcx
002A475E: test      al,al
002A4760: jne       0x1802a4791
002A4762: cmp       BYTE PTR [rcx+0x3d0],al
002A4768: jne       0x1802a4791
002A476A: cmp       DWORD PTR [rcx+0x3dc],0x3
002A4771: je        0x1802a4791
002A4773: cmp       BYTE PTR [rcx+0x2a8],al
002A4779: je        0x1802a4783
002A477B: cmp       BYTE PTR [rcx+0x2c5],al
002A4781: jne       0x1802a4791
002A4783: cmp       BYTE PTR [rcx+0x4e4],0x0
002A478A: jne       0x1802a4791
002A478C: xor       sil,sil
002A478F: jmp       0x1802a4794
002A4791: mov       sil,0x1
002A4794: cmp       DWORD PTR [rcx+0x4fc],0x0
002A479B: jne       0x1802a47aa
002A479D: test      al,al
002A479F: je        0x1802a47aa
002A47A1: cmp       DWORD PTR [rcx+0x3dc],0x0
002A47A8: jg        0x1802a47dc
002A47AA: call      0x180293f20
002A47AF: test      al,al
002A47B1: jne       0x1802a47dc
002A47B3: cmp       BYTE PTR [rdi+0x3d0],al
002A47B9: je        0x1802a47c5
002A47BB: cmp       QWORD PTR [rdi+0x1570],0x0
002A47C3: jne       0x1802a47dc
002A47C5: cmp       BYTE PTR [rdi+0x4e4],0x0
002A47CC: je        0x1802a47d7
002A47CE: cmp       DWORD PTR [rdi+0x4fc],0x0
002A47D5: je        0x1802a47dc
002A47D7: xor       r14b,r14b
002A47DA: jmp       0x1802a47df
002A47DC: mov       r14b,0x1
002A47DF: mov       rcx,QWORD PTR [rdi+0x998]
002A47E6: xor       r15d,r15d
002A47E9: test      rcx,rcx
002A47EC: je        0x1802a47fb
002A47EE: mov       rax,QWORD PTR [rcx]
002A47F1: call      QWORD PTR [rax+0x10]
002A47F4: mov       QWORD PTR [rdi+0x998],r15
002A47FB: mov       rcx,QWORD PTR [rdi+0x9a0]
002A4802: test      rcx,rcx
002A4805: je        0x1802a4814
002A4807: mov       rax,QWORD PTR [rcx]
002A480A: call      QWORD PTR [rax+0x10]
002A480D: mov       QWORD PTR [rdi+0x9a0],r15
002A4814: mov       rcx,QWORD PTR [rdi+0x9a8]
002A481B: test      rcx,rcx
002A481E: je        0x1802a482d
002A4820: mov       rax,QWORD PTR [rcx]
002A4823: call      QWORD PTR [rax+0x10]
002A4826: mov       QWORD PTR [rdi+0x9a8],r15
002A482D: mov       rcx,QWORD PTR [rdi+0x988]
002A4834: test      rcx,rcx
002A4837: je        0x1802a4846
002A4839: mov       rax,QWORD PTR [rcx]
002A483C: call      QWORD PTR [rax+0x10]
002A483F: mov       QWORD PTR [rdi+0x988],r15
002A4846: mov       rcx,QWORD PTR [rdi+0x9f0]
002A484D: test      rcx,rcx
002A4850: je        0x1802a485f
002A4852: mov       rax,QWORD PTR [rcx]
002A4855: call      QWORD PTR [rax+0x10]
002A4858: mov       QWORD PTR [rdi+0x9f0],r15
002A485F: mov       rcx,QWORD PTR [rdi+0x9f8]
002A4866: test      rcx,rcx
002A4869: je        0x1802a4878
002A486B: mov       rax,QWORD PTR [rcx]
002A486E: call      QWORD PTR [rax+0x10]
002A4871: mov       QWORD PTR [rdi+0x9f8],r15
002A4878: mov       rcx,QWORD PTR [rdi+0xa00]
002A487F: test      rcx,rcx
002A4882: je        0x1802a4891
002A4884: mov       rax,QWORD PTR [rcx]
002A4887: call      QWORD PTR [rax+0x10]
002A488A: mov       QWORD PTR [rdi+0xa00],r15
002A4891: mov       rcx,QWORD PTR [rdi+0x9e0]
002A4898: test      rcx,rcx
002A489B: je        0x1802a48aa
002A489D: mov       rax,QWORD PTR [rcx]
002A48A0: call      QWORD PTR [rax+0x10]
002A48A3: mov       QWORD PTR [rdi+0x9e0],r15
002A48AA: lea       r12,[rip+0x16187f]        # 0x180406130 ; 'F:\\GithubMods\\DLSS\\SkyrimUpscaler\\src\\SkyrimUpscaler.cpp'
002A48B1: mov       ebx,0x28
002A48B6: lea       r13,[rip+0x162c53]        # 0x180407510 ; 'void __cdecl SkyrimUpscaler::CheckUIBuffer(void)'
002A48BD: test      sil,sil
002A48C0: je        0x1802a4986
002A48C6: mov       rsi,QWORD PTR [rdi+0x1670]
002A48CD: mov       rcx,QWORD PTR [rsi+0x40]
002A48D1: test      rcx,rcx
002A48D4: je        0x1802a48e0
002A48D6: mov       rax,QWORD PTR [rcx]
002A48D9: lea       rdx,[rsi+0x68]
002A48DD: call      QWORD PTR [rax+0x50]
002A48E0: vmovups   ymm0,YMMWORD PTR [rbx+rsi*1+0x40]
002A48E6: mov       rcx,QWORD PTR [rdi+0x1678]
002A48ED: lea       r9,[rdi+0x988]
002A48F4: vmovups   YMMWORD PTR [rbp+0x7],ymm0
002A48F9: vmovsd    xmm1,QWORD PTR [rbx+rsi*1+0x60]
002A48FF: vmovsd    QWORD PTR [rbp+0x27],xmm1
002A4904: mov       rax,rbx
002A4907: mov       DWORD PTR [rbp+0x27],0xa8
002A490E: mov       DWORD PTR [rbp+0x2f],0x2
002A4915: lea       rdx,[rbp+0x7]
002A4919: mov       rax,QWORD PTR [rcx]
002A491C: xor       r8d,r8d
002A491F: vzeroupper 
002A4922: call      QWORD PTR [rax+0x28]
002A4925: mov       DWORD PTR [rbp+0x67],eax
002A4928: test      eax,eax
002A492A: jns       0x1802a4986
002A492C: call      0x180222050
002A4931: mov       ecx,DWORD PTR [rbp-0xd]
002A4934: lea       r9,[rbp-0x49]
002A4938: mov       DWORD PTR [rbp-0x2d],ecx
002A493B: lea       rdx,[rbp-0x19]
002A493F: lea       rcx,[rip+0x162c72]        # 0x1804075b8 ; 'Create mTempUIColor failed! ErrorCode: {}'
002A4946: mov       QWORD PTR [rbp-0x39],r12
002A494A: mov       QWORD PTR [rbp-0x49],rcx
002A494E: lea       rcx,[rbp+0x67]
002A4952: mov       QWORD PTR [rsp+0x20],rcx
002A4957: mov       rcx,rax
002A495A: mov       DWORD PTR [rbp-0x31],0xbdf
002A4961: vmovups   xmm0,XMMWORD PTR [rbp-0x39]
002A4966: mov       QWORD PTR [rbp-0x29],r13
002A496A: vmovsd    xmm1,QWORD PTR [rbp-0x29]
002A496F: mov       QWORD PTR [rbp-0x41],0x29
002A4977: vmovups   XMMWORD PTR [rbp-0x19],xmm0
002A497C: vmovsd    QWORD PTR [rbp-0x9],xmm1
002A4981: call      0x180196e10
002A4986: test      r14b,r14b
002A4989: je        0x1802a4a48
002A498F: mov       rsi,QWORD PTR [rdi+0x1670]
002A4996: mov       rcx,QWORD PTR [rsi+0x40]
002A499A: test      rcx,rcx
002A499D: je        0x1802a49a9
002A499F: mov       rax,QWORD PTR [rcx]
002A49A2: lea       rdx,[rsi+0x68]
002A49A6: call      QWORD PTR [rax+0x50]
002A49A9: vmovups   ymm0,YMMWORD PTR [rbx+rsi*1+0x40]
002A49AF: mov       rcx,QWORD PTR [rdi+0x1678]
002A49B6: lea       r9,[rdi+0x9e0]
002A49BD: vmovups   YMMWORD PTR [rbp+0x7],ymm0
002A49C2: vmovsd    xmm1,QWORD PTR [rbx+rsi*1+0x60]
002A49C8: vmovsd    QWORD PTR [rbp+0x27],xmm1
002A49CD: mov       rax,rbx
002A49D0: mov       DWORD PTR [rbp+0x27],ebx
002A49D3: mov       DWORD PTR [rbp+0x2f],r15d
002A49D7: lea       rdx,[rbp+0x7]
002A49DB: mov       rax,QWORD PTR [rcx]
002A49DE: xor       r8d,r8d
002A49E1: vzeroupper 
002A49E4: call      QWORD PTR [rax+0x28]
002A49E7: mov       DWORD PTR [rbp+0x67],eax
002A49EA: test      eax,eax
002A49EC: jns       0x1802a4a48
002A49EE: call      0x180222050
002A49F3: mov       ecx,DWORD PTR [rbp-0xd]
002A49F6: lea       r9,[rbp-0x49]
002A49FA: mov       DWORD PTR [rbp-0x2d],ecx
002A49FD: lea       rdx,[rbp-0x19]
002A4A01: lea       rcx,[rip+0x162b78]        # 0x180407580 ; 'Create mTempUIColorForRender failed! ErrorCode: {}'
002A4A08: mov       QWORD PTR [rbp-0x39],r12
002A4A0C: mov       QWORD PTR [rbp-0x49],rcx
002A4A10: lea       rcx,[rbp+0x67]
002A4A14: mov       QWORD PTR [rsp+0x20],rcx
002A4A19: mov       rcx,rax
002A4A1C: mov       DWORD PTR [rbp-0x31],0xbe7
002A4A23: vmovups   xmm0,XMMWORD PTR [rbp-0x39]
002A4A28: mov       QWORD PTR [rbp-0x29],r13
002A4A2C: vmovsd    xmm1,QWORD PTR [rbp-0x29]
002A4A31: mov       QWORD PTR [rbp-0x41],0x32
002A4A39: vmovups   XMMWORD PTR [rbp-0x19],xmm0
002A4A3E: vmovsd    QWORD PTR [rbp-0x9],xmm1
002A4A43: call      0x180196e10
002A4A48: cmp       BYTE PTR [rdi+0x343],r15b
002A4A4F: je        0x1802a4b05
002A4A55: mov       rcx,QWORD PTR [rdi+0xa38]
002A4A5C: test      rcx,rcx
002A4A5F: je        0x1802a4a80
002A4A61: mov       rax,QWORD PTR [rdi+0x778]
002A4A68: test      rax,rax
002A4A6B: je        0x1802a4a80
002A4A6D: cmp       rcx,rax
002A4A70: jne       0x1802a4a80
002A4A72: mov       dl,0x1
002A4A74: lea       rcx,[rdi+0xa38]
002A4A7B: call      0x180152cd0
002A4A80: cmp       QWORD PTR [rdi+0xa38],r15
002A4A87: jne       0x1802a4cb9
002A4A8D: mov       rsi,QWORD PTR [rdi+0x1670]
002A4A94: mov       rcx,QWORD PTR [rsi+0x40]
002A4A98: test      rcx,rcx
002A4A9B: je        0x1802a4aa7
002A4A9D: mov       rax,QWORD PTR [rcx]
002A4AA0: lea       rdx,[rsi+0x68]
002A4AA4: call      QWORD PTR [rax+0x50]
002A4AA7: vmovups   ymm0,YMMWORD PTR [rsi+rbx*1+0x40]
002A4AAD: mov       rcx,QWORD PTR [rdi+0x1678]
002A4AB4: lea       r9,[rdi+0xa38]
002A4ABB: vmovups   YMMWORD PTR [rbp+0x7],ymm0
002A4AC0: vmovsd    xmm1,QWORD PTR [rsi+rbx*1+0x60]
002A4AC6: vmovsd    QWORD PTR [rbp+0x27],xmm1
002A4ACB: mov       DWORD PTR [rbp+0x27],0xa8
002A4AD2: lea       rdx,[rbp+0x7]
002A4AD6: mov       DWORD PTR [rbp+0x2f],0x2
002A4ADD: xor       r8d,r8d
002A4AE0: mov       rax,QWORD PTR [rcx]
002A4AE3: vzeroupper 
002A4AE6: call      QWORD PTR [rax+0x28]
002A4AE9: mov       DWORD PTR [rbp+0x67],eax
002A4AEC: test      eax,eax
002A4AEE: jns       0x1802a4cb9
002A4AF4: call      0x180222050
002A4AF9: mov       DWORD PTR [rbp-0x31],0xbf3
002A4B00: jmp       0x1802a4c6b
002A4B05: vmovss    xmm0,DWORD PTR [rip+0x167227]        # 0x18040bd34
002A4B0D: vcomiss   xmm0,DWORD PTR [rdi+0x294]
002A4B15: jae       0x1802a4ba0
002A4B1B: cmp       QWORD PTR [rdi+0xa38],r15
002A4B22: jne       0x1802a4cb9
002A4B28: mov       rsi,QWORD PTR [rdi+0x1670]
002A4B2F: mov       rcx,QWORD PTR [rsi+0x40]
002A4B33: test      rcx,rcx
002A4B36: je        0x1802a4b42
002A4B38: mov       rax,QWORD PTR [rcx]
002A4B3B: lea       rdx,[rsi+0x68]
002A4B3F: call      QWORD PTR [rax+0x50]
002A4B42: vmovups   ymm0,YMMWORD PTR [rsi+rbx*1+0x40]
002A4B48: mov       rcx,QWORD PTR [rdi+0x1678]
002A4B4F: lea       r9,[rdi+0xa38]
002A4B56: vmovups   YMMWORD PTR [rbp+0x7],ymm0
002A4B5B: vmovsd    xmm1,QWORD PTR [rsi+rbx*1+0x60]
002A4B61: vmovsd    QWORD PTR [rbp+0x27],xmm1
002A4B66: mov       DWORD PTR [rbp+0x27],0xa8
002A4B6D: lea       rdx,[rbp+0x7]
002A4B71: mov       DWORD PTR [rbp+0x2f],0x2
002A4B78: xor       r8d,r8d
002A4B7B: mov       rax,QWORD PTR [rcx]
002A4B7E: vzeroupper 
002A4B81: call      QWORD PTR [rax+0x28]
002A4B84: mov       DWORD PTR [rbp+0x67],eax
002A4B87: test      eax,eax
002A4B89: jns       0x1802a4cb9
002A4B8F: call      0x180222050
002A4B94: mov       DWORD PTR [rbp-0x31],0xbfc
002A4B9B: jmp       0x1802a4c6b
002A4BA0: cmp       QWORD PTR [rdi+0x778],r15
002A4BA7: je        0x1802a4bd0
002A4BA9: mov       dl,0x1
002A4BAB: lea       rcx,[rdi+0xa38]
002A4BB2: call      0x180152cd0
002A4BB7: mov       rcx,QWORD PTR [rdi+0x778]
002A4BBE: mov       QWORD PTR [rdi+0xa38],rcx
002A4BC5: mov       rax,QWORD PTR [rcx]
002A4BC8: call      QWORD PTR [rax+0x8]
002A4BCB: jmp       0x1802a4cb9
002A4BD0: cmp       BYTE PTR [rdi+0x2a8],r15b
002A4BD7: je        0x1802a4be2
002A4BD9: cmp       BYTE PTR [rdi+0x2c5],r15b
002A4BE0: jne       0x1802a4bef
002A4BE2: cmp       BYTE PTR [rdi+0x4e4],r15b
002A4BE9: je        0x1802a4cb9
002A4BEF: cmp       QWORD PTR [rdi+0xa38],r15
002A4BF6: jne       0x1802a4cb9
002A4BFC: mov       rsi,QWORD PTR [rdi+0x1670]
002A4C03: mov       rcx,QWORD PTR [rsi+0x40]
002A4C07: test      rcx,rcx
002A4C0A: je        0x1802a4c16
002A4C0C: mov       rax,QWORD PTR [rcx]
002A4C0F: lea       rdx,[rsi+0x68]
002A4C13: call      QWORD PTR [rax+0x50]
002A4C16: vmovups   ymm0,YMMWORD PTR [rbx+rsi*1+0x40]
002A4C1C: mov       rcx,QWORD PTR [rdi+0x1678]
002A4C23: lea       r9,[rdi+0xa38]
002A4C2A: vmovups   YMMWORD PTR [rbp+0x7],ymm0
002A4C2F: vmovsd    xmm1,QWORD PTR [rbx+rsi*1+0x60]
002A4C35: vmovsd    QWORD PTR [rbp+0x27],xmm1
002A4C3A: mov       DWORD PTR [rbp+0x27],0xa8
002A4C41: lea       rdx,[rbp+0x7]
002A4C45: mov       DWORD PTR [rbp+0x2f],0x2
002A4C4C: xor       r8d,r8d
002A4C4F: mov       rax,QWORD PTR [rcx]
002A4C52: vzeroupper 
002A4C55: call      QWORD PTR [rax+0x28]
002A4C58: mov       DWORD PTR [rbp+0x67],eax
002A4C5B: test      eax,eax
002A4C5D: jns       0x1802a4cb9
002A4C5F: call      0x180222050
002A4C64: mov       DWORD PTR [rbp-0x31],0xc08
002A4C6B: mov       ecx,DWORD PTR [rbp-0xd]
002A4C6E: lea       r9,[rbp-0x49]
002A4C72: mov       DWORD PTR [rbp-0x2d],ecx
002A4C75: lea       rdx,[rbp-0x19]
002A4C79: lea       rcx,[rip+0x162990]        # 0x180407610 ; 'Create mTempHudlessColor failed! ErrorCode: {}'
002A4C80: mov       QWORD PTR [rbp-0x39],r12
002A4C84: vmovups   xmm0,XMMWORD PTR [rbp-0x39]
002A4C89: mov       QWORD PTR [rbp-0x49],rcx
002A4C8D: lea       rcx,[rbp+0x67]
002A4C91: mov       QWORD PTR [rsp+0x20],rcx
002A4C96: mov       rcx,rax
002A4C99: mov       QWORD PTR [rbp-0x29],r13
002A4C9D: vmovsd    xmm1,QWORD PTR [rbp-0x29]
002A4CA2: mov       QWORD PTR [rbp-0x41],0x2e
002A4CAA: vmovups   XMMWORD PTR [rbp-0x19],xmm0
002A4CAF: vmovsd    QWORD PTR [rbp-0x9],xmm1
002A4CB4: call      0x180196e10
002A4CB9: lea       r11,[rsp+0xb0]
002A4CC1: mov       rbx,QWORD PTR [r11+0x38]
002A4CC5: mov       rsi,QWORD PTR [r11+0x40]
002A4CC9: mov       rdi,QWORD PTR [r11+0x48]
002A4CCD: mov       rsp,r11
002A4CD0: pop       r15
002A4CD2: pop       r14
002A4CD4: pop       r13
002A4CD6: pop       r12
002A4CD8: pop       rbp
002A4CD9: ret       
