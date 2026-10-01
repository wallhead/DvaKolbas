; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x19FC80..0x19FD89; unnamed
0019FC85: add       rcx,0x9e0
0019FC8C: jmp       0x18019fca0
0019FC8E: mov       rax,QWORD PTR [rip+0xce128b]        # 0x180e80f20
0019FC95: imul      rcx,r13,0x58
0019FC99: add       rax,0x40
0019FC9D: add       rcx,rax
0019FCA0: call      0x18016c140
0019FCA5: vmovss    xmm3,DWORD PTR [rip+0x26c087]        # 0x18040bd34
0019FCAD: mov       QWORD PTR [rbp+0x77],rax
0019FCB1: mov       r8d,0x3
0019FCB7: mov       rax,QWORD PTR [rip+0xce1262]        # 0x180e80f20
0019FCBE: mov       rdx,rbx
0019FCC1: mov       BYTE PTR [rsp+0x20],0x0
0019FCC6: mov       rcx,QWORD PTR [rax+0x250]
0019FCCD: mov       rax,QWORD PTR [rcx]
0019FCD0: call      QWORD PTR [rax+0x1a8]
0019FCD6: mov       rax,QWORD PTR [rip+0xce1243]        # 0x180e80f20
0019FCDD: lea       r8,[rbp+0x77]
0019FCE1: mov       r9,rbx
0019FCE4: mov       edx,0x1
0019FCE9: mov       rcx,QWORD PTR [rax+0x250]
0019FCF0: mov       rax,QWORD PTR [rcx]
0019FCF3: call      QWORD PTR [rax+0x108]
0019FCF9: mov       rcx,QWORD PTR [rip+0xce10e0]        # 0x180e80de0
0019FD00: mov       BYTE PTR [rip+0xce138b],0x1        # 0x180e81092
0019FD07: call      0x180293f20
0019FD0C: mov       rcx,QWORD PTR [rip+0xce10cd]        # 0x180e80de0
0019FD13: test      al,al
0019FD15: jne       0x18019fd29
0019FD17: cmp       BYTE PTR [rcx+0x3d0],al
0019FD1D: je        0x18019fd76
0019FD1F: cmp       QWORD PTR [rcx+0x1570],0x0
0019FD27: je        0x18019fd76
0019FD29: add       rcx,0x9e0
0019FD30: cmp       QWORD PTR [rcx],0x0
0019FD34: je        0x18019fd76
0019FD36: mov       rax,QWORD PTR [rip+0xce11e3]        # 0x180e80f20
0019FD3D: vxorps    xmm0,xmm0,xmm0
0019FD41: vmovups   XMMWORD PTR [rbp+0x7],xmm0
0019FD46: mov       rdi,QWORD PTR [rax+0x250]
0019FD4D: mov       rax,QWORD PTR [rdi]
0019FD50: mov       rbx,QWORD PTR [rax+0x190]
0019FD57: call      0x18016c140
0019FD5C: lea       r8,[rbp+0x7]
0019FD60: mov       rdx,rax
0019FD63: mov       rcx,rdi
0019FD66: call      rbx
0019FD68: mov       rax,QWORD PTR [rip+0xce1071]        # 0x180e80de0
0019FD6F: mov       BYTE PTR [rax+0x3d3],0x1
0019FD76: mov       r13,QWORD PTR [rsp+0xb0]
0019FD7E: add       rsp,0xc0
0019FD85: pop       rdi
0019FD86: pop       rbx
0019FD87: pop       rbp
0019FD88: ret       
