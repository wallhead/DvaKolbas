; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x2A8450..0x2A849C; unnamed
002A8450: mov       QWORD PTR [rsp+0x8],rbx
002A8455: mov       QWORD PTR [rsp+0x10],rsi
002A845A: push      rdi
002A845B: sub       rsp,0xa0
002A8462: mov       rsi,rcx
002A8465: mov       ebx,r8d
002A8468: lea       rcx,[rsp+0x20]
002A846D: mov       edi,edx
002A846F: call      0x1802a8570
002A8474: lea       r9,[rsp+0x20]
002A8479: mov       r8d,ebx
002A847C: mov       edx,edi
002A847E: mov       rcx,rsi
002A8481: call      QWORD PTR [rip+0x1d4fc1]        # 0x18047d448
002A8487: lea       r11,[rsp+0xa0]
002A848F: mov       rbx,QWORD PTR [r11+0x10]
002A8493: mov       rsi,QWORD PTR [r11+0x18]
002A8497: mov       rsp,r11
002A849A: pop       rdi
002A849B: ret       
