; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x293DD0..0x293E29; unnamed
00293DD0: mov       QWORD PTR [rsp+0x8],rbx
00293DD5: mov       QWORD PTR [rsp+0x10],rsi
00293DDA: push      rdi
00293DDB: sub       rsp,0x20
00293DDF: mov       rbx,QWORD PTR [rip+0xbecffa]        # 0x180e80de0
00293DE6: xor       ecx,ecx
00293DE8: mov       rdi,r8
00293DEB: mov       rsi,rdx
00293DEE: call      QWORD PTR [rip+0x1dee7c]        # 0x180472c70 ; PDPerfPlugin.dll!GetJitterPhaseCount
00293DF4: vmovss    xmm0,DWORD PTR [rbx+0x4]
00293DF9: vaddss    xmm1,xmm0,DWORD PTR [rip+0x177f33]        # 0x18040bd34
00293E01: vcvttss2si r8d,xmm1
00293E05: mov       r9d,eax
00293E08: mov       rdx,rdi
00293E0B: mov       rcx,rsi
00293E0E: vmovss    DWORD PTR [rbx+0x4],xmm1
00293E13: mov       rbx,QWORD PTR [rsp+0x30]
00293E18: mov       rsi,QWORD PTR [rsp+0x38]
00293E1D: add       rsp,0x20
00293E21: pop       rdi
00293E22: rex.W     jmp QWORD PTR [rip+0x1dee4f]        # 0x180472c78 ; PDPerfPlugin.dll!GetJitterOffset
