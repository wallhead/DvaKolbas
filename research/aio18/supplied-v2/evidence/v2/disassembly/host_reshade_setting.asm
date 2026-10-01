; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x29B487..0x29B53E; unnamed
0029B487: mov       BYTE PTR [rbp+0x1d0],0x1
0029B48E: mov       r9b,0x1
0029B491: lea       r8,[rip+0x16ae28]        # 0x1804062c0 ; 'mRenderReShadeBeforeUpscaling'
0029B498: lea       rdx,[rip+0x16add1]        # 0x180406270 ; 'Settings'
0029B49F: lea       rcx,[rsp+0x40]
0029B4A4: call      0x180153230
0029B4A9: mov       BYTE PTR [rsi+0x487],al
0029B4AF: xor       r9d,r9d
0029B4B2: lea       r8,[rip+0x16ae07]        # 0x1804062c0 ; 'mRenderReShadeBeforeUpscaling'
0029B4B9: lea       rdx,[rip+0x16adb0]        # 0x180406270 ; 'Settings'
0029B4C0: lea       rcx,[rsp+0x40]
0029B4C5: call      0x180153110
0029B4CA: lea       rcx,[rip+0x1462df]        # 0x1803e17b0
0029B4D1: lea       r13,[rip+0x1462d0]        # 0x1803e17a8
0029B4D8: test      rax,rax
0029B4DB: jne       0x18029b53e
0029B4DD: mov       rdx,r13
0029B4E0: cmp       BYTE PTR [rsi+0x487],al
0029B4E6: cmovne    rdx,rcx
0029B4EA: mov       rax,r15
0029B4ED: nop       DWORD PTR [rax]
0029B4F0: lea       rax,[rax+0x1]
0029B4F4: cmp       BYTE PTR [rdx+rax*1],0x0
0029B4F8: jne       0x18029b4f0
0029B4FA: lea       r8,[rax+0x1]
0029B4FE: cmp       r8,0x40
0029B502: ja        0x18029b50d
0029B504: lea       rcx,[rbp+0x10]
0029B508: call      0x18023a826
0029B50D: mov       BYTE PTR [rsp+0x30],0x1
0029B512: mov       QWORD PTR [rsp+0x20],0x0
0029B51B: lea       r9,[rbp+0x10]
0029B51F: lea       r8,[rip+0x16ad9a]        # 0x1804062c0 ; 'mRenderReShadeBeforeUpscaling'
0029B526: lea       rdx,[rip+0x16ad43]        # 0x180406270 ; 'Settings'
0029B52D: lea       rcx,[rsp+0x40]
0029B532: call      0x1801561a0
0029B537: mov       BYTE PTR [rbp+0x1d0],0x1
