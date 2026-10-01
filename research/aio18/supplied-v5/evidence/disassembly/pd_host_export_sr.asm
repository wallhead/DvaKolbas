; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xF8D20..0xF8DAE; EvaluateUpscaler
000F8D20: sub       rsp,0xd8
000F8D27: movups    xmm0,XMMWORD PTR [rcx]
000F8D2A: lea       rax,[rsp+0x20]
000F8D2F: movups    xmm1,XMMWORD PTR [rcx+0x10]
000F8D33: lea       rdx,[rsp+0x20]
000F8D38: movups    XMMWORD PTR [rax],xmm0
000F8D3B: movups    xmm0,XMMWORD PTR [rcx+0x20]
000F8D3F: movups    XMMWORD PTR [rax+0x10],xmm1
000F8D43: movups    xmm1,XMMWORD PTR [rcx+0x30]
000F8D47: movups    XMMWORD PTR [rax+0x20],xmm0
000F8D4B: movups    xmm0,XMMWORD PTR [rcx+0x40]
000F8D4F: movups    XMMWORD PTR [rax+0x30],xmm1
000F8D53: movups    xmm1,XMMWORD PTR [rcx+0x50]
000F8D57: movups    XMMWORD PTR [rax+0x40],xmm0
000F8D5B: movups    xmm0,XMMWORD PTR [rcx+0x60]
000F8D5F: movups    XMMWORD PTR [rax+0x50],xmm1
000F8D63: movups    xmm1,XMMWORD PTR [rcx+0x80]
000F8D6A: movups    XMMWORD PTR [rax+0x60],xmm0
000F8D6E: movups    xmm0,XMMWORD PTR [rcx+0x70]
000F8D72: movups    XMMWORD PTR [rax+0x70],xmm0
000F8D76: movups    xmm0,XMMWORD PTR [rcx+0x90]
000F8D7D: movups    XMMWORD PTR [rax+0x80],xmm1
000F8D84: movups    xmm1,XMMWORD PTR [rcx+0xa0]
000F8D8B: mov       rcx,QWORD PTR [rip+0x1126c4e]        # 0x18121f9e0
000F8D92: movups    XMMWORD PTR [rax+0x90],xmm0
000F8D99: movups    XMMWORD PTR [rax+0xa0],xmm1
000F8DA0: mov       rax,QWORD PTR [rcx]
000F8DA3: call      QWORD PTR [rax+0x28]
000F8DA6: add       rsp,0xd8
000F8DAD: ret       
