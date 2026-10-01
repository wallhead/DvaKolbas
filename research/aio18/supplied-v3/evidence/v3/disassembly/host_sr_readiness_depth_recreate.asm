; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x296550..0x296795; unnamed
00296550: rex       push rbx
00296552: sub       rsp,0xa0
00296559: cmp       QWORD PTR [rcx+0x1678],0x0
00296561: mov       rbx,rcx
00296564: je        0x180296595
00296566: cmp       QWORD PTR [rcx+0x1670],0x0
0029656E: je        0x180296595
00296570: xor       ecx,ecx
00296572: call      0x1802a4410
00296577: test      rax,rax
0029657A: je        0x180296595
0029657C: mov       rdx,QWORD PTR [rax]
0029657F: test      rdx,rdx
00296582: je        0x180296595
00296584: cmp       rdx,QWORD PTR [rbx+0x7d0]
0029658B: je        0x180296595
0029658D: mov       rcx,rbx
00296590: call      0x180293c30
00296595: movzx     eax,BYTE PTR [rbx+0x2fc]
0029659C: mov       QWORD PTR [rsp+0xb8],rdi
002965A4: vmovaps   XMMWORD PTR [rsp+0x90],xmm6
002965AD: vmovaps   XMMWORD PTR [rsp+0x80],xmm7
002965B6: cmp       BYTE PTR [rbx+0x46c],al
002965BC: je        0x180296609
002965BE: vmovups   xmm0,XMMWORD PTR [rbx+0x464]
002965C6: vmovsd    xmm6,QWORD PTR [rbx+0x474]
002965CE: vmovups   XMMWORD PTR [rsp+0x20],xmm0
002965D4: mov       BYTE PTR [rsp+0x28],al
002965D8: lea       rcx,[rsp+0x20]
002965DD: vmovups   xmm7,XMMWORD PTR [rsp+0x20]
002965E3: vmovups   XMMWORD PTR [rsp+0x20],xmm7
002965E9: vmovsd    QWORD PTR [rsp+0x30],xmm6
002965EF: call      QWORD PTR [rip+0x1dc5fb]        # 0x180472bf0 ; PDPerfPlugin.dll!SetFrameGenParams
002965F5: test      al,al
002965F7: je        0x18029666d
002965F9: vmovups   XMMWORD PTR [rbx+0x464],xmm7
00296601: vmovsd    QWORD PTR [rbx+0x474],xmm6
00296609: cmp       BYTE PTR [rbx+0x338],0x0
00296610: je        0x180296744
00296616: movzx     eax,BYTE PTR [rbx+0x2fc]
0029661D: cmp       BYTE PTR [rbx+0x319],al
00296623: je        0x180296744
00296629: vmovups   ymm0,YMMWORD PTR [rbx+0x300]
00296631: vmovups   xmm1,XMMWORD PTR [rbx+0x320]
00296639: vmovups   YMMWORD PTR [rsp+0x40],ymm0
0029663F: vmovsd    xmm0,QWORD PTR [rbx+0x330]
00296647: vmovsd    QWORD PTR [rsp+0x70],xmm0
0029664D: vmovups   XMMWORD PTR [rsp+0x60],xmm1
00296653: mov       BYTE PTR [rsp+0x59],al
00296657: lea       rcx,[rsp+0x40]
0029665C: vzeroupper 
0029665F: call      QWORD PTR [rip+0x1dc5a3]        # 0x180472c08 ; PDPerfPlugin.dll!InitUpscaler
00296665: mov       rdi,rax
00296668: test      rax,rax
0029666B: jne       0x180296674
0029666D: xor       al,al
0029666F: jmp       0x18029676f
00296674: cmp       BYTE PTR [rbx+0x343],0x0
0029667B: mov       QWORD PTR [rsp+0xb0],rsi
00296683: mov       QWORD PTR [rsp+0xc0],r14
0029668B: jne       0x1802966e7
0029668D: mov       rax,QWORD PTR [rbx+0x778]
00296694: mov       dl,0x1
00296696: cmp       QWORD PTR [rbx+0xa38],rax
0029669D: jne       0x1802966d2
0029669F: lea       rcx,[rbx+0xa38]
002966A6: call      0x180152cd0
002966AB: mov       dl,0x1
002966AD: lea       rcx,[rbx+0x778]
002966B4: call      0x180152cd0
002966B9: mov       QWORD PTR [rbx+0x778],rdi
002966C0: mov       rcx,rdi
002966C3: mov       QWORD PTR [rbx+0xa38],rdi
002966CA: mov       rax,QWORD PTR [rdi]
002966CD: call      QWORD PTR [rax+0x8]
002966D0: jmp       0x1802966f0
002966D2: lea       rcx,[rbx+0x778]
002966D9: call      0x180152cd0
002966DE: mov       QWORD PTR [rbx+0x778],rdi
002966E5: jmp       0x1802966f0
002966E7: mov       rax,QWORD PTR [rax]
002966EA: mov       rcx,rdi
002966ED: call      QWORD PTR [rax+0x10]
002966F0: vmovss    xmm1,DWORD PTR [rbx+0x10]
002966F5: xor       ecx,ecx
002966F7: call      QWORD PTR [rip+0x1dc513]        # 0x180472c10 ; PDPerfPlugin.dll!SetMotionScaleX
002966FD: vmovss    xmm1,DWORD PTR [rbx+0x14]
00296702: xor       ecx,ecx
00296704: call      QWORD PTR [rip+0x1dc50e]        # 0x180472c18 ; PDPerfPlugin.dll!SetMotionScaleY
0029670A: vmovups   ymm0,YMMWORD PTR [rsp+0x40]
00296710: vmovups   xmm1,XMMWORD PTR [rsp+0x60]
00296716: mov       r14,QWORD PTR [rsp+0xc0]
0029671E: mov       rsi,QWORD PTR [rsp+0xb0]
00296726: vmovups   YMMWORD PTR [rbx+0x300],ymm0
0029672E: vmovsd    xmm0,QWORD PTR [rsp+0x70]
00296734: vmovups   XMMWORD PTR [rbx+0x320],xmm1
0029673C: vmovsd    QWORD PTR [rbx+0x330],xmm0
00296744: cmp       BYTE PTR [rbx+0x2fe],0x0
0029674B: je        0x18029676d
0029674D: mov       BYTE PTR [rbx+0x2fe],0x0
00296754: mov       rcx,rbx
00296757: mov       BYTE PTR [rbx+0x264],0x1
0029675E: vzeroupper 
00296761: call      0x180266db0
00296766: mov       BYTE PTR [rbx+0x1190],0x0
0029676D: mov       al,0x1
0029676F: vzeroupper 
00296772: vmovaps   xmm7,XMMWORD PTR [rsp+0x80]
0029677B: vmovaps   xmm6,XMMWORD PTR [rsp+0x90]
00296784: mov       rdi,QWORD PTR [rsp+0xb8]
0029678C: add       rsp,0xa0
00296793: pop       rbx
00296794: ret       
