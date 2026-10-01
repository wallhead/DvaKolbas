; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x293820..0x2939DE; unnamed
00293820: mov       QWORD PTR [rsp+0x10],rbx
00293825: mov       QWORD PTR [rsp+0x18],rsi
0029382A: mov       QWORD PTR [rsp+0x20],rdi
0029382F: mov       QWORD PTR [rsp+0x8],rcx
00293834: push      rbp
00293835: push      r14
00293837: push      r15
00293839: lea       rbp,[rsp-0x47]
0029383E: sub       rsp,0xb0
00293845: mov       rbx,QWORD PTR [rip+0xbed594]        # 0x180e80de0
0029384C: mov       rdi,rdx
0029384F: lea       r14,[rip+0x1728da]        # 0x180406130
00293856: lea       r15,[rip+0x173c63]        # 0x1804074c0
0029385D: mov       QWORD PTR [rbx+0x828],rdx
00293864: test      rdx,rdx
00293867: je        0x180293918
0029386D: cmp       QWORD PTR [rbx+0xba8],0x0
00293875: jne       0x180293918
0029387B: cmp       BYTE PTR [rbx+0x343],0x0
00293882: jne       0x180293918
00293888: mov       rax,QWORD PTR [rdx]
0029388B: mov       rcx,rdi
0029388E: lea       rdx,[rbp+0x17]
00293892: call      QWORD PTR [rax+0x50]
00293895: mov       rcx,QWORD PTR [rbx+0x1678]
0029389C: lea       r9,[rbx+0xba8]
002938A3: or        DWORD PTR [rbp+0x37],0x80
002938AA: lea       rdx,[rbp+0x17]
002938AE: xor       r8d,r8d
002938B1: mov       rax,QWORD PTR [rcx]
002938B4: call      QWORD PTR [rax+0x28]
002938B7: mov       DWORD PTR [rbp+0x67],eax
002938BA: test      eax,eax
002938BC: jns       0x180293918
002938BE: call      0x180222050
002938C3: mov       ecx,DWORD PTR [rbp+0x3]
002938C6: lea       r9,[rbp-0x39]
002938CA: mov       DWORD PTR [rbp-0x1d],ecx
002938CD: lea       rdx,[rbp-0x9]
002938D1: lea       rcx,[rip+0x173bb0]        # 0x180407488
002938D8: mov       QWORD PTR [rbp-0x29],r14
002938DC: mov       QWORD PTR [rbp-0x39],rcx
002938E0: lea       rcx,[rbp+0x67]
002938E4: mov       QWORD PTR [rsp+0x20],rcx
002938E9: mov       rcx,rax
002938EC: mov       DWORD PTR [rbp-0x21],0xbb5
002938F3: vmovups   xmm0,XMMWORD PTR [rbp-0x29]
002938F8: mov       QWORD PTR [rbp-0x19],r15
002938FC: vmovsd    xmm1,QWORD PTR [rbp-0x19]
00293901: mov       QWORD PTR [rbp-0x31],0x2f
00293909: vmovups   XMMWORD PTR [rbp-0x9],xmm0
0029390E: vmovsd    QWORD PTR [rbp+0x7],xmm1
00293913: call      0x180196e10
00293918: cmp       QWORD PTR [rbx+0x880],0x0
00293920: jne       0x1802939c1
00293926: mov       rax,QWORD PTR [rdi]
00293929: lea       rdx,[rbp+0x17]
0029392D: mov       rcx,rdi
00293930: call      QWORD PTR [rax+0x50]
00293933: mov       eax,DWORD PTR [rbx+0x270]
00293939: lea       r9,[rbx+0x880]
00293940: mov       rcx,QWORD PTR [rbx+0x1678]
00293947: lea       rdx,[rbp+0x17]
0029394B: mov       DWORD PTR [rbp+0x17],eax
0029394E: xor       r8d,r8d
00293951: mov       eax,DWORD PTR [rbx+0x274]
00293957: mov       DWORD PTR [rbp+0x1b],eax
0029395A: mov       rax,QWORD PTR [rcx]
0029395D: call      QWORD PTR [rax+0x28]
00293960: mov       DWORD PTR [rbp+0x67],eax
00293963: test      eax,eax
00293965: jns       0x1802939c1
00293967: call      0x180222050
0029396C: mov       ecx,DWORD PTR [rbp+0x3]
0029396F: lea       r9,[rbp-0x39]
00293973: mov       DWORD PTR [rbp-0x1d],ecx
00293976: lea       rdx,[rbp-0x9]
0029397A: lea       rcx,[rip+0x173bc7]        # 0x180407548
00293981: mov       QWORD PTR [rbp-0x29],r14
00293985: mov       QWORD PTR [rbp-0x39],rcx
00293989: lea       rcx,[rbp+0x67]
0029398D: mov       QWORD PTR [rsp+0x20],rcx
00293992: mov       rcx,rax
00293995: mov       DWORD PTR [rbp-0x21],0xbbe
0029399C: vmovups   xmm0,XMMWORD PTR [rbp-0x29]
002939A1: mov       QWORD PTR [rbp-0x19],r15
002939A5: vmovsd    xmm1,QWORD PTR [rbp-0x19]
002939AA: mov       QWORD PTR [rbp-0x31],0x37
002939B2: vmovups   XMMWORD PTR [rbp-0x9],xmm0
002939B7: vmovsd    QWORD PTR [rbp+0x7],xmm1
002939BC: call      0x180196e10
002939C1: lea       r11,[rsp+0xb0]
002939C9: mov       rbx,QWORD PTR [r11+0x28]
002939CD: mov       rsi,QWORD PTR [r11+0x30]
002939D1: mov       rdi,QWORD PTR [r11+0x38]
002939D5: mov       rsp,r11
002939D8: pop       r15
002939DA: pop       r14
002939DC: pop       rbp
002939DD: ret       
