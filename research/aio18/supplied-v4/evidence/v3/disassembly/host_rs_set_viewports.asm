; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x2A95F0..0x2A973A; unnamed
002A95F0: mov       QWORD PTR [rsp+0x8],rbx
002A95F5: mov       QWORD PTR [rsp+0x10],rsi
002A95FA: push      rdi
002A95FB: sub       rsp,0x40
002A95FF: cmp       BYTE PTR [rip+0x1b7d9e],0x0        # 0x1804613a4
002A9606: mov       rbx,r8
002A9609: mov       edi,edx
002A960B: mov       rsi,rcx
002A960E: je        0x1802a9681
002A9610: call      0x180152f00
002A9615: mov       rcx,rax
002A9618: call      0x18025edd0
002A961D: test      al,al
002A961F: je        0x1802a9681
002A9621: mov       rax,QWORD PTR [rip+0xbd77b8]        # 0x180e80de0
002A9628: lea       r8,[rsp+0x20]
002A962D: vxorps    xmm0,xmm0,xmm0
002A9631: vxorps    xmm1,xmm1,xmm1
002A9635: vcvtsi2ss xmm0,xmm0,DWORD PTR [rax+0x278]
002A963D: vcvtsi2ss xmm1,xmm1,DWORD PTR [rax+0x27c]
002A9645: vmovss    DWORD PTR [rsp+0x28],xmm0
002A964B: vmovss    xmm0,DWORD PTR [rbx]
002A964F: vmovss    DWORD PTR [rsp+0x2c],xmm1
002A9655: vmovss    xmm1,DWORD PTR [rbx+0x4]
002A965A: vmovss    DWORD PTR [rsp+0x20],xmm0
002A9660: vmovss    xmm0,DWORD PTR [rbx+0x14]
002A9665: vmovss    DWORD PTR [rsp+0x24],xmm1
002A966B: vmovss    xmm1,DWORD PTR [rbx+0x10]
002A9670: vmovss    DWORD PTR [rsp+0x34],xmm0
002A9676: vmovss    DWORD PTR [rsp+0x30],xmm1
002A967C: jmp       0x1802a971f
002A9681: cmp       BYTE PTR [rip+0xbd7a0a],0x0        # 0x180e81092
002A9688: je        0x1802a971c
002A968E: cmp       edi,0x1
002A9691: jne       0x1802a971c
002A9697: mov       rax,QWORD PTR [rip+0xbd7742]        # 0x180e80de0
002A969E: vxorps    xmm0,xmm0,xmm0
002A96A2: vcvtsi2ss xmm0,xmm0,DWORD PTR [rax+0x278]
002A96AA: vucomiss  xmm0,DWORD PTR [rbx+0x8]
002A96AF: jne       0x1802a971c
002A96B1: vxorps    xmm0,xmm0,xmm0
002A96B5: vcvtsi2ss xmm0,xmm0,DWORD PTR [rax+0x27c]
002A96BD: vucomiss  xmm0,DWORD PTR [rbx+0xc]
002A96C2: jne       0x1802a971c
002A96C4: vxorps    xmm0,xmm0,xmm0
002A96C8: vcvtsi2ss xmm0,xmm0,DWORD PTR [rax+0x270]
002A96D0: vxorps    xmm1,xmm1,xmm1
002A96D4: vcvtsi2ss xmm1,xmm1,DWORD PTR [rax+0x274]
002A96DC: vmovss    DWORD PTR [rsp+0x28],xmm0
002A96E2: vmovss    xmm0,DWORD PTR [rbx]
002A96E6: vmovss    DWORD PTR [rsp+0x2c],xmm1
002A96EC: vmovss    xmm1,DWORD PTR [rbx+0x4]
002A96F1: vmovss    DWORD PTR [rsp+0x20],xmm0
002A96F7: vmovss    xmm0,DWORD PTR [rbx+0x14]
002A96FC: vmovss    DWORD PTR [rsp+0x24],xmm1
002A9702: vmovss    xmm1,DWORD PTR [rbx+0x10]
002A9707: vmovss    DWORD PTR [rsp+0x34],xmm0
002A970D: vmovss    DWORD PTR [rsp+0x30],xmm1
002A9713: lea       r8,[rsp+0x20]
002A9718: mov       edx,edi
002A971A: jmp       0x1802a9721
002A971C: mov       r8,rbx
002A971F: mov       edx,edi
002A9721: mov       rcx,rsi
002A9724: call      QWORD PTR [rip+0x1d3d0e]        # 0x18047d438
002A972A: mov       rbx,QWORD PTR [rsp+0x50]
002A972F: mov       rsi,QWORD PTR [rsp+0x58]
002A9734: add       rsp,0x40
002A9738: pop       rdi
002A9739: ret       
