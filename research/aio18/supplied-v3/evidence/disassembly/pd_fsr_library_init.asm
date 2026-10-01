; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xCC8C0..0xCCC72; unnamed
000CC8C0: mov       QWORD PTR [rsp+0x8],rbx
000CC8C5: mov       QWORD PTR [rsp+0x10],rbp
000CC8CA: mov       QWORD PTR [rsp+0x18],rsi
000CC8CF: push      rdi
000CC8D0: push      r14
000CC8D2: push      r15
000CC8D4: sub       rsp,0x20
000CC8D8: mov       rbx,rcx
000CC8DB: lea       rcx,[rip+0x10dacce]        # 0x1811a75b0 ; 'amd_fidelityfx_dx11.dll'
000CC8E2: call      QWORD PTR [rip+0x46830]        # 0x180113118 ; KERNEL32.dll!GetModuleHandleW
000CC8E8: mov       rdi,rax
000CC8EB: test      rax,rax
000CC8EE: jne       0x1800cc905
000CC8F0: lea       rcx,[rip+0x10dad09]        # 0x1811a7600
000CC8F7: call      QWORD PTR [rip+0x4681b]        # 0x180113118 ; KERNEL32.dll!GetModuleHandleW
000CC8FD: mov       rdi,rax
000CC900: test      rax,rax
000CC903: je        0x1800cc911
000CC905: lea       rcx,[rip+0x10dacd4]        # 0x1811a75e0 ; 'amd_fidelityfx_dx11.dll loaded.'
000CC90C: call      0x1800fbb40
000CC911: lea       rcx,[rip+0x10dad58]        # 0x1811a7670 ; 'amd_fidelityfx_dx12.dll'
000CC918: call      QWORD PTR [rip+0x467fa]        # 0x180113118 ; KERNEL32.dll!GetModuleHandleW
000CC91E: mov       rbp,rax
000CC921: test      rax,rax
000CC924: jne       0x1800cc93b
000CC926: lea       rcx,[rip+0x10dad0b]        # 0x1811a7638 ; 'amd_fidelityfx_dx12d.dll'
000CC92D: call      QWORD PTR [rip+0x467e5]        # 0x180113118 ; KERNEL32.dll!GetModuleHandleW
000CC933: mov       rbp,rax
000CC936: test      rax,rax
000CC939: je        0x1800cc947
000CC93B: lea       rcx,[rip+0x10dad9e]        # 0x1811a76e0 ; 'amd_fidelityfx_dx12.dll loaded.'
000CC942: call      0x1800fbb40
000CC947: lea       rcx,[rip+0x10dad52]        # 0x1811a76a0 ; 'amd_fidelityfx_loader_dx12.dll'
000CC94E: call      QWORD PTR [rip+0x467c4]        # 0x180113118 ; KERNEL32.dll!GetModuleHandleW
000CC954: mov       r14,rax
000CC957: test      rax,rax
000CC95A: jne       0x1800cc971
000CC95C: lea       rcx,[rip+0x10dadcd]        # 0x1811a7730 ; 'amd_fidelityfx_loader_dx12d.dll'
000CC963: call      QWORD PTR [rip+0x467af]        # 0x180113118 ; KERNEL32.dll!GetModuleHandleW
000CC969: mov       r14,rax
000CC96C: test      rax,rax
000CC96F: je        0x1800cc97d
000CC971: lea       rcx,[rip+0x10dad88]        # 0x1811a7700 ; 'amd_fidelityfx_loader_dx12.dll loaded.'
000CC978: call      0x1800fbb40
000CC97D: lea       rcx,[rip+0x10dae1c]        # 0x1811a77a0 ; 'amd_fidelityfx_vk.dll'
000CC984: call      QWORD PTR [rip+0x4678e]        # 0x180113118 ; KERNEL32.dll!GetModuleHandleW
000CC98A: mov       rsi,rax
000CC98D: test      rax,rax
000CC990: jne       0x1800cc9a7
000CC992: lea       rcx,[rip+0x10dadd7]        # 0x1811a7770 ; 'amd_fidelityfx_vkd.dll'
000CC999: call      QWORD PTR [rip+0x46779]        # 0x180113118 ; KERNEL32.dll!GetModuleHandleW
000CC99F: mov       rsi,rax
000CC9A2: test      rax,rax
000CC9A5: je        0x1800cc9b3
000CC9A7: lea       rcx,[rip+0x10dae32]        # 0x1811a77e0 ; 'amd_fidelityfx_vk.dll loaded.'
000CC9AE: call      0x1800fbb40
000CC9B3: mov       r15,rdi
000CC9B6: test      rdi,rdi
000CC9B9: je        0x1800cca44
000CC9BF: lea       rdx,[rip+0x10dae0a]        # 0x1811a77d0 ; 'ffxConfigure'
000CC9C6: mov       rcx,rdi
000CC9C9: call      QWORD PTR [rip+0x46709]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000CC9CF: lea       rdx,[rip+0x10dae42]        # 0x1811a7818 ; 'ffxCreateContext'
000CC9D6: mov       rcx,rdi
000CC9D9: mov       QWORD PTR [rbx+0x28],rax
000CC9DD: call      QWORD PTR [rip+0x466f5]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000CC9E3: lea       rdx,[rip+0x10dae16]        # 0x1811a7800 ; 'ffxDestroyContext'
000CC9EA: mov       rcx,rdi
000CC9ED: mov       QWORD PTR [rbx+0x30],rax
000CC9F1: call      QWORD PTR [rip+0x466e1]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000CC9F7: lea       rdx,[rip+0x10dae42]        # 0x1811a7840 ; 'ffxDispatch'
000CC9FE: mov       rcx,rdi
000CCA01: mov       QWORD PTR [rbx+0x38],rax
000CCA05: call      QWORD PTR [rip+0x466cd]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000CCA0B: lea       rdx,[rip+0x10dae1e]        # 0x1811a7830 ; 'ffxQuery'
000CCA12: mov       rcx,rdi
000CCA15: mov       QWORD PTR [rbx+0x40],rax
000CCA19: call      QWORD PTR [rip+0x466b9]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000CCA1F: xor       r8d,r8d
000CCA22: mov       DWORD PTR [rbx+0xd0],0x0
000CCA2C: mov       edx,0x3
000CCA31: mov       QWORD PTR [rbx+0x48],rax
000CCA35: mov       rcx,rbx
000CCA38: mov       BYTE PTR [rbx+0xca],0x1
000CCA3F: call      0x1800ccc80
000CCA44: test      rbp,rbp
000CCA47: je        0x1800ccad8
000CCA4D: lea       rdx,[rip+0x10dad7c]        # 0x1811a77d0 ; 'ffxConfigure'
000CCA54: mov       rcx,rbp
000CCA57: mov       r15,rbp
000CCA5A: call      QWORD PTR [rip+0x46678]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000CCA60: lea       rdx,[rip+0x10dadb1]        # 0x1811a7818 ; 'ffxCreateContext'
000CCA67: mov       rcx,rbp
000CCA6A: mov       QWORD PTR [rbx+0x50],rax
000CCA6E: call      QWORD PTR [rip+0x46664]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000CCA74: lea       rdx,[rip+0x10dad85]        # 0x1811a7800 ; 'ffxDestroyContext'
000CCA7B: mov       rcx,rbp
000CCA7E: mov       QWORD PTR [rbx+0x58],rax
000CCA82: call      QWORD PTR [rip+0x46650]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000CCA88: lea       rdx,[rip+0x10dadb1]        # 0x1811a7840 ; 'ffxDispatch'
000CCA8F: mov       rcx,rbp
000CCA92: mov       QWORD PTR [rbx+0x60],rax
000CCA96: call      QWORD PTR [rip+0x4663c]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000CCA9C: lea       rdx,[rip+0x10dad8d]        # 0x1811a7830 ; 'ffxQuery'
000CCAA3: mov       rcx,rbp
000CCAA6: mov       QWORD PTR [rbx+0x68],rax
000CCAAA: call      QWORD PTR [rip+0x46628]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000CCAB0: mov       edx,0x3
000CCAB5: mov       DWORD PTR [rbx+0xd0],0x1
000CCABF: mov       r8d,0x1
000CCAC5: mov       QWORD PTR [rbx+0x70],rax
000CCAC9: mov       rcx,rbx
000CCACC: mov       BYTE PTR [rbx+0xca],0x1
000CCAD3: call      0x1800ccc80
000CCAD8: test      r14,r14
000CCADB: je        0x1800ccb93
000CCAE1: lea       rdx,[rip+0x10dace8]        # 0x1811a77d0 ; 'ffxConfigure'
000CCAE8: mov       rcx,r14
000CCAEB: mov       r15,r14
000CCAEE: call      QWORD PTR [rip+0x465e4]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000CCAF4: lea       rdx,[rip+0x10dad1d]        # 0x1811a7818 ; 'ffxCreateContext'
000CCAFB: mov       rcx,r14
000CCAFE: mov       QWORD PTR [rbx+0x78],rax
000CCB02: call      QWORD PTR [rip+0x465d0]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000CCB08: lea       rdx,[rip+0x10dacf1]        # 0x1811a7800 ; 'ffxDestroyContext'
000CCB0F: mov       rcx,r14
000CCB12: mov       QWORD PTR [rbx+0x80],rax
000CCB19: call      QWORD PTR [rip+0x465b9]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000CCB1F: lea       rdx,[rip+0x10dad1a]        # 0x1811a7840 ; 'ffxDispatch'
000CCB26: mov       rcx,r14
000CCB29: mov       QWORD PTR [rbx+0x88],rax
000CCB30: call      QWORD PTR [rip+0x465a2]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000CCB36: lea       rdx,[rip+0x10dacf3]        # 0x1811a7830 ; 'ffxQuery'
000CCB3D: mov       rcx,r14
000CCB40: mov       QWORD PTR [rbx+0x90],rax
000CCB47: call      QWORD PTR [rip+0x4658b]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000CCB4D: mov       edx,0x4
000CCB52: mov       DWORD PTR [rbx+0xd0],0x1
000CCB5C: mov       r8d,0x1
000CCB62: mov       QWORD PTR [rbx+0x98],rax
000CCB69: mov       rcx,rbx
000CCB6C: mov       WORD PTR [rbx+0xca],0x101
000CCB75: call      0x1800ccc80
000CCB7A: lea       rcx,[rip+0x10dacef]        # 0x1811a7870 ; 'amd_fidelityfx_framegeneration_dx12.dll'
000CCB81: call      QWORD PTR [rip+0x46591]        # 0x180113118 ; KERNEL32.dll!GetModuleHandleW
000CCB87: test      rax,rax
000CCB8A: setne     al
000CCB8D: mov       BYTE PTR [rbx+0xcc],al
000CCB93: test      rsi,rsi
000CCB96: je        0x1800ccc36
000CCB9C: lea       rdx,[rip+0x10dac2d]        # 0x1811a77d0 ; 'ffxConfigure'
000CCBA3: mov       rcx,rsi
000CCBA6: mov       r15,rsi
000CCBA9: call      QWORD PTR [rip+0x46529]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000CCBAF: lea       rdx,[rip+0x10dac62]        # 0x1811a7818 ; 'ffxCreateContext'
000CCBB6: mov       rcx,rsi
000CCBB9: mov       QWORD PTR [rbx+0xa0],rax
000CCBC0: call      QWORD PTR [rip+0x46512]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000CCBC6: lea       rdx,[rip+0x10dac33]        # 0x1811a7800 ; 'ffxDestroyContext'
000CCBCD: mov       rcx,rsi
000CCBD0: mov       QWORD PTR [rbx+0xa8],rax
000CCBD7: call      QWORD PTR [rip+0x464fb]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000CCBDD: lea       rdx,[rip+0x10dac5c]        # 0x1811a7840 ; 'ffxDispatch'
000CCBE4: mov       rcx,rsi
000CCBE7: mov       QWORD PTR [rbx+0xb0],rax
000CCBEE: call      QWORD PTR [rip+0x464e4]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000CCBF4: lea       rdx,[rip+0x10dac35]        # 0x1811a7830 ; 'ffxQuery'
000CCBFB: mov       rcx,rsi
000CCBFE: mov       QWORD PTR [rbx+0xb8],rax
000CCC05: call      QWORD PTR [rip+0x464cd]        # 0x1801130d8 ; KERNEL32.dll!GetProcAddress
000CCC0B: mov       edx,0x3
000CCC10: mov       DWORD PTR [rbx+0xd0],0x2
000CCC1A: mov       r8d,0x2
000CCC20: mov       QWORD PTR [rbx+0xc0],rax
000CCC27: mov       rcx,rbx
000CCC2A: mov       BYTE PTR [rbx+0xca],0x1
000CCC31: call      0x1800ccc80
000CCC36: test      r15,r15
000CCC39: lea       rcx,[rip+0x10dac10]        # 0x1811a7850 ; 'FFXAPIInterface Inited!'
000CCC40: setne     al
000CCC43: test      rdi,rdi
000CCC46: mov       BYTE PTR [rbx+0xc8],al
000CCC4C: setne     al
000CCC4F: mov       BYTE PTR [rbx+0xc9],al
000CCC55: mov       rbx,QWORD PTR [rsp+0x40]
000CCC5A: mov       rbp,QWORD PTR [rsp+0x48]
000CCC5F: mov       rsi,QWORD PTR [rsp+0x50]
000CCC64: add       rsp,0x20
000CCC68: pop       r15
000CCC6A: pop       r14
000CCC6C: pop       rdi
000CCC6D: jmp       0x1800fbb40
