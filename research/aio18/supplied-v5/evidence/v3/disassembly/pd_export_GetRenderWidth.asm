; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xF8E40..0xF8E72; GetRenderWidth
000F8E40: mov       edx,ecx
000F8E42: mov       rcx,QWORD PTR [rip+0x1126b97]        # 0x18121f9e0
000F8E49: mov       rax,QWORD PTR [rcx]
000F8E4C: rex.W     jmp QWORD PTR [rax+0x48]
000F8E50: mov       edx,ecx
000F8E52: mov       rcx,QWORD PTR [rip+0x1126b87]        # 0x18121f9e0
000F8E59: mov       rax,QWORD PTR [rcx]
000F8E5C: rex.W     jmp QWORD PTR [rax+0x50]
000F8E60: mov       QWORD PTR [rsp+0x8],rbx
000F8E65: push      rdi
000F8E66: sub       rsp,0x30
000F8E6A: movaps    XMMWORD PTR [rsp+0x20],xmm6
000F8E6F: mov       rbx,r9
