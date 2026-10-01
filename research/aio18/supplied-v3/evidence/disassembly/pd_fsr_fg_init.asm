; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xEE870..0xEEF13; unnamed
000EE870: mov       QWORD PTR [rsp+0x10],rbx
000EE875: push      rbp
000EE876: push      rsi
000EE877: push      rdi
000EE878: push      r12
000EE87A: push      r13
000EE87C: push      r14
000EE87E: push      r15
000EE880: lea       rbp,[rsp-0x27]
000EE885: sub       rsp,0xe0
000EE88C: movups    xmm0,XMMWORD PTR [rdx+0x10]
000EE890: movzx     eax,BYTE PTR [rcx+0x124]
000EE897: mov       rdi,rdx
000EE89A: movups    xmm1,XMMWORD PTR [rdx]
000EE89D: mov       rbx,rcx
000EE8A0: mov       WORD PTR [rbp+0x67],0x0
000EE8A6: movsd     xmm2,QWORD PTR [rdx+0x30]
000EE8AB: movaps    XMMWORD PTR [rbp-0x9],xmm0
000EE8AF: movups    xmm0,XMMWORD PTR [rdx+0x20]
000EE8B3: mov       BYTE PTR [rbp+0x0],al
000EE8B6: movzx     eax,WORD PTR [rbp+0x67]
000EE8BA: movups    XMMWORD PTR [rdx],xmm1
000EE8BD: mov       WORD PTR [rcx+0x1c8],ax
000EE8C4: movaps    XMMWORD PTR [rbp-0x49],xmm1
000EE8C8: movaps    xmm1,XMMWORD PTR [rbp-0x9]
000EE8CC: movups    XMMWORD PTR [rdx+0x10],xmm1
000EE8D0: movups    XMMWORD PTR [rdx+0x20],xmm0
000EE8D4: movsd     QWORD PTR [rdx+0x30],xmm2
000EE8D9: movaps    XMMWORD PTR [rbp-0x69],xmm0
000EE8DD: movsd     QWORD PTR [rbp+0x77],xmm2
000EE8E2: call      0x1800cc790
000EE8E7: mov       r8d,DWORD PTR [rbx+0x8]
000EE8EB: mov       rcx,rax
000EE8EE: mov       edx,DWORD PTR [rbx+0x148]
000EE8F4: call      0x1800ccc80
000EE8F9: cmp       QWORD PTR [rip+0x11310c7],0x0        # 0x18121f9c8
000EE901: je        0x1800ee946
000EE903: movzx     eax,BYTE PTR [rdi+0x1a]
000EE907: cmp       BYTE PTR [rbx+0x2a],al
000EE90A: jne       0x1800ee946
000EE90C: movzx     eax,BYTE PTR [rdi+0x19]
000EE910: lea       r14,[rbx+0x29]
000EE914: cmp       BYTE PTR [r14],al
000EE917: jne       0x1800ee946
000EE919: movzx     eax,BYTE PTR [rdi+0x25]
000EE91D: lea       rsi,[rbx+0x35]
000EE921: cmp       BYTE PTR [rsi],al
000EE923: jne       0x1800ee946
000EE925: mov       eax,DWORD PTR [rdi+0xc]
000EE928: cmp       DWORD PTR [rbx+0x1c],eax
000EE92B: jne       0x1800ee946
000EE92D: mov       eax,DWORD PTR [rdi+0x10]
000EE930: cmp       DWORD PTR [rbx+0x20],eax
000EE933: jne       0x1800ee946
000EE935: movzx     eax,BYTE PTR [rdi+0x24]
000EE939: cmp       BYTE PTR [rbx+0x146],al
000EE93F: jne       0x1800ee946
000EE941: xor       r12b,r12b
000EE944: jmp       0x1800ee951
000EE946: mov       r12b,0x1
000EE949: lea       r14,[rbx+0x29]
000EE94D: lea       rsi,[rbx+0x35]
000EE951: mov       rax,QWORD PTR [rbx]
000EE954: mov       rcx,rbx
000EE957: call      QWORD PTR [rax+0x50]
000EE95A: test      al,al
000EE95C: je        0x1800ee96c
000EE95E: test      r12b,r12b
000EE961: je        0x1800ee96c
000EE963: mov       rax,QWORD PTR [rbx]
000EE966: mov       rcx,rbx
000EE969: call      QWORD PTR [rax+0x10]
000EE96C: mov       eax,DWORD PTR [rdi+0xc]
000EE96F: xor       r13d,r13d
000EE972: mov       ecx,DWORD PTR [rdi+0x10]
000EE975: mov       r15d,0x1
000EE97B: movaps    xmm0,XMMWORD PTR [rbp-0x49]
000EE97F: movzx     edx,BYTE PTR [rdi+0x24]
000EE983: movups    XMMWORD PTR [rbx+0x10],xmm0
000EE987: test      dl,dl
000EE989: mov       DWORD PTR [rsp+0x34],eax
000EE98D: movaps    xmm0,XMMWORD PTR [rbp-0x9]
000EE991: movups    XMMWORD PTR [rbx+0x20],xmm0
000EE995: mov       DWORD PTR [rbp-0x7d],eax
000EE998: mov       eax,0x20
000EE99D: movaps    xmm0,XMMWORD PTR [rbp-0x69]
000EE9A1: movups    XMMWORD PTR [rbx+0x30],xmm0
000EE9A5: mov       DWORD PTR [rsp+0x38],ecx
000EE9A9: movsd     xmm0,QWORD PTR [rbp+0x77]
000EE9AE: mov       DWORD PTR [rbp-0x79],ecx
000EE9B1: mov       ecx,0x21
000EE9B6: cmovne    eax,ecx
000EE9B9: movsd     QWORD PTR [rbx+0x40],xmm0
000EE9BE: mov       BYTE PTR [rbx+0x146],dl
000EE9C4: mov       DWORD PTR [rbp-0x75],r13d
000EE9C8: mov       QWORD PTR [rsp+0x28],r13
000EE9CD: mov       QWORD PTR [rsp+0x20],0x20001
000EE9D6: mov       DWORD PTR [rsp+0x30],eax
000EE9DA: cmp       BYTE PTR [r14],r13b
000EE9DD: je        0x1800ee9e6
000EE9DF: or        eax,0x8
000EE9E2: mov       DWORD PTR [rsp+0x30],eax
000EE9E6: cmp       BYTE PTR [rsi],r13b
000EE9E9: je        0x1800ee9f2
000EE9EB: or        eax,0x2
000EE9EE: mov       DWORD PTR [rsp+0x30],eax
000EE9F2: mov       eax,DWORD PTR [rbx+0x8]
000EE9F5: cmp       eax,r15d
000EE9F8: jne       0x1800eea3a
000EE9FA: mov       r8d,DWORD PTR [rbx+0x110]
000EEA01: test      r8d,r8d
000EEA04: jne       0x1800eea0a
000EEA06: mov       r8d,DWORD PTR [rdi+0x14]
000EEA0A: cmp       r8d,0x5b
000EEA0E: ja        0x1800eeb59
000EEA14: lea       rdx,[rip+0xfffffffffff115e5]        # 0x180000000
000EEA1B: movsxd    rax,r8d
000EEA1E: movzx     eax,BYTE PTR [rdx+rax*1+0xeedc4]
000EEA26: mov       ecx,DWORD PTR [rdx+rax*4+0xeed4c]
000EEA2D: add       rcx,rdx
000EEA30: jmp       rcx
000EEA32: mov       eax,r15d
000EEA35: jmp       0x1800eeb6b
000EEA3A: cmp       eax,0x2
000EEA3D: jne       0x1800eeb6e
000EEA43: mov       r8d,DWORD PTR [rbx+0x114]
000EEA4A: test      r8d,r8d
000EEA4D: jne       0x1800eea53
000EEA4F: mov       r8d,DWORD PTR [rdi+0x14]
000EEA53: cmp       r8d,0x7e
000EEA57: ja        0x1800eeb59
000EEA5D: lea       rdx,[rip+0xfffffffffff1159c]        # 0x180000000
000EEA64: movsxd    rax,r8d
000EEA67: movzx     eax,BYTE PTR [rdx+rax*1+0xeee94]
000EEA6F: mov       ecx,DWORD PTR [rdx+rax*4+0xeee20]
000EEA76: add       rcx,rdx
000EEA79: jmp       rcx
000EEA7B: mov       eax,0x2
000EEA80: jmp       0x1800eeb6b
000EEA85: mov       eax,0x3
000EEA8A: jmp       0x1800eeb6b
000EEA8F: mov       eax,0x4
000EEA94: jmp       0x1800eeb6b
000EEA99: mov       eax,0x5
000EEA9E: jmp       0x1800eeb6b
000EEAA3: mov       eax,0x6
000EEAA8: jmp       0x1800eeb6b
000EEAAD: mov       eax,0x7
000EEAB2: jmp       0x1800eeb6b
000EEAB7: mov       eax,0x8
000EEABC: jmp       0x1800eeb6b
000EEAC1: mov       eax,0x9
000EEAC6: jmp       0x1800eeb6b
000EEACB: mov       eax,0xa
000EEAD0: jmp       0x1800eeb6b
000EEAD5: mov       eax,0xb
000EEADA: jmp       0x1800eeb6b
000EEADF: mov       eax,0xc
000EEAE4: jmp       0x1800eeb6b
000EEAE9: mov       eax,0xd
000EEAEE: jmp       0x1800eeb6b
000EEAF0: mov       eax,0xe
000EEAF5: jmp       0x1800eeb6b
000EEAF7: mov       eax,0xf
000EEAFC: jmp       0x1800eeb6b
000EEAFE: mov       eax,0x10
000EEB03: jmp       0x1800eeb6b
000EEB05: mov       eax,0x11
000EEB0A: jmp       0x1800eeb6b
000EEB0C: mov       eax,0x12
000EEB11: jmp       0x1800eeb6b
000EEB13: mov       eax,0x13
000EEB18: jmp       0x1800eeb6b
000EEB1A: mov       eax,0x14
000EEB1F: jmp       0x1800eeb6b
000EEB21: mov       eax,0x15
000EEB26: jmp       0x1800eeb6b
000EEB28: mov       eax,0x16
000EEB2D: jmp       0x1800eeb6b
000EEB2F: mov       eax,0x17
000EEB34: jmp       0x1800eeb6b
000EEB36: mov       eax,0x18
000EEB3B: jmp       0x1800eeb6b
000EEB3D: mov       eax,0x19
000EEB42: jmp       0x1800eeb6b
000EEB44: mov       eax,0x1a
000EEB49: jmp       0x1800eeb6b
000EEB4B: mov       eax,0x1b
000EEB50: jmp       0x1800eeb6b
000EEB52: mov       eax,0x1c
000EEB57: jmp       0x1800eeb6b
000EEB59: mov       edx,r8d
000EEB5C: lea       rcx,[rip+0x10bae8d]        # 0x1811a99f0 ; 'ValidationRemap: Unsupported format requested: %d. Please implement.'
000EEB63: call      0x1800fbb40
000EEB68: mov       eax,r13d
000EEB6B: mov       DWORD PTR [rbp-0x75],eax
000EEB6E: test      r12b,r12b
000EEB71: je        0x1800eec3c
000EEB77: mov       eax,DWORD PTR [rbx+0x8]
000EEB7A: mov       QWORD PTR [rbp-0x59],0x1000000
000EEB82: mov       QWORD PTR [rbp-0x61],r13
000EEB86: mov       QWORD PTR [rbp-0x69],0x2000e
000EEB8E: cmp       eax,r15d
000EEB91: jne       0x1800eebac
000EEB93: mov       rax,QWORD PTR [rbx+0x150]
000EEB9A: lea       rdx,[rbp-0x49]
000EEB9E: mov       QWORD PTR [rbp-0x39],rax
000EEBA2: mov       QWORD PTR [rbp-0x49],0x2
000EEBAA: jmp       0x1800eebde
000EEBAC: cmp       eax,0x2
000EEBAF: jne       0x1800eec26
000EEBB1: mov       rax,QWORD PTR [rbx+0x158]
000EEBB8: lea       rdx,[rbp-0x49]
000EEBBC: mov       QWORD PTR [rbp-0x39],rax
000EEBC0: mov       rax,QWORD PTR [rbx+0x160]
000EEBC7: mov       QWORD PTR [rbp-0x31],rax
000EEBCB: mov       rax,QWORD PTR [rip+0x1114b06]        # 0x1812036d8 ; vulkan-1.dll!vkGetDeviceProcAddr
000EEBD2: mov       QWORD PTR [rbp-0x29],rax
000EEBD6: mov       QWORD PTR [rbp-0x49],0x3
000EEBDE: lea       r8,[rbp-0x69]
000EEBE2: mov       QWORD PTR [rbp-0x41],r13
000EEBE6: lea       rcx,[rsp+0x20]
000EEBEB: call      0x1800f0b30
000EEBF0: mov       rsi,QWORD PTR [rip+0x1127801]        # 0x1812163f8
000EEBF7: mov       r14,rax
000EEBFA: cmp       BYTE PTR [rsi+0xca],r13b
000EEC01: jne       0x1800eec0b
000EEC03: mov       rcx,rsi
000EEC06: call      0x1800cc8c0
000EEC0B: mov       r9,QWORD PTR [rsi+0x8]
000EEC0F: lea       rcx,[rip+0x1130db2]        # 0x18121f9c8
000EEC16: xor       r8d,r8d
000EEC19: mov       rdx,r14
000EEC1C: call      r9
000EEC1F: mov       r15d,eax
000EEC22: test      eax,eax
000EEC24: je        0x1800eec3c
000EEC26: mov       edx,r15d
000EEC29: lea       rcx,[rip+0x10baf00]        # 0x1811a9b30 ; 'ffx::CreateContext for frame generation failed! ErrorCode: %d'
000EEC30: call      0x1800fbb40
000EEC35: xor       al,al
000EEC37: jmp       0x1800eed2e
000EEC3C: mov       r8,QWORD PTR [rdi+0x28]
000EEC40: lea       rdx,[rbx+0x1a8]
000EEC47: mov       QWORD PTR [rbx+0x1b8],r8
000EEC4E: cmp       QWORD PTR [rdx],r13
000EEC51: jne       0x1800eec60
000EEC53: cmp       QWORD PTR [rbx+0x1b0],r13
000EEC5A: je        0x1800eed20
000EEC60: mov       BYTE PTR [rip+0x110b1a1],r13b        # 0x1811f9e08
000EEC67: cmp       DWORD PTR [rbx+0x8],0x1
000EEC6B: jne       0x1800eec8d
000EEC6D: call      0x1800badc0
000EEC72: test      al,al
000EEC74: je        0x1800eec86
000EEC76: lea       rax,[rip+0xfffffffffffff853]        # 0x1800ee4d0
000EEC7D: mov       QWORD PTR [rip+0x110b164],rax        # 0x1811f9de8
000EEC84: jmp       0x1800eec94
000EEC86: lea       rdx,[rbx+0x1a8]
000EEC8D: mov       QWORD PTR [rip+0x110b154],r13        # 0x1811f9de8
000EEC94: mov       eax,DWORD PTR [rbx+0x8]
000EEC97: mov       QWORD PTR [rbx+0x1b8],r8
000EEC9E: cmp       eax,0x1
000EECA1: jne       0x1800eeca8
000EECA3: mov       rax,QWORD PTR [rdx]
000EECA6: jmp       0x1800eecb4
000EECA8: cmp       eax,0x2
000EECAB: jne       0x1800eecbb
000EECAD: mov       rax,QWORD PTR [rbx+0x1b0]
000EECB4: mov       QWORD PTR [rip+0x110b125],rax        # 0x1811f9de0
000EECBB: xorps     xmm0,xmm0
000EECBE: lea       rcx,[rip+0x110b10b]        # 0x1811f9dd0
000EECC5: movaps    XMMWORD PTR [rip+0x110b144],xmm0        # 0x1811f9e10
000EECCC: movaps    XMMWORD PTR [rip+0x110b14d],xmm0        # 0x1811f9e20
000EECD3: movaps    XMMWORD PTR [rip+0x110b156],xmm0        # 0x1811f9e30
000EECDA: call      0x1800f0b60
000EECDF: mov       rbx,QWORD PTR [rip+0x1127712]        # 0x1812163f8
000EECE6: mov       rdi,rax
000EECE9: cmp       BYTE PTR [rbx+0xca],r13b
000EECF0: jne       0x1800eecfa
000EECF2: mov       rcx,rbx
000EECF5: call      0x1800cc8c0
000EECFA: mov       r8,QWORD PTR [rbx]
000EECFD: lea       rcx,[rip+0x1130cc4]        # 0x18121f9c8
000EED04: mov       rdx,rdi
000EED07: call      r8
000EED0A: test      eax,eax
000EED0C: je        0x1800eed20
000EED0E: mov       edx,eax
000EED10: lea       rcx,[rip+0x10bade9]        # 0x1811a9b00 ; "Couldn't set the ffxapi framegen config: %d"
000EED17: call      0x1800fbb40
000EED1C: xor       al,al
000EED1E: jmp       0x1800eed2e
000EED20: lea       rcx,[rip+0x10b1e21]        # 0x1811a0b48 ; 'FrameGenMethod FSR3 ffxFsr3ContextCreate Success!'
000EED27: call      0x1800fbb40
000EED2C: mov       al,0x1
000EED2E: mov       rbx,QWORD PTR [rsp+0x128]
000EED36: add       rsp,0xe0
000EED3D: pop       r15
000EED3F: pop       r14
000EED41: pop       r13
000EED43: pop       r12
000EED45: pop       rdi
000EED46: pop       rsi
000EED47: pop       rbp
000EED48: ret       
000EED49: nop       DWORD PTR [rax]
000EED4C: push      0x32000eeb
000EED51: (bad)     
000EED52: (bad)     
000EED53: add       BYTE PTR [rbp+0x7b000eea],al
000EED59: (bad)     
000EED5A: (bad)     
000EED5B: add       BYTE PTR [rcx-0x70fff116],bl
000EED61: (bad)     
000EED62: (bad)     
000EED63: add       BYTE PTR [rbx+0x5000eea],ah
000EED69: jmp       0x1800eed79
000EED6B: add       dh,bh
000EED6D: (bad)     
000EED6E: (bad)     
000EED6F: add       cl,al
000EED71: (bad)     
000EED72: (bad)     
000EED73: add       bl,cl
000EED75: (bad)     
000EED76: (bad)     
000EED77: add       bh,bl
000EED79: (bad)     
000EED7A: (bad)     
000EED7B: add       ch,dl
000EED7D: (bad)     
000EED7E: (bad)     
000EED7F: add       BYTE PTR [rbx+rbp*8],cl
000EED82: (bad)     
000EED83: add       BYTE PTR [rbx],dl
000EED85: jmp       0x1800eed95
000EED87: add       BYTE PTR [rdx],bl
000EED89: jmp       0x1800eed99
000EED8B: add       BYTE PTR [rdx-0x15],dl
000EED8E: (bad)     
000EED8F: add       BYTE PTR [rdi+0x44000eea],dh
000EED95: jmp       0x1800eeda5
000EED97: add       BYTE PTR [rbx-0x15],cl
000EED9A: (bad)     
000EED9B: add       BYTE PTR [rcx],ah
000EED9D: jmp       0x1800eedad
000EED9F: add       BYTE PTR [rdi],ch
000EEDA1: jmp       0x1800eedb1
000EEDA3: add       BYTE PTR [rax],ch
000EEDA5: jmp       0x1800eedb5
000EEDA7: add       BYTE PTR [rsi],dh
000EEDA9: jmp       0x1800eedb9
000EEDAB: add       BYTE PTR [rip+0xffffffffad000eeb],bh        # 0x12d0efc9c
000EEDB1: (bad)     
000EEDB2: (bad)     
000EEDB3: add       al,dh
000EEDB5: (bad)     
000EEDB6: (bad)     
000EEDB7: add       cl,ch
000EEDB9: (bad)     
000EEDBA: (bad)     
000EEDBB: add       bh,dh
000EEDBD: (bad)     
000EEDBE: (bad)     
000EEDBF: add       BYTE PTR [rcx-0x15],bl
000EEDC2: (bad)     
000EEDC3: add       BYTE PTR [rax],al
000EEDC5: add       DWORD PTR [rdx],eax
000EEDC7: add       ebx,DWORD PTR [rip+0x1d1d041d]        # 0x19d2bf1ea
000EEDCD: sbb       eax,0x1d1d1d05
000EEDD2: sbb       eax,0x1d1d061d
000EEDD7: sbb       eax,0x1d1d1d1d
000EEDDC: (bad)     
000EEDDD: sbb       eax,0xb0a0908
000EEDE2: sbb       eax,0xd1d1d0c
000EEDE7: sbb       eax,0x1d0f1d0e
000EEDEC: adc       BYTE PTR [rax],dl
000EEDEE: adc       DWORD PTR [rip+0x1d1d1d1d],ebx        # 0x19d2c0b11
000EEDF4: sbb       eax,0x1d1d1312
000EEDF9: sbb       eax,0x16151d14
000EEDFE: (bad)     
000EEDFF: sbb       eax,0x1d19181d
000EEE04: sbb       eax,0x1d1d1d1d
000EEE09: sbb       eax,0x1d1d1d1d
000EEE0E: sbb       eax,0x1d1d1d1d
000EEE13: sbb       eax,0x1d1d1d1d
000EEE18: sbb       eax,0x1d1a1d1d
000EEE1D: sbb       eax,0xeb681c1b
000EEE22: (bad)     
000EEE23: add       BYTE PTR [rip+0xffffffffad000eeb],bh        # 0x12d0efd14
000EEE29: (bad)     
000EEE2A: (bad)     
000EEE2B: add       BYTE PTR [rbx+rbp*8+0xe],al
000EEE2F: add       BYTE PTR [rbx-0x15],cl
000EEE32: (bad)     
000EEE33: add       bl,cl
000EEE35: (bad)     
000EEE36: (bad)     
000EEE37: add       ch,dl
000EEE39: (bad)     
000EEE3A: (bad)     
000EEE3B: add       cl,al
000EEE3D: (bad)     
000EEE3E: (bad)     
000EEE3F: add       bh,bl
000EEE41: (bad)     
000EEE42: (bad)     
000EEE43: add       al,dh
000EEE45: (bad)     
000EEE46: (bad)     
000EEE47: add       cl,ch
000EEE49: (bad)     
000EEE4A: (bad)     
000EEE4B: add       bh,dh
000EEE4D: (bad)     
000EEE4E: (bad)     
000EEE4F: add       BYTE PTR [rip+0x2f000eeb],al        # 0x1af0efd40
000EEE55: jmp       0x1800eee65
000EEE57: add       BYTE PTR [rsi],dh
000EEE59: jmp       0x1800eee69
000EEE5B: add       BYTE PTR [rax],ch
000EEE5D: jmp       0x1800eee6d
000EEE5F: add       BYTE PTR [rcx],ah
000EEE61: jmp       0x1800eee71
000EEE63: add       BYTE PTR [rbx],dl
000EEE65: jmp       0x1800eee75
000EEE67: add       BYTE PTR [rdx],bl
000EEE69: jmp       0x1800eee79
000EEE6B: add       BYTE PTR [rbx+rbp*8],cl
000EEE6E: (bad)     
000EEE6F: add       BYTE PTR [rdi-0x48fff116],cl
000EEE75: (bad)     
000EEE76: (bad)     
000EEE77: add       BYTE PTR [rdx-0x15],dl
000EEE7A: (bad)     
000EEE7B: add       BYTE PTR [rbx-0x66fff116],ah
000EEE81: (bad)     
000EEE82: (bad)     
000EEE83: add       BYTE PTR [rbx-0x16],bh
000EEE86: (bad)     
000EEE87: add       BYTE PTR [rbp-0x1fff116],al
000EEE8D: (bad)     
000EEE8E: (bad)     
000EEE8F: add       BYTE PTR [rcx-0x15],bl
000EEE92: (bad)     
000EEE93: add       BYTE PTR [rax],al
000EEE95: sbb       al,0x1c
000EEE97: sbb       al,0x1c
000EEE99: sbb       al,0x1c
000EEE9B: sbb       al,0x1c
000EEE9D: add       DWORD PTR [rsp+rbx*1],ebx
000EEEA0: sbb       al,0x2
000EEEA2: sbb       al,0x1c
000EEEA4: add       ebx,DWORD PTR [rsp+rbx*1]
000EEEA7: sbb       al,0x4
000EEEA9: sbb       al,0x1c
000EEEAB: sbb       al,0x1c
000EEEAD: sbb       al,0x1c
000EEEAF: sbb       al,0x1c
000EEEB1: sbb       al,0x1c
000EEEB3: sbb       al,0x1c
000EEEB5: sbb       al,0x1c
000EEEB7: sbb       al,0x1c
000EEEB9: add       eax,0x1c1c0706
000EEEBE: sbb       al,0x8
000EEEC0: or        DWORD PTR [rdx+rcx*1],ebx
000EEEC3: sbb       al,0x1c
000EEEC5: sbb       al,0xb
000EEEC7: sbb       al,0x1c
000EEEC9: sbb       al,0x1c
000EEECB: sbb       al,0x1c
000EEECD: sbb       al,0x1c
000EEECF: sbb       al,0x1c
000EEED1: sbb       al,0x1c
000EEED3: sbb       al,0xc
000EEED5: sbb       al,0x1c
000EEED7: sbb       al,0x1c
000EEED9: sbb       al,0xd
000EEEDB: (bad)     
000EEEDC: sbb       al,0x1c
000EEEDE: nop       DWORD PTR [rax]
000EEEE1: sbb       al,0x1c
000EEEE3: sbb       al,0x1c
000EEEE5: adc       DWORD PTR [rdx],edx
000EEEE7: adc       ebx,DWORD PTR [rsp+rbx*1]
000EEEEA: sbb       al,0x1c
000EEEEC: sbb       al,0x1c
000EEEEE: sbb       al,0x1c
000EEEF0: sbb       al,0x1c
000EEEF2: sbb       al,0x1c
000EEEF4: sbb       al,0x14
000EEEF6: adc       eax,0x1c1c161c
000EEEFB: (bad)     
000EEEFC: sbb       al,0x1c
000EEEFE: sbb       BYTE PTR [rcx],bl
000EEF00: sbb       al,0x1a
000EEF02: sbb       al,0x1c
000EEF04: sbb       al,0x1c
000EEF06: sbb       al,0x1c
000EEF08: sbb       al,0x1c
000EEF0A: sbb       al,0x1c
000EEF0C: sbb       al,0x1c
000EEF0E: sbb       ebx,DWORD PTR [rsp+rbx*1]
000EEF11: sbb       al,0x16
