; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xEEF20..0xEF166; unnamed
000EEF20: rex       push rdi
000EEF22: sub       rsp,0xd0
000EEF29: mov       rax,QWORD PTR [rip+0x10dba90]        # 0x1811ca9c0
000EEF30: xor       rax,rsp
000EEF33: mov       QWORD PTR [rsp+0xc0],rax
000EEF3B: mov       rdi,rcx
000EEF3E: mov       BYTE PTR [rcx+0x1c8],0x0
000EEF45: call      0x1800cc790
000EEF4A: mov       r8d,DWORD PTR [rdi+0x8]
000EEF4E: mov       rcx,rax
000EEF51: mov       edx,DWORD PTR [rdi+0x148]
000EEF57: call      0x1800ccc80
000EEF5C: cmp       QWORD PTR [rip+0x1130a64],0x0        # 0x18121f9c8
000EEF64: jne       0x1800eef74
000EEF66: cmp       QWORD PTR [rip+0x1130a52],0x0        # 0x18121f9c0
000EEF6E: je        0x1800ef14d
000EEF74: lea       rcx,[rip+0x10bac45]        # 0x1811a9bc0 ; 'FSR3 FG Feature gets released!'
000EEF7B: mov       QWORD PTR [rsp+0xf0],rbp
000EEF83: call      0x1800fbb40
000EEF88: mov       rcx,QWORD PTR [rip+0x1127471]        # 0x181216400
000EEF8F: call      0x18006fcc0
000EEF94: xor       ebp,ebp
000EEF96: test      al,al
000EEF98: jne       0x1800ef130
000EEF9E: cmp       QWORD PTR [rip+0x1130a23],rbp        # 0x18121f9c8
000EEFA5: xorps     xmm0,xmm0
000EEFA8: mov       QWORD PTR [rsp+0xe8],rbx
000EEFB0: movaps    XMMWORD PTR [rip+0x110ae59],xmm0        # 0x1811f9e10
000EEFB7: movaps    XMMWORD PTR [rip+0x110ae62],xmm0        # 0x1811f9e20
000EEFBE: movaps    XMMWORD PTR [rip+0x110ae6b],xmm0        # 0x1811f9e30
000EEFC5: mov       QWORD PTR [rsp+0xf8],rsi
000EEFCD: mov       BYTE PTR [rip+0x110ae34],bpl        # 0x1811f9e08
000EEFD4: mov       QWORD PTR [rip+0x110ae0d],rbp        # 0x1811f9de8
000EEFDB: je        0x1800ef014
000EEFDD: lea       rcx,[rip+0x110adec]        # 0x1811f9dd0
000EEFE4: call      0x1800f0b60
000EEFE9: mov       rbx,QWORD PTR [rip+0x1127408]        # 0x1812163f8
000EEFF0: mov       rsi,rax
000EEFF3: cmp       BYTE PTR [rbx+0xca],bpl
000EEFFA: jne       0x1800ef004
000EEFFC: mov       rcx,rbx
000EEFFF: call      0x1800cc8c0
000EF004: mov       r8,QWORD PTR [rbx]
000EF007: lea       rcx,[rip+0x11309ba]        # 0x18121f9c8
000EF00E: mov       rdx,rsi
000EF011: call      r8
000EF014: cmp       QWORD PTR [rip+0x11309a5],rbp        # 0x18121f9c0
000EF01B: je        0x1800ef0c0
000EF021: mov       eax,DWORD PTR [rdi+0x8]
000EF024: cmp       eax,0x1
000EF027: jne       0x1800ef055
000EF029: xorps     xmm0,xmm0
000EF02C: mov       QWORD PTR [rsp+0x60],rbp
000EF031: movaps    XMMWORD PTR [rsp+0x30],xmm0
000EF036: lea       rcx,[rsp+0x20]
000EF03B: movaps    XMMWORD PTR [rsp+0x40],xmm0
000EF040: movaps    XMMWORD PTR [rsp+0x50],xmm0
000EF045: mov       QWORD PTR [rsp+0x28],rbp
000EF04A: mov       QWORD PTR [rsp+0x20],0x30002
000EF053: jmp       0x1800ef090
000EF055: cmp       eax,0x2
000EF058: jne       0x1800ef0c0
000EF05A: xorps     xmm0,xmm0
000EF05D: mov       QWORD PTR [rsp+0xb0],rbp
000EF065: movaps    XMMWORD PTR [rsp+0x80],xmm0
000EF06D: lea       rcx,[rsp+0x70]
000EF072: movaps    XMMWORD PTR [rsp+0x90],xmm0
000EF07A: movaps    XMMWORD PTR [rsp+0xa0],xmm0
000EF082: mov       QWORD PTR [rsp+0x78],rbp
000EF087: mov       QWORD PTR [rsp+0x70],0x40002
000EF090: call      0x1800f0b60
000EF095: mov       rbx,QWORD PTR [rip+0x112735c]        # 0x1812163f8
000EF09C: mov       rsi,rax
000EF09F: cmp       BYTE PTR [rbx+0xca],bpl
000EF0A6: jne       0x1800ef0b0
000EF0A8: mov       rcx,rbx
000EF0AB: call      0x1800cc8c0
000EF0B0: mov       r8,QWORD PTR [rbx]
000EF0B3: lea       rcx,[rip+0x1130906]        # 0x18121f9c0
000EF0BA: mov       rdx,rsi
000EF0BD: call      r8
000EF0C0: cmp       QWORD PTR [rip+0x1130901],rbp        # 0x18121f9c8
000EF0C7: mov       rsi,QWORD PTR [rsp+0xf8]
000EF0CF: je        0x1800ef0f8
000EF0D1: mov       rbx,QWORD PTR [rip+0x1127320]        # 0x1812163f8
000EF0D8: cmp       BYTE PTR [rbx+0xca],bpl
000EF0DF: jne       0x1800ef0e9
000EF0E1: mov       rcx,rbx
000EF0E4: call      0x1800cc8c0
000EF0E9: mov       rax,QWORD PTR [rbx+0x10]
000EF0ED: lea       rcx,[rip+0x11308d4]        # 0x18121f9c8
000EF0F4: xor       edx,edx
000EF0F6: call      rax
000EF0F8: cmp       QWORD PTR [rip+0x11308c1],rbp        # 0x18121f9c0
000EF0FF: je        0x1800ef128
000EF101: mov       rbx,QWORD PTR [rip+0x11272f0]        # 0x1812163f8
000EF108: cmp       BYTE PTR [rbx+0xca],bpl
000EF10F: jne       0x1800ef119
000EF111: mov       rcx,rbx
000EF114: call      0x1800cc8c0
000EF119: mov       rax,QWORD PTR [rbx+0x10]
000EF11D: lea       rcx,[rip+0x113089c]        # 0x18121f9c0
000EF124: xor       edx,edx
000EF126: call      rax
000EF128: mov       rbx,QWORD PTR [rsp+0xe8]
000EF130: mov       QWORD PTR [rip+0x1130891],rbp        # 0x18121f9c8
000EF137: mov       QWORD PTR [rip+0x1130882],rbp        # 0x18121f9c0
000EF13E: mov       QWORD PTR [rdi+0x1a8],rbp
000EF145: mov       rbp,QWORD PTR [rsp+0xf0]
000EF14D: mov       rcx,QWORD PTR [rsp+0xc0]
000EF155: xor       rcx,rsp
000EF158: call      0x18010c270
000EF15D: add       rsp,0xd0
000EF164: pop       rdi
000EF165: ret       
