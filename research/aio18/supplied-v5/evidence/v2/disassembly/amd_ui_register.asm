; amd_fidelityfx_framegeneration_dx12.dll SHA256=3f5e674a59b400756e98ef31fd583a4eb08ad567ec8dee362886bc96e55ed347
; ImageBase=0x180000000, RVAs in left column
; range 0xADC0..0xAE41; TFrameInterpolationSwapChainDX12<IFrameInterpolationSwapChainDX12,FrameinterpolationPresentInfoExt,FfxFrameGenerationConfig,ffxCallbackDescFrameGenerationPresent,16>::registerUiResource
0000ADC0: mov       QWORD PTR [rsp+0x8],rbx
0000ADC5: mov       QWORD PTR [rsp+0x10],rbp
0000ADCA: mov       QWORD PTR [rsp+0x18],rsi
0000ADCF: push      rdi
0000ADD0: sub       rsp,0x20
0000ADD4: mov       rsi,rcx
0000ADD7: mov       edi,r8d
0000ADDA: add       rcx,0x17f8
0000ADE1: mov       rbx,rdx
0000ADE4: call      QWORD PTR [rip+0xfb326]        # 0x180106110 ; KERNEL32.dll!EnterCriticalSection
0000ADEA: cmp       QWORD PTR [rbx],0x0
0000ADEE: movups    xmm0,XMMWORD PTR [rbx]
0000ADF1: mov       DWORD PTR [rsi+0x16b8],edi
0000ADF7: movups    xmm1,XMMWORD PTR [rbx+0x10]
0000ADFB: movups    XMMWORD PTR [rsi+0x1688],xmm0
0000AE02: movups    xmm0,XMMWORD PTR [rbx+0x20]
0000AE06: movups    XMMWORD PTR [rsi+0x1698],xmm1
0000AE0D: movups    XMMWORD PTR [rsi+0x16a8],xmm0
0000AE14: jne       0x18000ae1f
0000AE16: and       edi,0xfffffffd
0000AE19: mov       DWORD PTR [rsi+0x16b8],edi
0000AE1F: lea       rcx,[rsi+0x17f8]
0000AE26: mov       rbx,QWORD PTR [rsp+0x30]
0000AE2B: mov       rbp,QWORD PTR [rsp+0x38]
0000AE30: mov       rsi,QWORD PTR [rsp+0x40]
0000AE35: add       rsp,0x20
0000AE39: pop       rdi
0000AE3A: rex.W     jmp QWORD PTR [rip+0xfb2b7]        # 0x1801060f8 ; KERNEL32.dll!LeaveCriticalSection
