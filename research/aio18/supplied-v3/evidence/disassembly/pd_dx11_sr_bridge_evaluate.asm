; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xD0CB0..0xD1AAD; unnamed
000D0CB0: mov       QWORD PTR [rsp+0x18],rbx
000D0CB5: push      rbp
000D0CB6: push      rsi
000D0CB7: push      rdi
000D0CB8: push      r12
000D0CBA: push      r13
000D0CBC: push      r14
000D0CBE: push      r15
000D0CC0: lea       rbp,[rsp-0x1b0]
000D0CC8: sub       rsp,0x2b0
000D0CCF: mov       rax,QWORD PTR [rip+0x10f9cea]        # 0x1811ca9c0
000D0CD6: xor       rax,rsp
000D0CD9: mov       QWORD PTR [rbp+0x1a0],rax
000D0CE0: mov       rbx,rdx
000D0CE3: mov       rsi,rcx
000D0CE6: movss     xmm2,DWORD PTR [rdx+0x38]
000D0CEB: movd      xmm0,DWORD PTR [rcx+0x194]
000D0CF3: cvtdq2ps  xmm0,xmm0
000D0CF6: ucomiss   xmm2,xmm0
000D0CF9: jp        0x1800d0d14
000D0CFB: jne       0x1800d0d14
000D0CFD: movd      xmm1,DWORD PTR [rcx+0x198]
000D0D05: cvtdq2ps  xmm1,xmm1
000D0D08: movss     xmm0,DWORD PTR [rdx+0x3c]
000D0D0D: ucomiss   xmm0,xmm1
000D0D10: jp        0x1800d0d14
000D0D12: je        0x1800d0d30
000D0D14: mov       BYTE PTR [rcx+0x190],0x0
000D0D1B: cvttss2si eax,xmm2
000D0D1F: mov       DWORD PTR [rcx+0x194],eax
000D0D25: cvttss2si eax,DWORD PTR [rdx+0x3c]
000D0D2A: mov       DWORD PTR [rcx+0x198],eax
000D0D30: cmp       BYTE PTR [rcx+0x190],0x0
000D0D37: jne       0x1800d0dce
000D0D3D: cmp       QWORD PTR [rdx+0x18],0x0
000D0D42: je        0x1800d0dce
000D0D48: cmp       QWORD PTR [rdx+0x10],0x0
000D0D4D: je        0x1800d0dce
000D0D4F: lea       rcx,[rbp+0x60]
000D0D53: movups    xmm0,XMMWORD PTR [rdx]
000D0D56: movups    XMMWORD PTR [rcx],xmm0
000D0D59: movups    xmm1,XMMWORD PTR [rdx+0x10]
000D0D5D: movups    XMMWORD PTR [rcx+0x10],xmm1
000D0D61: movups    xmm0,XMMWORD PTR [rdx+0x20]
000D0D65: movups    XMMWORD PTR [rcx+0x20],xmm0
000D0D69: movups    xmm1,XMMWORD PTR [rdx+0x30]
000D0D6D: movups    XMMWORD PTR [rcx+0x30],xmm1
000D0D71: movups    xmm0,XMMWORD PTR [rdx+0x40]
000D0D75: movups    XMMWORD PTR [rcx+0x40],xmm0
000D0D79: movups    xmm1,XMMWORD PTR [rdx+0x50]
000D0D7D: movups    XMMWORD PTR [rcx+0x50],xmm1
000D0D81: movups    xmm0,XMMWORD PTR [rdx+0x60]
000D0D85: movups    XMMWORD PTR [rcx+0x60],xmm0
000D0D89: movups    xmm1,XMMWORD PTR [rdx+0x70]
000D0D8D: movups    XMMWORD PTR [rcx+0x70],xmm1
000D0D91: movups    xmm0,XMMWORD PTR [rdx+0x80]
000D0D98: movups    XMMWORD PTR [rcx+0x80],xmm0
000D0D9F: movups    xmm1,XMMWORD PTR [rdx+0x90]
000D0DA6: movups    XMMWORD PTR [rcx+0x90],xmm1
000D0DAD: movups    xmm0,XMMWORD PTR [rdx+0xa0]
000D0DB4: movups    XMMWORD PTR [rcx+0xa0],xmm0
000D0DBB: lea       rdx,[rbp+0x60]
000D0DBF: mov       rcx,rsi
000D0DC2: call      0x1800cfbd0
000D0DC7: mov       BYTE PTR [rsi+0x190],0x1
000D0DCE: mov       rcx,QWORD PTR [rbx+0x8]
000D0DD2: mov       rax,QWORD PTR [rcx]
000D0DD5: lea       rdx,[rbp+0x140]
000D0DDC: call      QWORD PTR [rax+0x50]
000D0DDF: mov       rcx,QWORD PTR [rsi+0x270]
000D0DE6: test      rcx,rcx
000D0DE9: je        0x1800d0df8
000D0DEB: mov       rax,QWORD PTR [rcx]
000D0DEE: lea       rdx,[rbp+0x170]
000D0DF5: call      QWORD PTR [rax+0x50]
000D0DF8: movups    xmm0,XMMWORD PTR [rbp+0x170]
000D0DFF: movups    XMMWORD PTR [rsi+0x290],xmm0
000D0E06: movups    xmm1,XMMWORD PTR [rbp+0x180]
000D0E0D: movups    XMMWORD PTR [rsi+0x2a0],xmm1
000D0E14: movsd     xmm0,QWORD PTR [rbp+0x190]
000D0E1C: movsd     QWORD PTR [rsi+0x2b0],xmm0
000D0E24: mov       eax,DWORD PTR [rbp+0x198]
000D0E2A: mov       DWORD PTR [rsi+0x2b8],eax
000D0E30: movd      eax,xmm1
000D0E34: cmp       DWORD PTR [rbp+0x150],eax
000D0E3A: je        0x1800d0e83
000D0E3C: movups    xmm0,XMMWORD PTR [rbp+0x140]
000D0E43: movaps    XMMWORD PTR [rbp+0x110],xmm0
000D0E4A: movups    xmm1,XMMWORD PTR [rbp+0x150]
000D0E51: movaps    XMMWORD PTR [rbp+0x120],xmm1
000D0E58: movsd     xmm0,QWORD PTR [rbp+0x160]
000D0E60: movsd     QWORD PTR [rbp+0x130],xmm0
000D0E68: mov       eax,DWORD PTR [rbp+0x168]
000D0E6E: mov       DWORD PTR [rbp+0x138],eax
000D0E74: lea       rdx,[rbp+0x110]
000D0E7B: mov       rcx,rsi
000D0E7E: call      0x1800d0290
000D0E83: mov       rcx,QWORD PTR [rip+0x1145526]        # 0x1812163b0
000D0E8A: mov       r14,QWORD PTR [rcx+0x10]
000D0E8E: cmp       BYTE PTR [rbx+0x64],0x0
000D0E92: jne       0x1800d0f08
000D0E94: lea       rax,[rsi+0x1a0]
000D0E9B: movups    xmm0,XMMWORD PTR [rbx]
000D0E9E: movups    XMMWORD PTR [rax],xmm0
000D0EA1: movups    xmm1,XMMWORD PTR [rbx+0x10]
000D0EA5: movups    XMMWORD PTR [rax+0x10],xmm1
000D0EA9: movups    xmm0,XMMWORD PTR [rbx+0x20]
000D0EAD: movups    XMMWORD PTR [rax+0x20],xmm0
000D0EB1: movups    xmm1,XMMWORD PTR [rbx+0x30]
000D0EB5: movups    XMMWORD PTR [rax+0x30],xmm1
000D0EB9: movups    xmm0,XMMWORD PTR [rbx+0x40]
000D0EBD: movups    XMMWORD PTR [rax+0x40],xmm0
000D0EC1: movups    xmm1,XMMWORD PTR [rbx+0x50]
000D0EC5: movups    XMMWORD PTR [rax+0x50],xmm1
000D0EC9: movups    xmm0,XMMWORD PTR [rbx+0x60]
000D0ECD: movups    XMMWORD PTR [rax+0x60],xmm0
000D0ED1: movups    xmm1,XMMWORD PTR [rbx+0x70]
000D0ED5: movups    XMMWORD PTR [rax+0x70],xmm1
000D0ED9: movups    xmm0,XMMWORD PTR [rbx+0x80]
000D0EE0: movups    XMMWORD PTR [rax+0x80],xmm0
000D0EE7: movups    xmm1,XMMWORD PTR [rbx+0x90]
000D0EEE: movups    XMMWORD PTR [rax+0x90],xmm1
000D0EF5: movups    xmm0,XMMWORD PTR [rbx+0xa0]
000D0EFC: movups    XMMWORD PTR [rax+0xa0],xmm0
000D0F03: jmp       0x1800d1a83
000D0F08: mov       rcx,QWORD PTR [rcx+0x20]
000D0F0C: mov       rax,QWORD PTR [rcx]
000D0F0F: call      QWORD PTR [rax+0x40]
000D0F12: mov       r8,QWORD PTR [rip+0x1145497]        # 0x1812163b0
000D0F19: mov       rdx,QWORD PTR [r8+0x208]
000D0F20: cmp       rax,rdx
000D0F23: jae       0x1800d0f49
000D0F25: mov       rcx,QWORD PTR [r8+0x20]
000D0F29: mov       rax,QWORD PTR [rcx]
000D0F2C: mov       r8,QWORD PTR [r8+0x40]
000D0F30: call      QWORD PTR [rax+0x48]
000D0F33: mov       edx,0xffffffff
000D0F38: mov       rcx,QWORD PTR [rip+0x1145471]        # 0x1812163b0
000D0F3F: mov       rcx,QWORD PTR [rcx+0x40]
000D0F43: call      QWORD PTR [rip+0x42267]        # 0x1801131b0 ; KERNEL32.dll!WaitForSingleObject
000D0F49: cmp       QWORD PTR [rsi+0x1a8],0x0
000D0F51: je        0x1800d10db
000D0F57: lea       rax,[rbp-0x80]
000D0F5B: mov       QWORD PTR [rsp+0x68],rax
000D0F60: mov       rax,QWORD PTR [rsi+0x4a0]
000D0F67: mov       QWORD PTR [rbp-0x80],rax
000D0F6B: mov       rax,QWORD PTR [rsi+0x4a8]
000D0F72: mov       QWORD PTR [rbp-0x78],rax
000D0F76: mov       rax,QWORD PTR [rsi+0x4b0]
000D0F7D: mov       QWORD PTR [rbp-0x70],rax
000D0F81: mov       rax,QWORD PTR [rsi+0x4b8]
000D0F88: mov       QWORD PTR [rbp-0x68],rax
000D0F8C: movups    xmm0,XMMWORD PTR [rsi+0x4c0]
000D0F93: movups    XMMWORD PTR [rbp-0x60],xmm0
000D0F97: movups    xmm1,XMMWORD PTR [rsi+0x4d0]
000D0F9E: movups    XMMWORD PTR [rbp-0x50],xmm1
000D0FA2: movsd     xmm0,QWORD PTR [rsi+0x4e0]
000D0FAA: movsd     QWORD PTR [rbp-0x40],xmm0
000D0FAF: mov       eax,DWORD PTR [rsi+0x4e8]
000D0FB5: mov       DWORD PTR [rbp-0x38],eax
000D0FB8: lea       rdx,[rsi+0x4f0]
000D0FBF: lea       rcx,[rbp-0x30]
000D0FC3: call      0x180080190
000D0FC8: nop       
000D0FC9: lea       rax,[rbp-0x10]
000D0FCD: mov       QWORD PTR [rsp+0x60],rax
000D0FD2: mov       rax,QWORD PTR [rsi+0x3c0]
000D0FD9: mov       QWORD PTR [rbp-0x10],rax
000D0FDD: mov       rax,QWORD PTR [rsi+0x3c8]
000D0FE4: mov       QWORD PTR [rbp-0x8],rax
000D0FE8: mov       rax,QWORD PTR [rsi+0x3d0]
000D0FEF: mov       QWORD PTR [rbp+0x0],rax
000D0FF3: mov       rax,QWORD PTR [rsi+0x3d8]
000D0FFA: mov       QWORD PTR [rbp+0x8],rax
000D0FFE: movups    xmm0,XMMWORD PTR [rsi+0x3e0]
000D1005: movups    XMMWORD PTR [rbp+0x10],xmm0
000D1009: movups    xmm1,XMMWORD PTR [rsi+0x3f0]
000D1010: movups    XMMWORD PTR [rbp+0x20],xmm1
000D1014: movsd     xmm0,QWORD PTR [rsi+0x400]
000D101C: movsd     QWORD PTR [rbp+0x30],xmm0
000D1021: mov       eax,DWORD PTR [rsi+0x408]
000D1027: mov       DWORD PTR [rbp+0x38],eax
000D102A: lea       rdx,[rsi+0x410]
000D1031: lea       rcx,[rbp+0x40]
000D1035: call      0x180080190
000D103A: nop       
000D103B: mov       rax,QWORD PTR [rsi+0x2e0]
000D1042: mov       QWORD PTR [rbp+0x60],rax
000D1046: mov       rax,QWORD PTR [rsi+0x2e8]
000D104D: mov       QWORD PTR [rbp+0x68],rax
000D1051: mov       rax,QWORD PTR [rsi+0x2f0]
000D1058: mov       QWORD PTR [rbp+0x70],rax
000D105C: mov       rax,QWORD PTR [rsi+0x2f8]
000D1063: mov       QWORD PTR [rbp+0x78],rax
000D1067: movups    xmm0,XMMWORD PTR [rsi+0x300]
000D106E: movups    XMMWORD PTR [rbp+0x80],xmm0
000D1075: movups    xmm1,XMMWORD PTR [rsi+0x310]
000D107C: movups    XMMWORD PTR [rbp+0x90],xmm1
000D1083: movsd     xmm0,QWORD PTR [rsi+0x320]
000D108B: movsd     QWORD PTR [rbp+0xa0],xmm0
000D1093: mov       eax,DWORD PTR [rsi+0x328]
000D1099: mov       DWORD PTR [rbp+0xa8],eax
000D109F: lea       rdx,[rsi+0x330]
000D10A6: lea       rcx,[rbp+0xb0]
000D10AD: call      0x180080190
000D10B2: nop       
000D10B3: lea       rax,[rbp-0x80]
000D10B7: mov       QWORD PTR [rsp+0x28],rax
000D10BC: lea       rax,[rbp-0x10]
000D10C0: mov       QWORD PTR [rsp+0x20],rax
000D10C5: lea       r9,[rbp+0x60]
000D10C9: lea       r8,[rsi+0x1a0]
000D10D0: mov       rdx,r14
000D10D3: mov       rcx,rsi
000D10D6: call      0x1800d09a0 ; '@USVWATAUAVAWH'
000D10DB: lea       rax,[rbp+0x60]
000D10DF: mov       QWORD PTR [rsp+0x68],rax
000D10E4: mov       rax,QWORD PTR [rsi+0x430]
000D10EB: mov       QWORD PTR [rbp+0x60],rax
000D10EF: mov       rax,QWORD PTR [rsi+0x438]
000D10F6: mov       QWORD PTR [rbp+0x68],rax
000D10FA: mov       rax,QWORD PTR [rsi+0x440]
000D1101: mov       QWORD PTR [rbp+0x70],rax
000D1105: mov       rax,QWORD PTR [rsi+0x448]
000D110C: mov       QWORD PTR [rbp+0x78],rax
000D1110: movups    xmm0,XMMWORD PTR [rsi+0x450]
000D1117: movups    XMMWORD PTR [rbp+0x80],xmm0
000D111E: movups    xmm1,XMMWORD PTR [rsi+0x460]
000D1125: movups    XMMWORD PTR [rbp+0x90],xmm1
000D112C: movsd     xmm0,QWORD PTR [rsi+0x470]
000D1134: movsd     QWORD PTR [rbp+0xa0],xmm0
000D113C: mov       eax,DWORD PTR [rsi+0x478]
000D1142: mov       DWORD PTR [rbp+0xa8],eax
000D1148: lea       rdx,[rsi+0x480]
000D114F: lea       rcx,[rbp+0xb0]
000D1156: call      0x180080190
000D115B: nop       
000D115C: lea       rax,[rbp-0x10]
000D1160: mov       QWORD PTR [rsp+0x60],rax
000D1165: mov       rax,QWORD PTR [rsi+0x350]
000D116C: mov       QWORD PTR [rbp-0x10],rax
000D1170: mov       rax,QWORD PTR [rsi+0x358]
000D1177: mov       QWORD PTR [rbp-0x8],rax
000D117B: mov       rax,QWORD PTR [rsi+0x360]
000D1182: mov       QWORD PTR [rbp+0x0],rax
000D1186: mov       rax,QWORD PTR [rsi+0x368]
000D118D: mov       QWORD PTR [rbp+0x8],rax
000D1191: movups    xmm0,XMMWORD PTR [rsi+0x370]
000D1198: movups    XMMWORD PTR [rbp+0x10],xmm0
000D119C: movups    xmm1,XMMWORD PTR [rsi+0x380]
000D11A3: movups    XMMWORD PTR [rbp+0x20],xmm1
000D11A7: movsd     xmm0,QWORD PTR [rsi+0x390]
000D11AF: movsd     QWORD PTR [rbp+0x30],xmm0
000D11B4: mov       eax,DWORD PTR [rsi+0x398]
000D11BA: mov       DWORD PTR [rbp+0x38],eax
000D11BD: lea       rdx,[rsi+0x3a0]
000D11C4: lea       rcx,[rbp+0x40]
000D11C8: call      0x180080190
000D11CD: nop       
000D11CE: mov       rax,QWORD PTR [rsi+0x270]
000D11D5: mov       QWORD PTR [rbp-0x80],rax
000D11D9: mov       rax,QWORD PTR [rsi+0x278]
000D11E0: mov       QWORD PTR [rbp-0x78],rax
000D11E4: mov       rax,QWORD PTR [rsi+0x280]
000D11EB: mov       QWORD PTR [rbp-0x70],rax
000D11EF: mov       rax,QWORD PTR [rsi+0x288]
000D11F6: mov       QWORD PTR [rbp-0x68],rax
000D11FA: movups    xmm0,XMMWORD PTR [rsi+0x290]
000D1201: movups    XMMWORD PTR [rbp-0x60],xmm0
000D1205: movups    xmm1,XMMWORD PTR [rsi+0x2a0]
000D120C: movups    XMMWORD PTR [rbp-0x50],xmm1
000D1210: movsd     xmm0,QWORD PTR [rsi+0x2b0]
000D1218: movsd     QWORD PTR [rbp-0x40],xmm0
000D121D: mov       eax,DWORD PTR [rsi+0x2b8]
000D1223: mov       DWORD PTR [rbp-0x38],eax
000D1226: lea       rdx,[rsi+0x2c0]
000D122D: lea       rcx,[rbp-0x30]
000D1231: call      0x180080190
000D1236: nop       
000D1237: lea       rax,[rbp+0x60]
000D123B: mov       QWORD PTR [rsp+0x28],rax
000D1240: lea       rax,[rbp-0x10]
000D1244: mov       QWORD PTR [rsp+0x20],rax
000D1249: lea       r9,[rbp-0x80]
000D124D: mov       r8,rbx
000D1250: mov       rdx,r14
000D1253: mov       rcx,rsi
000D1256: call      0x1800d09a0 ; '@USVWATAUAVAWH'
000D125B: xor       r13d,r13d
000D125E: mov       QWORD PTR [rsp+0x70],r13
000D1263: mov       QWORD PTR [rsp+0x50],r13
000D1268: cmp       QWORD PTR [rsi+0x1c0],r13
000D126F: je        0x1800d1299
000D1271: mov       rdx,QWORD PTR [rsi+0x580]
000D1278: test      rdx,rdx
000D127B: je        0x1800d1299
000D127D: mov       rax,QWORD PTR [r14]
000D1280: mov       r8,QWORD PTR [rbx+0x20]
000D1284: mov       rcx,r14
000D1287: call      QWORD PTR [rax+0x178]
000D128D: mov       rax,QWORD PTR [rsi+0x178]
000D1294: mov       QWORD PTR [rsp+0x50],rax
000D1299: mov       r8,QWORD PTR [rbx+0x20]
000D129D: test      r8,r8
000D12A0: je        0x1800d12c6
000D12A2: mov       rdx,QWORD PTR [rsi+0x510]
000D12A9: test      rdx,rdx
000D12AC: je        0x1800d12c6
000D12AE: mov       rax,QWORD PTR [r14]
000D12B1: mov       rcx,r14
000D12B4: call      QWORD PTR [rax+0x178]
000D12BA: mov       rax,QWORD PTR [rsi+0x170]
000D12C1: mov       QWORD PTR [rsp+0x70],rax
000D12C6: mov       rax,QWORD PTR [r14]
000D12C9: mov       rdx,QWORD PTR [rip+0x11450e0]        # 0x1812163b0
000D12D0: inc       QWORD PTR [rdx+0x208]
000D12D7: mov       r8,QWORD PTR [rdx+0x208]
000D12DE: mov       rdx,QWORD PTR [rdx+0x20]
000D12E2: mov       rcx,r14
000D12E5: call      QWORD PTR [rax+0x498]
000D12EB: mov       rax,QWORD PTR [rsi+0x1d0]
000D12F2: mov       QWORD PTR [rsp+0x68],rax
000D12F7: mov       rdi,QWORD PTR [rbx+0x30]
000D12FB: mov       QWORD PTR [rsp+0x60],rdi
000D1300: mov       rax,QWORD PTR [rsi+0x1c8]
000D1307: mov       QWORD PTR [rsp+0x78],rax
000D130C: mov       r15,QWORD PTR [rbx+0x28]
000D1310: mov       rax,QWORD PTR [rbx+0x8]
000D1314: mov       QWORD PTR [rsp+0x58],rax
000D1319: test      rdi,rdi
000D131C: je        0x1800d1337
000D131E: mov       rdx,rdi
000D1321: mov       rcx,QWORD PTR [rip+0x1145088]        # 0x1812163b0
000D1328: call      0x180073f30
000D132D: test      rax,rax
000D1330: je        0x1800d1337
000D1332: mov       r12b,0x1
000D1335: jmp       0x1800d133a
000D1337: xor       r12b,r12b
000D133A: test      r12b,r12b
000D133D: cmovne    r15,rdi
000D1341: cmp       QWORD PTR [rsi+0x1a8],r13
000D1348: je        0x1800d146c
000D134E: mov       rcx,QWORD PTR [rsi+0x118]
000D1355: test      rcx,rcx
000D1358: je        0x1800d146c
000D135E: mov       rax,QWORD PTR [rcx]
000D1361: mov       edx,DWORD PTR [rsi+0x1a0]
000D1367: call      QWORD PTR [rax+0x28]
000D136A: test      al,al
000D136C: je        0x1800d146c
000D1372: xor       edx,edx
000D1374: mov       rcx,QWORD PTR [rip+0x1145035]        # 0x1812163b0
000D137B: call      0x180073410
000D1380: mov       r13,rax
000D1383: mov       QWORD PTR [rsi+0x208],rax
000D138A: mov       rcx,QWORD PTR [rsi+0x148]
000D1391: mov       QWORD PTR [rsi+0x1a8],rcx
000D1398: mov       rcx,QWORD PTR [rsi+0x168]
000D139F: mov       QWORD PTR [rsi+0x1b0],rcx
000D13A6: mov       rcx,QWORD PTR [rsi+0x158]
000D13AD: mov       QWORD PTR [rsi+0x1b8],rcx
000D13B4: mov       rax,QWORD PTR [rsp+0x50]
000D13B9: mov       QWORD PTR [rsi+0x1c0],rax
000D13C0: lea       rdx,[rsi+0x1a0]
000D13C7: lea       rcx,[rsi+0x180]
000D13CE: call      0x180091e70
000D13D3: mov       rcx,QWORD PTR [rax]
000D13D6: mov       QWORD PTR [rsi+0x1c8],rcx
000D13DD: mov       QWORD PTR [rsi+0x1d0],0x0
000D13E8: mov       rcx,QWORD PTR [rsi+0x118]
000D13EF: lea       rax,[rsi+0x1a0]
000D13F6: lea       rdx,[rbp+0x60]
000D13FA: movups    xmm0,XMMWORD PTR [rax]
000D13FD: movups    XMMWORD PTR [rdx],xmm0
000D1400: movups    xmm1,XMMWORD PTR [rax+0x10]
000D1404: movups    XMMWORD PTR [rdx+0x10],xmm1
000D1408: movups    xmm0,XMMWORD PTR [rax+0x20]
000D140C: movups    XMMWORD PTR [rdx+0x20],xmm0
000D1410: movups    xmm1,XMMWORD PTR [rax+0x30]
000D1414: movups    XMMWORD PTR [rdx+0x30],xmm1
000D1418: movups    xmm0,XMMWORD PTR [rax+0x40]
000D141C: movups    XMMWORD PTR [rdx+0x40],xmm0
000D1420: movups    xmm1,XMMWORD PTR [rax+0x50]
000D1424: movups    XMMWORD PTR [rdx+0x50],xmm1
000D1428: movups    xmm0,XMMWORD PTR [rax+0x60]
000D142C: movups    XMMWORD PTR [rdx+0x60],xmm0
000D1430: movups    xmm1,XMMWORD PTR [rax+0x70]
000D1434: movups    XMMWORD PTR [rdx+0x70],xmm1
000D1438: movups    xmm0,XMMWORD PTR [rax+0x80]
000D143F: movups    XMMWORD PTR [rdx+0x80],xmm0
000D1446: movups    xmm1,XMMWORD PTR [rax+0x90]
000D144D: movups    XMMWORD PTR [rdx+0x90],xmm1
000D1454: movups    xmm0,XMMWORD PTR [rax+0xa0]
000D145B: movups    XMMWORD PTR [rdx+0xa0],xmm0
000D1462: mov       rax,QWORD PTR [rcx]
000D1465: lea       rdx,[rbp+0x60]
000D1469: call      QWORD PTR [rax+0x8]
000D146C: cmp       QWORD PTR [rsi+0x1a8],0x0
000D1474: jne       0x1800d1487
000D1476: xor       edx,edx
000D1478: mov       rcx,QWORD PTR [rip+0x1144f31]        # 0x1812163b0
000D147F: call      0x180073410
000D1484: mov       r13,rax
000D1487: mov       QWORD PTR [rbx+0x68],r13
000D148B: mov       rax,QWORD PTR [rsi+0x140]
000D1492: mov       QWORD PTR [rbx+0x8],rax
000D1496: mov       rax,QWORD PTR [rsi+0x160]
000D149D: mov       QWORD PTR [rbx+0x10],rax
000D14A1: mov       rax,QWORD PTR [rsi+0x150]
000D14A8: mov       QWORD PTR [rbx+0x18],rax
000D14AC: mov       rax,QWORD PTR [rsp+0x70]
000D14B1: mov       QWORD PTR [rbx+0x20],rax
000D14B5: test      r12b,r12b
000D14B8: je        0x1800d14cb
000D14BA: mov       rdx,rdi
000D14BD: mov       rcx,QWORD PTR [rip+0x1144eec]        # 0x1812163b0
000D14C4: call      0x180073f30
000D14C9: jmp       0x1800d14dd
000D14CB: lea       rcx,[rsi+0x180]
000D14D2: mov       rdx,rbx
000D14D5: call      0x180091e70
000D14DA: mov       rax,QWORD PTR [rax]
000D14DD: mov       QWORD PTR [rbx+0x28],rax
000D14E1: xor       r12d,r12d
000D14E4: mov       QWORD PTR [rbx+0x30],r12
000D14E8: mov       QWORD PTR [rbx+0x88],r12
000D14EF: mov       QWORD PTR [rbx+0x90],r12
000D14F6: mov       rcx,QWORD PTR [rsi+0x118]
000D14FD: test      rcx,rcx
000D1500: je        0x1800d158f
000D1506: mov       rax,QWORD PTR [rcx]
000D1509: mov       edx,DWORD PTR [rbx]
000D150B: call      QWORD PTR [rax+0x28]
000D150E: test      al,al
000D1510: je        0x1800d158f
000D1512: mov       rcx,QWORD PTR [rsi+0x118]
000D1519: lea       rdx,[rbp+0x60]
000D151D: movups    xmm0,XMMWORD PTR [rbx]
000D1520: movups    XMMWORD PTR [rdx],xmm0
000D1523: movups    xmm1,XMMWORD PTR [rbx+0x10]
000D1527: movups    XMMWORD PTR [rdx+0x10],xmm1
000D152B: movups    xmm0,XMMWORD PTR [rbx+0x20]
000D152F: movups    XMMWORD PTR [rdx+0x20],xmm0
000D1533: movups    xmm1,XMMWORD PTR [rbx+0x30]
000D1537: movups    XMMWORD PTR [rdx+0x30],xmm1
000D153B: movups    xmm0,XMMWORD PTR [rbx+0x40]
000D153F: movups    XMMWORD PTR [rdx+0x40],xmm0
000D1543: movups    xmm1,XMMWORD PTR [rbx+0x50]
000D1547: movups    XMMWORD PTR [rdx+0x50],xmm1
000D154B: movups    xmm0,XMMWORD PTR [rbx+0x60]
000D154F: movups    XMMWORD PTR [rdx+0x60],xmm0
000D1553: movups    xmm1,XMMWORD PTR [rbx+0x70]
000D1557: movups    XMMWORD PTR [rdx+0x70],xmm1
000D155B: movups    xmm0,XMMWORD PTR [rbx+0x80]
000D1562: movups    XMMWORD PTR [rdx+0x80],xmm0
000D1569: movups    xmm1,XMMWORD PTR [rbx+0x90]
000D1570: movups    XMMWORD PTR [rdx+0x90],xmm1
000D1577: movups    xmm0,XMMWORD PTR [rbx+0xa0]
000D157E: movups    XMMWORD PTR [rdx+0xa0],xmm0
000D1585: mov       rax,QWORD PTR [rcx]
000D1588: lea       rdx,[rbp+0x60]
000D158C: call      QWORD PTR [rax+0x8]
000D158F: xor       edx,edx
000D1591: mov       rcx,QWORD PTR [rip+0x1144e18]        # 0x1812163b0
000D1598: call      0x180073660
000D159D: mov       rax,QWORD PTR [r14]
000D15A0: mov       rdx,QWORD PTR [rip+0x1144e09]        # 0x1812163b0
000D15A7: mov       r8,QWORD PTR [rdx+0x208]
000D15AE: mov       rdx,QWORD PTR [rdx+0x20]
000D15B2: mov       rcx,r14
000D15B5: call      QWORD PTR [rax+0x4a0]
000D15BB: cmp       BYTE PTR [rsi+0x2c],r12b
000D15BF: je        0x1800d1666
000D15C5: mov       eax,DWORD PTR [rsi+0x110]
000D15CB: cmp       eax,0x2
000D15CE: je        0x1800d15d8
000D15D0: test      eax,eax
000D15D2: jne       0x1800d1666
000D15D8: cmp       QWORD PTR [rsi+0xb0],r12
000D15DF: jne       0x1800d160e
000D15E1: mov       rcx,QWORD PTR [rsp+0x58]
000D15E6: mov       rax,QWORD PTR [rcx]
000D15E9: lea       rdx,[rbp+0x110]
000D15F0: call      QWORD PTR [rax+0x50]
000D15F3: lea       rcx,[rsi+0x8]
000D15F7: mov       r8d,DWORD PTR [rbp+0x120]
000D15FE: mov       rdx,QWORD PTR [rip+0x1144dab]        # 0x1812163b0
000D1605: mov       rdx,QWORD PTR [rdx+0x8]
000D1609: call      0x1800ce5f0
000D160E: cmp       QWORD PTR [rsi+0x1a8],r12
000D1615: je        0x1800d1641
000D1617: lea       rcx,[rsi+0x8]
000D161B: movss     xmm0,DWORD PTR [rsi+0x1e0]
000D1623: movss     DWORD PTR [rsp+0x20],xmm0
000D1629: mov       r9,QWORD PTR [rsp+0x78]
000D162E: mov       r8,r14
000D1631: mov       rdx,QWORD PTR [rip+0x1144d78]        # 0x1812163b0
000D1638: mov       rdx,QWORD PTR [rdx+0x8]
000D163C: call      0x1800ce740
000D1641: lea       rcx,[rsi+0x8]
000D1645: movss     xmm0,DWORD PTR [rbx+0x40]
000D164A: movss     DWORD PTR [rsp+0x20],xmm0
000D1650: mov       r9,r15
000D1653: mov       r8,r14
000D1656: mov       rdx,QWORD PTR [rip+0x1144d53]        # 0x1812163b0
000D165D: mov       rdx,QWORD PTR [rdx+0x8]
000D1661: call      0x1800ce740
000D1666: cmp       BYTE PTR [rsi+0x48],r12b
000D166A: je        0x1800d1a17
000D1670: mov       ecx,0x4
000D1675: call      QWORD PTR [rip+0x41f55]        # 0x1801135d0 ; USER32.dll!GetAsyncKeyState
000D167B: test      ax,ax
000D167E: jns       0x1800d1690
000D1680: cmp       BYTE PTR [rip+0x114ea94],r12b        # 0x18122011b
000D1687: jne       0x1800d1690
000D1689: mov       BYTE PTR [rip+0x114ea8b],0x1        # 0x18122011b
000D1690: mov       ecx,0x4
000D1695: call      QWORD PTR [rip+0x41f35]        # 0x1801135d0 ; USER32.dll!GetAsyncKeyState
000D169B: test      ax,ax
000D169E: jne       0x1800d16c0
000D16A0: cmp       BYTE PTR [rip+0x114ea74],0x1        # 0x18122011b
000D16A7: jne       0x1800d16c0
000D16A9: mov       BYTE PTR [rip+0x114ea6b],r12b        # 0x18122011b
000D16B0: cmp       BYTE PTR [rsi+0x660],r12b
000D16B7: sete      al
000D16BA: mov       BYTE PTR [rsi+0x660],al
000D16C0: mov       ecx,0x60
000D16C5: call      QWORD PTR [rip+0x41f05]        # 0x1801135d0 ; USER32.dll!GetAsyncKeyState
000D16CB: test      ax,ax
000D16CE: jns       0x1800d16e0
000D16D0: cmp       BYTE PTR [rip+0x114ea45],r12b        # 0x18122011c
000D16D7: jne       0x1800d16e0
000D16D9: mov       BYTE PTR [rip+0x114ea3c],0x1        # 0x18122011c
000D16E0: mov       ecx,0x60
000D16E5: call      QWORD PTR [rip+0x41ee5]        # 0x1801135d0 ; USER32.dll!GetAsyncKeyState
000D16EB: test      ax,ax
000D16EE: jne       0x1800d171c
000D16F0: cmp       BYTE PTR [rip+0x114ea25],0x1        # 0x18122011c
000D16F7: jne       0x1800d171c
000D16F9: mov       BYTE PTR [rip+0x114ea1c],r12b        # 0x18122011c
000D1700: mov       eax,DWORD PTR [rsi+0x664]
000D1706: inc       eax
000D1708: and       eax,0x80000003
000D170D: jge       0x1800d1716
000D170F: dec       eax
000D1711: or        eax,0xfffffffc
000D1714: inc       eax
000D1716: mov       DWORD PTR [rsi+0x664],eax
000D171C: mov       ecx,0x11
000D1721: call      QWORD PTR [rip+0x41ea9]        # 0x1801135d0 ; USER32.dll!GetAsyncKeyState
000D1727: test      ax,ax
000D172A: jns       0x1800d173c
000D172C: cmp       BYTE PTR [rip+0x114e9e7],r12b        # 0x18122011a
000D1733: jne       0x1800d173c
000D1735: mov       BYTE PTR [rip+0x114e9de],0x1        # 0x18122011a
000D173C: mov       ecx,0x11
000D1741: call      QWORD PTR [rip+0x41e89]        # 0x1801135d0 ; USER32.dll!GetAsyncKeyState
000D1747: test      ax,ax
000D174A: jne       0x1800d175c
000D174C: cmp       BYTE PTR [rip+0x114e9c7],0x1        # 0x18122011a
000D1753: jne       0x1800d175c
000D1755: mov       BYTE PTR [rip+0x114e9be],r12b        # 0x18122011a
000D175C: mov       ecx,0x61
000D1761: call      QWORD PTR [rip+0x41e69]        # 0x1801135d0 ; USER32.dll!GetAsyncKeyState
000D1767: test      ax,ax
000D176A: jns       0x1800d177c
000D176C: cmp       BYTE PTR [rip+0x114e9a5],r12b        # 0x181220118
000D1773: jne       0x1800d177c
000D1775: mov       BYTE PTR [rip+0x114e99c],0x1        # 0x181220118
000D177C: mov       ecx,0x61
000D1781: call      QWORD PTR [rip+0x41e49]        # 0x1801135d0 ; USER32.dll!GetAsyncKeyState
000D1787: test      ax,ax
000D178A: jne       0x1800d17ac
000D178C: cmp       BYTE PTR [rip+0x114e985],0x1        # 0x181220118
000D1793: jne       0x1800d17ac
000D1795: mov       BYTE PTR [rip+0x114e97c],r12b        # 0x181220118
000D179C: cmp       BYTE PTR [rip+0x114e977],r12b        # 0x18122011a
000D17A3: je        0x1800d17ac
000D17A5: mov       BYTE PTR [rsi+0x668],0x1
000D17AC: mov       ecx,0x62
000D17B1: call      QWORD PTR [rip+0x41e19]        # 0x1801135d0 ; USER32.dll!GetAsyncKeyState
000D17B7: test      ax,ax
000D17BA: jns       0x1800d17cc
000D17BC: cmp       BYTE PTR [rip+0x114e956],r12b        # 0x181220119
000D17C3: jne       0x1800d17cc
000D17C5: mov       BYTE PTR [rip+0x114e94d],0x1        # 0x181220119
000D17CC: mov       ecx,0x62
000D17D1: call      QWORD PTR [rip+0x41df9]        # 0x1801135d0 ; USER32.dll!GetAsyncKeyState
000D17D7: test      ax,ax
000D17DA: jne       0x1800d17fc
000D17DC: cmp       BYTE PTR [rip+0x114e936],0x1        # 0x181220119
000D17E3: jne       0x1800d17fc
000D17E5: mov       BYTE PTR [rip+0x114e92d],r12b        # 0x181220119
000D17EC: cmp       BYTE PTR [rip+0x114e927],r12b        # 0x18122011a
000D17F3: je        0x1800d17fc
000D17F5: mov       BYTE PTR [rsi+0x668],r12b
000D17FC: mov       QWORD PTR [rsp+0x50],r12
000D1801: mov       ecx,DWORD PTR [rsi+0x664]
000D1807: sub       ecx,0x1
000D180A: je        0x1800d187b
000D180C: sub       ecx,0x1
000D180F: je        0x1800d184b
000D1811: sub       ecx,0x1
000D1814: je        0x1800d1833
000D1816: cmp       ecx,0x1
000D1819: jne       0x1800d1896
000D181B: lea       rcx,[rsi+0x510]
000D1822: call      0x180086250
000D1827: mov       DWORD PTR [rsi+0x100],0x40600000
000D1831: jmp       0x1800d1891
000D1833: lea       rcx,[rsi+0x430]
000D183A: call      0x180086250
000D183F: mov       DWORD PTR [rsi+0x100],0x40200000
000D1849: jmp       0x1800d1891
000D184B: lea       rcx,[rsi+0x350]
000D1852: call      0x180086250
000D1857: mov       QWORD PTR [rsp+0x50],rax
000D185C: mov       DWORD PTR [rsi+0x100],0x3fc00000
000D1866: movzx     eax,BYTE PTR [rsi+0x29]
000D186A: movd      xmm0,eax
000D186E: cvtdq2ps  xmm0,xmm0
000D1871: movss     DWORD PTR [rsi+0x104],xmm0
000D1879: jmp       0x1800d1896
000D187B: lea       rcx,[rsi+0x270]
000D1882: call      0x180086250
000D1887: mov       DWORD PTR [rsi+0x100],0x3f000000
000D1891: mov       QWORD PTR [rsp+0x50],rax
000D1896: mov       ecx,DWORD PTR [rip+0x1143800]        # 0x18121509c
000D189C: mov       rax,QWORD PTR gs:0x58
000D18A5: mov       edx,0x18
000D18AA: mov       rax,QWORD PTR [rax+rcx*8]
000D18AE: mov       ecx,DWORD PTR [rdx+rax*1]
000D18B1: cmp       DWORD PTR [rip+0x1152199],ecx        # 0x181223a50
000D18B7: jle       0x1800d18e6
000D18B9: lea       rcx,[rip+0x1152190]        # 0x181223a50
000D18C0: call      0x18010c8d4
000D18C5: cmp       DWORD PTR [rip+0x1152184],0xffffffff        # 0x181223a50
000D18CC: jne       0x1800d18e6
000D18CE: lea       rcx,[rip+0x40a4b]        # 0x180112320
000D18D5: call      0x18010c814
000D18DA: lea       rcx,[rip+0x115216f]        # 0x181223a50
000D18E1: call      0x18010c868
000D18E6: mov       rax,QWORD PTR [rip+0x1128623]        # 0x1811f9f10
000D18ED: test      rax,rax
000D18F0: je        0x1800d18f7
000D18F2: cmp       rax,r15
000D18F5: je        0x1800d1962
000D18F7: mov       rcx,QWORD PTR [rip+0x112861a]        # 0x1811f9f18
000D18FE: test      rcx,rcx
000D1901: je        0x1800d1910
000D1903: mov       rax,QWORD PTR [rcx]
000D1906: call      QWORD PTR [rax+0x10]
000D1909: mov       QWORD PTR [rip+0x1128608],r12        # 0x1811f9f18
000D1910: mov       rcx,QWORD PTR [rip+0x1128609]        # 0x1811f9f20
000D1917: test      rcx,rcx
000D191A: je        0x1800d1929
000D191C: mov       rax,QWORD PTR [rcx]
000D191F: call      QWORD PTR [rax+0x10]
000D1922: mov       QWORD PTR [rip+0x11285f7],r12        # 0x1811f9f20
000D1929: mov       rcx,QWORD PTR [rip+0x11285f8]        # 0x1811f9f28
000D1930: test      rcx,rcx
000D1933: je        0x1800d1942
000D1935: mov       rax,QWORD PTR [rcx]
000D1938: call      QWORD PTR [rax+0x10]
000D193B: mov       QWORD PTR [rip+0x11285e6],r12        # 0x1811f9f28
000D1942: mov       QWORD PTR [rip+0x11285c7],r15        # 0x1811f9f10
000D1949: mov       r8d,0x9
000D194F: lea       rdx,[rip+0x10d636a]        # 0x1811a7cc0 ; 'destImage'
000D1956: lea       rcx,[rip+0x1128603]        # 0x1811f9f60
000D195D: call      0x180078710
000D1962: mov       r13d,DWORD PTR [rsi+0x1c]
000D1966: movd      xmm2,r13d
000D196B: cvtdq2ps  xmm2,xmm2
000D196E: movaps    xmm0,xmm2
000D1971: mulss     xmm0,DWORD PTR [rip+0x10db113]        # 0x1811aca8c
000D1979: cvttss2si r8d,xmm0
000D197E: mov       r12d,DWORD PTR [rsi+0x20]
000D1982: movd      xmm0,r12d
000D1987: cvtdq2ps  xmm0,xmm0
000D198A: mulss     xmm0,DWORD PTR [rip+0x10db0fa]        # 0x1811aca8c
000D1992: cvttss2si edx,xmm0
000D1996: mulss     xmm2,DWORD PTR [rip+0x10db10e]        # 0x1811acaac
000D199E: subss     xmm2,DWORD PTR [rip+0x10db1d6]        # 0x1811acb7c
000D19A6: cvttss2si ecx,xmm2
000D19AA: movzx     eax,BYTE PTR [rsi+0x660]
000D19B1: cmp       QWORD PTR [rsp+0x50],0x0
000D19B7: je        0x1800d1a14
000D19B9: xor       edi,edi
000D19BB: mov       r9d,0x64
000D19C1: test      al,al
000D19C3: cmove     edi,r9d
000D19C7: xor       ebx,ebx
000D19C9: test      al,al
000D19CB: cmove     ebx,ecx
000D19CE: cmove     r12d,edx
000D19D2: cmove     r13d,r8d
000D19D6: lea       rcx,[rip+0x1128533]        # 0x1811f9f10
000D19DD: call      0x180086130
000D19E2: lea       rcx,[rsi+0x8]
000D19E6: mov       DWORD PTR [rsp+0x48],edi
000D19EA: mov       DWORD PTR [rsp+0x40],ebx
000D19EE: mov       DWORD PTR [rsp+0x38],r12d
000D19F3: mov       DWORD PTR [rsp+0x30],r13d
000D19F8: mov       QWORD PTR [rsp+0x28],rax
000D19FD: lea       rax,[rsp+0x50]
000D1A02: mov       QWORD PTR [rsp+0x20],rax
000D1A07: mov       rdx,r14
000D1A0A: call      0x1800cea60
000D1A0F: mov       rdi,QWORD PTR [rsp+0x60]
000D1A14: xor       r12d,r12d
000D1A17: mov       rdx,QWORD PTR [rsp+0x68]
000D1A1C: test      rdx,rdx
000D1A1F: je        0x1800d1a32
000D1A21: mov       rax,QWORD PTR [r14]
000D1A24: mov       r8,QWORD PTR [rsp+0x78]
000D1A29: mov       rcx,r14
000D1A2C: call      QWORD PTR [rax+0x178]
000D1A32: test      rdi,rdi
000D1A35: je        0x1800d1a49
000D1A37: mov       rax,QWORD PTR [r14]
000D1A3A: mov       r8,r15
000D1A3D: mov       rdx,rdi
000D1A40: mov       rcx,r14
000D1A43: call      QWORD PTR [rax+0x178]
000D1A49: mov       rax,QWORD PTR [r14]
000D1A4C: mov       rdx,QWORD PTR [rip+0x114495d]        # 0x1812163b0
000D1A53: inc       QWORD PTR [rdx+0x208]
000D1A5A: mov       r8,QWORD PTR [rdx+0x208]
000D1A61: mov       rdx,QWORD PTR [rdx+0x20]
000D1A65: mov       rcx,r14
000D1A68: call      QWORD PTR [rax+0x498]
000D1A6E: mov       QWORD PTR [rsi+0x1a8],r12
000D1A75: mov       QWORD PTR [rsi+0x1c0],r12
000D1A7C: mov       QWORD PTR [rsi+0x1d0],r12
000D1A83: mov       rcx,QWORD PTR [rbp+0x1a0]
000D1A8A: xor       rcx,rsp
000D1A8D: call      0x18010c270
000D1A92: mov       rbx,QWORD PTR [rsp+0x300]
000D1A9A: add       rsp,0x2b0
000D1AA1: pop       r15
000D1AA3: pop       r14
000D1AA5: pop       r13
000D1AA7: pop       r12
000D1AA9: pop       rdi
000D1AAA: pop       rsi
000D1AAB: pop       rbp
000D1AAC: ret       
