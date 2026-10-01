; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x2A68C0..0x2A6A39; unnamed
002A68C0: mov       QWORD PTR [rsp+0x8],rbx
002A68C5: mov       QWORD PTR [rsp+0x10],rsi
002A68CA: mov       QWORD PTR [rsp+0x18],rdi
002A68CF: mov       QWORD PTR [rsp+0x20],r14
002A68D4: push      rbp
002A68D5: mov       rbp,rsp
002A68D8: sub       rsp,0x80
002A68DF: mov       rax,QWORD PTR [rbp+0x30]
002A68E3: mov       rdi,QWORD PTR [rbp+0x38]
002A68E7: mov       r9,QWORD PTR [r9+0x10]
002A68EB: mov       QWORD PTR [rsp+0x28],rdi
002A68F0: mov       QWORD PTR [rsp+0x20],rax
002A68F5: mov       BYTE PTR [rip+0xbda794],0x1        # 0x180e81090
002A68FC: call      QWORD PTR [rip+0x1d6b16]        # 0x18047d418
002A6902: mov       rdx,QWORD PTR [rip+0xbda4d7]        # 0x180e80de0
002A6909: lea       rsi,[rip+0x163308]        # 0x180409c18 ; 'F:\\GithubMods\\DLSS\\SkyrimUpscaler\\src\\UpscalerHooks.cpp'
002A6910: mov       rcx,QWORD PTR [rdi]
002A6913: movzx     ebx,al
002A6916: mov       BYTE PTR [rbp+0x38],al
002A6919: lea       r14,[rip+0x163ec0]        # 0x18040a7e0 ; 'bool __cdecl hk_ReShadeCreateEffectRuntime(enum reshade::api::device_api,void *,void *,void *,const char *,struct reshade::api::effect_runtime **)'
002A6920: mov       QWORD PTR [rdx+0x1690],rcx
002A6927: mov       BYTE PTR [rdx+0x1689],0x1
002A692E: test      al,al
002A6930: je        0x1802a69bf
002A6936: call      0x180222050
002A693B: mov       ecx,DWORD PTR [rbp-0x14]
002A693E: lea       r9,[rbp-0x50]
002A6942: mov       DWORD PTR [rbp-0x34],ecx
002A6945: lea       rdx,[rbp-0x20]
002A6949: lea       rcx,[rip+0x163f90]        # 0x18040a8e0 ; 'Hooked ReShade render_effects'
002A6950: mov       QWORD PTR [rbp-0x40],rsi
002A6954: mov       QWORD PTR [rbp-0x50],rcx
002A6958: mov       r8d,0x2
002A695E: mov       DWORD PTR [rbp-0x38],0x655
002A6965: mov       rcx,rax
002A6968: vmovups   xmm0,XMMWORD PTR [rbp-0x40]
002A696D: mov       QWORD PTR [rbp-0x30],r14
002A6971: vmovsd    xmm1,QWORD PTR [rbp-0x30]
002A6976: mov       QWORD PTR [rbp-0x48],0x1d
002A697E: vmovups   XMMWORD PTR [rbp-0x20],xmm0
002A6983: vmovsd    QWORD PTR [rbp-0x10],xmm1
002A6988: call      0x180151990
002A698D: mov       rcx,QWORD PTR [rdi]
002A6990: lea       rdx,[rip+0x109]        # 0x1802a6aa0
002A6997: mov       r8d,0x9
002A699D: mov       rcx,QWORD PTR [rcx]
002A69A0: call      0x1801be000
002A69A5: mov       rcx,QWORD PTR [rip+0xbda434]        # 0x180e80de0
002A69AC: mov       QWORD PTR [rip+0x1d6a5d],rax        # 0x18047d410
002A69B3: mov       rcx,QWORD PTR [rcx+0x1680]
002A69BA: call      0x1802a9a00
002A69BF: call      0x180222050
002A69C4: mov       ecx,DWORD PTR [rbp-0x14]
002A69C7: lea       r9,[rbp-0x50]
002A69CB: mov       DWORD PTR [rbp-0x34],ecx
002A69CE: lea       rdx,[rbp-0x20]
002A69D2: lea       rcx,[rip+0x163ed7]        # 0x18040a8b0 ; 'hk_ReShadeCreateEffectRuntime result: {}'
002A69D9: mov       QWORD PTR [rbp-0x40],rsi
002A69DD: mov       QWORD PTR [rbp-0x50],rcx
002A69E1: lea       rcx,[rbp+0x38]
002A69E5: mov       QWORD PTR [rsp+0x20],rcx
002A69EA: mov       rcx,rax
002A69ED: mov       DWORD PTR [rbp-0x38],0x65a
002A69F4: vmovups   xmm0,XMMWORD PTR [rbp-0x40]
002A69F9: mov       QWORD PTR [rbp-0x30],r14
002A69FD: vmovsd    xmm1,QWORD PTR [rbp-0x30]
002A6A02: mov       QWORD PTR [rbp-0x48],0x28
002A6A0A: vmovups   XMMWORD PTR [rbp-0x20],xmm0
002A6A0F: vmovsd    QWORD PTR [rbp-0x10],xmm1
002A6A14: call      0x180152360
002A6A19: lea       r11,[rsp+0x80]
002A6A21: movzx     eax,bl
002A6A24: mov       rbx,QWORD PTR [r11+0x10]
002A6A28: mov       rsi,QWORD PTR [r11+0x18]
002A6A2C: mov       rdi,QWORD PTR [r11+0x20]
002A6A30: mov       r14,QWORD PTR [r11+0x28]
002A6A34: mov       rsp,r11
002A6A37: pop       rbp
002A6A38: ret       
