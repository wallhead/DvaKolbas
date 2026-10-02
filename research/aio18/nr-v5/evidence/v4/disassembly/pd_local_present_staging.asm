
/mnt/data/aio18_nr_work/PDPerfPlugin.dll:	file format coff-x86-64

Disassembly of section .text:

0000000180078890 <SetPDFrameWarpNativeCameraSource>:
1800a12e0: 70 0f                       	jo	0x1800a12f1 <SetPDFrameWarpNativeCameraSource+0x28a61>
1800a12e2: 11 49 f0                    	adcl	%ecx, -0x10(%rcx)
1800a12e5: 48 8d bf 80 00 00 00        	leaq	0x80(%rdi), %rdi
1800a12ec: 48 83 e8 01                 	subq	$0x1, %rax
1800a12f0: 75 ae                       	jne	0x1800a12a0 <SetPDFrameWarpNativeCameraSource+0x28a10>
1800a12f2: 0f 10 07                    	movups	(%rdi), %xmm0
1800a12f5: 0f 11 01                    	movups	%xmm0, (%rcx)
1800a12f8: 0f 10 4f 10                 	movups	0x10(%rdi), %xmm1
1800a12fc: 0f 11 49 10                 	movups	%xmm1, 0x10(%rcx)
1800a1300: 0f 10 47 20                 	movups	0x20(%rdi), %xmm0
1800a1304: 0f 11 41 20                 	movups	%xmm0, 0x20(%rcx)
1800a1308: 48 8b 47 30                 	movq	0x30(%rdi), %rax
1800a130c: 48 89 41 30                 	movq	%rax, 0x30(%rcx)
1800a1310: 41 c6 86 b1 01 00 00 00     	movb	$0x0, 0x1b1(%r14)
1800a1318: 4d 8b 46 38                 	movq	0x38(%r14), %r8
1800a131c: 49 8b 40 08                 	movq	0x8(%r8), %rax
1800a1320: 49 8b c8                    	movq	%r8, %rcx
1800a1323: 8b 94 24 b0 00 00 00        	movl	0xb0(%rsp), %edx
1800a132a: 80 78 19 00                 	cmpb	$0x0, 0x19(%rax)
1800a132e: 75 17                       	jne	0x1800a1347 <SetPDFrameWarpNativeCameraSource+0x28ab7>
1800a1330: 39 50 20                    	cmpl	%edx, 0x20(%rax)
1800a1333: 7d 06                       	jge	0x1800a133b <SetPDFrameWarpNativeCameraSource+0x28aab>
1800a1335: 48 83 c0 10                 	addq	$0x10, %rax
1800a1339: eb 03                       	jmp	0x1800a133e <SetPDFrameWarpNativeCameraSource+0x28aae>
1800a133b: 48 8b c8                    	movq	%rax, %rcx
1800a133e: 48 8b 00                    	movq	(%rax), %rax
1800a1341: 80 78 19 00                 	cmpb	$0x0, 0x19(%rax)
1800a1345: 74 e9                       	je	0x1800a1330 <SetPDFrameWarpNativeCameraSource+0x28aa0>
1800a1347: 80 79 19 00                 	cmpb	$0x0, 0x19(%rcx)
1800a134b: 0f 85 f0 03 00 00           	jne	0x1800a1741 <SetPDFrameWarpNativeCameraSource+0x28eb1>
1800a1351: 3b 51 20                    	cmpl	0x20(%rcx), %edx
1800a1354: 0f 8c e7 03 00 00           	jl	0x1800a1741 <SetPDFrameWarpNativeCameraSource+0x28eb1>
1800a135a: 49 3b c8                    	cmpq	%r8, %rcx
1800a135d: 0f 84 de 03 00 00           	je	0x1800a1741 <SetPDFrameWarpNativeCameraSource+0x28eb1>
1800a1363: 48 8b 41 28                 	movq	0x28(%rcx), %rax
1800a1367: 48 89 84 24 98 00 00 00     	movq	%rax, 0x98(%rsp)
1800a136f: 48 85 c0                    	testq	%rax, %rax
1800a1372: 0f 84 c9 03 00 00           	je	0x1800a1741 <SetPDFrameWarpNativeCameraSource+0x28eb1>
1800a1378: 48 8b 0d 71 45 17 01        	movq	0x1174571(%rip), %rcx   # 0x1812158f0
1800a137f: 48 8b 91 c0 0b 00 00        	movq	0xbc0(%rcx), %rdx
1800a1386: 48 85 d2                    	testq	%rdx, %rdx
1800a1389: 74 15                       	je	0x1800a13a0 <SetPDFrameWarpNativeCameraSource+0x28b10>
1800a138b: 48 85 f6                    	testq	%rsi, %rsi
1800a138e: 74 10                       	je	0x1800a13a0 <SetPDFrameWarpNativeCameraSource+0x28b10>
1800a1390: 48 8b 89 20 0b 00 00        	movq	0xb20(%rcx), %rcx
1800a1397: 48 8b 01                    	movq	(%rcx), %rax
1800a139a: 4c 8b c6                    	movq	%rsi, %r8
1800a139d: ff 50 78                    	callq	*0x78(%rax)
1800a13a0: 48 8b d5                    	movq	%rbp, %rdx
1800a13a3: e8 f8 31 fd ff              	callq	0x1800745a0 <SetPDFrameWarpDiagnosticHud+0x4bb50>
1800a13a8: 48 8b f8                    	movq	%rax, %rdi
1800a13ab: 48 89 84 24 90 00 00 00     	movq	%rax, 0x90(%rsp)
1800a13b3: 48 85 c0                    	testq	%rax, %rax
1800a13b6: 75 1c                       	jne	0x1800a13d4 <SetPDFrameWarpNativeCameraSource+0x28b44>
1800a13b8: 45 8b c7                    	movl	%r15d, %r8d
1800a13bb: e8 00 31 fd ff              	callq	0x1800744c0 <SetPDFrameWarpDiagnosticHud+0x4ba70>
1800a13c0: 48 8b f8                    	movq	%rax, %rdi
1800a13c3: 48 89 84 24 90 00 00 00     	movq	%rax, 0x90(%rsp)
1800a13cb: 48 85 c0                    	testq	%rax, %rax
1800a13ce: 0f 84 6d 03 00 00           	je	0x1800a1741 <SetPDFrameWarpNativeCameraSource+0x28eb1>
1800a13d4: 48 8b d7                    	movq	%rdi, %rdx
1800a13d7: e8 84 2c fd ff              	callq	0x180074060 <SetPDFrameWarpDiagnosticHud+0x4b610>
1800a13dc: 48 89 84 24 88 00 00 00     	movq	%rax, 0x88(%rsp)
1800a13e4: 48 85 c0                    	testq	%rax, %rax
1800a13e7: 0f 84 54 03 00 00           	je	0x1800a1741 <SetPDFrameWarpNativeCameraSource+0x28eb1>
1800a13ed: e8 5e 32 fd ff              	callq	0x180074650 <SetPDFrameWarpDiagnosticHud+0x4bc00>
1800a13f2: 4c 8b e8                    	movq	%rax, %r13
1800a13f5: 48 85 c0                    	testq	%rax, %rax
1800a13f8: 75 11                       	jne	0x1800a140b <SetPDFrameWarpNativeCameraSource+0x28b7b>
1800a13fa: 48 8d 0d b7 36 10 01        	leaq	0x11036b7(%rip), %rcx   # 0x1811a4ab8
1800a1401: e8 3a a7 05 00              	callq	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
1800a1406: e9 36 03 00 00              	jmp	0x1800a1741 <SetPDFrameWarpNativeCameraSource+0x28eb1>
1800a140b: 4c 8b c7                    	movq	%rdi, %r8
1800a140e: 49 8b d5                    	movq	%r13, %rdx
1800a1411: e8 2a 2d fd ff              	callq	0x180074140 <SetPDFrameWarpDiagnosticHud+0x4b6f0>
1800a1416: 84 c0                       	testb	%al, %al
1800a1418: 75 0c                       	jne	0x1800a1426 <SetPDFrameWarpNativeCameraSource+0x28b96>
1800a141a: 48 8d 0d 6f 37 10 01        	leaq	0x110376f(%rip), %rcx   # 0x1811a4b90
1800a1421: e9 10 03 00 00              	jmp	0x1800a1736 <SetPDFrameWarpNativeCameraSource+0x28ea6>
1800a1426: 33 ff                       	xorl	%edi, %edi
1800a1428: 48 39 bc 24 c8 00 00 00     	cmpq	%rdi, 0xc8(%rsp)
1800a1430: 74 34                       	je	0x1800a1466 <SetPDFrameWarpNativeCameraSource+0x28bd6>
1800a1432: 48 8b 05 b7 44 17 01        	movq	0x11744b7(%rip), %rax   # 0x1812158f0
1800a1439: 48 8b b0 90 03 00 00        	movq	0x390(%rax), %rsi
1800a1440: 48 85 f6                    	testq	%rsi, %rsi
1800a1443: 74 21                       	je	0x1800a1466 <SetPDFrameWarpNativeCameraSource+0x28bd6>
1800a1445: 48 8b d6                    	movq	%rsi, %rdx
1800a1448: e8 13 2c fd ff              	callq	0x180074060 <SetPDFrameWarpDiagnosticHud+0x4b610>
1800a144d: 48 89 44 24 78              	movq	%rax, 0x78(%rsp)
1800a1452: 48 85 c0                    	testq	%rax, %rax
1800a1455: 74 0f                       	je	0x1800a1466 <SetPDFrameWarpNativeCameraSource+0x28bd6>
1800a1457: 4c 8b c6                    	movq	%rsi, %r8
1800a145a: 49 8b d5                    	movq	%r13, %rdx
1800a145d: e8 de 2c fd ff              	callq	0x180074140 <SetPDFrameWarpDiagnosticHud+0x4b6f0>
1800a1462: 84 c0                       	testb	%al, %al
1800a1464: 75 05                       	jne	0x1800a146b <SetPDFrameWarpNativeCameraSource+0x28bdb>
1800a1466: 48 89 7c 24 78              	movq	%rdi, 0x78(%rsp)
1800a146b: 48 83 bc 24 c0 00 00 00 00  	cmpq	$0x0, 0xc0(%rsp)
1800a1474: 74 37                       	je	0x1800a14ad <SetPDFrameWarpNativeCameraSource+0x28c1d>
1800a1476: 48 8b 05 73 44 17 01        	movq	0x1174473(%rip), %rax   # 0x1812158f0
1800a147d: 48 8b b0 98 03 00 00        	movq	0x398(%rax), %rsi
1800a1484: 48 85 f6                    	testq	%rsi, %rsi
1800a1487: 74 24                       	je	0x1800a14ad <SetPDFrameWarpNativeCameraSource+0x28c1d>
1800a1489: 48 8b d6                    	movq	%rsi, %rdx
1800a148c: e8 cf 2b fd ff              	callq	0x180074060 <SetPDFrameWarpDiagnosticHud+0x4b610>
1800a1491: 48 8b e8                    	movq	%rax, %rbp
1800a1494: 48 89 44 24 70              	movq	%rax, 0x70(%rsp)
1800a1499: 48 85 c0                    	testq	%rax, %rax
1800a149c: 74 0f                       	je	0x1800a14ad <SetPDFrameWarpNativeCameraSource+0x28c1d>
1800a149e: 4c 8b c6                    	movq	%rsi, %r8
1800a14a1: 49 8b d5                    	movq	%r13, %rdx
1800a14a4: e8 97 2c fd ff              	callq	0x180074140 <SetPDFrameWarpDiagnosticHud+0x4b6f0>
1800a14a9: 84 c0                       	testb	%al, %al
1800a14ab: 75 08                       	jne	0x1800a14b5 <SetPDFrameWarpNativeCameraSource+0x28c25>
1800a14ad: 48 8b ef                    	movq	%rdi, %rbp
1800a14b0: 48 89 7c 24 70              	movq	%rdi, 0x70(%rsp)
1800a14b5: 48 83 bc 24 d8 00 00 00 00  	cmpq	$0x0, 0xd8(%rsp)
1800a14be: 74 37                       	je	0x1800a14f7 <SetPDFrameWarpNativeCameraSource+0x28c67>
1800a14c0: 48 8b 05 29 44 17 01        	movq	0x1174429(%rip), %rax   # 0x1812158f0
1800a14c7: 48 8b b0 78 04 00 00        	movq	0x478(%rax), %rsi
1800a14ce: 48 85 f6                    	testq	%rsi, %rsi
1800a14d1: 74 24                       	je	0x1800a14f7 <SetPDFrameWarpNativeCameraSource+0x28c67>
1800a14d3: 48 8b d6                    	movq	%rsi, %rdx
1800a14d6: e8 85 2b fd ff              	callq	0x180074060 <SetPDFrameWarpDiagnosticHud+0x4b610>
1800a14db: 48 89 84 24 80 00 00 00     	movq	%rax, 0x80(%rsp)
1800a14e3: 48 85 c0                    	testq	%rax, %rax
1800a14e6: 74 0f                       	je	0x1800a14f7 <SetPDFrameWarpNativeCameraSource+0x28c67>
1800a14e8: 4c 8b c6                    	movq	%rsi, %r8
1800a14eb: 49 8b d5                    	movq	%r13, %rdx
1800a14ee: e8 4d 2c fd ff              	callq	0x180074140 <SetPDFrameWarpDiagnosticHud+0x4b6f0>
1800a14f3: 84 c0                       	testb	%al, %al
1800a14f5: 75 08                       	jne	0x1800a14ff <SetPDFrameWarpNativeCameraSource+0x28c6f>
1800a14f7: 48 89 bc 24 80 00 00 00     	movq	%rdi, 0x80(%rsp)
1800a14ff: 48 83 bc 24 e0 00 00 00 00  	cmpq	$0x0, 0xe0(%rsp)
1800a1508: 74 32                       	je	0x1800a153c <SetPDFrameWarpNativeCameraSource+0x28cac>
1800a150a: 48 8b 05 df 43 17 01        	movq	0x11743df(%rip), %rax   # 0x1812158f0
1800a1511: 48 8b b0 88 04 00 00        	movq	0x488(%rax), %rsi
1800a1518: 48 85 f6                    	testq	%rsi, %rsi
1800a151b: 74 1f                       	je	0x1800a153c <SetPDFrameWarpNativeCameraSource+0x28cac>
1800a151d: 48 8b d6                    	movq	%rsi, %rdx
1800a1520: e8 3b 2b fd ff              	callq	0x180074060 <SetPDFrameWarpDiagnosticHud+0x4b610>
1800a1525: 4c 8b f8                    	movq	%rax, %r15
1800a1528: 48 85 c0                    	testq	%rax, %rax
1800a152b: 74 0f                       	je	0x1800a153c <SetPDFrameWarpNativeCameraSource+0x28cac>
1800a152d: 4c 8b c6                    	movq	%rsi, %r8
1800a1530: 49 8b d5                    	movq	%r13, %rdx
1800a1533: e8 08 2c fd ff              	callq	0x180074140 <SetPDFrameWarpDiagnosticHud+0x4b6f0>
1800a1538: 84 c0                       	testb	%al, %al
1800a153a: 75 03                       	jne	0x1800a153f <SetPDFrameWarpNativeCameraSource+0x28caf>
1800a153c: 4c 8b ff                    	movq	%rdi, %r15
1800a153f: 48 83 bc 24 e8 00 00 00 00  	cmpq	$0x0, 0xe8(%rsp)
1800a1548: 74 32                       	je	0x1800a157c <SetPDFrameWarpNativeCameraSource+0x28cec>
1800a154a: 48 8b 05 9f 43 17 01        	movq	0x117439f(%rip), %rax   # 0x1812158f0
1800a1551: 48 8b b0 80 04 00 00        	movq	0x480(%rax), %rsi
1800a1558: 48 85 f6                    	testq	%rsi, %rsi
1800a155b: 74 1f                       	je	0x1800a157c <SetPDFrameWarpNativeCameraSource+0x28cec>
1800a155d: 48 8b d6                    	movq	%rsi, %rdx
1800a1560: e8 fb 2a fd ff              	callq	0x180074060 <SetPDFrameWarpDiagnosticHud+0x4b610>
1800a1565: 4c 8b e0                    	movq	%rax, %r12
1800a1568: 48 85 c0                    	testq	%rax, %rax
1800a156b: 74 0f                       	je	0x1800a157c <SetPDFrameWarpNativeCameraSource+0x28cec>
1800a156d: 4c 8b c6                    	movq	%rsi, %r8
1800a1570: 49 8b d5                    	movq	%r13, %rdx
1800a1573: e8 c8 2b fd ff              	callq	0x180074140 <SetPDFrameWarpDiagnosticHud+0x4b6f0>
1800a1578: 84 c0                       	testb	%al, %al
1800a157a: 75 03                       	jne	0x1800a157f <SetPDFrameWarpNativeCameraSource+0x28cef>
1800a157c: 4c 8b e7                    	movq	%rdi, %r12
1800a157f: 48 83 bc 24 f8 00 00 00 00  	cmpq	$0x0, 0xf8(%rsp)
1800a1588: 74 3a                       	je	0x1800a15c4 <SetPDFrameWarpNativeCameraSource+0x28d34>
1800a158a: 48 8b 05 5f 43 17 01        	movq	0x117435f(%rip), %rax   # 0x1812158f0
1800a1591: 48 8b b0 b0 04 00 00        	movq	0x4b0(%rax), %rsi
1800a1598: 48 85 f6                    	testq	%rsi, %rsi
1800a159b: 74 27                       	je	0x1800a15c4 <SetPDFrameWarpNativeCameraSource+0x28d34>
1800a159d: 48 8b d6                    	movq	%rsi, %rdx
1800a15a0: e8 bb 2a fd ff              	callq	0x180074060 <SetPDFrameWarpDiagnosticHud+0x4b610>
1800a15a5: 48 8b e8                    	movq	%rax, %rbp
1800a15a8: 48 85 c0                    	testq	%rax, %rax
1800a15ab: 74 12                       	je	0x1800a15bf <SetPDFrameWarpNativeCameraSource+0x28d2f>
1800a15ad: 4c 8b c6                    	movq	%rsi, %r8
1800a15b0: 49 8b d5                    	movq	%r13, %rdx
1800a15b3: e8 88 2b fd ff              	callq	0x180074140 <SetPDFrameWarpDiagnosticHud+0x4b6f0>
1800a15b8: 84 c0                       	testb	%al, %al
1800a15ba: 74 03                       	je	0x1800a15bf <SetPDFrameWarpNativeCameraSource+0x28d2f>
1800a15bc: 48 8b fd                    	movq	%rbp, %rdi
1800a15bf: 48 8b 6c 24 70              	movq	0x70(%rsp), %rbp
1800a15c4: 48 8b 44 24 78              	movq	0x78(%rsp), %rax
1800a15c9: 48 83 bc 24 c8 00 00 00 00  	cmpq	$0x0, 0xc8(%rsp)
1800a15d2: 74 05                       	je	0x1800a15d9 <SetPDFrameWarpNativeCameraSource+0x28d49>
1800a15d4: 48 85 c0                    	testq	%rax, %rax
1800a15d7: 74 10                       	je	0x1800a15e9 <SetPDFrameWarpNativeCameraSource+0x28d59>
1800a15d9: 48 83 bc 24 c0 00 00 00 00  	cmpq	$0x0, 0xc0(%rsp)
1800a15e2: 74 1c                       	je	0x1800a1600 <SetPDFrameWarpNativeCameraSource+0x28d70>
1800a15e4: 48 85 ed                    	testq	%rbp, %rbp
1800a15e7: 75 17                       	jne	0x1800a1600 <SetPDFrameWarpNativeCameraSource+0x28d70>
1800a15e9: 4c 8b c5                    	movq	%rbp, %r8
1800a15ec: 48 8b d0                    	movq	%rax, %rdx
1800a15ef: 48 8d 0d 3a 35 10 01        	leaq	0x110353a(%rip), %rcx   # 0x1811a4b30
1800a15f6: e8 45 a5 05 00              	callq	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
1800a15fb: e9 3b 01 00 00              	jmp	0x1800a173b <SetPDFrameWarpNativeCameraSource+0x28eab>
1800a1600: 80 bc 24 b3 01 00 00 00     	cmpb	$0x0, 0x1b3(%rsp)
1800a1608: 74 12                       	je	0x1800a161c <SetPDFrameWarpNativeCameraSource+0x28d8c>
1800a160a: 4d 85 ff                    	testq	%r15, %r15
1800a160d: 74 0d                       	je	0x1800a161c <SetPDFrameWarpNativeCameraSource+0x28d8c>
1800a160f: 49 8b ef                    	movq	%r15, %rbp
1800a1612: 48 8b b4 24 88 00 00 00     	movq	0x88(%rsp), %rsi
1800a161a: eb 2e                       	jmp	0x1800a164a <SetPDFrameWarpNativeCameraSource+0x28dba>
1800a161c: 48 8b b4 24 88 00 00 00     	movq	0x88(%rsp), %rsi
1800a1624: 48 8b d6                    	movq	%rsi, %rdx
1800a1627: 49 8b ce                    	movq	%r14, %rcx
1800a162a: e8 91 b4 ff ff              	callq	0x18009cac0 <SetPDFrameWarpNativeCameraSource+0x24230>
1800a162f: 48 8b e8                    	movq	%rax, %rbp
1800a1632: 48 85 c0                    	testq	%rax, %rax
1800a1635: 74 10                       	je	0x1800a1647 <SetPDFrameWarpNativeCameraSource+0x28db7>
1800a1637: 4c 8b c6                    	movq	%rsi, %r8
1800a163a: 49 8b d5                    	movq	%r13, %rdx
1800a163d: 49 8b ce                    	movq	%r14, %rcx
1800a1640: e8 db b6 ff ff              	callq	0x18009cd20 <SetPDFrameWarpNativeCameraSource+0x24490>
1800a1645: eb 03                       	jmp	0x1800a164a <SetPDFrameWarpNativeCameraSource+0x28dba>
1800a1647: 48 8b ee                    	movq	%rsi, %rbp
1800a164a: 49 8b ce                    	movq	%r14, %rcx
1800a164d: 48 8b d6                    	movq	%rsi, %rdx
1800a1650: e8 cb b5 ff ff              	callq	0x18009cc20 <SetPDFrameWarpNativeCameraSource+0x24390>
1800a1655: 48 8b f0                    	movq	%rax, %rsi
1800a1658: 48 85 c0                    	testq	%rax, %rax
1800a165b: 0f 84 ce 00 00 00           	je	0x1800a172f <SetPDFrameWarpNativeCameraSource+0x28e9f>
1800a1661: 48 8b d5                    	movq	%rbp, %rdx
1800a1664: 48 8b 0d 85 42 17 01        	movq	0x1174285(%rip), %rcx   # 0x1812158f0
1800a166b: e8 70 2f fd ff              	callq	0x1800745e0 <SetPDFrameWarpDiagnosticHud+0x4bb90>
1800a1670: 84 c0                       	testb	%al, %al
1800a1672: 0f 84 b7 00 00 00           	je	0x1800a172f <SetPDFrameWarpNativeCameraSource+0x28e9f>
1800a1678: 48 8b d6                    	movq	%rsi, %rdx
1800a167b: 48 8b 0d 6e 42 17 01        	movq	0x117426e(%rip), %rcx   # 0x1812158f0
1800a1682: e8 59 2f fd ff              	callq	0x1800745e0 <SetPDFrameWarpDiagnosticHud+0x4bb90>
1800a1687: 84 c0                       	testb	%al, %al
1800a1689: 0f 84 a0 00 00 00           	je	0x1800a172f <SetPDFrameWarpNativeCameraSource+0x28e9f>
1800a168f: 48 89 74 24 60              	movq	%rsi, 0x60(%rsp)
1800a1694: 48 89 7c 24 58              	movq	%rdi, 0x58(%rsp)
1800a1699: 4c 89 64 24 50              	movq	%r12, 0x50(%rsp)
1800a169e: 4c 89 7c 24 48              	movq	%r15, 0x48(%rsp)
1800a16a3: 48 8b 84 24 80 00 00 00     	movq	0x80(%rsp), %rax
1800a16ab: 48 89 44 24 40              	movq	%rax, 0x40(%rsp)
1800a16b0: 48 8b 44 24 70              	movq	0x70(%rsp), %rax
1800a16b5: 48 89 44 24 38              	movq	%rax, 0x38(%rsp)
1800a16ba: 48 8b 44 24 78              	movq	0x78(%rsp), %rax
1800a16bf: 48 89 44 24 30              	movq	%rax, 0x30(%rsp)
1800a16c4: 48 89 74 24 28              	movq	%rsi, 0x28(%rsp)
1800a16c9: 48 89 6c 24 20              	movq	%rbp, 0x20(%rsp)
1800a16ce: 4c 8d 8c 24 b0 00 00 00     	leaq	0xb0(%rsp), %r9
1800a16d6: 4c 8b 84 24 98 00 00 00     	movq	0x98(%rsp), %r8
1800a16de: 49 8b d5                    	movq	%r13, %rdx
1800a16e1: 49 8b ce                    	movq	%r14, %rcx
1800a16e4: e8 37 da ff ff              	callq	0x18009f120 <SetPDFrameWarpNativeCameraSource+0x26890>
1800a16e9: 84 c0                       	testb	%al, %al
1800a16eb: 75 16                       	jne	0x1800a1703 <SetPDFrameWarpNativeCameraSource+0x28e73>
1800a16ed: 41 c6 86 b4 01 00 00 01     	movb	$0x1, 0x1b4(%r14)
1800a16f5: 48 8d 0d d4 34 10 01        	leaq	0x11034d4(%rip), %rcx   # 0x1811a4bd0
1800a16fc: e8 ef ab ff ff              	callq	0x18009c2f0 <SetPDFrameWarpNativeCameraSource+0x23a60>
1800a1701: eb 38                       	jmp	0x1800a173b <SetPDFrameWarpNativeCameraSource+0x28eab>
1800a1703: 4c 8b ce                    	movq	%rsi, %r9
1800a1706: 4c 8b 84 24 90 00 00 00     	movq	0x90(%rsp), %r8
1800a170e: 49 8b d5                    	movq	%r13, %rdx
1800a1711: e8 ea 2b fd ff              	callq	0x180074300 <SetPDFrameWarpDiagnosticHud+0x4b8b0>
1800a1716: e8 f5 2f fd ff              	callq	0x180074710 <SetPDFrameWarpDiagnosticHud+0x4bcc0>
1800a171b: 41 80 3e 00                 	cmpb	$0x0, (%r14)
1800a171f: 74 20                       	je	0x1800a1741 <SetPDFrameWarpNativeCameraSource+0x28eb1>
1800a1721: 48 8b 0d c8 41 17 01        	movq	0x11741c8(%rip), %rcx   # 0x1812158f0
1800a1728: e8 63 5e fd ff              	callq	0x180077590 <SetPDFrameWarpDiagnosticHud+0x4eb40>
1800a172d: eb 12                       	jmp	0x1800a1741 <SetPDFrameWarpNativeCameraSource+0x28eb1>
1800a172f: 48 8d 0d d2 34 10 01        	leaq	0x11034d2(%rip), %rcx   # 0x1811a4c08
1800a1736: e8 05 a4 05 00              	callq	0x1800fbb40 <ReleaseUpscaleFeature+0x340>
1800a173b: e8 d0 2f fd ff              	callq	0x180074710 <SetPDFrameWarpDiagnosticHud+0x4bcc0>
1800a1740: 90                          	nop
1800a1741: 48 8b cb                    	movq	%rbx, %rcx
1800a1744: ff 15 de 1d 07 00           	callq	*0x71dde(%rip)          # 0x180113528
1800a174a: 4c 8d 9c 24 f0 01 00 00     	leaq	0x1f0(%rsp), %r11
1800a1752: 49 8b 5b 30                 	movq	0x30(%r11), %rbx
1800a1756: 49 8b 6b 38                 	movq	0x38(%r11), %rbp
1800a175a: 49 8b 73 40                 	movq	0x40(%r11), %rsi
1800a175e: 49 8b e3                    	movq	%r11, %rsp
1800a1761: 41 5f                       	popq	%r15
1800a1763: 41 5e                       	popq	%r14
1800a1765: 41 5d                       	popq	%r13
1800a1767: 41 5c                       	popq	%r12
1800a1769: 5f                          	popq	%rdi
1800a176a: c3                          	retq
