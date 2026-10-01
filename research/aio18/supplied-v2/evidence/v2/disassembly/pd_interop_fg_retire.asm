; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0x77D60..0x77EAC; unnamed
00077D60: mov       rax,rsp
00077D63: mov       QWORD PTR [rax+0x8],rcx
00077D67: push      rbx
00077D68: push      rbp
00077D69: sub       rsp,0x68
00077D6D: mov       rbx,QWORD PTR [rip+0x119db04]        # 0x181215878
00077D74: mov       rbp,r8
00077D77: mov       QWORD PTR [rax+0x18],rdi
00077D7B: mov       rdi,rdx
00077D7E: cmp       QWORD PTR [rbx+0x428],rdx
00077D85: je        0x180077e5d
00077D8B: mov       QWORD PTR [rax+0x10],rsi
00077D8F: lea       rsi,[rbx+0x410]
00077D96: mov       QWORD PTR [rax+0x20],r12
00077D9A: mov       r12,QWORD PTR [rsi]
00077D9D: mov       QWORD PTR [rax-0x18],r13
00077DA1: mov       r13d,DWORD PTR [rbx+0x3ec]
00077DA8: mov       QWORD PTR [rax-0x20],r14
00077DAC: mov       r14,QWORD PTR [rbx+0x420]
00077DB3: mov       QWORD PTR [rax-0x28],r15
00077DB7: mov       eax,DWORD PTR [rbx+0x3f0]
00077DBD: mov       r15,QWORD PTR [rbx+0x418]
00077DC4: mov       DWORD PTR [rsp+0x80],eax
00077DCB: xor       eax,eax
00077DCD: test      rdx,rdx
00077DD0: je        0x180077ddb
00077DD2: mov       rax,QWORD PTR [rdx]
00077DD5: mov       rcx,rdx
00077DD8: call      QWORD PTR [rax+0x40]
00077DDB: mov       ecx,DWORD PTR [rsp+0x80]
00077DE2: mov       r9,rbp
00077DE5: mov       rdx,QWORD PTR [rbx+0x428]
00077DEC: mov       r8,rdi
00077DEF: mov       QWORD PTR [rsp+0x48],r14
00077DF4: mov       QWORD PTR [rsp+0x40],r15
00077DF9: mov       QWORD PTR [rsp+0x38],r12
00077DFE: mov       DWORD PTR [rsp+0x30],r13d
00077E03: mov       DWORD PTR [rsp+0x28],ecx
00077E07: lea       rcx,[rip+0x1127252]        # 0x18119f060 ; 'FGFence: replace=%p->%p last=%llu completed=%llu previousSlot=%d currentSlot=%d oldSlots=%llu,%llu,%llu'
00077E0E: mov       QWORD PTR [rsp+0x20],rax
00077E13: call      0x1800fbb40
00077E18: mov       r15,QWORD PTR [rsp+0x50]
00077E1D: lea       rax,[rsi+0x18]
00077E21: mov       r14,QWORD PTR [rsp+0x58]
00077E26: cmp       rsi,rax
00077E29: mov       r13,QWORD PTR [rsp+0x60]
00077E2E: mov       r8d,0x3
00077E34: mov       r12,QWORD PTR [rsp+0x98]
00077E3C: mov       ecx,0x0
00077E41: cmova     r8d,ecx
00077E45: ja        0x180077e55
00077E47: shl       r8,0x3
00077E4B: xor       edx,edx
00077E4D: mov       rcx,rsi
00077E50: call      0x18010d61a
00077E55: mov       rsi,QWORD PTR [rsp+0x88]
00077E5D: mov       ecx,DWORD PTR [rbx+0x3f0]
00077E63: mov       QWORD PTR [rbx+0x428],rdi
00077E6A: mov       rdi,QWORD PTR [rsp+0x90]
00077E72: test      ecx,ecx
00077E74: js        0x180077e99
00077E76: test      rbp,rbp
00077E79: je        0x180077e99
00077E7B: mov       eax,0x55555556
00077E80: imul      ecx
00077E82: mov       eax,edx
00077E84: shr       eax,0x1f
00077E87: add       edx,eax
00077E89: lea       eax,[rdx+rdx*2]
00077E8C: sub       ecx,eax
00077E8E: movsxd    rax,ecx
00077E91: mov       QWORD PTR [rbx+rax*8+0x410],rbp
00077E99: mov       eax,DWORD PTR [rbx+0x3ec]
00077E9F: mov       DWORD PTR [rbx+0x3f0],eax
00077EA5: add       rsp,0x68
00077EA9: pop       rbp
00077EAA: pop       rbx
00077EAB: ret       
