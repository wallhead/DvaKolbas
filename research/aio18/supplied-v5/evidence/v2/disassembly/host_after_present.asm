; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x2AC050..0x2AC645; unnamed
002AC050: mov       rax,rsp
002AC053: mov       QWORD PTR [rax+0x10],rbx
002AC057: mov       QWORD PTR [rax+0x18],rsi
002AC05B: mov       QWORD PTR [rax+0x20],rdi
002AC05F: push      r12
002AC061: push      r14
002AC063: push      r15
002AC065: sub       rsp,0x90
002AC06C: vmovaps   XMMWORD PTR [rax-0x28],xmm6
002AC071: vmovaps   XMMWORD PTR [rax-0x38],xmm7
002AC076: cmp       BYTE PTR [rip+0xbd4f22],0x0        # 0x180e80f9f
002AC07D: je        0x1802ac10c
002AC083: cmp       BYTE PTR [rip+0xbd4ef7],0x0        # 0x180e80f81
002AC08A: jne       0x1802ac105
002AC08C: call      0x180222050
002AC091: lea       rcx,[rip+0x15db80]        # 0x180409c18 ; 'F:\\GithubMods\\DLSS\\SkyrimUpscaler\\src\\UpscalerHooks.cpp'
002AC098: mov       QWORD PTR [rsp+0x30],rcx
002AC09D: mov       DWORD PTR [rsp+0x38],0x2fd
002AC0A5: mov       ecx,DWORD PTR [rsp+0x5c]
002AC0A9: mov       DWORD PTR [rsp+0x3c],ecx
002AC0AD: lea       rcx,[rip+0x15deac]        # 0x180409f60 ; 'void __cdecl AfterPresent(void)'
002AC0B4: mov       QWORD PTR [rsp+0x40],rcx
002AC0B9: lea       rcx,[rip+0x15de50]        # 0x180409f10 ; 'ReShade final submission callback missing; no completed source for this Present'
002AC0C0: mov       QWORD PTR [rsp+0x20],rcx
002AC0C5: mov       QWORD PTR [rsp+0x28],0x4f
002AC0CE: vmovups   xmm0,XMMWORD PTR [rsp+0x30]
002AC0D4: vmovups   XMMWORD PTR [rsp+0x50],xmm0
002AC0DA: vmovsd    xmm1,QWORD PTR [rsp+0x40]
002AC0E0: vmovsd    QWORD PTR [rsp+0x60],xmm1
002AC0E6: lea       r9,[rsp+0x20]
002AC0EB: mov       r8d,0x4
002AC0F1: lea       rdx,[rsp+0x50]
002AC0F6: mov       rcx,rax
002AC0F9: call      0x180151990
002AC0FE: mov       BYTE PTR [rip+0xbd4e7c],0x1        # 0x180e80f81
002AC105: mov       BYTE PTR [rip+0xbd4e93],0x0        # 0x180e80f9f
002AC10C: call      0x1802562f0
002AC111: mov       rax,QWORD PTR [rip+0xbd4cc8]        # 0x180e80de0
002AC118: cmp       BYTE PTR [rax+0x4e4],0x0
002AC11F: je        0x1802ac5ba
002AC125: cmp       BYTE PTR [rax+0x4a5],0x0
002AC12C: je        0x1802ac144
002AC12E: mov       eax,DWORD PTR [rip+0x1b33bc]        # 0x18045f4f0
002AC134: and       eax,0xb8
002AC139: cmp       al,0xb8
002AC13B: je        0x1802ac171
002AC13D: mov       rax,QWORD PTR [rip+0xbd4c9c]        # 0x180e80de0
002AC144: cmp       BYTE PTR [rax+0x4e4],0x0
002AC14B: je        0x1802ac5ba
002AC151: cmp       DWORD PTR [rip+0x1c95a8],0x2        # 0x180475700
002AC158: jne       0x1802ac5ba
002AC15E: mov       eax,DWORD PTR [rip+0x1b338c]        # 0x18045f4f0
002AC164: and       eax,0xb8
002AC169: cmp       al,0xb8
002AC16B: jne       0x1802ac5ba
002AC171: xor       bl,bl
002AC173: mov       rax,QWORD PTR [rip+0xbd4c66]        # 0x180e80de0
002AC17A: cmp       BYTE PTR [rax+0x4e4],bl
002AC180: je        0x1802ac249
002AC186: cmp       BYTE PTR [rip+0xbd4e55],bl        # 0x180e80fe1
002AC18C: je        0x1802ac249
002AC192: call      QWORD PTR [rip+0x1c6b00]        # 0x180472c98 ; PDPerfPlugin.dll!GetPDFrameWarpRenderFrameCount
002AC198: cmp       rax,QWORD PTR [rip+0xbd4e31]        # 0x180e80fd0
002AC19F: jbe       0x1802ac249
002AC1A5: lea       rax,[rip+0x1c2cf4]        # 0x18046eea0
002AC1AC: lea       rcx,[rip+0x1c2a1d]        # 0x18046ebd0
002AC1B3: mov       edx,0x5
002AC1B8: nop       DWORD PTR [rax+rax*1+0x0]
002AC1C0: vmovups   ymm0,YMMWORD PTR [rcx]
002AC1C4: vmovups   YMMWORD PTR [rax],ymm0
002AC1C8: vmovups   ymm0,YMMWORD PTR [rcx+0x20]
002AC1CD: vmovups   YMMWORD PTR [rax+0x20],ymm0
002AC1D2: vmovups   ymm0,YMMWORD PTR [rcx+0x40]
002AC1D7: vmovups   YMMWORD PTR [rax+0x40],ymm0
002AC1DC: vmovups   xmm0,XMMWORD PTR [rcx+0x60]
002AC1E1: vmovups   XMMWORD PTR [rax+0x60],xmm0
002AC1E6: lea       rax,[rax+0x80]
002AC1ED: vmovups   xmm1,XMMWORD PTR [rcx+0x70]
002AC1F2: vmovups   XMMWORD PTR [rax-0x10],xmm1
002AC1F7: lea       rcx,[rcx+0x80]
002AC1FE: sub       rdx,0x1
002AC202: jne       0x1802ac1c0
002AC204: vmovups   ymm0,YMMWORD PTR [rcx]
002AC208: vmovups   YMMWORD PTR [rax],ymm0
002AC20C: vmovups   ymm0,YMMWORD PTR [rcx+0x20]
002AC211: vmovups   YMMWORD PTR [rax+0x20],ymm0
002AC216: vmovups   xmm0,XMMWORD PTR [rcx+0x40]
002AC21B: vmovups   XMMWORD PTR [rax+0x40],xmm0
002AC220: mov       BYTE PTR [rip+0xbd4db9],0x1        # 0x180e80fe0
002AC227: mov       rcx,QWORD PTR [rip+0xbd4bb2]        # 0x180e80de0
002AC22E: mov       rax,QWORD PTR [rcx+0x78]
002AC232: mov       QWORD PTR [rip+0xbd4d87],rax        # 0x180e80fc0
002AC239: mov       rax,QWORD PTR [rcx+0x88]
002AC240: mov       QWORD PTR [rip+0xbd4d81],rax        # 0x180e80fc8
002AC247: mov       bl,0x1
002AC249: mov       eax,DWORD PTR [rip+0x1b32a1]        # 0x18045f4f0
002AC24F: test      al,0x8
002AC251: je        0x1802ac25b
002AC253: vzeroupper 
002AC256: call      0x180284420
002AC25B: movzx     ecx,bl
002AC25E: vzeroupper 
002AC261: call      0x18027f680
002AC266: mov       BYTE PTR [rip+0xbd4d74],0x0        # 0x180e80fe1
002AC26D: call      0x1802562f0
002AC272: mov       ecx,DWORD PTR [rip+0x1c723c]        # 0x1804734b4
002AC278: mov       rax,QWORD PTR gs:0x58
002AC281: mov       edx,0xac
002AC286: mov       rax,QWORD PTR [rax+rcx*8]
002AC28A: xor       r12d,r12d
002AC28D: lea       rbx,[rip+0xbd56ac]        # 0x180e81940
002AC294: mov       eax,DWORD PTR [rdx+rax*1]
002AC297: cmp       DWORD PTR [rip+0xbd5693],eax        # 0x180e81930
002AC29D: jg        0x1802ac5f6
002AC2A3: mov       rax,QWORD PTR [rip+0xbd4b36]        # 0x180e80de0
002AC2AA: cmp       BYTE PTR [rax+0x4e4],0x0
002AC2B1: je        0x1802ac59d
002AC2B7: lea       rcx,[rsp+0xb0]
002AC2BF: call      0x180179bf0
002AC2C4: call      QWORD PTR [rip+0x1c69ce]        # 0x180472c98 ; PDPerfPlugin.dll!GetPDFrameWarpRenderFrameCount
002AC2CA: mov       rdi,rax
002AC2CD: cmp       BYTE PTR [rip+0xbd4cac],0x0        # 0x180e80f80
002AC2D4: je        0x1802ac55c
002AC2DA: mov       rdx,QWORD PTR [rip+0xbd4c8f]        # 0x180e80f70
002AC2E1: cmp       rax,rdx
002AC2E4: jb        0x1802ac55c
002AC2EA: mov       rbx,QWORD PTR [rsp+0xb0]
002AC2F2: mov       rcx,rbx
002AC2F5: sub       rcx,QWORD PTR [rip+0xbd4c6c]        # 0x180e80f68
002AC2FC: vxorps    xmm0,xmm0,xmm0
002AC300: vcvtsi2sd xmm0,xmm0,rcx
002AC305: vmovsd    xmm7,QWORD PTR [rip+0x15f96b]        # 0x18040bc78
002AC30D: vmulsd    xmm6,xmm0,xmm7
002AC311: vxorpd    xmm1,xmm1,xmm1
002AC315: vcomisd   xmm6,xmm1
002AC319: jbe       0x1802ac5ba
002AC31F: mov       r14,rax
002AC322: sub       r14,rdx
002AC325: mov       r9,QWORD PTR [rip+0xbd5634]        # 0x180e81960
002AC32C: lea       rax,[r9+0x1]
002AC330: mov       r8,QWORD PTR [rip+0xbd5619]        # 0x180e81950
002AC337: cmp       r8,rax
002AC33A: ja        0x1802ac356
002AC33C: call      0x1802a5690
002AC341: mov       r9,QWORD PTR [rip+0xbd5618]        # 0x180e81960
002AC348: mov       r8,QWORD PTR [rip+0xbd5601]        # 0x180e81950
002AC34F: mov       rdx,QWORD PTR [rip+0xbd4c1a]        # 0x180e80f70
002AC356: lea       rcx,[r8-0x1]
002AC35A: mov       rax,QWORD PTR [rip+0xbd55f7]        # 0x180e81958
002AC361: and       rax,rcx
002AC364: mov       QWORD PTR [rip+0xbd55ed],rax        # 0x180e81958
002AC36B: lea       r15,[r9+rax*1]
002AC36F: mov       rsi,r15
002AC372: and       rsi,rcx
002AC375: mov       rcx,QWORD PTR [rip+0xbd55cc]        # 0x180e81948
002AC37C: cmp       QWORD PTR [rcx+rsi*8],0x0
002AC381: jne       0x1802ac3ad
002AC383: mov       ecx,0x18
002AC388: call      0x180239238
002AC38D: mov       rcx,QWORD PTR [rip+0xbd55b4]        # 0x180e81948
002AC394: mov       QWORD PTR [rcx+rsi*8],rax
002AC398: mov       r8,QWORD PTR [rip+0xbd55b1]        # 0x180e81950
002AC39F: mov       rcx,QWORD PTR [rip+0xbd55a2]        # 0x180e81948
002AC3A6: mov       rdx,QWORD PTR [rip+0xbd4bc3]        # 0x180e80f70
002AC3AD: lea       rax,[r8-0x1]
002AC3B1: and       rax,r15
002AC3B4: mov       rcx,QWORD PTR [rcx+rax*8]
002AC3B8: mov       QWORD PTR [rcx],rbx
002AC3BB: vmovsd    QWORD PTR [rcx+0x8],xmm6
002AC3C0: mov       QWORD PTR [rcx+0x10],r14
002AC3C4: mov       r9,QWORD PTR [rip+0xbd5595]        # 0x180e81960
002AC3CB: inc       r9
002AC3CE: mov       QWORD PTR [rip+0xbd558b],r9        # 0x180e81960
002AC3D5: vaddsd    xmm2,xmm6,QWORD PTR [rip+0xbd4b9b]        # 0x180e80f78
002AC3DD: vmovsd    QWORD PTR [rip+0xbd4b93],xmm2        # 0x180e80f78
002AC3E5: mov       rcx,QWORD PTR [rip+0xbd4b74]        # 0x180e80f60
002AC3EC: sub       rcx,rdx
002AC3EF: add       rcx,rdi
002AC3F2: mov       QWORD PTR [rip+0xbd4b67],rcx        # 0x180e80f60
002AC3F9: mov       QWORD PTR [rip+0xbd4b68],rbx        # 0x180e80f68
002AC400: mov       QWORD PTR [rip+0xbd4b69],rdi        # 0x180e80f70
002AC407: vmovaps   xmm3,xmm2
002AC40B: vmovsd    xmm4,QWORD PTR [rip+0x15f975]        # 0x18040bd88
002AC413: cmp       r9,0x1
002AC417: jbe       0x1802ac4ba
002AC41D: vmovaps   xmm5,xmm2
002AC421: mov       rdi,rcx
002AC424: vmovaps   xmm0,xmm2
002AC428: mov       r10,QWORD PTR [rip+0xbd5521]        # 0x180e81950
002AC42F: dec       r10
002AC432: mov       r11,QWORD PTR [rip+0xbd550f]        # 0x180e81948
002AC439: mov       rdx,QWORD PTR [rip+0xbd5518]        # 0x180e81958
002AC440: mov       rax,rdx
002AC443: and       rax,r10
002AC446: mov       r8,QWORD PTR [r11+rax*8]
002AC44A: vmovaps   xmm2,xmm0
002AC44E: mov       rax,rbx
002AC451: sub       rax,QWORD PTR [r8]
002AC454: vxorpd    xmm0,xmm0,xmm0
002AC458: vcvtsi2sd xmm0,xmm0,rax
002AC45D: vmulsd    xmm1,xmm0,xmm7
002AC461: vcomisd   xmm1,xmm4
002AC465: jb        0x1802ac4ba
002AC467: vsubsd    xmm2,xmm5,QWORD PTR [r8+0x8]
002AC46D: vmovaps   xmm3,xmm2
002AC471: vmovaps   xmm5,xmm2
002AC475: vmovsd    QWORD PTR [rip+0xbd4afb],xmm2        # 0x180e80f78
002AC47D: mov       rcx,rdi
002AC480: sub       rcx,QWORD PTR [r8+0x10]
002AC484: mov       rdi,rcx
002AC487: mov       QWORD PTR [rip+0xbd4ad2],rcx        # 0x180e80f60
002AC48E: mov       rax,r9
002AC491: dec       r9
002AC494: mov       QWORD PTR [rip+0xbd54c5],r9        # 0x180e81960
002AC49B: cmp       rax,0x1
002AC49F: jne       0x1802ac4a6
002AC4A1: mov       rdx,r12
002AC4A4: jmp       0x1802ac4a9
002AC4A6: inc       rdx
002AC4A9: mov       QWORD PTR [rip+0xbd54a8],rdx        # 0x180e81958
002AC4B0: vmovaps   xmm0,xmm2
002AC4B4: cmp       r9,0x1
002AC4B8: ja        0x1802ac440
002AC4BA: mov       rax,rbx
002AC4BD: sub       rax,QWORD PTR [rip+0xbd4a8c]        # 0x180e80f50
002AC4C4: vxorpd    xmm0,xmm0,xmm0
002AC4C8: vcvtsi2sd xmm0,xmm0,rax
002AC4CD: vmulsd    xmm1,xmm0,xmm7
002AC4D1: vcomisd   xmm1,xmm4
002AC4D5: jb        0x1802ac5ba
002AC4DB: vxorpd    xmm0,xmm0,xmm0
002AC4DF: test      rcx,rcx
002AC4E2: js        0x1802ac4eb
002AC4E4: vcvtsi2sd xmm0,xmm0,rcx
002AC4E9: jmp       0x1802ac500
002AC4EB: mov       rax,rcx
002AC4EE: shr       rax,1
002AC4F1: and       ecx,0x1
002AC4F4: or        rax,rcx
002AC4F7: vcvtsi2sd xmm0,xmm0,rax
002AC4FC: vaddsd    xmm0,xmm0,xmm0
002AC500: vdivsd    xmm0,xmm0,xmm2
002AC504: vcvtsd2ss xmm1,xmm0,xmm0
002AC508: mov       rdx,QWORD PTR [rip+0xbd48d1]        # 0x180e80de0
002AC50F: vmovss    DWORD PTR [rdx+0x4dc],xmm1
002AC517: mov       rcx,QWORD PTR [rip+0xbd5442]        # 0x180e81960
002AC51E: vxorpd    xmm0,xmm0,xmm0
002AC522: test      rcx,rcx
002AC525: js        0x1802ac52e
002AC527: vcvtsi2sd xmm0,xmm0,rcx
002AC52C: jmp       0x1802ac543
002AC52E: mov       rax,rcx
002AC531: shr       rax,1
002AC534: and       ecx,0x1
002AC537: or        rax,rcx
002AC53A: vcvtsi2sd xmm0,xmm0,rax
002AC53F: vaddsd    xmm0,xmm0,xmm0
002AC543: vdivsd    xmm0,xmm0,xmm3
002AC547: vcvtsd2ss xmm1,xmm0,xmm0
002AC54B: vmovss    DWORD PTR [rdx+0x4e0],xmm1
002AC553: mov       QWORD PTR [rip+0xbd49f6],rbx        # 0x180e80f50
002AC55A: jmp       0x1802ac5ba
002AC55C: mov       rcx,rbx
002AC55F: call      0x1802a5a20
002AC564: vxorpd    xmm0,xmm0,xmm0
002AC568: vmovsd    QWORD PTR [rip+0xbd4a08],xmm0        # 0x180e80f78
002AC570: mov       QWORD PTR [rip+0xbd49e9],r12        # 0x180e80f60
002AC577: mov       rax,QWORD PTR [rsp+0xb0]
002AC57F: mov       QWORD PTR [rip+0xbd49ca],rax        # 0x180e80f50
002AC586: mov       QWORD PTR [rip+0xbd49db],rax        # 0x180e80f68
002AC58D: mov       QWORD PTR [rip+0xbd49dc],rdi        # 0x180e80f70
002AC594: mov       BYTE PTR [rip+0xbd49e5],0x1        # 0x180e80f80
002AC59B: jmp       0x1802ac5ac
002AC59D: mov       BYTE PTR [rip+0xbd49dc],0x0        # 0x180e80f80
002AC5A4: mov       rcx,rbx
002AC5A7: call      0x1802a5a20
002AC5AC: mov       rax,QWORD PTR [rip+0xbd482d]        # 0x180e80de0
002AC5B3: mov       QWORD PTR [rax+0x4dc],r12
002AC5BA: mov       BYTE PTR [rip+0xbd4a20],0x0        # 0x180e80fe1
002AC5C1: mov       BYTE PTR [rip+0xbd4ac9],0x0        # 0x180e81091
002AC5C8: lea       r11,[rsp+0x90]
002AC5D0: mov       rbx,QWORD PTR [r11+0x28]
002AC5D4: mov       rsi,QWORD PTR [r11+0x30]
002AC5D8: mov       rdi,QWORD PTR [r11+0x38]
002AC5DC: vmovaps   xmm6,XMMWORD PTR [r11-0x10]
002AC5E2: vmovaps   xmm7,XMMWORD PTR [rsp+0x70]
002AC5E8: mov       rsp,r11
002AC5EB: pop       r15
002AC5ED: pop       r14
002AC5EF: pop       r12
002AC5F1: jmp       0x180293e30
002AC5F6: lea       rcx,[rip+0xbd5333]        # 0x180e81930
002AC5FD: call      0x1802391bc
002AC602: cmp       DWORD PTR [rip+0xbd5327],0xffffffff        # 0x180e81930
002AC609: jne       0x1802ac2a3
002AC60F: mov       ecx,0x10
002AC614: call      0x180239238
002AC619: mov       QWORD PTR [rax+0x8],r12
002AC61D: mov       QWORD PTR [rip+0xbd531c],rax        # 0x180e81940
002AC624: mov       QWORD PTR [rax],rbx
002AC627: lea       rcx,[rip+0x15a52]        # 0x1802c2080
002AC62E: call      0x1802396d0
002AC633: nop       
002AC634: lea       rcx,[rip+0xbd52f5]        # 0x180e81930
002AC63B: call      0x180239150
002AC640: jmp       0x1802ac2a3
