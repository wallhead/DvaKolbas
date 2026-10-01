; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xE77F0..0xE7A95; unnamed
000E77F0: rex       push rbp
000E77F2: push      rbx
000E77F3: push      rsi
000E77F4: push      r14
000E77F6: push      r15
000E77F8: lea       rbp,[rsp-0x20]
000E77FD: sub       rsp,0x120
000E7804: mov       rax,QWORD PTR [rip+0x10e31b5]        # 0x1811ca9c0
000E780B: xor       rax,rsp
000E780E: mov       QWORD PTR [rbp-0x10],rax
000E7812: mov       rbx,rcx
000E7815: mov       r15d,r9d
000E7818: mov       rcx,QWORD PTR [rcx+0x8]
000E781C: mov       r14d,r8d
000E781F: mov       esi,edx
000E7821: test      rcx,rcx
000E7824: jne       0x1800e7830
000E7826: mov       eax,0x887a0005
000E782B: jmp       0x1800e7a7a
000E7830: call      0x1800bbcd0
000E7835: test      eax,eax
000E7837: js        0x1800e7a7a
000E783D: mov       QWORD PTR [rsp+0x118],rdi
000E7845: mov       QWORD PTR [rsp+0x110],r12
000E784D: test      esi,esi
000E784F: je        0x1800e7854
000E7851: mov       DWORD PTR [rbx+0x58],esi
000E7854: mov       edi,DWORD PTR [rbp+0x70]
000E7857: mov       BYTE PTR [rbx+0x5c],0x0
000E785B: cmp       edi,0xa
000E785E: jne       0x1800e7869
000E7860: mov       BYTE PTR [rbx+0x5c],0x1
000E7864: mov       edi,0x18
000E7869: mov       rcx,QWORD PTR [rbx+0x28]
000E786D: xor       r12d,r12d
000E7870: test      rcx,rcx
000E7873: je        0x1800e787f
000E7875: mov       rax,QWORD PTR [rcx]
000E7878: call      QWORD PTR [rax+0x10]
000E787B: mov       QWORD PTR [rbx+0x28],r12
000E787F: mov       rcx,QWORD PTR [rbx+0x30]
000E7883: test      rcx,rcx
000E7886: je        0x1800e7892
000E7888: mov       rax,QWORD PTR [rcx]
000E788B: call      QWORD PTR [rax+0x10]
000E788E: mov       QWORD PTR [rbx+0x30],r12
000E7892: mov       rcx,QWORD PTR [rbx+0x38]
000E7896: test      rcx,rcx
000E7899: je        0x1800e78a5
000E789B: mov       rax,QWORD PTR [rcx]
000E789E: call      QWORD PTR [rax+0x10]
000E78A1: mov       QWORD PTR [rbx+0x38],r12
000E78A5: mov       rcx,QWORD PTR [rbx+0x8]
000E78A9: mov       r9d,r15d
000E78AC: mov       r8d,r14d
000E78AF: mov       edx,esi
000E78B1: mov       rax,QWORD PTR [rcx]
000E78B4: mov       r10,QWORD PTR [rax+0x68]
000E78B8: mov       eax,DWORD PTR [rbp+0x78]
000E78BB: mov       DWORD PTR [rsp+0x28],eax
000E78BF: mov       DWORD PTR [rsp+0x20],edi
000E78C3: call      r10
000E78C6: test      eax,eax
000E78C8: js        0x1800e7a6a
000E78CE: mov       rcx,QWORD PTR [rbx+0x8]
000E78D2: mov       edx,eax
000E78D4: call      0x1800bbd00
000E78D9: mov       r14d,eax
000E78DC: test      eax,eax
000E78DE: js        0x1800e7a67
000E78E4: cmp       BYTE PTR [rbx+0x5c],r12b
000E78E8: je        0x1800e7a67
000E78EE: lea       rcx,[rip+0x10c10db]        # 0x1811a89d0 ; 'DXGISwapChainProxy::ResizeBuffers: Creating Fake HDR Buffer DXGI_FORMAT_R16G16B16A16_FLOAT'
000E78F5: mov       QWORD PTR [rsp+0x108],r13
000E78FD: call      0x1800fbb40
000E7902: mov       r10,QWORD PTR [rbx+0x8]
000E7906: lea       rcx,[rsp+0x40]
000E790B: mov       QWORD PTR [rsp+0x40],r12
000E7910: call      0x180008420
000E7915: mov       r9,rax
000E7918: lea       r13,[rip+0x10b7851]        # 0x18119f170
000E791F: mov       rax,QWORD PTR [r10]
000E7922: mov       r8,r13
000E7925: xor       edx,edx
000E7927: mov       rcx,r10
000E792A: call      QWORD PTR [rax+0x48]
000E792D: mov       rcx,QWORD PTR [rsp+0x40]
000E7932: lea       rdx,[rbp-0x48]
000E7936: mov       rax,QWORD PTR [rcx]
000E7939: call      QWORD PTR [rax+0x50]
000E793C: mov       rcx,QWORD PTR [rsp+0x40]
000E7941: mov       rax,QWORD PTR [rcx]
000E7944: call      QWORD PTR [rax+0x10]
000E7947: mov       esi,r12d
000E794A: mov       DWORD PTR [rbp-0x28],0xa
000E7951: cmp       DWORD PTR [rbx+0x58],r12d
000E7955: jle       0x1800e7a4e
000E795B: lea       rcx,[rsp+0x48]
000E7960: call      0x180008420
000E7965: mov       r15,rax
000E7968: lea       rdi,[rbx+0x40]
000E796C: nop       DWORD PTR [rax+0x0]
000E7970: mov       r8,QWORD PTR [rdi]
000E7973: test      r8,r8
000E7976: je        0x1800e7987
000E7978: mov       rcx,QWORD PTR [r8]
000E797B: mov       rdx,QWORD PTR [rcx+0x10]
000E797F: mov       rcx,r8
000E7982: call      rdx
000E7984: mov       QWORD PTR [rdi],r12
000E7987: movups    xmm0,XMMWORD PTR [rbp-0x48]
000E798B: movzx     edx,BYTE PTR [rbx+0x5d]
000E798F: mov       DWORD PTR [rbp-0x60],0x1
000E7996: movaps    XMMWORD PTR [rsp+0x60],xmm0
000E799B: movups    xmm0,XMMWORD PTR [rbp-0x28]
000E799F: movups    xmm1,XMMWORD PTR [rbp-0x38]
000E79A3: movaps    XMMWORD PTR [rbp-0x80],xmm0
000E79A7: xorps     xmm0,xmm0
000E79AA: movaps    XMMWORD PTR [rsp+0x70],xmm1
000E79AF: movsd     xmm1,QWORD PTR [rbp-0x18]
000E79B4: movsd     QWORD PTR [rbp-0x70],xmm1
000E79B9: movdqu    XMMWORD PTR [rbp-0x5c],xmm0
000E79BE: test      dl,dl
000E79C0: je        0x1800e79cb
000E79C2: mov       eax,DWORD PTR [rbp-0x18]
000E79C5: or        eax,0x20
000E79C8: mov       DWORD PTR [rbp-0x70],eax
000E79CB: mov       rax,QWORD PTR [rip+0x112ea0e]        # 0x1812163e0
000E79D2: lea       r9,[rsp+0x60]
000E79D7: test      dl,dl
000E79D9: mov       QWORD PTR [rsp+0x38],r15
000E79DE: mov       QWORD PTR [rsp+0x48],r12
000E79E3: lea       rdx,[rbp-0x60]
000E79E7: mov       r8d,r12d
000E79EA: mov       QWORD PTR [rsp+0x30],r13
000E79EF: mov       rcx,QWORD PTR [rax+0x98]
000E79F6: setne     r8b
000E79FA: mov       QWORD PTR [rsp+0x28],r12
000E79FF: mov       DWORD PTR [rsp+0x20],0x4
000E7A07: mov       rax,QWORD PTR [rcx]
000E7A0A: call      QWORD PTR [rax+0xd8]
000E7A10: test      eax,eax
000E7A12: jns       0x1800e7a22
000E7A14: mov       edx,eax
000E7A16: lea       rcx,[rip+0x10c095b]        # 0x1811a8378 ; 'Create ID3D12Resource failed: %d '
000E7A1D: call      0x1800fbb40
000E7A22: mov       r8,QWORD PTR [rsp+0x48]
000E7A27: lea       r9d,[rsi+0xa]
000E7A2B: mov       rcx,QWORD PTR [rip+0x112e9ae]        # 0x1812163e0
000E7A32: lea       rdx,[rsp+0x50]
000E7A37: mov       QWORD PTR [rdi],r8
000E7A3A: call      0x180073170 ; '@SUVWAVH'
000E7A3F: inc       esi
000E7A41: add       rdi,0x8
000E7A45: cmp       esi,DWORD PTR [rbx+0x58]
000E7A48: jl        0x1800e7970
000E7A4E: mov       rax,QWORD PTR [rbx]
000E7A51: mov       edx,0xc
000E7A56: mov       rcx,rbx
000E7A59: call      QWORD PTR [rax+0x130]
000E7A5F: mov       r13,QWORD PTR [rsp+0x108]
000E7A67: mov       eax,r14d
000E7A6A: mov       rdi,QWORD PTR [rsp+0x118]
000E7A72: mov       r12,QWORD PTR [rsp+0x110]
000E7A7A: mov       rcx,QWORD PTR [rbp-0x10]
000E7A7E: xor       rcx,rsp
000E7A81: call      0x18010c270
000E7A86: add       rsp,0x120
000E7A8D: pop       r15
000E7A8F: pop       r14
000E7A91: pop       rsi
000E7A92: pop       rbx
000E7A93: pop       rbp
000E7A94: ret       
