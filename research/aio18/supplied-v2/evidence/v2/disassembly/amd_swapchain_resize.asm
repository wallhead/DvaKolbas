; amd_fidelityfx_framegeneration_dx12.dll SHA256=3f5e674a59b400756e98ef31fd583a4eb08ad567ec8dee362886bc96e55ed347
; ImageBase=0x180000000, RVAs in left column
; range 0xBEC0..0xBF84; TFrameInterpolationSwapChainDX12<IFrameInterpolationSwapChainDX12,FrameinterpolationPresentInfoExt,FfxFrameGenerationConfig,ffxCallbackDescFrameGenerationPresent,16>::ResizeBuffers
0000BEC0: mov       QWORD PTR [rsp+0x8],rbx
0000BEC5: mov       QWORD PTR [rsp+0x10],rbp
0000BECA: mov       QWORD PTR [rsp+0x18],rsi
0000BECF: mov       QWORD PTR [rsp+0x20],rdi
0000BED4: push      r12
0000BED6: push      r14
0000BED8: push      r15
0000BEDA: sub       rsp,0x30
0000BEDE: mov       rax,QWORD PTR [rcx]
0000BEE1: mov       r14d,r9d
0000BEE4: mov       r15d,r8d
0000BEE7: mov       edi,edx
0000BEE9: mov       rbx,rcx
0000BEEC: call      QWORD PTR [rax+0x188]
0000BEF2: lea       rsi,[rbx+0x17f8]
0000BEF9: mov       rcx,rsi
0000BEFC: call      QWORD PTR [rip+0xfa20e]        # 0x180106110 ; KERNEL32.dll!EnterCriticalSection
0000BF02: mov       rax,QWORD PTR [rbx]
0000BF05: mov       rcx,rbx
0000BF08: mov       ebp,DWORD PTR [rsp+0x78]
0000BF0C: mov       edx,ebp
0000BF0E: call      QWORD PTR [rax+0x200]
0000BF14: mov       r12d,eax
0000BF17: test      edi,edi
0000BF19: je        0x18000bf21
0000BF1B: mov       DWORD PTR [rbx+0x1848],edi
0000BF21: mov       rcx,QWORD PTR [rbx]
0000BF24: mov       DWORD PTR [rbx+0x184c],ebp
0000BF2A: mov       rdx,QWORD PTR [rcx+0x1a8]
0000BF31: mov       rcx,rbx
0000BF34: call      rdx
0000BF36: mov       DWORD PTR [rsp+0x28],r12d
0000BF3B: mov       r9d,r14d
0000BF3E: mov       r8d,r15d
0000BF41: mov       rcx,rax
0000BF44: mov       rdx,QWORD PTR [rax]
0000BF47: mov       r10,QWORD PTR [rdx+0x68]
0000BF4B: mov       edx,DWORD PTR [rsp+0x70]
0000BF4F: mov       DWORD PTR [rsp+0x20],edx
0000BF53: xor       edx,edx
0000BF55: call      r10
0000BF58: mov       rcx,rsi
0000BF5B: mov       ebx,eax
0000BF5D: call      QWORD PTR [rip+0xfa195]        # 0x1801060f8 ; KERNEL32.dll!LeaveCriticalSection
0000BF63: mov       rbp,QWORD PTR [rsp+0x58]
0000BF68: mov       eax,ebx
0000BF6A: mov       rbx,QWORD PTR [rsp+0x50]
0000BF6F: mov       rsi,QWORD PTR [rsp+0x60]
0000BF74: mov       rdi,QWORD PTR [rsp+0x68]
0000BF79: add       rsp,0x30
0000BF7D: pop       r15
0000BF7F: pop       r14
0000BF81: pop       r12
0000BF83: ret       
