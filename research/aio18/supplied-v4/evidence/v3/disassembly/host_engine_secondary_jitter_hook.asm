; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x1A0860..0x1A0897; unnamed
001A0860: rex       push rbx
001A0862: sub       rsp,0x20
001A0866: mov       rbx,rdx
001A0869: call      QWORD PTR [rip+0x1230e9]        # 0x1802c3958
001A086F: mov       rdx,QWORD PTR [rip+0xce056a]        # 0x180e80de0
001A0876: cmp       BYTE PTR [rdx+0x343],0x0
001A087D: jne       0x1801a0891
001A087F: mov       ecx,DWORD PTR [rdx+0x278]
001A0885: mov       DWORD PTR [rbx+0x8],ecx
001A0888: mov       ecx,DWORD PTR [rdx+0x27c]
001A088E: mov       DWORD PTR [rbx+0xc],ecx
001A0891: add       rsp,0x20
001A0895: pop       rbx
001A0896: ret       
