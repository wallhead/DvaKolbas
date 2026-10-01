; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0x7F3F0..0x7F5B0; unnamed
0007F3F2: mov       BYTE PTR [rsp+0x70],0x0
0007F3F7: test      bl,bl
0007F3F9: mov       eax,0x1
0007F3FE: jne       0x18007f406
0007F400: mov       eax,DWORD PTR [rdi+0x478]
0007F406: mov       DWORD PTR [rdi+0x254],eax
0007F40C: or        DWORD PTR [rdi+0x258],0x8
0007F413: movss     xmm0,DWORD PTR [rdi+0x12c]
0007F41B: comiss    xmm0,DWORD PTR [rip+0x112d756]        # 0x1811acb78
0007F422: ja        0x18007f427
0007F424: xorps     xmm0,xmm0
0007F427: movss     DWORD PTR [rdi+0x2a4],xmm0
0007F42F: call      0x1800badc0
0007F434: test      al,al
0007F436: je        0x18007f43c
0007F438: xor       eax,eax
0007F43A: jmp       0x18007f46b
0007F43C: cmp       BYTE PTR [rdi+0x144],0x0
0007F443: je        0x18007f469
0007F445: cmp       BYTE PTR [rdi+rsi*1+0xee8],0x0
0007F44D: je        0x18007f469
0007F44F: cmp       QWORD PTR [rdi+rsi*8+0xea0],0x0
0007F458: je        0x18007f469
0007F45A: cmp       QWORD PTR [rdi+rsi*8+0xeb8],0x0
0007F463: je        0x18007f469
0007F465: mov       al,0x1
0007F467: jmp       0x18007f46b
0007F469: xor       al,al
0007F46B: mov       r12,rdi
0007F46E: mov       esi,0x144
0007F473: mov       BYTE PTR [rdi+0x2a0],al
0007F479: movzx     ebx,BYTE PTR [rdi+0x2f8]
0007F480: lea       r14,[rdi+0x230]
0007F487: lea       rdx,[rdi+0x2a8]
0007F48E: lea       rcx,[rdi+0x340]
0007F495: mov       rax,QWORD PTR [rdi+0x438]
0007F49C: mov       r8,r14
0007F49F: call      rax
0007F4A1: mov       ecx,eax
0007F4A3: lea       rdx,[rip+0x111ff36]        # 0x18119f3e0 ; 'slDLSSGGetState'
0007F4AA: call      0x18007bae0
0007F4AF: mov       rax,QWORD PTR [rdi+0x2c8]
0007F4B6: mov       QWORD PTR [rbp+0x260],rax
0007F4BD: mov       eax,DWORD PTR [rdi+0x2d8]
0007F4C3: mov       DWORD PTR [rbp+0x268],eax
0007F4C9: mov       eax,DWORD PTR [rdi+0x2d4]
0007F4CF: mov       DWORD PTR [rbp+0x270],eax
0007F4D5: mov       eax,DWORD PTR [rdi+0x2dc]
0007F4DB: mov       DWORD PTR [rsp+0x78],eax
0007F4DF: mov       DWORD PTR [rbp+0x274],eax
0007F4E5: movzx     edx,BYTE PTR [rdi+0x2f8]
0007F4EC: mov       BYTE PTR [rsp+0x71],dl
0007F4F0: cmp       dl,0x1
0007F4F3: sete      cl
0007F4F6: mov       BYTE PTR [rbp+0x278],cl
0007F4FC: cmp       BYTE PTR [rdi+0x2e1],0x1
0007F503: sete      BYTE PTR [rbp+0x279]
0007F50A: cmp       bl,0x1
0007F50D: sete      al
0007F510: cmp       al,cl
0007F512: je        0x18007f52a
0007F514: cmp       dl,0x1
0007F517: jne       0x18007f52a
0007F519: lea       rcx,[rip+0x1120c20]        # 0x1811a0140 ; 'DLSS-G Dynamic Multi Frame Generation is supported!'
0007F520: call      0x1800fbb40
0007F525: movzx     edx,BYTE PTR [rsp+0x71]
0007F52A: cmp       BYTE PTR [r12+rsi*1],0x0
0007F52F: je        0x18007f566
0007F531: cmp       BYTE PTR [rdi+0x145],0x0
0007F538: jne       0x18007f566
0007F53A: cmp       dl,0x1
0007F53D: jne       0x18007f55f
0007F53F: cmp       BYTE PTR [rdi+0x128],0x0
0007F546: je        0x18007f55f
0007F548: cmp       BYTE PTR [rsp+0x72],0x0
0007F54D: je        0x18007f556
0007F54F: cmp       BYTE PTR [rsp+0x70],0x0
0007F554: je        0x18007f55f
0007F556: mov       DWORD PTR [r15],0x3
0007F55D: jmp       0x18007f566
0007F55F: mov       DWORD PTR [r15],0x1
0007F566: mov       rdx,QWORD PTR [rdi+0x2e8]
0007F56D: test      rdx,rdx
0007F570: je        0x18007f57e
0007F572: mov       r8,QWORD PTR [rdi+0x2f0]
0007F579: call      0x180077d60
0007F57E: mov       edx,DWORD PTR [rdi+0x2d0]
0007F584: test      edx,edx
0007F586: je        0x18007f594
0007F588: lea       rcx,[rip+0x1120be9]        # 0x1811a0178 ; 'DLSS-G Status error 0x%x'
0007F58F: call      0x1800fbb40
0007F594: mov       eax,DWORD PTR [rsp+0x78]
0007F598: mov       DWORD PTR [rdi+0x47c],eax
0007F59E: movups    xmm0,XMMWORD PTR [r14]
0007F5A2: movaps    XMMWORD PTR [rbp+0x430],xmm0
0007F5A9: movups    xmm1,XMMWORD PTR [r14+0x10]
0007F5AE: movaps    XMMWORD PTR [rbp+0x440],xmm1
