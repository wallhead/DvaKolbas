; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x1A07F0..0x1A0858; unnamed
001A07F0: sub       rsp,0x28
001A07F4: cmp       BYTE PTR [rip+0xce0825],0x0        # 0x180e81020
001A07FB: je        0x1801a084a
001A07FD: mov       rax,QWORD PTR [rip+0xce05dc]        # 0x180e80de0
001A0804: cmp       BYTE PTR [rax+0x343],0x0
001A080B: jne       0x1801a084a
001A080D: mov       rax,QWORD PTR [rip+0x2dcb8c]        # 0x18047d3a0
001A0814: mov       QWORD PTR [rsp+0x20],rbx
001A0819: mov       BYTE PTR [rip+0xce0800],0x0        # 0x180e81020
001A0820: mov       BYTE PTR [rip+0xce07fa],0x1        # 0x180e81021
001A0827: mov       BYTE PTR [rip+0xce0864],0x0        # 0x180e81092
001A082E: call      rax
001A0830: mov       ebx,eax
001A0832: mov       BYTE PTR [rip+0xce07e8],0x0        # 0x180e81021
001A0839: call      0x1801a0000
001A083E: mov       eax,ebx
001A0840: mov       rbx,QWORD PTR [rsp+0x20]
001A0845: add       rsp,0x28
001A0849: ret       
001A084A: mov       rax,QWORD PTR [rip+0x2dcb4f]        # 0x18047d3a0
001A0851: add       rsp,0x28
001A0855: rex.W     jmp rax
