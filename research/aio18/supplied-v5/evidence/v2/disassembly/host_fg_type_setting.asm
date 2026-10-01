; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x29BE80..0x29BF80; unnamed
0029BE83: mov       BYTE PTR [rbp+0x1d0],0x1
0029BE8A: lea       rbx,[rsi+0x3e0]
0029BE91: mov       r9d,0x1
0029BE97: lea       r8,[rip+0x16a512]        # 0x1804063b0 ; 'mFrameGenType'
0029BE9E: lea       rdx,[rip+0x163ea3]        # 0x1803ffd48 ; 'Frame Generation'
0029BEA5: lea       rcx,[rsp+0x40]
0029BEAA: call      0x1801900d0
0029BEAF: mov       DWORD PTR [rbx],eax
0029BEB1: xor       r9d,r9d
0029BEB4: lea       r8,[rip+0x16a4f5]        # 0x1804063b0 ; 'mFrameGenType'
0029BEBB: lea       rdx,[rip+0x163e86]        # 0x1803ffd48 ; 'Frame Generation'
0029BEC2: lea       rcx,[rsp+0x40]
0029BEC7: call      0x180153110
0029BECC: test      rax,rax
0029BECF: jne       0x18029bf3e
0029BED1: mov       r8d,DWORD PTR [rbx]
0029BED4: lea       rdx,[rip+0x16b7f1]        # 0x1804076cc
0029BEDB: lea       rcx,[rbp+0x10]
0029BEDF: call      0x180190a80
0029BEE4: lea       rcx,[rbp+0x10]
0029BEE8: mov       rax,r15
0029BEEB: nop       DWORD PTR [rax+rax*1+0x0]
0029BEF0: lea       rax,[rax+0x1]
0029BEF4: cmp       BYTE PTR [rcx+rax*1],r14b
0029BEF8: jne       0x18029bef0
0029BEFA: lea       r8,[rax+0x1]
0029BEFE: cmp       r8,0x40
0029BF02: ja        0x18029bf11
0029BF04: lea       rdx,[rbp+0x10]
0029BF08: lea       rcx,[rbp-0x70]
0029BF0C: call      0x18023a826
0029BF11: mov       BYTE PTR [rsp+0x30],0x1
0029BF16: mov       QWORD PTR [rsp+0x20],r14
0029BF1B: lea       r9,[rbp-0x70]
0029BF1F: lea       r8,[rip+0x16a48a]        # 0x1804063b0 ; 'mFrameGenType'
0029BF26: lea       rdx,[rip+0x163e1b]        # 0x1803ffd48 ; 'Frame Generation'
0029BF2D: lea       rcx,[rsp+0x40]
0029BF32: call      0x1801561a0
0029BF37: mov       BYTE PTR [rbp+0x1d0],0x1
0029BF3E: mov       DWORD PTR [rbp+0x1e0],0x3
0029BF48: mov       DWORD PTR [rbp+0x1d8],r14d
0029BF4F: mov       edx,DWORD PTR [rbx]
0029BF51: lea       rcx,[rbp+0x1d8]
0029BF58: test      edx,edx
0029BF5A: cmovns    rcx,rbx
0029BF5E: lea       rax,[rbp+0x1e0]
0029BF65: cmp       edx,0x3
0029BF68: cmovle    rax,rcx
0029BF6C: mov       eax,DWORD PTR [rax]
0029BF6E: mov       DWORD PTR [rbx],eax
0029BF70: mov       DWORD PTR [rsi+0x3dc],eax
0029BF76: xor       r9d,r9d
0029BF79: lea       r8,[rip+0x16a420]        # 0x1804063a0 ; 'mAdapterName'
