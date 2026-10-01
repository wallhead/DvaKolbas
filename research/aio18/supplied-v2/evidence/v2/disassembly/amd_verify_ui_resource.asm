; amd_fidelityfx_framegeneration_dx12.dll SHA256=3f5e674a59b400756e98ef31fd583a4eb08ad567ec8dee362886bc96e55ed347
; ImageBase=0x180000000, RVAs in left column
; range 0x9C60..0x9E9D; TFrameInterpolationSwapChainDX12<IFrameInterpolationSwapChainDX12,FrameinterpolationPresentInfoExt,FfxFrameGenerationConfig,ffxCallbackDescFrameGenerationPresent,16>::verifyUiDuplicateResource
00009C60: mov       QWORD PTR [rsp+0x10],rbx
00009C65: mov       QWORD PTR [rsp+0x18],rsi
00009C6A: mov       QWORD PTR [rsp+0x20],rdi
00009C6F: push      r14
00009C71: sub       rsp,0xe0
00009C78: mov       rax,QWORD PTR [rip+0x2464381]        # 0x18246e000 ; __security_cookie
00009C7F: xor       rax,rsp
00009C82: mov       QWORD PTR [rsp+0xd8],rax
00009C8A: mov       eax,DWORD PTR [rcx+0x16b8]
00009C90: xor       edi,edi
00009C92: mov       QWORD PTR [rsp+0x40],rdi
00009C97: mov       rbx,rcx
00009C9A: test      al,0x2
00009C9C: je        0x180009e11
00009CA2: mov       r14,QWORD PTR [rcx+0x1688]
00009CA9: test      r14,r14
00009CAC: je        0x180009e11
00009CB2: mov       rax,QWORD PTR [r14]
00009CB5: lea       rdx,[rsp+0x68]
00009CBA: mov       rcx,r14
00009CBD: call      QWORD PTR [rax+0x50]
00009CC0: lea       rsi,[rbx+0x1a30]
00009CC7: mov       rcx,QWORD PTR [rsi]
00009CCA: test      rcx,rcx
00009CCD: je        0x180009d3c
00009CCF: mov       rax,QWORD PTR [rcx]
00009CD2: lea       rdx,[rsp+0xa0]
00009CDA: call      QWORD PTR [rax+0x50]
00009CDD: mov       eax,DWORD PTR [rsp+0xc0]
00009CE4: cmp       DWORD PTR [rsp+0x88],eax
00009CEB: jne       0x180009d0c
00009CED: mov       rax,QWORD PTR [rsp+0xb0]
00009CF5: cmp       QWORD PTR [rsp+0x78],rax
00009CFA: jne       0x180009d0c
00009CFC: mov       eax,DWORD PTR [rsp+0xb8]
00009D03: cmp       DWORD PTR [rsp+0x80],eax
00009D0A: je        0x180009d3c
00009D0C: mov       r9,QWORD PTR [rbx+0x1750]
00009D13: mov       rdx,QWORD PTR [rbx+0x1a68]
00009D1A: mov       rcx,QWORD PTR [rbx+0x1708]
00009D21: mov       BYTE PTR [rsp+0x20],dil
00009D26: call      0x1800f4c30 ; waitForFenceValue
00009D2B: mov       rcx,QWORD PTR [rsi]
00009D2E: test      rcx,rcx
00009D31: je        0x180009d3c
00009D33: mov       rax,QWORD PTR [rcx]
00009D36: call      QWORD PTR [rax+0x10]
00009D39: mov       QWORD PTR [rsi],rdi
00009D3C: cmp       QWORD PTR [rsi],rdi
00009D3F: jne       0x180009e68
00009D45: mov       rax,QWORD PTR [r14]
00009D48: lea       r8,[rsp+0x40]
00009D4D: lea       rdx,[rip+0xfc7d4]        # 0x180106528 ; _GUID_189819f1_1db6_4b57_be54_1821339b85f7
00009D54: mov       rcx,r14
00009D57: call      QWORD PTR [rax+0x38]
00009D5A: test      eax,eax
00009D5C: js        0x180009e68
00009D62: xor       eax,eax
00009D64: lea       r8,[rsp+0x48]
00009D69: mov       DWORD PTR [rsp+0x60],eax
00009D6D: lea       rdx,[rsp+0x50]
00009D72: mov       rax,QWORD PTR [r14]
00009D75: xorps     xmm0,xmm0
00009D78: mov       rcx,r14
00009D7B: movups    XMMWORD PTR [rsp+0x50],xmm0
00009D80: call      QWORD PTR [rax+0x70]
00009D83: mov       r8d,DWORD PTR [rsp+0x48]
00009D88: mov       ecx,DWORD PTR [rbx+0x16b0]
00009D8E: and       r8d,0xffffff33
00009D95: mov       DWORD PTR [rsp+0x48],r8d
00009D9A: call      0x180001aa0 ; ffxGetDX12StateFromResourceState
00009D9F: mov       rcx,QWORD PTR [rsp+0x40]
00009DA4: lea       r9,[rsp+0x68]
00009DA9: mov       QWORD PTR [rsp+0x38],rsi
00009DAE: mov       rdx,QWORD PTR [rcx]
00009DB1: mov       r10,QWORD PTR [rdx+0xd8]
00009DB8: lea       rdx,[rip+0xfca51]        # 0x180106810 ; _GUID_696442be_a72e_4059_bc79_5b5c98040fad
00009DBF: mov       QWORD PTR [rsp+0x30],rdx
00009DC4: lea       rdx,[rsp+0x50]
00009DC9: mov       QWORD PTR [rsp+0x28],rdi
00009DCE: mov       DWORD PTR [rsp+0x20],eax
00009DD2: call      r10
00009DD5: test      eax,eax
00009DD7: jns       0x180009de0
00009DD9: mov       edi,0x80004005
00009DDE: jmp       0x180009dff
00009DE0: mov       rcx,QWORD PTR [rsi]
00009DE3: call      0x1800f5050 ; GetResourceGpuMemorySize
00009DE8: mov       rcx,QWORD PTR [rsi]
00009DEB: lea       rdx,[rip+0xfcdf6]        # 0x180106be8 ; 'AMD FSR Internal Ui Resource'
00009DF2: add       QWORD PTR [rbx+0x1ab0],rax
00009DF9: mov       rax,QWORD PTR [rcx]
00009DFC: call      QWORD PTR [rax+0x30]
00009DFF: mov       rcx,QWORD PTR [rsp+0x40]
00009E04: test      rcx,rcx
00009E07: je        0x180009e68
00009E09: mov       rax,QWORD PTR [rcx]
00009E0C: call      QWORD PTR [rax+0x10]
00009E0F: jmp       0x180009e68
00009E11: mov       rcx,QWORD PTR [rcx+0x1a30]
00009E18: test      rcx,rcx
00009E1B: je        0x180009e68
00009E1D: call      0x1800f5050 ; GetResourceGpuMemorySize
00009E22: mov       r9,QWORD PTR [rbx+0x1750]
00009E29: mov       rdx,QWORD PTR [rbx+0x1a68]
00009E30: mov       rcx,QWORD PTR [rbx+0x1708]
00009E37: sub       QWORD PTR [rbx+0x1ab0],rax
00009E3E: mov       BYTE PTR [rsp+0x20],dil
00009E43: call      0x1800f4c30 ; waitForFenceValue
00009E48: mov       rcx,QWORD PTR [rbx+0x1a30]
00009E4F: test      rcx,rcx
00009E52: je        0x180009e5a
00009E54: mov       rdx,QWORD PTR [rcx]
00009E57: call      QWORD PTR [rdx+0x10]
00009E5A: mov       QWORD PTR [rbx+0x1a30],rdi
00009E61: mov       QWORD PTR [rbx+0x1a38],rdi
00009E68: shr       edi,0x1f
00009E6B: xor       dil,0x1
00009E6F: movzx     eax,dil
00009E73: mov       rcx,QWORD PTR [rsp+0xd8]
00009E7B: xor       rcx,rsp
00009E7E: call      0x180104650 ; __security_check_cookie
00009E83: lea       r11,[rsp+0xe0]
00009E8B: mov       rbx,QWORD PTR [r11+0x18]
00009E8F: mov       rsi,QWORD PTR [r11+0x20]
00009E93: mov       rdi,QWORD PTR [r11+0x28]
00009E97: mov       rsp,r11
00009E9A: pop       r14
00009E9C: ret       
