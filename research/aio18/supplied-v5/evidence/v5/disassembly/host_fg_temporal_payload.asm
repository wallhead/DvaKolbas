
/mnt/data/aio18_work/extracted/SKSE/Plugins/SkyrimUpscaler.dll:     file format pei-x86-64


Disassembly of section .text:

00000001802941a0 <.text+0x2931a0>:
   1802941a0:	48 89 4c 24 08       	mov    QWORD PTR [rsp+0x8],rcx
   1802941a5:	55                   	push   rbp
   1802941a6:	57                   	push   rdi
   1802941a7:	41 57                	push   r15
   1802941a9:	48 8d ac 24 50 fd ff 	lea    rbp,[rsp-0x2b0]
   1802941b0:	ff 
   1802941b1:	48 81 ec b0 03 00 00 	sub    rsp,0x3b0
   1802941b8:	48 8b 3d 21 cc be 00 	mov    rdi,QWORD PTR [rip+0xbecc21]        # 0x180e80de0
   1802941bf:	4c 8b fa             	mov    r15,rdx
   1802941c2:	48 8b cf             	mov    rcx,rdi
   1802941c5:	e8 86 23 00 00       	call   0x180296550
   1802941ca:	84 c0                	test   al,al
   1802941cc:	0f 84 34 07 00 00    	je     0x180294906
   1802941d2:	8b 87 60 04 00 00    	mov    eax,DWORD PTR [rdi+0x460]
   1802941d8:	48 89 9c 24 d8 03 00 	mov    QWORD PTR [rsp+0x3d8],rbx
   1802941df:	00 
   1802941e0:	bb 02 00 00 00       	mov    ebx,0x2
   1802941e5:	39 87 14 12 00 00    	cmp    DWORD PTR [rdi+0x1214],eax
   1802941eb:	75 6a                	jne    0x180294257
   1802941ed:	48 8d 87 6c 14 00 00 	lea    rax,[rdi+0x146c]
   1802941f4:	8b d3                	mov    edx,ebx
   1802941f6:	48 8d 4c 24 70       	lea    rcx,[rsp+0x70]
   1802941fb:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
   180294200:	48 8d 89 80 00 00 00 	lea    rcx,[rcx+0x80]
   180294207:	c5 fc 10 00          	vmovups ymm0,YMMWORD PTR [rax]
   18029420b:	c5 f8 10 48 70       	vmovups xmm1,XMMWORD PTR [rax+0x70]
   180294210:	48 8d 80 80 00 00 00 	lea    rax,[rax+0x80]
   180294217:	c5 fc 11 41 80       	vmovups YMMWORD PTR [rcx-0x80],ymm0
   18029421c:	c5 fc 10 40 a0       	vmovups ymm0,YMMWORD PTR [rax-0x60]
   180294221:	c5 fc 11 41 a0       	vmovups YMMWORD PTR [rcx-0x60],ymm0
   180294226:	c5 fc 10 40 c0       	vmovups ymm0,YMMWORD PTR [rax-0x40]
   18029422b:	c5 fc 11 41 c0       	vmovups YMMWORD PTR [rcx-0x40],ymm0
   180294230:	c5 f8 10 40 e0       	vmovups xmm0,XMMWORD PTR [rax-0x20]
   180294235:	c5 f8 11 41 e0       	vmovups XMMWORD PTR [rcx-0x20],xmm0
   18029423a:	c5 f8 11 49 f0       	vmovups XMMWORD PTR [rcx-0x10],xmm1
   18029423f:	48 83 ea 01          	sub    rdx,0x1
   180294243:	75 bb                	jne    0x180294200
   180294245:	8b 00                	mov    eax,DWORD PTR [rax]
   180294247:	89 01                	mov    DWORD PTR [rcx],eax
   180294249:	48 8d 4c 24 70       	lea    rcx,[rsp+0x70]
   18029424e:	c5 f8 77             	vzeroupper
   180294251:	ff 15 81 e9 1d 00    	call   QWORD PTR [rip+0x1de981]        # 0x180472bd8
   180294257:	44 8b 87 60 04 00 00 	mov    r8d,DWORD PTR [rdi+0x460]
   18029425e:	44 3b 05 7b ce 1c 00 	cmp    r8d,DWORD PTR [rip+0x1cce7b]        # 0x1804610e0
   180294265:	4c 89 b4 24 90 03 00 	mov    QWORD PTR [rsp+0x390],r14
   18029426c:	00 
   18029426d:	0f 85 18 01 00 00    	jne    0x18029438b
   180294273:	80 3d ca 1a 1e 00 00 	cmp    BYTE PTR [rip+0x1e1aca],0x0        # 0x180475d44
   18029427a:	0f 84 0b 01 00 00    	je     0x18029438b
   180294280:	41 b6 01             	mov    r14b,0x1
   180294283:	44 39 87 e8 0f 00 00 	cmp    DWORD PTR [rdi+0xfe8],r8d
   18029428a:	0f 85 66 06 00 00    	jne    0x1802948f6
   180294290:	48 8d 87 68 13 00 00 	lea    rax,[rdi+0x1368]
   180294297:	48 8b d3             	mov    rdx,rbx
   18029429a:	48 8d 4c 24 70       	lea    rcx,[rsp+0x70]
   18029429f:	90                   	nop
   1802942a0:	48 8d 89 80 00 00 00 	lea    rcx,[rcx+0x80]
   1802942a7:	c5 fc 10 00          	vmovups ymm0,YMMWORD PTR [rax]
   1802942ab:	c5 f8 10 48 70       	vmovups xmm1,XMMWORD PTR [rax+0x70]
   1802942b0:	48 8d 80 80 00 00 00 	lea    rax,[rax+0x80]
   1802942b7:	c5 fc 11 41 80       	vmovups YMMWORD PTR [rcx-0x80],ymm0
   1802942bc:	c5 fc 10 40 a0       	vmovups ymm0,YMMWORD PTR [rax-0x60]
   1802942c1:	c5 fc 11 41 a0       	vmovups YMMWORD PTR [rcx-0x60],ymm0
   1802942c6:	c5 fc 10 40 c0       	vmovups ymm0,YMMWORD PTR [rax-0x40]
   1802942cb:	c5 fc 11 41 c0       	vmovups YMMWORD PTR [rcx-0x40],ymm0
   1802942d0:	c5 f8 10 40 e0       	vmovups xmm0,XMMWORD PTR [rax-0x20]
   1802942d5:	c5 f8 11 41 e0       	vmovups XMMWORD PTR [rcx-0x20],xmm0
   1802942da:	c5 f8 11 49 f0       	vmovups XMMWORD PTR [rcx-0x10],xmm1
   1802942df:	48 83 ea 01          	sub    rdx,0x1
   1802942e3:	75 bb                	jne    0x1802942a0
   1802942e5:	8b 00                	mov    eax,DWORD PTR [rax]
   1802942e7:	89 01                	mov    DWORD PTR [rcx],eax
   1802942e9:	48 8d 8d 60 01 00 00 	lea    rcx,[rbp+0x160]
   1802942f0:	c5 fc 10 44 24 70    	vmovups ymm0,YMMWORD PTR [rsp+0x70]
   1802942f6:	c5 fc 10 4d 90       	vmovups ymm1,YMMWORD PTR [rbp-0x70]
   1802942fb:	c5 fc 11 45 f0       	vmovups YMMWORD PTR [rbp-0x10],ymm0
   180294300:	c5 fc 10 45 b0       	vmovups ymm0,YMMWORD PTR [rbp-0x50]
   180294305:	c5 fc 11 4d 10       	vmovups YMMWORD PTR [rbp+0x10],ymm1
   18029430a:	c5 fc 10 4d d0       	vmovups ymm1,YMMWORD PTR [rbp-0x30]
   18029430f:	c5 fc 11 45 30       	vmovups YMMWORD PTR [rbp+0x30],ymm0
   180294314:	c5 fc 11 4d 50       	vmovups YMMWORD PTR [rbp+0x50],ymm1
   180294319:	44 89 45 70          	mov    DWORD PTR [rbp+0x70],r8d
   18029431d:	48 8d 44 24 70       	lea    rax,[rsp+0x70]
   180294322:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
   180294326:	66 66 0f 1f 84 00 00 	data16 nop WORD PTR [rax+rax*1+0x0]
   18029432d:	00 00 00 
   180294330:	48 8d 89 80 00 00 00 	lea    rcx,[rcx+0x80]
   180294337:	c5 fc 10 00          	vmovups ymm0,YMMWORD PTR [rax]
   18029433b:	c5 f8 10 48 70       	vmovups xmm1,XMMWORD PTR [rax+0x70]
   180294340:	48 8d 80 80 00 00 00 	lea    rax,[rax+0x80]
   180294347:	c5 fc 11 41 80       	vmovups YMMWORD PTR [rcx-0x80],ymm0
   18029434c:	c5 fc 10 40 a0       	vmovups ymm0,YMMWORD PTR [rax-0x60]
   180294351:	c5 fc 11 41 a0       	vmovups YMMWORD PTR [rcx-0x60],ymm0
   180294356:	c5 fc 10 40 c0       	vmovups ymm0,YMMWORD PTR [rax-0x40]
   18029435b:	c5 fc 11 41 c0       	vmovups YMMWORD PTR [rcx-0x40],ymm0
   180294360:	c5 f8 10 40 e0       	vmovups xmm0,XMMWORD PTR [rax-0x20]
   180294365:	c5 f8 11 41 e0       	vmovups XMMWORD PTR [rcx-0x20],xmm0
   18029436a:	c5 f8 11 49 f0       	vmovups XMMWORD PTR [rcx-0x10],xmm1
   18029436f:	48 83 eb 01          	sub    rbx,0x1
   180294373:	75 bb                	jne    0x180294330
   180294375:	8b 00                	mov    eax,DWORD PTR [rax]
   180294377:	89 01                	mov    DWORD PTR [rcx],eax
   180294379:	48 8d 8d 60 01 00 00 	lea    rcx,[rbp+0x160]
   180294380:	c5 f8 77             	vzeroupper
   180294383:	ff 15 4f e8 1d 00    	call   QWORD PTR [rip+0x1de84f]        # 0x180472bd8
   180294389:	eb 03                	jmp    0x18029438e
   18029438b:	45 32 f6             	xor    r14b,r14b
   18029438e:	4c 8b 87 50 0b 00 00 	mov    r8,QWORD PTR [rdi+0xb50]
   180294395:	4d 85 c0             	test   r8,r8
   180294398:	0f 84 58 05 00 00    	je     0x1802948f6
   18029439e:	48 83 bf a8 0b 00 00 	cmp    QWORD PTR [rdi+0xba8],0x0
   1802943a5:	00 
   1802943a6:	0f 84 4a 05 00 00    	je     0x1802948f6
   1802943ac:	83 3d 4d 13 1e 00 02 	cmp    DWORD PTR [rip+0x1e134d],0x2        # 0x180475700
   1802943b3:	75 2d                	jne    0x1802943e2
   1802943b5:	83 bf dc 03 00 00 02 	cmp    DWORD PTR [rdi+0x3dc],0x2
   1802943bc:	74 09                	je     0x1802943c7
   1802943be:	80 bf a5 04 00 00 00 	cmp    BYTE PTR [rdi+0x4a5],0x0
   1802943c5:	74 1b                	je     0x1802943e2
   1802943c7:	80 bf 8e 04 00 00 00 	cmp    BYTE PTR [rdi+0x48e],0x0
   1802943ce:	74 12                	je     0x1802943e2
   1802943d0:	80 3f 00             	cmp    BYTE PTR [rdi],0x0
   1802943d3:	75 09                	jne    0x1802943de
   1802943d5:	80 bf 43 03 00 00 00 	cmp    BYTE PTR [rdi+0x343],0x0
   1802943dc:	74 04                	je     0x1802943e2
   1802943de:	b2 01                	mov    dl,0x1
   1802943e0:	eb 02                	jmp    0x1802943e4
   1802943e2:	32 d2                	xor    dl,dl
   1802943e4:	48 8b cf             	mov    rcx,rdi
   1802943e7:	e8 74 fd ff ff       	call   0x180294160
   1802943ec:	84 c0                	test   al,al
   1802943ee:	75 08                	jne    0x1802943f8
   1802943f0:	84 d2                	test   dl,dl
   1802943f2:	0f 84 fe 04 00 00    	je     0x1802948f6
   1802943f8:	83 bf dc 03 00 00 00 	cmp    DWORD PTR [rdi+0x3dc],0x0
   1802943ff:	7f 08                	jg     0x180294409
   180294401:	84 d2                	test   dl,dl
   180294403:	0f 84 ed 04 00 00    	je     0x1802948f6
   180294409:	80 bf 84 04 00 00 00 	cmp    BYTE PTR [rdi+0x484],0x0
   180294410:	0f 85 e0 04 00 00    	jne    0x1802948f6
   180294416:	49 8b 00             	mov    rax,QWORD PTR [r8]
   180294419:	48 8d 97 78 0b 00 00 	lea    rdx,[rdi+0xb78]
   180294420:	48 89 b4 24 a8 03 00 	mov    QWORD PTR [rsp+0x3a8],rsi
   180294427:	00 
   180294428:	49 8b c8             	mov    rcx,r8
   18029442b:	4c 89 ac 24 98 03 00 	mov    QWORD PTR [rsp+0x398],r13
   180294432:	00 
   180294433:	ff 50 50             	call   QWORD PTR [rax+0x50]
   180294436:	c5 fc 10 8f 78 0b 00 	vmovups ymm1,YMMWORD PTR [rdi+0xb78]
   18029443d:	00 
   18029443e:	c5 fb 10 87 98 0b 00 	vmovsd xmm0,QWORD PTR [rdi+0xb98]
   180294445:	00 
   180294446:	8b 87 a0 0b 00 00    	mov    eax,DWORD PTR [rdi+0xba0]
   18029444c:	c4 c1 79 7e cd       	vmovd  r13d,xmm1
   180294451:	89 85 a8 00 00 00    	mov    DWORD PTR [rbp+0xa8],eax
   180294457:	c5 fb 11 85 a0 00 00 	vmovsd QWORD PTR [rbp+0xa0],xmm0
   18029445e:	00 
   18029445f:	45 85 ed             	test   r13d,r13d
   180294462:	75 07                	jne    0x18029446b
   180294464:	44 8b af 4c 03 00 00 	mov    r13d,DWORD PTR [rdi+0x34c]
   18029446b:	c4 e1 f9 7e ce       	vmovq  rsi,xmm1
   180294470:	48 c1 ee 20          	shr    rsi,0x20
   180294474:	85 f6                	test   esi,esi
   180294476:	75 06                	jne    0x18029447e
   180294478:	8b b7 50 03 00 00    	mov    esi,DWORD PTR [rdi+0x350]
   18029447e:	45 85 ed             	test   r13d,r13d
   180294481:	0f 8e 5f 04 00 00    	jle    0x1802948e6
   180294487:	85 f6                	test   esi,esi
   180294489:	0f 8e 57 04 00 00    	jle    0x1802948e6
   18029448f:	4c 89 a4 24 a0 03 00 	mov    QWORD PTR [rsp+0x3a0],r12
   180294496:	00 
   180294497:	45 33 c0             	xor    r8d,r8d
   18029449a:	c5 f8 29 b4 24 80 03 	vmovaps XMMWORD PTR [rsp+0x380],xmm6
   1802944a1:	00 00 
   1802944a3:	c5 f8 29 bc 24 70 03 	vmovaps XMMWORD PTR [rsp+0x370],xmm7
   1802944aa:	00 00 
   1802944ac:	4d 85 ff             	test   r15,r15
   1802944af:	74 0f                	je     0x1802944c0
   1802944b1:	49 8b 07             	mov    rax,QWORD PTR [r15]
   1802944b4:	48 89 85 d0 02 00 00 	mov    QWORD PTR [rbp+0x2d0],rax
   1802944bb:	48 85 c0             	test   rax,rax
   1802944be:	75 07                	jne    0x1802944c7
   1802944c0:	4c 89 85 d0 02 00 00 	mov    QWORD PTR [rbp+0x2d0],r8
   1802944c7:	44 38 87 72 07 00 00 	cmp    BYTE PTR [rdi+0x772],r8b
   1802944ce:	74 09                	je     0x1802944d9
   1802944d0:	4c 89 85 e0 02 00 00 	mov    QWORD PTR [rbp+0x2e0],r8
   1802944d7:	eb 0e                	jmp    0x1802944e7
   1802944d9:	48 8b 87 50 0b 00 00 	mov    rax,QWORD PTR [rdi+0xb50]
   1802944e0:	48 89 85 e0 02 00 00 	mov    QWORD PTR [rbp+0x2e0],rax
   1802944e7:	48 8b 87 a8 0b 00 00 	mov    rax,QWORD PTR [rdi+0xba8]
   1802944ee:	48 8b cf             	mov    rcx,rdi
   1802944f1:	48 89 85 e8 02 00 00 	mov    QWORD PTR [rbp+0x2e8],rax
   1802944f8:	c5 f8 77             	vzeroupper
   1802944fb:	e8 10 fc ff ff       	call   0x180294110
   180294500:	84 c0                	test   al,al
   180294502:	74 1b                	je     0x18029451f
   180294504:	44 39 87 fc 04 00 00 	cmp    DWORD PTR [rdi+0x4fc],r8d
   18029450b:	74 09                	je     0x180294516
   18029450d:	83 bf dc 03 00 00 03 	cmp    DWORD PTR [rdi+0x3dc],0x3
   180294514:	75 09                	jne    0x18029451f
   180294516:	4c 8b bf 88 09 00 00 	mov    r15,QWORD PTR [rdi+0x988]
   18029451d:	eb 03                	jmp    0x180294522
   18029451f:	4d 8b f8             	mov    r15,r8
   180294522:	e8 e9 fb ff ff       	call   0x180294110
   180294527:	84 c0                	test   al,al
   180294529:	74 24                	je     0x18029454f
   18029452b:	44 38 87 8b 16 00 00 	cmp    BYTE PTR [rdi+0x168b],r8b
   180294532:	75 1b                	jne    0x18029454f
   180294534:	83 bf dc 03 00 00 03 	cmp    DWORD PTR [rdi+0x3dc],0x3
   18029453b:	75 09                	jne    0x180294546
   18029453d:	44 38 87 a5 04 00 00 	cmp    BYTE PTR [rdi+0x4a5],r8b
   180294544:	74 09                	je     0x18029454f
   180294546:	4c 8b a7 38 0a 00 00 	mov    r12,QWORD PTR [rdi+0xa38]
   18029454d:	eb 03                	jmp    0x180294552
   18029454f:	4d 8b e0             	mov    r12,r8
   180294552:	0f b6 97 a5 04 00 00 	movzx  edx,BYTE PTR [rdi+0x4a5]
   180294559:	84 d2                	test   dl,dl
   18029455b:	74 10                	je     0x18029456d
   18029455d:	e8 ae fb ff ff       	call   0x180294110
   180294562:	84 c0                	test   al,al
   180294564:	74 07                	je     0x18029456d
   180294566:	4c 8b bf 88 09 00 00 	mov    r15,QWORD PTR [rdi+0x988]
   18029456d:	83 bf dc 03 00 00 02 	cmp    DWORD PTR [rdi+0x3dc],0x2
   180294574:	74 04                	je     0x18029457a
   180294576:	84 d2                	test   dl,dl
   180294578:	74 0f                	je     0x180294589
   18029457a:	44 38 87 d2 03 00 00 	cmp    BYTE PTR [rdi+0x3d2],r8b
   180294581:	75 06                	jne    0x180294589
   180294583:	4d 8b f8             	mov    r15,r8
   180294586:	4d 8b e0             	mov    r12,r8
   180294589:	44 38 87 65 02 00 00 	cmp    BYTE PTR [rdi+0x265],r8b
   180294590:	74 0a                	je     0x18029459c
   180294592:	c5 fa 10 b7 68 02 00 	vmovss xmm6,DWORD PTR [rdi+0x268]
   180294599:	00 
   18029459a:	eb 04                	jmp    0x1802945a0
   18029459c:	c5 c8 57 f6          	vxorps xmm6,xmm6,xmm6
   1802945a0:	45 84 f6             	test   r14b,r14b
   1802945a3:	74 0a                	je     0x1802945af
   1802945a5:	c5 fa 10 bf f4 0f 00 	vmovss xmm7,DWORD PTR [rdi+0xff4]
   1802945ac:	00 
   1802945ad:	eb 2c                	jmp    0x1802945db
   1802945af:	c5 f0 57 c9          	vxorps xmm1,xmm1,xmm1
   1802945b3:	c5 f2 2a 8f 74 02 00 	vcvtsi2ss xmm1,xmm1,DWORD PTR [rdi+0x274]
   1802945ba:	00 
   1802945bb:	c5 f8 57 c0          	vxorps xmm0,xmm0,xmm0
   1802945bf:	c5 fa 2a 87 70 02 00 	vcvtsi2ss xmm0,xmm0,DWORD PTR [rdi+0x270]
   1802945c6:	00 
   1802945c7:	c5 f2 5e c8          	vdivss xmm1,xmm1,xmm0
   1802945cb:	c5 f2 59 97 f0 02 00 	vmulss xmm2,xmm1,DWORD PTR [rdi+0x2f0]
   1802945d2:	00 
   1802945d3:	c5 ea 59 3d 7d 76 17 	vmulss xmm7,xmm2,DWORD PTR [rip+0x17767d]        # 0x18040bc58
   1802945da:	00 
   1802945db:	49 8b d8             	mov    rbx,r8
   1802945de:	c7 44 24 48 28 00 00 	mov    DWORD PTR [rsp+0x48],0x28
   1802945e5:	00 
   1802945e6:	c5 f9 ef c0          	vpxor  xmm0,xmm0,xmm0
   1802945ea:	c5 f8 11 44 24 58    	vmovups XMMWORD PTR [rsp+0x58],xmm0
   1802945f0:	c7 44 24 4c 01 00 00 	mov    DWORD PTR [rsp+0x4c],0x1
   1802945f7:	00 
   1802945f8:	4c 89 44 24 50       	mov    QWORD PTR [rsp+0x50],r8
   1802945fd:	4c 89 44 24 68       	mov    QWORD PTR [rsp+0x68],r8
   180294602:	44 38 87 a5 04 00 00 	cmp    BYTE PTR [rdi+0x4a5],r8b
   180294609:	0f 84 ef 00 00 00    	je     0x1802946fe
   18029460f:	45 84 f6             	test   r14b,r14b
   180294612:	74 2b                	je     0x18029463f
   180294614:	c5 fb 10 87 38 0f 00 	vmovsd xmm0,QWORD PTR [rdi+0xf38]
   18029461b:	00 
   18029461c:	c5 fc 10 8f 18 0f 00 	vmovups ymm1,YMMWORD PTR [rdi+0xf18]
   180294623:	00 
   180294624:	8b 87 60 04 00 00    	mov    eax,DWORD PTR [rdi+0x460]
   18029462a:	c5 fb 11 44 24 40    	vmovsd QWORD PTR [rsp+0x40],xmm0
   180294630:	89 44 24 40          	mov    DWORD PTR [rsp+0x40],eax
   180294634:	c5 fc 11 4c 24 20    	vmovups YMMWORD PTR [rsp+0x20],ymm1
   18029463a:	e9 80 00 00 00       	jmp    0x1802946bf
   18029463f:	44 8b 87 60 04 00 00 	mov    r8d,DWORD PTR [rdi+0x460]
   180294646:	44 39 87 98 11 00 00 	cmp    DWORD PTR [rdi+0x1198],r8d
   18029464d:	75 12                	jne    0x180294661
   18029464f:	c5 fc 10 97 e8 11 00 	vmovups ymm2,YMMWORD PTR [rdi+0x11e8]
   180294656:	00 
   180294657:	c5 fb 10 87 08 12 00 	vmovsd xmm0,QWORD PTR [rdi+0x1208]
   18029465e:	00 
   18029465f:	eb 1c                	jmp    0x18029467d
   180294661:	48 8b 97 50 0b 00 00 	mov    rdx,QWORD PTR [rdi+0xb50]
   180294668:	48 8d 8d 80 00 00 00 	lea    rcx,[rbp+0x80]
   18029466f:	e8 7c c1 fc ff       	call   0x1802607f0
   180294674:	c5 fc 10 10          	vmovups ymm2,YMMWORD PTR [rax]
   180294678:	c5 fb 10 40 20       	vmovsd xmm0,QWORD PTR [rax+0x20]
   18029467d:	8b 87 60 04 00 00    	mov    eax,DWORD PTR [rdi+0x460]
   180294683:	c5 fb 11 85 a0 00 00 	vmovsd QWORD PTR [rbp+0xa0],xmm0
   18029468a:	00 
   18029468b:	c5 fc 11 54 24 20    	vmovups YMMWORD PTR [rsp+0x20],ymm2
   180294691:	c5 fb 11 44 24 40    	vmovsd QWORD PTR [rsp+0x40],xmm0
   180294697:	39 87 9c 11 00 00    	cmp    DWORD PTR [rdi+0x119c],eax
   18029469d:	75 20                	jne    0x1802946bf
   18029469f:	48 8b 8f 10 0e 00 00 	mov    rcx,QWORD PTR [rdi+0xe10]
   1802946a6:	48 85 c9             	test   rcx,rcx
   1802946a9:	74 14                	je     0x1802946bf
   1802946ab:	48 8b 44 24 28       	mov    rax,QWORD PTR [rsp+0x28]
   1802946b0:	39 9d a4 00 00 00    	cmp    DWORD PTR [rbp+0xa4],ebx
   1802946b6:	48 0f 45 c1          	cmovne rax,rcx
   1802946ba:	48 89 44 24 28       	mov    QWORD PTR [rsp+0x28],rax
   1802946bf:	48 8d 4c 24 20       	lea    rcx,[rsp+0x20]
   1802946c4:	c5 f8 77             	vzeroupper
   1802946c7:	ff 15 7b e5 1d 00    	call   QWORD PTR [rip+0x1de57b]        # 0x180472c48
   1802946cd:	39 5c 24 44          	cmp    DWORD PTR [rsp+0x44],ebx
   1802946d1:	74 10                	je     0x1802946e3
   1802946d3:	8b 87 60 04 00 00    	mov    eax,DWORD PTR [rdi+0x460]
   1802946d9:	39 44 24 40          	cmp    DWORD PTR [rsp+0x40],eax
   1802946dd:	48 0f 44 5c 24 28    	cmove  rbx,QWORD PTR [rsp+0x28]
   1802946e3:	c5 fc 10 44 24 20    	vmovups ymm0,YMMWORD PTR [rsp+0x20]
   1802946e9:	c5 fb 10 4c 24 40    	vmovsd xmm1,QWORD PTR [rsp+0x40]
   1802946ef:	c5 fc 11 44 24 48    	vmovups YMMWORD PTR [rsp+0x48],ymm0
   1802946f5:	c5 fb 11 4c 24 68    	vmovsd QWORD PTR [rsp+0x68],xmm1
   1802946fb:	45 33 c0             	xor    r8d,r8d
   1802946fe:	c5 fa 10 4f 0c       	vmovss xmm1,DWORD PTR [rdi+0xc]
   180294703:	8b 97 60 04 00 00    	mov    edx,DWORD PTR [rdi+0x460]
   180294709:	48 8d 8d b0 00 00 00 	lea    rcx,[rbp+0xb0]
   180294710:	c5 f9 ef c0          	vpxor  xmm0,xmm0,xmm0
   180294714:	c5 fc 11 45 a0       	vmovups YMMWORD PTR [rbp-0x60],ymm0
   180294719:	c5 f8 11 45 c0       	vmovups XMMWORD PTR [rbp-0x40],xmm0
   18029471e:	c5 fa 10 47 08       	vmovss xmm0,DWORD PTR [rdi+0x8]
   180294723:	33 c0                	xor    eax,eax
   180294725:	48 c7 44 24 70 00 00 	mov    QWORD PTR [rsp+0x70],0x0
   18029472c:	00 00 
   18029472e:	48 89 45 d0          	mov    QWORD PTR [rbp-0x30],rax
   180294732:	48 89 45 e0          	mov    QWORD PTR [rbp-0x20],rax
   180294736:	48 89 45 e8          	mov    QWORD PTR [rbp-0x18],rax
   18029473a:	48 89 45 f0          	mov    QWORD PTR [rbp-0x10],rax
   18029473e:	48 8b 85 d0 02 00 00 	mov    rax,QWORD PTR [rbp+0x2d0]
   180294745:	48 89 44 24 78       	mov    QWORD PTR [rsp+0x78],rax
   18029474a:	48 8b 85 e8 02 00 00 	mov    rax,QWORD PTR [rbp+0x2e8]
   180294751:	48 89 45 80          	mov    QWORD PTR [rbp-0x80],rax
   180294755:	48 8b 85 e0 02 00 00 	mov    rax,QWORD PTR [rbp+0x2e0]
   18029475c:	48 89 45 88          	mov    QWORD PTR [rbp-0x78],rax
   180294760:	8b 87 a0 16 00 00    	mov    eax,DWORD PTR [rdi+0x16a0]
   180294766:	c5 fa 11 45 b4       	vmovss DWORD PTR [rbp-0x4c],xmm0
   18029476b:	c5 fa 10 87 f8 02 00 	vmovss xmm0,DWORD PTR [rdi+0x2f8]
   180294772:	00 
   180294773:	c5 fa 11 4d b8       	vmovss DWORD PTR [rbp-0x48],xmm1
   180294778:	c5 fa 10 8f f4 02 00 	vmovss xmm1,DWORD PTR [rdi+0x2f4]
   18029477f:	00 
   180294780:	c5 fa 11 75 b0       	vmovss DWORD PTR [rbp-0x50],xmm6
   180294785:	c5 fa 11 45 c8       	vmovss DWORD PTR [rbp-0x38],xmm0
   18029478a:	c5 fa 11 4d cc       	vmovss DWORD PTR [rbp-0x34],xmm1
   18029478f:	c5 fa 11 7d d0       	vmovss DWORD PTR [rbp-0x30],xmm7
   180294794:	4c 89 45 98          	mov    QWORD PTR [rbp-0x68],r8
   180294798:	ff c8                	dec    eax
   18029479a:	89 45 10             	mov    DWORD PTR [rbp+0x10],eax
   18029479d:	48 8d 44 24 70       	lea    rax,[rsp+0x70]
   1802947a2:	4c 89 45 d8          	mov    QWORD PTR [rbp-0x28],r8
   1802947a6:	48 89 5d 90          	mov    QWORD PTR [rbp-0x70],rbx
   1802947aa:	4c 89 45 a0          	mov    QWORD PTR [rbp-0x60],r8
   1802947ae:	4c 89 7d f8          	mov    QWORD PTR [rbp-0x8],r15
   1802947b2:	4c 89 65 00          	mov    QWORD PTR [rbp+0x0],r12
   1802947b6:	c6 45 c4 00          	mov    BYTE PTR [rbp-0x3c],0x0
   1802947ba:	89 55 08             	mov    DWORD PTR [rbp+0x8],edx
   1802947bd:	c6 45 d4 01          	mov    BYTE PTR [rbp-0x2c],0x1
   1802947c1:	c6 45 0c 01          	mov    BYTE PTR [rbp+0xc],0x1
   1802947c5:	4c 89 45 18          	mov    QWORD PTR [rbp+0x18],r8
   1802947c9:	c5 e0 57 db          	vxorps xmm3,xmm3,xmm3
   1802947cd:	c4 c1 62 2a dd       	vcvtsi2ss xmm3,xmm3,r13d
   1802947d2:	c5 fa 11 5d a8       	vmovss DWORD PTR [rbp-0x58],xmm3
   1802947d7:	c5 fa 11 5d bc       	vmovss DWORD PTR [rbp-0x44],xmm3
   1802947dc:	c5 e8 57 d2          	vxorps xmm2,xmm2,xmm2
   1802947e0:	c5 ea 2a d6          	vcvtsi2ss xmm2,xmm2,esi
   1802947e4:	c5 fa 11 55 ac       	vmovss DWORD PTR [rbp-0x54],xmm2
   1802947e9:	c5 fa 11 55 c0       	vmovss DWORD PTR [rbp-0x40],xmm2
   1802947ee:	c5 fc 10 00          	vmovups ymm0,YMMWORD PTR [rax]
   1802947f2:	c5 fc 11 01          	vmovups YMMWORD PTR [rcx],ymm0
   1802947f6:	c5 fc 10 40 20       	vmovups ymm0,YMMWORD PTR [rax+0x20]
   1802947fb:	c5 fc 11 41 20       	vmovups YMMWORD PTR [rcx+0x20],ymm0
   180294800:	c5 fc 10 40 40       	vmovups ymm0,YMMWORD PTR [rax+0x40]
   180294805:	c5 fc 11 41 40       	vmovups YMMWORD PTR [rcx+0x40],ymm0
   18029480a:	c5 fc 10 40 60       	vmovups ymm0,YMMWORD PTR [rax+0x60]
   18029480f:	c5 fc 11 41 60       	vmovups YMMWORD PTR [rcx+0x60],ymm0
   180294814:	c5 fc 10 80 80 00 00 	vmovups ymm0,YMMWORD PTR [rax+0x80]
   18029481b:	00 
   18029481c:	c5 fc 11 81 80 00 00 	vmovups YMMWORD PTR [rcx+0x80],ymm0
   180294823:	00 
   180294824:	c5 f8 10 80 a0 00 00 	vmovups xmm0,XMMWORD PTR [rax+0xa0]
   18029482b:	00 
   18029482c:	c5 f8 11 81 a0 00 00 	vmovups XMMWORD PTR [rcx+0xa0],xmm0
   180294833:	00 
   180294834:	33 c9                	xor    ecx,ecx
   180294836:	c5 f8 77             	vzeroupper
   180294839:	ff 15 69 e3 1d 00    	call   QWORD PTR [rip+0x1de369]        # 0x180472ba8
   18029483f:	83 bf dc 03 00 00 02 	cmp    DWORD PTR [rdi+0x3dc],0x2
   180294846:	c5 f8 28 bc 24 70 03 	vmovaps xmm7,XMMWORD PTR [rsp+0x370]
   18029484d:	00 00 
   18029484f:	c5 f8 28 b4 24 80 03 	vmovaps xmm6,XMMWORD PTR [rsp+0x380]
   180294856:	00 00 
   180294858:	4c 8b a4 24 a0 03 00 	mov    r12,QWORD PTR [rsp+0x3a0]
   18029485f:	00 
   180294860:	74 09                	je     0x18029486b
   180294862:	80 bf a5 04 00 00 00 	cmp    BYTE PTR [rdi+0x4a5],0x0
   180294869:	74 43                	je     0x1802948ae
   18029486b:	45 84 f6             	test   r14b,r14b
   18029486e:	75 1b                	jne    0x18029488b
   180294870:	c5 fa 10 8d 10 01 00 	vmovss xmm1,DWORD PTR [rbp+0x110]
   180294877:	00 
   180294878:	8b 8f 60 04 00 00    	mov    ecx,DWORD PTR [rdi+0x460]
   18029487e:	e8 5d 84 fe ff       	call   0x18027cce0
   180294883:	c5 fa 11 85 10 01 00 	vmovss DWORD PTR [rbp+0x110],xmm0
   18029488a:	00 
   18029488b:	80 bf 43 03 00 00 00 	cmp    BYTE PTR [rdi+0x343],0x0
   180294892:	75 1a                	jne    0x1802948ae
   180294894:	c5 fa 10 47 10       	vmovss xmm0,DWORD PTR [rdi+0x10]
   180294899:	c5 fa 10 4f 14       	vmovss xmm1,DWORD PTR [rdi+0x14]
   18029489e:	c5 fa 11 85 fc 00 00 	vmovss DWORD PTR [rbp+0xfc],xmm0
   1802948a5:	00 
   1802948a6:	c5 fa 11 8d 00 01 00 	vmovss DWORD PTR [rbp+0x100],xmm1
   1802948ad:	00 
   1802948ae:	45 84 f6             	test   r14b,r14b
   1802948b1:	75 26                	jne    0x1802948d9
   1802948b3:	44 38 b7 a5 04 00 00 	cmp    BYTE PTR [rdi+0x4a5],r14b
   1802948ba:	74 1d                	je     0x1802948d9
   1802948bc:	c5 fa 10 85 10 01 00 	vmovss xmm0,DWORD PTR [rbp+0x110]
   1802948c3:	00 
   1802948c4:	48 8d 54 24 48       	lea    rdx,[rsp+0x48]
   1802948c9:	48 8b cf             	mov    rcx,rdi
   1802948cc:	c5 fa 11 87 f4 0f 00 	vmovss DWORD PTR [rdi+0xff4],xmm0
   1802948d3:	00 
   1802948d4:	e8 97 16 fd ff       	call   0x180265f70
   1802948d9:	48 8d 8d b0 00 00 00 	lea    rcx,[rbp+0xb0]
   1802948e0:	ff 15 72 e3 1d 00    	call   QWORD PTR [rip+0x1de372]        # 0x180472c58
   1802948e6:	4c 8b ac 24 98 03 00 	mov    r13,QWORD PTR [rsp+0x398]
   1802948ed:	00 
   1802948ee:	48 8b b4 24 a8 03 00 	mov    rsi,QWORD PTR [rsp+0x3a8]
   1802948f5:	00 
   1802948f6:	4c 8b b4 24 90 03 00 	mov    r14,QWORD PTR [rsp+0x390]
   1802948fd:	00 
   1802948fe:	48 8b 9c 24 d8 03 00 	mov    rbx,QWORD PTR [rsp+0x3d8]
   180294905:	00 
   180294906:	c5 f8 77             	vzeroupper
   180294909:	48 81 c4 b0 03 00 00 	add    rsp,0x3b0
   180294910:	41 5f                	pop    r15
   180294912:	5f                   	pop    rdi
   180294913:	5d                   	pop    rbp
   180294914:	c3                   	ret
   180294915:	cc                   	int3
   180294916:	cc                   	int3
   180294917:	cc                   	int3
   180294918:	cc                   	int3
   180294919:	cc                   	int3
   18029491a:	cc                   	int3
   18029491b:	cc                   	int3
   18029491c:	cc                   	int3
   18029491d:	cc                   	int3
   18029491e:	cc                   	int3
   18029491f:	cc                   	int3
   180294920:	40 53                	rex push rbx
   180294922:	48 83 ec 50          	sub    rsp,0x50
   180294926:	33 c0                	xor    eax,eax
   180294928:	48 8b da             	mov    rbx,rdx
   18029492b:	48 89 44 24 40       	mov    QWORD PTR [rsp+0x40],rax
   180294930:	89 44 24 48          	mov    DWORD PTR [rsp+0x48],eax
   180294934:	c5 f9 ef c0          	vpxor  xmm0,xmm0,xmm0
   180294938:	c5 fc 11 44 24 20    	vmovups YMMWORD PTR [rsp+0x20],ymm0
   18029493e:	4d 85 c0             	test   r8,r8
   180294941:	74 11                	je     0x180294954
   180294943:	49 8b 00             	mov    rax,QWORD PTR [r8]
   180294946:	48 8d 54 24 20       	lea    rdx,[rsp+0x20]
   18029494b:	49 8b c8             	mov    rcx,r8
   18029494e:	c5 f8 77             	vzeroupper
   180294951:	ff 50 50             	call   QWORD PTR [rax+0x50]
   180294954:	33 c0                	xor    eax,eax
   180294956:	48 89 03             	mov    QWORD PTR [rbx],rax
   180294959:	8b 44 24 20          	mov    eax,DWORD PTR [rsp+0x20]
   18029495d:	89 43 08             	mov    DWORD PTR [rbx+0x8],eax
   180294960:	8b 44 24 24          	mov    eax,DWORD PTR [rsp+0x24]
   180294964:	89 43 0c             	mov    DWORD PTR [rbx+0xc],eax
   180294967:	48 8b c3             	mov    rax,rbx
   18029496a:	c5 f8 77             	vzeroupper
   18029496d:	48 83 c4 50          	add    rsp,0x50
   180294971:	5b                   	pop    rbx
   180294972:	c3                   	ret
   180294973:	cc                   	int3
   180294974:	cc                   	int3
   180294975:	cc                   	int3
   180294976:	cc                   	int3
   180294977:	cc                   	int3
   180294978:	cc                   	int3
   180294979:	cc                   	int3
   18029497a:	cc                   	int3
   18029497b:	cc                   	int3
   18029497c:	cc                   	int3
   18029497d:	cc                   	int3
   18029497e:	cc                   	int3
   18029497f:	cc                   	int3
   180294980:	48 8b 05 59 c4 be 00 	mov    rax,QWORD PTR [rip+0xbec459]        # 0x180e80de0
   180294987:	80 b8 a5 04 00 00 00 	cmp    BYTE PTR [rax+0x4a5],0x0
   18029498e:	74 21                	je     0x1802949b1
   180294990:	83 b8 a8 04 00 00 00 	cmp    DWORD PTR [rax+0x4a8],0x0
   180294997:	74 18                	je     0x1802949b1
   180294999:	83 3d 60 0d 1e 00 02 	cmp    DWORD PTR [rip+0x1e0d60],0x2        # 0x180475700
   1802949a0:	75 0c                	jne    0x1802949ae
   1802949a2:	0f b6 05 7f 0d 1e 00 	movzx  eax,BYTE PTR [rip+0x1e0d7f]        # 0x180475728
   1802949a9:	90                   	nop
   1802949aa:	84 c0                	test   al,al
   1802949ac:	75 03                	jne    0x1802949b1
   1802949ae:	b0 01                	mov    al,0x1
   1802949b0:	c3                   	ret
   1802949b1:	32 c0                	xor    al,al
   1802949b3:	c3                   	ret
   1802949b4:	cc                   	int3
   1802949b5:	cc                   	int3
   1802949b6:	cc                   	int3
   1802949b7:	cc                   	int3
   1802949b8:	cc                   	int3
   1802949b9:	cc                   	int3
   1802949ba:	cc                   	int3
   1802949bb:	cc                   	int3
   1802949bc:	cc                   	int3
   1802949bd:	cc                   	int3
   1802949be:	cc                   	int3
   1802949bf:	cc                   	int3
