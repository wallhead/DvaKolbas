; ffx_fsr3_x64.dll SHA256=2734c4ffa563f675df9ccb33c6cd6af60ca014e0c7d3eea284816befa83bd12a
; ImageBase=0x180000000, RVAs in left column
; range 0x28B0..0x3227; ffxFsr3ContextDispatchUpscale
000028B0: mov       r11,rsp
000028B3: push      rbp
000028B4: push      r13
000028B6: push      r15
000028B8: lea       rbp,[r11-0x7e8]
000028BF: sub       rsp,0x8d0
000028C6: mov       rax,QWORD PTR [rip+0x5733]        # 0x180008000
000028CD: xor       rax,rsp
000028D0: mov       QWORD PTR [rbp+0x7b0],rax
000028D7: mov       r15,rdx
000028DA: mov       r13,rcx
000028DD: test      rcx,rcx
000028E0: je        0x180003206
000028E6: test      rdx,rdx
000028E9: je        0x180003206
000028EF: movss     xmm0,DWORD PTR [rdx+0x500]
000028F7: xorps     xmm2,xmm2
000028FA: divss     xmm0,DWORD PTR [rip+0x398e]        # 0x180006290
00002902: mov       QWORD PTR [r11+0x18],rbx
00002906: mov       QWORD PTR [r11+0x20],rsi
0000290A: mov       QWORD PTR [r11-0x20],rdi
0000290E: mov       QWORD PTR [r11-0x28],r14
00002912: movss     xmm1,DWORD PTR [rip+0x3972]        # 0x18000628c
0000291A: minss     xmm1,xmm0
0000291E: comiss    xmm2,xmm1
00002921: ja        0x180002926
00002923: movaps    xmm2,xmm1
00002926: movss     DWORD PTR [rcx+0x1805ec],xmm2
0000292E: xor       edx,edx
00002930: mov       ecx,DWORD PTR [rcx+0x1805f4]
00002936: mov       rax,QWORD PTR [r15+0x520]
0000293D: div       rcx
00002940: lea       rcx,[rbp-0x30]
00002944: mov       rbx,rdx
00002947: lea       edi,[rdx+rdx*2]
0000294A: lea       eax,[rdi+0x5]
0000294D: mov       r8d,DWORD PTR [r13+rax*4+0x1805a0]
00002955: lea       rdx,[r13+0x2e8]
0000295C: call      QWORD PTR [r13+0x320]
00002963: lea       rcx,[rsp+0x20]
00002968: lea       r14,[r13+0x1806b0]
0000296F: lea       rdx,[r13+0x2e8]
00002976: movups    xmm0,XMMWORD PTR [rax]
00002979: movups    xmm1,XMMWORD PTR [rax+0x10]
0000297D: movups    XMMWORD PTR [rcx],xmm0
00002980: movups    xmm0,XMMWORD PTR [rax+0x20]
00002984: movups    XMMWORD PTR [rcx+0x10],xmm1
00002988: movups    xmm1,XMMWORD PTR [rax+0x30]
0000298C: movups    XMMWORD PTR [rcx+0x20],xmm0
00002990: movups    xmm0,XMMWORD PTR [rax+0x40]
00002994: movups    XMMWORD PTR [rcx+0x30],xmm1
00002998: movups    xmm1,XMMWORD PTR [rax+0x50]
0000299C: movups    XMMWORD PTR [rcx+0x40],xmm0
000029A0: movups    xmm0,XMMWORD PTR [rax+0x60]
000029A4: movups    XMMWORD PTR [rcx+0x50],xmm1
000029A8: movups    xmm1,XMMWORD PTR [rax+0x80]
000029AF: movups    XMMWORD PTR [rcx+0x60],xmm0
000029B3: movups    xmm0,XMMWORD PTR [rax+0x70]
000029B7: movups    XMMWORD PTR [rcx+0x70],xmm0
000029BB: movups    xmm0,XMMWORD PTR [rax+0x90]
000029C2: movups    XMMWORD PTR [rcx+0x80],xmm1
000029C9: movups    xmm1,XMMWORD PTR [rax+0xa0]
000029D0: lea       rax,[rsp+0x20]
000029D5: movups    XMMWORD PTR [rcx+0x90],xmm0
000029DC: movups    XMMWORD PTR [rcx+0xa0],xmm1
000029E3: movups    xmm0,XMMWORD PTR [rax]
000029E6: movups    xmm1,XMMWORD PTR [rax+0x10]
000029EA: movups    XMMWORD PTR [r14],xmm0
000029EE: movups    xmm0,XMMWORD PTR [rax+0x20]
000029F2: movups    XMMWORD PTR [r14+0x10],xmm1
000029F7: movups    xmm1,XMMWORD PTR [rax+0x30]
000029FB: movups    XMMWORD PTR [r14+0x20],xmm0
00002A00: movups    xmm0,XMMWORD PTR [rax+0x40]
00002A04: movups    XMMWORD PTR [r14+0x30],xmm1
00002A09: movups    xmm1,XMMWORD PTR [rax+0x50]
00002A0D: movups    XMMWORD PTR [r14+0x40],xmm0
00002A12: movups    xmm0,XMMWORD PTR [rax+0x60]
00002A16: movups    XMMWORD PTR [r14+0x50],xmm1
00002A1B: movups    xmm1,XMMWORD PTR [rax+0x70]
00002A1F: movups    XMMWORD PTR [r14+0x60],xmm0
00002A24: movups    xmm0,XMMWORD PTR [rax+0x80]
00002A2B: movups    XMMWORD PTR [r14+0x70],xmm1
00002A30: movups    xmm1,XMMWORD PTR [rax+0x90]
00002A37: movups    XMMWORD PTR [r14+0x80],xmm0
00002A3F: movups    xmm0,XMMWORD PTR [rax+0xa0]
00002A46: lea       eax,[rbx+0x2]
00002A49: lea       ecx,[rax+rax*2]
00002A4C: movups    XMMWORD PTR [r14+0x90],xmm1
00002A54: movups    XMMWORD PTR [r14+0xa0],xmm0
00002A5C: mov       r8d,DWORD PTR [r13+rcx*4+0x1805a0]
00002A64: lea       rcx,[rbp-0x30]
00002A68: call      QWORD PTR [r13+0x320]
00002A6F: lea       rcx,[rsp+0x20]
00002A74: movups    xmm0,XMMWORD PTR [rax]
00002A77: movups    xmm1,XMMWORD PTR [rax+0x10]
00002A7B: movups    XMMWORD PTR [rcx],xmm0
00002A7E: movups    xmm0,XMMWORD PTR [rax+0x20]
00002A82: movups    XMMWORD PTR [rcx+0x10],xmm1
00002A86: movups    xmm1,XMMWORD PTR [rax+0x30]
00002A8A: movups    XMMWORD PTR [rcx+0x20],xmm0
00002A8E: movups    xmm0,XMMWORD PTR [rax+0x40]
00002A92: movups    XMMWORD PTR [rcx+0x30],xmm1
00002A96: movups    xmm1,XMMWORD PTR [rax+0x50]
00002A9A: movups    XMMWORD PTR [rcx+0x40],xmm0
00002A9E: movups    xmm0,XMMWORD PTR [rax+0x60]
00002AA2: movups    XMMWORD PTR [rcx+0x50],xmm1
00002AA6: movups    xmm1,XMMWORD PTR [rax+0x70]
00002AAA: movups    XMMWORD PTR [rcx+0x60],xmm0
00002AAE: movups    xmm0,XMMWORD PTR [rax+0x80]
00002AB5: lea       rbx,[r13+0x180760]
00002ABC: movups    XMMWORD PTR [rcx+0x70],xmm1
00002AC0: lea       rdx,[r13+0x2e8]
00002AC7: movups    xmm1,XMMWORD PTR [rax+0x90]
00002ACE: movups    XMMWORD PTR [rcx+0x80],xmm0
00002AD5: movups    xmm0,XMMWORD PTR [rax+0xa0]
00002ADC: lea       rax,[rsp+0x20]
00002AE1: movups    XMMWORD PTR [rcx+0x90],xmm1
00002AE8: movups    XMMWORD PTR [rcx+0xa0],xmm0
00002AEF: lea       rcx,[rbp-0x30]
00002AF3: movups    xmm0,XMMWORD PTR [rax]
00002AF6: movups    xmm1,XMMWORD PTR [rax+0x10]
00002AFA: movups    XMMWORD PTR [rbx],xmm0
00002AFD: movups    xmm0,XMMWORD PTR [rax+0x20]
00002B01: movups    XMMWORD PTR [rbx+0x10],xmm1
00002B05: movups    xmm1,XMMWORD PTR [rax+0x30]
00002B09: movups    XMMWORD PTR [rbx+0x20],xmm0
00002B0D: movups    xmm0,XMMWORD PTR [rax+0x40]
00002B11: movups    XMMWORD PTR [rbx+0x30],xmm1
00002B15: movups    xmm1,XMMWORD PTR [rax+0x50]
00002B19: movups    XMMWORD PTR [rbx+0x40],xmm0
00002B1D: movups    xmm0,XMMWORD PTR [rax+0x60]
00002B21: movups    XMMWORD PTR [rbx+0x50],xmm1
00002B25: movups    xmm1,XMMWORD PTR [rax+0x70]
00002B29: movups    XMMWORD PTR [rbx+0x60],xmm0
00002B2D: movups    xmm0,XMMWORD PTR [rax+0x80]
00002B34: movups    XMMWORD PTR [rbx+0x70],xmm1
00002B38: movups    xmm1,XMMWORD PTR [rax+0x90]
00002B3F: movups    XMMWORD PTR [rbx+0x80],xmm0
00002B46: movups    xmm0,XMMWORD PTR [rax+0xa0]
00002B4D: lea       eax,[rdi+0x7]
00002B50: movups    XMMWORD PTR [rbx+0x90],xmm1
00002B57: movups    XMMWORD PTR [rbx+0xa0],xmm0
00002B5E: mov       r8d,DWORD PTR [r13+rax*4+0x1805a0]
00002B66: call      QWORD PTR [r13+0x320]
00002B6D: lea       rcx,[rsp+0x20]
00002B72: movups    xmm0,XMMWORD PTR [rax]
00002B75: movups    xmm1,XMMWORD PTR [rax+0x10]
00002B79: movups    XMMWORD PTR [rcx],xmm0
00002B7C: movups    xmm0,XMMWORD PTR [rax+0x20]
00002B80: movups    XMMWORD PTR [rcx+0x10],xmm1
00002B84: movups    xmm1,XMMWORD PTR [rax+0x30]
00002B88: movups    XMMWORD PTR [rcx+0x20],xmm0
00002B8C: movups    xmm0,XMMWORD PTR [rax+0x40]
00002B90: movups    XMMWORD PTR [rcx+0x30],xmm1
00002B94: movups    xmm1,XMMWORD PTR [rax+0x50]
00002B98: movups    XMMWORD PTR [rcx+0x40],xmm0
00002B9C: movups    xmm0,XMMWORD PTR [rax+0x60]
00002BA0: movups    XMMWORD PTR [rcx+0x50],xmm1
00002BA4: movups    xmm1,XMMWORD PTR [rax+0x70]
00002BA8: movups    XMMWORD PTR [rcx+0x60],xmm0
00002BAC: movups    xmm0,XMMWORD PTR [rax+0x80]
00002BB3: movups    XMMWORD PTR [rcx+0x70],xmm1
00002BB7: movups    xmm1,XMMWORD PTR [rax+0x90]
00002BBE: movups    XMMWORD PTR [rcx+0x80],xmm0
00002BC5: movups    xmm0,XMMWORD PTR [rax+0xa0]
00002BCC: lea       rax,[rsp+0x20]
00002BD1: movups    XMMWORD PTR [rcx+0x90],xmm1
00002BD8: movups    XMMWORD PTR [rcx+0xa0],xmm0
00002BDF: lea       rcx,[r13+0x180810]
00002BE6: movups    xmm0,XMMWORD PTR [rax]
00002BE9: movups    xmm1,XMMWORD PTR [rax+0x10]
00002BED: movups    XMMWORD PTR [rcx],xmm0
00002BF0: movups    xmm0,XMMWORD PTR [rax+0x20]
00002BF4: movups    XMMWORD PTR [rcx+0x10],xmm1
00002BF8: movups    xmm1,XMMWORD PTR [rax+0x30]
00002BFC: movups    XMMWORD PTR [rcx+0x20],xmm0
00002C00: movups    xmm0,XMMWORD PTR [rax+0x40]
00002C04: movups    XMMWORD PTR [rcx+0x30],xmm1
00002C08: movups    xmm1,XMMWORD PTR [rax+0x50]
00002C0C: movups    XMMWORD PTR [rcx+0x40],xmm0
00002C10: movups    xmm0,XMMWORD PTR [rax+0x60]
00002C14: movups    XMMWORD PTR [rcx+0x50],xmm1
00002C18: movups    xmm1,XMMWORD PTR [rax+0x70]
00002C1C: movups    XMMWORD PTR [rcx+0x60],xmm0
00002C20: movups    xmm0,XMMWORD PTR [rax+0x80]
00002C27: movups    XMMWORD PTR [rcx+0x70],xmm1
00002C2B: movups    xmm1,XMMWORD PTR [rax+0x90]
00002C32: movups    XMMWORD PTR [rcx+0x80],xmm0
00002C39: movups    XMMWORD PTR [rcx+0x90],xmm1
00002C40: movups    xmm0,XMMWORD PTR [rax+0xa0]
00002C47: xor       edx,edx
00002C49: mov       r8d,0x580
00002C4F: movups    XMMWORD PTR [rcx+0xa0],xmm0
00002C56: lea       rcx,[rbp+0x88]
00002C5D: call      0x180005470
00002C62: xor       edx,edx
00002C64: lea       rcx,[rbp+0x6b8]
00002C6B: mov       r8d,0xb0
00002C71: call      0x180005470
00002C76: movups    xmm0,XMMWORD PTR [r15+0x8]
00002C7B: lea       rcx,[rbp+0x88]
00002C82: xor       eax,eax
00002C84: movups    xmm1,XMMWORD PTR [r15+0x18]
00002C89: mov       QWORD PTR [rbp+0x780],rax
00002C90: mov       WORD PTR [rbp+0x789],ax
00002C97: mov       BYTE PTR [rbp+0x78b],al
00002C9D: mov       WORD PTR [rbp+0x799],ax
00002CA4: mov       BYTE PTR [rbp+0x79b],al
00002CAA: mov       DWORD PTR [rbp+0x7ac],eax
00002CB0: mov       rax,QWORD PTR [r15]
00002CB3: mov       QWORD PTR [rbp+0x80],rax
00002CBA: lea       rax,[r15+0xb8]
00002CC1: movups    XMMWORD PTR [rcx],xmm0
00002CC4: movups    XMMWORD PTR [rcx+0x10],xmm1
00002CC8: movups    xmm0,XMMWORD PTR [r15+0x28]
00002CCD: movups    xmm1,XMMWORD PTR [r15+0x38]
00002CD2: movups    XMMWORD PTR [rcx+0x20],xmm0
00002CD6: movups    XMMWORD PTR [rcx+0x30],xmm1
00002CDA: movups    xmm0,XMMWORD PTR [r15+0x48]
00002CDF: movups    xmm1,XMMWORD PTR [r15+0x58]
00002CE4: movups    XMMWORD PTR [rcx+0x40],xmm0
00002CE8: movups    xmm0,XMMWORD PTR [r15+0x68]
00002CED: movups    XMMWORD PTR [rcx+0x50],xmm1
00002CF1: movups    XMMWORD PTR [rcx+0x60],xmm0
00002CF5: movups    xmm1,XMMWORD PTR [r15+0x78]
00002CFA: movups    xmm0,XMMWORD PTR [r15+0x88]
00002D02: movups    XMMWORD PTR [rcx+0x70],xmm1
00002D06: movups    xmm1,XMMWORD PTR [r15+0x98]
00002D0E: movups    XMMWORD PTR [rcx+0x80],xmm0
00002D15: movups    xmm0,XMMWORD PTR [r15+0xa8]
00002D1D: movups    XMMWORD PTR [rcx+0x90],xmm1
00002D24: movups    xmm1,XMMWORD PTR [rax+0x10]
00002D28: movups    XMMWORD PTR [rcx+0xa0],xmm0
00002D2F: lea       rcx,[rbp+0x138]
00002D36: movups    xmm0,XMMWORD PTR [rax]
00002D39: movups    XMMWORD PTR [rcx],xmm0
00002D3C: movups    xmm0,XMMWORD PTR [rax+0x20]
00002D40: movups    XMMWORD PTR [rcx+0x10],xmm1
00002D44: movups    xmm1,XMMWORD PTR [rax+0x30]
00002D48: movups    XMMWORD PTR [rcx+0x20],xmm0
00002D4C: movups    xmm0,XMMWORD PTR [rax+0x40]
00002D50: movups    XMMWORD PTR [rcx+0x30],xmm1
00002D54: movups    xmm1,XMMWORD PTR [rax+0x50]
00002D58: movups    XMMWORD PTR [rcx+0x40],xmm0
00002D5C: movups    xmm0,XMMWORD PTR [rax+0x60]
00002D60: movups    XMMWORD PTR [rcx+0x50],xmm1
00002D64: movups    xmm1,XMMWORD PTR [rax+0x70]
00002D68: movups    XMMWORD PTR [rcx+0x60],xmm0
00002D6C: movups    xmm0,XMMWORD PTR [rax+0x80]
00002D73: movups    XMMWORD PTR [rcx+0x70],xmm1
00002D77: movups    xmm1,XMMWORD PTR [rax+0x90]
00002D7E: movups    XMMWORD PTR [rcx+0x80],xmm0
00002D85: movups    xmm0,XMMWORD PTR [rax+0xa0]
00002D8C: lea       rax,[r15+0x168]
00002D93: movups    XMMWORD PTR [rcx+0x90],xmm1
00002D9A: movups    xmm1,XMMWORD PTR [rax+0x10]
00002D9E: movups    XMMWORD PTR [rcx+0xa0],xmm0
00002DA5: lea       rcx,[rbp+0x1e8]
00002DAC: movups    xmm0,XMMWORD PTR [rax]
00002DAF: movups    XMMWORD PTR [rcx],xmm0
00002DB2: movups    xmm0,XMMWORD PTR [rax+0x20]
00002DB6: movups    XMMWORD PTR [rcx+0x10],xmm1
00002DBA: movups    xmm1,XMMWORD PTR [rax+0x30]
00002DBE: movups    XMMWORD PTR [rcx+0x20],xmm0
00002DC2: movups    xmm0,XMMWORD PTR [rax+0x40]
00002DC6: movups    XMMWORD PTR [rcx+0x30],xmm1
00002DCA: movups    xmm1,XMMWORD PTR [rax+0x50]
00002DCE: movups    XMMWORD PTR [rcx+0x40],xmm0
00002DD2: movups    xmm0,XMMWORD PTR [rax+0x60]
00002DD6: movups    XMMWORD PTR [rcx+0x50],xmm1
00002DDA: movups    XMMWORD PTR [rcx+0x60],xmm0
00002DDE: movups    xmm1,XMMWORD PTR [rax+0x70]
00002DE2: movups    xmm0,XMMWORD PTR [rax+0x80]
00002DE9: movups    XMMWORD PTR [rcx+0x70],xmm1
00002DED: movups    XMMWORD PTR [rcx+0x80],xmm0
00002DF4: movups    xmm1,XMMWORD PTR [rax+0x90]
00002DFB: movups    xmm0,XMMWORD PTR [rax+0xa0]
00002E02: lea       rax,[r15+0x218]
00002E09: movups    XMMWORD PTR [rcx+0x90],xmm1
00002E10: movups    XMMWORD PTR [rcx+0xa0],xmm0
00002E17: lea       rcx,[rbp+0x298]
00002E1E: movups    xmm0,XMMWORD PTR [rax]
00002E21: movups    xmm1,XMMWORD PTR [rax+0x10]
00002E25: movups    XMMWORD PTR [rcx],xmm0
00002E28: movups    XMMWORD PTR [rcx+0x10],xmm1
00002E2C: movups    xmm0,XMMWORD PTR [rax+0x20]
00002E30: movups    xmm1,XMMWORD PTR [rax+0x30]
00002E34: movups    XMMWORD PTR [rcx+0x20],xmm0
00002E38: movups    XMMWORD PTR [rcx+0x30],xmm1
00002E3C: movups    xmm0,XMMWORD PTR [rax+0x40]
00002E40: movups    xmm1,XMMWORD PTR [rax+0x50]
00002E44: movups    XMMWORD PTR [rcx+0x40],xmm0
00002E48: movups    XMMWORD PTR [rcx+0x50],xmm1
00002E4C: movups    xmm0,XMMWORD PTR [rax+0x60]
00002E50: movups    xmm1,XMMWORD PTR [rax+0x70]
00002E54: movups    XMMWORD PTR [rcx+0x60],xmm0
00002E58: movups    XMMWORD PTR [rcx+0x70],xmm1
00002E5C: movups    xmm0,XMMWORD PTR [rax+0x80]
00002E63: movups    xmm1,XMMWORD PTR [rax+0x90]
00002E6A: movups    XMMWORD PTR [rcx+0x80],xmm0
00002E71: movups    xmm0,XMMWORD PTR [rax+0xa0]
00002E78: lea       rax,[r15+0x2c8]
00002E7F: movups    XMMWORD PTR [rcx+0x90],xmm1
00002E86: movups    XMMWORD PTR [rcx+0xa0],xmm0
00002E8D: lea       rcx,[rbp+0x348]
00002E94: movups    xmm0,XMMWORD PTR [rax]
00002E97: movups    xmm1,XMMWORD PTR [rax+0x10]
00002E9B: movups    XMMWORD PTR [rcx],xmm0
00002E9E: movups    xmm0,XMMWORD PTR [rax+0x20]
00002EA2: movups    XMMWORD PTR [rcx+0x10],xmm1
00002EA6: movups    xmm1,XMMWORD PTR [rax+0x30]
00002EAA: movups    XMMWORD PTR [rcx+0x20],xmm0
00002EAE: movups    xmm0,XMMWORD PTR [rax+0x40]
00002EB2: movups    XMMWORD PTR [rcx+0x30],xmm1
00002EB6: movups    xmm1,XMMWORD PTR [rax+0x50]
00002EBA: movups    XMMWORD PTR [rcx+0x40],xmm0
00002EBE: movups    xmm0,XMMWORD PTR [rax+0x60]
00002EC2: movups    XMMWORD PTR [rcx+0x50],xmm1
00002EC6: movups    xmm1,XMMWORD PTR [rax+0x70]
00002ECA: movups    XMMWORD PTR [rcx+0x60],xmm0
00002ECE: movups    xmm0,XMMWORD PTR [rax+0x80]
00002ED5: movups    XMMWORD PTR [rcx+0x70],xmm1
00002ED9: movups    xmm1,XMMWORD PTR [rax+0x90]
00002EE0: movups    XMMWORD PTR [rcx+0x80],xmm0
00002EE7: movups    xmm0,XMMWORD PTR [rax+0xa0]
00002EEE: lea       rax,[r15+0x378]
00002EF5: movups    XMMWORD PTR [rcx+0x90],xmm1
00002EFC: movups    xmm1,XMMWORD PTR [rax+0x10]
00002F00: movups    XMMWORD PTR [rcx+0xa0],xmm0
00002F07: lea       rcx,[rbp+0x3f8]
00002F0E: movups    xmm0,XMMWORD PTR [rax]
00002F11: movups    XMMWORD PTR [rcx],xmm0
00002F14: movups    xmm0,XMMWORD PTR [rax+0x20]
00002F18: movups    XMMWORD PTR [rcx+0x10],xmm1
00002F1C: movups    xmm1,XMMWORD PTR [rax+0x30]
00002F20: movups    XMMWORD PTR [rcx+0x20],xmm0
00002F24: movups    xmm0,XMMWORD PTR [rax+0x40]
00002F28: movups    XMMWORD PTR [rcx+0x30],xmm1
00002F2C: movups    xmm1,XMMWORD PTR [rax+0x50]
00002F30: movups    XMMWORD PTR [rcx+0x40],xmm0
00002F34: movups    xmm0,XMMWORD PTR [rax+0x60]
00002F38: movups    XMMWORD PTR [rcx+0x50],xmm1
00002F3C: movups    xmm1,XMMWORD PTR [rax+0x70]
00002F40: movups    XMMWORD PTR [rcx+0x60],xmm0
00002F44: movups    xmm0,XMMWORD PTR [rax+0x80]
00002F4B: movups    XMMWORD PTR [rcx+0x70],xmm1
00002F4F: movups    xmm1,XMMWORD PTR [rax+0x90]
00002F56: movups    XMMWORD PTR [rcx+0x80],xmm0
00002F5D: movups    xmm0,XMMWORD PTR [rax+0xa0]
00002F64: movups    XMMWORD PTR [rcx+0x90],xmm1
00002F6B: movups    XMMWORD PTR [rcx+0xa0],xmm0
00002F72: lea       rax,[r15+0x428]
00002F79: movups    xmm0,XMMWORD PTR [rax]
00002F7C: lea       rcx,[rbp+0x6b8]
00002F83: movups    xmm1,XMMWORD PTR [rax+0x10]
00002F87: movups    XMMWORD PTR [rcx],xmm0
00002F8A: movups    XMMWORD PTR [rcx+0x10],xmm1
00002F8E: movups    xmm0,XMMWORD PTR [rax+0x20]
00002F92: movups    xmm1,XMMWORD PTR [rax+0x30]
00002F96: movups    XMMWORD PTR [rcx+0x20],xmm0
00002F9A: movups    XMMWORD PTR [rcx+0x30],xmm1
00002F9E: movups    xmm0,XMMWORD PTR [rax+0x40]
00002FA2: movups    xmm1,XMMWORD PTR [rax+0x50]
00002FA6: movups    XMMWORD PTR [rcx+0x40],xmm0
00002FAA: movups    XMMWORD PTR [rcx+0x50],xmm1
00002FAE: movups    xmm0,XMMWORD PTR [rax+0x60]
00002FB2: movups    xmm1,XMMWORD PTR [rax+0x70]
00002FB6: movups    XMMWORD PTR [rcx+0x60],xmm0
00002FBA: movups    XMMWORD PTR [rcx+0x70],xmm1
00002FBE: movups    xmm0,XMMWORD PTR [rax+0x80]
00002FC5: movups    xmm1,XMMWORD PTR [rax+0x90]
00002FCC: movups    XMMWORD PTR [rcx+0x80],xmm0
00002FD3: movups    xmm0,XMMWORD PTR [rax+0xa0]
00002FDA: mov       rax,QWORD PTR [r15+0x4e8]
00002FE1: movups    XMMWORD PTR [rcx+0x90],xmm1
00002FE8: movups    XMMWORD PTR [rcx+0xa0],xmm0
00002FEF: mov       QWORD PTR [rbp+0x778],rax
00002FF6: movzx     eax,BYTE PTR [r15+0x4f8]
00002FFE: movss     xmm0,DWORD PTR [r15+0x4fc]
00003007: movups    xmm1,XMMWORD PTR [r15+0x4d8]
0000300F: mov       BYTE PTR [rbp+0x788],al
00003015: movzx     eax,BYTE PTR [r15+0x508]
0000301D: movss     DWORD PTR [rbp+0x78c],xmm0
00003025: movss     xmm0,DWORD PTR [r15+0x504]
0000302E: movss     DWORD PTR [rbp+0x794],xmm0
00003036: movups    xmm0,XMMWORD PTR [r15+0x50c]
0000303E: mov       BYTE PTR [rbp+0x798],al
00003044: lea       rax,[rbp+0x4a8]
0000304B: movups    XMMWORD PTR [rbp+0x768],xmm1
00003052: movss     xmm1,DWORD PTR [r15+0x500]
0000305B: movups    XMMWORD PTR [rbp+0x79c],xmm0
00003062: movups    xmm0,XMMWORD PTR [r14]
00003066: movss     DWORD PTR [rbp+0x790],xmm1
0000306E: movups    xmm1,XMMWORD PTR [r14+0x10]
00003073: movups    XMMWORD PTR [rax],xmm0
00003076: movups    xmm0,XMMWORD PTR [r14+0x20]
0000307B: movups    XMMWORD PTR [rax+0x10],xmm1
0000307F: movups    xmm1,XMMWORD PTR [r14+0x30]
00003084: movups    XMMWORD PTR [rax+0x20],xmm0
00003088: movups    xmm0,XMMWORD PTR [r14+0x40]
0000308D: movups    XMMWORD PTR [rax+0x30],xmm1
00003091: movups    xmm1,XMMWORD PTR [r14+0x50]
00003096: movups    XMMWORD PTR [rax+0x40],xmm0
0000309A: movups    xmm0,XMMWORD PTR [r14+0x60]
0000309F: movups    XMMWORD PTR [rax+0x50],xmm1
000030A3: movups    xmm1,XMMWORD PTR [r14+0x70]
000030A8: movups    XMMWORD PTR [rax+0x60],xmm0
000030AC: movups    xmm0,XMMWORD PTR [r14+0x80]
000030B4: movups    XMMWORD PTR [rax+0x70],xmm1
000030B8: movups    xmm1,XMMWORD PTR [r14+0x90]
000030C0: movups    XMMWORD PTR [rax+0x80],xmm0
000030C7: movups    xmm0,XMMWORD PTR [r14+0xa0]
000030CF: movups    XMMWORD PTR [rax+0x90],xmm1
000030D6: movups    xmm1,XMMWORD PTR [rbx+0x10]
000030DA: movups    XMMWORD PTR [rax+0xa0],xmm0
000030E1: lea       rax,[rbp+0x558]
000030E8: movups    xmm0,XMMWORD PTR [rbx]
000030EB: movups    XMMWORD PTR [rax],xmm0
000030EE: movups    xmm0,XMMWORD PTR [rbx+0x20]
000030F2: movups    XMMWORD PTR [rax+0x10],xmm1
000030F6: movups    xmm1,XMMWORD PTR [rbx+0x30]
000030FA: movups    XMMWORD PTR [rax+0x20],xmm0
000030FE: movups    xmm0,XMMWORD PTR [rbx+0x40]
00003102: movups    XMMWORD PTR [rax+0x30],xmm1
00003106: movups    xmm1,XMMWORD PTR [rbx+0x50]
0000310A: movups    XMMWORD PTR [rax+0x40],xmm0
0000310E: movups    xmm0,XMMWORD PTR [rbx+0x60]
00003112: movups    XMMWORD PTR [rax+0x50],xmm1
00003116: movups    xmm1,XMMWORD PTR [rbx+0x70]
0000311A: movups    XMMWORD PTR [rax+0x60],xmm0
0000311E: movups    xmm0,XMMWORD PTR [rbx+0x80]
00003125: movups    XMMWORD PTR [rax+0x70],xmm1
00003129: test      BYTE PTR [r15+0x51c],0x1
00003131: lea       rcx,[rbp+0x608]
00003138: movups    xmm1,XMMWORD PTR [rbx+0x90]
0000313F: mov       r14,QWORD PTR [rsp+0x8c0]
00003147: mov       rdi,QWORD PTR [rsp+0x8c8]
0000314F: mov       rsi,QWORD PTR [rsp+0x908]
00003157: movups    XMMWORD PTR [rax+0x80],xmm0
0000315E: movups    xmm0,XMMWORD PTR [rbx+0xa0]
00003165: mov       rbx,QWORD PTR [rsp+0x900]
0000316D: movups    XMMWORD PTR [rax+0x90],xmm1
00003174: movups    XMMWORD PTR [rax+0xa0],xmm0
0000317B: lea       rax,[rsp+0x20]
00003180: movups    xmm0,XMMWORD PTR [rax]
00003183: movups    xmm1,XMMWORD PTR [rax+0x10]
00003187: movups    XMMWORD PTR [rcx],xmm0
0000318A: movups    xmm0,XMMWORD PTR [rax+0x20]
0000318E: movups    XMMWORD PTR [rcx+0x10],xmm1
00003192: movups    xmm1,XMMWORD PTR [rax+0x30]
00003196: movups    XMMWORD PTR [rcx+0x20],xmm0
0000319A: movups    xmm0,XMMWORD PTR [rax+0x40]
0000319E: movups    XMMWORD PTR [rcx+0x30],xmm1
000031A2: movups    xmm1,XMMWORD PTR [rax+0x50]
000031A6: movups    XMMWORD PTR [rcx+0x40],xmm0
000031AA: movups    xmm0,XMMWORD PTR [rax+0x60]
000031AE: movups    XMMWORD PTR [rcx+0x50],xmm1
000031B2: movups    xmm1,XMMWORD PTR [rax+0x70]
000031B6: movups    XMMWORD PTR [rcx+0x60],xmm0
000031BA: movups    xmm0,XMMWORD PTR [rax+0x80]
000031C1: movups    XMMWORD PTR [rcx+0x70],xmm1
000031C5: movups    xmm1,XMMWORD PTR [rax+0x90]
000031CC: movups    XMMWORD PTR [rcx+0x80],xmm0
000031D3: movups    xmm0,XMMWORD PTR [rax+0xa0]
000031DA: movups    XMMWORD PTR [rcx+0x90],xmm1
000031E1: movups    XMMWORD PTR [rcx+0xa0],xmm0
000031E8: je        0x1800031f1
000031EA: or        DWORD PTR [rbp+0x7ac],0x1
000031F1: lea       rcx,[r13+0x5a0]
000031F8: lea       rdx,[rbp+0x80]
000031FF: call      0x180003eba
00003204: jmp       0x18000320b
00003206: mov       eax,0x80000000
0000320B: mov       rcx,QWORD PTR [rbp+0x7b0]
00003212: xor       rcx,rsp
00003215: call      0x1800047b0
0000321A: add       rsp,0x8d0
00003221: pop       r15
00003223: pop       r13
00003225: pop       rbp
00003226: ret       
