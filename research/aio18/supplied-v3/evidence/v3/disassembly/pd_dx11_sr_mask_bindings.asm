; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xFEDC0..0xFF4B7; unnamed
000FEDC0: mov       QWORD PTR [rsp+0x20],rbx
000FEDC5: push      rbp
000FEDC6: push      rsi
000FEDC7: push      rdi
000FEDC8: push      r14
000FEDCA: push      r15
000FEDCC: lea       rbp,[rsp-0x570]
000FEDD4: sub       rsp,0x670
000FEDDB: movaps    XMMWORD PTR [rsp+0x660],xmm6
000FEDE3: mov       rax,QWORD PTR [rip+0x10cbbd6]        # 0x1811ca9c0
000FEDEA: xor       rax,rsp
000FEDED: mov       QWORD PTR [rbp+0x550],rax
000FEDF4: mov       r15,r8
000FEDF7: mov       rdi,rdx
000FEDFA: mov       rsi,rcx
000FEDFD: xor       edx,edx
000FEDFF: mov       r8d,0x528
000FEE05: lea       rcx,[rbp+0x20]
000FEE09: call      0x18010d61a
000FEE0E: mov       rax,QWORD PTR [rip+0x1120be3]        # 0x18121f9f8
000FEE15: mov       rcx,QWORD PTR [rdi+0x68]
000FEE19: mov       rdx,QWORD PTR [rax+0xa8]
000FEE20: call      rdx
000FEE22: mov       rdx,QWORD PTR [r15+0x8]
000FEE26: lea       rcx,[rsp+0x50]
000FEE2B: mov       QWORD PTR [rbp+0x20],rax
000FEE2F: mov       rax,QWORD PTR [rip+0x1120bc2]        # 0x18121f9f8
000FEE36: mov       r8,QWORD PTR [rax+0xc0]
000FEE3D: mov       rbx,QWORD PTR [rax+0xb0]
000FEE44: call      r8
000FEE47: mov       rdx,QWORD PTR [r15+0x8]
000FEE4B: lea       r9,[rip+0x10ad17e]        # 0x1811abfd0 ; 'FSR3_Input_Color'
000FEE52: lea       r8,[rsp+0x30]
000FEE57: mov       DWORD PTR [rsp+0x20],0xc
000FEE5F: lea       rcx,[rsp+0x70]
000FEE64: movups    xmm0,XMMWORD PTR [rax]
000FEE67: movups    xmm1,XMMWORD PTR [rax+0x10]
000FEE6B: movaps    XMMWORD PTR [rsp+0x30],xmm0
000FEE70: movaps    XMMWORD PTR [rsp+0x40],xmm1
000FEE75: call      rbx
000FEE77: mov       rdx,QWORD PTR [r15+0x18]
000FEE7B: lea       rcx,[rbp+0x28]
000FEE7F: movups    xmm0,XMMWORD PTR [rax]
000FEE82: movups    XMMWORD PTR [rcx],xmm0
000FEE85: movups    xmm1,XMMWORD PTR [rax+0x10]
000FEE89: movups    XMMWORD PTR [rcx+0x10],xmm1
000FEE8D: movups    xmm0,XMMWORD PTR [rax+0x20]
000FEE91: movups    XMMWORD PTR [rcx+0x20],xmm0
000FEE95: movups    xmm1,XMMWORD PTR [rax+0x30]
000FEE99: movups    XMMWORD PTR [rcx+0x30],xmm1
000FEE9D: movups    xmm0,XMMWORD PTR [rax+0x40]
000FEEA1: movups    XMMWORD PTR [rcx+0x40],xmm0
000FEEA5: movups    xmm1,XMMWORD PTR [rax+0x50]
000FEEA9: movups    XMMWORD PTR [rcx+0x50],xmm1
000FEEAD: movups    xmm0,XMMWORD PTR [rax+0x60]
000FEEB1: movups    XMMWORD PTR [rcx+0x60],xmm0
000FEEB5: movups    xmm0,XMMWORD PTR [rax+0x70]
000FEEB9: movups    XMMWORD PTR [rcx+0x70],xmm0
000FEEBD: movups    xmm1,XMMWORD PTR [rax+0x80]
000FEEC4: movups    XMMWORD PTR [rcx+0x80],xmm1
000FEECB: movups    xmm0,XMMWORD PTR [rax+0x90]
000FEED2: movups    XMMWORD PTR [rcx+0x90],xmm0
000FEED9: movups    xmm1,XMMWORD PTR [rax+0xa0]
000FEEE0: mov       rax,QWORD PTR [rip+0x1120b11]        # 0x18121f9f8
000FEEE7: movups    XMMWORD PTR [rcx+0xa0],xmm1
000FEEEE: lea       rcx,[rsp+0x50]
000FEEF3: mov       r8,QWORD PTR [rax+0xc0]
000FEEFA: mov       rbx,QWORD PTR [rax+0xb0]
000FEF01: call      r8
000FEF04: mov       rdx,QWORD PTR [r15+0x18]
000FEF08: lea       r9,[rip+0x10ad0e9]        # 0x1811abff8 ; 'FSR3_InputDepth'
000FEF0F: lea       r8,[rsp+0x30]
000FEF14: mov       DWORD PTR [rsp+0x20],0xc
000FEF1C: lea       rcx,[rsp+0x70]
000FEF21: movups    xmm0,XMMWORD PTR [rax]
000FEF24: movups    xmm1,XMMWORD PTR [rax+0x10]
000FEF28: movaps    XMMWORD PTR [rsp+0x30],xmm0
000FEF2D: movaps    XMMWORD PTR [rsp+0x40],xmm1
000FEF32: call      rbx
000FEF34: lea       rcx,[rbp+0xd8]
000FEF3B: movups    xmm0,XMMWORD PTR [rax]
000FEF3E: movups    XMMWORD PTR [rcx],xmm0
000FEF41: movups    xmm1,XMMWORD PTR [rax+0x10]
000FEF45: movups    XMMWORD PTR [rcx+0x10],xmm1
000FEF49: movups    xmm0,XMMWORD PTR [rax+0x20]
000FEF4D: movups    XMMWORD PTR [rcx+0x20],xmm0
000FEF51: movups    xmm1,XMMWORD PTR [rax+0x30]
000FEF55: movups    XMMWORD PTR [rcx+0x30],xmm1
000FEF59: movups    xmm0,XMMWORD PTR [rax+0x40]
000FEF5D: movups    XMMWORD PTR [rcx+0x40],xmm0
000FEF61: movups    xmm1,XMMWORD PTR [rax+0x50]
000FEF65: movups    XMMWORD PTR [rcx+0x50],xmm1
000FEF69: movups    xmm0,XMMWORD PTR [rax+0x60]
000FEF6D: mov       rdx,QWORD PTR [r15+0x10]
000FEF71: movups    XMMWORD PTR [rcx+0x60],xmm0
000FEF75: movups    xmm1,XMMWORD PTR [rax+0x70]
000FEF79: movups    XMMWORD PTR [rcx+0x70],xmm1
000FEF7D: movups    xmm0,XMMWORD PTR [rax+0x80]
000FEF84: movups    XMMWORD PTR [rcx+0x80],xmm0
000FEF8B: movups    xmm1,XMMWORD PTR [rax+0x90]
000FEF92: movups    XMMWORD PTR [rcx+0x90],xmm1
000FEF99: movups    xmm0,XMMWORD PTR [rax+0xa0]
000FEFA0: mov       rax,QWORD PTR [rip+0x1120a51]        # 0x18121f9f8
000FEFA7: movups    XMMWORD PTR [rcx+0xa0],xmm0
000FEFAE: lea       rcx,[rsp+0x50]
000FEFB3: mov       r8,QWORD PTR [rax+0xc0]
000FEFBA: mov       rbx,QWORD PTR [rax+0xb0]
000FEFC1: call      r8
000FEFC4: mov       rdx,QWORD PTR [r15+0x10]
000FEFC8: lea       r9,[rip+0x10ad0a1]        # 0x1811ac070 ; 'FSR3_InputMotionVectors'
000FEFCF: lea       r8,[rsp+0x30]
000FEFD4: mov       DWORD PTR [rsp+0x20],0xc
000FEFDC: lea       rcx,[rsp+0x70]
000FEFE1: movups    xmm0,XMMWORD PTR [rax]
000FEFE4: movups    xmm1,XMMWORD PTR [rax+0x10]
000FEFE8: movaps    XMMWORD PTR [rsp+0x30],xmm0
000FEFED: movaps    XMMWORD PTR [rsp+0x40],xmm1
000FEFF2: call      rbx
000FEFF4: lea       rcx,[rbp+0x188]
000FEFFB: xor       edx,edx
000FEFFD: movups    xmm0,XMMWORD PTR [rax]
000FF000: movups    XMMWORD PTR [rcx],xmm0
000FF003: movups    xmm1,XMMWORD PTR [rax+0x10]
000FF007: movups    XMMWORD PTR [rcx+0x10],xmm1
000FF00B: movups    xmm0,XMMWORD PTR [rax+0x20]
000FF00F: movups    XMMWORD PTR [rcx+0x20],xmm0
000FF013: movups    xmm1,XMMWORD PTR [rax+0x30]
000FF017: movups    XMMWORD PTR [rcx+0x30],xmm1
000FF01B: movups    xmm0,XMMWORD PTR [rax+0x40]
000FF01F: movups    XMMWORD PTR [rcx+0x40],xmm0
000FF023: movups    xmm1,XMMWORD PTR [rax+0x50]
000FF027: movups    XMMWORD PTR [rcx+0x50],xmm1
000FF02B: movups    xmm0,XMMWORD PTR [rax+0x60]
000FF02F: movups    XMMWORD PTR [rcx+0x60],xmm0
000FF033: movups    xmm1,XMMWORD PTR [rax+0x70]
000FF037: movups    XMMWORD PTR [rcx+0x70],xmm1
000FF03B: movups    xmm0,XMMWORD PTR [rax+0x80]
000FF042: movups    XMMWORD PTR [rcx+0x80],xmm0
000FF049: movups    xmm1,XMMWORD PTR [rax+0x90]
000FF050: movups    XMMWORD PTR [rcx+0x90],xmm1
000FF057: movups    xmm0,XMMWORD PTR [rax+0xa0]
000FF05E: mov       rax,QWORD PTR [rip+0x1120993]        # 0x18121f9f8
000FF065: movups    XMMWORD PTR [rcx+0xa0],xmm0
000FF06C: lea       rcx,[rsp+0x50]
000FF071: mov       r8,QWORD PTR [rax+0xc0]
000FF078: mov       rbx,QWORD PTR [rax+0xb0]
000FF07F: call      r8
000FF082: lea       r9,[rip+0x10ad017]        # 0x1811ac0a0 ; 'FSR3_InputExposure'
000FF089: mov       DWORD PTR [rsp+0x20],0xc
000FF091: lea       r8,[rsp+0x30]
000FF096: xor       edx,edx
000FF098: lea       rcx,[rsp+0x70]
000FF09D: movups    xmm0,XMMWORD PTR [rax]
000FF0A0: movups    xmm1,XMMWORD PTR [rax+0x10]
000FF0A4: movaps    XMMWORD PTR [rsp+0x30],xmm0
000FF0A9: movaps    XMMWORD PTR [rsp+0x40],xmm1
000FF0AE: call      rbx
000FF0B0: lea       rcx,[rbp+0x238]
000FF0B7: movups    xmm0,XMMWORD PTR [rax]
000FF0BA: movups    XMMWORD PTR [rcx],xmm0
000FF0BD: movups    xmm1,XMMWORD PTR [rax+0x10]
000FF0C1: movups    XMMWORD PTR [rcx+0x10],xmm1
000FF0C5: movups    xmm0,XMMWORD PTR [rax+0x20]
000FF0C9: movups    XMMWORD PTR [rcx+0x20],xmm0
000FF0CD: movups    xmm1,XMMWORD PTR [rax+0x30]
000FF0D1: movups    XMMWORD PTR [rcx+0x30],xmm1
000FF0D5: movups    xmm0,XMMWORD PTR [rax+0x40]
000FF0D9: movups    XMMWORD PTR [rcx+0x40],xmm0
000FF0DD: movups    xmm1,XMMWORD PTR [rax+0x50]
000FF0E1: movups    XMMWORD PTR [rcx+0x50],xmm1
000FF0E5: movups    xmm0,XMMWORD PTR [rax+0x60]
000FF0E9: movups    XMMWORD PTR [rcx+0x60],xmm0
000FF0ED: movups    xmm1,XMMWORD PTR [rax+0x70]
000FF0F1: movups    XMMWORD PTR [rcx+0x70],xmm1
000FF0F5: movups    xmm0,XMMWORD PTR [rax+0x80]
000FF0FC: mov       rdx,QWORD PTR [r15+0x20]
000FF100: movups    XMMWORD PTR [rcx+0x80],xmm0
000FF107: movups    xmm1,XMMWORD PTR [rax+0x90]
000FF10E: movups    XMMWORD PTR [rcx+0x90],xmm1
000FF115: movups    xmm0,XMMWORD PTR [rax+0xa0]
000FF11C: mov       rax,QWORD PTR [rip+0x11208d5]        # 0x18121f9f8
000FF123: movups    XMMWORD PTR [rcx+0xa0],xmm0
000FF12A: lea       rcx,[rsp+0x50]
000FF12F: mov       r8,QWORD PTR [rax+0xc0]
000FF136: mov       rbx,QWORD PTR [rax+0xb0]
000FF13D: call      r8
000FF140: mov       rdx,QWORD PTR [r15+0x20]
000FF144: lea       r9,[rip+0x10acecd]        # 0x1811ac018 ; 'FSR3_InputReactiveMap'
000FF14B: lea       r8,[rsp+0x30]
000FF150: mov       DWORD PTR [rsp+0x20],0xc
000FF158: lea       rcx,[rsp+0x70]
000FF15D: movups    xmm0,XMMWORD PTR [rax]
000FF160: movups    xmm1,XMMWORD PTR [rax+0x10]
000FF164: movaps    XMMWORD PTR [rsp+0x30],xmm0
000FF169: movaps    XMMWORD PTR [rsp+0x40],xmm1
000FF16E: call      rbx
000FF170: mov       r14,QWORD PTR [r15+0x38]
000FF174: lea       rcx,[rbp+0x2e8]
000FF17B: movups    xmm0,XMMWORD PTR [rax]
000FF17E: movups    XMMWORD PTR [rcx],xmm0
000FF181: movups    xmm1,XMMWORD PTR [rax+0x10]
000FF185: movups    XMMWORD PTR [rcx+0x10],xmm1
000FF189: movups    xmm0,XMMWORD PTR [rax+0x20]
000FF18D: movups    XMMWORD PTR [rcx+0x20],xmm0
000FF191: movups    xmm1,XMMWORD PTR [rax+0x30]
000FF195: movups    XMMWORD PTR [rcx+0x30],xmm1
000FF199: movups    xmm0,XMMWORD PTR [rax+0x40]
000FF19D: movups    XMMWORD PTR [rcx+0x40],xmm0
000FF1A1: movups    xmm1,XMMWORD PTR [rax+0x50]
000FF1A5: movups    XMMWORD PTR [rcx+0x50],xmm1
000FF1A9: movups    xmm0,XMMWORD PTR [rax+0x60]
000FF1AD: movups    XMMWORD PTR [rcx+0x60],xmm0
000FF1B1: movups    xmm1,XMMWORD PTR [rax+0x70]
000FF1B5: movups    XMMWORD PTR [rcx+0x70],xmm1
000FF1B9: movups    xmm0,XMMWORD PTR [rax+0x80]
000FF1C0: movups    XMMWORD PTR [rcx+0x80],xmm0
000FF1C7: movups    xmm1,XMMWORD PTR [rax+0x90]
000FF1CE: movups    XMMWORD PTR [rcx+0x90],xmm1
000FF1D5: movups    xmm0,XMMWORD PTR [rax+0xa0]
000FF1DC: movups    XMMWORD PTR [rcx+0xa0],xmm0
000FF1E3: test      r14,r14
000FF1E6: jne       0x1800ff1ec
000FF1E8: mov       r14,QWORD PTR [r15+0x30]
000FF1EC: mov       rax,QWORD PTR [rip+0x1120805]        # 0x18121f9f8
000FF1F3: lea       rcx,[rsp+0x50]
000FF1F8: mov       rdx,r14
000FF1FB: mov       r8,QWORD PTR [rax+0xc0]
000FF202: mov       rbx,QWORD PTR [rax+0xb0]
000FF209: call      r8
000FF20C: lea       r9,[rip+0x10ace35]        # 0x1811ac048 ; 'FSR3_Output_Color'
000FF213: mov       DWORD PTR [rsp+0x20],0xc
000FF21B: lea       r8,[rsp+0x30]
000FF220: mov       rdx,r14
000FF223: lea       rcx,[rsp+0x70]
000FF228: movups    xmm0,XMMWORD PTR [rax]
000FF22B: movups    xmm1,XMMWORD PTR [rax+0x10]
000FF22F: movaps    XMMWORD PTR [rsp+0x30],xmm0
000FF234: movaps    XMMWORD PTR [rsp+0x40],xmm1
000FF239: call      rbx
000FF23B: lea       rcx,[rbp+0x448]
000FF242: xor       edx,edx
000FF244: movups    xmm0,XMMWORD PTR [rax]
000FF247: movups    XMMWORD PTR [rcx],xmm0
000FF24A: movups    xmm1,XMMWORD PTR [rax+0x10]
000FF24E: movups    XMMWORD PTR [rcx+0x10],xmm1
000FF252: movups    xmm0,XMMWORD PTR [rax+0x20]
000FF256: movups    XMMWORD PTR [rcx+0x20],xmm0
000FF25A: movups    xmm1,XMMWORD PTR [rax+0x30]
000FF25E: movups    XMMWORD PTR [rcx+0x30],xmm1
000FF262: movups    xmm0,XMMWORD PTR [rax+0x40]
000FF266: movups    XMMWORD PTR [rcx+0x40],xmm0
000FF26A: movups    xmm1,XMMWORD PTR [rax+0x50]
000FF26E: movups    XMMWORD PTR [rcx+0x50],xmm1
000FF272: movups    xmm0,XMMWORD PTR [rax+0x60]
000FF276: movups    XMMWORD PTR [rcx+0x60],xmm0
000FF27A: movups    xmm1,XMMWORD PTR [rax+0x70]
000FF27E: movups    XMMWORD PTR [rcx+0x70],xmm1
000FF282: movups    xmm0,XMMWORD PTR [rax+0x80]
000FF289: movups    XMMWORD PTR [rcx+0x80],xmm0
000FF290: movups    xmm1,XMMWORD PTR [rax+0x90]
000FF297: movups    XMMWORD PTR [rcx+0x90],xmm1
000FF29E: movups    xmm0,XMMWORD PTR [rax+0xa0]
000FF2A5: mov       rax,QWORD PTR [rip+0x112074c]        # 0x18121f9f8
000FF2AC: movups    XMMWORD PTR [rcx+0xa0],xmm0
000FF2B3: lea       rcx,[rsp+0x50]
000FF2B8: mov       r8,QWORD PTR [rax+0xc0]
000FF2BF: mov       rbx,QWORD PTR [rax+0xb0]
000FF2C6: call      r8
000FF2C9: lea       r9,[rip+0x10ace00]        # 0x1811ac0d0 ; 'FSR3_TransparencyAndCompositionMap'
000FF2D0: mov       DWORD PTR [rsp+0x20],0xc
000FF2D8: lea       r8,[rsp+0x30]
000FF2DD: xor       edx,edx
000FF2DF: lea       rcx,[rsp+0x70]
000FF2E4: movups    xmm0,XMMWORD PTR [rax]
000FF2E7: movups    xmm1,XMMWORD PTR [rax+0x10]
000FF2EB: movaps    XMMWORD PTR [rsp+0x30],xmm0
000FF2F0: movaps    XMMWORD PTR [rsp+0x40],xmm1
000FF2F5: call      rbx
000FF2F7: lea       rcx,[rbp+0x398]
000FF2FE: movups    xmm0,XMMWORD PTR [rax]
000FF301: movups    XMMWORD PTR [rcx],xmm0
000FF304: movups    xmm1,XMMWORD PTR [rax+0x10]
000FF308: movups    XMMWORD PTR [rcx+0x10],xmm1
000FF30C: movups    xmm0,XMMWORD PTR [rax+0x20]
000FF310: movups    XMMWORD PTR [rcx+0x20],xmm0
000FF314: movups    xmm1,XMMWORD PTR [rax+0x30]
000FF318: movups    XMMWORD PTR [rcx+0x30],xmm1
000FF31C: movups    xmm0,XMMWORD PTR [rax+0x40]
000FF320: movups    XMMWORD PTR [rcx+0x40],xmm0
000FF324: movups    xmm1,XMMWORD PTR [rax+0x50]
000FF328: movups    XMMWORD PTR [rcx+0x50],xmm1
000FF32C: movups    xmm0,XMMWORD PTR [rax+0x60]
000FF330: movups    XMMWORD PTR [rcx+0x60],xmm0
000FF334: movups    xmm1,XMMWORD PTR [rax+0x70]
000FF338: movups    XMMWORD PTR [rcx+0x70],xmm1
000FF33C: movups    xmm0,XMMWORD PTR [rax+0x80]
000FF343: movups    XMMWORD PTR [rcx+0x80],xmm0
000FF34A: movups    xmm1,XMMWORD PTR [rax+0x90]
000FF351: movups    XMMWORD PTR [rcx+0x90],xmm1
000FF358: movups    xmm0,XMMWORD PTR [rax+0xa0]
000FF35F: movzx     eax,BYTE PTR [rdi+0x54]
000FF363: movups    xmm1,XMMWORD PTR [rdi+0x44]
000FF367: movups    XMMWORD PTR [rcx+0xa0],xmm0
000FF36E: movups    XMMWORD PTR [rbp+0x4f8],xmm1
000FF375: movss     xmm0,DWORD PTR [rdi+0x40]
000FF37A: mov       BYTE PTR [rbp+0x528],al
000FF380: movzx     eax,BYTE PTR [rsi+0x12c]
000FF387: mov       BYTE PTR [rbp+0x518],al
000FF38D: cvttss2si rax,DWORD PTR [rdi+0x38]
000FF393: mov       DWORD PTR [rbp+0x524],0x3f800000
000FF39D: movss     DWORD PTR [rbp+0x51c],xmm0
000FF3A5: mov       DWORD PTR [rbp+0x508],eax
000FF3AB: cvttss2si rax,DWORD PTR [rdi+0x3c]
000FF3B1: mov       DWORD PTR [rbp+0x50c],eax
000FF3B7: call      0x1800fdd00
000FF3BC: cmp       BYTE PTR [rsi+0x29],0x0
000FF3C0: movaps    xmm1,xmm0
000FF3C3: subsd     xmm1,QWORD PTR [rsi+0x130]
000FF3CB: mov       eax,DWORD PTR [rdi+0x98]
000FF3D1: movss     xmm6,DWORD PTR [rdi+0x58]
000FF3D6: movsd     QWORD PTR [rsi+0x130],xmm0
000FF3DE: xorps     xmm0,xmm0
000FF3E1: mov       QWORD PTR [rbp+0x540],rax
000FF3E8: cvtsd2ss  xmm0,xmm1
000FF3EC: movss     xmm1,DWORD PTR [rdi+0x5c]
000FF3F1: movss     DWORD PTR [rbp+0x520],xmm0
000FF3F9: movss     xmm0,DWORD PTR [rdi+0x60]
000FF3FE: movss     DWORD PTR [rbp+0x534],xmm0
000FF406: movaps    xmm0,xmm6
000FF409: je        0x1800ff422
000FF40B: movss     DWORD PTR [rbp+0x530],xmm6
000FF413: call      0x1800ee440
000FF418: movss     DWORD PTR [rbp+0x52c],xmm0
000FF420: jmp       0x1800ff437
000FF422: call      0x1800ee440
000FF427: movss     DWORD PTR [rbp+0x530],xmm0
000FF42F: movss     DWORD PTR [rbp+0x52c],xmm6
000FF437: cmp       BYTE PTR [rsi+0x48],0x0
000FF43B: je        0x1800ff450
000FF43D: cmp       BYTE PTR [rsi+0x49],0x0
000FF441: je        0x1800ff449
000FF443: cmp       DWORD PTR [rsi+0x4c],0x1
000FF447: je        0x1800ff450
000FF449: mov       eax,0x1
000FF44E: jmp       0x1800ff452
000FF450: xor       eax,eax
000FF452: or        DWORD PTR [rbp+0x53c],eax
000FF458: mov       rdx,rdi
000FF45B: mov       rax,QWORD PTR [rip+0x1120596]        # 0x18121f9f8
000FF462: mov       rbx,QWORD PTR [rax+0xd8]
000FF469: call      0x1800ffc10
000FF46E: lea       rdx,[rbp+0x20]
000FF472: mov       rcx,rax
000FF475: call      rbx
000FF477: test      eax,eax
000FF479: je        0x1800ff489
000FF47B: mov       edx,eax
000FF47D: lea       rcx,[rip+0x10acc94]        # 0x1811ac118 ; 'ffxFsr3ContextDispatchUpscale Failed! ErrorCode: 0x%08x'
000FF484: call      0x1800fbb40
000FF489: mov       rcx,QWORD PTR [rbp+0x550]
000FF490: xor       rcx,rsp
000FF493: call      0x18010c270
000FF498: mov       rbx,QWORD PTR [rsp+0x6b8]
000FF4A0: movaps    xmm6,XMMWORD PTR [rsp+0x660]
000FF4A8: add       rsp,0x670
000FF4AF: pop       r15
000FF4B1: pop       r14
000FF4B3: pop       rdi
000FF4B4: pop       rsi
000FF4B5: pop       rbp
000FF4B6: ret       
