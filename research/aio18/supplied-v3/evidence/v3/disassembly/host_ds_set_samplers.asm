; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x2A8390..0x2A83DC; unnamed
002A8390: mov       QWORD PTR [rsp+0x8],rbx
002A8395: mov       QWORD PTR [rsp+0x10],rsi
002A839A: push      rdi
002A839B: sub       rsp,0xa0
002A83A2: mov       rsi,rcx
002A83A5: mov       ebx,r8d
002A83A8: lea       rcx,[rsp+0x20]
002A83AD: mov       edi,edx
002A83AF: call      0x1802a8570
002A83B4: lea       r9,[rsp+0x20]
002A83B9: mov       r8d,ebx
002A83BC: mov       edx,edi
002A83BE: mov       rcx,rsi
002A83C1: call      QWORD PTR [rip+0x1d5091]        # 0x18047d458
002A83C7: lea       r11,[rsp+0xa0]
002A83CF: mov       rbx,QWORD PTR [r11+0x10]
002A83D3: mov       rsi,QWORD PTR [r11+0x18]
002A83D7: mov       rsp,r11
002A83DA: pop       rdi
002A83DB: ret       
