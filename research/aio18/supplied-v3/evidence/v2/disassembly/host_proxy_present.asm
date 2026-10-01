; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x25F540..0x25F6A2; unnamed
0025F540: mov       DWORD PTR [rsp+0x18],r8d
0025F545: mov       DWORD PTR [rsp+0x10],edx
0025F549: push      rbx
0025F54A: push      rbp
0025F54B: push      rsi
0025F54C: push      rdi
0025F54D: push      r14
0025F54F: sub       rsp,0x60
0025F553: mov       r14,rcx
0025F556: movzx     eax,BYTE PTR [rcx+0x32]
0025F55A: test      al,al
0025F55C: je        0x18025f569
0025F55E: test      r8b,0x1
0025F562: jne       0x18025f569
0025F564: mov       rbp,rcx
0025F567: jmp       0x18025f56b
0025F569: xor       ebp,ebp
0025F56B: mov       QWORD PTR [rsp+0x90],rbp
0025F573: test      al,al
0025F575: je        0x18025f67c
0025F57B: test      r8b,0x1
0025F57F: jne       0x18025f67c
0025F585: vpxor     xmm0,xmm0,xmm0
0025F589: vmovups   YMMWORD PTR [rsp+0x40],ymm0
0025F58F: cmp       BYTE PTR [rcx+0x31],0x0
0025F593: je        0x18025f638
0025F599: cmp       BYTE PTR [rcx+0x30],0x0
0025F59D: je        0x18025f5bf
0025F59F: mov       rsi,QWORD PTR [rcx+0x250]
0025F5A6: mov       rax,QWORD PTR [rsi]
0025F5A9: mov       rdi,QWORD PTR [rax+0x178]
0025F5B0: mov       rbx,QWORD PTR [rcx+0x148]
0025F5B7: mov       rax,QWORD PTR [rcx]
0025F5BA: vzeroupper 
0025F5BD: jmp       0x18025f5ea
0025F5BF: mov       rax,QWORD PTR [rcx]
0025F5C2: vzeroupper 
0025F5C5: call      QWORD PTR [rax+0x120]
0025F5CB: test      eax,eax
0025F5CD: je        0x18025f603
0025F5CF: mov       rsi,QWORD PTR [r14+0x250]
0025F5D6: mov       rax,QWORD PTR [rsi]
0025F5D9: mov       rdi,QWORD PTR [rax+0x178]
0025F5E0: mov       rbx,QWORD PTR [r14+0x40]
0025F5E4: mov       rax,QWORD PTR [r14]
0025F5E7: mov       rcx,r14
0025F5EA: call      QWORD PTR [rax+0x120]
0025F5F0: mov       eax,eax
0025F5F2: imul      rdx,rax,0x58
0025F5F6: mov       r8,rbx
0025F5F9: mov       rdx,QWORD PTR [rdx+r14*1+0x40]
0025F5FE: mov       rcx,rsi
0025F601: call      rdi
0025F603: mov       QWORD PTR [rsp+0x20],r14
0025F608: lea       rax,[rsp+0x98]
0025F610: mov       QWORD PTR [rsp+0x28],rax
0025F615: lea       rax,[rsp+0xa0]
0025F61D: mov       QWORD PTR [rsp+0x30],rax
0025F622: lea       rax,[rsp+0x40]
0025F627: mov       QWORD PTR [rsp+0x38],rax
0025F62C: lea       rcx,[rsp+0x20]
0025F631: call      0x18025f460
0025F636: jmp       0x18025f689
0025F638: vzeroupper 
0025F63B: call      0x1802aa8a0
0025F640: mov       QWORD PTR [rsp+0x20],r14
0025F645: lea       rax,[rsp+0x98]
0025F64D: mov       QWORD PTR [rsp+0x28],rax
0025F652: lea       rax,[rsp+0xa0]
0025F65A: mov       QWORD PTR [rsp+0x30],rax
0025F65F: lea       rax,[rsp+0x40]
0025F664: mov       QWORD PTR [rsp+0x38],rax
0025F669: lea       rcx,[rsp+0x20]
0025F66E: call      0x18025f370
0025F673: mov       ebx,eax
0025F675: call      0x1802ac050
0025F67A: jmp       0x18025f68b
0025F67C: mov       rcx,QWORD PTR [rcx+0x10]
0025F680: mov       rax,QWORD PTR [rcx]
0025F683: call      QWORD PTR [rax+0xb0]
0025F689: mov       ebx,eax
0025F68B: test      rbp,rbp
0025F68E: je        0x18025f695
0025F690: call      0x1802648f0
0025F695: mov       eax,ebx
0025F697: add       rsp,0x60
0025F69B: pop       r14
0025F69D: pop       rdi
0025F69E: pop       rsi
0025F69F: pop       rbp
0025F6A0: pop       rbx
0025F6A1: ret       
