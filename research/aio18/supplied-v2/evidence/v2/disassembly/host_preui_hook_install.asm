; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x1A16E3..0x1A17D9; unnamed
001A16E5: nop       
001A16E6: call      0x180135920
001A16EB: movzx     ebx,BYTE PTR [rax+0x118]
001A16F2: mov       QWORD PTR [rsp+0x40],0x1384b
001A16FB: mov       QWORD PTR [rsp+0x48],0x140a4
001A1704: lea       rcx,[rsp+0x40]
001A1709: call      0x1801365e0
001A170E: mov       esi,0x17a
001A1713: mov       edi,esi
001A1715: mov       r14d,0x16f
001A171B: cmp       bl,0x1
001A171E: cmovne    edi,r14d
001A1722: add       rdi,rax
001A1725: mov       dl,0x1
001A1727: mov       ecx,0xe
001A172C: call      0x1801af520
001A1731: call      0x1801aacc0
001A1736: movsxd    rbx,DWORD PTR [rdi+0x1]
001A173A: add       rbx,0x5
001A173E: add       rbx,rdi
001A1741: mov       r9b,0xe8
001A1744: lea       r8,[rip+0xffffffffffffdcd5]        # 0x18019f420
001A174B: mov       rdx,rdi
001A174E: mov       rcx,rax
001A1751: call      0x1801ab720
001A1756: mov       QWORD PTR [rip+0x2dbc53],rbx        # 0x18047d3b0
001A175D: call      0x180135920
001A1762: movzx     ebx,BYTE PTR [rax+0x118]
001A1769: mov       QWORD PTR [rsp+0x40],0x1384b
001A1772: mov       QWORD PTR [rsp+0x48],0x140a4
001A177B: lea       rcx,[rsp+0x40]
001A1780: call      0x1801365e0
001A1785: cmp       bl,0x1
001A1788: cmovne    esi,r14d
001A178C: add       rax,rsi
001A178F: mov       QWORD PTR [rbp+0x1a0],rax
001A1796: call      0x180222050
001A179B: mov       QWORD PTR [rsp+0x50],r15
001A17A0: mov       DWORD PTR [rsp+0x58],0x9ef
001A17A8: mov       ecx,DWORD PTR [rsp+0x7c]
001A17AC: mov       DWORD PTR [rsp+0x5c],ecx
001A17B0: mov       QWORD PTR [rsp+0x60],r12
001A17B5: lea       rcx,[rip+0x269bdc]        # 0x18040b398 ; 'Main_DrawWorld_PreUI {}'
001A17BC: mov       QWORD PTR [rsp+0x40],rcx
001A17C1: mov       QWORD PTR [rsp+0x48],0x17
001A17CA: vmovups   xmm0,XMMWORD PTR [rsp+0x50]
001A17D0: vmovups   XMMWORD PTR [rsp+0x70],xmm0
001A17D6: vmovsd    xmm1,QWORD PTR [rsp+0x60]
