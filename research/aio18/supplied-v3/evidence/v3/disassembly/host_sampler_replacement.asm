; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x2A8570..0x2A8983; unnamed
002A8570: mov       QWORD PTR [rsp+0x10],rbx
002A8575: mov       QWORD PTR [rsp+0x18],rsi
002A857A: mov       QWORD PTR [rsp+0x20],rdi
002A857F: push      rbp
002A8580: push      r14
002A8582: push      r15
002A8584: lea       rbp,[rsp-0x47]
002A8589: sub       rsp,0x100
002A8590: mov       rdi,QWORD PTR [rip+0xbd8849]        # 0x180e80de0
002A8597: mov       r15,r9
002A859A: vmovss    xmm0,DWORD PTR [rip+0xbd8af2]        # 0x180e81094
002A85A2: add       rdi,0x2ec
002A85A9: mov       ebx,r8d
002A85AC: mov       rsi,rcx
002A85AF: vucomiss  xmm0,DWORD PTR [rdi]
002A85B3: je        0x1802a864f
002A85B9: call      0x180222050
002A85BE: lea       rcx,[rip+0x161653]        # 0x180409c18
002A85C5: mov       DWORD PTR [rbp-0x71],0x3ba
002A85CC: mov       QWORD PTR [rbp-0x79],rcx
002A85D0: lea       r9,[rsp+0x30]
002A85D5: mov       ecx,DWORD PTR [rbp-0x6d]
002A85D8: lea       rdx,[rbp-0x59]
002A85DC: mov       DWORD PTR [rbp-0x6d],ecx
002A85DF: lea       rcx,[rip+0x161aea]        # 0x18040a0d0
002A85E6: vmovups   xmm0,XMMWORD PTR [rbp-0x79]
002A85EB: mov       QWORD PTR [rbp-0x69],rcx
002A85EF: lea       rcx,[rip+0x161a92]        # 0x18040a088
002A85F6: vmovsd    xmm1,QWORD PTR [rbp-0x69]
002A85FB: mov       QWORD PTR [rsp+0x30],rcx
002A8600: mov       rcx,rax
002A8603: mov       QWORD PTR [rsp+0x38],0x38
002A860C: vmovups   XMMWORD PTR [rbp-0x59],xmm0
002A8611: vmovsd    QWORD PTR [rbp-0x49],xmm1
002A8616: mov       QWORD PTR [rsp+0x28],rdi
002A861B: call      0x1801a6310
002A8620: lea       rcx,[rip+0xbd9259]        # 0x180e81880
002A8627: call      0x1801a4830
002A862C: lea       rcx,[rip+0xbd928d]        # 0x180e818c0
002A8633: call      0x1801a4780
002A8638: mov       rax,QWORD PTR [rip+0xbd87a1]        # 0x180e80de0
002A863F: vmovss    xmm0,DWORD PTR [rax+0x2ec]
002A8647: vmovss    DWORD PTR [rip+0xbd8a45],xmm0        # 0x180e81094
002A864F: lea       r8,[rbx*8+0x0]
002A8657: mov       rdx,r15
002A865A: mov       rcx,rsi
002A865D: mov       r14,rbx
002A8660: call      0x18023a826
002A8665: test      ebx,ebx
002A8667: je        0x1802a8956
002A866D: mov       QWORD PTR [rsp+0x120],r12
002A8675: movabs    r15,0x100000001b3
002A867F: vmovaps   XMMWORD PTR [rsp+0xf0],xmm6
002A8688: vxorps    xmm6,xmm6,xmm6
002A868C: movabs    r12,0xcbf29ce484222325
002A8696: data16    nop WORD PTR [rax+rax*1+0x0]
002A86A0: mov       r8,QWORD PTR [rsi]
002A86A3: mov       QWORD PTR [rsp+0x30],r8
002A86A8: test      r8,r8
002A86AB: je        0x1802a8937
002A86B1: mov       rax,r8
002A86B4: shr       rax,0x8
002A86B8: movzx     r9d,al
002A86BC: movzx     eax,r8b
002A86C0: xor       rax,r12
002A86C3: imul      rax,r15
002A86C7: xor       r9,rax
002A86CA: mov       rax,r8
002A86CD: imul      r9,r15
002A86D1: shr       rax,0x10
002A86D5: movzx     ecx,al
002A86D8: mov       rax,r8
002A86DB: xor       r9,rcx
002A86DE: shr       rax,0x18
002A86E2: imul      r9,r15
002A86E6: movzx     ecx,al
002A86E9: mov       rax,r8
002A86EC: xor       r9,rcx
002A86EF: shr       rax,0x20
002A86F3: imul      r9,r15
002A86F7: movzx     ecx,al
002A86FA: mov       rax,r8
002A86FD: xor       r9,rcx
002A8700: shr       rax,0x28
002A8704: imul      r9,r15
002A8708: movzx     ecx,al
002A870B: mov       rax,r8
002A870E: xor       r9,rcx
002A8711: shr       rax,0x30
002A8715: imul      r9,r15
002A8719: movzx     ecx,al
002A871C: mov       rax,r8
002A871F: xor       r9,rcx
002A8722: shr       rax,0x38
002A8726: imul      r9,r15
002A872A: xor       r9,rax
002A872D: mov       rax,QWORD PTR [rip+0xbd9164]        # 0x180e81898
002A8734: imul      r9,r15
002A8738: and       r9,QWORD PTR [rip+0xbd9171]        # 0x180e818b0
002A873F: add       r9,r9
002A8742: mov       rdx,QWORD PTR [rax+r9*8+0x8]
002A8747: cmp       rdx,QWORD PTR [rip+0xbd913a]        # 0x180e81888
002A874E: je        0x1802a8771
002A8750: mov       rax,QWORD PTR [rax+r9*8]
002A8754: cmp       r8,QWORD PTR [rdx+0x10]
002A8758: je        0x1802a8773
002A875A: nop       WORD PTR [rax+rax*1+0x0]
002A8760: cmp       rdx,rax
002A8763: je        0x1802a8771
002A8765: mov       rdx,QWORD PTR [rdx+0x8]
002A8769: cmp       r8,QWORD PTR [rdx+0x10]
002A876D: jne       0x1802a8760
002A876F: jmp       0x1802a8773
002A8771: xor       edx,edx
002A8773: mov       rax,QWORD PTR [rip+0xbd910e]        # 0x180e81888
002A877A: test      rdx,rdx
002A877D: cmovne    rax,rdx
002A8781: cmp       rax,QWORD PTR [rip+0xbd9100]        # 0x180e81888
002A8788: jne       0x1802a8937
002A878E: call      0x180152f00
002A8793: cmp       BYTE PTR [rax+0x11],0x0
002A8797: jne       0x1802a8937
002A879D: cmp       BYTE PTR [rax+0x12],0x0
002A87A1: jne       0x1802a8937
002A87A7: call      0x1801ac3d0
002A87AC: mov       rbx,rax
002A87AF: test      rax,rax
002A87B2: je        0x1802a87e2
002A87B4: lea       rdx,[rip+0x13cf7d]        # 0x1803e5738
002A87BB: mov       rcx,rax
002A87BE: call      0x1801ac480
002A87C3: test      al,al
002A87C5: jne       0x1802a8937
002A87CB: lea       rdx,[rip+0x13cf46]        # 0x1803e5718
002A87D2: mov       rcx,rbx
002A87D5: call      0x1801ac480
002A87DA: test      al,al
002A87DC: jne       0x1802a8937
002A87E2: movzx     eax,BYTE PTR [rsp+0x31]
002A87E7: mov       r8,QWORD PTR [rsp+0x30]
002A87EC: mov       r9,QWORD PTR [rip+0xbd90d5]        # 0x180e818c8
002A87F3: movzx     edx,r8b
002A87F7: xor       rdx,r12
002A87FA: imul      rdx,r15
002A87FE: xor       rdx,rax
002A8801: movzx     eax,BYTE PTR [rsp+0x32]
002A8806: imul      rdx,r15
002A880A: xor       rdx,rax
002A880D: movzx     eax,BYTE PTR [rsp+0x33]
002A8812: imul      rdx,r15
002A8816: xor       rdx,rax
002A8819: movzx     eax,BYTE PTR [rsp+0x34]
002A881E: imul      rdx,r15
002A8822: xor       rdx,rax
002A8825: movzx     eax,BYTE PTR [rsp+0x35]
002A882A: imul      rdx,r15
002A882E: xor       rdx,rax
002A8831: movzx     eax,BYTE PTR [rsp+0x36]
002A8836: imul      rdx,r15
002A883A: xor       rdx,rax
002A883D: movzx     eax,BYTE PTR [rsp+0x37]
002A8842: imul      rdx,r15
002A8846: xor       rdx,rax
002A8849: mov       rax,QWORD PTR [rip+0xbd9088]        # 0x180e818d8
002A8850: imul      rdx,r15
002A8854: and       rdx,QWORD PTR [rip+0xbd9095]        # 0x180e818f0
002A885B: add       rdx,rdx
002A885E: mov       rcx,QWORD PTR [rax+rdx*8+0x8]
002A8863: cmp       rcx,r9
002A8866: je        0x1802a8883
002A8868: mov       rax,QWORD PTR [rax+rdx*8]
002A886C: cmp       r8,QWORD PTR [rcx+0x10]
002A8870: je        0x1802a8885
002A8872: cmp       rcx,rax
002A8875: je        0x1802a8883
002A8877: mov       rcx,QWORD PTR [rcx+0x8]
002A887B: cmp       r8,QWORD PTR [rcx+0x10]
002A887F: jne       0x1802a8872
002A8881: jmp       0x1802a8885
002A8883: xor       ecx,ecx
002A8885: test      rcx,rcx
002A8888: mov       rax,r9
002A888B: cmovne    rax,rcx
002A888F: cmp       rax,r9
002A8892: jne       0x1802a891f
002A8898: mov       rax,QWORD PTR [r8]
002A889B: lea       rdx,[rbp-0x39]
002A889F: mov       rcx,r8
002A88A2: call      QWORD PTR [rax+0x38]
002A88A5: vmovss    xmm0,DWORD PTR [rbp-0x29]
002A88AA: vucomiss  xmm0,xmm6
002A88AE: jne       0x1802a8973
002A88B4: cmp       DWORD PTR [rbp-0x25],0x1
002A88B8: jbe       0x1802a8973
002A88BE: mov       rax,QWORD PTR [rip+0xbd851b]        # 0x180e80de0
002A88C5: lea       r8,[rsp+0x30]
002A88CA: vmovss    xmm0,DWORD PTR [rip+0xbd87c2]        # 0x180e81094
002A88D2: vmovss    DWORD PTR [rbp-0x29],xmm0
002A88D7: lea       rdx,[rbp-0x1]
002A88DB: mov       rdi,QWORD PTR [rax+0x1678]
002A88E2: mov       rax,QWORD PTR [rdi]
002A88E5: mov       rbx,QWORD PTR [rax+0xb8]
002A88EC: call      0x1801a4910
002A88F1: lea       rdx,[rbp-0x39]
002A88F5: mov       rcx,rdi
002A88F8: mov       r8,QWORD PTR [rax]
002A88FB: add       r8,0x18
002A88FF: call      rbx
002A8901: lea       r8,[rsp+0x30]
002A8906: lea       rdx,[rbp+0xf]
002A890A: call      0x1801a4910
002A890F: lea       rdx,[rbp+0x1f]
002A8913: mov       r8,QWORD PTR [rax]
002A8916: add       r8,0x18
002A891A: call      0x1801a4c10
002A891F: lea       r8,[rsp+0x30]
002A8924: lea       rdx,[rbp-0x79]
002A8928: call      0x1801a4910
002A892D: mov       rcx,QWORD PTR [rax]
002A8930: mov       rax,QWORD PTR [rcx+0x18]
002A8934: mov       QWORD PTR [rsi],rax
002A8937: add       rsi,0x8
002A893B: sub       r14,0x1
002A893F: jne       0x1802a86a0
002A8945: vmovaps   xmm6,XMMWORD PTR [rsp+0xf0]
002A894E: mov       r12,QWORD PTR [rsp+0x120]
002A8956: lea       r11,[rsp+0x100]
002A895E: mov       rbx,QWORD PTR [r11+0x28]
002A8962: mov       rsi,QWORD PTR [r11+0x30]
002A8966: mov       rdi,QWORD PTR [r11+0x38]
002A896A: mov       rsp,r11
002A896D: pop       r15
002A896F: pop       r14
002A8971: pop       rbp
002A8972: ret       
002A8973: lea       r8,[rsp+0x30]
002A8978: lea       rdx,[rbp-0x59]
002A897C: call      0x1801a4c10
002A8981: jmp       0x1802a8937
