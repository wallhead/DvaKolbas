; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x2A8330..0x2A837C; unnamed
002A8330: mov       QWORD PTR [rsp+0x8],rbx
002A8335: mov       QWORD PTR [rsp+0x10],rsi
002A833A: push      rdi
002A833B: sub       rsp,0xa0
002A8342: mov       rsi,rcx
002A8345: mov       ebx,r8d
002A8348: lea       rcx,[rsp+0x20]
002A834D: mov       edi,edx
002A834F: call      0x1802a8570
002A8354: lea       r9,[rsp+0x20]
002A8359: mov       r8d,ebx
002A835C: mov       edx,edi
002A835E: mov       rcx,rsi
002A8361: call      QWORD PTR [rip+0x1d50f9]        # 0x18047d460
002A8367: lea       r11,[rsp+0xa0]
002A836F: mov       rbx,QWORD PTR [r11+0x10]
002A8373: mov       rsi,QWORD PTR [r11+0x18]
002A8377: mov       rsp,r11
002A837A: pop       rdi
002A837B: ret       
