; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x299750..0x299789; unnamed
00299750: sub       rsp,0x28
00299754: lea       rcx,[rip+0x16d6ad]        # 0x180406e08 ; 'd3d11.dll'
0029975B: call      QWORD PTR [rip+0x299df]        # 0x1802c3140
00299761: test      rax,rax
00299764: je        0x180299782
00299766: lea       rdx,[rip+0x16c613]        # 0x180405d80 ; 'ENBGetSDKVersion'
0029976D: mov       rcx,rax
00299770: call      QWORD PTR [rip+0x299e2]        # 0x1802c3158
00299776: test      rax,rax
00299779: je        0x180299782
0029977B: mov       al,0x1
0029977D: add       rsp,0x28
00299781: ret       
00299782: xor       al,al
00299784: add       rsp,0x28
00299788: ret       
