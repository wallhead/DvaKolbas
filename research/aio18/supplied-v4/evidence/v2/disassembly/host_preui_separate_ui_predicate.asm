; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x293F20..0x293FD1; unnamed
00293F20: mov       QWORD PTR [rsp+0x8],rbx
00293F25: mov       QWORD PTR [rsp+0x10],rsi
00293F2A: push      rdi
00293F2B: sub       rsp,0x20
00293F2F: cmp       BYTE PTR [rcx+0x343],0x0
00293F36: lea       rdi,[rcx+0x3d0]
00293F3D: mov       rbx,rcx
00293F40: je        0x180293f47
00293F42: cmp       BYTE PTR [rdi],0x0
00293F45: jne       0x180293fad
00293F47: lea       rsi,[rcx+0x4fc]
00293F4E: cmp       DWORD PTR [rsi],0x0
00293F51: jne       0x180293fad
00293F53: cmp       BYTE PTR [rcx+0x168b],0x0
00293F5A: jne       0x180293f8e
00293F5C: cmp       BYTE PTR [rcx+0x4fa],0x0
00293F63: je        0x180293f8e
00293F65: cmp       DWORD PTR [rcx+0x3dc],0x0
00293F6C: jg        0x180293fbf
00293F6E: cmp       DWORD PTR [rip+0x1e178b],0x2        # 0x180475700
00293F75: jne       0x180293f80
00293F77: cmp       BYTE PTR [rcx+0x4a5],0x0
00293F7E: jne       0x180293fbf
00293F80: lea       rdi,[rcx+0x3d0]
00293F87: lea       rsi,[rcx+0x4fc]
00293F8E: call      0x180294090
00293F93: test      al,al
00293F95: je        0x180293fa1
00293F97: cmp       BYTE PTR [rdi],0x0
00293F9A: jne       0x180293fa1
00293F9C: cmp       DWORD PTR [rsi],0x0
00293F9F: je        0x180293fbf
00293FA1: mov       rcx,rbx
00293FA4: call      0x180293fe0
00293FA9: test      al,al
00293FAB: jne       0x180293fbf
00293FAD: xor       al,al
00293FAF: mov       rbx,QWORD PTR [rsp+0x30]
00293FB4: mov       rsi,QWORD PTR [rsp+0x38]
00293FB9: add       rsp,0x20
00293FBD: pop       rdi
00293FBE: ret       
00293FBF: mov       rbx,QWORD PTR [rsp+0x30]
00293FC4: mov       al,0x1
00293FC6: mov       rsi,QWORD PTR [rsp+0x38]
00293FCB: add       rsp,0x20
00293FCF: pop       rdi
00293FD0: ret       
