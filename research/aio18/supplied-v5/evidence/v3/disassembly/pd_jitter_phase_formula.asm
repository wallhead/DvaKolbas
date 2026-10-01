; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xFA2C0..0xFA2F4; unnamed
000FA2C0: sub       rsp,0x28
000FA2C4: movd      xmm0,edx
000FA2C8: movd      xmm1,ecx
000FA2CC: cvtdq2ps  xmm1,xmm1
000FA2CF: cvtdq2ps  xmm0,xmm0
000FA2D2: divss     xmm0,xmm1
000FA2D6: movss     xmm1,DWORD PTR [rip+0x10b284a]        # 0x1811acb28
000FA2DE: call      0x18010d7ea
000FA2E3: mulss     xmm0,DWORD PTR [rip+0x10b2871]        # 0x1811acb5c
000FA2EB: cvttss2si eax,xmm0
000FA2EF: add       rsp,0x28
000FA2F3: ret       
