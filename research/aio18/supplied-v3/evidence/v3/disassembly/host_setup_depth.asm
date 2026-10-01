; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x293C30..0x293D9B; unnamed
00293C30: test      rdx,rdx
00293C33: je        0x180293d9a
00293C39: mov       QWORD PTR [rsp+0x10],rbx
00293C3E: push      rdi
00293C3F: sub       rsp,0x70
00293C43: cmp       QWORD PTR [rcx+0x1678],0x0
00293C4B: mov       rdi,rdx
00293C4E: mov       rbx,rcx
00293C51: je        0x180293d8d
00293C57: cmp       QWORD PTR [rcx+0x1680],0x0
00293C5F: je        0x180293d8d
00293C65: mov       QWORD PTR [rsp+0x80],rsi
00293C6D: cmp       QWORD PTR [rcx+0x7d0],rdx
00293C74: je        0x180293c8b
00293C76: xor       edx,edx
00293C78: add       rcx,0x7d0
00293C7F: call      0x180152cd0
00293C84: mov       QWORD PTR [rbx+0x7d0],rdi
00293C8B: xor       eax,eax
00293C8D: lea       rdx,[rsp+0x40]
00293C92: vpxor     xmm0,xmm0,xmm0
00293C96: mov       QWORD PTR [rsp+0x60],rax
00293C9B: mov       rcx,rdi
00293C9E: mov       DWORD PTR [rsp+0x68],eax
00293CA2: mov       rax,QWORD PTR [rdi]
00293CA5: vmovups   YMMWORD PTR [rsp+0x40],ymm0
00293CAB: vzeroupper 
00293CAE: call      QWORD PTR [rax+0x50]
00293CB1: mov       r9d,DWORD PTR [rbx+0x274]
00293CB8: lea       rax,[rsp+0x40]
00293CBD: mov       r8d,DWORD PTR [rbx+0x270]
00293CC4: lea       rdx,[rbx+0x8d8]
00293CCB: mov       QWORD PTR [rsp+0x30],rax
00293CD0: lea       rcx,[rsp+0x30]
00293CD5: lea       rax,[rip+0x173744]        # 0x180407420
00293CDC: mov       QWORD PTR [rsp+0x38],rbx
00293CE1: mov       QWORD PTR [rsp+0x20],rax
00293CE6: call      0x180293a10
00293CEB: mov       r9d,DWORD PTR [rbx+0x274]
00293CF2: lea       rax,[rip+0x173777]        # 0x180407470
00293CF9: mov       r8d,DWORD PTR [rbx+0x270]
00293D00: lea       rdx,[rbx+0x930]
00293D07: lea       rcx,[rsp+0x30]
00293D0C: mov       QWORD PTR [rsp+0x20],rax
00293D11: call      0x180293a10
00293D16: mov       rsi,QWORD PTR [rsp+0x80]
00293D1E: test      al,al
00293D20: je        0x180293d5d
00293D22: lea       rcx,[rbx+0x930]
00293D29: call      0x18016c450
00293D2E: test      rax,rax
00293D31: je        0x180293d5d
00293D33: mov       rcx,QWORD PTR [rbx+0x1680]
00293D3A: mov       r8d,0x3
00293D40: vmovss    xmm3,DWORD PTR [rip+0x177fec]        # 0x18040bd34
00293D48: mov       BYTE PTR [rsp+0x20],0x0
00293D4D: mov       rdx,QWORD PTR [rcx]
00293D50: mov       r9,QWORD PTR [rdx+0x1a8]
00293D57: mov       rdx,rax
00293D5A: call      r9
00293D5D: cmp       BYTE PTR [rbx+0x343],0x0
00293D64: jne       0x180293d8d
00293D66: mov       r9d,DWORD PTR [rsp+0x44]
00293D6B: lea       rax,[rip+0x1736ee]        # 0x180407460
00293D72: mov       r8d,DWORD PTR [rsp+0x40]
00293D77: lea       rdx,[rbx+0xb50]
00293D7E: lea       rcx,[rsp+0x30]
00293D83: mov       QWORD PTR [rsp+0x20],rax
00293D88: call      0x180293a10
00293D8D: mov       rbx,QWORD PTR [rsp+0x88]
00293D95: add       rsp,0x70
00293D99: pop       rdi
00293D9A: ret       
