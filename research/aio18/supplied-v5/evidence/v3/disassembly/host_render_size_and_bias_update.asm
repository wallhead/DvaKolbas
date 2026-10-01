; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x2A4500..0x2A4728; unnamed
002A4500: mov       QWORD PTR [rsp+0x20],rbx
002A4505: push      rbp
002A4506: mov       rbp,rsp
002A4509: sub       rsp,0x80
002A4510: cmp       BYTE PTR [rcx+0x343],0x0
002A4517: mov       rbx,rcx
002A451A: mov       eax,DWORD PTR [rcx+0x270]
002A4520: je        0x1802a4603
002A4526: mov       DWORD PTR [rcx+0x290],0x3f800000
002A4530: test      eax,eax
002A4532: jle       0x1802a4562
002A4534: mov       ecx,DWORD PTR [rcx+0x274]
002A453A: test      ecx,ecx
002A453C: jle       0x1802a4562
002A453E: mov       DWORD PTR [rbx+0x278],eax
002A4544: mov       DWORD PTR [rbx+0x27c],ecx
002A454A: mov       DWORD PTR [rbx+0x280],eax
002A4550: mov       DWORD PTR [rbx+0x284],ecx
002A4556: mov       DWORD PTR [rbx+0x288],eax
002A455C: mov       DWORD PTR [rbx+0x28c],ecx
002A4562: xor       eax,eax
002A4564: mov       DWORD PTR [rbx+0x2e8],eax
002A456A: cmp       BYTE PTR [rbx+0x485],al
002A4570: je        0x1802a4578
002A4572: mov       DWORD PTR [rbx+0x2ec],eax
002A4578: call      0x180222050
002A457D: mov       r8,rax
002A4580: mov       DWORD PTR [rbp-0x38],0xb57
002A4587: lea       rcx,[rip+0x161ba2]        # 0x180406130
002A458E: mov       QWORD PTR [rbp-0x48],0x2c
002A4596: mov       QWORD PTR [rbp-0x40],rcx
002A459A: lea       rax,[rip+0x162d77]        # 0x180407318
002A45A1: mov       ecx,DWORD PTR [rbp-0x14]
002A45A4: lea       r9,[rbp-0x50]
002A45A8: mov       DWORD PTR [rbp-0x34],ecx
002A45AB: lea       rdx,[rbp-0x20]
002A45AF: vmovups   xmm0,XMMWORD PTR [rbp-0x40]
002A45B4: lea       rcx,[rip+0x162d8d]        # 0x180407348
002A45BB: mov       QWORD PTR [rbp-0x50],rax
002A45BF: mov       QWORD PTR [rbp-0x30],rcx
002A45C3: lea       rax,[rbx+0x284]
002A45CA: vmovsd    xmm1,QWORD PTR [rbp-0x30]
002A45CF: lea       rcx,[rbx+0x280]
002A45D6: mov       QWORD PTR [rsp+0x28],rax
002A45DB: mov       QWORD PTR [rsp+0x20],rcx
002A45E0: mov       rcx,r8
002A45E3: vmovups   XMMWORD PTR [rbp-0x20],xmm0
002A45E8: vmovsd    QWORD PTR [rbp-0x10],xmm1
002A45ED: call      0x180196810
002A45F2: mov       rbx,QWORD PTR [rsp+0xa8]
002A45FA: add       rsp,0x80
002A4601: pop       rbp
002A4602: ret       
002A4603: vmovss    xmm2,DWORD PTR [rcx+0x294]
002A460B: vxorps    xmm0,xmm0,xmm0
002A460F: vcvtsi2ss xmm0,xmm0,eax
002A4613: vmulss    xmm1,xmm0,xmm2
002A4617: vxorps    xmm0,xmm0,xmm0
002A461B: vcvtsi2ss xmm0,xmm0,DWORD PTR [rcx+0x274]
002A4623: vcvttss2si eax,xmm1
002A4627: vmulss    xmm1,xmm0,xmm2
002A462B: vmovss    xmm0,DWORD PTR [rcx+0x290]
002A4633: mov       QWORD PTR [rsp+0x98],rsi
002A463B: lea       rsi,[rcx+0x284]
002A4642: mov       QWORD PTR [rsp+0xa0],rdi
002A464A: lea       rdi,[rcx+0x280]
002A4651: mov       DWORD PTR [rdi],eax
002A4653: vcvttss2si eax,xmm1
002A4657: vdivss    xmm0,xmm0,xmm2
002A465B: mov       DWORD PTR [rsi],eax
002A465D: call      0x1802af9c3
002A4662: cmp       BYTE PTR [rbx+0x485],0x0
002A4669: vminss    xmm2,xmm0,DWORD PTR [rip+0x1677af]        # 0x18040be20
002A4671: vmovss    xmm1,DWORD PTR [rip+0x1678cf]        # 0x18040bf48
002A4679: vcmpltss  xmm0,xmm1,xmm2
002A467E: vblendvps xmm3,xmm1,xmm2,xmm0
002A4684: vmovss    DWORD PTR [rbx+0x2e8],xmm3
002A468C: jne       0x1802a4696
002A468E: vmovss    xmm3,DWORD PTR [rbx+0x2ec]
002A4696: vmovss    DWORD PTR [rbx+0x2ec],xmm3
002A469E: call      0x180222050
002A46A3: lea       rcx,[rip+0x161a86]        # 0x180406130
002A46AA: mov       DWORD PTR [rbp-0x38],0xb60
002A46B1: mov       QWORD PTR [rbp-0x40],rcx
002A46B5: lea       r9,[rbp-0x50]
002A46B9: mov       ecx,DWORD PTR [rbp-0x14]
002A46BC: lea       rdx,[rbp-0x20]
002A46C0: mov       DWORD PTR [rbp-0x34],ecx
002A46C3: lea       rcx,[rip+0x162c7e]        # 0x180407348
002A46CA: vmovups   xmm0,XMMWORD PTR [rbp-0x40]
002A46CF: mov       QWORD PTR [rbp-0x30],rcx
002A46D3: lea       rcx,[rip+0x162d2e]        # 0x180407408
002A46DA: vmovsd    xmm1,QWORD PTR [rbp-0x30]
002A46DF: mov       QWORD PTR [rbp-0x50],rcx
002A46E3: mov       rcx,rax
002A46E6: mov       QWORD PTR [rsp+0x28],rsi
002A46EB: mov       QWORD PTR [rbp-0x48],0x16
002A46F3: vmovups   XMMWORD PTR [rbp-0x20],xmm0
002A46F8: vmovsd    QWORD PTR [rbp-0x10],xmm1
002A46FD: mov       QWORD PTR [rsp+0x20],rdi
002A4702: call      0x180196810
002A4707: mov       rdi,QWORD PTR [rsp+0xa0]
002A470F: mov       rsi,QWORD PTR [rsp+0x98]
002A4717: mov       rbx,QWORD PTR [rsp+0xa8]
002A471F: add       rsp,0x80
002A4726: pop       rbp
002A4727: ret       
