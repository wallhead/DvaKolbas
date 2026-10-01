; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xFA3E0..0xFA44E; GetJitterOffset
000FA3E0: mov       QWORD PTR [rsp+0x8],rbx
000FA3E5: mov       QWORD PTR [rsp+0x10],rsi
000FA3EA: push      rdi
000FA3EB: sub       rsp,0x20
000FA3EF: mov       rsi,rdx
000FA3F2: mov       r10d,0x8
000FA3F8: cmp       r9d,r10d
000FA3FB: mov       eax,r8d
000FA3FE: cdq       
000FA3FF: mov       rdi,rcx
000FA402: cmovg     r10d,r9d
000FA406: idiv      r10d
000FA409: lea       ebx,[rdx+0x1]
000FA40C: mov       edx,0x2
000FA411: mov       ecx,ebx
000FA413: call      0x1800fa300
000FA418: subss     xmm0,DWORD PTR [rip+0x10b2680]        # 0x1811acaa0
000FA420: mov       edx,0x3
000FA425: mov       ecx,ebx
000FA427: movss     DWORD PTR [rdi],xmm0
000FA42B: call      0x1800fa300
000FA430: subss     xmm0,DWORD PTR [rip+0x10b2668]        # 0x1811acaa0
000FA438: mov       rbx,QWORD PTR [rsp+0x30]
000FA43D: xor       eax,eax
000FA43F: movss     DWORD PTR [rsi],xmm0
000FA443: mov       rsi,QWORD PTR [rsp+0x38]
000FA448: add       rsp,0x20
000FA44C: pop       rdi
000FA44D: ret       
