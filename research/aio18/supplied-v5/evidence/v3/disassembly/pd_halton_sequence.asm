; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xFA300..0xFA38F; unnamed
000FA300: rex       push rbx
000FA302: sub       rsp,0x50
000FA306: movaps    XMMWORD PTR [rsp+0x40],xmm6
000FA30B: mov       ebx,edx
000FA30D: movss     xmm6,DWORD PTR [rip+0x10b27bb]        # 0x1811acad0
000FA315: movaps    XMMWORD PTR [rsp+0x30],xmm7
000FA31A: xorps     xmm7,xmm7
000FA31D: test      ecx,ecx
000FA31F: jle       0x1800fa37c
000FA321: movaps    XMMWORD PTR [rsp+0x20],xmm8
000FA327: movd      xmm8,edx
000FA32C: cvtdq2ps  xmm8,xmm8
000FA330: mov       eax,ecx
000FA332: movd      xmm0,ecx
000FA336: cdq       
000FA337: idiv      ebx
000FA339: cvtdq2ps  xmm0,xmm0
000FA33C: divss     xmm6,xmm8
000FA341: movd      xmm1,edx
000FA345: cvtdq2ps  xmm1,xmm1
000FA348: divss     xmm0,xmm8
000FA34D: mulss     xmm1,xmm6
000FA351: addss     xmm7,xmm1
000FA355: call      0x18010d7de
000FA35A: cvttss2si rcx,xmm0
000FA35F: test      ecx,ecx
000FA361: jg        0x1800fa330
000FA363: movaps    xmm8,XMMWORD PTR [rsp+0x20]
000FA369: movaps    xmm0,xmm7
000FA36C: movaps    xmm6,XMMWORD PTR [rsp+0x40]
000FA371: movaps    xmm7,XMMWORD PTR [rsp+0x30]
000FA376: add       rsp,0x50
000FA37A: pop       rbx
000FA37B: ret       
000FA37C: movaps    xmm6,XMMWORD PTR [rsp+0x40]
000FA381: xorps     xmm0,xmm0
000FA384: movaps    xmm7,XMMWORD PTR [rsp+0x30]
000FA389: add       rsp,0x50
000FA38D: pop       rbx
000FA38E: ret       
