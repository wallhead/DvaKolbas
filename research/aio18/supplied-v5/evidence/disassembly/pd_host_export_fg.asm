; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xF8DB0..0xF8E3E; EvaluateFrameGeneration
000F8DB0: sub       rsp,0xd8
000F8DB7: movups    xmm0,XMMWORD PTR [rcx]
000F8DBA: lea       rax,[rsp+0x20]
000F8DBF: movups    xmm1,XMMWORD PTR [rcx+0x10]
000F8DC3: lea       rdx,[rsp+0x20]
000F8DC8: movups    XMMWORD PTR [rax],xmm0
000F8DCB: movups    xmm0,XMMWORD PTR [rcx+0x20]
000F8DCF: movups    XMMWORD PTR [rax+0x10],xmm1
000F8DD3: movups    xmm1,XMMWORD PTR [rcx+0x30]
000F8DD7: movups    XMMWORD PTR [rax+0x20],xmm0
000F8DDB: movups    xmm0,XMMWORD PTR [rcx+0x40]
000F8DDF: movups    XMMWORD PTR [rax+0x30],xmm1
000F8DE3: movups    xmm1,XMMWORD PTR [rcx+0x50]
000F8DE7: movups    XMMWORD PTR [rax+0x40],xmm0
000F8DEB: movups    xmm0,XMMWORD PTR [rcx+0x60]
000F8DEF: movups    XMMWORD PTR [rax+0x50],xmm1
000F8DF3: movups    xmm1,XMMWORD PTR [rcx+0x80]
000F8DFA: movups    XMMWORD PTR [rax+0x60],xmm0
000F8DFE: movups    xmm0,XMMWORD PTR [rcx+0x70]
000F8E02: movups    XMMWORD PTR [rax+0x70],xmm0
000F8E06: movups    xmm0,XMMWORD PTR [rcx+0x90]
000F8E0D: movups    XMMWORD PTR [rax+0x80],xmm1
000F8E14: movups    xmm1,XMMWORD PTR [rcx+0xa0]
000F8E1B: mov       rcx,QWORD PTR [rip+0x1126bbe]        # 0x18121f9e0
000F8E22: movups    XMMWORD PTR [rax+0x90],xmm0
000F8E29: movups    XMMWORD PTR [rax+0xa0],xmm1
000F8E30: mov       rax,QWORD PTR [rcx]
000F8E33: call      QWORD PTR [rax+0x40]
000F8E36: add       rsp,0xd8
000F8E3D: ret       
