; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x213420..0x213650; unnamed
00213421: movss     xmm6,DWORD PTR [r13+0x20]
00213427: mov       edx,r12d
0021342A: movss     xmm7,DWORD PTR [r13+0x24]
00213430: mov       r9d,r12d
00213433: mov       DWORD PTR [rsp+0x34],ecx
00213437: mov       DWORD PTR [rsp+0x38],edx
0021343B: mov       DWORD PTR [rsp+0x30],r12d
00213440: jle       0x18021359d
00213446: mov       r8,r12
00213449: mov       QWORD PTR [rsp+0x40],r12
0021344E: xchg      ax,ax
00213450: mov       rax,QWORD PTR [r13+0x18]
00213454: mov       r14d,r12d
00213457: mov       rsi,QWORD PTR [r8+rax*1]
0021345B: cmp       DWORD PTR [rsi],0x0
0021345E: jle       0x180213574
00213464: mov       r15,r12
00213467: mov       r12d,DWORD PTR [rsp+0x34]
0021346C: nop       DWORD PTR [rax+0x0]
00213470: mov       rbx,QWORD PTR [rsi+0x8]
00213474: add       rbx,r15
00213477: mov       rax,QWORD PTR [rbx+0x28]
0021347B: test      rax,rax
0021347E: je        0x1802134a3
00213480: cmp       rax,0xfffffffffffffff8
00213484: jne       0x180213496
00213486: mov       rdx,rdi
00213489: mov       rcx,r13
0021348C: call      0x180213a50
00213491: jmp       0x18021354f
00213496: mov       rdx,rbx
00213499: mov       rcx,rsi
0021349C: call      rax
0021349E: jmp       0x18021354f
002134A3: movss     xmm2,DWORD PTR [rbx]
002134A7: movss     xmm3,DWORD PTR [rbx+0x8]
002134AC: subss     xmm2,xmm6
002134B0: movss     xmm0,DWORD PTR [rbx+0x4]
002134B5: subss     xmm3,xmm6
002134B9: movss     xmm1,DWORD PTR [rbx+0xc]
002134BE: subss     xmm0,xmm7
002134C2: subss     xmm1,xmm7
002134C6: comiss    xmm2,xmm3
002134C9: jae       0x18021354f
002134CF: comiss    xmm0,xmm1
002134D2: jae       0x18021354f
002134D4: cvttss2si eax,xmm2
002134D8: lea       r8,[rbp+0x1aa0]
002134DF: mov       edx,0x1
002134E4: mov       rcx,rdi
002134E7: mov       DWORD PTR [rbp+0x1aa0],eax
002134ED: cvttss2si eax,xmm0
002134F1: mov       DWORD PTR [rbp+0x1aa4],eax
002134F7: cvttss2si eax,xmm3
002134FB: mov       DWORD PTR [rbp+0x1aa8],eax
00213501: cvttss2si eax,xmm1
00213505: mov       DWORD PTR [rbp+0x1aac],eax
0021350B: mov       rax,QWORD PTR [rdi]
0021350E: call      QWORD PTR [rax+0x168]
00213514: mov       rax,QWORD PTR [rbx+0x10]
00213518: lea       r9,[rsp+0x48]
0021351D: mov       QWORD PTR [rsp+0x48],rax
00213522: xor       edx,edx
00213524: mov       rax,QWORD PTR [rdi]
00213527: mov       r8d,0x1
0021352D: mov       rcx,rdi
00213530: call      QWORD PTR [rax+0x40]
00213533: mov       r9d,DWORD PTR [rsp+0x38]
00213538: mov       rcx,rdi
0021353B: mov       r8d,DWORD PTR [rbx+0x1c]
0021353F: mov       rax,QWORD PTR [rdi]
00213542: add       r8d,r12d
00213545: add       r9d,DWORD PTR [rbx+0x18]
00213549: mov       edx,DWORD PTR [rbx+0x20]
0021354C: call      QWORD PTR [rax+0x60]
0021354F: inc       r14d
00213552: add       r15,0x38
00213556: cmp       r14d,DWORD PTR [rsi]
00213559: jl        0x180213470
0021355F: mov       ecx,DWORD PTR [rsp+0x34]
00213563: xor       r12d,r12d
00213566: mov       edx,DWORD PTR [rsp+0x38]
0021356A: mov       r8,QWORD PTR [rsp+0x40]
0021356F: mov       r9d,DWORD PTR [rsp+0x30]
00213574: add       ecx,DWORD PTR [rsi+0x10]
00213577: inc       r9d
0021357A: add       edx,DWORD PTR [rsi+0x20]
0021357D: add       r8,0x8
00213581: mov       DWORD PTR [rsp+0x34],ecx
00213585: mov       DWORD PTR [rsp+0x38],edx
00213589: mov       DWORD PTR [rsp+0x30],r9d
0021358E: mov       QWORD PTR [rsp+0x40],r8
00213593: cmp       r9d,DWORD PTR [r13+0x4]
00213597: jl        0x180213450
0021359D: mov       rax,QWORD PTR [rdi]
002135A0: lea       r8,[rsp+0x78]
002135A5: mov       edx,DWORD PTR [rsp+0x70]
002135A9: mov       rcx,rdi
002135AC: call      QWORD PTR [rax+0x168]
002135B2: mov       rax,QWORD PTR [rdi]
002135B5: lea       r8,[rbp+0x78]
002135B9: mov       edx,DWORD PTR [rsp+0x74]
002135BD: mov       rcx,rdi
002135C0: call      QWORD PTR [rax+0x160]
002135C6: mov       rax,QWORD PTR [rdi]
002135C9: mov       rcx,rdi
002135CC: mov       rdx,QWORD PTR [rbp+0x1f8]
002135D3: call      QWORD PTR [rax+0x158]
002135D9: mov       rcx,QWORD PTR [rbp+0x1f8]
002135E0: movaps    xmm7,XMMWORD PTR [rsp+0x1bc0]
002135E8: movaps    xmm6,XMMWORD PTR [rsp+0x1bd0]
002135F0: test      rcx,rcx
002135F3: je        0x1802135fb
002135F5: mov       rax,QWORD PTR [rcx]
002135F8: call      QWORD PTR [rax+0x10]
002135FB: mov       rax,QWORD PTR [rdi]
002135FE: lea       r8,[rbp+0x208]
00213605: mov       r9d,DWORD PTR [rbp+0x218]
0021360C: mov       rcx,rdi
0021360F: mov       rdx,QWORD PTR [rbp+0x200]
00213616: call      QWORD PTR [rax+0x118]
0021361C: mov       rcx,QWORD PTR [rbp+0x200]
00213623: test      rcx,rcx
00213626: je        0x18021362e
00213628: mov       rax,QWORD PTR [rcx]
0021362B: call      QWORD PTR [rax+0x10]
0021362E: mov       rax,QWORD PTR [rdi]
00213631: mov       rcx,rdi
00213634: mov       r8d,DWORD PTR [rbp+0x21c]
0021363B: mov       rdx,QWORD PTR [rbp+0x220]
00213642: call      QWORD PTR [rax+0x120]
00213648: mov       rcx,QWORD PTR [rbp+0x220]
0021364F: test      rcx,rcx
