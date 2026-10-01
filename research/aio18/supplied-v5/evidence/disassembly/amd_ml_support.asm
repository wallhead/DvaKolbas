; amd_fidelityfx_framegeneration_dx12.dll SHA256=3f5e674a59b400756e98ef31fd583a4eb08ad567ec8dee362886bc96e55ed347
; ImageBase=0x180000000, RVAs in left column
; range 0xECA90..0xECB62; ffxProvider_MLFrameGeneration::IsSupported
000ECA90: rex       push rbx
000ECA92: sub       rsp,0x160
000ECA99: mov       rax,QWORD PTR [rip+0x2381560]        # 0x18246e000 ; __security_cookie
000ECAA0: xor       rax,rsp
000ECAA3: mov       QWORD PTR [rsp+0x150],rax
000ECAAB: mov       rbx,rdx
000ECAAE: test      rdx,rdx
000ECAB1: je        0x1800ecb47
000ECAB7: lea       rcx,[rip+0x2378eb2]        # 0x182465970
000ECABE: call      QWORD PTR [rip+0x1965c]        # 0x180106120 ; __imp_GetModuleHandleA | KERNEL32.dll!GetModuleHandleA
000ECAC4: test      rax,rax
000ECAC7: je        0x1800ecb47
000ECAC9: lea       rdx,[rip+0x2378ea8]        # 0x182465978 ; 'RtlGetVersion'
000ECAD0: mov       rcx,rax
000ECAD3: call      QWORD PTR [rip+0x1956f]        # 0x180106048 ; __imp_GetProcAddress | KERNEL32.dll!GetProcAddress
000ECAD9: test      rax,rax
000ECADC: je        0x1800ecb47
000ECADE: xor       ecx,ecx
000ECAE0: mov       QWORD PTR [rsp+0x30],0x11c
000ECAE9: mov       DWORD PTR [rsp+0x3c],ecx
000ECAED: lea       rcx,[rsp+0x30]
000ECAF2: call      rax
000ECAF4: cmp       DWORD PTR [rsp+0x34],0xa
000ECAF9: ja        0x1800ecb0b
000ECAFB: jne       0x1800ecb07
000ECAFD: cmp       DWORD PTR [rsp+0x3c],0x55ec
000ECB05: jae       0x1800ecb0b
000ECB07: xor       al,al
000ECB09: jmp       0x1800ecb0d
000ECB0B: mov       al,0x1
000ECB0D: test      al,al
000ECB0F: je        0x1800ecb49
000ECB11: mov       rax,QWORD PTR [rbx]
000ECB14: lea       r8,[rsp+0x20]
000ECB19: mov       r9d,0x4
000ECB1F: mov       DWORD PTR [rsp+0x20],0x66
000ECB27: mov       edx,0x7
000ECB2C: mov       rcx,rbx
000ECB2F: call      QWORD PTR [rax+0x68]
000ECB32: test      eax,eax
000ECB34: js        0x1800ecb47
000ECB36: cmp       DWORD PTR [rsp+0x20],0x66
000ECB3B: jl        0x1800ecb47
000ECB3D: mov       rcx,rbx
000ECB40: call      0x1800f6e90 ; EnableAMDExtensions
000ECB45: jmp       0x1800ecb49
000ECB47: xor       al,al
000ECB49: mov       rcx,QWORD PTR [rsp+0x150]
000ECB51: xor       rcx,rsp
000ECB54: call      0x180104650 ; __security_check_cookie
000ECB59: add       rsp,0x160
000ECB60: pop       rbx
000ECB61: ret       
