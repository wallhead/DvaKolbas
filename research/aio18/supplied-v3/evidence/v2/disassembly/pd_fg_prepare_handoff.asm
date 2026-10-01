; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xEAD90..0xEB07D; unnamed
000EAD90: mov       QWORD PTR [rsp+0x8],rbx
000EAD95: mov       QWORD PTR [rsp+0x10],rbp
000EAD9A: mov       QWORD PTR [rsp+0x18],rsi
000EAD9F: push      rdi
000EADA0: push      r14
000EADA2: push      r15
000EADA4: sub       rsp,0xf0
000EADAB: mov       rdi,rdx
000EADAE: mov       r15,rcx
000EADB1: xor       esi,esi
000EADB3: mov       QWORD PTR [rsp+0x28],rsi
000EADB8: mov       DWORD PTR [rsp+0x20],0x2
000EADC0: mov       DWORD PTR [rsp+0x24],0xffffffff
000EADC8: mov       rbx,QWORD PTR [rip+0x112b619]        # 0x1812163e8
000EADCF: mov       rax,QWORD PTR [rbx+0xc98]
000EADD6: cmp       QWORD PTR [rbx+0xc90],rax
000EADDD: je        0x1800eae11
000EADDF: lea       r9,[rsp+0x20]
000EADE4: mov       r8,QWORD PTR [rbx+0x210]
000EADEB: mov       edx,0x1
000EADF0: mov       rcx,rbx
000EADF3: call      0x180075920
000EADF8: test      rax,rax
000EADFB: je        0x1800eae8e
000EAE01: mov       rcx,QWORD PTR [rip+0x112b5e0]        # 0x1812163e8
000EAE08: mov       QWORD PTR [rcx+0xc78],rax
000EAE0F: jmp       0x1800eae8e
000EAE11: add       rbx,0x938
000EAE18: mov       QWORD PTR [rsp+0x30],rbx
000EAE1D: mov       BYTE PTR [rsp+0x38],sil
000EAE22: je        0x1800eb072
000EAE28: mov       rcx,rbx
000EAE2B: call      QWORD PTR [rip+0x28707]        # 0x180113538 ; MSVCP140.dll!_Mtx_lock
000EAE31: test      eax,eax
000EAE33: je        0x1800eae41
000EAE35: mov       ecx,0x5
000EAE3A: call      QWORD PTR [rip+0x28700]        # 0x180113540 ; MSVCP140.dll!?_Throw_Cpp_error@std@@YAXH@Z
000EAE40: int3      
000EAE41: cmp       DWORD PTR [rbx+0x4c],0x7fffffff
000EAE48: jne       0x1800eae5d
000EAE4A: mov       DWORD PTR [rbx+0x4c],0x7ffffffe
000EAE51: mov       ecx,0x6
000EAE56: call      QWORD PTR [rip+0x286e4]        # 0x180113540 ; MSVCP140.dll!?_Throw_Cpp_error@std@@YAXH@Z
000EAE5C: int3      
000EAE5D: mov       BYTE PTR [rsp+0x38],0x1
000EAE62: mov       rdx,QWORD PTR [rip+0x112b57f]        # 0x1812163e8
000EAE69: mov       rcx,QWORD PTR [rdx+0xa8]
000EAE70: mov       rax,QWORD PTR [rcx]
000EAE73: mov       r8,QWORD PTR [rdx+0x210]
000EAE7A: mov       rdx,QWORD PTR [rdx+0x180]
000EAE81: call      QWORD PTR [rax+0x78]
000EAE84: nop       
000EAE85: mov       rcx,rbx
000EAE88: call      QWORD PTR [rip+0x2869a]        # 0x180113528 ; MSVCP140.dll!_Mtx_unlock
000EAE8E: mov       rax,QWORD PTR [rip+0x112b553]        # 0x1812163e8
000EAE95: movzx     ebp,BYTE PTR [rax+0x1f9]
000EAE9C: mov       eax,0xaaaaaaab
000EAEA1: mul       ebp
000EAEA3: shr       edx,1
000EAEA5: lea       eax,[rdx+rdx*2]
000EAEA8: sub       ebp,eax
000EAEAA: mov       edx,ebp
000EAEAC: call      0x180077940
000EAEB1: mov       edx,0x1
000EAEB6: mov       rcx,QWORD PTR [rip+0x112b52b]        # 0x1812163e8
000EAEBD: call      0x180073410
000EAEC2: mov       r14,rax
000EAEC5: test      rax,rax
000EAEC8: je        0x1800eb055
000EAECE: mov       rcx,QWORD PTR [rip+0x112b513]        # 0x1812163e8
000EAED5: mov       rdx,QWORD PTR [rcx+0xc98]
000EAEDC: cmp       QWORD PTR [rcx+0xc90],rdx
000EAEE3: je        0x1800eaef8
000EAEE5: lea       r9,[rsp+0x20]
000EAEEA: mov       r8d,0x1
000EAEF0: mov       rdx,rax
000EAEF3: call      0x180075fe0
000EAEF8: mov       rbx,QWORD PTR [rdi+0x8]
000EAEFC: test      rbx,rbx
000EAEFF: setne     r9b
000EAF03: mov       r8d,ebp
000EAF06: mov       rdx,r14
000EAF09: call      0x180077a50
000EAF0E: mov       r8,QWORD PTR [rip+0x112b4d3]        # 0x1812163e8
000EAF15: mov       DWORD PTR [r8+0x3e8],ebp
000EAF1C: mov       QWORD PTR [rdi+0x68],r14
000EAF20: test      rbx,rbx
000EAF23: je        0x1800eaf2e
000EAF25: mov       edx,ebp
000EAF27: call      0x180077d10
000EAF2C: jmp       0x1800eaf31
000EAF2E: mov       rax,rsi
000EAF31: mov       QWORD PTR [rdi+0x8],rax
000EAF35: cmp       QWORD PTR [rdi+0x10],0x0
000EAF3A: je        0x1800eaf48
000EAF3C: mov       edx,ebp
000EAF3E: mov       rcx,r8
000EAF41: call      0x180077cf0
000EAF46: jmp       0x1800eaf4b
000EAF48: mov       rax,rsi
000EAF4B: mov       QWORD PTR [rdi+0x10],rax
000EAF4F: cmp       QWORD PTR [rdi+0x18],0x0
000EAF54: je        0x1800eaf62
000EAF56: mov       edx,ebp
000EAF58: mov       rcx,r8
000EAF5B: call      0x180077cd0
000EAF60: jmp       0x1800eaf65
000EAF62: mov       rax,rsi
000EAF65: mov       QWORD PTR [rdi+0x18],rax
000EAF69: cmp       QWORD PTR [rdi+0x20],0x0
000EAF6E: je        0x1800eaf79
000EAF70: mov       rax,QWORD PTR [r8+0x478]
000EAF77: jmp       0x1800eaf7c
000EAF79: mov       rax,rsi
000EAF7C: mov       QWORD PTR [rdi+0x20],rax
000EAF80: mov       QWORD PTR [rdi+0x28],rsi
000EAF84: mov       QWORD PTR [rdi+0x30],rsi
000EAF88: cmp       QWORD PTR [rdi+0x88],0x0
000EAF90: je        0x1800eaf9b
000EAF92: mov       rax,QWORD PTR [r8+0x480]
000EAF99: jmp       0x1800eaf9e
000EAF9B: mov       rax,rsi
000EAF9E: mov       QWORD PTR [rdi+0x88],rax
000EAFA5: cmp       QWORD PTR [rdi+0x90],0x0
000EAFAD: je        0x1800eafb6
000EAFAF: mov       rsi,QWORD PTR [r8+0x488]
000EAFB6: mov       QWORD PTR [rdi+0x90],rsi
000EAFBD: mov       rcx,QWORD PTR [r15+0x148]
000EAFC4: test      rcx,rcx
000EAFC7: je        0x1800eb048
000EAFC9: lea       rax,[rsp+0x40]
000EAFCE: movups    xmm0,XMMWORD PTR [rdi]
000EAFD1: movups    XMMWORD PTR [rax],xmm0
000EAFD4: movups    xmm1,XMMWORD PTR [rdi+0x10]
000EAFD8: movups    XMMWORD PTR [rax+0x10],xmm1
000EAFDC: movups    xmm0,XMMWORD PTR [rdi+0x20]
000EAFE0: movups    XMMWORD PTR [rax+0x20],xmm0
000EAFE4: movups    xmm1,XMMWORD PTR [rdi+0x30]
000EAFE8: movups    XMMWORD PTR [rax+0x30],xmm1
000EAFEC: movups    xmm0,XMMWORD PTR [rdi+0x40]
000EAFF0: movups    XMMWORD PTR [rax+0x40],xmm0
000EAFF4: movups    xmm1,XMMWORD PTR [rdi+0x50]
000EAFF8: movups    XMMWORD PTR [rax+0x50],xmm1
000EAFFC: movups    xmm0,XMMWORD PTR [rdi+0x60]
000EB000: movups    XMMWORD PTR [rax+0x60],xmm0
000EB004: movups    xmm0,XMMWORD PTR [rdi+0x70]
000EB008: movups    XMMWORD PTR [rax+0x70],xmm0
000EB00C: movups    xmm1,XMMWORD PTR [rdi+0x80]
000EB013: movups    XMMWORD PTR [rax+0x80],xmm1
000EB01A: movups    xmm0,XMMWORD PTR [rdi+0x90]
000EB021: movups    XMMWORD PTR [rax+0x90],xmm0
000EB028: movups    xmm1,XMMWORD PTR [rdi+0xa0]
000EB02F: movups    XMMWORD PTR [rax+0xa0],xmm1
000EB036: mov       rax,QWORD PTR [rcx]
000EB039: lea       rdx,[rsp+0x40]
000EB03E: call      QWORD PTR [rax+0x8]
000EB041: mov       r8,QWORD PTR [rip+0x112b3a0]        # 0x1812163e8
000EB048: mov       edx,0x1
000EB04D: mov       rcx,r8
000EB050: call      0x180073660
000EB055: lea       r11,[rsp+0xf0]
000EB05D: mov       rbx,QWORD PTR [r11+0x20]
000EB061: mov       rbp,QWORD PTR [r11+0x28]
000EB065: mov       rsi,QWORD PTR [r11+0x30]
000EB069: mov       rsp,r11
000EB06C: pop       r15
000EB06E: pop       r14
000EB070: pop       rdi
000EB071: ret       
000EB072: mov       ecx,0x1
000EB077: call      0x18006cfd0
000EB07C: int3      
