; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x2A9740..0x2A97D6; unnamed
002A9740: mov       QWORD PTR [rsp+0x8],rbx
002A9745: mov       QWORD PTR [rsp+0x10],rbp
002A974A: mov       QWORD PTR [rsp+0x18],rsi
002A974F: push      rdi
002A9750: sub       rsp,0x30
002A9754: cmp       BYTE PTR [rip+0xbd7937],0x0        # 0x180e81092
002A975B: mov       rdi,r9
002A975E: mov       ebx,r8d
002A9761: mov       esi,edx
002A9763: mov       rbp,rcx
002A9766: je        0x1802a97b0
002A9768: cmp       ebx,0x1
002A976B: jne       0x1802a97b0
002A976D: mov       rcx,QWORD PTR [r9]
002A9770: test      rcx,rcx
002A9773: je        0x1802a97b0
002A9775: mov       rax,QWORD PTR [rcx]
002A9778: lea       rdx,[rsp+0x20]
002A977D: call      QWORD PTR [rax+0x38]
002A9780: mov       rcx,QWORD PTR [rip+0xbd7659]        # 0x180e80de0
002A9787: mov       rax,QWORD PTR [rcx+0x7d0]
002A978E: cmp       QWORD PTR [rsp+0x20],rax
002A9793: jne       0x1802a97b0
002A9795: add       rcx,0x930
002A979C: call      0x18016c290
002A97A1: mov       QWORD PTR [rsp+0x28],rax
002A97A6: lea       r9,[rsp+0x28]
002A97AB: mov       r8d,ebx
002A97AE: jmp       0x1802a97b6
002A97B0: mov       r9,rdi
002A97B3: mov       r8d,ebx
002A97B6: mov       edx,esi
002A97B8: mov       rcx,rbp
002A97BB: call      QWORD PTR [rip+0x1d3c47]        # 0x18047d408
002A97C1: mov       rbx,QWORD PTR [rsp+0x40]
002A97C6: mov       rbp,QWORD PTR [rsp+0x48]
002A97CB: mov       rsi,QWORD PTR [rsp+0x50]
002A97D0: add       rsp,0x30
002A97D4: pop       rdi
002A97D5: ret       
