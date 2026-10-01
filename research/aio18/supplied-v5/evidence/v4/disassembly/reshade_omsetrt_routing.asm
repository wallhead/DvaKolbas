; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x2A97E0..0x2A99DA; unnamed
002A97E0: mov       QWORD PTR [rsp+0x10],rbx
002A97E5: mov       QWORD PTR [rsp+0x18],rbp
002A97EA: push      rsi
002A97EB: push      rdi
002A97EC: push      r14
002A97EE: sub       rsp,0x30
002A97F2: mov       r14,r9
002A97F5: mov       rbx,r8
002A97F8: mov       edi,edx
002A97FA: mov       rsi,rcx
002A97FD: movzx     eax,BYTE PTR [rip+0xbd779a]        # 0x180e80f9e
002A9804: cmp       rcx,QWORD PTR [rip+0xbd7795]        # 0x180e80fa0
002A980B: jne       0x1802a9835
002A980D: test      al,al
002A980F: jne       0x1802a9835
002A9811: mov       rax,QWORD PTR [rip+0xbd75c8]        # 0x180e80de0
002A9818: cmp       QWORD PTR [rax+0x1690],0x0
002A9820: je        0x1802a982b
002A9822: cmp       BYTE PTR [rip+0xbd7868],0x0        # 0x180e81091
002A9829: jne       0x1802a984a
002A982B: call      0x1802a9b30
002A9830: jmp       0x1802a99c7
002A9835: cmp       BYTE PTR [rip+0xbd7855],0x0        # 0x180e81091
002A983C: je        0x1802a999d
002A9842: test      al,al
002A9844: jne       0x1802a999d
002A984A: cmp       QWORD PTR [rip+0xbd76ce],0x0        # 0x180e80f20
002A9852: je        0x1802a999d
002A9858: cmp       edi,0x1
002A985B: jne       0x1802a999d
002A9861: test      rbx,rbx
002A9864: je        0x1802a999d
002A986A: mov       rcx,QWORD PTR [r8]
002A986D: test      rcx,rcx
002A9870: je        0x1802a999d
002A9876: mov       QWORD PTR [rsp+0x50],0x0
002A987F: mov       rax,QWORD PTR [rcx]
002A9882: lea       rdx,[rsp+0x50]
002A9887: call      QWORD PTR [rax+0x38]
002A988A: xor       r8b,r8b
002A988D: mov       r10,QWORD PTR [rip+0xbd768c]        # 0x180e80f20
002A9894: lea       rbp,[r10+0x40]
002A9898: mov       rdx,rbp
002A989B: lea       r9,[rbp+0x108]
002A98A2: mov       rcx,QWORD PTR [rsp+0x50]
002A98A7: cmp       rbp,r9
002A98AA: je        0x1802a9988
002A98B0: cmp       rcx,QWORD PTR [rdx]
002A98B3: sete      al
002A98B6: or        r8b,al
002A98B9: add       rdx,0x58
002A98BD: cmp       rdx,r9
002A98C0: jne       0x1802a98b0
002A98C2: test      r8b,r8b
002A98C5: je        0x1802a9988
002A98CB: mov       rax,QWORD PTR [r10]
002A98CE: mov       rcx,r10
002A98D1: call      QWORD PTR [rax+0x120]
002A98D7: mov       eax,eax
002A98D9: imul      rcx,rax,0x58
002A98DD: add       rcx,rbp
002A98E0: call      0x18016c140
002A98E5: mov       QWORD PTR [rsp+0x20],rax
002A98EA: mov       rcx,QWORD PTR [rip+0xbd74ef]        # 0x180e80de0
002A98F1: cmp       BYTE PTR [rip+0xbd76a7],0x0        # 0x180e80f9f
002A98F8: je        0x1802a991b
002A98FA: cmp       BYTE PTR [rcx+0x3d2],0x0
002A9901: je        0x1802a993c
002A9903: call      0x180294110
002A9908: test      al,al
002A990A: je        0x1802a993c
002A990C: add       rcx,0x988
002A9913: cmp       QWORD PTR [rcx],0x0
002A9917: je        0x1802a993c
002A9919: jmp       0x1802a9932
002A991B: call      0x180293f20
002A9920: test      al,al
002A9922: je        0x1802a993c
002A9924: mov       rcx,QWORD PTR [rip+0xbd74b5]        # 0x180e80de0
002A992B: add       rcx,0x9e0
002A9932: call      0x18016c140
002A9937: mov       QWORD PTR [rsp+0x20],rax
002A993C: mov       edx,DWORD PTR [rip+0x1c9b72]        # 0x1804734b4
002A9942: mov       rax,QWORD PTR gs:0x58
002A994B: mov       ecx,0x3f28
002A9950: mov       rax,QWORD PTR [rax+rdx*8]
002A9954: mov       r10,QWORD PTR [rcx+rax*1]
002A9958: mov       r9,r14
002A995B: lea       r8,[rsp+0x20]
002A9960: mov       edx,0x1
002A9965: mov       rcx,rsi
002A9968: call      r10
002A996B: nop       
002A996C: mov       rcx,QWORD PTR [rsp+0x50]
002A9971: test      rcx,rcx
002A9974: je        0x1802a9986
002A9976: mov       QWORD PTR [rsp+0x50],0x0
002A997F: mov       rax,QWORD PTR [rcx]
002A9982: call      QWORD PTR [rax+0x10]
002A9985: nop       
002A9986: jmp       0x1802a99c7
002A9988: test      rcx,rcx
002A998B: je        0x1802a999d
002A998D: mov       QWORD PTR [rsp+0x50],0x0
002A9996: mov       rax,QWORD PTR [rcx]
002A9999: call      QWORD PTR [rax+0x10]
002A999C: nop       
002A999D: mov       edx,DWORD PTR [rip+0x1c9b11]        # 0x1804734b4
002A99A3: mov       rax,QWORD PTR gs:0x58
002A99AC: mov       ecx,0x3f28
002A99B1: mov       rax,QWORD PTR [rax+rdx*8]
002A99B5: mov       r10,QWORD PTR [rcx+rax*1]
002A99B9: mov       r9,r14
002A99BC: mov       r8,rbx
002A99BF: mov       edx,edi
002A99C1: mov       rcx,rsi
002A99C4: call      r10
002A99C7: mov       rbx,QWORD PTR [rsp+0x58]
002A99CC: mov       rbp,QWORD PTR [rsp+0x60]
002A99D1: add       rsp,0x30
002A99D5: pop       r14
002A99D7: pop       rdi
002A99D8: pop       rsi
002A99D9: ret       
