; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xEF880..0xEF94B; unnamed
000EF880: mov       QWORD PTR [rsp+0x18],rbx
000EF885: mov       QWORD PTR [rsp+0x20],rsi
000EF88A: push      rdi
000EF88B: sub       rsp,0x50
000EF88F: mov       rax,QWORD PTR [rip+0x10db12a]        # 0x1811ca9c0
000EF896: xor       rax,rsp
000EF899: mov       QWORD PTR [rsp+0x40],rax
000EF89E: mov       rdi,QWORD PTR [rip+0x1126b53]        # 0x1812163f8
000EF8A5: mov       rsi,rdx
000EF8A8: mov       rbx,rcx
000EF8AB: cmp       BYTE PTR [rdi+0xca],0x0
000EF8B2: jne       0x1800ef8bc
000EF8B4: mov       rcx,rdi
000EF8B7: call      0x1800cc8c0
000EF8BC: mov       rax,QWORD PTR [rdi+0x18]
000EF8C0: mov       rdx,rbx
000EF8C3: mov       rcx,rsi
000EF8C6: call      rax
000EF8C8: xorps     xmm0,xmm0
000EF8CB: mov       edi,eax
000EF8CD: movups    XMMWORD PTR [rsp+0x20],xmm0
000EF8D2: movups    XMMWORD PTR [rsp+0x30],xmm0
000EF8D7: test      eax,eax
000EF8D9: jne       0x1800ef916
000EF8DB: mov       r9d,DWORD PTR [rbx+0x108]
000EF8E2: cmp       r9d,0x4
000EF8E6: ja        0x1800ef919
000EF8E8: test      r9d,r9d
000EF8EB: je        0x1800ef919
000EF8ED: lea       rcx,[rsp+0x20]
000EF8F2: mov       r8d,r9d
000EF8F5: lea       rdx,[rbx+0x48]
000EF8F9: nop       DWORD PTR [rax+0x0]
000EF900: mov       rax,QWORD PTR [rdx]
000EF903: lea       rdx,[rdx+0x30]
000EF907: mov       QWORD PTR [rcx],rax
000EF90A: lea       rcx,[rcx+0x8]
000EF90E: sub       r8,0x1
000EF912: jne       0x1800ef900
000EF914: jmp       0x1800ef919
000EF916: xor       r9d,r9d
000EF919: mov       ecx,DWORD PTR [rbx+0x130]
000EF91F: lea       r8,[rsp+0x20]
000EF924: mov       edx,r9d
000EF927: call      0x1800b98b0
000EF92C: mov       eax,edi
000EF92E: mov       rcx,QWORD PTR [rsp+0x40]
000EF933: xor       rcx,rsp
000EF936: call      0x18010c270
000EF93B: mov       rbx,QWORD PTR [rsp+0x70]
000EF940: mov       rsi,QWORD PTR [rsp+0x78]
000EF945: add       rsp,0x50
000EF949: pop       rdi
000EF94A: ret       
