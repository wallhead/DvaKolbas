; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xFE2E0..0xFE5A1; unnamed
000FE2E0: mov       QWORD PTR [rsp+0x8],rbx
000FE2E5: mov       QWORD PTR [rsp+0x10],rbp
000FE2EA: mov       QWORD PTR [rsp+0x18],rsi
000FE2EF: push      rdi
000FE2F0: sub       rsp,0x610
000FE2F7: mov       rsi,rdx
000FE2FA: mov       rbp,rcx
000FE2FD: mov       edx,DWORD PTR [rdx]
000FE2FF: call      0x1800fe650
000FE304: test      al,al
000FE306: je        0x1800fe30d
000FE308: call      0x1800fe5b0
000FE30D: movzx     eax,BYTE PTR [rsi+0x1c]
000FE311: lea       rcx,[rsp+0x30]
000FE316: xor       edx,edx
000FE318: mov       BYTE PTR [rbp+0x12c],al
000FE31E: mov       r8d,0x2e8
000FE324: call      0x18010d61a
000FE329: mov       rax,QWORD PTR [rip+0x11216c8]        # 0x18121f9f8
000FE330: mov       ecx,0x1
000FE335: mov       rdx,QWORD PTR [rax+0x90]
000FE33C: call      rdx
000FE33E: mov       edx,0x1
000FE343: mov       rcx,rax
000FE346: mov       rdi,rax
000FE349: call      QWORD PTR [rip+0x153a9]        # 0x1801136f8 ; api-ms-win-crt-heap-l1-1-0.dll!calloc
000FE34F: mov       r8,rdi
000FE352: xor       edx,edx
000FE354: mov       rcx,rax
000FE357: mov       rbx,rax
000FE35A: call      0x18010d61a
000FE35F: mov       rcx,QWORD PTR [rip+0x1121692]        # 0x18121f9f8
000FE366: mov       r9,rdi
000FE369: mov       rdx,QWORD PTR [rbp+0x118]
000FE370: mov       r8,rbx
000FE373: mov       DWORD PTR [rsp+0x20],0x1
000FE37B: mov       r10,QWORD PTR [rcx+0xa0]
000FE382: lea       rcx,[rsp+0x138]
000FE38A: call      r10
000FE38D: test      eax,eax
000FE38F: je        0x1800fe3a6
000FE391: mov       edx,eax
000FE393: lea       rcx,[rip+0x10adb2e]        # 0x1811abec8 ; 'ffxGetInterface for DX11 failed! ErrorCode: 0x%08x'
000FE39A: call      0x1800fbb40
000FE39F: xor       al,al
000FE3A1: jmp       0x1800fe588
000FE3A6: mov       ecx,DWORD PTR [rsi+0xc]
000FE3A9: mov       eax,DWORD PTR [rsi+0x10]
000FE3AC: cmp       BYTE PTR [rsi+0x19],0x0
000FE3B0: mov       DWORD PTR [rsp+0x38],eax
000FE3B4: mov       DWORD PTR [rsp+0x40],eax
000FE3B8: mov       DWORD PTR [rsp+0x48],eax
000FE3BC: mov       eax,0x220
000FE3C1: mov       DWORD PTR [rsp+0x34],ecx
000FE3C5: mov       DWORD PTR [rsp+0x3c],ecx
000FE3C9: mov       DWORD PTR [rsp+0x44],ecx
000FE3CD: mov       ecx,0x228
000FE3D2: cmovne    eax,ecx
000FE3D5: cmp       BYTE PTR [rsi+0x18],0x0
000FE3D9: mov       DWORD PTR [rsp+0x30],eax
000FE3DD: je        0x1800fe3e6
000FE3DF: or        eax,0x1
000FE3E2: mov       DWORD PTR [rsp+0x30],eax
000FE3E6: cmp       BYTE PTR [rsi+0x25],0x0
000FE3EA: je        0x1800fe3f3
000FE3EC: or        eax,0x2
000FE3EF: mov       DWORD PTR [rsp+0x30],eax
000FE3F3: mov       rax,QWORD PTR [rip+0x11215fe]        # 0x18121f9f8
000FE3FA: mov       rdx,rsi
000FE3FD: mov       rbx,QWORD PTR [rax+0xc8]
000FE404: call      0x1800ffc10
000FE409: lea       rdx,[rsp+0x30]
000FE40E: mov       rcx,rax
000FE411: call      rbx
000FE413: test      eax,eax
000FE415: je        0x1800fe42c
000FE417: mov       edx,eax
000FE419: lea       rcx,[rip+0x10adae0]        # 0x1811abf00 ; 'ffxFsr3ContextCreate failed! ErrorCode: 0x%08x'
000FE420: call      0x1800fbb40
000FE425: xor       al,al
000FE427: jmp       0x1800fe588
000FE42C: mov       ebx,0x5
000FE431: lea       rcx,[rsp+0x320]
000FE439: mov       edx,ebx
000FE43B: lea       rax,[rsp+0x30]
000FE440: lea       rcx,[rcx+0x80]
000FE447: movups    xmm0,XMMWORD PTR [rax]
000FE44A: movups    xmm1,XMMWORD PTR [rax+0x10]
000FE44E: lea       rax,[rax+0x80]
000FE455: movups    XMMWORD PTR [rcx-0x80],xmm0
000FE459: movups    xmm0,XMMWORD PTR [rax-0x60]
000FE45D: movups    XMMWORD PTR [rcx-0x70],xmm1
000FE461: movups    xmm1,XMMWORD PTR [rax-0x50]
000FE465: movups    XMMWORD PTR [rcx-0x60],xmm0
000FE469: movups    xmm0,XMMWORD PTR [rax-0x40]
000FE46D: movups    XMMWORD PTR [rcx-0x50],xmm1
000FE471: movups    xmm1,XMMWORD PTR [rax-0x30]
000FE475: movups    XMMWORD PTR [rcx-0x40],xmm0
000FE479: movups    xmm0,XMMWORD PTR [rax-0x20]
000FE47D: movups    XMMWORD PTR [rcx-0x30],xmm1
000FE481: movups    xmm1,XMMWORD PTR [rax-0x10]
000FE485: movups    XMMWORD PTR [rcx-0x20],xmm0
000FE489: movups    XMMWORD PTR [rcx-0x10],xmm1
000FE48D: sub       rdx,0x1
000FE491: jne       0x1800fe440
000FE493: movups    xmm0,XMMWORD PTR [rax]
000FE496: mov       rdx,rsi
000FE499: movups    xmm1,XMMWORD PTR [rax+0x10]
000FE49D: movups    XMMWORD PTR [rcx],xmm0
000FE4A0: movups    xmm0,XMMWORD PTR [rax+0x20]
000FE4A4: movups    XMMWORD PTR [rcx+0x10],xmm1
000FE4A8: movups    xmm1,XMMWORD PTR [rax+0x30]
000FE4AC: movups    XMMWORD PTR [rcx+0x20],xmm0
000FE4B0: movups    xmm0,XMMWORD PTR [rax+0x40]
000FE4B4: movups    XMMWORD PTR [rcx+0x30],xmm1
000FE4B8: movups    xmm1,XMMWORD PTR [rax+0x50]
000FE4BC: mov       rax,QWORD PTR [rax+0x60]
000FE4C0: movups    XMMWORD PTR [rcx+0x40],xmm0
000FE4C4: movups    XMMWORD PTR [rcx+0x50],xmm1
000FE4C8: mov       QWORD PTR [rcx+0x60],rax
000FE4CC: call      0x1800ffa60
000FE4D1: mov       rdx,rax
000FE4D4: lea       rcx,[rsp+0x320]
000FE4DC: nop       DWORD PTR [rax+0x0]
000FE4E0: lea       rdx,[rdx+0x80]
000FE4E7: movups    xmm0,XMMWORD PTR [rcx]
000FE4EA: movups    xmm1,XMMWORD PTR [rcx+0x10]
000FE4EE: lea       rcx,[rcx+0x80]
000FE4F5: movups    XMMWORD PTR [rdx-0x80],xmm0
000FE4F9: movups    xmm0,XMMWORD PTR [rcx-0x60]
000FE4FD: movups    XMMWORD PTR [rdx-0x70],xmm1
000FE501: movups    xmm1,XMMWORD PTR [rcx-0x50]
000FE505: movups    XMMWORD PTR [rdx-0x60],xmm0
000FE509: movups    xmm0,XMMWORD PTR [rcx-0x40]
000FE50D: movups    XMMWORD PTR [rdx-0x50],xmm1
000FE511: movups    xmm1,XMMWORD PTR [rcx-0x30]
000FE515: movups    XMMWORD PTR [rdx-0x40],xmm0
000FE519: movups    xmm0,XMMWORD PTR [rcx-0x20]
000FE51D: movups    XMMWORD PTR [rdx-0x30],xmm1
000FE521: movups    xmm1,XMMWORD PTR [rcx-0x10]
000FE525: movups    XMMWORD PTR [rdx-0x20],xmm0
000FE529: movups    XMMWORD PTR [rdx-0x10],xmm1
000FE52D: sub       rbx,0x1
000FE531: jne       0x1800fe4e0
000FE533: movups    xmm0,XMMWORD PTR [rcx]
000FE536: mov       rax,QWORD PTR [rcx+0x60]
000FE53A: movups    xmm1,XMMWORD PTR [rcx+0x10]
000FE53E: movups    XMMWORD PTR [rdx],xmm0
000FE541: movups    xmm0,XMMWORD PTR [rcx+0x20]
000FE545: movups    XMMWORD PTR [rdx+0x10],xmm1
000FE549: movups    xmm1,XMMWORD PTR [rcx+0x30]
000FE54D: movups    XMMWORD PTR [rdx+0x20],xmm0
000FE551: movups    xmm0,XMMWORD PTR [rcx+0x40]
000FE555: movups    XMMWORD PTR [rdx+0x30],xmm1
000FE559: movups    xmm1,XMMWORD PTR [rcx+0x50]
000FE55D: lea       rcx,[rip+0x10ada24]        # 0x1811abf88 ; 'ffxFsr3ContextCreate success!'
000FE564: movups    XMMWORD PTR [rdx+0x40],xmm0
000FE568: movups    XMMWORD PTR [rdx+0x50],xmm1
000FE56C: mov       QWORD PTR [rdx+0x60],rax
000FE570: call      0x1800fbb40
000FE575: cmp       QWORD PTR [rip+0x1117eb3],0x1        # 0x181216430
000FE57D: jbe       0x1800fe586
000FE57F: mov       BYTE PTR [rbp+0x13a],0x1
000FE586: mov       al,0x1
000FE588: lea       r11,[rsp+0x610]
000FE590: mov       rbx,QWORD PTR [r11+0x10]
000FE594: mov       rbp,QWORD PTR [r11+0x18]
000FE598: mov       rsi,QWORD PTR [r11+0x20]
000FE59C: mov       rsp,r11
000FE59F: pop       rdi
000FE5A0: ret       
