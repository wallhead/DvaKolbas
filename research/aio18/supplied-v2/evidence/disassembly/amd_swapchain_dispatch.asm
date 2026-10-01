; amd_fidelityfx_framegeneration_dx12.dll SHA256=3f5e674a59b400756e98ef31fd583a4eb08ad567ec8dee362886bc96e55ed347
; ImageBase=0x180000000, RVAs in left column
; range 0x94A0..0x98D2; TFrameInterpolationSwapChainDX12<IFrameInterpolationSwapChainDX12,FrameinterpolationPresentInfoExt,FfxFrameGenerationConfig,ffxCallbackDescFrameGenerationPresent,16>::dispatchInterpolationCommands
000094A0: mov       rax,rsp
000094A3: push      rbp
000094A4: push      rbx
000094A5: push      rsi
000094A6: push      rdi
000094A7: push      r13
000094A9: push      r14
000094AB: lea       rbp,[rax-0x128]
000094B2: sub       rsp,0x1f8
000094B9: movaps    XMMWORD PTR [rax-0x48],xmm6
000094BD: movaps    XMMWORD PTR [rax-0x58],xmm7
000094C1: movaps    XMMWORD PTR [rax-0x68],xmm8
000094C6: mov       rax,QWORD PTR [rip+0x2464b33]        # 0x18246e000 ; __security_cookie
000094CD: xor       rax,rsp
000094D0: mov       QWORD PTR [rbp+0xb8],rax
000094D7: mov       rax,QWORD PTR [rcx]
000094DA: mov       rsi,r8
000094DD: mov       r13,rdx
000094E0: mov       r14,rcx
000094E3: call      QWORD PTR [rax+0x120]
000094E9: mov       edi,eax
000094EB: lea       rcx,[rsp+0x50]
000094F0: lea       rbx,[rdi+0x191]
000094F7: add       rbx,rbx
000094FA: mov       rdx,QWORD PTR [r14+rbx*8]
000094FE: call      0x180001f50 ; ffxGetResourceDescriptionDX12
00009503: mov       rcx,QWORD PTR [r14+rbx*8]
00009507: mov       QWORD PTR [rsp+0x20],rcx
0000950C: mov       QWORD PTR [rsp+0x48],0x80
00009515: movups    xmm0,XMMWORD PTR [rax]
00009518: movups    xmm1,XMMWORD PTR [rax+0x10]
0000951C: movups    XMMWORD PTR [rsp+0x28],xmm0
00009521: movups    xmm6,XMMWORD PTR [rsp+0x20]
00009526: movups    XMMWORD PTR [rsp+0x38],xmm1
0000952B: movups    xmm7,XMMWORD PTR [rsp+0x30]
00009530: movups    xmm8,XMMWORD PTR [rsp+0x40]
00009536: movups    XMMWORD PTR [rsi],xmm6
00009539: movups    XMMWORD PTR [rsi+0x10],xmm7
0000953D: movups    XMMWORD PTR [rsi+0x20],xmm8
00009542: mov       rcx,QWORD PTR [r14+0x16c0]
00009549: movsxd    r8,DWORD PTR [r14+0x1a44]
00009550: mov       rdx,QWORD PTR [r14+0x1708]
00009557: add       r8,r8
0000955A: mov       rax,QWORD PTR [rcx]
0000955D: mov       r8,QWORD PTR [r14+r8*8+0x1a18]
00009565: call      QWORD PTR [rax+0x78]
00009568: mov       rbx,QWORD PTR [r14+rdi*8+0x1890]
00009570: test      rbx,rbx
00009573: je        0x180009611
00009579: mov       rcx,QWORD PTR [rbx+0x10]
0000957D: mov       rax,QWORD PTR [rcx]
00009580: call      QWORD PTR [rax+0x48]
00009583: mov       rax,QWORD PTR [rbx+0x10]
00009587: lea       r8,[rbp+0xb0]
0000958E: mov       rcx,QWORD PTR [rbx]
00009591: mov       edx,0x1
00009596: mov       QWORD PTR [rbp+0xb0],rax
0000959D: mov       rax,QWORD PTR [rcx]
000095A0: call      QWORD PTR [rax+0x50]
000095A3: mov       rcx,QWORD PTR [rbx]
000095A6: mov       r8,QWORD PTR [rbx+0x20]
000095AA: mov       rdx,QWORD PTR [rbx+0x18]
000095AE: mov       rax,QWORD PTR [rcx]
000095B1: call      QWORD PTR [rax+0x70]
000095B4: mov       rcx,QWORD PTR [r14+0x16c0]
000095BB: mov       rdx,QWORD PTR [r14+0x16e8]
000095C2: mov       rax,QWORD PTR [rcx]
000095C5: inc       QWORD PTR [r14+0x1868]
000095CC: mov       r8,QWORD PTR [r14+0x1868]
000095D3: call      QWORD PTR [rax+0x70]
000095D6: mov       rax,QWORD PTR [r14]
000095D9: lea       rdx,[rsp+0x20]
000095DE: xor       r8d,r8d
000095E1: mov       rcx,r14
000095E4: call      QWORD PTR [rax+0x1c0]
000095EA: movups    xmm0,XMMWORD PTR [rax]
000095ED: movups    XMMWORD PTR [r13+0x0],xmm0
000095F2: movups    xmm1,XMMWORD PTR [rax+0x10]
000095F6: movups    XMMWORD PTR [r13+0x10],xmm1
000095FB: movups    xmm0,XMMWORD PTR [rax+0x20]
000095FF: movups    XMMWORD PTR [r13+0x20],xmm0
00009604: mov       BYTE PTR [r14+0x1878],0x0
0000960C: jmp       0x1800098a0
00009611: mov       QWORD PTR [rsp+0x248],r12
00009619: lea       rdx,[rsp+0x50]
0000961E: mov       QWORD PTR [rsp+0x1f0],r15
00009626: lea       r12,[r14+0x40]
0000962A: mov       r15,QWORD PTR [r14+0x16c0]
00009631: mov       rcx,r15
00009634: mov       rax,QWORD PTR [r15]
00009637: call      QWORD PTR [rax+0x90]
0000963D: mov       rcx,r12
00009640: call      QWORD PTR [rip+0xfcaca]        # 0x180106110 ; __imp_EnterCriticalSection | KERNEL32.dll!EnterCriticalSection
00009646: xor       edi,edi
00009648: xor       ebx,ebx
0000964A: nop       WORD PTR [rax+rax*1+0x0]
00009650: test      rdi,rdi
00009653: jne       0x180009696
00009655: movsxd    rax,DWORD PTR [rsp+0x50]
0000965A: mov       rdx,r15
0000965D: shl       rax,0x5
00009661: inc       rax
00009664: add       rax,rbx
00009667: lea       rcx,[rax+rax*4]
0000966B: lea       rsi,[r12+rcx*8]
0000966F: mov       rcx,rsi
00009672: call      0x180007320 ; Dx12Commands::verify
00009677: test      al,al
00009679: je        0x18000968d
0000967B: mov       rcx,QWORD PTR [rsi+0x18]
0000967F: mov       rax,QWORD PTR [rcx]
00009682: call      QWORD PTR [rax+0x40]
00009685: cmp       rax,QWORD PTR [rsi+0x20]
00009689: cmovae    rdi,rsi
0000968D: inc       rbx
00009690: cmp       rbx,0x20
00009694: jb        0x180009650
00009696: mov       rcx,QWORD PTR [rdi+0x8]
0000969A: lea       rdx,[rip+0xfd507]        # 0x180106ba8 ; 'getInterpolationCommandList()'
000096A1: inc       QWORD PTR [rdi+0x20]
000096A5: mov       QWORD PTR [rdi],r15
000096A8: mov       rax,QWORD PTR [rcx]
000096AB: call      QWORD PTR [rax+0x30]
000096AE: mov       rcx,QWORD PTR [rdi+0x10]
000096B2: lea       rdx,[rip+0xfd4ef]        # 0x180106ba8 ; 'getInterpolationCommandList()'
000096B9: mov       rax,QWORD PTR [rcx]
000096BC: call      QWORD PTR [rax+0x30]
000096BF: mov       rcx,QWORD PTR [rdi+0x18]
000096C3: lea       rdx,[rip+0xfd4de]        # 0x180106ba8 ; 'getInterpolationCommandList()'
000096CA: mov       rax,QWORD PTR [rcx]
000096CD: call      QWORD PTR [rax+0x30]
000096D0: mov       rcx,r12
000096D3: call      QWORD PTR [rip+0xfca1f]        # 0x1801060f8 ; __imp_LeaveCriticalSection | KERNEL32.dll!LeaveCriticalSection
000096D9: mov       rcx,QWORD PTR [rdi+0x8]
000096DD: mov       rax,QWORD PTR [rcx]
000096E0: call      QWORD PTR [rax+0x40]
000096E3: mov       r15,QWORD PTR [rsp+0x1f0]
000096EB: mov       r12,QWORD PTR [rsp+0x248]
000096F3: test      eax,eax
000096F5: js        0x180009708
000096F7: mov       rcx,QWORD PTR [rdi+0x10]
000096FB: xor       r8d,r8d
000096FE: mov       rdx,QWORD PTR [rdi+0x8]
00009702: mov       rax,QWORD PTR [rcx]
00009705: call      QWORD PTR [rax+0x50]
00009708: mov       rbx,QWORD PTR [rdi+0x10]
0000970C: lea       rcx,[rbp-0x48]
00009710: xor       edx,edx
00009712: mov       r8d,0xc0
00009718: call      0x1801056da ; memset
0000971D: xor       eax,eax
0000971F: mov       QWORD PTR [rsp+0x70],0x20003
00009728: mov       WORD PTR [rbp+0x7d],ax
0000972C: lea       rdx,[rsp+0x20]
00009731: mov       BYTE PTR [rbp+0x7f],al
00009734: xorps     xmm0,xmm0
00009737: mov       DWORD PTR [rbp+0x9c],eax
0000973D: xor       r8d,r8d
00009740: mov       QWORD PTR [rsp+0x78],rax
00009745: mov       rcx,r14
00009748: mov       rax,QWORD PTR [r14]
0000974B: movups    XMMWORD PTR [rbp+0x8c],xmm0
00009752: mov       QWORD PTR [rbp-0x80],rbx
00009756: call      QWORD PTR [rax+0x1c0]
0000975C: mov       rdx,QWORD PTR [r14+0x1aa8]
00009763: lea       rcx,[rsp+0x70]
00009768: movups    xmm0,XMMWORD PTR [rax]
0000976B: movups    XMMWORD PTR [rbp-0x48],xmm0
0000976F: movups    xmm1,XMMWORD PTR [rax+0x10]
00009773: movups    XMMWORD PTR [rbp-0x38],xmm1
00009777: movups    xmm0,XMMWORD PTR [rax+0x20]
0000977B: movzx     eax,BYTE PTR [r14+0x1878]
00009783: movss     xmm1,DWORD PTR [r14+0x1a8c]
0000978C: mov       BYTE PTR [rbp+0x7c],al
0000978F: mov       eax,DWORD PTR [r14+0x1a84]
00009796: movups    XMMWORD PTR [rbp-0x28],xmm0
0000979A: mov       DWORD PTR [rbp+0x80],eax
000097A0: movss     xmm0,DWORD PTR [r14+0x1a88]
000097A9: mov       rax,QWORD PTR [r14+0x1a60]
000097B0: movss     DWORD PTR [rbp+0x84],xmm0
000097B8: movups    xmm0,XMMWORD PTR [r14+0x187c]
000097C0: mov       QWORD PTR [rbp+0xa0],rax
000097C7: mov       rax,QWORD PTR [r14+0x1aa0]
000097CE: movups    XMMWORD PTR [rbp+0x8c],xmm0
000097D5: mov       DWORD PTR [rbp+0x78],0x1
000097DC: movss     DWORD PTR [rbp+0x88],xmm1
000097E4: movups    XMMWORD PTR [rbp-0x78],xmm6
000097E8: movups    XMMWORD PTR [rbp-0x68],xmm7
000097EC: movups    XMMWORD PTR [rbp-0x58],xmm8
000097F1: call      rax
000097F3: test      eax,eax
000097F5: jne       0x180009853
000097F7: mov       rcx,QWORD PTR [rdi+0x10]
000097FB: mov       rax,QWORD PTR [rcx]
000097FE: call      QWORD PTR [rax+0x48]
00009801: mov       rax,QWORD PTR [rdi+0x10]
00009805: lea       r8,[rbp+0xb0]
0000980C: mov       rcx,QWORD PTR [rdi]
0000980F: mov       edx,0x1
00009814: mov       QWORD PTR [rbp+0xb0],rax
0000981B: mov       rax,QWORD PTR [rcx]
0000981E: call      QWORD PTR [rax+0x50]
00009821: mov       rcx,QWORD PTR [rdi]
00009824: mov       r8,QWORD PTR [rdi+0x20]
00009828: mov       rdx,QWORD PTR [rdi+0x18]
0000982C: mov       rax,QWORD PTR [rcx]
0000982F: call      QWORD PTR [rax+0x70]
00009832: mov       rcx,QWORD PTR [r14+0x16c0]
00009839: mov       rdx,QWORD PTR [r14+0x16e8]
00009840: mov       rax,QWORD PTR [rcx]
00009843: inc       QWORD PTR [r14+0x1868]
0000984A: mov       r8,QWORD PTR [r14+0x1868]
00009851: jmp       0x180009861
00009853: mov       rcx,QWORD PTR [rdi]
00009856: mov       r8,QWORD PTR [rdi+0x20]
0000985A: mov       rdx,QWORD PTR [rdi+0x18]
0000985E: mov       rax,QWORD PTR [rcx]
00009861: call      QWORD PTR [rax+0x70]
00009864: cmp       DWORD PTR [rbp+0x78],0x0
00009868: jbe       0x1800098a0
0000986A: mov       rax,QWORD PTR [r14]
0000986D: lea       rdx,[rsp+0x20]
00009872: xor       r8d,r8d
00009875: mov       BYTE PTR [r14+0x1878],0x0
0000987D: mov       rcx,r14
00009880: call      QWORD PTR [rax+0x1c0]
00009886: movups    xmm0,XMMWORD PTR [rax]
00009889: movups    XMMWORD PTR [r13+0x0],xmm0
0000988E: movups    xmm1,XMMWORD PTR [rax+0x10]
00009892: movups    XMMWORD PTR [r13+0x10],xmm1
00009897: movups    xmm0,XMMWORD PTR [rax+0x20]
0000989B: movups    XMMWORD PTR [r13+0x20],xmm0
000098A0: mov       rcx,QWORD PTR [rbp+0xb8]
000098A7: xor       rcx,rsp
000098AA: call      0x180104650 ; __security_check_cookie
000098AF: lea       r11,[rsp+0x1f8]
000098B7: movaps    xmm6,XMMWORD PTR [r11-0x18]
000098BC: movaps    xmm7,XMMWORD PTR [r11-0x28]
000098C1: movaps    xmm8,XMMWORD PTR [r11-0x38]
000098C6: mov       rsp,r11
000098C9: pop       r14
000098CB: pop       r13
000098CD: pop       rdi
000098CE: pop       rsi
000098CF: pop       rbx
000098D0: pop       rbp
000098D1: ret       
