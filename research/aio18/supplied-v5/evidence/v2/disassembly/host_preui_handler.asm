; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x19F420..0x19FD89; unnamed
0019F420: rex       push rbp
0019F422: push      rbx
0019F423: push      rdi
0019F424: lea       rbp,[rsp-0x47]
0019F429: sub       rsp,0xc0
0019F430: mov       rax,QWORD PTR [rip+0xce19a9]        # 0x180e80de0
0019F437: mov       ebx,edx
0019F439: mov       rdi,rcx
0019F43C: cmp       BYTE PTR [rax+0x343],0x0
0019F443: je        0x18019f552
0019F449: call      0x180152f00
0019F44E: mov       rcx,rax
0019F451: call      0x18025edd0
0019F456: test      al,al
0019F458: jne       0x18019f464
0019F45A: call      0x180264ff0
0019F45F: call      0x180297e90
0019F464: call      0x180297bd0
0019F469: call      0x180152f00
0019F46E: mov       rcx,rax
0019F471: call      0x18025edd0
0019F476: test      al,al
0019F478: jne       0x18019fd7e
0019F47E: mov       rcx,QWORD PTR [rip+0xce195b]        # 0x180e80de0
0019F485: call      0x180293f20
0019F48A: test      al,al
0019F48C: je        0x18019fd7e
0019F492: mov       rcx,QWORD PTR [rip+0xce1947]        # 0x180e80de0
0019F499: cmp       BYTE PTR [rcx+0x3d5],0x0
0019F4A0: je        0x18019fd7e
0019F4A6: add       rcx,0x9e0
0019F4AD: call      0x18016c140
0019F4B2: test      rax,rax
0019F4B5: je        0x18019fd7e
0019F4BB: mov       rcx,QWORD PTR [rip+0xce191e]        # 0x180e80de0
0019F4C2: add       rcx,0x9e0
0019F4C9: call      0x18016c140
0019F4CE: mov       rcx,QWORD PTR [rip+0xce190b]        # 0x180e80de0
0019F4D5: lea       r8,[rbp+0x7]
0019F4D9: mov       QWORD PTR [rbp+0x77],rax
0019F4DD: vpxor     xmm0,xmm0,xmm0
0019F4E1: vmovups   XMMWORD PTR [rbp+0x7],xmm0
0019F4E6: mov       rcx,QWORD PTR [rcx+0x1680]
0019F4ED: mov       rdx,QWORD PTR [rcx]
0019F4F0: mov       r9,QWORD PTR [rdx+0x190]
0019F4F7: mov       rdx,rax
0019F4FA: call      r9
0019F4FD: mov       rcx,QWORD PTR [rip+0xce18dc]        # 0x180e80de0
0019F504: mov       rdi,QWORD PTR [rcx+0x1680]
0019F50B: add       rcx,0x8d8
0019F512: mov       rax,QWORD PTR [rdi]
0019F515: mov       rbx,QWORD PTR [rax+0x108]
0019F51C: call      0x18016c450
0019F521: mov       r9,rax
0019F524: lea       r8,[rbp+0x77]
0019F528: mov       edx,0x1
0019F52D: mov       rcx,rdi
0019F530: call      rbx
0019F532: mov       rax,QWORD PTR [rip+0xce18a7]        # 0x180e80de0
0019F539: mov       BYTE PTR [rip+0xce1b52],0x1        # 0x180e81092
0019F540: mov       BYTE PTR [rax+0x3d3],0x1
0019F547: add       rsp,0xc0
0019F54E: pop       rdi
0019F54F: pop       rbx
0019F550: pop       rbp
0019F551: ret       
0019F552: lea       rcx,[rbp+0x7]
0019F556: mov       QWORD PTR [rbp+0x7],0x80d73
0019F55E: mov       QWORD PTR [rbp+0xf],0x653c4
0019F566: call      0x1801365e0
0019F56B: mov       rax,QWORD PTR [rax]
0019F56E: test      rax,rax
0019F571: je        0x18019f57e
0019F573: mov       rax,QWORD PTR [rax+0x1f0]
0019F57A: mov       BYTE PTR [rax+0x18],0x0
0019F57E: mov       edx,ebx
0019F580: mov       rcx,rdi
0019F583: call      QWORD PTR [rip+0x2dde27]        # 0x18047d3b0
0019F589: cmp       BYTE PTR [rip+0xce1a92],0x0        # 0x180e81022
0019F590: mov       BYTE PTR [rip+0xce1a89],0x0        # 0x180e81020
0019F597: je        0x18019f5c8
0019F599: call      0x1801ac3d0
0019F59E: test      rax,rax
0019F5A1: je        0x18019f5c8
0019F5A3: lea       rdx,[rip+0x24614e]        # 0x1803e56f8
0019F5AA: mov       rcx,rax
0019F5AD: call      0x1801ac480
0019F5B2: test      al,al
0019F5B4: je        0x18019f5c8
0019F5B6: mov       BYTE PTR [rip+0xce1a63],0x1        # 0x180e81020
0019F5BD: add       rsp,0xc0
0019F5C4: pop       rdi
0019F5C5: pop       rbx
0019F5C6: pop       rbp
0019F5C7: ret       
0019F5C8: mov       rcx,QWORD PTR [rip+0xce1951]        # 0x180e80f20
0019F5CF: mov       QWORD PTR [rsp+0xb0],r13
0019F5D7: mov       rax,QWORD PTR [rcx]
0019F5DA: call      QWORD PTR [rax+0x120]
0019F5E0: mov       rcx,QWORD PTR [rip+0xce17f9]        # 0x180e80de0
0019F5E7: mov       r13d,eax
0019F5EA: call      0x180298370 ; '@USVWAVH'
0019F5EF: mov       rcx,QWORD PTR [rip+0xce17ea]        # 0x180e80de0
0019F5F6: call      0x180298030
0019F5FB: test      al,al
0019F5FD: je        0x18019f6c6
0019F603: mov       rcx,QWORD PTR [rip+0xce17d6]        # 0x180e80de0
0019F60A: add       rcx,0x8d8
0019F611: call      0x18016c450
0019F616: mov       rcx,QWORD PTR [rip+0xce17c3]        # 0x180e80de0
0019F61D: mov       rbx,rax
0019F620: add       rcx,0x9e0
0019F627: call      0x18016c140
0019F62C: mov       rcx,QWORD PTR [rip+0xce18ed]        # 0x180e80f20
0019F633: lea       r8,[rbp+0x77]
0019F637: mov       QWORD PTR [rbp+0x77],rax
0019F63B: mov       r9,rbx
0019F63E: mov       rcx,QWORD PTR [rcx+0x250]
0019F645: mov       rdx,QWORD PTR [rcx]
0019F648: mov       r10,QWORD PTR [rdx+0x108]
0019F64F: mov       edx,0x1
0019F654: call      r10
0019F657: mov       rax,QWORD PTR [rip+0xce18c2]        # 0x180e80f20
0019F65E: lea       r8,[rbp+0x7]
0019F662: mov       rdx,QWORD PTR [rbp+0x77]
0019F666: vxorps    xmm0,xmm0,xmm0
0019F66A: vmovups   XMMWORD PTR [rbp+0x7],xmm0
0019F66F: mov       rcx,QWORD PTR [rax+0x250]
0019F676: mov       BYTE PTR [rip+0xce1a15],0x1        # 0x180e81092
0019F67D: mov       rax,QWORD PTR [rcx]
0019F680: call      QWORD PTR [rax+0x190]
0019F686: mov       rax,QWORD PTR [rip+0xce1753]        # 0x180e80de0
0019F68D: mov       r8d,0x3
0019F693: vmovss    xmm3,DWORD PTR [rip+0x26c699]        # 0x18040bd34
0019F69B: mov       rdx,rbx
0019F69E: mov       BYTE PTR [rsp+0x20],0x0
0019F6A3: mov       BYTE PTR [rax+0x3d3],0x1
0019F6AA: mov       rax,QWORD PTR [rip+0xce186f]        # 0x180e80f20
0019F6B1: mov       rcx,QWORD PTR [rax+0x250]
0019F6B8: mov       rax,QWORD PTR [rcx]
0019F6BB: call      QWORD PTR [rax+0x1a8]
0019F6C1: jmp       0x18019fd76
0019F6C6: mov       QWORD PTR [rsp+0xa0],r15
0019F6CE: call      0x180152f00
0019F6D3: mov       rcx,rax
0019F6D6: call      0x18025edd0
0019F6DB: mov       rcx,QWORD PTR [rip+0xce16fe]        # 0x180e80de0
0019F6E2: test      al,al
0019F6E4: jne       0x18019fbff
0019F6EA: mov       BYTE PTR [rcx+0x489],0x1
0019F6F1: mov       eax,DWORD PTR [rip+0x2c19e9]        # 0x1804610e0
0019F6F7: mov       QWORD PTR [rsp+0xa8],r14
0019F6FF: mov       QWORD PTR [rsp+0xb8],r12
0019F707: cmp       DWORD PTR [rcx+0x460],eax
0019F70D: jne       0x18019f71d
0019F70F: cmp       BYTE PTR [rip+0x2d662e],0x0        # 0x180475d44
0019F716: je        0x18019f71d
0019F718: mov       r12b,0x1
0019F71B: jmp       0x18019f720
0019F71D: xor       r12b,r12b
0019F720: xor       r14d,r14d
0019F723: mov       QWORD PTR [rsp+0xe0],rsi
0019F72B: cmp       BYTE PTR [rcx],r14b
0019F72E: je        0x18019f9fb
0019F734: cmp       QWORD PTR [rcx+0x1690],r14
0019F73B: je        0x18019f9fb
0019F741: cmp       BYTE PTR [rcx+0x487],r14b
0019F748: jne       0x18019f766
0019F74A: test      r12b,r12b
0019F74D: je        0x18019f9fb
0019F753: mov       edi,DWORD PTR [rcx+0x270]
0019F759: mov       ebx,0x1
0019F75E: mov       esi,DWORD PTR [rcx+0x274]
0019F764: jmp       0x18019f775
0019F766: mov       edi,DWORD PTR [rcx+0x278]
0019F76C: mov       ebx,r14d
0019F76F: mov       esi,DWORD PTR [rcx+0x27c]
0019F775: mov       rcx,QWORD PTR [rip+0xce17a4]        # 0x180e80f20
0019F77C: add       rcx,0x148
0019F783: mov       DWORD PTR [rbp+0x77],ebx
0019F786: call      0x18016c290
0019F78B: mov       rcx,QWORD PTR [rip+0xce178e]        # 0x180e80f20
0019F792: add       rcx,0x40
0019F796: mov       QWORD PTR [rbp+0x7f],rax
0019F79A: imul      r15,r13,0x58
0019F79E: add       rcx,r15
0019F7A1: call      0x18016c140
0019F7A6: mov       rcx,QWORD PTR [rip+0xce1633]        # 0x180e80de0
0019F7AD: xor       r9d,r9d
0019F7B0: mov       QWORD PTR [rsp+0x68],r14
0019F7B5: mov       r8d,ebx
0019F7B8: mov       BYTE PTR [rsp+0x60],r14b
0019F7BD: mov       edx,0x1
0019F7C2: mov       DWORD PTR [rsp+0x58],r14d
0019F7C7: mov       DWORD PTR [rsp+0x50],r14d
0019F7CC: mov       DWORD PTR [rsp+0x48],esi
0019F7D0: mov       DWORD PTR [rsp+0x40],edi
0019F7D4: mov       QWORD PTR [rsp+0x38],rax
0019F7D9: lea       rax,[rbp+0x7f]
0019F7DD: mov       QWORD PTR [rsp+0x30],r14
0019F7E2: mov       QWORD PTR [rsp+0x28],rax
0019F7E7: mov       DWORD PTR [rsp+0x20],0x1
0019F7EF: call      0x1802a1240
0019F7F4: mov       rcx,QWORD PTR [rip+0xce15e5]        # 0x180e80de0
0019F7FB: add       rcx,0x7d0
0019F802: call      0x18016c290
0019F807: mov       rcx,QWORD PTR [rip+0xce15d2]        # 0x180e80de0
0019F80E: add       rcx,0x8d8
0019F815: mov       QWORD PTR [rbp+0x7f],rax
0019F819: call      0x18016c450
0019F81E: mov       rcx,QWORD PTR [rip+0xce15bb]        # 0x180e80de0
0019F825: xor       r9d,r9d
0019F828: mov       QWORD PTR [rsp+0x68],r14
0019F82D: xor       r8d,r8d
0019F830: mov       BYTE PTR [rsp+0x60],r14b
0019F835: mov       edx,0x2
0019F83A: mov       DWORD PTR [rsp+0x58],r14d
0019F83F: mov       DWORD PTR [rsp+0x50],r14d
0019F844: mov       DWORD PTR [rsp+0x48],esi
0019F848: mov       DWORD PTR [rsp+0x40],edi
0019F84C: mov       QWORD PTR [rsp+0x38],r14
0019F851: mov       QWORD PTR [rsp+0x30],rax
0019F856: lea       rax,[rbp+0x7f]
0019F85A: mov       QWORD PTR [rsp+0x28],rax
0019F85F: mov       DWORD PTR [rsp+0x20],0x1
0019F867: call      0x1802a1240
0019F86C: mov       rcx,QWORD PTR [rip+0xce156d]        # 0x180e80de0
0019F873: add       rcx,0x8d8
0019F87A: call      0x18016c290
0019F87F: mov       rcx,QWORD PTR [rip+0xce155a]        # 0x180e80de0
0019F886: mov       r9,rax
0019F889: mov       r8,rax
0019F88C: mov       rcx,QWORD PTR [rcx+0x1690]
0019F893: mov       rdx,QWORD PTR [rcx]
0019F896: mov       r10,QWORD PTR [rdx+0x170]
0019F89D: lea       rdx,[rip+0x24950c]        # 0x1803e8db0
0019F8A4: call      r10
0019F8A7: mov       rcx,QWORD PTR [rip+0xce1672]        # 0x180e80f20
0019F8AE: add       rcx,0x40
0019F8B2: add       rcx,r15
0019F8B5: call      0x18016c140
0019F8BA: mov       rcx,QWORD PTR [rip+0xce165f]        # 0x180e80f20
0019F8C1: mov       rdi,rax
0019F8C4: add       rcx,0x40
0019F8C8: add       rcx,r15
0019F8CB: call      0x18016c010
0019F8D0: mov       rbx,rax
0019F8D3: mov       rax,QWORD PTR [rip+0xce1506]        # 0x180e80de0
0019F8DA: mov       r14,QWORD PTR [rax+0x1690]
0019F8E1: mov       rcx,r14
0019F8E4: mov       rsi,QWORD PTR [r14]
0019F8E7: call      QWORD PTR [rsi+0x40]
0019F8EA: mov       rcx,rax
0019F8ED: mov       rdx,QWORD PTR [rax]
0019F8F0: call      QWORD PTR [rdx+0x38]
0019F8F3: mov       r9,rbx
0019F8F6: mov       r8,rdi
0019F8F9: mov       rdx,rax
0019F8FC: mov       rcx,r14
0019F8FF: call      QWORD PTR [rsi+0x48]
0019F902: mov       rax,QWORD PTR [rip+0xce14d7]        # 0x180e80de0
0019F909: mov       rcx,QWORD PTR [rip+0xce1610]        # 0x180e80f20
0019F910: vxorps    xmm0,xmm0,xmm0
0019F914: vxorps    xmm3,xmm3,xmm3
0019F918: vcvtsi2ss xmm0,xmm0,DWORD PTR [rax+0x278]
0019F920: vcvtsi2ss xmm3,xmm3,DWORD PTR [rax+0x270]
0019F928: vdivss    xmm1,xmm0,xmm3
0019F92C: vxorps    xmm0,xmm0,xmm0
0019F930: vcvtsi2ss xmm0,xmm0,DWORD PTR [rax+0x27c]
0019F938: vxorps    xmm2,xmm2,xmm2
0019F93C: vcvtsi2ss xmm2,xmm2,DWORD PTR [rax+0x274]
0019F944: vmovss    DWORD PTR [rax+0x1618],xmm1
0019F94C: add       rcx,0x40
0019F950: vdivss    xmm1,xmm0,xmm2
0019F954: add       rcx,r15
0019F957: vmovss    DWORD PTR [rax+0x161c],xmm1
0019F95F: vmovss    DWORD PTR [rax+0x1620],xmm3
0019F967: vmovss    DWORD PTR [rax+0x1624],xmm2
0019F96F: call      0x18016c290
0019F974: mov       rcx,QWORD PTR [rip+0xce15a5]        # 0x180e80f20
0019F97B: mov       QWORD PTR [rbp+0x7f],rax
0019F97F: add       rcx,0x148
0019F986: mov       rax,QWORD PTR [rip+0xce1453]        # 0x180e80de0
0019F98D: mov       ebx,DWORD PTR [rax+0x27c]
0019F993: mov       edi,DWORD PTR [rax+0x278]
0019F999: call      0x18016c140
0019F99E: mov       r8d,DWORD PTR [rbp+0x77]
0019F9A2: xor       ecx,ecx
0019F9A4: mov       QWORD PTR [rsp+0x68],rcx
0019F9A9: mov       edx,r8d
0019F9AC: mov       BYTE PTR [rsp+0x60],cl
0019F9B0: xor       edx,0x1
0019F9B3: mov       DWORD PTR [rsp+0x58],ecx
0019F9B7: xor       r9d,r9d
0019F9BA: mov       DWORD PTR [rsp+0x50],ecx
0019F9BE: mov       DWORD PTR [rsp+0x48],ebx
0019F9C2: mov       DWORD PTR [rsp+0x40],edi
0019F9C6: lea       edx,[rdx*2+0x1]
0019F9CD: mov       QWORD PTR [rsp+0x38],rax
0019F9D2: lea       rax,[rbp+0x7f]
0019F9D6: mov       QWORD PTR [rsp+0x30],rcx
0019F9DB: mov       rcx,QWORD PTR [rip+0xce13fe]        # 0x180e80de0
0019F9E2: mov       QWORD PTR [rsp+0x28],rax
0019F9E7: mov       DWORD PTR [rsp+0x20],0x1
0019F9EF: call      0x1802a1240
0019F9F4: mov       rcx,QWORD PTR [rip+0xce13e5]        # 0x180e80de0
0019F9FB: mov       rax,QWORD PTR [rip+0xce151e]        # 0x180e80f20
0019FA02: imul      r15,r13,0x58
0019FA06: lea       rdx,[rax+0x40]
0019FA0A: add       rdx,r15
0019FA0D: lea       r8,[rax+0x148]
0019FA14: call      0x1802949c0 ; '@UVATAVH'
0019FA19: mov       rcx,QWORD PTR [rip+0xce13c0]        # 0x180e80de0
0019FA20: cmp       QWORD PTR [rcx+0x1690],0x0
0019FA28: je        0x18019fb09
0019FA2E: cmp       BYTE PTR [rcx+0x487],0x0
0019FA35: jne       0x18019fb09
0019FA3B: test      r12b,r12b
0019FA3E: jne       0x18019fb09
0019FA44: add       rcx,0x7d0
0019FA4B: call      0x18016c290
0019FA50: mov       rcx,QWORD PTR [rip+0xce1389]        # 0x180e80de0
0019FA57: lea       rdx,[rip+0x249352]        # 0x1803e8db0
0019FA5E: mov       r9,rax
0019FA61: mov       rcx,QWORD PTR [rcx+0x1690]
0019FA68: mov       r8,QWORD PTR [rcx]
0019FA6B: mov       r10,QWORD PTR [r8+0x170]
0019FA72: mov       r8,rax
0019FA75: call      r10
0019FA78: mov       rcx,QWORD PTR [rip+0xce14a1]        # 0x180e80f20
0019FA7F: add       rcx,0x40
0019FA83: add       rcx,r15
0019FA86: call      0x18016c140
0019FA8B: mov       rcx,QWORD PTR [rip+0xce148e]        # 0x180e80f20
0019FA92: mov       rdi,rax
0019FA95: add       rcx,0x40
0019FA99: add       rcx,r15
0019FA9C: call      0x18016c010
0019FAA1: mov       rbx,rax
0019FAA4: mov       rax,QWORD PTR [rip+0xce1335]        # 0x180e80de0
0019FAAB: mov       r14,QWORD PTR [rax+0x1690]
0019FAB2: mov       rcx,r14
0019FAB5: mov       rsi,QWORD PTR [r14]
0019FAB8: call      QWORD PTR [rsi+0x40]
0019FABB: mov       rcx,rax
0019FABE: mov       rdx,QWORD PTR [rax]
0019FAC1: call      QWORD PTR [rdx+0x38]
0019FAC4: mov       r9,rbx
0019FAC7: mov       r8,rdi
0019FACA: mov       rdx,rax
0019FACD: mov       rcx,r14
0019FAD0: call      QWORD PTR [rsi+0x48]
0019FAD3: mov       rcx,QWORD PTR [rip+0xce1306]        # 0x180e80de0
0019FADA: mov       rdx,QWORD PTR [rcx+0x778]
0019FAE1: test      rdx,rdx
0019FAE4: je        0x18019fb09
0019FAE6: mov       rcx,QWORD PTR [rcx+0x1680]
0019FAED: mov       r8,QWORD PTR [rip+0xce142c]        # 0x180e80f20
0019FAF4: mov       rax,QWORD PTR [rcx]
0019FAF7: mov       r8,QWORD PTR [r15+r8*1+0x40]
0019FAFC: call      QWORD PTR [rax+0x178]
0019FB02: mov       rcx,QWORD PTR [rip+0xce12d7]        # 0x180e80de0
0019FB09: cmp       BYTE PTR [rcx+0x48d],0x0
0019FB10: mov       r14,QWORD PTR [rsp+0xa8]
0019FB18: mov       r12,QWORD PTR [rsp+0xb8]
0019FB20: mov       rsi,QWORD PTR [rsp+0xe0]
0019FB28: mov       BYTE PTR [rcx+0x489],0x0
0019FB2F: je        0x18019fbff
0019FB35: mov       edi,DWORD PTR [rcx+0x270]
0019FB3B: mov       ebx,DWORD PTR [rcx+0x274]
0019FB41: add       rcx,0x828
0019FB48: call      0x18016c290
0019FB4D: mov       rcx,QWORD PTR [rip+0xce13cc]        # 0x180e80f20
0019FB54: add       rcx,0x40
0019FB58: mov       QWORD PTR [rbp+0x77],rax
0019FB5C: add       rcx,r15
0019FB5F: call      0x18016c140
0019FB64: mov       r10,rax
0019FB67: mov       eax,ebx
0019FB69: cdq       
0019FB6A: and       edx,0x3
0019FB6D: vxorps    xmm0,xmm0,xmm0
0019FB71: vcvtsi2ss xmm0,xmm0,ebx
0019FB75: vmulss    xmm0,xmm0,DWORD PTR [rip+0x26c0df]        # 0x18040bc5c
0019FB7D: lea       ecx,[rdx+rax*1]
0019FB80: mov       eax,edi
0019FB82: cdq       
0019FB83: sar       ecx,0x2
0019FB86: and       edx,0x3
0019FB89: add       eax,edx
0019FB8B: xor       edx,edx
0019FB8D: mov       QWORD PTR [rsp+0x68],rdx
0019FB92: mov       BYTE PTR [rsp+0x60],dl
0019FB96: vcvttss2si r9d,xmm0
0019FB9A: mov       DWORD PTR [rsp+0x58],r9d
0019FB9F: mov       r9d,0x1
0019FBA5: sar       eax,0x2
0019FBA8: vxorps    xmm1,xmm1,xmm1
0019FBAC: vcvtsi2ss xmm1,xmm1,edi
0019FBB0: vmulss    xmm0,xmm1,DWORD PTR [rip+0x26c140]        # 0x18040bcf8
0019FBB8: vcvttss2si r8d,xmm0
0019FBBC: mov       DWORD PTR [rsp+0x50],r8d
0019FBC1: xor       r8d,r8d
0019FBC4: mov       DWORD PTR [rsp+0x48],ecx
0019FBC8: mov       rcx,QWORD PTR [rip+0xce1211]        # 0x180e80de0
0019FBCF: mov       DWORD PTR [rsp+0x40],eax
0019FBD3: lea       rax,[rbp+0x77]
0019FBD7: mov       QWORD PTR [rsp+0x38],r10
0019FBDC: mov       QWORD PTR [rsp+0x30],rdx
0019FBE1: mov       edx,0x4
0019FBE6: mov       QWORD PTR [rsp+0x28],rax
0019FBEB: mov       DWORD PTR [rsp+0x20],0x1
0019FBF3: call      0x1802a1240
0019FBF8: mov       rcx,QWORD PTR [rip+0xce11e1]        # 0x180e80de0
0019FBFF: add       rcx,0x880
0019FC06: call      0x18016c140
0019FC0B: mov       rcx,QWORD PTR [rip+0xce130e]        # 0x180e80f20
0019FC12: lea       r8,[rbp+0x17]
0019FC16: vxorps    xmm0,xmm0,xmm0
0019FC1A: vmovups   XMMWORD PTR [rbp+0x17],xmm0
0019FC1F: mov       rcx,QWORD PTR [rcx+0x250]
0019FC26: mov       rdx,QWORD PTR [rcx]
0019FC29: mov       r9,QWORD PTR [rdx+0x190]
0019FC30: mov       rdx,rax
0019FC33: call      r9
0019FC36: call      0x180152f00
0019FC3B: mov       rcx,rax
0019FC3E: call      0x18025edd0
0019FC43: mov       r15,QWORD PTR [rsp+0xa0]
0019FC4B: test      al,al
0019FC4D: jne       0x18019fd76
0019FC53: call      0x180297a40
0019FC58: mov       rcx,QWORD PTR [rip+0xce1181]        # 0x180e80de0
0019FC5F: add       rcx,0x8d8
0019FC66: call      0x18016c450
0019FC6B: mov       rcx,QWORD PTR [rip+0xce116e]        # 0x180e80de0
0019FC72: mov       rbx,rax
0019FC75: call      0x180293f20
0019FC7A: test      al,al
0019FC7C: je        0x18019fc8e
0019FC7E: mov       rcx,QWORD PTR [rip+0xce115b]        # 0x180e80de0
0019FC85: add       rcx,0x9e0
0019FC8C: jmp       0x18019fca0
0019FC8E: mov       rax,QWORD PTR [rip+0xce128b]        # 0x180e80f20
0019FC95: imul      rcx,r13,0x58
0019FC99: add       rax,0x40
0019FC9D: add       rcx,rax
0019FCA0: call      0x18016c140
0019FCA5: vmovss    xmm3,DWORD PTR [rip+0x26c087]        # 0x18040bd34
0019FCAD: mov       QWORD PTR [rbp+0x77],rax
0019FCB1: mov       r8d,0x3
0019FCB7: mov       rax,QWORD PTR [rip+0xce1262]        # 0x180e80f20
0019FCBE: mov       rdx,rbx
0019FCC1: mov       BYTE PTR [rsp+0x20],0x0
0019FCC6: mov       rcx,QWORD PTR [rax+0x250]
0019FCCD: mov       rax,QWORD PTR [rcx]
0019FCD0: call      QWORD PTR [rax+0x1a8]
0019FCD6: mov       rax,QWORD PTR [rip+0xce1243]        # 0x180e80f20
0019FCDD: lea       r8,[rbp+0x77]
0019FCE1: mov       r9,rbx
0019FCE4: mov       edx,0x1
0019FCE9: mov       rcx,QWORD PTR [rax+0x250]
0019FCF0: mov       rax,QWORD PTR [rcx]
0019FCF3: call      QWORD PTR [rax+0x108]
0019FCF9: mov       rcx,QWORD PTR [rip+0xce10e0]        # 0x180e80de0
0019FD00: mov       BYTE PTR [rip+0xce138b],0x1        # 0x180e81092
0019FD07: call      0x180293f20
0019FD0C: mov       rcx,QWORD PTR [rip+0xce10cd]        # 0x180e80de0
0019FD13: test      al,al
0019FD15: jne       0x18019fd29
0019FD17: cmp       BYTE PTR [rcx+0x3d0],al
0019FD1D: je        0x18019fd76
0019FD1F: cmp       QWORD PTR [rcx+0x1570],0x0
0019FD27: je        0x18019fd76
0019FD29: add       rcx,0x9e0
0019FD30: cmp       QWORD PTR [rcx],0x0
0019FD34: je        0x18019fd76
0019FD36: mov       rax,QWORD PTR [rip+0xce11e3]        # 0x180e80f20
0019FD3D: vxorps    xmm0,xmm0,xmm0
0019FD41: vmovups   XMMWORD PTR [rbp+0x7],xmm0
0019FD46: mov       rdi,QWORD PTR [rax+0x250]
0019FD4D: mov       rax,QWORD PTR [rdi]
0019FD50: mov       rbx,QWORD PTR [rax+0x190]
0019FD57: call      0x18016c140
0019FD5C: lea       r8,[rbp+0x7]
0019FD60: mov       rdx,rax
0019FD63: mov       rcx,rdi
0019FD66: call      rbx
0019FD68: mov       rax,QWORD PTR [rip+0xce1071]        # 0x180e80de0
0019FD6F: mov       BYTE PTR [rax+0x3d3],0x1
0019FD76: mov       r13,QWORD PTR [rsp+0xb0]
0019FD7E: add       rsp,0xc0
0019FD85: pop       rdi
0019FD86: pop       rbx
0019FD87: pop       rbp
0019FD88: ret       
