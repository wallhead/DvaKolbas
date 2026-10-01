; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x2A83F0..0x2A843C; unnamed
002A83F0: mov       QWORD PTR [rsp+0x8],rbx
002A83F5: mov       QWORD PTR [rsp+0x10],rsi
002A83FA: push      rdi
002A83FB: sub       rsp,0xa0
002A8402: mov       rsi,rcx
002A8405: mov       ebx,r8d
002A8408: lea       rcx,[rsp+0x20]
002A840D: mov       edi,edx
002A840F: call      0x1802a8570
002A8414: lea       r9,[rsp+0x20]
002A8419: mov       r8d,ebx
002A841C: mov       edx,edi
002A841E: mov       rcx,rsi
002A8421: call      QWORD PTR [rip+0x1d5029]        # 0x18047d450
002A8427: lea       r11,[rsp+0xa0]
002A842F: mov       rbx,QWORD PTR [r11+0x10]
002A8433: mov       rsi,QWORD PTR [r11+0x18]
002A8437: mov       rsp,r11
002A843A: pop       rdi
002A843B: ret       
