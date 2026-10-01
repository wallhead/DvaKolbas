; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xF03F0..0xF0470; unnamed
000F03F0: mov       eax,DWORD PTR [rcx+0x8]
000F03F3: mov       BYTE PTR [rcx+0x1c8],0x0
000F03FA: cmp       eax,0x1
000F03FD: jne       0x1800f040a
000F03FF: mov       eax,DWORD PTR [rdx+0x4]
000F0402: mov       DWORD PTR [rcx+0x110],eax
000F0408: jmp       0x1800f0418
000F040A: cmp       eax,0x2
000F040D: jne       0x1800f0418
000F040F: mov       eax,DWORD PTR [rdx+0x4]
000F0412: mov       DWORD PTR [rcx+0x114],eax
000F0418: movups    xmm0,XMMWORD PTR [rdx]
000F041B: mov       al,0x1
000F041D: movsd     xmm1,QWORD PTR [rdx+0x10]
000F0422: movups    XMMWORD PTR [rcx+0x11c],xmm0
000F0429: movsd     QWORD PTR [rcx+0x12c],xmm1
000F0431: ret       
000F0432: int3      
000F0433: int3      
000F0434: int3      
000F0435: int3      
000F0436: int3      
000F0437: int3      
000F0438: int3      
000F0439: int3      
000F043A: int3      
000F043B: int3      
000F043C: int3      
000F043D: int3      
000F043E: int3      
000F043F: int3      
000F0440: mov       QWORD PTR [rsp+0x10],rdi
000F0445: push      rbp
000F0446: lea       rbp,[rsp-0x4f]
000F044B: sub       rsp,0xb0
000F0452: mov       eax,DWORD PTR [rcx+0x8]
000F0455: xorps     xmm0,xmm0
000F0458: mov       rdi,rdx
000F045B: movups    XMMWORD PTR [rdx],xmm0
000F045E: movups    XMMWORD PTR [rdx+0x10],xmm0
000F0462: movups    XMMWORD PTR [rdx+0x20],xmm0
000F0466: cmp       eax,0x1
000F0469: jne       0x1800f048a
000F046B: mov       rdx,r8
000F046E: lea       rcx,[rbp-0x21]
