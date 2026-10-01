; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x2AB300..0x2AB3AD; unnamed
002AB306: cmp       BYTE PTR [rax+0x3d2],r13b
002AB30D: je        0x1802ab32a
002AB30F: lea       rcx,[rax+0x988]
002AB316: call      0x18016c140
002AB31B: mov       rdx,rax
002AB31E: call      0x180285500
002AB323: mov       rax,QWORD PTR [rip+0xbd5ab6]        # 0x180e80de0
002AB32A: cmp       BYTE PTR [rax+0x4e4],r13b
002AB331: jne       0x1802ab34e
002AB333: cmp       DWORD PTR [rax+0x3dc],0x2
002AB33A: je        0x1802ab34e
002AB33C: cmp       BYTE PTR [rax+0x4a5],r13b
002AB343: jne       0x1802ab34e
002AB345: lea       rdx,[rbp-0x50]
002AB349: call      0x1802941a0
002AB34E: vzeroupper 
002AB351: call      0x180152f00
002AB356: cmp       BYTE PTR [rax+0x11],r13b
002AB35A: jne       0x1802ab399
002AB35C: cmp       BYTE PTR [rax+0x12],r13b
002AB360: jne       0x1802ab399
002AB362: call      0x1801ac3d0
002AB367: mov       rbx,rax
002AB36A: test      rax,rax
002AB36D: je        0x1802ab395
002AB36F: lea       rdx,[rip+0x13a3c2]        # 0x1803e5738
002AB376: mov       rcx,rax
002AB379: call      0x1801ac480
002AB37E: test      al,al
002AB380: jne       0x1802ab399
002AB382: lea       rdx,[rip+0x13a38f]        # 0x1803e5718
002AB389: mov       rcx,rbx
002AB38C: call      0x1801ac480
002AB391: test      al,al
002AB393: jne       0x1802ab399
002AB395: xor       bl,bl
002AB397: jmp       0x1802ab39b
002AB399: mov       bl,0x1
002AB39B: mov       rax,QWORD PTR [rip+0xbd5a3e]        # 0x180e80de0
002AB3A2: cmp       BYTE PTR [rax+0x343],r13b
002AB3A9: jne       0x1802ab3ce
002AB3AB: mov       rcx,rax
