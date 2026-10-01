; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xEFAD0..0xEFC7A; unnamed
000EFAD0: rex       push rbx
000EFAD2: push      rbp
000EFAD3: push      rsi
000EFAD4: push      rdi
000EFAD5: push      r14
000EFAD7: sub       rsp,0xc0
000EFADE: mov       rax,QWORD PTR [rip+0x10daedb]        # 0x1811ca9c0
000EFAE5: xor       rax,rsp
000EFAE8: mov       QWORD PTR [rsp+0xb0],rax
000EFAF0: mov       r14,QWORD PTR [rsp+0x110]
000EFAF8: mov       rbx,r9
000EFAFB: mov       rbp,r8
000EFAFE: mov       BYTE PTR [rcx+0x1c8],0x0
000EFB05: mov       rsi,rdx
000EFB08: mov       rdi,rcx
000EFB0B: call      0x1800cc790
000EFB10: mov       r8d,DWORD PTR [rdi+0x8]
000EFB14: mov       rcx,rax
000EFB17: mov       edx,DWORD PTR [rdi+0x148]
000EFB1D: call      0x1800ccc80
000EFB22: lea       rcx,[rip+0x10ba1bf]        # 0x1811a9ce8 ; 'Create FSR3 Frame Interpolation SwapChain'
000EFB29: call      0x1800fbb40
000EFB2E: mov       eax,DWORD PTR [rbx+0x40]
000EFB31: bt        eax,0xd
000EFB35: jae       0x1800efb3f
000EFB37: add       eax,0xffffe000
000EFB3C: mov       DWORD PTR [rbx+0x40],eax
000EFB3F: bt        eax,0xe
000EFB43: jae       0x1800efb4d
000EFB45: add       eax,0xffffc000
000EFB4A: mov       DWORD PTR [rbx+0x40],eax
000EFB4D: xor       ecx,ecx
000EFB4F: mov       QWORD PTR [rsp+0x48],0x30005
000EFB58: lea       rax,[rsp+0x20]
000EFB5D: mov       QWORD PTR [rsp+0x50],rcx
000EFB62: mov       QWORD PTR [rsp+0x58],rax
000EFB67: lea       rdx,[rsp+0x30]
000EFB6C: mov       eax,DWORD PTR [rbx]
000EFB6E: mov       DWORD PTR [rdi+0x134],eax
000EFB74: mov       eax,DWORD PTR [rbx+0x4]
000EFB77: mov       QWORD PTR [rsp+0x38],rcx
000EFB7C: mov       QWORD PTR [rsp+0x20],rcx
000EFB81: lea       rcx,[rsp+0x48]
000EFB86: mov       DWORD PTR [rdi+0x138],eax
000EFB8C: mov       QWORD PTR [rsp+0x60],rbx
000EFB91: mov       QWORD PTR [rsp+0x68],rsi
000EFB96: mov       QWORD PTR [rsp+0x70],rbp
000EFB9B: mov       QWORD PTR [rsp+0x40],0xc01006
000EFBA4: mov       QWORD PTR [rsp+0x30],0x3000b
000EFBAD: call      0x1800f0b70
000EFBB2: mov       rbx,QWORD PTR [rip+0x112683f]        # 0x1812163f8
000EFBB9: mov       rsi,rax
000EFBBC: cmp       BYTE PTR [rbx+0xca],0x0
000EFBC3: jne       0x1800efbcd
000EFBC5: mov       rcx,rbx
000EFBC8: call      0x1800cc8c0
000EFBCD: mov       r9,QWORD PTR [rbx+0x8]
000EFBD1: lea       rcx,[rip+0x112fde8]        # 0x18121f9c0
000EFBD8: xor       r8d,r8d
000EFBDB: mov       rdx,rsi
000EFBDE: call      r9
000EFBE1: test      eax,eax
000EFBE3: je        0x1800efbfa
000EFBE5: mov       edx,eax
000EFBE7: lea       rcx,[rip+0x10ba0c2]        # 0x1811a9cb0 ; "Couldn't create the FFXAPI FG SwapChain (dx12): %d"
000EFBEE: call      0x1800fbb40
000EFBF3: mov       eax,0x80004005
000EFBF8: jmp       0x1800efc5c
000EFBFA: lea       rcx,[rsp+0x28]
000EFBFF: call      0x180008420
000EFC04: mov       rcx,QWORD PTR [rsp+0x20]
000EFC09: lea       r8,[rip+0x10af560]        # 0x18119f170
000EFC10: mov       r9,rax
000EFC13: xor       edx,edx
000EFC15: mov       rax,QWORD PTR [rcx]
000EFC18: call      QWORD PTR [rax+0x48]
000EFC1B: mov       rcx,QWORD PTR [rsp+0x28]
000EFC20: lea       rdx,[rsp+0x78]
000EFC25: mov       rax,QWORD PTR [rcx]
000EFC28: call      QWORD PTR [rax+0x50]
000EFC2B: mov       ecx,DWORD PTR [rax+0x20]
000EFC2E: mov       DWORD PTR [rdi+0x110],ecx
000EFC34: mov       rcx,QWORD PTR [rsp+0x28]
000EFC39: mov       rax,QWORD PTR [rcx]
000EFC3C: call      QWORD PTR [rax+0x10]
000EFC3F: mov       rax,QWORD PTR [rsp+0x20]
000EFC44: lea       rcx,[rip+0x10ba0fd]        # 0x1811a9d48 ; 'Create FSR3 Frame Interpolation SwapChain success'
000EFC4B: mov       QWORD PTR [r14],rax
000EFC4E: mov       QWORD PTR [rdi+0x1a8],rax
000EFC55: call      0x1800fbb40
000EFC5A: xor       eax,eax
000EFC5C: mov       rcx,QWORD PTR [rsp+0xb0]
000EFC64: xor       rcx,rsp
000EFC67: call      0x18010c270
000EFC6C: add       rsp,0xc0
000EFC73: pop       r14
000EFC75: pop       rdi
000EFC76: pop       rsi
000EFC77: pop       rbp
000EFC78: pop       rbx
000EFC79: ret       
