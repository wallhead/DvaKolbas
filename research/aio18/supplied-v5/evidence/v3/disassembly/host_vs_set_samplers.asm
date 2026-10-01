; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x2A84B0..0x2A84FC; unnamed
002A84B0: mov       QWORD PTR [rsp+0x8],rbx
002A84B5: mov       QWORD PTR [rsp+0x10],rsi
002A84BA: push      rdi
002A84BB: sub       rsp,0xa0
002A84C2: mov       rsi,rcx
002A84C5: mov       ebx,r8d
002A84C8: lea       rcx,[rsp+0x20]
002A84CD: mov       edi,edx
002A84CF: call      0x1802a8570
002A84D4: lea       r9,[rsp+0x20]
002A84D9: mov       r8d,ebx
002A84DC: mov       edx,edi
002A84DE: mov       rcx,rsi
002A84E1: call      QWORD PTR [rip+0x1d4f89]        # 0x18047d470
002A84E7: lea       r11,[rsp+0xa0]
002A84EF: mov       rbx,QWORD PTR [r11+0x10]
002A84F3: mov       rsi,QWORD PTR [r11+0x18]
002A84F7: mov       rsp,r11
002A84FA: pop       rdi
002A84FB: ret       
