; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0x100620..0x1008F8; unnamed
00100620: mov       QWORD PTR [rsp+0x10],rbx
00100625: mov       QWORD PTR [rsp+0x18],rsi
0010062A: push      rbp
0010062B: push      rdi
0010062C: push      r14
0010062E: lea       rbp,[rsp-0x47]
00100633: sub       rsp,0xd0
0010063A: mov       rdi,rdx
0010063D: mov       rbx,rcx
00100640: call      0x1800cc790
00100645: mov       r8d,DWORD PTR [rbx+0x8]
00100649: mov       edx,DWORD PTR [rbx+0x110]
0010064F: mov       rcx,rax
00100652: call      0x1800ccc80
00100657: mov       edx,DWORD PTR [rdi]
00100659: call      0x1801009a0
0010065E: test      al,al
00100660: je        0x18010066c
00100662: mov       edx,DWORD PTR [rdi]
00100664: mov       rcx,rbx
00100667: call      0x1800ceea0
0010066C: movzx     eax,BYTE PTR [rdi+0x1c]
00100670: mov       BYTE PTR [rbx+0x13c],al
00100676: xor       r14d,r14d
00100679: mov       DWORD PTR [rbp-0x31],r14d
0010067D: mov       DWORD PTR [rbp-0x1d],r14d
00100681: mov       QWORD PTR [rbp-0x39],r14
00100685: mov       QWORD PTR [rbp-0x41],0x10000
0010068D: mov       eax,DWORD PTR [rdi+0xc]
00100690: mov       ecx,DWORD PTR [rdi+0x10]
00100693: mov       DWORD PTR [rbp-0x25],eax
00100696: mov       DWORD PTR [rbp-0x21],ecx
00100699: mov       DWORD PTR [rbp-0x2d],eax
0010069C: mov       DWORD PTR [rbp-0x29],ecx
0010069F: mov       eax,0x220
001006A4: mov       ecx,0x228
001006A9: cmp       BYTE PTR [rdi+0x19],r14b
001006AD: cmovne    eax,ecx
001006B0: cmp       BYTE PTR [rdi+0x18],r14b
001006B4: je        0x1801006b9
001006B6: or        eax,0x1
001006B9: cmp       BYTE PTR [rdi+0x25],r14b
001006BD: je        0x1801006c2
001006BF: or        eax,0x2
001006C2: bts       eax,0x7
001006C6: mov       DWORD PTR [rbp-0x31],eax
001006C9: lea       rax,[rip+0xfffffffffffff850]        # 0x1800fff20
001006D0: mov       QWORD PTR [rbp-0x19],rax
001006D4: mov       QWORD PTR [rbp-0x1],0x1000003
001006DC: mov       QWORD PTR [rbp-0x9],r14
001006E0: mov       QWORD PTR [rbp-0x11],0x1000b
001006E8: mov       esi,0x1
001006ED: mov       eax,DWORD PTR [rbx+0x8]
001006F0: test      eax,eax
001006F2: jne       0x18010070d
001006F4: mov       QWORD PTR [rbp-0x69],0x2
001006FC: mov       rax,QWORD PTR [rbx+0x118]
00100703: mov       QWORD PTR [rbp-0x59],rax
00100707: lea       rdx,[rbp-0x69]
0010070B: jmp       0x18010075c
0010070D: cmp       eax,esi
0010070F: jne       0x18010072a
00100711: mov       QWORD PTR [rbp-0x69],0x2
00100719: mov       rax,QWORD PTR [rbx+0x120]
00100720: mov       QWORD PTR [rbp-0x59],rax
00100724: lea       rdx,[rbp-0x69]
00100728: jmp       0x18010075c
0010072A: cmp       eax,0x2
0010072D: jne       0x18010079b
0010072F: mov       QWORD PTR [rbp-0x69],0x3
00100737: mov       rax,QWORD PTR [rbx+0x128]
0010073E: mov       QWORD PTR [rbp-0x59],rax
00100742: mov       rax,QWORD PTR [rbx+0x130]
00100749: mov       QWORD PTR [rbp-0x51],rax
0010074D: mov       rax,QWORD PTR [rip+0x1102f84]        # 0x1812036d8 ; vulkan-1.dll!vkGetDeviceProcAddr
00100754: mov       QWORD PTR [rbp-0x49],rax
00100758: lea       rdx,[rbp-0x69]
0010075C: mov       QWORD PTR [rbp-0x61],r14
00100760: lea       r8,[rbp-0x11]
00100764: lea       rcx,[rbp-0x41]
00100768: call      0x1800f0b30
0010076D: mov       rbx,QWORD PTR [rip+0x1115cec]        # 0x181216460
00100774: mov       rsi,rax
00100777: cmp       BYTE PTR [rbx+0xca],r14b
0010077E: jne       0x180100788
00100780: mov       rcx,rbx
00100783: call      0x1800cc8c0
00100788: xor       r8d,r8d
0010078B: mov       rdx,rsi
0010078E: lea       rcx,[rbp+0x67]
00100792: mov       r9,QWORD PTR [rbx+0x8]
00100796: call      r9
00100799: mov       esi,eax
0010079B: mov       rbx,QWORD PTR [rbp+0x67]
0010079F: mov       rdx,rdi
001007A2: call      0x180100fb0
001007A7: mov       QWORD PTR [rax],rbx
001007AA: test      esi,esi
001007AC: je        0x1801007c3
001007AE: mov       edx,esi
001007B0: lea       rcx,[rip+0x10aba41]        # 0x1811ac1f8 ; 'ffx::CreateContext for upscaling failed! ErrorCode: %d'
001007B7: call      0x1800fbb40
001007BC: xor       al,al
001007BE: jmp       0x1801008da
001007C3: movups    xmm0,XMMWORD PTR [rbp-0x41]
001007C7: movups    XMMWORD PTR [rbp+0x17],xmm0
001007CB: movups    xmm1,XMMWORD PTR [rbp-0x31]
001007CF: movups    XMMWORD PTR [rbp+0x27],xmm1
001007D3: movups    xmm2,XMMWORD PTR [rbp-0x21]
001007D7: movups    XMMWORD PTR [rbp+0x37],xmm2
001007DB: mov       rbx,QWORD PTR [rip+0x1115c86]        # 0x181216468
001007E2: mov       rax,QWORD PTR [rbx+0x8]
001007E6: mov       QWORD PTR [rbp-0x69],rax
001007EA: mov       DWORD PTR [rbp-0x61],r14d
001007EE: mov       rcx,rbx
001007F1: cmp       BYTE PTR [rax+0x19],r14b
001007F5: jne       0x180100826
001007F7: mov       edx,DWORD PTR [rdi]
001007F9: nop       DWORD PTR [rax+0x0]
00100800: mov       QWORD PTR [rbp-0x69],rax
00100804: cmp       DWORD PTR [rax+0x20],edx
00100807: jge       0x180100813
00100809: mov       DWORD PTR [rbp-0x61],r14d
0010080D: add       rax,0x10
00100811: jmp       0x18010081d
00100813: mov       DWORD PTR [rbp-0x61],0x1
0010081A: mov       rcx,rax
0010081D: mov       rax,QWORD PTR [rax]
00100820: cmp       BYTE PTR [rax+0x19],r14b
00100824: je        0x180100800
00100826: cmp       BYTE PTR [rcx+0x19],r14b
0010082A: jne       0x180100837
0010082C: mov       eax,DWORD PTR [rcx+0x20]
0010082F: cmp       DWORD PTR [rdi],eax
00100831: jge       0x1801008c0
00100837: movabs    rax,0x2e8ba2e8ba2e8ba
00100841: cmp       QWORD PTR [rip+0x1115c28],rax        # 0x181216470
00100848: je        0x1801008f2
0010084E: lea       rsi,[rip+0x1115c13]        # 0x181216468
00100855: mov       QWORD PTR [rbp+0x7],rsi
00100859: mov       QWORD PTR [rbp+0xf],r14
0010085D: mov       ecx,0x58
00100862: call      0x18010c290
00100867: nop       
00100868: mov       ecx,DWORD PTR [rdi]
0010086A: mov       DWORD PTR [rax+0x20],ecx
0010086D: mov       QWORD PTR [rax+0x38],r14
00100871: mov       QWORD PTR [rax+0x40],r14
00100875: mov       QWORD PTR [rax+0x48],r14
00100879: mov       QWORD PTR [rax+0x50],r14
0010087D: mov       QWORD PTR [rax+0x30],r14
00100881: mov       QWORD PTR [rax+0x28],0x10000
00100889: mov       QWORD PTR [rax],rbx
0010088C: mov       QWORD PTR [rax+0x8],rbx
00100890: mov       QWORD PTR [rax+0x10],rbx
00100894: mov       WORD PTR [rax+0x18],0x0
0010089A: movups    xmm0,XMMWORD PTR [rbp-0x69]
0010089E: movaps    XMMWORD PTR [rbp+0x7],xmm0
001008A2: mov       r8,rax
001008A5: lea       rdx,[rbp+0x7]
001008A9: mov       rcx,rsi
001008AC: call      0x180059bb0
001008B1: mov       rcx,rax
001008B4: movups    xmm0,XMMWORD PTR [rbp+0x17]
001008B8: movups    xmm1,XMMWORD PTR [rbp+0x27]
001008BC: movups    xmm2,XMMWORD PTR [rbp+0x37]
001008C0: movups    XMMWORD PTR [rcx+0x28],xmm0
001008C4: movups    XMMWORD PTR [rcx+0x38],xmm1
001008C8: movups    XMMWORD PTR [rcx+0x48],xmm2
001008CC: lea       rcx,[rip+0x10ab9cd]        # 0x1811ac2a0 ; 'ffx::CreateContext Success!'
001008D3: call      0x1800fbb40
001008D8: mov       al,0x1
001008DA: lea       r11,[rsp+0xd0]
001008E2: mov       rbx,QWORD PTR [r11+0x28]
001008E6: mov       rsi,QWORD PTR [r11+0x30]
001008EA: mov       rsp,r11
001008ED: pop       r14
001008EF: pop       rdi
001008F0: pop       rbp
001008F1: ret       
001008F2: call      0x18002aab0
001008F7: int3      
