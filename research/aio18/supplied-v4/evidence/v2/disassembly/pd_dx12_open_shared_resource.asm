; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0x6E850..0x6E8AD; unnamed
0006E850: sub       rsp,0x28
0006E854: mov       r10,rcx
0006E857: test      rcx,rcx
0006E85A: je        0x18006e89c
0006E85C: test      rdx,rdx
0006E85F: je        0x18006e89c
0006E861: lea       rcx,[rsp+0x30]
0006E866: mov       QWORD PTR [rsp+0x30],0x0
0006E86F: call      0x180008420
0006E874: mov       r9,rax
0006E877: lea       r8,[rip+0x11308f2]        # 0x18119f170
0006E87E: mov       rax,QWORD PTR [r10]
0006E881: mov       rcx,r10
0006E884: call      QWORD PTR [rax+0x100]
0006E88A: test      eax,eax
0006E88C: jns       0x18006e8a3
0006E88E: mov       edx,eax
0006E890: lea       rcx,[rip+0x112ede1]        # 0x18119d678 ; 'OpenSharedHandle failed: 0x%08x'
0006E897: call      0x1800fbb40
0006E89C: xor       eax,eax
0006E89E: add       rsp,0x28
0006E8A2: ret       
0006E8A3: mov       rax,QWORD PTR [rsp+0x30]
0006E8A8: add       rsp,0x28
0006E8AC: ret       
