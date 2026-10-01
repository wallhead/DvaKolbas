; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xEE440..0xEE4CC; unnamed
000EE440: sub       rsp,0x58
000EE444: movaps    XMMWORD PTR [rsp+0x40],xmm6
000EE449: movaps    xmm6,xmm1
000EE44C: ucomiss   xmm6,DWORD PTR [rip+0x10be755]        # 0x1811acba8
000EE453: movaps    XMMWORD PTR [rsp+0x30],xmm7
000EE458: movaps    xmm7,xmm0
000EE45B: movaps    XMMWORD PTR [rsp+0x20],xmm8
000EE461: xorps     xmm8,xmm8
000EE465: jp        0x1800ee469
000EE467: je        0x1800ee47d
000EE469: movaps    xmm0,xmm6
000EE46C: call      0x18010d7c6
000EE471: cmp       ax,0x1
000EE475: jne       0x1800ee4b4
000EE477: comiss    xmm6,xmm8
000EE47B: jbe       0x1800ee4b4
000EE47D: movaps    xmm0,xmm7
000EE480: call      0x18010d7c6
000EE485: test      ax,ax
000EE488: jg        0x1800ee4b4
000EE48A: comiss    xmm8,xmm7
000EE48E: jae       0x1800ee4b4
000EE490: movsd     xmm0,QWORD PTR [rip+0x10be700]        # 0x1811acb98
000EE498: xorps     xmm1,xmm1
000EE49B: cvtss2sd  xmm1,xmm7
000EE49F: mulsd     xmm1,QWORD PTR [rip+0x10be6e1]        # 0x1811acb88
000EE4A7: minsd     xmm0,xmm1
000EE4AB: cvtpd2ps  xmm0,xmm0
000EE4AF: comiss    xmm0,xmm7
000EE4B2: ja        0x1800ee4b7
000EE4B4: movaps    xmm0,xmm6
000EE4B7: movaps    xmm6,XMMWORD PTR [rsp+0x40]
000EE4BC: movaps    xmm7,XMMWORD PTR [rsp+0x30]
000EE4C1: movaps    xmm8,XMMWORD PTR [rsp+0x20]
000EE4C7: add       rsp,0x58
000EE4CB: ret       
