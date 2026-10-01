; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x2A8510..0x2A855C; unnamed
002A8510: mov       QWORD PTR [rsp+0x8],rbx
002A8515: mov       QWORD PTR [rsp+0x10],rsi
002A851A: push      rdi
002A851B: sub       rsp,0xa0
002A8522: mov       rsi,rcx
002A8525: mov       ebx,r8d
002A8528: lea       rcx,[rsp+0x20]
002A852D: mov       edi,edx
002A852F: call      0x1802a8570
002A8534: lea       r9,[rsp+0x20]
002A8539: mov       r8d,ebx
002A853C: mov       edx,edi
002A853E: mov       rcx,rsi
002A8541: call      QWORD PTR [rip+0x1d4f21]        # 0x18047d468
002A8547: lea       r11,[rsp+0xa0]
002A854F: mov       rbx,QWORD PTR [r11+0x10]
002A8553: mov       rsi,QWORD PTR [r11+0x18]
002A8557: mov       rsp,r11
002A855A: pop       rdi
002A855B: ret       
