; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xEF230..0xEF876; unnamed
000EF230: rex       push rbp
000EF232: push      rbx
000EF233: push      rsi
000EF234: push      rdi
000EF235: lea       rbp,[rsp-0x118]
000EF23D: sub       rsp,0x218
000EF244: mov       rax,QWORD PTR [rip+0x10db775]        # 0x1811ca9c0
000EF24B: xor       rax,rsp
000EF24E: mov       QWORD PTR [rbp+0xd8],rax
000EF255: cmp       BYTE PTR [rcx+0x144],0x0
000EF25C: mov       rdi,rdx
000EF25F: mov       rbx,rcx
000EF262: jne       0x1800ef278
000EF264: mov       rax,QWORD PTR [rcx]
000EF267: mov       edx,DWORD PTR [rdx+0x98]
000EF26D: call      QWORD PTR [rax+0x40]
000EF270: test      al,al
000EF272: jne       0x1800ef85b
000EF278: movaps    XMMWORD PTR [rsp+0x1f0],xmm6
000EF280: mov       BYTE PTR [rbx+0x810],0x1
000EF287: call      0x1800badc0
000EF28C: test      al,al
000EF28E: je        0x1800ef2ed
000EF290: mov       eax,DWORD PTR [rdi+0x98]
000EF296: mov       DWORD PTR [rbx+0x1d8],eax
000EF29C: mov       rax,QWORD PTR [rdi+0x90]
000EF2A3: mov       QWORD PTR [rbx+0x1e0],rax
000EF2AA: mov       rax,QWORD PTR [rdi+0x88]
000EF2B1: mov       QWORD PTR [rbx+0x1e8],rax
000EF2B8: mov       rax,QWORD PTR [rdi+0x20]
000EF2BC: mov       QWORD PTR [rbx+0x1f0],rax
000EF2C3: mov       rax,QWORD PTR [rdi+0x18]
000EF2C7: mov       QWORD PTR [rbx+0x1f8],rax
000EF2CE: mov       rax,QWORD PTR [rdi+0x8]
000EF2D2: mov       QWORD PTR [rbx+0x200],rax
000EF2D9: movzx     eax,BYTE PTR [rdi+0x9c]
000EF2E0: mov       BYTE PTR [rbx+0x208],al
000EF2E6: mov       BYTE PTR [rbx+0x209],0x1
000EF2ED: mov       rsi,QWORD PTR [rdi+0x10]
000EF2F1: movzx     eax,BYTE PTR [rdi+0x9c]
000EF2F8: mov       BYTE PTR [rbx+0x13c],al
000EF2FE: test      rsi,rsi
000EF301: je        0x1800ef3a1
000EF307: mov       rax,QWORD PTR [rsi]
000EF30A: lea       rdx,[rbp+0xa0]
000EF311: mov       rcx,rsi
000EF314: call      QWORD PTR [rax+0x50]
000EF317: mov       r8d,DWORD PTR [rbx+0x134]
000EF31E: test      r8d,r8d
000EF321: je        0x1800ef39a
000EF323: mov       edx,DWORD PTR [rbx+0x138]
000EF329: test      edx,edx
000EF32B: je        0x1800ef39a
000EF32D: mov       rcx,QWORD PTR [rbp+0xb0]
000EF334: xorps     xmm2,xmm2
000EF337: test      rcx,rcx
000EF33A: js        0x1800ef343
000EF33C: cvtsi2ss  xmm2,rcx
000EF341: jmp       0x1800ef358
000EF343: mov       rax,rcx
000EF346: and       ecx,0x1
000EF349: shr       rax,1
000EF34C: or        rax,rcx
000EF34F: cvtsi2ss  xmm2,rax
000EF354: addss     xmm2,xmm2
000EF358: mov       eax,DWORD PTR [rbp+0xb8]
000EF35E: xorps     xmm1,xmm1
000EF361: movd      xmm0,r8d
000EF366: cvtdq2ps  xmm0,xmm0
000EF369: cvtsi2ss  xmm1,rax
000EF36E: divss     xmm2,xmm0
000EF372: movd      xmm0,edx
000EF376: cvtdq2ps  xmm0,xmm0
000EF379: divss     xmm1,xmm0
000EF37D: subss     xmm2,xmm1
000EF381: andps     xmm2,XMMWORD PTR [rip+0x10be158]        # 0x1811ad4e0
000EF388: comiss    xmm2,DWORD PTR [rip+0x10bd6e9]        # 0x1811aca78
000EF38F: ja        0x1800ef39a
000EF391: mov       BYTE PTR [rbx+0x145],0x0
000EF398: jmp       0x1800ef3a1
000EF39A: mov       BYTE PTR [rbx+0x145],0x1
000EF3A1: call      0x1800cc790
000EF3A6: mov       r8d,DWORD PTR [rbx+0x8]
000EF3AA: mov       rcx,rax
000EF3AD: mov       edx,DWORD PTR [rbx+0x148]
000EF3B3: call      0x1800ccc80
000EF3B8: call      0x1800edc90
000EF3BD: cmp       QWORD PTR [rbx+0x1a8],0x0
000EF3C5: movaps    xmm6,xmm0
000EF3C8: subsd     xmm6,QWORD PTR [rbx+0x1c0]
000EF3D0: movsd     QWORD PTR [rbx+0x1c0],xmm0
000EF3D8: jne       0x1800ef3e8
000EF3DA: cmp       QWORD PTR [rbx+0x1b0],0x0
000EF3E2: je        0x1800ef853
000EF3E8: mov       r8,QWORD PTR [rdi+0x18]
000EF3EC: lea       rdx,[rsp+0x30]
000EF3F1: xorps     xmm0,xmm0
000EF3F4: mov       QWORD PTR [rsp+0x250],r12
000EF3FC: xor       eax,eax
000EF3FE: mov       QWORD PTR [rsp+0x210],r13
000EF406: mov       QWORD PTR [rsp+0x78],rax
000EF40B: xor       r12d,r12d
000EF40E: mov       WORD PTR [rbp-0x5b],ax
000EF412: mov       rcx,rbx
000EF415: mov       BYTE PTR [rbp-0x59],al
000EF418: mov       rax,QWORD PTR [rdi+0x68]
000EF41C: mov       QWORD PTR [rsp+0x200],r15
000EF424: mov       QWORD PTR [rbp-0x80],rax
000EF428: movaps    XMMWORD PTR [rsp+0x1e0],xmm7
000EF430: movups    XMMWORD PTR [rbp-0x48],xmm0
000EF434: mov       QWORD PTR [rsp+0x68],r12
000EF439: movups    XMMWORD PTR [rbp-0x38],xmm0
000EF43D: mov       QWORD PTR [rsp+0x60],0x2000c
000EF446: movups    XMMWORD PTR [rbp-0x28],xmm0
000EF44A: movups    XMMWORD PTR [rbp-0x18],xmm0
000EF44E: movups    XMMWORD PTR [rbp-0x8],xmm0
000EF452: movups    XMMWORD PTR [rbp+0x8],xmm0
000EF456: movups    XMMWORD PTR [rbp+0x18],xmm0
000EF45A: movups    XMMWORD PTR [rbp+0x28],xmm0
000EF45E: movups    XMMWORD PTR [rbp+0x38],xmm0
000EF462: call      0x1800f0440
000EF467: mov       r8,rsi
000EF46A: lea       rdx,[rsp+0x30]
000EF46F: mov       rcx,rbx
000EF472: movups    xmm0,XMMWORD PTR [rax]
000EF475: movups    XMMWORD PTR [rbp-0x48],xmm0
000EF479: movups    xmm1,XMMWORD PTR [rax+0x10]
000EF47D: movups    XMMWORD PTR [rbp-0x38],xmm1
000EF481: movups    xmm0,XMMWORD PTR [rax+0x20]
000EF485: movups    XMMWORD PTR [rbp-0x28],xmm0
000EF489: call      0x1800f0440
000EF48E: movups    xmm7,XMMWORD PTR [rax]
000EF491: movups    XMMWORD PTR [rbp-0x18],xmm7
000EF495: movups    xmm0,XMMWORD PTR [rax+0x10]
000EF499: movups    XMMWORD PTR [rbp-0x8],xmm0
000EF49D: movups    xmm1,XMMWORD PTR [rax+0x20]
000EF4A1: movups    xmm0,XMMWORD PTR [rdi+0x44]
000EF4A5: movups    XMMWORD PTR [rbp+0x8],xmm1
000EF4A9: movaps    XMMWORD PTR [rbp-0x70],xmm0
000EF4AD: cmp       BYTE PTR [rdi+0x54],r12b
000EF4B1: jne       0x1800ef4c0
000EF4B3: mov       BYTE PTR [rbp-0x5c],r12b
000EF4B7: cmp       BYTE PTR [rbx+0x1c9],r12b
000EF4BE: je        0x1800ef4c4
000EF4C0: mov       BYTE PTR [rbp-0x5c],0x1
000EF4C4: xorps     xmm1,xmm1
000EF4C7: movss     xmm0,DWORD PTR [rdi+0x60]
000EF4CC: cvttss2si rax,DWORD PTR [rdi+0x38]
000EF4D2: mov       DWORD PTR [rbp-0x4c],r12d
000EF4D6: cvtsd2ss  xmm1,xmm6
000EF4DA: mov       DWORD PTR [rbp-0x78],eax
000EF4DD: cvttss2si rax,DWORD PTR [rdi+0x3c]
000EF4E3: movss     xmm6,DWORD PTR [rdi+0x58]
000EF4E8: movss     DWORD PTR [rbp-0x60],xmm1
000EF4ED: movss     xmm1,DWORD PTR [rdi+0x5c]
000EF4F2: movss     DWORD PTR [rbp-0x50],xmm0
000EF4F7: movaps    xmm0,xmm6
000EF4FA: mov       DWORD PTR [rbp-0x74],eax
000EF4FD: cmp       BYTE PTR [rbx+0x29],r12b
000EF501: je        0x1800ef514
000EF503: movss     DWORD PTR [rbp-0x54],xmm6
000EF508: call      0x1800ee440
000EF50D: movss     DWORD PTR [rbp-0x58],xmm0
000EF512: jmp       0x1800ef523
000EF514: call      0x1800ee440
000EF519: movss     DWORD PTR [rbp-0x54],xmm0
000EF51E: movss     DWORD PTR [rbp-0x58],xmm6
000EF523: mov       eax,DWORD PTR [rdi+0x98]
000EF529: lea       r13,[rip+0x1130498]        # 0x18121f9c8
000EF530: mov       QWORD PTR [rsp+0x70],rax
000EF535: mov       edx,r12d
000EF538: mov       rax,QWORD PTR [rbp-0x48]
000EF53C: mov       QWORD PTR [rsp+0x208],r14
000EF544: test      rax,rax
000EF547: je        0x1800ef5a1
000EF549: movq      rcx,xmm7
000EF54E: test      rcx,rcx
000EF551: je        0x1800ef58e
000EF553: lea       rcx,[rsp+0x60]
000EF558: call      0x1800f0b60
000EF55D: mov       rsi,QWORD PTR [rip+0x1126e94]        # 0x1812163f8
000EF564: mov       r14,rax
000EF567: cmp       BYTE PTR [rsi+0xca],dl
000EF56D: jne       0x1800ef577
000EF56F: mov       rcx,rsi
000EF572: call      0x1800cc8c0
000EF577: mov       r8,QWORD PTR [rsi+0x18]
000EF57B: mov       rdx,r14
000EF57E: mov       rcx,r13
000EF581: call      r8
000EF584: mov       rcx,QWORD PTR [rbp-0x18]
000EF588: mov       edx,eax
000EF58A: mov       rax,QWORD PTR [rbp-0x48]
000EF58E: test      rax,rax
000EF591: je        0x1800ef5a1
000EF593: test      rcx,rcx
000EF596: je        0x1800ef5a1
000EF598: test      edx,edx
000EF59A: jne       0x1800ef5a1
000EF59C: mov       r14b,0x1
000EF59F: jmp       0x1800ef5a4
000EF5A1: xor       r14b,r14b
000EF5A4: mov       r8,QWORD PTR [rdi+0x90]
000EF5AB: movaps    xmm7,XMMWORD PTR [rsp+0x1e0]
000EF5B3: test      r8,r8
000EF5B6: je        0x1800ef5db
000EF5B8: cmp       QWORD PTR [rdi+0x88],r12
000EF5BF: jne       0x1800ef5db
000EF5C1: call      0x1800badc0
000EF5C6: test      al,al
000EF5C8: jne       0x1800ef5db
000EF5CA: lea       rdx,[rbp+0xa0]
000EF5D1: mov       rcx,rbx
000EF5D4: call      0x1800f0440
000EF5D9: jmp       0x1800ef5f2
000EF5DB: xorps     xmm0,xmm0
000EF5DE: lea       rax,[rsp+0x30]
000EF5E3: movups    XMMWORD PTR [rsp+0x30],xmm0
000EF5E8: movups    XMMWORD PTR [rsp+0x40],xmm0
000EF5ED: movups    XMMWORD PTR [rsp+0x50],xmm0
000EF5F2: movups    xmm0,XMMWORD PTR [rax]
000EF5F5: movups    xmm1,XMMWORD PTR [rax+0x10]
000EF5F9: movups    xmm2,XMMWORD PTR [rax+0x20]
000EF5FD: cmp       BYTE PTR [rbx+0x144],r12b
000EF604: je        0x1800ef626
000EF606: cmp       BYTE PTR [rbx+0x145],r12b
000EF60D: jne       0x1800ef626
000EF60F: cmp       BYTE PTR [rbx+0x1c9],r12b
000EF616: je        0x1800ef61d
000EF618: test      r14b,r14b
000EF61B: je        0x1800ef626
000EF61D: mov       BYTE PTR [rip+0x110a7e4],0x1        # 0x1811f9e08
000EF624: jmp       0x1800ef62d
000EF626: mov       BYTE PTR [rip+0x110a7db],r12b        # 0x1811f9e08
000EF62D: cmp       DWORD PTR [rbx+0x8],0x1
000EF631: jne       0x1800ef64c
000EF633: call      0x1800badc0
000EF638: test      al,al
000EF63A: je        0x1800ef64c
000EF63C: lea       rax,[rip+0xffffffffffffee8d]        # 0x1800ee4d0
000EF643: mov       QWORD PTR [rip+0x110a79e],rax        # 0x1811f9de8
000EF64A: jmp       0x1800ef653
000EF64C: mov       QWORD PTR [rip+0x110a795],r12        # 0x1811f9de8
000EF653: mov       DWORD PTR [rip+0x110a7e6],r12d        # 0x1811f9e40
000EF65A: mov       ecx,r12d
000EF65D: cmp       BYTE PTR [rbx+0x48],r12b
000EF661: setne     cl
000EF664: mov       DWORD PTR [rip+0x110a7d6],ecx        # 0x1811f9e40
000EF66A: movzx     eax,BYTE PTR [rbx+0x48]
000EF66E: neg       al
000EF670: sbb       edx,edx
000EF672: and       edx,0x4
000EF675: or        edx,ecx
000EF677: mov       DWORD PTR [rip+0x110a7c3],edx        # 0x1811f9e40
000EF67D: movzx     eax,BYTE PTR [rbx+0x146]
000EF684: mov       BYTE PTR [rip+0x110a77f],al        # 0x1811f9e09
000EF68A: lea       rax,[rip+0x1ef]        # 0x1800ef880
000EF691: mov       QWORD PTR [rip+0x110a760],rax        # 0x1811f9df8
000EF698: mov       eax,DWORD PTR [rdi+0x98]
000EF69E: mov       QWORD PTR [rip+0x110a7b3],rax        # 0x1811f9e58
000EF6A5: mov       QWORD PTR [rip+0x110a754],r13        # 0x1811f9e00
000EF6AC: movaps    XMMWORD PTR [rip+0x110a75d],xmm0        # 0x1811f9e10
000EF6B3: movaps    XMMWORD PTR [rip+0x110a766],xmm1        # 0x1811f9e20
000EF6BA: movaps    XMMWORD PTR [rip+0x110a76f],xmm2        # 0x1811f9e30
000EF6C1: mov       eax,DWORD PTR [rbx+0x8]
000EF6C4: mov       DWORD PTR [rsp+0x78],edx
000EF6C8: cmp       eax,0x1
000EF6CB: jne       0x1800ef6d6
000EF6CD: mov       rax,QWORD PTR [rbx+0x1a8]
000EF6D4: jmp       0x1800ef6e2
000EF6D6: cmp       eax,0x2
000EF6D9: jne       0x1800ef6e9
000EF6DB: mov       rax,QWORD PTR [rbx+0x1b0]
000EF6E2: mov       QWORD PTR [rip+0x110a6f7],rax        # 0x1811f9de0
000EF6E9: lea       rcx,[rip+0x110a6e0]        # 0x1811f9dd0
000EF6F0: call      0x1800f0b60
000EF6F5: mov       rsi,QWORD PTR [rip+0x1126cfc]        # 0x1812163f8
000EF6FC: mov       r15,rax
000EF6FF: cmp       BYTE PTR [rsi+0xca],r12b
000EF706: jne       0x1800ef710
000EF708: mov       rcx,rsi
000EF70B: call      0x1800cc8c0
000EF710: mov       r8,QWORD PTR [rsi]
000EF713: mov       rdx,r15
000EF716: mov       rcx,r13
000EF719: call      r8
000EF71C: mov       r15,QWORD PTR [rsp+0x200]
000EF724: mov       esi,eax
000EF726: mov       r13,QWORD PTR [rsp+0x210]
000EF72E: test      eax,eax
000EF730: je        0x1800ef742
000EF732: mov       edx,eax
000EF734: lea       rcx,[rip+0x10ba3c5]        # 0x1811a9b00 ; "Couldn't set the ffxapi framegen config: %d"
000EF73B: call      0x1800fbb40
000EF740: jmp       0x1800ef760
000EF742: test      r14b,r14b
000EF745: je        0x1800ef760
000EF747: cmp       BYTE PTR [rbx+0x144],r12b
000EF74E: je        0x1800ef760
000EF750: cmp       BYTE PTR [rbx+0x1c9],r12b
000EF757: je        0x1800ef760
000EF759: mov       BYTE PTR [rbx+0x1c9],r12b
000EF760: mov       r8,QWORD PTR [rdi+0x88]
000EF767: mov       r14,QWORD PTR [rsp+0x208]
000EF76F: test      r8,r8
000EF772: je        0x1800ef78e
000EF774: call      0x1800badc0
000EF779: test      al,al
000EF77B: jne       0x1800ef78e
000EF77D: lea       rdx,[rbp+0xa0]
000EF784: mov       rcx,rbx
000EF787: call      0x1800f0440
000EF78C: jmp       0x1800ef7a5
000EF78E: xorps     xmm0,xmm0
000EF791: lea       rax,[rsp+0x30]
000EF796: movups    XMMWORD PTR [rsp+0x30],xmm0
000EF79B: movups    XMMWORD PTR [rsp+0x40],xmm0
000EF7A0: movups    XMMWORD PTR [rsp+0x50],xmm0
000EF7A5: mov       ecx,DWORD PTR [rbx+0x8]
000EF7A8: cmp       ecx,0x1
000EF7AB: jne       0x1800ef7b7
000EF7AD: mov       QWORD PTR [rbp+0x50],0x30002
000EF7B5: jmp       0x1800ef7c4
000EF7B7: cmp       ecx,0x2
000EF7BA: jne       0x1800ef839
000EF7BC: mov       QWORD PTR [rbp+0x50],0x40002
000EF7C4: movups    xmm0,XMMWORD PTR [rax]
000EF7C7: cmp       BYTE PTR [rdi+0x9c],r12b
000EF7CE: mov       ecx,0x3
000EF7D3: movups    xmm1,XMMWORD PTR [rax+0x10]
000EF7D7: mov       DWORD PTR [rbp+0x94],r12d
000EF7DE: movaps    XMMWORD PTR [rbp+0x60],xmm0
000EF7E2: movups    xmm0,XMMWORD PTR [rax+0x20]
000EF7E6: mov       eax,0x2
000EF7EB: mov       QWORD PTR [rbp+0x58],r12
000EF7EF: cmovne    eax,ecx
000EF7F2: movaps    XMMWORD PTR [rbp+0x70],xmm1
000EF7F6: lea       rcx,[rbp+0x50]
000EF7FA: mov       DWORD PTR [rbp+0x90],eax
000EF800: movaps    XMMWORD PTR [rbp+0x80],xmm0
000EF807: call      0x1800f0b60
000EF80C: mov       rbx,QWORD PTR [rip+0x1126be5]        # 0x1812163f8
000EF813: mov       rdi,rax
000EF816: cmp       BYTE PTR [rbx+0xca],r12b
000EF81D: jne       0x1800ef827
000EF81F: mov       rcx,rbx
000EF822: call      0x1800cc8c0
000EF827: mov       r8,QWORD PTR [rbx]
000EF82A: lea       rcx,[rip+0x113018f]        # 0x18121f9c0
000EF831: mov       rdx,rdi
000EF834: call      r8
000EF837: mov       esi,eax
000EF839: mov       r12,QWORD PTR [rsp+0x250]
000EF841: test      esi,esi
000EF843: je        0x1800ef853
000EF845: mov       edx,esi
000EF847: lea       rcx,[rip+0x10ba3c2]        # 0x1811a9c10 ; "Couldn't set the ffxapi swapchain config: %d"
000EF84E: call      0x1800fbb40
000EF853: movaps    xmm6,XMMWORD PTR [rsp+0x1f0]
000EF85B: mov       rcx,QWORD PTR [rbp+0xd8]
000EF862: xor       rcx,rsp
000EF865: call      0x18010c270
000EF86A: add       rsp,0x218
000EF871: pop       rdi
000EF872: pop       rsi
000EF873: pop       rbx
000EF874: pop       rbp
000EF875: ret       
