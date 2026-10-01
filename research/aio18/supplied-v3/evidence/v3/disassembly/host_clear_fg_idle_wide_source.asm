; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x266DB0..0x266E13; unnamed
00266DB0: rex       push rbx
00266DB2: sub       rsp,0x20
00266DB6: cmp       DWORD PTR [rcx+0x11a0],0xffffffff
00266DBD: mov       rbx,rcx
00266DC0: je        0x180266e0d
00266DC2: mov       rax,QWORD PTR [rip+0x20ed77]        # 0x180475b40
00266DC9: test      rax,rax
00266DCC: jne       0x180266df7
00266DCE: lea       rcx,[rip+0x17bf03]        # 0x1803e2cd8
00266DD5: call      QWORD PTR [rip+0x5c38d]        # 0x1802c3168
00266DDB: mov       rcx,rax
00266DDE: lea       rdx,[rip+0x17c7ab]        # 0x1803e3590
00266DE5: call      QWORD PTR [rip+0x5c36d]        # 0x1802c3158
00266DEB: mov       QWORD PTR [rip+0x20ed4e],rax        # 0x180475b40
00266DF2: test      rax,rax
00266DF5: je        0x180266e03
00266DF7: xor       r9d,r9d
00266DFA: xor       r8d,r8d
00266DFD: xor       edx,edx
00266DFF: xor       ecx,ecx
00266E01: call      rax
00266E03: mov       DWORD PTR [rbx+0x11a0],0xffffffff
00266E0D: add       rsp,0x20
00266E11: pop       rbx
00266E12: ret       
