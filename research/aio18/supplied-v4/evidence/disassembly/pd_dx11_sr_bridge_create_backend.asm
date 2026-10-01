; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xCFA50..0xCFBD0; unnamed
000CFA50: mov       QWORD PTR [rsp+0x10],rbx
000CFA55: push      rdi
000CFA56: sub       rsp,0x40
000CFA5A: mov       rdi,rcx
000CFA5D: cmp       QWORD PTR [rcx+0x118],0x0
000CFA65: jne       0x1800cfbc5
000CFA6B: mov       eax,DWORD PTR [rcx+0x110]
000CFA71: cmp       eax,0x1
000CFA74: jne       0x1800cfa82
000CFA76: mov       DWORD PTR [rcx+0x110],0x3
000CFA80: jmp       0x1800cfafb
000CFA82: test      eax,eax
000CFA84: jne       0x1800cfab7
000CFA86: mov       rbx,QWORD PTR [rip+0x1146923]        # 0x1812163b0
000CFA8D: mov       ecx,0x1a8
000CFA92: call      0x18010c290
000CFA97: mov       QWORD PTR [rsp+0x50],rax
000CFA9C: mov       r8d,0x1
000CFAA2: mov       rdx,QWORD PTR [rbx+0x98]
000CFAA9: mov       rcx,rax
000CFAAC: call      0x1800cc5b0
000CFAB1: nop       
000CFAB2: jmp       0x1800cfb96
000CFAB7: cmp       eax,0x2
000CFABA: jne       0x1800cfaef
000CFABC: mov       rbx,QWORD PTR [rip+0x11468ed]        # 0x1812163b0
000CFAC3: mov       ecx,0x168
000CFAC8: call      0x18010c290
000CFACD: mov       QWORD PTR [rsp+0x50],rax
000CFAD2: mov       r9d,0x1
000CFAD8: mov       r8,QWORD PTR [rbx+0x98]
000CFADF: xor       edx,edx
000CFAE1: mov       rcx,rax
000CFAE4: call      0x1801017e0
000CFAE9: nop       
000CFAEA: jmp       0x1800cfb96
000CFAEF: add       eax,0xfffffffd
000CFAF2: cmp       eax,0x1
000CFAF5: ja        0x1800cfbc5
000CFAFB: call      0x1800cc790
000CFB00: mov       rbx,rax
000CFB03: cmp       BYTE PTR [rax+0xca],0x0
000CFB0A: jne       0x1800cfb14
000CFB0C: mov       rcx,rax
000CFB0F: call      0x1800cc8c0
000CFB14: cmp       BYTE PTR [rbx+0xc8],0x0
000CFB1B: mov       rbx,QWORD PTR [rip+0x114688e]        # 0x1812163b0
000CFB22: je        0x1800cfb65
000CFB24: mov       ecx,0x190
000CFB29: call      0x18010c290
000CFB2E: mov       QWORD PTR [rsp+0x50],rax
000CFB33: mov       ecx,DWORD PTR [rdi+0x110]
000CFB39: mov       DWORD PTR [rsp+0x30],ecx
000CFB3D: mov       DWORD PTR [rsp+0x28],0x1
000CFB45: mov       QWORD PTR [rsp+0x20],0x0
000CFB4E: xor       r9d,r9d
000CFB51: mov       r8,QWORD PTR [rbx+0x98]
000CFB58: xor       edx,edx
000CFB5A: mov       rcx,rax
000CFB5D: call      0x1801003e0
000CFB62: nop       
000CFB63: jmp       0x1800cfb96
000CFB65: mov       DWORD PTR [rdi+0x110],0x2
000CFB6F: mov       ecx,0x168
000CFB74: call      0x18010c290
000CFB79: mov       QWORD PTR [rsp+0x50],rax
000CFB7E: mov       r9d,0x1
000CFB84: mov       r8,QWORD PTR [rbx+0x98]
000CFB8B: xor       edx,edx
000CFB8D: mov       rcx,rax
000CFB90: call      0x1801017e0
000CFB95: nop       
000CFB96: mov       rcx,QWORD PTR [rdi+0x118]
000CFB9D: test      rcx,rcx
000CFBA0: mov       QWORD PTR [rdi+0x118],rax
000CFBA7: je        0x1800cfbb4
000CFBA9: mov       rax,QWORD PTR [rcx]
000CFBAC: mov       edx,0x1
000CFBB1: call      QWORD PTR [rax+0x48]
000CFBB4: mov       rcx,QWORD PTR [rdi+0x118]
000CFBBB: mov       rax,QWORD PTR [rcx]
000CFBBE: movzx     edx,BYTE PTR [rdi+0x48]
000CFBC2: call      QWORD PTR [rax+0x38]
000CFBC5: mov       rbx,QWORD PTR [rsp+0x58]
000CFBCA: add       rsp,0x40
000CFBCE: pop       rdi
000CFBCF: ret       
