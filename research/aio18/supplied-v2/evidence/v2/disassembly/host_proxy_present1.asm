; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x25FE70..0x25FFAE; unnamed
0025FE70: mov       DWORD PTR [rsp+0x18],r8d
0025FE75: mov       DWORD PTR [rsp+0x10],edx
0025FE79: push      rbx
0025FE7A: push      rbp
0025FE7B: push      rsi
0025FE7C: push      rdi
0025FE7D: push      r14
0025FE7F: sub       rsp,0x40
0025FE83: mov       r14,rcx
0025FE86: movzx     eax,BYTE PTR [rcx+0x32]
0025FE8A: test      al,al
0025FE8C: je        0x18025fe99
0025FE8E: test      r8b,0x1
0025FE92: jne       0x18025fe99
0025FE94: mov       rbp,rcx
0025FE97: jmp       0x18025fe9b
0025FE99: xor       ebp,ebp
0025FE9B: mov       QWORD PTR [rsp+0x70],rbp
0025FEA0: test      al,al
0025FEA2: jne       0x18025feb5
0025FEA4: mov       rcx,QWORD PTR [rcx+0x10]
0025FEA8: mov       rax,QWORD PTR [rcx]
0025FEAB: call      QWORD PTR [rax+0x40]
0025FEAE: mov       ebx,eax
0025FEB0: jmp       0x18025ff97
0025FEB5: test      r8b,0x1
0025FEB9: je        0x18025fecc
0025FEBB: mov       rcx,QWORD PTR [rcx+0x10]
0025FEBF: mov       rax,QWORD PTR [rcx]
0025FEC2: call      QWORD PTR [rax+0x40]
0025FEC5: mov       ebx,eax
0025FEC7: jmp       0x18025ff97
0025FECC: cmp       BYTE PTR [rcx+0x31],0x0
0025FED0: je        0x18025ff64
0025FED6: cmp       BYTE PTR [rcx+0x30],0x0
0025FEDA: je        0x18025fef9
0025FEDC: mov       rsi,QWORD PTR [rcx+0x250]
0025FEE3: mov       rax,QWORD PTR [rsi]
0025FEE6: mov       rdi,QWORD PTR [rax+0x178]
0025FEED: mov       rbx,QWORD PTR [rcx+0x148]
0025FEF4: mov       rax,QWORD PTR [rcx]
0025FEF7: jmp       0x18025ff21
0025FEF9: mov       rax,QWORD PTR [rcx]
0025FEFC: call      QWORD PTR [rax+0x120]
0025FF02: test      eax,eax
0025FF04: je        0x18025ff3a
0025FF06: mov       rsi,QWORD PTR [r14+0x250]
0025FF0D: mov       rax,QWORD PTR [rsi]
0025FF10: mov       rdi,QWORD PTR [rax+0x178]
0025FF17: mov       rbx,QWORD PTR [r14+0x40]
0025FF1B: mov       rax,QWORD PTR [r14]
0025FF1E: mov       rcx,r14
0025FF21: call      QWORD PTR [rax+0x120]
0025FF27: mov       eax,eax
0025FF29: imul      rdx,rax,0x58
0025FF2D: mov       r8,rbx
0025FF30: mov       rdx,QWORD PTR [rdx+r14*1+0x40]
0025FF35: mov       rcx,rsi
0025FF38: call      rdi
0025FF3A: mov       QWORD PTR [rsp+0x20],r14
0025FF3F: lea       rax,[rsp+0x78]
0025FF44: mov       QWORD PTR [rsp+0x28],rax
0025FF49: lea       rax,[rsp+0x80]
0025FF51: mov       QWORD PTR [rsp+0x30],rax
0025FF56: lea       rcx,[rsp+0x20]
0025FF5B: call      0x18025fda0
0025FF60: mov       ebx,eax
0025FF62: jmp       0x18025ff97
0025FF64: call      0x1802aa8a0
0025FF69: mov       QWORD PTR [rsp+0x20],r14
0025FF6E: lea       rax,[rsp+0x78]
0025FF73: mov       QWORD PTR [rsp+0x28],rax
0025FF78: lea       rax,[rsp+0x80]
0025FF80: mov       QWORD PTR [rsp+0x30],rax
0025FF85: lea       rcx,[rsp+0x20]
0025FF8A: call      0x18025fcc0
0025FF8F: mov       ebx,eax
0025FF91: call      0x1802ac050
0025FF96: nop       
0025FF97: test      rbp,rbp
0025FF9A: je        0x18025ffa1
0025FF9C: call      0x1802648f0
0025FFA1: mov       eax,ebx
0025FFA3: add       rsp,0x40
0025FFA7: pop       r14
0025FFA9: pop       rdi
0025FFAA: pop       rsi
0025FFAB: pop       rbp
0025FFAC: pop       rbx
0025FFAD: ret       
