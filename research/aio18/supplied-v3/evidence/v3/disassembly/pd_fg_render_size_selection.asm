; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xEF230..0xEF430; unnamed
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
