; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0x77940..0x77A46; unnamed
00077940: test      edx,edx
00077942: js        0x180077a3a
00077948: mov       QWORD PTR [rsp+0x8],rbx
0007794D: mov       QWORD PTR [rsp+0x10],rsi
00077952: push      rdi
00077953: sub       rsp,0x30
00077957: mov       r8d,edx
0007795A: mov       rbx,QWORD PTR [rip+0x119ea87]        # 0x1812163e8
00077961: mov       eax,0xaaaaaaab
00077966: mul       r8d
00077969: shr       edx,1
0007796B: lea       eax,[rdx+rdx*2]
0007796E: sub       r8d,eax
00077971: movsxd    rsi,r8d
00077974: mov       r8,QWORD PTR [rbx+rsi*8+0x3f8]
0007797C: test      r8,r8
0007797F: je        0x18007798e
00077981: mov       edx,0x2
00077986: mov       rcx,rbx
00077989: call      0x180073910
0007798E: cmp       QWORD PTR [rbx+0x428],0x0
00077996: je        0x180077a2b
0007799C: cmp       QWORD PTR [rbx+rsi*8+0x410],0x0
000779A5: jbe       0x180077a2b
000779AB: cmp       QWORD PTR [rbx+0xa8],0x0
000779B3: je        0x180077a2b
000779B5: lea       rdi,[rbx+0x938]
000779BC: mov       QWORD PTR [rsp+0x20],rdi
000779C1: mov       BYTE PTR [rsp+0x28],0x0
000779C6: test      rdi,rdi
000779C9: je        0x180077a3b
000779CB: mov       rcx,rdi
000779CE: call      QWORD PTR [rip+0x9bb64]        # 0x180113538 ; MSVCP140.dll!_Mtx_lock
000779D4: test      eax,eax
000779D6: je        0x1800779e4
000779D8: mov       ecx,0x5
000779DD: call      QWORD PTR [rip+0x9bb5d]        # 0x180113540 ; MSVCP140.dll!?_Throw_Cpp_error@std@@YAXH@Z
000779E3: int3      
000779E4: cmp       DWORD PTR [rdi+0x4c],0x7fffffff
000779EB: jne       0x180077a00
000779ED: mov       DWORD PTR [rdi+0x4c],0x7ffffffe
000779F4: mov       ecx,0x6
000779F9: call      QWORD PTR [rip+0x9bb41]        # 0x180113540 ; MSVCP140.dll!?_Throw_Cpp_error@std@@YAXH@Z
000779FF: int3      
00077A00: mov       BYTE PTR [rsp+0x28],0x1
00077A05: mov       rcx,QWORD PTR [rbx+0xa8]
00077A0C: mov       rax,QWORD PTR [rcx]
00077A0F: mov       r8,QWORD PTR [rbx+rsi*8+0x410]
00077A17: mov       rdx,QWORD PTR [rbx+0x428]
00077A1E: call      QWORD PTR [rax+0x78]
00077A21: nop       
00077A22: mov       rcx,rdi
00077A25: call      QWORD PTR [rip+0x9bafd]        # 0x180113528 ; MSVCP140.dll!_Mtx_unlock
00077A2B: mov       rbx,QWORD PTR [rsp+0x40]
00077A30: mov       rsi,QWORD PTR [rsp+0x48]
00077A35: add       rsp,0x30
00077A39: pop       rdi
00077A3A: ret       
00077A3B: mov       ecx,0x1
00077A40: call      0x18006cfd0
00077A45: int3      
