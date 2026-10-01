; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x27F330..0x27F400; unnamed
0027F330: rex       push rbx
0027F332: sub       rsp,0x20
0027F336: call      0x18027ee50
0027F33B: vmovdqu   xmm0,XMMWORD PTR [rip+0x18d4bd]        # 0x18040c800
0027F343: xor       ebx,ebx
0027F345: mov       WORD PTR [rip+0x1f6ce2],0x0        # 0x180476030
0027F34E: vmovups   XMMWORD PTR [rip+0x1e1da2],xmm0        # 0x1804610f8
0027F356: mov       BYTE PTR [rip+0x1f69ea],0x0        # 0x180475d47
0027F35D: mov       BYTE PTR [rip+0x1f69e2],0x0        # 0x180475d46
0027F364: mov       QWORD PTR [rip+0x1f69f5],rbx        # 0x180475d60
0027F36B: mov       DWORD PTR [rip+0x1f69df],ebx        # 0x180475d50
0027F371: mov       QWORD PTR [rip+0x1f69d0],rbx        # 0x180475d48
0027F378: call      0x1801503a0
0027F37D: mov       rcx,rax
0027F380: call      0x180266db0
0027F385: mov       BYTE PTR [rip+0x1f6999],bl        # 0x180475d24
0027F38B: mov       BYTE PTR [rip+0x1f69af],bl        # 0x180475d40
0027F391: mov       BYTE PTR [rip+0x1f69ad],bl        # 0x180475d44
0027F397: mov       QWORD PTR [rip+0x1f69ba],rbx        # 0x180475d58
0027F39E: mov       QWORD PTR [rip+0x1f69bb],rbx        # 0x180475d60
0027F3A5: mov       DWORD PTR [rip+0x1f69a5],ebx        # 0x180475d50
0027F3AB: mov       QWORD PTR [rip+0x1f6996],rbx        # 0x180475d48
0027F3B2: mov       BYTE PTR [rip+0x1f698a],bl        # 0x180475d42
0027F3B8: mov       BYTE PTR [rip+0x1f6983],bl        # 0x180475d41
0027F3BE: mov       BYTE PTR [rip+0x1f697f],bl        # 0x180475d43
0027F3C4: mov       BYTE PTR [rip+0x1e1d19],0x1        # 0x1804610e4
0027F3CB: mov       BYTE PTR [rip+0x1f6b9f],bl        # 0x180475f70
0027F3D1: mov       BYTE PTR [rip+0x1f6b91],bl        # 0x180475f68
0027F3D7: mov       DWORD PTR [rip+0x1e1cff],0xffffffff        # 0x1804610e0
0027F3E1: mov       BYTE PTR [rip+0x1f68f1],bl        # 0x180475cd8
0027F3E7: mov       BYTE PTR [rip+0x1f68fb],0x1        # 0x180475ce9
0027F3EE: call      0x1801503a0
0027F3F3: mov       BYTE PTR [rax+0x264],0x1
0027F3FA: add       rsp,0x20
0027F3FE: pop       rbx
0027F3FF: ret       
