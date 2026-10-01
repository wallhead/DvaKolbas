; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xFF520..0xFF9C6; unnamed
000FF520: mov       rax,rsp
000FF523: mov       QWORD PTR [rax+0x18],rbx
000FF527: push      rbp
000FF528: push      rsi
000FF529: push      rdi
000FF52A: lea       rbp,[rax-0x158]
000FF531: sub       rsp,0x240
000FF538: movaps    XMMWORD PTR [rax-0x28],xmm6
000FF53C: movaps    XMMWORD PTR [rax-0x38],xmm7
000FF540: mov       rax,QWORD PTR [rip+0x10cb479]        # 0x1811ca9c0
000FF547: xor       rax,rsp
000FF54A: mov       QWORD PTR [rbp+0x110],rax
000FF551: mov       rsi,rdx
000FF554: mov       rbx,rcx
000FF557: movss     xmm7,DWORD PTR [rdx+0x38]
000FF55C: movsxd    rdi,DWORD PTR [rdx]
000FF55F: movd      xmm0,DWORD PTR [rcx+rdi*4+0x13c]
000FF568: cvtdq2ps  xmm0,xmm0
000FF56B: ucomiss   xmm7,xmm0
000FF56E: jp        0x1800ff58a
000FF570: jne       0x1800ff58a
000FF572: movss     xmm6,DWORD PTR [rdx+0x3c]
000FF577: movd      xmm0,DWORD PTR [rcx+rdi*4+0x144]
000FF580: cvtdq2ps  xmm0,xmm0
000FF583: ucomiss   xmm6,xmm0
000FF586: jp        0x1800ff58a
000FF588: je        0x1800ff5b5
000FF58A: mov       edx,0x51
000FF58F: mov       r8,rdi
000FF592: mov       BYTE PTR [rdi+rcx*1+0x138],0x0
000FF59A: cvttss2si eax,xmm7
000FF59E: mov       DWORD PTR [rcx+rdi*4+0x13c],eax
000FF5A5: movss     xmm6,DWORD PTR [rsi+0x3c]
000FF5AA: cvttss2si ecx,xmm6
000FF5AE: lea       rax,[rdx+rdi*1]
000FF5B2: mov       DWORD PTR [rbx+rax*4],ecx
000FF5B5: cmp       BYTE PTR [rdi+rbx*1+0x138],0x0
000FF5BD: jne       0x1800ff643
000FF5C3: lea       rcx,[rbp-0x40]
000FF5C7: movups    xmm0,XMMWORD PTR [rsi]
000FF5CA: movups    XMMWORD PTR [rcx],xmm0
000FF5CD: movups    xmm1,XMMWORD PTR [rsi+0x10]
000FF5D1: movups    XMMWORD PTR [rcx+0x10],xmm1
000FF5D5: movups    xmm0,XMMWORD PTR [rsi+0x20]
000FF5D9: movups    XMMWORD PTR [rcx+0x20],xmm0
000FF5DD: movups    xmm1,XMMWORD PTR [rsi+0x30]
000FF5E1: movups    XMMWORD PTR [rcx+0x30],xmm1
000FF5E5: movups    xmm0,XMMWORD PTR [rsi+0x40]
000FF5E9: movups    XMMWORD PTR [rcx+0x40],xmm0
000FF5ED: movups    xmm1,XMMWORD PTR [rsi+0x50]
000FF5F1: movups    XMMWORD PTR [rcx+0x50],xmm1
000FF5F5: movups    xmm0,XMMWORD PTR [rsi+0x60]
000FF5F9: movups    XMMWORD PTR [rcx+0x60],xmm0
000FF5FD: movups    xmm1,XMMWORD PTR [rsi+0x70]
000FF601: movups    XMMWORD PTR [rcx+0x70],xmm1
000FF605: movups    xmm0,XMMWORD PTR [rsi+0x80]
000FF60C: movups    XMMWORD PTR [rcx+0x80],xmm0
000FF613: movups    xmm1,XMMWORD PTR [rsi+0x90]
000FF61A: movups    XMMWORD PTR [rcx+0x90],xmm1
000FF621: movups    xmm0,XMMWORD PTR [rsi+0xa0]
000FF628: movups    XMMWORD PTR [rcx+0xa0],xmm0
000FF62F: lea       rdx,[rbp-0x40]
000FF633: mov       rcx,rbx
000FF636: call      0x1800fe750
000FF63B: mov       BYTE PTR [rdi+rbx*1+0x138],0x1
000FF643: mov       rcx,QWORD PTR [rsi+0x8]
000FF647: mov       rax,QWORD PTR [rcx]
000FF64A: lea       rdx,[rbp+0xe0]
000FF651: call      QWORD PTR [rax+0x50]
000FF654: imul      rdi,rdi,0x70
000FF658: mov       rcx,QWORD PTR [rdi+rbx*1+0x170]
000FF660: test      rcx,rcx
000FF663: je        0x1800ff66f
000FF665: mov       rax,QWORD PTR [rcx]
000FF668: lea       rdx,[rbp+0x70]
000FF66C: call      QWORD PTR [rax+0x50]
000FF66F: movups    xmm0,XMMWORD PTR [rbp+0x70]
000FF673: movups    XMMWORD PTR [rdi+rbx*1+0x190],xmm0
000FF67B: movups    xmm1,XMMWORD PTR [rbp+0x80]
000FF682: movups    XMMWORD PTR [rdi+rbx*1+0x1a0],xmm1
000FF68A: movsd     xmm0,QWORD PTR [rbp+0x90]
000FF692: movsd     QWORD PTR [rdi+rbx*1+0x1b0],xmm0
000FF69B: mov       eax,DWORD PTR [rbp+0x98]
000FF6A1: mov       DWORD PTR [rdi+rbx*1+0x1b8],eax
000FF6A8: movd      eax,xmm1
000FF6AC: cmp       DWORD PTR [rbp+0xf0],eax
000FF6B2: je        0x1800ff70d
000FF6B4: cvttss2si rax,xmm7
000FF6B9: mov       DWORD PTR [rbp+0xe0],eax
000FF6BF: cvttss2si rax,xmm6
000FF6C4: mov       DWORD PTR [rbp+0xe4],eax
000FF6CA: movups    xmm0,XMMWORD PTR [rbp+0xe0]
000FF6D1: movaps    XMMWORD PTR [rbp+0x70],xmm0
000FF6D5: movups    xmm1,XMMWORD PTR [rbp+0xf0]
000FF6DC: movaps    XMMWORD PTR [rbp+0x80],xmm1
000FF6E3: movsd     xmm0,QWORD PTR [rbp+0x100]
000FF6EB: movsd     QWORD PTR [rbp+0x90],xmm0
000FF6F3: mov       eax,DWORD PTR [rbp+0x108]
000FF6F9: mov       DWORD PTR [rbp+0x98],eax
000FF6FF: lea       r8,[rbp+0x70]
000FF703: mov       edx,DWORD PTR [rsi]
000FF705: mov       rcx,rbx
000FF708: call      0x1800fea20 ; '@VWAVH'
000FF70D: lea       rax,[rbp+0x70]
000FF711: mov       QWORD PTR [rsp+0x30],rax
000FF716: mov       rax,QWORD PTR [rdi+rbx*1+0x330]
000FF71E: mov       QWORD PTR [rbp+0x70],rax
000FF722: mov       rax,QWORD PTR [rdi+rbx*1+0x338]
000FF72A: mov       QWORD PTR [rbp+0x78],rax
000FF72E: mov       rax,QWORD PTR [rdi+rbx*1+0x340]
000FF736: mov       QWORD PTR [rbp+0x80],rax
000FF73D: mov       rax,QWORD PTR [rdi+rbx*1+0x348]
000FF745: mov       QWORD PTR [rbp+0x88],rax
000FF74C: movups    xmm0,XMMWORD PTR [rdi+rbx*1+0x350]
000FF754: movups    XMMWORD PTR [rbp+0x90],xmm0
000FF75B: movups    xmm1,XMMWORD PTR [rdi+rbx*1+0x360]
000FF763: movups    XMMWORD PTR [rbp+0xa0],xmm1
000FF76A: movsd     xmm0,QWORD PTR [rdi+rbx*1+0x370]
000FF773: movsd     QWORD PTR [rbp+0xb0],xmm0
000FF77B: mov       eax,DWORD PTR [rdi+rbx*1+0x378]
000FF782: mov       DWORD PTR [rbp+0xb8],eax
000FF788: lea       rdx,[rbx+0x380]
000FF78F: add       rdx,rdi
000FF792: lea       rcx,[rbp+0xc0]
000FF799: call      0x180080190
000FF79E: nop       
000FF79F: lea       rax,[rsp+0x48]
000FF7A4: mov       QWORD PTR [rsp+0x38],rax
000FF7A9: mov       rax,QWORD PTR [rdi+rbx*1+0x250]
000FF7B1: mov       QWORD PTR [rsp+0x48],rax
000FF7B6: mov       rax,QWORD PTR [rdi+rbx*1+0x258]
000FF7BE: mov       QWORD PTR [rsp+0x50],rax
000FF7C3: mov       rax,QWORD PTR [rdi+rbx*1+0x260]
000FF7CB: mov       QWORD PTR [rsp+0x58],rax
000FF7D0: mov       rax,QWORD PTR [rdi+rbx*1+0x268]
000FF7D8: mov       QWORD PTR [rsp+0x60],rax
000FF7DD: movups    xmm0,XMMWORD PTR [rdi+rbx*1+0x270]
000FF7E5: movups    XMMWORD PTR [rsp+0x68],xmm0
000FF7EA: movups    xmm1,XMMWORD PTR [rdi+rbx*1+0x280]
000FF7F2: movups    XMMWORD PTR [rsp+0x78],xmm1
000FF7F7: movsd     xmm0,QWORD PTR [rdi+rbx*1+0x290]
000FF800: movsd     QWORD PTR [rbp-0x78],xmm0
000FF805: mov       eax,DWORD PTR [rdi+rbx*1+0x298]
000FF80C: mov       DWORD PTR [rbp-0x70],eax
000FF80F: lea       rdx,[rbx+0x2a0]
000FF816: add       rdx,rdi
000FF819: lea       rcx,[rbp-0x68]
000FF81D: call      0x180080190
000FF822: nop       
000FF823: mov       rax,QWORD PTR [rdi+rbx*1+0x170]
000FF82B: mov       QWORD PTR [rbp-0x40],rax
000FF82F: mov       rax,QWORD PTR [rdi+rbx*1+0x178]
000FF837: mov       QWORD PTR [rbp-0x38],rax
000FF83B: mov       rax,QWORD PTR [rdi+rbx*1+0x180]
000FF843: mov       QWORD PTR [rbp-0x30],rax
000FF847: mov       rax,QWORD PTR [rdi+rbx*1+0x188]
000FF84F: mov       QWORD PTR [rbp-0x28],rax
000FF853: movups    xmm0,XMMWORD PTR [rdi+rbx*1+0x190]
000FF85B: movups    XMMWORD PTR [rbp-0x20],xmm0
000FF85F: movups    xmm1,XMMWORD PTR [rdi+rbx*1+0x1a0]
000FF867: movups    XMMWORD PTR [rbp-0x10],xmm1
000FF86B: movsd     xmm0,QWORD PTR [rdi+rbx*1+0x1b0]
000FF874: movsd     QWORD PTR [rbp+0x0],xmm0
000FF879: mov       eax,DWORD PTR [rdi+rbx*1+0x1b8]
000FF880: mov       DWORD PTR [rbp+0x8],eax
000FF883: lea       rdx,[rbx+0x1c0]
000FF88A: add       rdx,rdi
000FF88D: lea       rcx,[rbp+0x10]
000FF891: call      0x180080190
000FF896: nop       
000FF897: lea       rax,[rbp+0x70]
000FF89B: mov       QWORD PTR [rsp+0x28],rax
000FF8A0: lea       rax,[rsp+0x48]
000FF8A5: mov       QWORD PTR [rsp+0x20],rax
000FF8AA: lea       r9,[rbp-0x40]
000FF8AE: mov       r8,rsi
000FF8B1: mov       rdx,QWORD PTR [rbx+0x120]
000FF8B8: call      0x1800feb50
000FF8BD: movsxd    rax,DWORD PTR [rsi]
000FF8C0: imul      rcx,rax,0x70
000FF8C4: mov       rax,QWORD PTR [rcx+rbx*1+0x170]
000FF8CC: mov       QWORD PTR [rbp+0x78],rax
000FF8D0: mov       rax,QWORD PTR [rcx+rbx*1+0x330]
000FF8D8: mov       QWORD PTR [rbp+0x80],rax
000FF8DF: mov       rax,QWORD PTR [rcx+rbx*1+0x250]
000FF8E7: mov       QWORD PTR [rbp+0x88],rax
000FF8EE: mov       rax,QWORD PTR [rsi+0x28]
000FF8F2: mov       QWORD PTR [rbp+0xa0],rax
000FF8F9: mov       rax,QWORD PTR [rsi+0x30]
000FF8FD: mov       QWORD PTR [rbp+0xa8],rax
000FF904: mov       rax,QWORD PTR [rcx+rbx*1+0x410]
000FF90C: mov       QWORD PTR [rbp+0x90],rax
000FF913: xorps     xmm0,xmm0
000FF916: movdqa    XMMWORD PTR [rbp+0xb0],xmm0
000FF91E: lea       rax,[rbp-0x40]
000FF922: movups    xmm0,XMMWORD PTR [rsi]
000FF925: movups    XMMWORD PTR [rax],xmm0
000FF928: movups    xmm1,XMMWORD PTR [rsi+0x10]
000FF92C: movups    XMMWORD PTR [rax+0x10],xmm1
000FF930: movups    xmm0,XMMWORD PTR [rsi+0x20]
000FF934: movups    XMMWORD PTR [rax+0x20],xmm0
000FF938: movups    xmm1,XMMWORD PTR [rsi+0x30]
000FF93C: movups    XMMWORD PTR [rax+0x30],xmm1
000FF940: movups    xmm0,XMMWORD PTR [rsi+0x40]
000FF944: movups    XMMWORD PTR [rax+0x40],xmm0
000FF948: movups    xmm1,XMMWORD PTR [rsi+0x50]
000FF94C: movups    XMMWORD PTR [rax+0x50],xmm1
000FF950: movups    xmm0,XMMWORD PTR [rsi+0x60]
000FF954: movups    XMMWORD PTR [rax+0x60],xmm0
000FF958: movups    xmm1,XMMWORD PTR [rsi+0x70]
000FF95C: movups    XMMWORD PTR [rax+0x70],xmm1
000FF960: movups    xmm0,XMMWORD PTR [rsi+0x80]
000FF967: movups    XMMWORD PTR [rax+0x80],xmm0
000FF96E: movups    xmm1,XMMWORD PTR [rsi+0x90]
000FF975: movups    XMMWORD PTR [rax+0x90],xmm1
000FF97C: movups    xmm0,XMMWORD PTR [rsi+0xa0]
000FF983: movups    XMMWORD PTR [rax+0xa0],xmm0
000FF98A: lea       r8,[rbp+0x70]
000FF98E: lea       rdx,[rbp-0x40]
000FF992: mov       rcx,rbx
000FF995: call      0x1800fedc0
000FF99A: mov       rcx,QWORD PTR [rbp+0x110]
000FF9A1: xor       rcx,rsp
000FF9A4: call      0x18010c270
000FF9A9: lea       r11,[rsp+0x240]
000FF9B1: mov       rbx,QWORD PTR [r11+0x30]
000FF9B5: movaps    xmm6,XMMWORD PTR [r11-0x10]
000FF9BA: movaps    xmm7,XMMWORD PTR [r11-0x20]
000FF9BF: mov       rsp,r11
000FF9C2: pop       rdi
000FF9C3: pop       rsi
000FF9C4: pop       rbp
000FF9C5: ret       
