; amd_fidelityfx_framegeneration_dx12.dll SHA256=3f5e674a59b400756e98ef31fd583a4eb08ad567ec8dee362886bc96e55ed347
; ImageBase=0x180000000, RVAs in left column
; range 0xA360..0xA5B3; TFrameInterpolationSwapChainDX12<IFrameInterpolationSwapChainDX12,FrameinterpolationPresentInfoExt,FfxFrameGenerationConfig,ffxCallbackDescFrameGenerationPresent,16>::destroyReplacementResources
0000A360: mov       QWORD PTR [rsp+0x8],rbx
0000A365: mov       QWORD PTR [rsp+0x10],rbp
0000A36A: mov       QWORD PTR [rsp+0x18],rsi
0000A36F: push      rdi
0000A370: push      r14
0000A372: push      r15
0000A374: sub       rsp,0x20
0000A378: mov       rbx,rcx
0000A37B: add       rcx,0x17f8
0000A382: call      QWORD PTR [rip+0xfbd88]        # 0x180106110 ; __imp_EnterCriticalSection | KERNEL32.dll!EnterCriticalSection
0000A388: mov       rax,QWORD PTR [rbx]
0000A38B: mov       rcx,rbx
0000A38E: call      QWORD PTR [rax+0x1b8]
0000A394: mov       rbp,QWORD PTR [rbx+0x1a78]
0000A39B: test      rbp,rbp
0000A39E: je        0x18000a3ac
0000A3A0: mov       rax,QWORD PTR [rbx]
0000A3A3: mov       rcx,rbx
0000A3A6: call      QWORD PTR [rax+0x190]
0000A3AC: mov       rax,QWORD PTR [rbx]
0000A3AF: mov       rcx,rbx
0000A3B2: call      QWORD PTR [rax+0x1a0]
0000A3B8: xor       r15d,r15d
0000A3BB: lea       rdi,[rbx+0x1910]
0000A3C2: mov       esi,0x10
0000A3C7: mov       rcx,QWORD PTR [rdi]
0000A3CA: call      0x1800f5050 ; GetResourceGpuMemorySize
0000A3CF: sub       QWORD PTR [rbx+0x1ab0],rax
0000A3D6: mov       rcx,QWORD PTR [rdi]
0000A3D9: test      rcx,rcx
0000A3DC: je        0x18000a3e7
0000A3DE: mov       rax,QWORD PTR [rcx]
0000A3E1: call      QWORD PTR [rax+0x10]
0000A3E4: mov       QWORD PTR [rdi],r15
0000A3E7: mov       QWORD PTR [rdi+0x8],r15
0000A3EB: add       rdi,0x10
0000A3EF: sub       rsi,0x1
0000A3F3: jne       0x18000a3c7
0000A3F5: mov       rcx,QWORD PTR [rbx+0x1858]
0000A3FC: test      rcx,rcx
0000A3FF: je        0x18000a40e
0000A401: mov       rax,QWORD PTR [rcx]
0000A404: call      QWORD PTR [rax+0x10]
0000A407: mov       QWORD PTR [rbx+0x1858],r15
0000A40E: mov       rcx,QWORD PTR [rbx+0x1a10]
0000A415: call      0x1800f5050 ; GetResourceGpuMemorySize
0000A41A: sub       QWORD PTR [rbx+0x1ab0],rax
0000A421: mov       rcx,QWORD PTR [rbx+0x1a10]
0000A428: test      rcx,rcx
0000A42B: je        0x18000a43a
0000A42D: mov       rax,QWORD PTR [rcx]
0000A430: call      QWORD PTR [rax+0x10]
0000A433: mov       QWORD PTR [rbx+0x1a10],r15
0000A43A: mov       QWORD PTR [rbx+0x1a18],r15
0000A441: mov       rcx,QWORD PTR [rbx+0x1a20]
0000A448: call      0x1800f5050 ; GetResourceGpuMemorySize
0000A44D: sub       QWORD PTR [rbx+0x1ab0],rax
0000A454: mov       rcx,QWORD PTR [rbx+0x1a20]
0000A45B: test      rcx,rcx
0000A45E: je        0x18000a46d
0000A460: mov       rax,QWORD PTR [rcx]
0000A463: call      QWORD PTR [rax+0x10]
0000A466: mov       QWORD PTR [rbx+0x1a20],r15
0000A46D: mov       QWORD PTR [rbx+0x1a28],r15
0000A474: mov       rcx,QWORD PTR [rbx+0x1a30]
0000A47B: test      rcx,rcx
0000A47E: je        0x18000a48c
0000A480: call      0x1800f5050 ; GetResourceGpuMemorySize
0000A485: sub       QWORD PTR [rbx+0x1ab0],rax
0000A48C: mov       rcx,QWORD PTR [rbx+0x1a30]
0000A493: test      rcx,rcx
0000A496: je        0x18000a4a5
0000A498: mov       rax,QWORD PTR [rcx]
0000A49B: call      QWORD PTR [rax+0x10]
0000A49E: mov       QWORD PTR [rbx+0x1a30],r15
0000A4A5: mov       QWORD PTR [rbx+0x1a38],r15
0000A4AC: mov       rcx,QWORD PTR [rbx+0x16e0]
0000A4B3: mov       QWORD PTR [rbx+0x1a68],r15
0000A4BA: mov       QWORD PTR [rbx+0x1a70],r15
0000A4C1: mov       DWORD PTR [rbx+0x1a40],r15d
0000A4C8: mov       QWORD PTR [rbx+0x1a48],r15
0000A4CF: mov       QWORD PTR [rbx+0x1868],r15
0000A4D6: mov       QWORD PTR [rbx+0x1870],r15
0000A4DD: test      rcx,rcx
0000A4E0: je        0x18000a4ea
0000A4E2: mov       rax,QWORD PTR [rcx]
0000A4E5: xor       edx,edx
0000A4E7: call      QWORD PTR [rax+0x50]
0000A4EA: mov       rcx,QWORD PTR [rbx+0x16e8]
0000A4F1: test      rcx,rcx
0000A4F4: je        0x18000a503
0000A4F6: mov       rax,QWORD PTR [rcx]
0000A4F9: mov       rdx,QWORD PTR [rbx+0x1868]
0000A500: call      QWORD PTR [rax+0x50]
0000A503: mov       rcx,QWORD PTR [rbx+0x16f0]
0000A50A: test      rcx,rcx
0000A50D: je        0x18000a51c
0000A50F: mov       rax,QWORD PTR [rcx]
0000A512: mov       rdx,QWORD PTR [rbx+0x1a68]
0000A519: call      QWORD PTR [rax+0x50]
0000A51C: mov       rcx,QWORD PTR [rbx+0x16f8]
0000A523: test      rcx,rcx
0000A526: je        0x18000a535
0000A528: mov       rax,QWORD PTR [rcx]
0000A52B: mov       rdx,QWORD PTR [rbx+0x1a68]
0000A532: call      QWORD PTR [rax+0x50]
0000A535: mov       rcx,QWORD PTR [rbx+0x1708]
0000A53C: test      rcx,rcx
0000A53F: je        0x18000a54e
0000A541: mov       rax,QWORD PTR [rcx]
0000A544: mov       rdx,QWORD PTR [rbx+0x1a68]
0000A54B: call      QWORD PTR [rax+0x50]
0000A54E: mov       rcx,QWORD PTR [rbx+0x1700]
0000A555: test      rcx,rcx
0000A558: je        0x18000a567
0000A55A: mov       rax,QWORD PTR [rcx]
0000A55D: mov       rdx,QWORD PTR [rbx+0x1a68]
0000A564: call      QWORD PTR [rax+0x50]
0000A567: mov       BYTE PTR [rbx+0x1878],0x1
0000A56E: test      rbp,rbp
0000A571: je        0x18000a57f
0000A573: mov       rax,QWORD PTR [rbx]
0000A576: mov       rcx,rbx
0000A579: call      QWORD PTR [rax+0x198]
0000A57F: mov       rax,QWORD PTR [rbx]
0000A582: mov       rcx,rbx
0000A585: call      QWORD PTR [rax+0x1a0]
0000A58B: lea       rcx,[rbx+0x17f8]
0000A592: call      QWORD PTR [rip+0xfbb60]        # 0x1801060f8 ; __imp_LeaveCriticalSection | KERNEL32.dll!LeaveCriticalSection
0000A598: mov       rbx,QWORD PTR [rsp+0x40]
0000A59D: mov       al,0x1
0000A59F: mov       rbp,QWORD PTR [rsp+0x48]
0000A5A4: mov       rsi,QWORD PTR [rsp+0x50]
0000A5A9: add       rsp,0x20
0000A5AD: pop       r15
0000A5AF: pop       r14
0000A5B1: pop       rdi
0000A5B2: ret       
