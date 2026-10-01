; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x284A70..0x284D26; unnamed
00284A70: rex       push rbx
00284A72: sub       rsp,0x20
00284A76: mov       rax,QWORD PTR gs:0x58
00284A7F: mov       ecx,DWORD PTR [rip+0x1eea2f]        # 0x1804734b4
00284A85: mov       edx,0xac
00284A8A: mov       rbx,QWORD PTR [rax+rcx*8]
00284A8E: add       rbx,rdx
00284A91: mov       eax,DWORD PTR [rbx]
00284A93: cmp       DWORD PTR [rip+0xbfc99f],eax        # 0x180e81438
00284A99: jg        0x180284bc8
00284A9F: mov       rax,QWORD PTR [rip+0xbfc99a]        # 0x180e81440
00284AA6: test      rax,rax
00284AA9: je        0x180284ab9
00284AAB: lea       rdx,[rip+0x59e]        # 0x180285050
00284AB2: mov       ecx,0x9
00284AB7: call      rax
00284AB9: mov       eax,DWORD PTR [rbx]
00284ABB: cmp       DWORD PTR [rip+0xbfc987],eax        # 0x180e81448
00284AC1: jg        0x180284c0e
00284AC7: mov       rax,QWORD PTR [rip+0xbfc982]        # 0x180e81450
00284ACE: test      rax,rax
00284AD1: je        0x180284ae1
00284AD3: lea       rdx,[rip+0x476]        # 0x180284f50
00284ADA: mov       ecx,0xa
00284ADF: call      rax
00284AE1: mov       eax,DWORD PTR [rbx]
00284AE3: cmp       DWORD PTR [rip+0xbfc96f],eax        # 0x180e81458
00284AE9: jg        0x180284c54
00284AEF: mov       rax,QWORD PTR [rip+0xbfc96a]        # 0x180e81460
00284AF6: test      rax,rax
00284AF9: je        0x180284b09
00284AFB: lea       rdx,[rip+0x40e]        # 0x180284f10
00284B02: mov       ecx,0x56
00284B07: call      rax
00284B09: mov       eax,DWORD PTR [rbx]
00284B0B: cmp       DWORD PTR [rip+0xbfc957],eax        # 0x180e81468
00284B11: jg        0x180284c9a
00284B17: mov       rax,QWORD PTR [rip+0xbfc952]        # 0x180e81470
00284B1E: test      rax,rax
00284B21: je        0x180284b31
00284B23: lea       rdx,[rip+0x396]        # 0x180284ec0
00284B2A: mov       ecx,0x4c
00284B2F: call      rax
00284B31: mov       eax,DWORD PTR [rbx]
00284B33: cmp       DWORD PTR [rip+0xbfc93f],eax        # 0x180e81478
00284B39: jg        0x180284ce0
00284B3F: mov       rax,QWORD PTR [rip+0xbfc93a]        # 0x180e81480
00284B46: test      rax,rax
00284B49: je        0x180284b59
00284B4B: lea       rdx,[rip+0x31e]        # 0x180284e70
00284B52: mov       ecx,0x4d
00284B57: call      rax
00284B59: mov       eax,DWORD PTR [rbx]
00284B5B: cmp       DWORD PTR [rip+0xbfc927],eax        # 0x180e81488
00284B61: jg        0x180284b89
00284B63: mov       rax,QWORD PTR [rip+0xbfc926]        # 0x180e81490
00284B6A: test      rax,rax
00284B6D: je        0x180284b83
00284B6F: lea       rdx,[rip+0x1ba]        # 0x180284d30
00284B76: mov       ecx,0x4b
00284B7B: add       rsp,0x20
00284B7F: pop       rbx
00284B80: rex.W     jmp rax
00284B83: add       rsp,0x20
00284B87: pop       rbx
00284B88: ret       
00284B89: lea       rcx,[rip+0xbfc8f8]        # 0x180e81488
00284B90: call      0x1802391bc
00284B95: cmp       DWORD PTR [rip+0xbfc8ec],0xffffffff        # 0x180e81488
00284B9C: jne       0x180284b63
00284B9E: call      0x18017cc20
00284BA3: mov       rcx,rax
00284BA6: lea       rdx,[rip+0x167623]        # 0x1803ec1d0 ; 'ReShadeRegisterEvent'
00284BAD: call      QWORD PTR [rip+0x3e5a5]        # 0x1802c3158
00284BB3: lea       rcx,[rip+0xbfc8ce]        # 0x180e81488
00284BBA: mov       QWORD PTR [rip+0xbfc8cf],rax        # 0x180e81490
00284BC1: call      0x180239150
00284BC6: jmp       0x180284b63
00284BC8: lea       rcx,[rip+0xbfc869]        # 0x180e81438
00284BCF: call      0x1802391bc
00284BD4: cmp       DWORD PTR [rip+0xbfc85d],0xffffffff        # 0x180e81438
00284BDB: jne       0x180284a9f
00284BE1: call      0x18017cc20
00284BE6: mov       rcx,rax
00284BE9: lea       rdx,[rip+0x1675e0]        # 0x1803ec1d0 ; 'ReShadeRegisterEvent'
00284BF0: call      QWORD PTR [rip+0x3e562]        # 0x1802c3158
00284BF6: lea       rcx,[rip+0xbfc83b]        # 0x180e81438
00284BFD: mov       QWORD PTR [rip+0xbfc83c],rax        # 0x180e81440
00284C04: call      0x180239150
00284C09: jmp       0x180284a9f
00284C0E: lea       rcx,[rip+0xbfc833]        # 0x180e81448
00284C15: call      0x1802391bc
00284C1A: cmp       DWORD PTR [rip+0xbfc827],0xffffffff        # 0x180e81448
00284C21: jne       0x180284ac7
00284C27: call      0x18017cc20
00284C2C: mov       rcx,rax
00284C2F: lea       rdx,[rip+0x16759a]        # 0x1803ec1d0 ; 'ReShadeRegisterEvent'
00284C36: call      QWORD PTR [rip+0x3e51c]        # 0x1802c3158
00284C3C: lea       rcx,[rip+0xbfc805]        # 0x180e81448
00284C43: mov       QWORD PTR [rip+0xbfc806],rax        # 0x180e81450
00284C4A: call      0x180239150
00284C4F: jmp       0x180284ac7
00284C54: lea       rcx,[rip+0xbfc7fd]        # 0x180e81458
00284C5B: call      0x1802391bc
00284C60: cmp       DWORD PTR [rip+0xbfc7f1],0xffffffff        # 0x180e81458
00284C67: jne       0x180284aef
00284C6D: call      0x18017cc20
00284C72: mov       rcx,rax
00284C75: lea       rdx,[rip+0x167554]        # 0x1803ec1d0 ; 'ReShadeRegisterEvent'
00284C7C: call      QWORD PTR [rip+0x3e4d6]        # 0x1802c3158
00284C82: lea       rcx,[rip+0xbfc7cf]        # 0x180e81458
00284C89: mov       QWORD PTR [rip+0xbfc7d0],rax        # 0x180e81460
00284C90: call      0x180239150
00284C95: jmp       0x180284aef
00284C9A: lea       rcx,[rip+0xbfc7c7]        # 0x180e81468
00284CA1: call      0x1802391bc
00284CA6: cmp       DWORD PTR [rip+0xbfc7bb],0xffffffff        # 0x180e81468
00284CAD: jne       0x180284b17
00284CB3: call      0x18017cc20
00284CB8: mov       rcx,rax
00284CBB: lea       rdx,[rip+0x16750e]        # 0x1803ec1d0 ; 'ReShadeRegisterEvent'
00284CC2: call      QWORD PTR [rip+0x3e490]        # 0x1802c3158
00284CC8: lea       rcx,[rip+0xbfc799]        # 0x180e81468
00284CCF: mov       QWORD PTR [rip+0xbfc79a],rax        # 0x180e81470
00284CD6: call      0x180239150
00284CDB: jmp       0x180284b17
00284CE0: lea       rcx,[rip+0xbfc791]        # 0x180e81478
00284CE7: call      0x1802391bc
00284CEC: cmp       DWORD PTR [rip+0xbfc785],0xffffffff        # 0x180e81478
00284CF3: jne       0x180284b3f
00284CF9: call      0x18017cc20
00284CFE: mov       rcx,rax
00284D01: lea       rdx,[rip+0x1674c8]        # 0x1803ec1d0 ; 'ReShadeRegisterEvent'
00284D08: call      QWORD PTR [rip+0x3e44a]        # 0x1802c3158
00284D0E: lea       rcx,[rip+0xbfc763]        # 0x180e81478
00284D15: mov       QWORD PTR [rip+0xbfc764],rax        # 0x180e81480
00284D1C: call      0x180239150
00284D21: jmp       0x180284b3f
