; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xCF080..0xCF5DF; unnamed
000CF080: mov       QWORD PTR [rsp+0x10],rbx
000CF085: mov       QWORD PTR [rsp+0x18],rbp
000CF08A: mov       QWORD PTR [rsp+0x8],rcx
000CF08F: push      rsi
000CF090: push      rdi
000CF091: push      r12
000CF093: push      r14
000CF095: push      r15
000CF097: sub       rsp,0x30
000CF09B: movzx     esi,r9b
000CF09F: mov       rbp,r8
000CF0A2: mov       edi,edx
000CF0A4: mov       r14,rcx
000CF0A7: add       rcx,0x8
000CF0AB: call      0x18007a990
000CF0B0: nop       
000CF0B1: lea       rax,[rip+0x10d9220]        # 0x1811a82d8
000CF0B8: mov       QWORD PTR [r14],rax
000CF0BB: xor       r12d,r12d
000CF0BE: mov       QWORD PTR [r14+0x118],r12
000CF0C5: mov       QWORD PTR [r14+0x120],r12
000CF0CC: mov       QWORD PTR [r14+0x128],r12
000CF0D3: mov       ecx,0x30
000CF0D8: call      0x18010c290
000CF0DD: mov       QWORD PTR [rax],rax
000CF0E0: mov       QWORD PTR [rax+0x8],rax
000CF0E4: mov       QWORD PTR [rax+0x10],rax
000CF0E8: mov       WORD PTR [rax+0x18],0x101
000CF0EE: mov       QWORD PTR [r14+0x120],rax
000CF0F5: mov       QWORD PTR [r14+0x130],r12
000CF0FC: mov       QWORD PTR [r14+0x138],r12
000CF103: mov       ecx,0x30
000CF108: call      0x18010c290
000CF10D: mov       QWORD PTR [rax],rax
000CF110: mov       QWORD PTR [rax+0x8],rax
000CF114: mov       QWORD PTR [rax+0x10],rax
000CF118: mov       WORD PTR [rax+0x18],0x101
000CF11E: mov       QWORD PTR [r14+0x130],rax
000CF125: mov       QWORD PTR [r14+0x140],r12
000CF12C: mov       QWORD PTR [r14+0x148],r12
000CF133: mov       QWORD PTR [r14+0x150],r12
000CF13A: mov       QWORD PTR [r14+0x158],r12
000CF141: mov       QWORD PTR [r14+0x160],r12
000CF148: mov       QWORD PTR [r14+0x168],r12
000CF14F: mov       QWORD PTR [r14+0x170],r12
000CF156: mov       QWORD PTR [r14+0x178],r12
000CF15D: mov       QWORD PTR [r14+0x180],r12
000CF164: mov       QWORD PTR [r14+0x188],r12
000CF16B: mov       ecx,0x30
000CF170: call      0x18010c290
000CF175: mov       QWORD PTR [rax],rax
000CF178: mov       QWORD PTR [rax+0x8],rax
000CF17C: mov       QWORD PTR [rax+0x10],rax
000CF180: mov       WORD PTR [rax+0x18],0x101
000CF186: mov       QWORD PTR [r14+0x180],rax
000CF18D: mov       WORD PTR [r14+0x190],r12w
000CF195: mov       QWORD PTR [r14+0x194],r12
000CF19C: mov       QWORD PTR [r14+0x1c8],r12
000CF1A3: mov       QWORD PTR [r14+0x208],r12
000CF1AA: mov       QWORD PTR [r14+0x210],r12
000CF1B1: mov       QWORD PTR [r14+0x218],r12
000CF1B8: mov       QWORD PTR [r14+0x220],r12
000CF1BF: mov       QWORD PTR [r14+0x228],r12
000CF1C6: mov       QWORD PTR [r14+0x230],r12
000CF1CD: mov       DWORD PTR [r14+0x238],r12d
000CF1D4: mov       BYTE PTR [r14+0x23c],r12b
000CF1DB: mov       DWORD PTR [r14+0x240],0x1
000CF1E6: mov       QWORD PTR [r14+0x248],r12
000CF1ED: mov       QWORD PTR [r14+0x250],r12
000CF1F4: mov       QWORD PTR [r14+0x258],r12
000CF1FB: mov       ecx,0x98
000CF200: call      0x18010c290
000CF205: mov       QWORD PTR [rax],rax
000CF208: mov       QWORD PTR [rax+0x8],rax
000CF20C: mov       QWORD PTR [rax+0x10],rax
000CF210: mov       WORD PTR [rax+0x18],0x101
000CF216: mov       QWORD PTR [r14+0x250],rax
000CF21D: mov       QWORD PTR [r14+0x260],r12
000CF224: mov       QWORD PTR [r14+0x268],r12
000CF22B: mov       ecx,0x98
000CF230: call      0x18010c290
000CF235: mov       QWORD PTR [rax],rax
000CF238: mov       QWORD PTR [rax+0x8],rax
000CF23C: mov       QWORD PTR [rax+0x10],rax
000CF240: mov       WORD PTR [rax+0x18],0x101
000CF246: mov       QWORD PTR [r14+0x260],rax
000CF24D: mov       QWORD PTR [r14+0x270],r12
000CF254: mov       QWORD PTR [r14+0x278],r12
000CF25B: mov       QWORD PTR [r14+0x280],r12
000CF262: mov       QWORD PTR [r14+0x288],r12
000CF269: xorps     xmm0,xmm0
000CF26C: movups    XMMWORD PTR [r14+0x290],xmm0
000CF274: movups    XMMWORD PTR [r14+0x2a0],xmm0
000CF27C: movups    XMMWORD PTR [r14+0x2b0],xmm0
000CF284: movups    XMMWORD PTR [r14+0x2c0],xmm0
000CF28C: mov       QWORD PTR [r14+0x2d0],r12
000CF293: mov       QWORD PTR [r14+0x2d8],0xf
000CF29E: mov       BYTE PTR [r14+0x2c0],r12b
000CF2A5: mov       QWORD PTR [r14+0x2e0],r12
000CF2AC: mov       QWORD PTR [r14+0x2e8],r12
000CF2B3: mov       QWORD PTR [r14+0x2f0],r12
000CF2BA: mov       QWORD PTR [r14+0x2f8],r12
000CF2C1: movups    XMMWORD PTR [r14+0x300],xmm0
000CF2C9: movups    XMMWORD PTR [r14+0x310],xmm0
000CF2D1: movups    XMMWORD PTR [r14+0x320],xmm0
000CF2D9: movups    XMMWORD PTR [r14+0x330],xmm0
000CF2E1: mov       QWORD PTR [r14+0x340],r12
000CF2E8: mov       QWORD PTR [r14+0x348],0xf
000CF2F3: mov       BYTE PTR [r14+0x330],r12b
000CF2FA: mov       QWORD PTR [r14+0x350],r12
000CF301: mov       QWORD PTR [r14+0x358],r12
000CF308: mov       QWORD PTR [r14+0x360],r12
000CF30F: mov       QWORD PTR [r14+0x368],r12
000CF316: movups    XMMWORD PTR [r14+0x370],xmm0
000CF31E: movups    XMMWORD PTR [r14+0x380],xmm0
000CF326: movups    XMMWORD PTR [r14+0x390],xmm0
000CF32E: movups    XMMWORD PTR [r14+0x3a0],xmm0
000CF336: mov       QWORD PTR [r14+0x3b0],r12
000CF33D: mov       QWORD PTR [r14+0x3b8],0xf
000CF348: mov       BYTE PTR [r14+0x3a0],r12b
000CF34F: mov       QWORD PTR [r14+0x3c0],r12
000CF356: mov       QWORD PTR [r14+0x3c8],r12
000CF35D: mov       QWORD PTR [r14+0x3d0],r12
000CF364: mov       QWORD PTR [r14+0x3d8],r12
000CF36B: movups    XMMWORD PTR [r14+0x3e0],xmm0
000CF373: movups    XMMWORD PTR [r14+0x3f0],xmm0
000CF37B: movups    XMMWORD PTR [r14+0x400],xmm0
000CF383: movups    XMMWORD PTR [r14+0x410],xmm0
000CF38B: mov       QWORD PTR [r14+0x420],r12
000CF392: mov       QWORD PTR [r14+0x428],0xf
000CF39D: mov       BYTE PTR [r14+0x410],r12b
000CF3A4: mov       QWORD PTR [r14+0x430],r12
000CF3AB: mov       QWORD PTR [r14+0x438],r12
000CF3B2: mov       QWORD PTR [r14+0x440],r12
000CF3B9: mov       QWORD PTR [r14+0x448],r12
000CF3C0: movups    XMMWORD PTR [r14+0x450],xmm0
000CF3C8: movups    XMMWORD PTR [r14+0x460],xmm0
000CF3D0: movups    XMMWORD PTR [r14+0x470],xmm0
000CF3D8: movups    XMMWORD PTR [r14+0x480],xmm0
000CF3E0: mov       QWORD PTR [r14+0x490],r12
000CF3E7: mov       QWORD PTR [r14+0x498],0xf
000CF3F2: mov       BYTE PTR [r14+0x480],r12b
000CF3F9: mov       QWORD PTR [r14+0x4a0],r12
000CF400: mov       QWORD PTR [r14+0x4a8],r12
000CF407: mov       QWORD PTR [r14+0x4b0],r12
000CF40E: mov       QWORD PTR [r14+0x4b8],r12
000CF415: movups    XMMWORD PTR [r14+0x4c0],xmm0
000CF41D: movups    XMMWORD PTR [r14+0x4d0],xmm0
000CF425: movups    XMMWORD PTR [r14+0x4e0],xmm0
000CF42D: movups    XMMWORD PTR [r14+0x4f0],xmm0
000CF435: mov       QWORD PTR [r14+0x500],r12
000CF43C: mov       QWORD PTR [r14+0x508],0xf
000CF447: mov       BYTE PTR [r14+0x4f0],r12b
000CF44E: mov       QWORD PTR [r14+0x510],r12
000CF455: mov       QWORD PTR [r14+0x518],r12
000CF45C: mov       QWORD PTR [r14+0x520],r12
000CF463: mov       QWORD PTR [r14+0x528],r12
000CF46A: movups    XMMWORD PTR [r14+0x530],xmm0
000CF472: movups    XMMWORD PTR [r14+0x540],xmm0
000CF47A: movups    XMMWORD PTR [r14+0x550],xmm0
000CF482: movups    XMMWORD PTR [r14+0x560],xmm0
000CF48A: mov       QWORD PTR [r14+0x570],r12
000CF491: mov       QWORD PTR [r14+0x578],0xf
000CF49C: mov       BYTE PTR [r14+0x560],r12b
000CF4A3: mov       QWORD PTR [r14+0x580],r12
000CF4AA: mov       QWORD PTR [r14+0x588],r12
000CF4B1: mov       QWORD PTR [r14+0x590],r12
000CF4B8: mov       QWORD PTR [r14+0x598],r12
000CF4BF: movups    XMMWORD PTR [r14+0x5a0],xmm0
000CF4C7: movups    XMMWORD PTR [r14+0x5b0],xmm0
000CF4CF: movups    XMMWORD PTR [r14+0x5c0],xmm0
000CF4D7: movups    XMMWORD PTR [r14+0x5d0],xmm0
000CF4DF: mov       QWORD PTR [r14+0x5e0],r12
000CF4E6: mov       QWORD PTR [r14+0x5e8],0xf
000CF4F1: mov       BYTE PTR [r14+0x5d0],r12b
000CF4F8: mov       QWORD PTR [r14+0x5f0],r12
000CF4FF: mov       QWORD PTR [r14+0x5f8],r12
000CF506: mov       QWORD PTR [r14+0x600],r12
000CF50D: mov       QWORD PTR [r14+0x608],r12
000CF514: movups    XMMWORD PTR [r14+0x610],xmm0
000CF51C: movups    XMMWORD PTR [r14+0x620],xmm0
000CF524: movups    XMMWORD PTR [r14+0x630],xmm0
000CF52C: movups    XMMWORD PTR [r14+0x640],xmm0
000CF534: mov       QWORD PTR [r14+0x650],r12
000CF53B: mov       QWORD PTR [r14+0x658],0xf
000CF546: mov       BYTE PTR [r14+0x640],r12b
000CF54D: mov       BYTE PTR [r14+0x660],r12b
000CF554: mov       DWORD PTR [r14+0x664],r12d
000CF55B: mov       WORD PTR [r14+0x668],r12w
000CF563: mov       DWORD PTR [r14+0x110],edi
000CF56A: mov       BYTE PTR [r14+0x48],sil
000CF56E: lea       rcx,[rip+0x10d871b]        # 0x1811a7c90 ; 'Creating UpscaleMethod_DX11WrapperForDX12'
000CF575: call      0x1800fbb40
000CF57A: mov       rcx,QWORD PTR [rip+0x1146e2f]        # 0x1812163b0
000CF581: cmp       BYTE PTR [rcx],r12b
000CF584: jne       0x1800cf598
000CF586: mov       r8b,0x1
000CF589: mov       rdx,rbp
000CF58C: call      0x180071ac0
000CF591: mov       rcx,QWORD PTR [rip+0x1146e18]        # 0x1812163b0
000CF598: xor       edx,edx
000CF59A: call      0x180072dd0
000CF59F: mov       r8b,0x1
000CF5A2: xor       edx,edx
000CF5A4: mov       rcx,QWORD PTR [rip+0x1146e05]        # 0x1812163b0
000CF5AB: call      0x180072810 ; '@VWAUAWH'
000CF5B0: mov       rcx,r14
000CF5B3: call      0x1800cfa50
000CF5B8: mov       rdx,rbp
000CF5BB: lea       rcx,[r14+0x8]
000CF5BF: call      0x1800ce890
000CF5C4: nop       
000CF5C5: mov       rax,r14
000CF5C8: mov       rbx,QWORD PTR [rsp+0x68]
000CF5CD: mov       rbp,QWORD PTR [rsp+0x70]
000CF5D2: add       rsp,0x30
000CF5D6: pop       r15
000CF5D8: pop       r14
000CF5DA: pop       r12
000CF5DC: pop       rdi
000CF5DD: pop       rsi
000CF5DE: ret       
