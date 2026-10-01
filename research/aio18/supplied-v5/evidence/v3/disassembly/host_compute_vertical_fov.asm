; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x27CCE0..0x27CD4B; unnamed
0027CCE0: sub       rsp,0x38
0027CCE4: cmp       ecx,DWORD PTR [rip+0x1e43f6]        # 0x1804610e0
0027CCEA: vmovaps   xmm2,xmm1
0027CCEE: jne       0x18027cd3e
0027CCF0: cmp       BYTE PTR [rip+0x1f9049],0x0        # 0x180475d40
0027CCF7: je        0x18027cd2c
0027CCF9: vmovss    xmm0,DWORD PTR [rip+0x1f9253]        # 0x180475f54
0027CD01: vmovaps   XMMWORD PTR [rsp+0x20],xmm6
0027CD07: call      0x1802af9bd
0027CD0C: vmovaps   xmm6,xmm0
0027CD10: vmovss    xmm0,DWORD PTR [rip+0x1f9238]        # 0x180475f50
0027CD18: call      0x1802af9bd
0027CD1D: vsubss    xmm0,xmm6,xmm0
0027CD21: vmovaps   xmm6,XMMWORD PTR [rsp+0x20]
0027CD27: add       rsp,0x38
0027CD2B: ret       
0027CD2C: vmovss    xmm1,DWORD PTR [rip+0x1f9238]        # 0x180475f6c
0027CD34: vxorps    xmm0,xmm0,xmm0
0027CD38: vcomiss   xmm1,xmm0
0027CD3C: ja        0x18027cd42
0027CD3E: vmovaps   xmm1,xmm2
0027CD42: vmovaps   xmm0,xmm1
0027CD46: add       rsp,0x38
0027CD4A: ret       
