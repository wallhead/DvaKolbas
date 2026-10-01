; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x19EC61..0x19EE02; unnamed
0019EC61: cmp       BYTE PTR [rax+0x90],0x0
0019EC68: jne       0x18019ec7a
0019EC6A: mov       rax,QWORD PTR [rip+0xce216f]        # 0x180e80de0
0019EC71: cmp       BYTE PTR [rax+0x343],0x0
0019EC78: je        0x18019ecaa
0019EC7A: mov       r8d,0x5
0019EC80: lea       rdx,[rip+0x109d09]        # 0x1802a8990
0019EC87: mov       rcx,QWORD PTR [r14]
0019EC8A: call      0x1801be000
0019EC8F: mov       QWORD PTR [rip+0x2de7f2],rax        # 0x18047d488
0019EC96: mov       rax,QWORD PTR [rip+0xce2143]        # 0x180e80de0
0019EC9D: cmp       BYTE PTR [rax+0x343],0x0
0019ECA4: jne       0x18019ed8a
0019ECAA: mov       r8d,0xa
0019ECB0: lea       rdx,[rip+0x109859]        # 0x1802a8510
0019ECB7: mov       rcx,QWORD PTR [rdi]
0019ECBA: call      0x1801be000
0019ECBF: mov       QWORD PTR [rip+0x2de7a2],rax        # 0x18047d468
0019ECC6: mov       r8d,0x1a
0019ECCC: lea       rdx,[rip+0x1097dd]        # 0x1802a84b0
0019ECD3: mov       rcx,QWORD PTR [rdi]
0019ECD6: call      0x1801be000
0019ECDB: mov       QWORD PTR [rip+0x2de78e],rax        # 0x18047d470
0019ECE2: mov       r8d,0x20
0019ECE8: lea       rdx,[rip+0x109761]        # 0x1802a8450
0019ECEF: mov       rcx,QWORD PTR [rdi]
0019ECF2: call      0x1801be000
0019ECF7: mov       QWORD PTR [rip+0x2de74a],rax        # 0x18047d448
0019ECFE: mov       r8d,0x3d
0019ED04: lea       rdx,[rip+0x1096e5]        # 0x1802a83f0
0019ED0B: mov       rcx,QWORD PTR [rdi]
0019ED0E: call      0x1801be000
0019ED13: mov       QWORD PTR [rip+0x2de736],rax        # 0x18047d450
0019ED1A: mov       r8d,0x41
0019ED20: lea       rdx,[rip+0x109669]        # 0x1802a8390
0019ED27: mov       rcx,QWORD PTR [rdi]
0019ED2A: call      0x1801be000
0019ED2F: mov       QWORD PTR [rip+0x2de722],rax        # 0x18047d458
0019ED36: mov       r8d,0x46
0019ED3C: lea       rdx,[rip+0x1095ed]        # 0x1802a8330
0019ED43: mov       rcx,QWORD PTR [rdi]
0019ED46: call      0x1801be000
0019ED4B: mov       QWORD PTR [rip+0x2de70e],rax        # 0x18047d460
0019ED52: mov       r8d,0x8
0019ED58: lea       rdx,[rip+0x10a9e1]        # 0x1802a9740
0019ED5F: mov       rcx,QWORD PTR [rdi]
0019ED62: call      0x1801be000
0019ED67: mov       QWORD PTR [rip+0x2de69a],rax        # 0x18047d408
0019ED6E: mov       r8d,0x2c
0019ED74: lea       rdx,[rip+0x10a875]        # 0x1802a95f0
0019ED7B: mov       rcx,QWORD PTR [rdi]
0019ED7E: call      0x1801be000
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
