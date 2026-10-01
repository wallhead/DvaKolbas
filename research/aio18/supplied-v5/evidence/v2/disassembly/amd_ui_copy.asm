; amd_fidelityfx_framegeneration_dx12.dll SHA256=3f5e674a59b400756e98ef31fd583a4eb08ad567ec8dee362886bc96e55ed347
; ImageBase=0x180000000, RVAs in left column
; range 0x9EA0..0xA0B4; TFrameInterpolationSwapChainDX12<IFrameInterpolationSwapChainDX12,FrameinterpolationPresentInfoExt,FfxFrameGenerationConfig,ffxCallbackDescFrameGenerationPresent,16>::copyUiResource
00009EA0: mov       QWORD PTR [rsp+0x10],rbx
00009EA5: mov       QWORD PTR [rsp+0x18],rsi
00009EAA: mov       QWORD PTR [rsp+0x20],rdi
00009EAF: push      rbp
00009EB0: push      r12
00009EB2: push      r13
00009EB4: push      r14
00009EB6: push      r15
00009EB8: mov       rbp,rsp
00009EBB: sub       rsp,0x80
00009EC2: mov       rax,QWORD PTR [rip+0x2464137]        # 0x18246e000
00009EC9: xor       rax,rsp
00009ECC: mov       QWORD PTR [rbp-0x8],rax
00009ED0: mov       r14,QWORD PTR [rcx+0x16d0]
00009ED7: lea       r15,[rcx+0x40]
00009EDB: mov       r13,rcx
00009EDE: lea       rdx,[rbp-0x60]
00009EE2: mov       rcx,r14
00009EE5: mov       rax,QWORD PTR [r14]
00009EE8: call      QWORD PTR [rax+0x90]
00009EEE: mov       rcx,r15
00009EF1: call      QWORD PTR [rip+0xfc219]        # 0x180106110 ; KERNEL32.dll!EnterCriticalSection
00009EF7: xor       r12d,r12d
00009EFA: mov       ebx,r12d
00009EFD: mov       esi,r12d
00009F00: test      rbx,rbx
00009F03: jne       0x180009f45
00009F05: movsxd    rax,DWORD PTR [rbp-0x60]
00009F09: mov       rdx,r14
00009F0C: shl       rax,0x5
00009F10: inc       rax
00009F13: add       rax,rsi
00009F16: lea       rcx,[rax+rax*4]
00009F1A: lea       rdi,[r15+rcx*8]
00009F1E: mov       rcx,rdi
00009F21: call      0x180007320
00009F26: test      al,al
00009F28: je        0x180009f3c
00009F2A: mov       rcx,QWORD PTR [rdi+0x18]
00009F2E: mov       rax,QWORD PTR [rcx]
00009F31: call      QWORD PTR [rax+0x40]
00009F34: cmp       rax,QWORD PTR [rdi+0x20]
00009F38: cmovae    rbx,rdi
00009F3C: inc       rsi
00009F3F: cmp       rsi,0x20
00009F43: jb        0x180009f00
00009F45: mov       rcx,QWORD PTR [rbx+0x8]
00009F49: lea       rdx,[rip+0xfccd8]        # 0x180106c28 ; 'uiResourceCopyList'
00009F50: inc       QWORD PTR [rbx+0x20]
00009F54: mov       QWORD PTR [rbx],r14
00009F57: mov       rax,QWORD PTR [rcx]
00009F5A: call      QWORD PTR [rax+0x30]
00009F5D: mov       rcx,QWORD PTR [rbx+0x10]
00009F61: lea       rdx,[rip+0xfccc0]        # 0x180106c28 ; 'uiResourceCopyList'
00009F68: mov       rax,QWORD PTR [rcx]
00009F6B: call      QWORD PTR [rax+0x30]
00009F6E: mov       rcx,QWORD PTR [rbx+0x18]
00009F72: lea       rdx,[rip+0xfccaf]        # 0x180106c28 ; 'uiResourceCopyList'
00009F79: mov       rax,QWORD PTR [rcx]
00009F7C: call      QWORD PTR [rax+0x30]
00009F7F: mov       rcx,r15
00009F82: call      QWORD PTR [rip+0xfc170]        # 0x1801060f8 ; KERNEL32.dll!LeaveCriticalSection
00009F88: mov       rcx,QWORD PTR [rbx+0x8]
00009F8C: mov       rax,QWORD PTR [rcx]
00009F8F: call      QWORD PTR [rax+0x40]
00009F92: test      eax,eax
00009F94: js        0x180009fa7
00009F96: mov       rcx,QWORD PTR [rbx+0x10]
00009F9A: xor       r8d,r8d
00009F9D: mov       rdx,QWORD PTR [rbx+0x8]
00009FA1: mov       rax,QWORD PTR [rcx]
00009FA4: call      QWORD PTR [rax+0x50]
00009FA7: mov       r14,QWORD PTR [rbx+0x10]
00009FAB: mov       rdi,QWORD PTR [r13+0x1688]
00009FB2: mov       ecx,DWORD PTR [r13+0x16b0]
00009FB9: mov       rsi,QWORD PTR [r13+0x1a30]
00009FC0: mov       QWORD PTR [rbp-0x48],rdi
00009FC4: mov       QWORD PTR [rbp-0x50],r12
00009FC8: mov       DWORD PTR [rbp-0x40],r12d
00009FCC: mov       QWORD PTR [rbp-0x38],0x800
00009FD4: mov       QWORD PTR [rbp-0x30],r12
00009FD8: mov       DWORD PTR [rbp-0x20],r12d
00009FDC: mov       QWORD PTR [rbp-0x18],0x400
00009FE4: call      0x180001aa0
00009FE9: mov       DWORD PTR [rbp-0x3c],eax
00009FEC: lea       r8,[rbp-0x50]
00009FF0: mov       DWORD PTR [rbp-0x1c],eax
00009FF3: mov       edx,0x2
00009FF8: mov       QWORD PTR [rbp-0x28],rsi
00009FFC: mov       rcx,r14
00009FFF: mov       rax,QWORD PTR [r14]
0000A002: call      QWORD PTR [rax+0xd0]
0000A008: mov       rax,QWORD PTR [r14]
0000A00B: mov       r8,rdi
0000A00E: mov       rdx,rsi
0000A011: mov       rcx,r14
0000A014: call      QWORD PTR [rax+0x88]
0000A01A: mov       r8d,DWORD PTR [rbp-0x3c]
0000A01E: mov       edx,0x2
0000A023: mov       eax,DWORD PTR [rbp-0x38]
0000A026: mov       rcx,r14
0000A029: mov       DWORD PTR [rbp-0x38],r8d
0000A02D: mov       r8d,DWORD PTR [rbp-0x1c]
0000A031: mov       DWORD PTR [rbp-0x3c],eax
0000A034: mov       eax,DWORD PTR [rbp-0x18]
0000A037: mov       DWORD PTR [rbp-0x18],r8d
0000A03B: lea       r8,[rbp-0x50]
0000A03F: mov       DWORD PTR [rbp-0x1c],eax
0000A042: mov       rax,QWORD PTR [r14]
0000A045: call      QWORD PTR [rax+0xd0]
0000A04B: mov       rcx,QWORD PTR [rbx+0x10]
0000A04F: mov       rax,QWORD PTR [rcx]
0000A052: call      QWORD PTR [rax+0x48]
0000A055: mov       rax,QWORD PTR [rbx+0x10]
0000A059: lea       r8,[rbp-0x10]
0000A05D: mov       rcx,QWORD PTR [rbx]
0000A060: mov       edx,0x1
0000A065: mov       QWORD PTR [rbp-0x10],rax
0000A069: mov       rax,QWORD PTR [rcx]
0000A06C: call      QWORD PTR [rax+0x50]
0000A06F: mov       rcx,QWORD PTR [rbx]
0000A072: mov       r8,QWORD PTR [rbx+0x20]
0000A076: mov       rdx,QWORD PTR [rbx+0x18]
0000A07A: mov       rax,QWORD PTR [rcx]
0000A07D: call      QWORD PTR [rax+0x70]
0000A080: mov       QWORD PTR [r13+0x1688],r12
0000A087: mov       rcx,QWORD PTR [rbp-0x8]
0000A08B: xor       rcx,rsp
0000A08E: call      0x180104650
0000A093: lea       r11,[rsp+0x80]
0000A09B: mov       rbx,QWORD PTR [r11+0x38]
0000A09F: mov       rsi,QWORD PTR [r11+0x40]
0000A0A3: mov       rdi,QWORD PTR [r11+0x48]
0000A0A7: mov       rsp,r11
0000A0AA: pop       r15
0000A0AC: pop       r14
0000A0AE: pop       r13
0000A0B0: pop       r12
0000A0B2: pop       rbp
0000A0B3: ret       
