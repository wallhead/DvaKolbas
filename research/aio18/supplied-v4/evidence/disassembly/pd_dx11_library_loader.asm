; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xF3EC0..0xF42FA; unnamed
000F3EC0: mov       QWORD PTR [rsp+0x20],rbx
000F3EC5: push      rdi
000F3EC6: sub       rsp,0x20
000F3ECA: mov       rbx,rcx
000F3ECD: mov       QWORD PTR [rsp+0x30],rbp
000F3ED2: lea       rcx,[rip+0x10b714f]        # 0x1811ab028 ; 'ffx_backend_dx12_x64.dll'
000F3ED9: mov       QWORD PTR [rsp+0x38],rsi
000F3EDE: call      QWORD PTR [rip+0x1f234]        # 0x180113118 ; KERNEL32.dll!GetModuleHandleW
000F3EE4: mov       rsi,rax
000F3EE7: test      rax,rax
000F3EEA: jne       0x1800f3f01
000F3EEC: lea       rcx,[rip+0x10b716d]        # 0x1811ab060 ; 'ffx_backend_dx12_x64d.dll'
000F3EF3: call      QWORD PTR [rip+0x1f21f]        # 0x180113118 ; KERNEL32.dll!GetModuleHandleW
000F3EF9: mov       rsi,rax
000F3EFC: test      rax,rax
000F3EFF: je        0x1800f3f0d
000F3F01: lea       rcx,[rip+0x10b70c8]        # 0x1811aafd0 ; 'Loaded ffx_backend_dx12_x64.dll'
000F3F08: call      0x1800fbb40
000F3F0D: lea       rcx,[rip+0x10b70dc]        # 0x1811aaff0
000F3F14: call      QWORD PTR [rip+0x1f1fe]        # 0x180113118 ; KERNEL32.dll!GetModuleHandleW
000F3F1A: mov       rdi,rax
000F3F1D: test      rax,rax
000F3F20: jne       0x1800f3f37
000F3F22: lea       rcx,[rip+0x10b71bf]        # 0x1811ab0e8 ; 'ffx_backend_dx11_x64d.dll'
000F3F29: call      QWORD PTR [rip+0x1f1e9]        # 0x180113118 ; KERNEL32.dll!GetModuleHandleW
000F3F2F: mov       rdi,rax
000F3F32: test      rax,rax
000F3F35: je        0x1800f3f43
000F3F37: lea       rcx,[rip+0x10b71e2]        # 0x1811ab120 ; 'Loaded ffx_backend_dx11_x64.dll'
000F3F3E: call      0x1800fbb40
000F3F43: lea       rcx,[rip+0x10b714e]        # 0x1811ab098 ; 'ffx_fsr3_x64.dll'
000F3F4A: call      QWORD PTR [rip+0x1f1c8]        # 0x180113118 ; KERNEL32.dll!GetModuleHandleW
000F3F50: mov       rbp,rax
000F3F53: test      rax,rax
000F3F56: jne       0x1800f3f6d
000F3F58: lea       rcx,[rip+0x10b7161]        # 0x1811ab0c0 ; 'ffx_fsr3_x64d.dll'
000F3F5F: call      QWORD PTR [rip+0x1f1b3]        # 0x180113118 ; KERNEL32.dll!GetModuleHandleW
000F3F65: mov       rbp,rax
000F3F68: test      rax,rax
000F3F6B: je        0x1800f3f79
000F3F6D: lea       rcx,[rip+0x10b71fc]        # 0x1811ab170 ; 'Loaded ffx_fsr3_x64.dll'
000F3F74: call      0x1800fbb40
000F3F79: mov       QWORD PTR [rsp+0x40],r14
000F3F7E: test      rsi,rsi
000F3F81: je        0x1800f412d
000F3F87: lea       rdx,[rip+0x10b71fa]        # 0x1811ab188 ; 'ffxGetScratchMemorySizeDX12'
000F3F8E: mov       rcx,rsi
000F3F91: call      QWORD PTR [rip+0x1f141]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F3F97: lea       rdx,[rip+0x10b71ea]        # 0x1811ab188 ; 'ffxGetScratchMemorySizeDX12'
000F3F9E: mov       rcx,rsi
000F3FA1: mov       QWORD PTR [rbx],rax
000F3FA4: call      QWORD PTR [rip+0x1f12e]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F3FAA: lea       rdx,[rip+0x10b718f]        # 0x1811ab140 ; 'ffxGetDeviceDX12'
000F3FB1: mov       rcx,rsi
000F3FB4: mov       QWORD PTR [rbx],rax
000F3FB7: call      QWORD PTR [rip+0x1f11b]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F3FBD: lea       rdx,[rip+0x10b7194]        # 0x1811ab158 ; 'ffxGetInterfaceDX12'
000F3FC4: mov       rcx,rsi
000F3FC7: mov       QWORD PTR [rbx+0x8],rax
000F3FCB: call      QWORD PTR [rip+0x1f107]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F3FD1: lea       rdx,[rip+0x10b7208]        # 0x1811ab1e0 ; 'ffxGetCommandListDX12'
000F3FD8: mov       rcx,rsi
000F3FDB: mov       QWORD PTR [rbx+0x10],rax
000F3FDF: call      QWORD PTR [rip+0x1f0f3]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F3FE5: lea       rdx,[rip+0x10b720c]        # 0x1811ab1f8 ; 'ffxGetResourceDX12'
000F3FEC: mov       rcx,rsi
000F3FEF: mov       QWORD PTR [rbx+0x18],rax
000F3FF3: call      QWORD PTR [rip+0x1f0df]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F3FF9: lea       rdx,[rip+0x10b71a8]        # 0x1811ab1a8 ; 'ffxGetSurfaceFormatDX12'
000F4000: mov       rcx,rsi
000F4003: mov       QWORD PTR [rbx+0x20],rax
000F4007: call      QWORD PTR [rip+0x1f0cb]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F400D: lea       rdx,[rip+0x10b71ac]        # 0x1811ab1c0 ; 'ffxGetResourceDescriptionDX12'
000F4014: mov       rcx,rsi
000F4017: mov       QWORD PTR [rbx+0x28],rax
000F401B: call      QWORD PTR [rip+0x1f0b7]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F4021: mov       QWORD PTR [rbx+0x30],rax
000F4025: test      rax,rax
000F4028: jne       0x1800f403e
000F402A: lea       rdx,[rip+0x10b720f]        # 0x1811ab240 ; 'GetFfxResourceDescriptionDX12'
000F4031: mov       rcx,rsi
000F4034: call      QWORD PTR [rip+0x1f09e]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F403A: mov       QWORD PTR [rbx+0x30],rax
000F403E: lea       rdx,[rip+0x10b721b]        # 0x1811ab260 ; 'ffxGetCommandQueueDX12'
000F4045: mov       rcx,rsi
000F4048: call      QWORD PTR [rip+0x1f08a]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F404E: lea       rdx,[rip+0x10b71bb]        # 0x1811ab210 ; 'ffxGetSwapchainDX12'
000F4055: mov       rcx,rsi
000F4058: mov       QWORD PTR [rbx+0x38],rax
000F405C: call      QWORD PTR [rip+0x1f076]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F4062: lea       rdx,[rip+0x10b71bf]        # 0x1811ab228 ; 'ffxGetDX12SwapchainPtr'
000F4069: mov       rcx,rsi
000F406C: mov       QWORD PTR [rbx+0x40],rax
000F4070: call      QWORD PTR [rip+0x1f062]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F4076: lea       rdx,[rip+0x10b7243]        # 0x1811ab2c0 ; 'ffxReplaceSwapchainForFrameinterpolationDX12'
000F407D: mov       rcx,rsi
000F4080: mov       QWORD PTR [rbx+0x48],rax
000F4084: call      QWORD PTR [rip+0x1f04e]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F408A: lea       rdx,[rip+0x10b725f]        # 0x1811ab2f0 ; 'ffxCreateFrameinterpolationSwapchainDX12'
000F4091: mov       rcx,rsi
000F4094: mov       QWORD PTR [rbx+0x50],rax
000F4098: call      QWORD PTR [rip+0x1f03a]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F409E: lea       rdx,[rip+0x10b71d3]        # 0x1811ab278 ; 'ffxCreateFrameinterpolationSwapchainForHwndDX12'
000F40A5: mov       rcx,rsi
000F40A8: mov       QWORD PTR [rbx+0x58],rax
000F40AC: call      QWORD PTR [rip+0x1f026]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F40B2: lea       rdx,[rip+0x10b71ef]        # 0x1811ab2a8 ; 'ffxWaitForPresents'
000F40B9: mov       rcx,rsi
000F40BC: mov       QWORD PTR [rbx+0x60],rax
000F40C0: call      QWORD PTR [rip+0x1f012]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F40C6: lea       rdx,[rip+0x10b72ab]        # 0x1811ab378 ; 'ffxRegisterFrameinterpolationUiResourceDX12'
000F40CD: mov       rcx,rsi
000F40D0: mov       QWORD PTR [rbx+0x68],rax
000F40D4: call      QWORD PTR [rip+0x1effe]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F40DA: lea       rdx,[rip+0x10b72c7]        # 0x1811ab3a8 ; 'ffxGetFrameinterpolationCommandlistDX12'
000F40E1: mov       rcx,rsi
000F40E4: mov       QWORD PTR [rbx+0x70],rax
000F40E8: call      QWORD PTR [rip+0x1efea]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F40EE: lea       rdx,[rip+0x10b722b]        # 0x1811ab320 ; 'ffxGetFrameinterpolationTextureDX12'
000F40F5: mov       rcx,rsi
000F40F8: mov       QWORD PTR [rbx+0x78],rax
000F40FC: call      QWORD PTR [rip+0x1efd6]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F4102: lea       rdx,[rip+0x10b723f]        # 0x1811ab348 ; 'ffxSetFrameGenerationConfigToSwapchainDX12'
000F4109: mov       rcx,rsi
000F410C: mov       QWORD PTR [rbx+0x80],rax
000F4113: call      QWORD PTR [rip+0x1efbf]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F4119: lea       r14,[rbx+0x100]
000F4120: mov       QWORD PTR [rbx+0x88],rax
000F4127: mov       BYTE PTR [r14],0x1
000F412B: jmp       0x1800f4134
000F412D: lea       r14,[rbx+0x100]
000F4134: test      rdi,rdi
000F4137: je        0x1800f4205
000F413D: lea       rdx,[rip+0x10b72bc]        # 0x1811ab400 ; 'ffxGetScratchMemorySizeDX11'
000F4144: mov       rcx,rdi
000F4147: call      QWORD PTR [rip+0x1ef8b]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F414D: lea       rdx,[rip+0x10b72cc]        # 0x1811ab420 ; 'ffxGetDeviceDX11'
000F4154: mov       rcx,rdi
000F4157: mov       QWORD PTR [rbx+0x90],rax
000F415E: call      QWORD PTR [rip+0x1ef74]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F4164: lea       rdx,[rip+0x10b7265]        # 0x1811ab3d0 ; 'ffxGetInterfaceDX11'
000F416B: mov       rcx,rdi
000F416E: mov       QWORD PTR [rbx+0x98],rax
000F4175: call      QWORD PTR [rip+0x1ef5d]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F417B: lea       rdx,[rip+0x10b7266]        # 0x1811ab3e8 ; 'ffxGetCommandListDX11'
000F4182: mov       rcx,rdi
000F4185: mov       QWORD PTR [rbx+0xa0],rax
000F418C: call      QWORD PTR [rip+0x1ef46]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F4192: lea       rdx,[rip+0x10b72df]        # 0x1811ab478 ; 'ffxGetResourceDX11'
000F4199: mov       rcx,rdi
000F419C: mov       QWORD PTR [rbx+0xa8],rax
000F41A3: call      QWORD PTR [rip+0x1ef2f]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F41A9: lea       rdx,[rip+0x10b72e0]        # 0x1811ab490 ; 'ffxGetSurfaceFormatDX11'
000F41B0: mov       rcx,rdi
000F41B3: mov       QWORD PTR [rbx+0xb0],rax
000F41BA: call      QWORD PTR [rip+0x1ef18]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F41C0: lea       rdx,[rip+0x10b7271]        # 0x1811ab438 ; 'ffxGetResourceDescriptionDX11'
000F41C7: mov       rcx,rdi
000F41CA: mov       QWORD PTR [rbx+0xb8],rax
000F41D1: call      QWORD PTR [rip+0x1ef01]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F41D7: mov       QWORD PTR [rbx+0xc0],rax
000F41DE: test      rax,rax
000F41E1: jne       0x1800f4201
000F41E3: lea       rdx,[rip+0x10b726e]        # 0x1811ab458 ; 'GetFfxResourceDescriptionDX11'
000F41EA: mov       rcx,rdi
000F41ED: call      QWORD PTR [rip+0x1eee5]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F41F3: mov       QWORD PTR [rbx+0xc0],rax
000F41FA: lea       r14,[rbx+0x100]
000F4201: mov       BYTE PTR [r14],0x1
000F4205: test      rbp,rbp
000F4208: je        0x1800f42b3
000F420E: lea       rdx,[rip+0x10b72d3]        # 0x1811ab4e8 ; 'ffxFsr3ContextCreate'
000F4215: mov       rcx,rbp
000F4218: call      QWORD PTR [rip+0x1eeba]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F421E: lea       rdx,[rip+0x10b72db]        # 0x1811ab500 ; 'ffxFsr3ContextDestroy'
000F4225: mov       rcx,rbp
000F4228: mov       QWORD PTR [rbx+0xc8],rax
000F422F: call      QWORD PTR [rip+0x1eea3]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F4235: lea       rdx,[rip+0x10b726c]        # 0x1811ab4a8 ; 'ffxFsr3ContextDispatchUpscale'
000F423C: mov       rcx,rbp
000F423F: mov       QWORD PTR [rbx+0xd0],rax
000F4246: call      QWORD PTR [rip+0x1ee8c]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F424C: lea       rdx,[rip+0x10b7275]        # 0x1811ab4c8 ; 'ffxFsr3ConfigureFrameGeneration'
000F4253: mov       rcx,rbp
000F4256: mov       QWORD PTR [rbx+0xd8],rax
000F425D: call      QWORD PTR [rip+0x1ee75]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F4263: lea       rdx,[rip+0x10b72f6]        # 0x1811ab560 ; 'ffxFsr3DispatchFrameGeneration'
000F426A: mov       rcx,rbp
000F426D: mov       QWORD PTR [rbx+0xe0],rax
000F4274: call      QWORD PTR [rip+0x1ee5e]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F427A: lea       rdx,[rip+0x10b72ff]        # 0x1811ab580 ; 'ffxFsr3ContextGenerateReactiveMask'
000F4281: mov       rcx,rbp
000F4284: mov       QWORD PTR [rbx+0xe8],rax
000F428B: call      QWORD PTR [rip+0x1ee47]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F4291: lea       rdx,[rip+0x10b7280]        # 0x1811ab518 ; 'ffxFsr3ContextDispatchFrameGenerationPrepare'
000F4298: mov       rcx,rbp
000F429B: mov       QWORD PTR [rbx+0xf0],rax
000F42A2: call      QWORD PTR [rip+0x1ee30]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000F42A8: mov       QWORD PTR [rbx+0xf8],rax
000F42AF: mov       BYTE PTR [r14],0x1
000F42B3: mov       r14,QWORD PTR [rsp+0x40]
000F42B8: test      rsi,rsi
000F42BB: mov       rsi,QWORD PTR [rsp+0x38]
000F42C0: mov       rbp,QWORD PTR [rsp+0x30]
000F42C5: jne       0x1800f42d0
000F42C7: test      rdi,rdi
000F42CA: jne       0x1800f42d0
000F42CC: xor       al,al
000F42CE: jmp       0x1800f42d2
000F42D0: mov       al,0x1
000F42D2: test      rdi,rdi
000F42D5: mov       BYTE PTR [rbx+0x101],al
000F42DB: lea       rcx,[rip+0x10b7266]        # 0x1811ab548 ; 'FSR3Interface Inited!'
000F42E2: setne     al
000F42E5: mov       BYTE PTR [rbx+0x102],al
000F42EB: mov       rbx,QWORD PTR [rsp+0x48]
000F42F0: add       rsp,0x20
000F42F4: pop       rdi
000F42F5: jmp       0x1800fbb40
