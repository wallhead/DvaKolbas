; amd_fidelityfx_framegeneration_dx12.dll SHA256=3f5e674a59b400756e98ef31fd583a4eb08ad567ec8dee362886bc96e55ed347
; ImageBase=0x180000000, RVAs in left column
; range 0xF4050..0xF4B16; ffxProvider_Fsr3FrameGeneration::Dispatch
000F4050: mov       QWORD PTR [rsp+0x8],rbx
000F4055: push      rbp
000F4056: push      rsi
000F4057: push      rdi
000F4058: push      r14
000F405A: push      r15
000F405C: lea       rbp,[rsp-0x1b0]
000F4064: sub       rsp,0x2b0
000F406B: mov       rax,QWORD PTR [rip+0x2379f8e]        # 0x18246e000 ; __security_cookie
000F4072: xor       rax,rsp
000F4075: mov       QWORD PTR [rbp+0x1a0],rax
000F407C: mov       rbx,r8
000F407F: test      rdx,rdx
000F4082: je        0x1800f4aeb
000F4088: mov       r14,QWORD PTR [rdx]
000F408B: test      r14,r14
000F408E: je        0x1800f4aeb
000F4094: mov       rcx,QWORD PTR [r8]
000F4097: mov       rax,rcx
000F409A: sub       rax,0x20003
000F40A0: je        0x1800f4591
000F40A6: sub       rax,0x1
000F40AA: je        0x1800f40b6
000F40AC: cmp       rax,0x8
000F40B0: jne       0x1800f4aeb
000F40B6: mov       eax,DWORD PTR [r8+0x10]
000F40BA: lea       rdx,[r8+0x10]
000F40BE: movups    xmm0,XMMWORD PTR [rbx]
000F40C1: and       eax,0x1
000F40C4: imul      r8,rax,0xe8
000F40CB: cmp       rcx,0x20004
000F40D2: jne       0x1800f4230
000F40D8: lea       rsi,[r14+0x100268]
000F40DF: mov       rcx,rbx
000F40E2: add       rsi,r8
000F40E5: xor       dil,dil
000F40E8: movups    XMMWORD PTR [rsi],xmm0
000F40EB: movups    xmm1,XMMWORD PTR [rbx+0x10]
000F40EF: movups    XMMWORD PTR [rsi+0x10],xmm1
000F40F3: movups    xmm0,XMMWORD PTR [rbx+0x20]
000F40F7: movups    XMMWORD PTR [rsi+0x20],xmm0
000F40FB: movups    xmm1,XMMWORD PTR [rbx+0x30]
000F40FF: movups    XMMWORD PTR [rsi+0x30],xmm1
000F4103: movups    xmm0,XMMWORD PTR [rbx+0x40]
000F4107: movups    XMMWORD PTR [rsi+0x40],xmm0
000F410B: movups    xmm1,XMMWORD PTR [rbx+0x50]
000F410F: movups    XMMWORD PTR [rsi+0x50],xmm1
000F4113: movups    xmm0,XMMWORD PTR [rbx+0x60]
000F4117: movups    XMMWORD PTR [rsi+0x60],xmm0
000F411B: movups    xmm0,XMMWORD PTR [rbx+0x70]
000F411F: movups    XMMWORD PTR [rsi+0x70],xmm0
000F4123: movups    xmm1,XMMWORD PTR [rbx+0x80]
000F412A: movups    XMMWORD PTR [rsi+0x80],xmm1
000F4131: movups    xmm0,XMMWORD PTR [rbx+0x90]
000F4138: movups    XMMWORD PTR [rsi+0x90],xmm0
000F413F: movups    xmm1,XMMWORD PTR [rbx+0xa0]
000F4146: movups    XMMWORD PTR [rsi+0xa0],xmm1
000F414D: mov       rax,QWORD PTR [rbx+0xb0]
000F4154: mov       QWORD PTR [rsi+0xb0],rax
000F415B: mov       BYTE PTR [rsi+0x44],0x0
000F415F: nop       
000F4160: cmp       QWORD PTR [rcx],0x2000a
000F4167: jne       0x1800f41c4
000F4169: movsd     xmm0,QWORD PTR [rcx+0x10]
000F416E: mov       dil,0x1
000F4171: movsd     QWORD PTR [rsi+0xb8],xmm0
000F4179: mov       eax,DWORD PTR [rcx+0x18]
000F417C: mov       DWORD PTR [rsi+0xc0],eax
000F4182: movsd     xmm0,QWORD PTR [rcx+0x1c]
000F4187: movsd     QWORD PTR [rsi+0xc4],xmm0
000F418F: mov       eax,DWORD PTR [rcx+0x24]
000F4192: mov       DWORD PTR [rsi+0xcc],eax
000F4198: movsd     xmm0,QWORD PTR [rcx+0x28]
000F419D: movsd     QWORD PTR [rsi+0xd0],xmm0
000F41A5: mov       eax,DWORD PTR [rcx+0x30]
000F41A8: mov       DWORD PTR [rsi+0xd8],eax
000F41AE: movsd     xmm0,QWORD PTR [rcx+0x34]
000F41B3: movsd     QWORD PTR [rsi+0xdc],xmm0
000F41BB: mov       eax,DWORD PTR [rcx+0x3c]
000F41BE: mov       DWORD PTR [rsi+0xe4],eax
000F41C4: mov       rcx,QWORD PTR [rcx+0x8]
000F41C8: test      rcx,rcx
000F41CB: jne       0x1800f4160
000F41CD: cmp       BYTE PTR [r14+0x1001f9],cl
000F41D4: je        0x1800f42c9
000F41DA: cmp       BYTE PTR [rip+0x237e1ca],cl        # 0x1824723aa ; bOnce
000F41E0: jne       0x1800f41fa
000F41E2: lea       rdx,[rip+0x23727a7]        # 0x182466990 ; 'ffxDispatchDescFrameGenerationPrepare is deprecated, update to ffxDispatchDescFrameGenerationPrepareV2.'
000F41E9: mov       ecx,0x1
000F41EE: call      0x1800120a0 ; ffxPrintMessage
000F41F3: mov       BYTE PTR [rip+0x237e1b0],0x1        # 0x1824723aa ; bOnce
000F41FA: cmp       BYTE PTR [rbx+0x44],0x0
000F41FE: je        0x1800f4211
000F4200: lea       rdx,[rip+0x2372859]        # 0x182466a60 ; 'ffxDispatchDescFrameGenerationPrepare::unused_reset was never implemented and will be ignored, update to ffxDispatchDescFrameGenerationPrepareV2::reset.'
000F4207: mov       ecx,0x1
000F420C: call      0x1800120a0 ; ffxPrintMessage
000F4211: test      dil,dil
000F4214: jne       0x1800f42c9
000F421A: lea       rdx,[rip+0x237310f]        # 0x182467330 ; 'ffxDispatchDescFrameGenerationPrepareCameraInfo is not linked to ffxDispatchDescFrameGenerationPrepare. Camera view matrix data is a prerequisite for MLFI provider enablement.'
000F4221: mov       ecx,0x1
000F4226: call      0x1800120a0 ; ffxPrintMessage
000F422B: jmp       0x1800f42c9
000F4230: lea       rcx,[r14+0x100268]
000F4237: add       rcx,r8
000F423A: movups    XMMWORD PTR [rcx],xmm0
000F423D: movups    xmm1,XMMWORD PTR [rbx+0x10]
000F4241: movups    XMMWORD PTR [rcx+0x10],xmm1
000F4245: movups    xmm0,XMMWORD PTR [rbx+0x20]
000F4249: movups    XMMWORD PTR [rcx+0x20],xmm0
000F424D: movups    xmm1,XMMWORD PTR [rbx+0x30]
000F4251: movups    XMMWORD PTR [rcx+0x30],xmm1
000F4255: movups    xmm0,XMMWORD PTR [rbx+0x40]
000F4259: movups    XMMWORD PTR [rcx+0x40],xmm0
000F425D: movups    xmm1,XMMWORD PTR [rbx+0x50]
000F4261: movups    XMMWORD PTR [rcx+0x50],xmm1
000F4265: movups    xmm0,XMMWORD PTR [rbx+0x60]
000F4269: movups    XMMWORD PTR [rcx+0x60],xmm0
000F426D: sub       rcx,0xffffffffffffff80
000F4271: movups    xmm0,XMMWORD PTR [rbx+0x70]
000F4275: sub       rbx,0xffffffffffffff80
000F4279: movups    XMMWORD PTR [rcx-0x10],xmm0
000F427D: movups    xmm1,XMMWORD PTR [rbx]
000F4280: movups    XMMWORD PTR [rcx],xmm1
000F4283: movups    xmm0,XMMWORD PTR [rbx+0x10]
000F4287: movups    XMMWORD PTR [rcx+0x10],xmm0
000F428B: movups    xmm1,XMMWORD PTR [rbx+0x20]
000F428F: movups    XMMWORD PTR [rcx+0x20],xmm1
000F4293: movups    xmm0,XMMWORD PTR [rbx+0x30]
000F4297: movups    XMMWORD PTR [rcx+0x30],xmm0
000F429B: movups    xmm1,XMMWORD PTR [rbx+0x40]
000F429F: movups    XMMWORD PTR [rcx+0x40],xmm1
000F42A3: movups    xmm0,XMMWORD PTR [rbx+0x50]
000F42A7: movups    XMMWORD PTR [rcx+0x50],xmm0
000F42AB: mov       rax,QWORD PTR [rbx+0x60]
000F42AF: mov       QWORD PTR [rcx+0x60],rax
000F42B3: mov       eax,DWORD PTR [rdx]
000F42B5: and       eax,0x1
000F42B8: imul      rsi,rax,0xe8
000F42BF: add       rsi,0x100268
000F42C6: add       rsi,r14
000F42C9: mov       edi,DWORD PTR [r14+0x1001ec]
000F42D0: lea       rcx,[rbp+0x28]
000F42D4: xorps     xmm0,xmm0
000F42D7: xor       eax,eax
000F42D9: dec       edi
000F42DB: mov       DWORD PTR [rbp-0x7c],eax
000F42DE: and       edi,0x1
000F42E1: mov       DWORD PTR [rbp+0x1c],eax
000F42E4: xor       edx,edx
000F42E6: mov       DWORD PTR [r14+0x1001ec],edi
000F42ED: mov       r8d,0xc0
000F42F3: movups    XMMWORD PTR [rbp-0x44],xmm0
000F42F7: movups    XMMWORD PTR [rbp-0x34],xmm0
000F42FB: movups    XMMWORD PTR [rbp-0x24],xmm0
000F42FF: movups    XMMWORD PTR [rbp-0x14],xmm0
000F4303: movups    XMMWORD PTR [rbp-0x4],xmm0
000F4307: movups    XMMWORD PTR [rbp+0xc],xmm0
000F430B: call      0x1801056da ; memset
000F4310: mov       eax,DWORD PTR [rsi+0x18]
000F4313: lea       r8d,[rdi*2+0x5]
000F431B: mov       DWORD PTR [rbp-0x80],eax
000F431E: lea       rdx,[r14+0xd8]
000F4325: mov       rax,QWORD PTR [rsi+0x20]
000F4329: lea       rcx,[rsp+0x20]
000F432E: mov       QWORD PTR [rbp-0x78],rax
000F4332: add       r8d,edi
000F4335: mov       eax,DWORD PTR [rsi+0x28]
000F4338: mov       DWORD PTR [rbp-0x70],eax
000F433B: mov       eax,DWORD PTR [rsi+0x2c]
000F433E: mov       DWORD PTR [rbp-0x6c],eax
000F4341: movss     xmm0,DWORD PTR [rsi+0x30]
000F4346: movss     DWORD PTR [rbp-0x68],xmm0
000F434B: movss     xmm1,DWORD PTR [rsi+0x34]
000F4350: movss     DWORD PTR [rbp-0x64],xmm1
000F4355: movss     xmm0,DWORD PTR [rsi+0x38]
000F435A: movss     DWORD PTR [rbp-0x60],xmm0
000F435F: movss     xmm1,DWORD PTR [rsi+0x3c]
000F4364: movss     DWORD PTR [rbp-0x5c],xmm1
000F4369: movss     xmm0,DWORD PTR [rsi+0x40]
000F436E: movss     DWORD PTR [rbp-0x58],xmm0
000F4373: movss     xmm1,DWORD PTR [rsi+0x48]
000F4378: movss     DWORD PTR [rbp-0x54],xmm1
000F437D: movss     xmm0,DWORD PTR [rsi+0x4c]
000F4382: movss     DWORD PTR [rbp-0x50],xmm0
000F4387: movss     xmm1,DWORD PTR [rsi+0x54]
000F438C: movss     DWORD PTR [rbp-0x4c],xmm1
000F4391: movss     xmm0,DWORD PTR [rsi+0x50]
000F4396: movss     DWORD PTR [rbp-0x48],xmm0
000F439B: movups    xmm1,XMMWORD PTR [rsi+0x58]
000F439F: movaps    XMMWORD PTR [rbp-0x40],xmm1
000F43A3: movups    xmm0,XMMWORD PTR [rsi+0x68]
000F43A7: movaps    XMMWORD PTR [rbp-0x30],xmm0
000F43AB: movups    xmm1,XMMWORD PTR [rsi+0x78]
000F43AF: movaps    XMMWORD PTR [rbp-0x20],xmm1
000F43B3: movups    xmm0,XMMWORD PTR [rsi+0x88]
000F43BA: movaps    XMMWORD PTR [rbp-0x10],xmm0
000F43BE: movups    xmm1,XMMWORD PTR [rsi+0x98]
000F43C5: movaps    XMMWORD PTR [rbp+0x0],xmm1
000F43C9: movups    xmm0,XMMWORD PTR [rsi+0xa8]
000F43D0: movaps    XMMWORD PTR [rbp+0x10],xmm0
000F43D4: mov       rax,QWORD PTR [rsi+0x10]
000F43D8: mov       QWORD PTR [rbp+0x20],rax
000F43DC: mov       rax,QWORD PTR [r14+0x110]
000F43E3: mov       r8d,DWORD PTR [r14+r8*4+0x1001a8]
000F43EB: call      rax
000F43ED: lea       rdx,[r14+0xd8]
000F43F4: lea       rcx,[rsp+0x20]
000F43F9: movups    xmm0,XMMWORD PTR [rax]
000F43FC: movups    XMMWORD PTR [rbp+0x28],xmm0
000F4400: movups    xmm1,XMMWORD PTR [rax+0x10]
000F4404: movups    XMMWORD PTR [rbp+0x38],xmm1
000F4408: movups    xmm0,XMMWORD PTR [rax+0x20]
000F440C: mov       eax,DWORD PTR [r14+0x1001ec]
000F4413: add       eax,0x2
000F4416: movups    XMMWORD PTR [rbp+0x48],xmm0
000F441A: lea       r8d,[rax+rax*2]
000F441E: mov       rax,QWORD PTR [r14+0x110]
000F4425: mov       r8d,DWORD PTR [r14+r8*4+0x1001a8]
000F442D: call      rax
000F442F: movups    xmm0,XMMWORD PTR [rax]
000F4432: movups    XMMWORD PTR [rbp+0x58],xmm0
000F4436: movups    xmm1,XMMWORD PTR [rax+0x10]
000F443A: lea       rdx,[r14+0xd8]
000F4441: movups    XMMWORD PTR [rbp+0x68],xmm1
000F4445: movups    xmm0,XMMWORD PTR [rax+0x20]
000F4449: mov       eax,DWORD PTR [r14+0x1001ec]
000F4450: movups    XMMWORD PTR [rbp+0x78],xmm0
000F4454: lea       ecx,[rax*2+0x7]
000F445B: add       ecx,eax
000F445D: mov       rax,QWORD PTR [r14+0x110]
000F4464: mov       r8d,DWORD PTR [r14+rcx*4+0x1001a8]
000F446C: lea       rcx,[rsp+0x20]
000F4471: call      rax
000F4473: cmp       BYTE PTR [r14+0x1001f9],0x0
000F447B: movups    xmm0,XMMWORD PTR [rax]
000F447E: movups    XMMWORD PTR [rbp+0x88],xmm0
000F4485: movups    xmm1,XMMWORD PTR [rax+0x10]
000F4489: movups    XMMWORD PTR [rbp+0x98],xmm1
000F4490: movups    xmm0,XMMWORD PTR [rax+0x20]
000F4494: movups    XMMWORD PTR [rbp+0xa8],xmm0
000F449B: movsd     xmm0,QWORD PTR [rsi+0xb8]
000F44A3: movsd     QWORD PTR [rbp+0xb8],xmm0
000F44AB: mov       eax,DWORD PTR [rsi+0xc0]
000F44B1: mov       DWORD PTR [rbp+0xc0],eax
000F44B7: movsd     xmm0,QWORD PTR [rsi+0xc4]
000F44BF: movsd     QWORD PTR [rbp+0xc4],xmm0
000F44C7: mov       eax,DWORD PTR [rsi+0xcc]
000F44CD: mov       DWORD PTR [rbp+0xcc],eax
000F44D3: movsd     xmm0,QWORD PTR [rsi+0xd0]
000F44DB: movsd     QWORD PTR [rbp+0xd0],xmm0
000F44E3: mov       eax,DWORD PTR [rsi+0xd8]
000F44E9: mov       DWORD PTR [rbp+0xd8],eax
000F44EF: movsd     xmm0,QWORD PTR [rsi+0xdc]
000F44F7: movsd     QWORD PTR [rbp+0xdc],xmm0
000F44FF: mov       eax,DWORD PTR [rsi+0xe4]
000F4505: mov       DWORD PTR [rbp+0xe4],eax
000F450B: je        0x1800f456f
000F450D: mov       rdx,QWORD PTR [rip+0x2372fc4]        # 0x1824674d8 ; zeroVector3D
000F4514: cmp       QWORD PTR [rsi+0xb8],rdx
000F451B: jne       0x1800f456f
000F451D: mov       eax,DWORD PTR [rip+0x2372fbd]        # 0x1824674e0
000F4523: cmp       DWORD PTR [rsi+0xc0],eax
000F4529: jne       0x1800f456f
000F452B: cmp       QWORD PTR [rsi+0xc4],rdx
000F4532: jne       0x1800f456f
000F4534: cmp       DWORD PTR [rsi+0xcc],eax
000F453A: jne       0x1800f456f
000F453C: cmp       QWORD PTR [rsi+0xd0],rdx
000F4543: jne       0x1800f456f
000F4545: cmp       DWORD PTR [rsi+0xd8],eax
000F454B: jne       0x1800f456f
000F454D: cmp       QWORD PTR [rsi+0xdc],rdx
000F4554: jne       0x1800f456f
000F4556: cmp       DWORD PTR [rsi+0xe4],eax
000F455C: jne       0x1800f456f
000F455E: lea       rdx,[rip+0x23727ab]        # 0x182466d10 ; 'Camera view matrix parameters (cameraPosition, cameraUp, cameraRight, cameraForward) are all zero vectors, indicating they remain at their default initialized values. These parameters must be properly set by the application for optimal MLFI quality.'
000F4565: mov       ecx,0x1
000F456A: call      0x1800120a0 ; ffxPrintMessage
000F456F: lea       rcx,[r14+0x801a8]
000F4576: lea       rdx,[rbp-0x80]
000F457A: call      0x1800f8c40 ; ffxFrameInterpolationPrepare
000F457F: test      eax,eax
000F4581: je        0x1800f4ae7
000F4587: mov       eax,0x3
000F458C: jmp       0x1800f4af0
000F4591: mov       rcx,QWORD PTR [r8+0x130]
000F4598: mov       rax,rcx
000F459B: mov       rdx,rcx
000F459E: and       eax,0x1
000F45A1: imul      rdi,rax,0xe8
000F45A8: mov       rax,QWORD PTR [r14+0x100480]
000F45AF: add       rdi,r14
000F45B2: sub       rdx,rax
000F45B5: cmp       rcx,rax
000F45B8: jb        0x1800f45cd
000F45BA: cmp       rdx,0x1
000F45BE: ja        0x1800f45cd
000F45C0: cmp       rcx,QWORD PTR [r14+0x100478]
000F45C7: jne       0x1800f45cd
000F45C9: xor       al,al
000F45CB: jmp       0x1800f45cf
000F45CD: mov       al,0x1
000F45CF: cmp       BYTE PTR [r8+0x10c],0x0
000F45D7: jne       0x1800f45eb
000F45D9: cmp       BYTE PTR [rdi+0x1002ac],0x0
000F45E0: jne       0x1800f45eb
000F45E2: test      al,al
000F45E4: jne       0x1800f45eb
000F45E6: xor       sil,sil
000F45E9: jmp       0x1800f45ee
000F45EB: mov       sil,0x1
000F45EE: xor       edx,edx
000F45F0: lea       rcx,[rbp-0x80]
000F45F4: mov       r8d,0xa8
000F45FA: call      0x1801056da ; memset
000F45FF: cmp       QWORD PTR [r14+0x100200],0x0
000F4607: movups    xmm0,XMMWORD PTR [rbx+0x18]
000F460B: mov       rax,QWORD PTR [rbx+0x10]
000F460F: movups    xmm1,XMMWORD PTR [rbx+0x28]
000F4613: mov       QWORD PTR [rbp-0x80],rax
000F4617: movups    XMMWORD PTR [rbp-0x78],xmm0
000F461B: movups    xmm0,XMMWORD PTR [rbx+0x38]
000F461F: movups    XMMWORD PTR [rbp-0x68],xmm1
000F4623: movups    XMMWORD PTR [rbp-0x58],xmm0
000F4627: je        0x1800f464d
000F4629: movups    xmm0,XMMWORD PTR [r14+0x100200]
000F4631: movups    xmm1,XMMWORD PTR [r14+0x100210]
000F4639: movups    XMMWORD PTR [rbp-0x78],xmm0
000F463D: movups    xmm0,XMMWORD PTR [r14+0x100220]
000F4645: movups    XMMWORD PTR [rbp-0x68],xmm1
000F4649: movups    XMMWORD PTR [rbp-0x58],xmm0
000F464D: mov       eax,DWORD PTR [rbx+0x110]
000F4653: lea       r15,[r14+0xd8]
000F465A: movss     xmm0,DWORD PTR [rbx+0x114]
000F4662: lea       rcx,[rsp+0x20]
000F4667: movss     xmm1,DWORD PTR [rbx+0x118]
000F466F: mov       rdx,r15
000F4672: mov       r8d,DWORD PTR [r14+0x1001ac]
000F4679: mov       DWORD PTR [rbp+0x1c],eax
000F467C: mov       rax,QWORD PTR [r14+0x110]
000F4683: movss     DWORD PTR [rbp+0x20],xmm0
000F4688: movss     DWORD PTR [rbp+0x24],xmm1
000F468D: mov       BYTE PTR [rbp+0x18],sil
000F4691: call      rax
000F4693: mov       r8d,DWORD PTR [r14+0x1001b0]
000F469A: lea       rcx,[rsp+0x50]
000F469F: mov       rdx,r15
000F46A2: movups    xmm0,XMMWORD PTR [rax]
000F46A5: movups    XMMWORD PTR [rbp-0x48],xmm0
000F46A9: movups    xmm1,XMMWORD PTR [rax+0x10]
000F46AD: movups    XMMWORD PTR [rbp-0x38],xmm1
000F46B1: movups    xmm0,XMMWORD PTR [rax+0x20]
000F46B5: mov       rax,QWORD PTR [r14+0x110]
000F46BC: movups    XMMWORD PTR [rbp-0x28],xmm0
000F46C0: call      rax
000F46C2: lea       rcx,[r14+0x1a8]
000F46C9: movups    xmm0,XMMWORD PTR [rax]
000F46CC: movups    XMMWORD PTR [rbp-0x18],xmm0
000F46D0: movups    xmm1,XMMWORD PTR [rax+0x10]
000F46D4: movups    XMMWORD PTR [rbp-0x8],xmm1
000F46D8: movups    xmm0,XMMWORD PTR [rax+0x20]
000F46DC: movups    XMMWORD PTR [rbp+0x8],xmm0
000F46E0: test      rcx,rcx
000F46E3: je        0x1800f4587
000F46E9: cmp       QWORD PTR [rbp-0x80],0x0
000F46EE: je        0x1800f4587
000F46F4: cmp       QWORD PTR [rbp-0x78],0x0
000F46F9: je        0x1800f4587
000F46FF: cmp       DWORD PTR [rbp-0x70],0x2
000F4703: jne       0x1800f4587
000F4709: cmp       QWORD PTR [rbp-0x48],0x0
000F470E: je        0x1800f4587
000F4714: mov       rax,QWORD PTR [rax]
000F4717: test      rax,rax
000F471A: je        0x1800f4587
000F4720: cmp       QWORD PTR [rcx+0x108],0x0
000F4728: je        0x1800f4587
000F472E: mov       eax,DWORD PTR [rcx+0xd4]
000F4734: cmp       DWORD PTR [rbp-0x68],eax
000F4737: ja        0x1800f4587
000F473D: mov       eax,DWORD PTR [rcx+0xd8]
000F4743: cmp       DWORD PTR [rbp-0x64],eax
000F4746: ja        0x1800f4587
000F474C: lea       rdx,[rbp-0x80]
000F4750: call      0x1800fc940 ; dispatch
000F4755: test      eax,eax
000F4757: jne       0x1800f4587
000F475D: xorps     xmm0,xmm0
000F4760: lea       rcx,[rbp+0xe0]
000F4767: xor       eax,eax
000F4769: movaps    XMMWORD PTR [rbp+0x40],xmm0
000F476D: xor       edx,edx
000F476F: mov       QWORD PTR [rbp-0x80],rax
000F4773: mov       r8d,0xc0
000F4779: movaps    XMMWORD PTR [rbp+0x50],xmm0
000F477D: movaps    XMMWORD PTR [rbp+0x60],xmm0
000F4781: movaps    XMMWORD PTR [rbp+0x70],xmm0
000F4785: movaps    XMMWORD PTR [rbp+0x80],xmm0
000F478C: movaps    XMMWORD PTR [rbp+0x90],xmm0
000F4793: mov       QWORD PTR [rbp+0xa0],rax
000F479A: mov       WORD PTR [rbp+0xc9],ax
000F47A1: mov       BYTE PTR [rbp+0xcb],al
000F47A7: call      0x1801056da ; memset
000F47AC: mov       rax,QWORD PTR [rbx+0x10]
000F47B0: lea       rcx,[rsp+0x50]
000F47B5: movups    xmm0,XMMWORD PTR [rbx+0x18]
000F47B9: mov       r8d,DWORD PTR [r14+0x1001ac]
000F47C0: mov       rdx,r15
000F47C3: movups    xmm1,XMMWORD PTR [rbx+0x28]
000F47C7: mov       QWORD PTR [rbp-0x78],rax
000F47CB: mov       eax,DWORD PTR [rbx+0x28]
000F47CE: movaps    XMMWORD PTR [rbp-0x60],xmm0
000F47D2: movups    xmm0,XMMWORD PTR [rbx+0x38]
000F47D6: mov       DWORD PTR [rbp-0x70],eax
000F47D9: mov       eax,DWORD PTR [rbx+0x2c]
000F47DC: movaps    XMMWORD PTR [rbp-0x40],xmm0
000F47E0: movups    xmm0,XMMWORD PTR [r14+0x100210]
000F47E8: mov       DWORD PTR [rbp-0x6c],eax
000F47EB: mov       eax,DWORD PTR [rdi+0x100290]
000F47F1: movaps    XMMWORD PTR [rbp-0x50],xmm1
000F47F5: movups    xmm1,XMMWORD PTR [r14+0x100200]
000F47FD: mov       DWORD PTR [rbp-0x68],eax
000F4800: mov       eax,DWORD PTR [rdi+0x100294]
000F4806: movaps    XMMWORD PTR [rbp-0x20],xmm0
000F480A: movups    xmm0,XMMWORD PTR [rbx+0x48]
000F480E: mov       DWORD PTR [rbp-0x64],eax
000F4811: mov       rax,QWORD PTR [r14+0x110]
000F4818: movaps    XMMWORD PTR [rbp-0x30],xmm1
000F481C: movups    xmm1,XMMWORD PTR [r14+0x100220]
000F4824: mov       BYTE PTR [rbp+0xc8],sil
000F482B: movaps    XMMWORD PTR [rbp+0x0],xmm0
000F482F: movups    xmm0,XMMWORD PTR [rbx+0x68]
000F4833: movaps    XMMWORD PTR [rbp-0x10],xmm1
000F4837: movups    xmm1,XMMWORD PTR [rbx+0x58]
000F483B: movaps    XMMWORD PTR [rbp+0x20],xmm0
000F483F: movaps    XMMWORD PTR [rbp+0x10],xmm1
000F4843: call      rax
000F4845: mov       r8d,DWORD PTR [r14+0x1001b0]
000F484C: lea       rcx,[rsp+0x50]
000F4851: mov       rdx,r15
000F4854: movups    xmm0,XMMWORD PTR [rax]
000F4857: movaps    XMMWORD PTR [rbp+0x40],xmm0
000F485B: movups    xmm1,XMMWORD PTR [rax+0x10]
000F485F: movaps    XMMWORD PTR [rbp+0x50],xmm1
000F4863: movups    xmm0,XMMWORD PTR [rax+0x20]
000F4867: mov       rax,QWORD PTR [r14+0x110]
000F486E: movaps    XMMWORD PTR [rbp+0x60],xmm0
000F4872: call      rax
000F4874: movss     xmm2,DWORD PTR [rip+0x2375310]        # 0x182469b8c ; __real@3f800000
000F487C: movups    xmm0,XMMWORD PTR [rax]
000F487F: movaps    XMMWORD PTR [rbp+0x70],xmm0
000F4883: movups    xmm1,XMMWORD PTR [rax+0x10]
000F4887: movaps    XMMWORD PTR [rbp+0x80],xmm1
000F488E: xorps     xmm1,xmm1
000F4891: movups    xmm0,XMMWORD PTR [rax+0x20]
000F4895: mov       eax,DWORD PTR [rbp-0x70]
000F4898: mov       DWORD PTR [rbp+0xb0],0x8
000F48A2: movaps    XMMWORD PTR [rbp+0x90],xmm0
000F48A9: movaps    xmm0,xmm2
000F48AC: cvtsi2ss  xmm1,rax
000F48B1: mov       eax,DWORD PTR [rbp-0x6c]
000F48B4: divss     xmm0,xmm1
000F48B8: xorps     xmm1,xmm1
000F48BB: cvtsi2ss  xmm1,rax
000F48C0: movss     DWORD PTR [rbp+0xa8],xmm0
000F48C8: movss     xmm0,DWORD PTR [rdi+0x1002a8]
000F48D0: divss     xmm2,xmm1
000F48D4: movss     DWORD PTR [rbp+0xac],xmm2
000F48DC: mov       eax,DWORD PTR [r14+0x1001ec]
000F48E3: mov       rdx,r15
000F48E6: movups    xmm1,XMMWORD PTR [rdi+0x1002b0]
000F48ED: movss     DWORD PTR [rbp+0xc4],xmm0
000F48F5: lea       ecx,[rax*2+0x5]
000F48FC: add       ecx,eax
000F48FE: mov       rax,QWORD PTR [r14+0x110]
000F4905: movups    XMMWORD PTR [rbp+0xb4],xmm1
000F490C: mov       r8d,DWORD PTR [r14+rcx*4+0x1001a8]
000F4914: lea       rcx,[rsp+0x50]
000F4919: call      rax
000F491B: mov       rdx,r15
000F491E: lea       rcx,[rsp+0x50]
000F4923: movups    xmm0,XMMWORD PTR [rax]
000F4926: movaps    XMMWORD PTR [rbp+0xe0],xmm0
000F492D: movups    xmm1,XMMWORD PTR [rax+0x10]
000F4931: movaps    XMMWORD PTR [rbp+0xf0],xmm1
000F4938: movups    xmm0,XMMWORD PTR [rax+0x20]
000F493C: mov       eax,DWORD PTR [r14+0x1001ec]
000F4943: add       eax,0x2
000F4946: movaps    XMMWORD PTR [rbp+0x100],xmm0
000F494D: lea       r8d,[rax+rax*2]
000F4951: mov       rax,QWORD PTR [r14+0x110]
000F4958: mov       r8d,DWORD PTR [r14+r8*4+0x1001a8]
000F4960: call      rax
000F4962: mov       rdx,r15
000F4965: movups    xmm0,XMMWORD PTR [rax]
000F4968: movaps    XMMWORD PTR [rbp+0x110],xmm0
000F496F: movups    xmm1,XMMWORD PTR [rax+0x10]
000F4973: movaps    XMMWORD PTR [rbp+0x120],xmm1
000F497A: movups    xmm0,XMMWORD PTR [rax+0x20]
000F497E: mov       eax,DWORD PTR [r14+0x1001ec]
000F4985: movaps    XMMWORD PTR [rbp+0x130],xmm0
000F498C: lea       ecx,[rax*2+0x7]
000F4993: add       ecx,eax
000F4995: mov       rax,QWORD PTR [r14+0x110]
000F499C: mov       r8d,DWORD PTR [r14+rcx*4+0x1001a8]
000F49A4: lea       rcx,[rsp+0x50]
000F49A9: call      rax
000F49AB: mov       ecx,DWORD PTR [rbx+0x128]
000F49B1: movups    xmm0,XMMWORD PTR [rax]
000F49B4: movaps    XMMWORD PTR [rbp+0x140],xmm0
000F49BB: movups    xmm1,XMMWORD PTR [rax+0x10]
000F49BF: movaps    XMMWORD PTR [rbp+0x150],xmm1
000F49C6: movups    xmm0,XMMWORD PTR [rax+0x20]
000F49CA: movaps    XMMWORD PTR [rbp+0x160],xmm0
000F49D1: test      ecx,ecx
000F49D3: jne       0x1800f49f1
000F49D5: cmp       DWORD PTR [rbx+0x124],ecx
000F49DB: jne       0x1800f49f1
000F49DD: xor       eax,eax
000F49DF: mov       QWORD PTR [rbp+0x30],rax
000F49E3: mov       eax,DWORD PTR [rbx+0x28]
000F49E6: mov       DWORD PTR [rbp+0x38],eax
000F49E9: mov       eax,DWORD PTR [rbx+0x2c]
000F49EC: mov       DWORD PTR [rbp+0x3c],eax
000F49EF: jmp       0x1800f4a0f
000F49F1: mov       eax,DWORD PTR [rbx+0x120]
000F49F7: mov       DWORD PTR [rbp+0x34],eax
000F49FA: mov       eax,DWORD PTR [rbx+0x11c]
000F4A00: mov       DWORD PTR [rbp+0x30],eax
000F4A03: mov       eax,DWORD PTR [rbx+0x124]
000F4A09: mov       DWORD PTR [rbp+0x38],eax
000F4A0C: mov       DWORD PTR [rbp+0x3c],ecx
000F4A0F: mov       ecx,DWORD PTR [r14+0x100264]
000F4A16: mov       eax,DWORD PTR [rbp-0x80]
000F4A19: test      cl,0x1
000F4A1C: je        0x1800f4a24
000F4A1E: or        eax,0x1
000F4A21: mov       DWORD PTR [rbp-0x80],eax
000F4A24: test      cl,0x2
000F4A27: je        0x1800f4a2f
000F4A29: or        eax,0x2
000F4A2C: mov       DWORD PTR [rbp-0x80],eax
000F4A2F: test      cl,0x4
000F4A32: je        0x1800f4a3a
000F4A34: or        eax,0x4
000F4A37: mov       DWORD PTR [rbp-0x80],eax
000F4A3A: test      cl,0x20
000F4A3D: je        0x1800f4a45
000F4A3F: or        eax,0x10
000F4A42: mov       DWORD PTR [rbp-0x80],eax
000F4A45: test      cl,0x40
000F4A48: je        0x1800f4a50
000F4A4A: or        eax,0x20
000F4A4D: mov       DWORD PTR [rbp-0x80],eax
000F4A50: cmp       QWORD PTR [r14+0x100230],0x0
000F4A58: mov       eax,DWORD PTR [rbx+0x110]
000F4A5E: movss     xmm0,DWORD PTR [rbx+0x114]
000F4A66: movss     xmm1,DWORD PTR [rbx+0x118]
000F4A6E: mov       DWORD PTR [rbp+0xcc],eax
000F4A74: mov       rax,QWORD PTR [rbx+0x130]
000F4A7B: mov       QWORD PTR [rbp+0xd8],rax
000F4A82: movss     DWORD PTR [rbp+0xd0],xmm0
000F4A8A: movss     DWORD PTR [rbp+0xd4],xmm1
000F4A92: je        0x1800f4ac1
000F4A94: movups    xmm0,XMMWORD PTR [r14+0x100230]
000F4A9C: movups    xmm1,XMMWORD PTR [r14+0x100240]
000F4AA4: movaps    XMMWORD PTR [rbp+0x170],xmm0
000F4AAB: movups    xmm0,XMMWORD PTR [r14+0x100250]
000F4AB3: movaps    XMMWORD PTR [rbp+0x180],xmm1
000F4ABA: movaps    XMMWORD PTR [rbp+0x190],xmm0
000F4AC1: lea       rcx,[r14+0x801a8]
000F4AC8: lea       rdx,[rbp-0x80]
000F4ACC: call      0x1800f8ec0 ; ffxFrameInterpolationDispatch
000F4AD1: test      eax,eax
000F4AD3: jne       0x1800f4587
000F4AD9: mov       rax,QWORD PTR [rbx+0x130]
000F4AE0: mov       QWORD PTR [r14+0x100480],rax
000F4AE7: xor       eax,eax
000F4AE9: jmp       0x1800f4af0
000F4AEB: mov       eax,0x6
000F4AF0: mov       rcx,QWORD PTR [rbp+0x1a0]
000F4AF7: xor       rcx,rsp
000F4AFA: call      0x180104650 ; __security_check_cookie
000F4AFF: mov       rbx,QWORD PTR [rsp+0x2e0]
000F4B07: add       rsp,0x2b0
000F4B0E: pop       r15
000F4B10: pop       r14
000F4B12: pop       rdi
000F4B13: pop       rsi
000F4B14: pop       rbp
000F4B15: ret       
