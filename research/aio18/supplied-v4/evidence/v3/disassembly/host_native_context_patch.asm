; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x2A9A00..0x2A9B20; unnamed
002A9A00: rex       push rbp
002A9A02: push      rbx
002A9A03: push      rdi
002A9A04: lea       rbp,[rsp-0x47]
002A9A09: sub       rsp,0x90
002A9A10: lea       rax,[rbp+0x67]
002A9A14: mov       BYTE PTR [rbp+0x67],0x1
002A9A18: mov       QWORD PTR [rbp-0x9],rax
002A9A1C: lea       rdx,[rbp-0x9]
002A9A20: lea       rax,[rbp+0x6f]
002A9A24: mov       DWORD PTR [rbp+0x6f],0x0
002A9A2B: mov       QWORD PTR [rbp-0x1],rax
002A9A2F: mov       rbx,rcx
002A9A32: call      0x1801a30c0
002A9A37: mov       BYTE PTR [rbp+0x67],al
002A9A3A: mov       QWORD PTR [rbp+0x77],rbx
002A9A3E: call      0x180222050
002A9A43: mov       ecx,DWORD PTR [rbp+0x33]
002A9A46: lea       rbx,[rip+0x1601cb]        # 0x180409c18
002A9A4D: mov       DWORD PTR [rbp+0x13],ecx
002A9A50: lea       rdi,[rip+0x160219]        # 0x180409c70
002A9A57: lea       rcx,[rip+0x1602e2]        # 0x180409d40
002A9A5E: mov       QWORD PTR [rbp+0x7],rbx
002A9A62: mov       QWORD PTR [rbp-0x9],rcx
002A9A66: lea       r9,[rbp-0x9]
002A9A6A: lea       rcx,[rbp+0x77]
002A9A6E: mov       DWORD PTR [rbp+0xf],0x110
002A9A75: vmovups   xmm0,XMMWORD PTR [rbp+0x7]
002A9A7A: mov       QWORD PTR [rsp+0x30],rcx
002A9A7F: lea       rdx,[rbp+0x27]
002A9A83: lea       rcx,[rbp+0x6f]
002A9A87: mov       QWORD PTR [rbp+0x17],rdi
002A9A8B: vmovsd    xmm1,QWORD PTR [rbp+0x17]
002A9A90: mov       QWORD PTR [rsp+0x28],rcx
002A9A95: lea       rcx,[rbp+0x67]
002A9A99: mov       QWORD PTR [rsp+0x20],rcx
002A9A9E: mov       rcx,rax
002A9AA1: mov       QWORD PTR [rbp-0x1],0x39
002A9AA9: vmovups   XMMWORD PTR [rbp+0x27],xmm0
002A9AAE: vmovsd    QWORD PTR [rbp+0x37],xmm1
002A9AB3: call      0x1801a68d0
002A9AB8: cmp       BYTE PTR [rbp+0x67],0x0
002A9ABC: jne       0x1802a9b15
002A9ABE: call      0x180222050
002A9AC3: mov       ecx,DWORD PTR [rbp+0x33]
002A9AC6: lea       r9,[rbp-0x9]
002A9ACA: mov       DWORD PTR [rbp+0x13],ecx
002A9ACD: lea       rdx,[rbp+0x27]
002A9AD1: lea       rcx,[rip+0x160208]        # 0x180409ce0
002A9AD8: mov       QWORD PTR [rbp+0x7],rbx
002A9ADC: mov       QWORD PTR [rbp-0x9],rcx
002A9AE0: mov       r8d,0x4
002A9AE6: mov       DWORD PTR [rbp+0xf],0x111
002A9AED: mov       rcx,rax
002A9AF0: vmovups   xmm0,XMMWORD PTR [rbp+0x7]
002A9AF5: mov       QWORD PTR [rbp+0x17],rdi
002A9AF9: vmovsd    xmm1,QWORD PTR [rbp+0x17]
002A9AFE: mov       QWORD PTR [rbp-0x1],0x5d
002A9B06: vmovups   XMMWORD PTR [rbp+0x27],xmm0
002A9B0B: vmovsd    QWORD PTR [rbp+0x37],xmm1
002A9B10: call      0x180151990
002A9B15: add       rsp,0x90
002A9B1C: pop       rdi
002A9B1D: pop       rbx
002A9B1E: pop       rbp
002A9B1F: ret       
