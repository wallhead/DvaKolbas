; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x2A1931..0x2A1A91; unnamed
002A1931: call      0x180222050
002A1936: lea       rcx,[rip+0x1647f3]        # 0x180406130
002A193D: mov       DWORD PTR [rsp+0x78],0x551
002A1945: mov       QWORD PTR [rsp+0x70],rcx
002A194A: lea       r9,[rbp+0x0]
002A194E: mov       ecx,DWORD PTR [rbp-0x64]
002A1951: lea       rdx,[rbp-0x40]
002A1955: mov       DWORD PTR [rsp+0x7c],ecx
002A1959: lea       rcx,[rip+0x1656a0]        # 0x180407000
002A1960: vmovups   xmm0,XMMWORD PTR [rsp+0x70]
002A1966: mov       QWORD PTR [rbp-0x80],rcx
002A196A: lea       rcx,[rip+0x1656f7]        # 0x180407068
002A1971: vmovsd    xmm1,QWORD PTR [rbp-0x80]
002A1976: mov       QWORD PTR [rbp+0x0],rcx
002A197A: lea       rcx,[rbp+0x360]
002A1981: mov       QWORD PTR [rsp+0x20],rcx
002A1986: mov       rcx,rax
002A1989: mov       QWORD PTR [rbp+0x8],0x3a
002A1991: vmovups   XMMWORD PTR [rbp-0x40],xmm0
002A1996: vmovsd    QWORD PTR [rbp-0x30],xmm1
002A199B: call      0x180196e10
002A19A0: cmp       QWORD PTR [r12],0x0
002A19A5: je        0x1802a20ef
002A19AB: cmp       QWORD PTR [rbx],0x0
002A19AF: je        0x1802a20ef
002A19B5: vmovaps   XMMWORD PTR [rsp+0x400],xmm6
002A19BE: vmovaps   XMMWORD PTR [rsp+0x3f0],xmm7
002A19C7: vmovaps   XMMWORD PTR [rsp+0x3e0],xmm8
002A19D0: vpxor     xmm0,xmm0,xmm0
002A19D4: vxorps    xmm7,xmm7,xmm7
002A19D8: mov       eax,r15d
002A19DB: lea       r9,[rbp-0x8]
002A19DF: vcvtsi2ss xmm7,xmm7,rax
002A19E4: vmovss    DWORD PTR [rdi+0x10],xmm7
002A19E9: mov       eax,r14d
002A19EC: lea       r8,[rbp+0xf0]
002A19F3: vxorps    xmm8,xmm8,xmm8
002A19F8: vcvtsi2ss xmm8,xmm8,rax
002A19FD: vmovss    DWORD PTR [rdi+0x14],xmm8
002A1A02: mov       rcx,QWORD PTR [rdi+0x1680]
002A1A09: mov       edx,0x8
002A1A0E: vmovups   YMMWORD PTR [rbp+0xf0],ymm0
002A1A16: vmovups   YMMWORD PTR [rbp+0x110],ymm0
002A1A1E: mov       QWORD PTR [rbp-0x8],0x0
002A1A26: mov       rax,QWORD PTR [rcx]
002A1A29: vzeroupper 
002A1A2C: call      QWORD PTR [rax+0x2c8]
002A1A32: mov       rcx,QWORD PTR [rdi+0x1680]
002A1A39: lea       rdx,[rbp+0x350]
002A1A40: mov       DWORD PTR [rbp+0x350],0x0
002A1A4A: xor       r8d,r8d
002A1A4D: mov       rax,QWORD PTR [rcx]
002A1A50: call      QWORD PTR [rax+0x2f8]
002A1A56: xor       edx,edx
002A1A58: lea       rcx,[rbp+0x160]
002A1A5F: mov       r8d,0x180
002A1A65: call      0x18023a82c
002A1A6A: cmp       DWORD PTR [rbp+0x350],0x0
002A1A71: jbe       0x1802a1a91
002A1A73: mov       rcx,QWORD PTR [rdi+0x1680]
002A1A7A: lea       r8,[rbp+0x160]
002A1A81: lea       rdx,[rbp+0x350]
002A1A88: mov       rax,QWORD PTR [rcx]
002A1A8B: call      QWORD PTR [rax+0x2f8]
