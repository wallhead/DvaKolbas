; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x29C03A..0x29C125; unnamed
0029C03A: mov       BYTE PTR [rsi+0x499],r14b
0029C041: mov       r9b,0x1
0029C044: lea       r8,[rip+0x16a375]        # 0x1804063c0 ; 'mPDFrameWarpActive'
0029C04B: lea       rdx,[rip+0x16992e]        # 0x180405980 ; 'PDFrameWarp'
0029C052: lea       rcx,[rsp+0x40]
0029C057: call      0x180153230
0029C05C: movzx     ebx,al
0029C05F: add       ebx,ebx
0029C061: je        0x18029c07a
0029C063: cmp       ebx,0x1
0029C066: je        0x18029c071
0029C068: lea       r9,[rip+0x14ad69]        # 0x1803e6dd8 ; 'First Person + Third Person'
0029C06F: jmp       0x18029c081
0029C071: lea       r9,[rip+0x14ad80]        # 0x1803e6df8 ; 'First Person Only'
0029C078: jmp       0x18029c081
0029C07A: lea       r9,[rip+0x14ad4f]        # 0x1803e6dd0
0029C081: lea       r8,[rip+0x16a378]        # 0x180406400 ; 'mPDFrameWarpMode'
0029C088: lea       rdx,[rip+0x1698f1]        # 0x180405980 ; 'PDFrameWarp'
0029C08F: lea       rcx,[rsp+0x40]
0029C094: call      0x180153110
0029C099: vpxor     xmm0,xmm0,xmm0
0029C09D: vmovups   XMMWORD PTR [rbp+0xc0],xmm0
0029C0A5: mov       QWORD PTR [rbp+0xd0],r14
0029C0AC: mov       QWORD PTR [rbp+0xd8],r14
0029C0B3: mov       r8,r15
0029C0B6: inc       r8
0029C0B9: cmp       BYTE PTR [rax+r8*1],r14b
0029C0BD: jne       0x18029c0b6
0029C0BF: mov       rdx,rax
0029C0C2: lea       rcx,[rbp+0xc0]
0029C0C9: call      0x18013a500
0029C0CE: nop       
0029C0CF: lea       rax,[rbp+0xc0]
0029C0D6: cmp       QWORD PTR [rbp+0xd8],0xf
0029C0DE: cmova     rax,QWORD PTR [rbp+0xc0]
0029C0E6: mov       QWORD PTR [rbp+0x70],rax
0029C0EA: mov       rax,QWORD PTR [rbp+0xd0]
0029C0F1: mov       QWORD PTR [rbp+0x78],rax
0029C0F5: mov       edx,ebx
0029C0F7: lea       rcx,[rbp+0x70]
0029C0FB: call      0x18018f3d0
0029C100: mov       DWORD PTR [rsi+0x49c],eax
0029C106: test      eax,eax
0029C108: je        0x18029c110
0029C10A: mov       DWORD PTR [rsi+0x4a0],eax
0029C110: setne     cl
0029C113: mov       BYTE PTR [rsi+0x4a5],cl
0029C119: cmp       eax,0x2
0029C11C: sete      al
0029C11F: mov       BYTE PTR [rsi+0x4a6],al
