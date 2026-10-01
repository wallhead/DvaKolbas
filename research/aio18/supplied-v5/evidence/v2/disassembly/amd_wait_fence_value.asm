; amd_fidelityfx_framegeneration_dx12.dll SHA256=3f5e674a59b400756e98ef31fd583a4eb08ad567ec8dee362886bc96e55ed347
; ImageBase=0x180000000, RVAs in left column
; range 0xF4C30..0xF4E00; waitForFenceValue
000F4C30: mov       r11,rsp
000F4C33: push      rbx
000F4C34: push      rsi
000F4C35: push      rdi
000F4C36: sub       rsp,0xf0
000F4C3D: mov       rax,QWORD PTR [rip+0x23793bc]        # 0x18246e000 ; __security_cookie
000F4C44: xor       rax,rsp
000F4C47: mov       QWORD PTR [rsp+0xc0],rax
000F4C4F: mov       rdi,r9
000F4C52: mov       rbx,rdx
000F4C55: mov       rsi,rcx
000F4C58: test      rcx,rcx
000F4C5B: je        0x1800f4df9
000F4C61: mov       QWORD PTR [r11-0x38],r15
000F4C65: lea       rcx,[rsp+0x30]
000F4C6A: mov       QWORD PTR [r11-0x20],rbp
000F4C6E: mov       QWORD PTR [r11-0x30],r14
000F4C72: mov       QWORD PTR [rsp+0x30],0x0
000F4C7B: call      QWORD PTR [rip+0x1142f]        # 0x1801060b0 ; __imp_QueryPerformanceCounter | KERNEL32.dll!QueryPerformanceCounter
000F4C81: mov       rax,QWORD PTR [rsp+0x30]
000F4C86: lea       rcx,[rsp+0x38]
000F4C8B: mov       QWORD PTR [rsp+0x20],rax
000F4C90: call      QWORD PTR [rip+0x1143a]        # 0x1801060d0 ; __imp_QueryPerformanceFrequency | KERNEL32.dll!QueryPerformanceFrequency
000F4C96: movabs    rax,0x20c49ba5e353f7cf
000F4CA0: mov       DWORD PTR [rsp+0x28],0x80
000F4CA8: imul      QWORD PTR [rsp+0x38]
000F4CAD: lea       r9,[rsp+0x40]
000F4CB2: mov       rcx,rsi
000F4CB5: mov       r15,rdx
000F4CB8: lea       r8,[rsp+0x28]
000F4CBD: sar       r15,0x7
000F4CC1: lea       rdx,[rip+0x124c0]        # 0x180107188 ; WKPDID_D3DDebugObjectNameW
000F4CC8: mov       rax,r15
000F4CCB: shr       rax,0x3f
000F4CCF: add       r15,rax
000F4CD2: mov       rax,QWORD PTR [rsi]
000F4CD5: call      QWORD PTR [rax+0x18]
000F4CD8: cmp       BYTE PTR [rsp+0x130],0x0
000F4CE0: jne       0x1800f4d76
000F4CE6: mov       r14,QWORD PTR [rsp+0x30]
000F4CEB: mov       QWORD PTR [rsp+0xe0],r12
000F4CF3: mov       rax,QWORD PTR [rsi]
000F4CF6: mov       rcx,rsi
000F4CF9: call      QWORD PTR [rax+0x40]
000F4CFC: cmp       rax,rbx
000F4CFF: lea       rcx,[rsp+0x20]
000F4D04: mov       rbp,rax
000F4D07: setae     r12b
000F4D0B: call      QWORD PTR [rip+0x1139f]        # 0x1801060b0 ; __imp_QueryPerformanceCounter | KERNEL32.dll!QueryPerformanceCounter
000F4D11: test      rdi,rdi
000F4D14: je        0x1800f4d32
000F4D16: mov       rax,QWORD PTR [rsp+0x20]
000F4D1B: sub       rax,r14
000F4D1E: cmp       rax,r15
000F4D21: jle       0x1800f4d32
000F4D23: mov       rdx,rbx
000F4D26: lea       rcx,[rsp+0x40]
000F4D2B: call      rdi
000F4D2D: mov       r14,QWORD PTR [rsp+0x20]
000F4D32: cmp       rbp,rbx
000F4D35: jb        0x1800f4cf3
000F4D37: movzx     eax,r12b
000F4D3B: mov       r12,QWORD PTR [rsp+0xe0]
000F4D43: mov       r14,QWORD PTR [rsp+0xd8]
000F4D4B: mov       rbp,QWORD PTR [rsp+0xe8]
000F4D53: mov       r15,QWORD PTR [rsp+0xd0]
000F4D5B: mov       rcx,QWORD PTR [rsp+0xc0]
000F4D63: xor       rcx,rsp
000F4D66: call      0x180104650 ; __security_check_cookie
000F4D6B: add       rsp,0xf0
000F4D72: pop       rdi
000F4D73: pop       rsi
000F4D74: pop       rbx
000F4D75: ret       
000F4D76: mov       rax,QWORD PTR [rsi]
000F4D79: mov       rcx,rsi
000F4D7C: call      QWORD PTR [rax+0x40]
000F4D7F: cmp       rax,rbx
000F4D82: setae     bpl
000F4D86: jae       0x1800f4df0
000F4D88: xor       r9d,r9d
000F4D8B: xor       r8d,r8d
000F4D8E: xor       edx,edx
000F4D90: xor       ecx,ecx
000F4D92: call      QWORD PTR [rip+0x11348]        # 0x1801060e0 ; __imp_CreateEventW | KERNEL32.dll!CreateEventW
000F4D98: mov       r14,rax
000F4D9B: test      rax,rax
000F4D9E: je        0x1800f4df0
000F4DA0: mov       rax,QWORD PTR [rsi]
000F4DA3: mov       r8,r14
000F4DA6: mov       rdx,rbx
000F4DA9: mov       rcx,rsi
000F4DAC: call      QWORD PTR [rax+0x48]
000F4DAF: test      eax,eax
000F4DB1: js        0x1800f4de7
000F4DB3: lea       rcx,[rsp+0x20]
000F4DB8: call      QWORD PTR [rip+0x112f2]        # 0x1801060b0 ; __imp_QueryPerformanceCounter | KERNEL32.dll!QueryPerformanceCounter
000F4DBE: mov       edx,0x1
000F4DC3: mov       rcx,r14
000F4DC6: call      QWORD PTR [rip+0x1131c]        # 0x1801060e8 ; __imp_WaitForSingleObject | KERNEL32.dll!WaitForSingleObject
000F4DCC: test      eax,eax
000F4DCE: mov       esi,eax
000F4DD0: sete      bpl
000F4DD4: test      rdi,rdi
000F4DD7: je        0x1800f4de3
000F4DD9: mov       rdx,rbx
000F4DDC: lea       rcx,[rsp+0x40]
000F4DE1: call      rdi
000F4DE3: test      esi,esi
000F4DE5: jne       0x1800f4db3
000F4DE7: mov       rcx,r14
000F4DEA: call      QWORD PTR [rip+0x112d8]        # 0x1801060c8 ; __imp_CloseHandle | KERNEL32.dll!CloseHandle
000F4DF0: movzx     eax,bpl
000F4DF4: jmp       0x1800f4d43
000F4DF9: xor       al,al
000F4DFB: jmp       0x1800f4d5b
