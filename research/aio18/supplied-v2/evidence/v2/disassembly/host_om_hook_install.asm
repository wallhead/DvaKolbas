; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x19ED7F..0x19EE0D; unnamed
0019ED83: mov       QWORD PTR [rip+0x2de6ae],rax        # 0x18047d438
0019ED8A: mov       rax,QWORD PTR [rdi]
0019ED8D: mov       rdx,QWORD PTR [rax+0x108]
0019ED94: mov       QWORD PTR [rbp-0x50],0x0
0019ED9C: lea       r8,[rbp-0x50]
0019EDA0: mov       ecx,0x6
0019EDA5: call      QWORD PTR [rip+0x1244b5]        # 0x1802c3260
0019EDAB: test      eax,eax
0019EDAD: je        0x18019edd8
0019EDAF: lea       rcx,[rip+0x24699a]        # 0x1803e5750 ; 'd3d11.dll'
0019EDB6: call      QWORD PTR [rip+0x1243ac]        # 0x1802c3168
0019EDBC: cmp       QWORD PTR [rbp-0x50],rax
0019EDC0: jne       0x18019edd8
0019EDC2: mov       BYTE PTR [rsp+0x60],0x1
0019EDC7: mov       QWORD PTR [rip+0xce21d2],rdi        # 0x180e80fa0
0019EDCE: mov       rcx,rdi
0019EDD1: call      0x1802a9a00
0019EDD6: jmp       0x18019edf9
0019EDD8: mov       BYTE PTR [rsp+0x60],0x0
0019EDDD: mov       r8d,0x21
0019EDE3: lea       rdx,[rip+0x10ad46]        # 0x1802a9b30
0019EDEA: mov       rcx,QWORD PTR [rdi]
0019EDED: call      0x1801be000
0019EDF2: mov       QWORD PTR [rip+0x2de647],rax        # 0x18047d440
0019EDF9: mov       QWORD PTR [rbp-0x40],rdi
0019EDFD: call      0x180222050
0019EE02: mov       QWORD PTR [rsp+0x68],rbx
0019EE07: mov       DWORD PTR [rsp+0x70],0x794
