; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x2AA630..0x2AA762; unnamed
002AA630: mov       r11,rsp
002AA633: push      rdi
002AA634: sub       rsp,0x80
002AA63B: mov       rax,QWORD PTR [rip+0xbd679e]        # 0x180e80de0
002AA642: mov       rdi,rcx
002AA645: cmp       rcx,QWORD PTR [rax+0x1690]
002AA64C: jne       0x1802aa759
002AA652: cmp       BYTE PTR [rip+0xbd6946],0x0        # 0x180e80f9f
002AA659: je        0x1802aa759
002AA65F: mov       rcx,QWORD PTR [rip+0xbd68ba]        # 0x180e80f20
002AA666: test      rcx,rcx
002AA669: je        0x1802aa759
002AA66F: mov       rax,QWORD PTR [rcx]
002AA672: mov       QWORD PTR [r11+0x10],rbx
002AA676: lea       rbx,[rcx+0x40]
002AA67A: mov       QWORD PTR [r11+0x18],rsi
002AA67E: movzx     esi,BYTE PTR [rip+0xbd6a0c]        # 0x180e81091
002AA685: mov       BYTE PTR [rip+0xbd6913],0x0        # 0x180e80f9f
002AA68C: mov       BYTE PTR [rip+0xbd69fe],0x0        # 0x180e81091
002AA693: call      QWORD PTR [rax+0x120]
002AA699: mov       eax,eax
002AA69B: imul      rdx,rax,0x58
002AA69F: add       rdx,rbx
002AA6A2: call      0x1802941a0
002AA6A7: cmp       BYTE PTR [rip+0xbd68ed],0x0        # 0x180e80f9b
002AA6AE: mov       rbx,QWORD PTR [rsp+0x98]
002AA6B6: jne       0x1802aa74a
002AA6BC: mov       QWORD PTR [rsp+0x90],rdi
002AA6C4: call      0x180222050
002AA6C9: lea       rcx,[rip+0x15f548]        # 0x180409c18 ; 'F:\\GithubMods\\DLSS\\SkyrimUpscaler\\src\\UpscalerHooks.cpp'
002AA6D0: mov       DWORD PTR [rsp+0x48],0x139
002AA6D8: mov       QWORD PTR [rsp+0x40],rcx
002AA6DD: lea       r9,[rsp+0x30]
002AA6E2: mov       ecx,DWORD PTR [rsp+0x6c]
002AA6E6: lea       rdx,[rsp+0x60]
002AA6EB: mov       DWORD PTR [rsp+0x4c],ecx
002AA6EF: mov       r8d,0x2
002AA6F5: vmovups   xmm0,XMMWORD PTR [rsp+0x40]
002AA6FB: lea       rcx,[rip+0x15f6ce]        # 0x180409dd0 ; 'void __cdecl ReShadeFrameComplete(struct reshade::api::effect_runtime *)'
002AA702: mov       QWORD PTR [rsp+0x38],0x45
002AA70B: mov       QWORD PTR [rsp+0x50],rcx
002AA710: lea       rcx,[rip+0x15f669]        # 0x180409d80 ; 'ReShade final overlay included before FG/Direct submission runtime={}'
002AA717: vmovsd    xmm1,QWORD PTR [rsp+0x50]
002AA71D: mov       QWORD PTR [rsp+0x30],rcx
002AA722: lea       rcx,[rsp+0x90]
002AA72A: mov       QWORD PTR [rsp+0x20],rcx
002AA72F: mov       rcx,rax
002AA732: vmovups   XMMWORD PTR [rsp+0x60],xmm0
002AA738: vmovsd    QWORD PTR [rsp+0x70],xmm1
002AA73E: call      0x18017d110
002AA743: mov       BYTE PTR [rip+0xbd6851],0x1        # 0x180e80f9b
002AA74A: mov       BYTE PTR [rip+0xbd6940],sil        # 0x180e81091
002AA751: mov       rsi,QWORD PTR [rsp+0xa0]
002AA759: add       rsp,0x80
002AA760: pop       rdi
002AA761: ret       
