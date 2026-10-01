; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xFA390..0xFA3D3; GetJitterPhaseCount
000FA390: mov       QWORD PTR [rsp+0x8],rbx
000FA395: push      rdi
000FA396: sub       rsp,0x20
000FA39A: mov       edi,ecx
000FA39C: mov       rcx,QWORD PTR [rip+0x112563d]        # 0x18121f9e0
000FA3A3: mov       edx,edi
000FA3A5: mov       rax,QWORD PTR [rcx]
000FA3A8: call      QWORD PTR [rax+0x58]
000FA3AB: mov       rcx,QWORD PTR [rip+0x112562e]        # 0x18121f9e0
000FA3B2: mov       ebx,eax
000FA3B4: mov       rdx,QWORD PTR [rcx]
000FA3B7: mov       r8,QWORD PTR [rdx+0x48]
000FA3BB: mov       edx,edi
000FA3BD: call      r8
000FA3C0: mov       ecx,eax
000FA3C2: mov       edx,ebx
000FA3C4: mov       rbx,QWORD PTR [rsp+0x30]
000FA3C9: add       rsp,0x20
000FA3CD: pop       rdi
000FA3CE: jmp       0x1800fa2c0
