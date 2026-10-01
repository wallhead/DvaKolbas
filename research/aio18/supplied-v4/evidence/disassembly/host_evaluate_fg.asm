; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x2941A0..0x294915; unnamed
002941A0: mov       QWORD PTR [rsp+0x8],rcx
002941A5: push      rbp
002941A6: push      rdi
002941A7: push      r15
002941A9: lea       rbp,[rsp-0x2b0]
002941B1: sub       rsp,0x3b0
002941B8: mov       rdi,QWORD PTR [rip+0xbecc21]        # 0x180e80de0
002941BF: mov       r15,rdx
002941C2: mov       rcx,rdi
002941C5: call      0x180296550
002941CA: test      al,al
002941CC: je        0x180294906
002941D2: mov       eax,DWORD PTR [rdi+0x460]
002941D8: mov       QWORD PTR [rsp+0x3d8],rbx
002941E0: mov       ebx,0x2
002941E5: cmp       DWORD PTR [rdi+0x1214],eax
002941EB: jne       0x180294257
002941ED: lea       rax,[rdi+0x146c]
002941F4: mov       edx,ebx
002941F6: lea       rcx,[rsp+0x70]
002941FB: nop       DWORD PTR [rax+rax*1+0x0]
00294200: lea       rcx,[rcx+0x80]
00294207: vmovups   ymm0,YMMWORD PTR [rax]
0029420B: vmovups   xmm1,XMMWORD PTR [rax+0x70]
00294210: lea       rax,[rax+0x80]
00294217: vmovups   YMMWORD PTR [rcx-0x80],ymm0
0029421C: vmovups   ymm0,YMMWORD PTR [rax-0x60]
00294221: vmovups   YMMWORD PTR [rcx-0x60],ymm0
00294226: vmovups   ymm0,YMMWORD PTR [rax-0x40]
0029422B: vmovups   YMMWORD PTR [rcx-0x40],ymm0
00294230: vmovups   xmm0,XMMWORD PTR [rax-0x20]
00294235: vmovups   XMMWORD PTR [rcx-0x20],xmm0
0029423A: vmovups   XMMWORD PTR [rcx-0x10],xmm1
0029423F: sub       rdx,0x1
00294243: jne       0x180294200
00294245: mov       eax,DWORD PTR [rax]
00294247: mov       DWORD PTR [rcx],eax
00294249: lea       rcx,[rsp+0x70]
0029424E: vzeroupper 
00294251: call      QWORD PTR [rip+0x1de981]        # 0x180472bd8 ; PDPerfPlugin.dll!SetCameraData
00294257: mov       r8d,DWORD PTR [rdi+0x460]
0029425E: cmp       r8d,DWORD PTR [rip+0x1cce7b]        # 0x1804610e0
00294265: mov       QWORD PTR [rsp+0x390],r14
0029426D: jne       0x18029438b
00294273: cmp       BYTE PTR [rip+0x1e1aca],0x0        # 0x180475d44
0029427A: je        0x18029438b
00294280: mov       r14b,0x1
00294283: cmp       DWORD PTR [rdi+0xfe8],r8d
0029428A: jne       0x1802948f6
00294290: lea       rax,[rdi+0x1368]
00294297: mov       rdx,rbx
0029429A: lea       rcx,[rsp+0x70]
0029429F: nop       
002942A0: lea       rcx,[rcx+0x80]
002942A7: vmovups   ymm0,YMMWORD PTR [rax]
002942AB: vmovups   xmm1,XMMWORD PTR [rax+0x70]
002942B0: lea       rax,[rax+0x80]
002942B7: vmovups   YMMWORD PTR [rcx-0x80],ymm0
002942BC: vmovups   ymm0,YMMWORD PTR [rax-0x60]
002942C1: vmovups   YMMWORD PTR [rcx-0x60],ymm0
002942C6: vmovups   ymm0,YMMWORD PTR [rax-0x40]
002942CB: vmovups   YMMWORD PTR [rcx-0x40],ymm0
002942D0: vmovups   xmm0,XMMWORD PTR [rax-0x20]
002942D5: vmovups   XMMWORD PTR [rcx-0x20],xmm0
002942DA: vmovups   XMMWORD PTR [rcx-0x10],xmm1
002942DF: sub       rdx,0x1
002942E3: jne       0x1802942a0
002942E5: mov       eax,DWORD PTR [rax]
002942E7: mov       DWORD PTR [rcx],eax
002942E9: lea       rcx,[rbp+0x160]
002942F0: vmovups   ymm0,YMMWORD PTR [rsp+0x70]
002942F6: vmovups   ymm1,YMMWORD PTR [rbp-0x70]
002942FB: vmovups   YMMWORD PTR [rbp-0x10],ymm0
00294300: vmovups   ymm0,YMMWORD PTR [rbp-0x50]
00294305: vmovups   YMMWORD PTR [rbp+0x10],ymm1
0029430A: vmovups   ymm1,YMMWORD PTR [rbp-0x30]
0029430F: vmovups   YMMWORD PTR [rbp+0x30],ymm0
00294314: vmovups   YMMWORD PTR [rbp+0x50],ymm1
00294319: mov       DWORD PTR [rbp+0x70],r8d
0029431D: lea       rax,[rsp+0x70]
00294322: nop       DWORD PTR [rax+0x0]
00294326: data16    nop WORD PTR [rax+rax*1+0x0]
00294330: lea       rcx,[rcx+0x80]
00294337: vmovups   ymm0,YMMWORD PTR [rax]
0029433B: vmovups   xmm1,XMMWORD PTR [rax+0x70]
00294340: lea       rax,[rax+0x80]
00294347: vmovups   YMMWORD PTR [rcx-0x80],ymm0
0029434C: vmovups   ymm0,YMMWORD PTR [rax-0x60]
00294351: vmovups   YMMWORD PTR [rcx-0x60],ymm0
00294356: vmovups   ymm0,YMMWORD PTR [rax-0x40]
0029435B: vmovups   YMMWORD PTR [rcx-0x40],ymm0
00294360: vmovups   xmm0,XMMWORD PTR [rax-0x20]
00294365: vmovups   XMMWORD PTR [rcx-0x20],xmm0
0029436A: vmovups   XMMWORD PTR [rcx-0x10],xmm1
0029436F: sub       rbx,0x1
00294373: jne       0x180294330
00294375: mov       eax,DWORD PTR [rax]
00294377: mov       DWORD PTR [rcx],eax
00294379: lea       rcx,[rbp+0x160]
00294380: vzeroupper 
00294383: call      QWORD PTR [rip+0x1de84f]        # 0x180472bd8 ; PDPerfPlugin.dll!SetCameraData
00294389: jmp       0x18029438e
0029438B: xor       r14b,r14b
0029438E: mov       r8,QWORD PTR [rdi+0xb50]
00294395: test      r8,r8
00294398: je        0x1802948f6
0029439E: cmp       QWORD PTR [rdi+0xba8],0x0
002943A6: je        0x1802948f6
002943AC: cmp       DWORD PTR [rip+0x1e134d],0x2        # 0x180475700
002943B3: jne       0x1802943e2
002943B5: cmp       DWORD PTR [rdi+0x3dc],0x2
002943BC: je        0x1802943c7
002943BE: cmp       BYTE PTR [rdi+0x4a5],0x0
002943C5: je        0x1802943e2
002943C7: cmp       BYTE PTR [rdi+0x48e],0x0
002943CE: je        0x1802943e2
002943D0: cmp       BYTE PTR [rdi],0x0
002943D3: jne       0x1802943de
002943D5: cmp       BYTE PTR [rdi+0x343],0x0
002943DC: je        0x1802943e2
002943DE: mov       dl,0x1
002943E0: jmp       0x1802943e4
002943E2: xor       dl,dl
002943E4: mov       rcx,rdi
002943E7: call      0x180294160
002943EC: test      al,al
002943EE: jne       0x1802943f8
002943F0: test      dl,dl
002943F2: je        0x1802948f6
002943F8: cmp       DWORD PTR [rdi+0x3dc],0x0
002943FF: jg        0x180294409
00294401: test      dl,dl
00294403: je        0x1802948f6
00294409: cmp       BYTE PTR [rdi+0x484],0x0
00294410: jne       0x1802948f6
00294416: mov       rax,QWORD PTR [r8]
00294419: lea       rdx,[rdi+0xb78]
00294420: mov       QWORD PTR [rsp+0x3a8],rsi
00294428: mov       rcx,r8
0029442B: mov       QWORD PTR [rsp+0x398],r13
00294433: call      QWORD PTR [rax+0x50]
00294436: vmovups   ymm1,YMMWORD PTR [rdi+0xb78]
0029443E: vmovsd    xmm0,QWORD PTR [rdi+0xb98]
00294446: mov       eax,DWORD PTR [rdi+0xba0]
0029444C: vmovd     r13d,xmm1
00294451: mov       DWORD PTR [rbp+0xa8],eax
00294457: vmovsd    QWORD PTR [rbp+0xa0],xmm0
0029445F: test      r13d,r13d
00294462: jne       0x18029446b
00294464: mov       r13d,DWORD PTR [rdi+0x34c]
0029446B: vmovq     rsi,xmm1
00294470: shr       rsi,0x20
00294474: test      esi,esi
00294476: jne       0x18029447e
00294478: mov       esi,DWORD PTR [rdi+0x350]
0029447E: test      r13d,r13d
00294481: jle       0x1802948e6
00294487: test      esi,esi
00294489: jle       0x1802948e6
0029448F: mov       QWORD PTR [rsp+0x3a0],r12
00294497: xor       r8d,r8d
0029449A: vmovaps   XMMWORD PTR [rsp+0x380],xmm6
002944A3: vmovaps   XMMWORD PTR [rsp+0x370],xmm7
002944AC: test      r15,r15
002944AF: je        0x1802944c0
002944B1: mov       rax,QWORD PTR [r15]
002944B4: mov       QWORD PTR [rbp+0x2d0],rax
002944BB: test      rax,rax
002944BE: jne       0x1802944c7
002944C0: mov       QWORD PTR [rbp+0x2d0],r8
002944C7: cmp       BYTE PTR [rdi+0x772],r8b
002944CE: je        0x1802944d9
002944D0: mov       QWORD PTR [rbp+0x2e0],r8
002944D7: jmp       0x1802944e7
002944D9: mov       rax,QWORD PTR [rdi+0xb50]
002944E0: mov       QWORD PTR [rbp+0x2e0],rax
002944E7: mov       rax,QWORD PTR [rdi+0xba8]
002944EE: mov       rcx,rdi
002944F1: mov       QWORD PTR [rbp+0x2e8],rax
002944F8: vzeroupper 
002944FB: call      0x180294110
00294500: test      al,al
00294502: je        0x18029451f
00294504: cmp       DWORD PTR [rdi+0x4fc],r8d
0029450B: je        0x180294516
0029450D: cmp       DWORD PTR [rdi+0x3dc],0x3
00294514: jne       0x18029451f
00294516: mov       r15,QWORD PTR [rdi+0x988]
0029451D: jmp       0x180294522
0029451F: mov       r15,r8
00294522: call      0x180294110
00294527: test      al,al
00294529: je        0x18029454f
0029452B: cmp       BYTE PTR [rdi+0x168b],r8b
00294532: jne       0x18029454f
00294534: cmp       DWORD PTR [rdi+0x3dc],0x3
0029453B: jne       0x180294546
0029453D: cmp       BYTE PTR [rdi+0x4a5],r8b
00294544: je        0x18029454f
00294546: mov       r12,QWORD PTR [rdi+0xa38]
0029454D: jmp       0x180294552
0029454F: mov       r12,r8
00294552: movzx     edx,BYTE PTR [rdi+0x4a5]
00294559: test      dl,dl
0029455B: je        0x18029456d
0029455D: call      0x180294110
00294562: test      al,al
00294564: je        0x18029456d
00294566: mov       r15,QWORD PTR [rdi+0x988]
0029456D: cmp       DWORD PTR [rdi+0x3dc],0x2
00294574: je        0x18029457a
00294576: test      dl,dl
00294578: je        0x180294589
0029457A: cmp       BYTE PTR [rdi+0x3d2],r8b
00294581: jne       0x180294589
00294583: mov       r15,r8
00294586: mov       r12,r8
00294589: cmp       BYTE PTR [rdi+0x265],r8b
00294590: je        0x18029459c
00294592: vmovss    xmm6,DWORD PTR [rdi+0x268]
0029459A: jmp       0x1802945a0
0029459C: vxorps    xmm6,xmm6,xmm6
002945A0: test      r14b,r14b
002945A3: je        0x1802945af
002945A5: vmovss    xmm7,DWORD PTR [rdi+0xff4]
002945AD: jmp       0x1802945db
002945AF: vxorps    xmm1,xmm1,xmm1
002945B3: vcvtsi2ss xmm1,xmm1,DWORD PTR [rdi+0x274]
002945BB: vxorps    xmm0,xmm0,xmm0
002945BF: vcvtsi2ss xmm0,xmm0,DWORD PTR [rdi+0x270]
002945C7: vdivss    xmm1,xmm1,xmm0
002945CB: vmulss    xmm2,xmm1,DWORD PTR [rdi+0x2f0]
002945D3: vmulss    xmm7,xmm2,DWORD PTR [rip+0x17767d]        # 0x18040bc58
002945DB: mov       rbx,r8
002945DE: mov       DWORD PTR [rsp+0x48],0x28
002945E6: vpxor     xmm0,xmm0,xmm0
002945EA: vmovups   XMMWORD PTR [rsp+0x58],xmm0
002945F0: mov       DWORD PTR [rsp+0x4c],0x1
002945F8: mov       QWORD PTR [rsp+0x50],r8
002945FD: mov       QWORD PTR [rsp+0x68],r8
00294602: cmp       BYTE PTR [rdi+0x4a5],r8b
00294609: je        0x1802946fe
0029460F: test      r14b,r14b
00294612: je        0x18029463f
00294614: vmovsd    xmm0,QWORD PTR [rdi+0xf38]
0029461C: vmovups   ymm1,YMMWORD PTR [rdi+0xf18]
00294624: mov       eax,DWORD PTR [rdi+0x460]
0029462A: vmovsd    QWORD PTR [rsp+0x40],xmm0
00294630: mov       DWORD PTR [rsp+0x40],eax
00294634: vmovups   YMMWORD PTR [rsp+0x20],ymm1
0029463A: jmp       0x1802946bf
0029463F: mov       r8d,DWORD PTR [rdi+0x460]
00294646: cmp       DWORD PTR [rdi+0x1198],r8d
0029464D: jne       0x180294661
0029464F: vmovups   ymm2,YMMWORD PTR [rdi+0x11e8]
00294657: vmovsd    xmm0,QWORD PTR [rdi+0x1208]
0029465F: jmp       0x18029467d
00294661: mov       rdx,QWORD PTR [rdi+0xb50]
00294668: lea       rcx,[rbp+0x80]
0029466F: call      0x1802607f0
00294674: vmovups   ymm2,YMMWORD PTR [rax]
00294678: vmovsd    xmm0,QWORD PTR [rax+0x20]
0029467D: mov       eax,DWORD PTR [rdi+0x460]
00294683: vmovsd    QWORD PTR [rbp+0xa0],xmm0
0029468B: vmovups   YMMWORD PTR [rsp+0x20],ymm2
00294691: vmovsd    QWORD PTR [rsp+0x40],xmm0
00294697: cmp       DWORD PTR [rdi+0x119c],eax
0029469D: jne       0x1802946bf
0029469F: mov       rcx,QWORD PTR [rdi+0xe10]
002946A6: test      rcx,rcx
002946A9: je        0x1802946bf
002946AB: mov       rax,QWORD PTR [rsp+0x28]
002946B0: cmp       DWORD PTR [rbp+0xa4],ebx
002946B6: cmovne    rax,rcx
002946BA: mov       QWORD PTR [rsp+0x28],rax
002946BF: lea       rcx,[rsp+0x20]
002946C4: vzeroupper 
002946C7: call      QWORD PTR [rip+0x1de57b]        # 0x180472c48 ; PDPerfPlugin.dll!SetPDFrameWarpEntityMask
002946CD: cmp       DWORD PTR [rsp+0x44],ebx
002946D1: je        0x1802946e3
002946D3: mov       eax,DWORD PTR [rdi+0x460]
002946D9: cmp       DWORD PTR [rsp+0x40],eax
002946DD: cmove     rbx,QWORD PTR [rsp+0x28]
002946E3: vmovups   ymm0,YMMWORD PTR [rsp+0x20]
002946E9: vmovsd    xmm1,QWORD PTR [rsp+0x40]
002946EF: vmovups   YMMWORD PTR [rsp+0x48],ymm0
002946F5: vmovsd    QWORD PTR [rsp+0x68],xmm1
002946FB: xor       r8d,r8d
002946FE: vmovss    xmm1,DWORD PTR [rdi+0xc]
00294703: mov       edx,DWORD PTR [rdi+0x460]
00294709: lea       rcx,[rbp+0xb0]
00294710: vpxor     xmm0,xmm0,xmm0
00294714: vmovups   YMMWORD PTR [rbp-0x60],ymm0
00294719: vmovups   XMMWORD PTR [rbp-0x40],xmm0
0029471E: vmovss    xmm0,DWORD PTR [rdi+0x8]
00294723: xor       eax,eax
00294725: mov       QWORD PTR [rsp+0x70],0x0
0029472E: mov       QWORD PTR [rbp-0x30],rax
00294732: mov       QWORD PTR [rbp-0x20],rax
00294736: mov       QWORD PTR [rbp-0x18],rax
0029473A: mov       QWORD PTR [rbp-0x10],rax
0029473E: mov       rax,QWORD PTR [rbp+0x2d0]
00294745: mov       QWORD PTR [rsp+0x78],rax
0029474A: mov       rax,QWORD PTR [rbp+0x2e8]
00294751: mov       QWORD PTR [rbp-0x80],rax
00294755: mov       rax,QWORD PTR [rbp+0x2e0]
0029475C: mov       QWORD PTR [rbp-0x78],rax
00294760: mov       eax,DWORD PTR [rdi+0x16a0]
00294766: vmovss    DWORD PTR [rbp-0x4c],xmm0
0029476B: vmovss    xmm0,DWORD PTR [rdi+0x2f8]
00294773: vmovss    DWORD PTR [rbp-0x48],xmm1
00294778: vmovss    xmm1,DWORD PTR [rdi+0x2f4]
00294780: vmovss    DWORD PTR [rbp-0x50],xmm6
00294785: vmovss    DWORD PTR [rbp-0x38],xmm0
0029478A: vmovss    DWORD PTR [rbp-0x34],xmm1
0029478F: vmovss    DWORD PTR [rbp-0x30],xmm7
00294794: mov       QWORD PTR [rbp-0x68],r8
00294798: dec       eax
0029479A: mov       DWORD PTR [rbp+0x10],eax
0029479D: lea       rax,[rsp+0x70]
002947A2: mov       QWORD PTR [rbp-0x28],r8
002947A6: mov       QWORD PTR [rbp-0x70],rbx
002947AA: mov       QWORD PTR [rbp-0x60],r8
002947AE: mov       QWORD PTR [rbp-0x8],r15
002947B2: mov       QWORD PTR [rbp+0x0],r12
002947B6: mov       BYTE PTR [rbp-0x3c],0x0
002947BA: mov       DWORD PTR [rbp+0x8],edx
002947BD: mov       BYTE PTR [rbp-0x2c],0x1
002947C1: mov       BYTE PTR [rbp+0xc],0x1
002947C5: mov       QWORD PTR [rbp+0x18],r8
002947C9: vxorps    xmm3,xmm3,xmm3
002947CD: vcvtsi2ss xmm3,xmm3,r13d
002947D2: vmovss    DWORD PTR [rbp-0x58],xmm3
002947D7: vmovss    DWORD PTR [rbp-0x44],xmm3
002947DC: vxorps    xmm2,xmm2,xmm2
002947E0: vcvtsi2ss xmm2,xmm2,esi
002947E4: vmovss    DWORD PTR [rbp-0x54],xmm2
002947E9: vmovss    DWORD PTR [rbp-0x40],xmm2
002947EE: vmovups   ymm0,YMMWORD PTR [rax]
002947F2: vmovups   YMMWORD PTR [rcx],ymm0
002947F6: vmovups   ymm0,YMMWORD PTR [rax+0x20]
002947FB: vmovups   YMMWORD PTR [rcx+0x20],ymm0
00294800: vmovups   ymm0,YMMWORD PTR [rax+0x40]
00294805: vmovups   YMMWORD PTR [rcx+0x40],ymm0
0029480A: vmovups   ymm0,YMMWORD PTR [rax+0x60]
0029480F: vmovups   YMMWORD PTR [rcx+0x60],ymm0
00294814: vmovups   ymm0,YMMWORD PTR [rax+0x80]
0029481C: vmovups   YMMWORD PTR [rcx+0x80],ymm0
00294824: vmovups   xmm0,XMMWORD PTR [rax+0xa0]
0029482C: vmovups   XMMWORD PTR [rcx+0xa0],xmm0
00294834: xor       ecx,ecx
00294836: vzeroupper 
00294839: call      QWORD PTR [rip+0x1de369]        # 0x180472ba8 ; PDPerfPlugin.dll!SetPDFrameWarpBackgroundInputColor
0029483F: cmp       DWORD PTR [rdi+0x3dc],0x2
00294846: vmovaps   xmm7,XMMWORD PTR [rsp+0x370]
0029484F: vmovaps   xmm6,XMMWORD PTR [rsp+0x380]
00294858: mov       r12,QWORD PTR [rsp+0x3a0]
00294860: je        0x18029486b
00294862: cmp       BYTE PTR [rdi+0x4a5],0x0
00294869: je        0x1802948ae
0029486B: test      r14b,r14b
0029486E: jne       0x18029488b
00294870: vmovss    xmm1,DWORD PTR [rbp+0x110]
00294878: mov       ecx,DWORD PTR [rdi+0x460]
0029487E: call      0x18027cce0
00294883: vmovss    DWORD PTR [rbp+0x110],xmm0
0029488B: cmp       BYTE PTR [rdi+0x343],0x0
00294892: jne       0x1802948ae
00294894: vmovss    xmm0,DWORD PTR [rdi+0x10]
00294899: vmovss    xmm1,DWORD PTR [rdi+0x14]
0029489E: vmovss    DWORD PTR [rbp+0xfc],xmm0
002948A6: vmovss    DWORD PTR [rbp+0x100],xmm1
002948AE: test      r14b,r14b
002948B1: jne       0x1802948d9
002948B3: cmp       BYTE PTR [rdi+0x4a5],r14b
002948BA: je        0x1802948d9
002948BC: vmovss    xmm0,DWORD PTR [rbp+0x110]
002948C4: lea       rdx,[rsp+0x48]
002948C9: mov       rcx,rdi
002948CC: vmovss    DWORD PTR [rdi+0xff4],xmm0
002948D4: call      0x180265f70
002948D9: lea       rcx,[rbp+0xb0]
002948E0: call      QWORD PTR [rip+0x1de372]        # 0x180472c58 ; PDPerfPlugin.dll!EvaluateFrameGeneration
002948E6: mov       r13,QWORD PTR [rsp+0x398]
002948EE: mov       rsi,QWORD PTR [rsp+0x3a8]
002948F6: mov       r14,QWORD PTR [rsp+0x390]
002948FE: mov       rbx,QWORD PTR [rsp+0x3d8]
00294906: vzeroupper 
00294909: add       rsp,0x3b0
00294910: pop       r15
00294912: pop       rdi
00294913: pop       rbp
00294914: ret       
