; amd_fidelityfx_framegeneration_dx12.dll SHA256=3f5e674a59b400756e98ef31fd583a4eb08ad567ec8dee362886bc96e55ed347
; ImageBase=0x180000000, RVAs in left column
; range 0xBAB0..0xBD02; TFrameInterpolationSwapChainDX12<IFrameInterpolationSwapChainDX12,FrameinterpolationPresentInfoExt,FfxFrameGenerationConfig,ffxCallbackDescFrameGenerationPresent,16>::Present
0000BAB0: mov       QWORD PTR [rsp+0x20],rbx
0000BAB5: push      rbp
0000BAB6: push      rsi
0000BAB7: push      r14
0000BAB9: sub       rsp,0x30
0000BABD: mov       r14,QWORD PTR [rcx+0x1a68]
0000BAC4: mov       esi,r8d
0000BAC7: mov       ebp,edx
0000BAC9: mov       rbx,rcx
0000BACC: test      r8b,0x1
0000BAD0: je        0x18000baea
0000BAD2: mov       rcx,QWORD PTR [rcx+0x38]
0000BAD6: mov       rax,QWORD PTR [rcx]
0000BAD9: mov       rbx,QWORD PTR [rsp+0x68]
0000BADE: add       rsp,0x30
0000BAE2: pop       r14
0000BAE4: pop       rsi
0000BAE5: pop       rbp
0000BAE6: rex.W     jmp QWORD PTR [rax+0x40]
0000BAEA: mov       rax,QWORD PTR [rcx]
0000BAED: lea       rdx,[rcx+0x1760]
0000BAF4: mov       QWORD PTR [rsp+0x50],rdi
0000BAF9: mov       QWORD PTR [rsp+0x58],r12
0000BAFE: mov       QWORD PTR [rsp+0x60],r15
0000BB03: call      QWORD PTR [rax+0x1b0]
0000BB09: lea       rcx,[rbx+0x17f8]
0000BB10: call      QWORD PTR [rip+0xfa5fa]        # 0x180106110 ; __imp_EnterCriticalSection | KERNEL32.dll!EnterCriticalSection
0000BB16: mov       rax,QWORD PTR [rbx]
0000BB19: mov       rcx,rbx
0000BB1C: call      QWORD PTR [rax+0x120]
0000BB22: cmp       BYTE PTR [rbx+0x1a55],0x0
0000BB29: mov       r15d,eax
0000BB2C: je        0x18000bb48
0000BB2E: cmp       QWORD PTR [rbx+0x1aa0],0x0
0000BB36: jne       0x18000bb43
0000BB38: cmp       QWORD PTR [rbx+r15*8+0x1890],0x0
0000BB41: je        0x18000bb48
0000BB43: mov       dil,0x1
0000BB46: jmp       0x18000bb4b
0000BB48: xor       dil,dil
0000BB4B: mov       rcx,QWORD PTR [rbx+0x1700]
0000BB52: xor       r9d,r9d
0000BB55: mov       rdx,r14
0000BB58: mov       BYTE PTR [rsp+0x20],0x0
0000BB5D: call      0x1800f4c30 ; waitForFenceValue
0000BB62: mov       rcx,QWORD PTR [rbx+0x16d0]
0000BB69: mov       r8,r14
0000BB6C: mov       rdx,QWORD PTR [rbx+0x1708]
0000BB73: mov       rax,QWORD PTR [rcx]
0000BB76: call      QWORD PTR [rax+0x78]
0000BB79: mov       rax,QWORD PTR [rbx]
0000BB7C: mov       rcx,rbx
0000BB7F: call      QWORD PTR [rax+0x170]
0000BB85: test      al,al
0000BB87: je        0x18000bba8
0000BB89: test      BYTE PTR [rbx+0x16b8],0x2
0000BB90: je        0x18000bba8
0000BB92: cmp       QWORD PTR [rbx+0x1688],0x0
0000BB9A: je        0x18000bba8
0000BB9C: mov       rax,QWORD PTR [rbx]
0000BB9F: mov       rcx,rbx
0000BBA2: call      QWORD PTR [rax+0x178]
0000BBA8: mov       BYTE PTR [rbx+0x1a57],dil
0000BBAF: test      dil,dil
0000BBB2: mov       rdi,QWORD PTR [rsp+0x50]
0000BBB7: je        0x18000bbe1
0000BBB9: mov       rcx,QWORD PTR [rbx+0x1718]
0000BBC0: mov       edx,0xffffffff
0000BBC5: call      QWORD PTR [rip+0xfa51d]        # 0x1801060e8 ; __imp_WaitForSingleObject | KERNEL32.dll!WaitForSingleObject
0000BBCB: mov       rax,QWORD PTR [rbx]
0000BBCE: mov       r8d,esi
0000BBD1: mov       edx,ebp
0000BBD3: mov       rcx,rbx
0000BBD6: call      QWORD PTR [rax+0x168]
0000BBDC: jmp       0x18000bc65
0000BBE1: mov       rcx,QWORD PTR [rbx+0x16d0]
0000BBE8: mov       rdx,QWORD PTR [rbx+0x16e0]
0000BBEF: mov       rax,QWORD PTR [rcx]
0000BBF2: inc       QWORD PTR [rbx+0x1870]
0000BBF9: mov       r8,QWORD PTR [rbx+0x1870]
0000BC00: call      QWORD PTR [rax+0x70]
0000BC03: mov       rcx,QWORD PTR [rbx+0x16d8]
0000BC0A: mov       r8,QWORD PTR [rbx+0x1870]
0000BC11: mov       rdx,QWORD PTR [rbx+0x16e0]
0000BC18: mov       rax,QWORD PTR [rcx]
0000BC1B: call      QWORD PTR [rax+0x78]
0000BC1E: mov       rax,QWORD PTR [rbx]
0000BC21: mov       r8d,esi
0000BC24: mov       edx,ebp
0000BC26: mov       rcx,rbx
0000BC29: call      QWORD PTR [rax+0x158]
0000BC2F: mov       r8,QWORD PTR [rbx+0x1860]
0000BC36: test      r8,r8
0000BC39: je        0x18000bc65
0000BC3B: mov       r9,QWORD PTR [rbx+0x1a68]
0000BC42: xor       edx,edx
0000BC44: mov       r10d,DWORD PTR [rbx+0x1854]
0000BC4B: mov       rax,r9
0000BC4E: mov       rcx,QWORD PTR [rbx+0x16f0]
0000BC55: sub       rax,r10
0000BC58: cmp       r9,r10
0000BC5B: cmovae    rdx,rax
0000BC5F: mov       rax,QWORD PTR [rcx]
0000BC62: call      QWORD PTR [rax+0x48]
0000BC65: mov       rax,QWORD PTR [rbx+0x1a68]
0000BC6C: mov       rcx,r15
0000BC6F: add       rcx,rcx
0000BC72: mov       QWORD PTR [rbx+rcx*8+0x1918],rax
0000BC7A: lea       rcx,[rbx+0x17f8]
0000BC81: mov       QWORD PTR [rbx+r15*8+0x1890],0x0
0000BC8D: inc       QWORD PTR [rbx+0x1a48]
0000BC94: mov       rax,QWORD PTR [rbx+0x1a48]
0000BC9B: mov       r8d,DWORD PTR [rbx+0x1848]
0000BCA2: mov       edx,eax
0000BCA4: and       edx,0x1
0000BCA7: mov       DWORD PTR [rbx+0x1a44],edx
0000BCAD: xor       edx,edx
0000BCAF: div       r8
0000BCB2: mov       DWORD PTR [rbx+0x1a40],edx
0000BCB8: call      QWORD PTR [rip+0xfa43a]        # 0x1801060f8 ; __imp_LeaveCriticalSection | KERNEL32.dll!LeaveCriticalSection
0000BCBE: movsxd    rdx,DWORD PTR [rbx+0x1a40]
0000BCC5: mov       r9,QWORD PTR [rbx+0x1750]
0000BCCC: add       rdx,rdx
0000BCCF: mov       rcx,QWORD PTR [rbx+0x16f8]
0000BCD6: mov       BYTE PTR [rsp+0x20],0x0
0000BCDB: mov       rdx,QWORD PTR [rbx+rdx*8+0x1918]
0000BCE3: call      0x1800f4c30 ; waitForFenceValue
0000BCE8: mov       r15,QWORD PTR [rsp+0x60]
0000BCED: xor       eax,eax
0000BCEF: mov       r12,QWORD PTR [rsp+0x58]
0000BCF4: mov       rbx,QWORD PTR [rsp+0x68]
0000BCF9: add       rsp,0x30
0000BCFD: pop       r14
0000BCFF: pop       rsi
0000BD00: pop       rbp
0000BD01: ret       
