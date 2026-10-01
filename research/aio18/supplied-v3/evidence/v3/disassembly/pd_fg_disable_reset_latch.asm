; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xF0320..0xF03F0; unnamed
000F0320: mov       QWORD PTR [rsp+0x8],rbx
000F0325: push      rdi
000F0326: sub       rsp,0x20
000F032A: movzx     edi,dl
000F032D: mov       rbx,rcx
000F0330: call      0x1800cc790
000F0335: mov       r8d,DWORD PTR [rbx+0x8]
000F0339: mov       rcx,rax
000F033C: mov       edx,DWORD PTR [rbx+0x148]
000F0342: call      0x1800ccc80
000F0347: cmp       DWORD PTR [rbx+0x8],0x1
000F034B: mov       BYTE PTR [rbx+0x144],dil
000F0352: mov       BYTE PTR [rbx+0x1c8],0x0
000F0359: jne       0x1800f0372
000F035B: test      dil,dil
000F035E: jne       0x1800f0369
000F0360: mov       BYTE PTR [rbx+0x1c9],0x1
000F0367: jmp       0x1800f0372
000F0369: cmp       BYTE PTR [rbx+0x1c9],0x0
000F0370: jne       0x1800f03e5
000F0372: mov       rax,QWORD PTR [rbx]
000F0375: mov       rcx,rbx
000F0378: call      QWORD PTR [rax+0x50]
000F037B: test      al,al
000F037D: je        0x1800f03d9
000F037F: lea       rcx,[rip+0x1109a4a]        # 0x1811f9dd0
000F0386: mov       BYTE PTR [rip+0x1109a7b],dil        # 0x1811f9e08
000F038D: call      0x1800f0b60
000F0392: mov       rbx,QWORD PTR [rip+0x112605f]        # 0x1812163f8
000F0399: mov       rdi,rax
000F039C: cmp       BYTE PTR [rbx+0xca],0x0
000F03A3: jne       0x1800f03ad
000F03A5: mov       rcx,rbx
000F03A8: call      0x1800cc8c0
000F03AD: mov       r8,QWORD PTR [rbx]
000F03B0: lea       rcx,[rip+0x112f611]        # 0x18121f9c8
000F03B7: mov       rdx,rdi
000F03BA: call      r8
000F03BD: test      eax,eax
000F03BF: je        0x1800f03e5
000F03C1: mov       edx,eax
000F03C3: lea       rcx,[rip+0x10b994e]        # 0x1811a9d18 ; "Couldn't set the FFXAPI FrameGen config: %d"
000F03CA: mov       rbx,QWORD PTR [rsp+0x30]
000F03CF: add       rsp,0x20
000F03D3: pop       rdi
000F03D4: jmp       0x1800fbb40
000F03D9: lea       rcx,[rip+0x10b9ae8]        # 0x1811a9ec8 ; 'FrameGenMethod_FSR3_FFXAPI is not initialized!'
000F03E0: call      0x1800fbb40
000F03E5: mov       rbx,QWORD PTR [rsp+0x30]
000F03EA: add       rsp,0x20
000F03EE: pop       rdi
000F03EF: ret       
