; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; Local Capstone decode, RVAs in addresses and branch operands; unwind range 0x2a1240..0x2a15a4
002a1240: mov rax, rsp
002a1243: mov qword ptr [rax + 8], rbx
002a1247: mov qword ptr [rax + 0x10], rbp
002a124b: mov qword ptr [rax + 0x18], rdi
002a124f: push r12
002a1251: push r14
002a1253: push r15
002a1255: sub rsp, 0x90
002a125c: vmovaps xmmword ptr [rax - 0x28], xmm6
002a1261: vmovss xmm6, dword ptr [rcx + 0x1618]
002a1269: movsxd rbp, edx
002a126c: mov rdi, rcx
002a126f: movsxd r14, r9d
002a1272: movsxd r15, r8d
002a1275: vmovaps xmmword ptr [rax - 0x38], xmm7
002a127a: vmovss xmm7, dword ptr [rcx + 0x161c]
002a1282: cmp ebp, 2
002a1285: jne 0x2a12a5
002a1287: cmp byte ptr [rsp + 0x110], 0
002a128f: jne 0x2a12a5
002a1291: mov dword ptr [rcx + 0x1618], 0x3f800000
002a129b: mov dword ptr [rcx + 0x161c], 0x3f800000
002a12a5: mov rcx, qword ptr [rcx + 0x1680]
002a12ac: lea rdx, [rdi + 0x1610]
002a12b3: xor r12d, r12d
002a12b6: xor r9d, r9d
002a12b9: mov dword ptr [rsp + 0x30], r12d
002a12be: xor r8d, r8d
002a12c1: mov dword ptr [rsp + 0x28], r12d
002a12c6: mov rax, qword ptr [rcx]
002a12c9: mov qword ptr [rsp + 0x20], rdx
002a12ce: mov rdx, qword ptr [rdi + 0x1608]
002a12d5: call qword ptr [rax + 0x180]
002a12db: mov rcx, qword ptr [rdi + 0x1680]
002a12e2: lea r9, [rdi + 0x1608]
002a12e9: xor edx, edx
002a12eb: mov r8d, 1
002a12f1: mov rax, qword ptr [rcx]
002a12f4: call qword ptr [rax + 0x80]
002a12fa: mov rax, qword ptr [rsp + 0xe8]
002a1302: test rax, rax
002a1305: je 0x2a1342
002a1307: mov rcx, qword ptr [rsp + 0x118]
002a130f: test rcx, rcx
002a1312: je 0x2a1342
002a1314: mov r9, qword ptr [rsp + 0xe0]
002a131c: lea r8, [rsp + 0x40]
002a1321: mov qword ptr [rsp + 0x48], rcx
002a1326: mov edx, 2
002a132b: mov rcx, qword ptr [rdi + 0x1680]
002a1332: mov qword ptr [rsp + 0x40], rax
002a1337: mov rax, qword ptr [rcx]
002a133a: call qword ptr [rax + 0x108]
002a1340: jmp 0x2a1374
002a1342: mov rcx, qword ptr [rdi + 0x1680]
002a1349: mov r9, qword ptr [rsp + 0xe0]
002a1351: mov r10, qword ptr [rcx]
002a1354: test rax, rax
002a1357: je 0x2a1368
002a1359: lea r8, [rsp + 0xe8]
002a1361: mov edx, 1
002a1366: jmp 0x2a136d
002a1368: xor r8d, r8d
002a136b: xor edx, edx
002a136d: call qword ptr [r10 + 0x108]
002a1374: mov rcx, qword ptr [rdi + 0x1680]
002a137b: mov r9d, 0xffffffff
002a1381: mov rdx, qword ptr [rdi + r14*8 + 0x15f0]
002a1389: xor r8d, r8d
002a138c: mov rax, qword ptr [rcx]
002a138f: call qword ptr [rax + 0x118]
002a1395: mov rcx, qword ptr [rdi + 0x1680]
002a139c: xor r8d, r8d
002a139f: mov rdx, qword ptr [rdi + 0x1668]
002a13a6: mov rax, qword ptr [rcx]
002a13a9: call qword ptr [rax + 0x120]
002a13af: mov rcx, qword ptr [rdi + 0x1680]
002a13b6: xor r9d, r9d
002a13b9: mov rdx, qword ptr [rdi + 0x1598]
002a13c0: xor r8d, r8d
002a13c3: mov rax, qword ptr [rcx]
002a13c6: call qword ptr [rax + 0x58]
002a13c9: mov rcx, qword ptr [rdi + 0x1680]
002a13d0: xor r9d, r9d
002a13d3: mov rdx, qword ptr [rdi + rbp*8 + 0x15a0]
002a13db: xor r8d, r8d
002a13de: mov rax, qword ptr [rcx]
002a13e1: call qword ptr [rax + 0x48]
002a13e4: mov rcx, qword ptr [rdi + 0x1680]
002a13eb: xor edx, edx
002a13ed: mov r9, qword ptr [rsp + 0xd8]
002a13f5: mov r8d, dword ptr [rsp + 0xd0]
002a13fd: mov rax, qword ptr [rcx]
002a1400: call qword ptr [rax + 0x40]
002a1403: mov rcx, qword ptr [rdi + 0x1680]
002a140a: add r15, 0x2bb
002a1411: xor edx, edx
002a1413: mov r8d, 1
002a1419: mov rax, qword ptr [rcx]
002a141c: lea r9, [rdi + r15*8]
002a1420: call qword ptr [rax + 0x50]
002a1423: mov rcx, qword ptr [rdi + 0x1680]
002a142a: xor r9d, r9d
002a142d: xor r8d, r8d
002a1430: xor edx, edx
002a1432: mov rax, qword ptr [rcx]
002a1435: call qword ptr [rax + 0x98]
002a143b: mov rcx, qword ptr [rdi + 0x1680]
002a1442: xor r9d, r9d
002a1445: mov qword ptr [rsp + 0x28], r12
002a144a: xor r8d, r8d
002a144d: xor edx, edx
002a144f: mov qword ptr [rsp + 0x20], r12
002a1454: mov rax, qword ptr [rcx]
002a1457: call qword ptr [rax + 0x90]
002a145d: mov rcx, qword ptr [rdi + 0x1680]
002a1464: xor edx, edx
002a1466: mov rax, qword ptr [rcx]
002a1469: call qword ptr [rax + 0x88]
002a146f: mov rcx, qword ptr [rdi + 0x1680]
002a1476: mov edx, 4
002a147b: mov rax, qword ptr [rcx]
002a147e: call qword ptr [rax + 0xc0]
002a1484: mov rcx, qword ptr [rdi + 0x1680]
002a148b: lea r8, [rsp + 0x50]
002a1490: vxorps xmm0, xmm0, xmm0
002a1494: vcvtsi2ss xmm0, xmm0, dword ptr [rsp + 0x100]
002a149d: vxorps xmm1, xmm1, xmm1
002a14a1: vcvtsi2ss xmm1, xmm1, dword ptr [rsp + 0x108]
002a14aa: vmovss dword ptr [rsp + 0x50], xmm0
002a14b0: vmovss dword ptr [rsp + 0x54], xmm1
002a14b6: vxorps xmm0, xmm0, xmm0
002a14ba: vcvtsi2ss xmm0, xmm0, dword ptr [rsp + 0xf0]
002a14c3: vxorps xmm1, xmm1, xmm1
002a14c7: vcvtsi2ss xmm1, xmm1, dword ptr [rsp + 0xf8]
002a14d0: vmovss dword ptr [rsp + 0x58], xmm0
002a14d6: vmovss dword ptr [rsp + 0x5c], xmm1
002a14dc: vmovss xmm1, dword ptr [rip + 0x16a850]
002a14e4: vxorps xmm0, xmm0, xmm0
002a14e8: vmovss dword ptr [rsp + 0x60], xmm0
002a14ee: vmovss dword ptr [rsp + 0x64], xmm1
002a14f4: mov rax, qword ptr [rcx]
002a14f7: mov edx, 1
002a14fc: call qword ptr [rax + 0x160]
002a1502: mov rcx, qword ptr [rdi + 0x1680]
002a1509: mov rax, qword ptr [rcx]
002a150c: mov rdx, qword ptr [rdi + 0x15e8]
002a1513: call qword ptr [rax + 0x158]
002a1519: mov rcx, qword ptr [rdi + 0x1680]
002a1520: xor r8d, r8d
002a1523: mov edx, 3
002a1528: mov rax, qword ptr [rcx]
002a152b: call qword ptr [rax + 0x68]
002a152e: mov rcx, qword ptr [rdi + 0x1680]
002a1535: xor r9d, r9d
002a1538: xor r8d, r8d
002a153b: xor edx, edx
002a153d: mov rax, qword ptr [rcx]
002a1540: call qword ptr [rax + 0x108]
002a1546: mov rcx, qword ptr [rdi + 0x1680]
002a154d: xor r9d, r9d
002a1550: xor r8d, r8d
002a1553: xor edx, edx
002a1555: mov rax, qword ptr [rcx]
002a1558: call qword ptr [rax + 0x40]
002a155b: cmp ebp, 2
002a155e: jne 0x2a157a
002a1560: cmp byte ptr [rsp + 0x110], r12b
002a1568: jne 0x2a157a
002a156a: vmovss dword ptr [rdi + 0x1618], xmm6
002a1572: vmovss dword ptr [rdi + 0x161c], xmm7
002a157a: vmovaps xmm7, xmmword ptr [rsp + 0x70]
002a1580: lea r11, [rsp + 0x90]
002a1588: mov rbx, qword ptr [r11 + 0x20]
002a158c: mov rbp, qword ptr [r11 + 0x28]
002a1590: mov rdi, qword ptr [r11 + 0x30]
002a1594: vmovaps xmm6, xmmword ptr [r11 - 0x10]
002a159a: mov rsp, r11
002a159d: pop r15
002a159f: pop r14
002a15a1: pop r12
002a15a3: ret 
