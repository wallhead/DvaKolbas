; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x29CC77..0x29CDCE; unnamed
0029CC77: mov       BYTE PTR [rbp+0x1d0],0x1
0029CC7E: xor       r9d,r9d
0029CC81: lea       r8,[rip+0x1698e8]        # 0x180406570 ; 'mEnableHUDFix'
0029CC88: lea       rdx,[rip+0x1630b9]        # 0x1803ffd48 ; 'Frame Generation'
0029CC8F: lea       rcx,[rsp+0x40]
0029CC94: call      0x180153230
0029CC99: mov       BYTE PTR [rsi+0x4fa],al
0029CC9F: xor       r9d,r9d
0029CCA2: lea       r8,[rip+0x1698c7]        # 0x180406570 ; 'mEnableHUDFix'
0029CCA9: lea       rdx,[rip+0x163098]        # 0x1803ffd48 ; 'Frame Generation'
0029CCB0: lea       rcx,[rsp+0x40]
0029CCB5: call      0x180153110
0029CCBA: test      rax,rax
0029CCBD: jne       0x18029cd1a
0029CCBF: mov       rdx,r13
0029CCC2: cmp       BYTE PTR [rsi+0x4fa],al
0029CCC8: cmovne    rdx,r14
0029CCCC: mov       rax,r15
0029CCCF: nop       
0029CCD0: lea       rax,[rax+0x1]
0029CCD4: cmp       BYTE PTR [rdx+rax*1],0x0
0029CCD8: jne       0x18029ccd0
0029CCDA: lea       r8,[rax+0x1]
0029CCDE: cmp       r8,0x40
0029CCE2: ja        0x18029cced
0029CCE4: lea       rcx,[rbp-0x30]
0029CCE8: call      0x18023a826
0029CCED: mov       BYTE PTR [rsp+0x30],0x1
0029CCF2: mov       QWORD PTR [rsp+0x20],r12
0029CCF7: lea       r9,[rbp-0x30]
0029CCFB: lea       r8,[rip+0x16986e]        # 0x180406570 ; 'mEnableHUDFix'
0029CD02: lea       rdx,[rip+0x16303f]        # 0x1803ffd48 ; 'Frame Generation'
0029CD09: lea       rcx,[rsp+0x40]
0029CD0E: call      0x1801561a0
0029CD13: mov       BYTE PTR [rbp+0x1d0],0x1
0029CD1A: mov       r9d,0x1
0029CD20: lea       r8,[rip+0x169839]        # 0x180406560 ; 'mHUDFixMethod'
0029CD27: lea       rdx,[rip+0x16301a]        # 0x1803ffd48 ; 'Frame Generation'
0029CD2E: lea       rcx,[rsp+0x40]
0029CD33: call      0x1801900d0
0029CD38: mov       DWORD PTR [rsi+0x4fc],eax
0029CD3E: xor       r9d,r9d
0029CD41: lea       r8,[rip+0x169818]        # 0x180406560 ; 'mHUDFixMethod'
0029CD48: lea       rdx,[rip+0x162ff9]        # 0x1803ffd48 ; 'Frame Generation'
0029CD4F: lea       rcx,[rsp+0x40]
0029CD54: call      0x180153110
0029CD59: test      rax,rax
0029CD5C: jne       0x18029cdce
0029CD5E: mov       r8d,DWORD PTR [rsi+0x4fc]
0029CD65: lea       rdx,[rip+0x16a960]        # 0x1804076cc
0029CD6C: lea       rcx,[rbp-0x70]
0029CD70: call      0x180190a80
0029CD75: lea       rcx,[rbp-0x70]
0029CD79: mov       rax,r15
0029CD7C: nop       DWORD PTR [rax+0x0]
0029CD80: lea       rax,[rax+0x1]
0029CD84: cmp       BYTE PTR [rcx+rax*1],0x0
0029CD88: jne       0x18029cd80
0029CD8A: lea       r8,[rax+0x1]
0029CD8E: cmp       r8,0x40
0029CD92: ja        0x18029cda1
0029CD94: lea       rdx,[rbp-0x70]
0029CD98: lea       rcx,[rbp-0x30]
0029CD9C: call      0x18023a826
0029CDA1: mov       BYTE PTR [rsp+0x30],0x1
0029CDA6: mov       QWORD PTR [rsp+0x20],r12
0029CDAB: lea       r9,[rbp-0x30]
0029CDAF: lea       r8,[rip+0x1697aa]        # 0x180406560 ; 'mHUDFixMethod'
0029CDB6: lea       rdx,[rip+0x162f8b]        # 0x1803ffd48 ; 'Frame Generation'
0029CDBD: lea       rcx,[rsp+0x40]
0029CDC2: call      0x1801561a0
0029CDC7: mov       BYTE PTR [rbp+0x1d0],0x1
