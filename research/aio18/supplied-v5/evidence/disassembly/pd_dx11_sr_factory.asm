; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xF5170..0xF53BF; unnamed
000F5170: mov       QWORD PTR [rsp+0x10],rbx
000F5175: push      rdi
000F5176: sub       rsp,0x40
000F517A: mov       rdi,rcx
000F517D: cmp       QWORD PTR [rcx+0x48],0x0
000F5182: jne       0x1800f53a5
000F5188: mov       eax,DWORD PTR [rcx+0x60]
000F518B: test      eax,eax
000F518D: jne       0x1800f51be
000F518F: mov       rax,QWORD PTR [rcx]
000F5192: call      QWORD PTR [rax+0x108]
000F5198: mov       rbx,rax
000F519B: mov       ecx,0x1a8
000F51A0: call      0x18010c290
000F51A5: mov       QWORD PTR [rsp+0x50],rax
000F51AA: xor       r8d,r8d
000F51AD: mov       rdx,rbx
000F51B0: mov       rcx,rax
000F51B3: call      0x1800cc5b0
000F51B8: nop       
000F51B9: jmp       0x1800f538d
000F51BE: cmp       eax,0x1
000F51C1: je        0x1800f523c
000F51C3: cmp       eax,0x3
000F51C6: je        0x1800f5243
000F51C8: cmp       eax,0x2
000F51CB: jne       0x1800f5208
000F51CD: cmp       BYTE PTR [rcx+0xa1],0x0
000F51D4: je        0x1800f5208
000F51D6: mov       rax,QWORD PTR [rcx]
000F51D9: call      QWORD PTR [rax+0x108]
000F51DF: mov       rbx,rax
000F51E2: mov       ecx,0x168
000F51E7: call      0x18010c290
000F51EC: mov       QWORD PTR [rsp+0x50],rax
000F51F1: xor       r9d,r9d
000F51F4: xor       r8d,r8d
000F51F7: mov       rdx,rbx
000F51FA: mov       rcx,rax
000F51FD: call      0x1801017e0
000F5202: nop       
000F5203: jmp       0x1800f538d
000F5208: mov       rax,QWORD PTR [rcx]
000F520B: call      QWORD PTR [rax+0x108]
000F5211: mov       rbx,rax
000F5214: mov       ecx,0x670
000F5219: call      0x18010c290
000F521E: mov       QWORD PTR [rsp+0x50],rax
000F5223: movzx     r9d,BYTE PTR [rdi+0x64]
000F5228: mov       r8,rbx
000F522B: mov       edx,DWORD PTR [rdi+0x60]
000F522E: mov       rcx,rax
000F5231: call      0x1800cf080
000F5236: nop       
000F5237: jmp       0x1800f538d
000F523C: mov       DWORD PTR [rcx+0x60],0x3
000F5243: mov       rax,QWORD PTR [rcx]
000F5246: call      QWORD PTR [rax+0x108]
000F524C: mov       rbx,rax
000F524F: mov       ecx,0x4f0
000F5254: call      0x18010c290
000F5259: mov       QWORD PTR [rsp+0x50],rax
000F525E: xor       r8d,r8d
000F5261: mov       rdx,rbx
000F5264: mov       rcx,rax
000F5267: call      0x1800fddb0
000F526C: nop       
000F526D: mov       rcx,QWORD PTR [rdi+0x48]
000F5271: mov       QWORD PTR [rdi+0x48],rax
000F5275: test      rcx,rcx
000F5278: je        0x1800f5285
000F527A: mov       rax,QWORD PTR [rcx]
000F527D: mov       edx,0x1
000F5282: call      QWORD PTR [rax+0x48]
000F5285: mov       ecx,DWORD PTR [rip+0x111fe11]        # 0x18121509c
000F528B: mov       rax,QWORD PTR gs:0x58
000F5294: mov       edx,0x18
000F5299: mov       rax,QWORD PTR [rax+rcx*8]
000F529D: mov       eax,DWORD PTR [rdx+rax*1]
000F52A0: cmp       DWORD PTR [rip+0x112ae8e],eax        # 0x181220134
000F52A6: jle       0x1800f52d6
000F52A8: lea       rcx,[rip+0x112ae85]        # 0x181220134
000F52AF: call      0x18010c8d4
000F52B4: cmp       DWORD PTR [rip+0x112ae79],0xffffffff        # 0x181220134
000F52BB: jne       0x1800f52d6
000F52BD: lea       rcx,[rip+0x112ae7c]        # 0x181220140
000F52C4: call      0x1800f3de0
000F52C9: nop       
000F52CA: lea       rcx,[rip+0x112ae63]        # 0x181220134
000F52D1: call      0x18010c868
000F52D6: cmp       BYTE PTR [rip+0x112af63],0x0        # 0x181220240
000F52DD: jne       0x1800f52eb
000F52DF: lea       rcx,[rip+0x112ae5a]        # 0x181220140
000F52E6: call      0x1800f3ec0
000F52EB: cmp       BYTE PTR [rip+0x112af50],0x0        # 0x181220242
000F52F2: je        0x1800f5323
000F52F4: mov       rax,QWORD PTR [rdi]
000F52F7: mov       rcx,rdi
000F52FA: call      QWORD PTR [rax+0x108]
000F5300: mov       rbx,rax
000F5303: mov       ecx,0x4f0
000F5308: call      0x18010c290
000F530D: mov       QWORD PTR [rsp+0x50],rax
000F5312: xor       r8d,r8d
000F5315: mov       rdx,rbx
000F5318: mov       rcx,rax
000F531B: call      0x1800fddb0
000F5320: nop       
000F5321: jmp       0x1800f538d
000F5323: call      0x1800cc790
000F5328: mov       rbx,rax
000F532B: cmp       BYTE PTR [rax+0xca],0x0
000F5332: jne       0x1800f533c
000F5334: mov       rcx,rax
000F5337: call      0x1800cc8c0
000F533C: cmp       BYTE PTR [rbx+0xc9],0x0
000F5343: je        0x1800f53a5
000F5345: mov       rax,QWORD PTR [rdi]
000F5348: mov       rcx,rdi
000F534B: call      QWORD PTR [rax+0x108]
000F5351: mov       rbx,rax
000F5354: mov       ecx,0x190
000F5359: call      0x18010c290
000F535E: mov       QWORD PTR [rsp+0x50],rax
000F5363: mov       ecx,DWORD PTR [rdi+0x60]
000F5366: mov       DWORD PTR [rsp+0x30],ecx
000F536A: mov       DWORD PTR [rsp+0x28],0x0
000F5372: mov       QWORD PTR [rsp+0x20],0x0
000F537B: xor       r9d,r9d
000F537E: xor       r8d,r8d
000F5381: mov       rdx,rbx
000F5384: mov       rcx,rax
000F5387: call      0x1801003e0
000F538C: nop       
000F538D: mov       rcx,QWORD PTR [rdi+0x48]
000F5391: mov       QWORD PTR [rdi+0x48],rax
000F5395: test      rcx,rcx
000F5398: je        0x1800f53a5
000F539A: mov       rax,QWORD PTR [rcx]
000F539D: mov       edx,0x1
000F53A2: call      QWORD PTR [rax+0x48]
000F53A5: mov       rcx,QWORD PTR [rdi+0x48]
000F53A9: test      rcx,rcx
000F53AC: je        0x1800f53b4
000F53AE: mov       eax,DWORD PTR [rdi+0x68]
000F53B1: mov       DWORD PTR [rcx+0x4c],eax
000F53B4: mov       rbx,QWORD PTR [rsp+0x58]
000F53B9: add       rsp,0x40
000F53BD: pop       rdi
000F53BE: ret       
