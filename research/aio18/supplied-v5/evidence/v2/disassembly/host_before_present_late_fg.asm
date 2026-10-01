; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x2ABCA8..0x2ABD18; unnamed
002ABCAB: mov       eax,DWORD PTR [rcx+0x460]
002ABCB1: cmp       eax,DWORD PTR [rip+0x1b5429]        # 0x1804610e0
002ABCB7: mov       BYTE PTR [rip+0xbd52e1],r13b        # 0x180e80f9f
002ABCBE: jne       0x1802abcd1
002ABCC0: cmp       BYTE PTR [rip+0x1ca07d],r13b        # 0x180475d44
002ABCC7: je        0x1802abcd1
002ABCC9: cmp       DWORD PTR [rcx+0xfe8],eax
002ABCCF: jne       0x1802abd0e
002ABCD1: cmp       DWORD PTR [rcx+0x3dc],0x2
002ABCD8: je        0x1802abce3
002ABCDA: cmp       BYTE PTR [rcx+0x4a5],r13b
002ABCE1: je        0x1802abd0e
002ABCE3: cmp       QWORD PTR [rcx+0x1690],r13
002ABCEA: je        0x1802abcfe
002ABCEC: cmp       BYTE PTR [rip+0xbd539d],r13b        # 0x180e81090
002ABCF3: jne       0x1802abcfe
002ABCF5: mov       BYTE PTR [rip+0xbd52a3],0x1        # 0x180e80f9f
002ABCFC: jmp       0x1802abd0e
002ABCFE: lea       rdx,[rbp-0x50]
002ABD02: call      0x1802941a0
002ABD07: mov       rcx,QWORD PTR [rip+0xbd50d2]        # 0x180e80de0
002ABD0E: cmp       BYTE PTR [rcx+0x4e4],r13b
002ABD15: je        0x1802abdbc
