
/mnt/data/aio18_work/extracted/SKSE/Plugins/SkyrimUpscaler.dll:     file format pei-x86-64


Disassembly of section .text:

000000018027d400 <.text+0x27c400>:
   18027d400:	88 54 24 10          	mov    BYTE PTR [rsp+0x10],dl
   18027d404:	89 4c 24 08          	mov    DWORD PTR [rsp+0x8],ecx
   18027d408:	4c 8b dc             	mov    r11,rsp
   18027d40b:	55                   	push   rbp
   18027d40c:	49 8d ab 88 fd ff ff 	lea    rbp,[r11-0x278]
   18027d413:	48 81 ec 70 03 00 00 	sub    rsp,0x370
   18027d41a:	8b 05 d0 20 1e 00    	mov    eax,DWORD PTR [rip+0x1e20d0]        # 0x18045f4f0
   18027d420:	a8 40                	test   al,0x40
   18027d422:	0f 84 72 19 00 00    	je     0x18027ed9a
   18027d428:	8b 05 c2 20 1e 00    	mov    eax,DWORD PTR [rip+0x1e20c2]        # 0x18045f4f0
   18027d42e:	25 b8 00 00 00       	and    eax,0xb8
   18027d433:	3c b8                	cmp    al,0xb8
   18027d435:	0f 85 5f 19 00 00    	jne    0x18027ed9a
   18027d43b:	49 89 5b f0          	mov    QWORD PTR [r11-0x10],rbx
   18027d43f:	49 89 73 e8          	mov    QWORD PTR [r11-0x18],rsi
   18027d443:	49 89 7b e0          	mov    QWORD PTR [r11-0x20],rdi
   18027d447:	4d 89 6b d0          	mov    QWORD PTR [r11-0x30],r13
   18027d44b:	4d 89 73 c8          	mov    QWORD PTR [r11-0x38],r14
   18027d44f:	4d 89 7b c0          	mov    QWORD PTR [r11-0x40],r15
   18027d453:	c4 41 78 29 8b 78 ff 	vmovaps XMMWORD PTR [r11-0x88],xmm9
   18027d45a:	ff ff 
   18027d45c:	e8 ef 19 00 00       	call   0x18027ee50
   18027d461:	c5 fa 6f 05 97 f3 18 	vmovdqu xmm0,XMMWORD PTR [rip+0x18f397]        # 0x18040c800
   18027d468:	00 
   18027d469:	c4 41 30 57 c9       	vxorps xmm9,xmm9,xmm9
   18027d46e:	c5 f8 11 05 82 3c 1e 	vmovups XMMWORD PTR [rip+0x1e3c82],xmm0        # 0x1804610f8
   18027d475:	00 
   18027d476:	c5 7a 11 0d ee 8a 1f 	vmovss DWORD PTR [rip+0x1f8aee],xmm9        # 0x180475f6c
   18027d47d:	00 
   18027d47e:	c6 05 e3 8a 1f 00 00 	mov    BYTE PTR [rip+0x1f8ae3],0x0        # 0x180475f68
   18027d485:	c6 05 98 88 1f 00 00 	mov    BYTE PTR [rip+0x1f8898],0x0        # 0x180475d24
   18027d48c:	c6 05 ad 88 1f 00 00 	mov    BYTE PTR [rip+0x1f88ad],0x0        # 0x180475d40
   18027d493:	c6 05 aa 88 1f 00 00 	mov    BYTE PTR [rip+0x1f88aa],0x0        # 0x180475d44
   18027d49a:	c6 05 90 8b 1f 00 00 	mov    BYTE PTR [rip+0x1f8b90],0x0        # 0x180476031
   18027d4a1:	e8 fa 2e ed ff       	call   0x1801503a0
   18027d4a6:	33 db                	xor    ebx,ebx
   18027d4a8:	4c 8b f0             	mov    r14,rax
   18027d4ab:	38 98 ad 04 00 00    	cmp    BYTE PTR [rax+0x4ad],bl
   18027d4b1:	74 0d                	je     0x18027d4c0
   18027d4b3:	48 8b c8             	mov    rcx,rax
   18027d4b6:	e8 65 c1 fe ff       	call   0x180269620
   18027d4bb:	48 8b f8             	mov    rdi,rax
   18027d4be:	eb 03                	jmp    0x18027d4c3
   18027d4c0:	48 8b fb             	mov    rdi,rbx
   18027d4c3:	48 39 3d 6e 88 1f 00 	cmp    QWORD PTR [rip+0x1f886e],rdi        # 0x180475d38
   18027d4ca:	74 13                	je     0x18027d4df
   18027d4cc:	e8 ef 1d 00 00       	call   0x18027f2c0
   18027d4d1:	48 89 1d 80 88 1f 00 	mov    QWORD PTR [rip+0x1f8880],rbx        # 0x180475d58
   18027d4d8:	48 89 3d 59 88 1f 00 	mov    QWORD PTR [rip+0x1f8859],rdi        # 0x180475d38
   18027d4df:	41 8b 86 60 04 00 00 	mov    eax,DWORD PTR [r14+0x460]
   18027d4e6:	89 05 f4 3b 1e 00    	mov    DWORD PTR [rip+0x1e3bf4],eax        # 0x1804610e0
   18027d4ec:	4c 89 a4 24 50 03 00 	mov    QWORD PTR [rsp+0x350],r12
   18027d4f3:	00 
   18027d4f4:	e8 d7 ee f2 ff       	call   0x1801ac3d0
   18027d4f9:	48 8b f8             	mov    rdi,rax
   18027d4fc:	e8 2f d3 f2 ff       	call   0x1801aa830
   18027d501:	4c 8b e8             	mov    r13,rax
   18027d504:	e8 e7 d1 f2 ff       	call   0x1801aa6f0
   18027d509:	48 89 44 24 70       	mov    QWORD PTR [rsp+0x70],rax
   18027d50e:	e8 9d e9 f2 ff       	call   0x1801abeb0
   18027d513:	4c 8b f8             	mov    r15,rax
   18027d516:	41 38 9e ac 04 00 00 	cmp    BYTE PTR [r14+0x4ac],bl
   18027d51d:	74 0e                	je     0x18027d52d
   18027d51f:	41 38 9e a6 04 00 00 	cmp    BYTE PTR [r14+0x4a6],bl
   18027d526:	74 05                	je     0x18027d52d
   18027d528:	41 b4 01             	mov    r12b,0x1
   18027d52b:	eb 03                	jmp    0x18027d530
   18027d52d:	45 32 e4             	xor    r12b,r12b
   18027d530:	38 1d b4 87 1f 00    	cmp    BYTE PTR [rip+0x1f87b4],bl        # 0x180475cea
   18027d536:	74 2e                	je     0x18027d566
   18027d538:	41 38 9e e4 04 00 00 	cmp    BYTE PTR [r14+0x4e4],bl
   18027d53f:	75 09                	jne    0x18027d54a
   18027d541:	83 3d b8 81 1f 00 02 	cmp    DWORD PTR [rip+0x1f81b8],0x2        # 0x180475700
   18027d548:	75 1c                	jne    0x18027d566
   18027d54a:	41 38 9e a5 04 00 00 	cmp    BYTE PTR [r14+0x4a5],bl
   18027d551:	74 13                	je     0x18027d566
   18027d553:	45 84 e4             	test   r12b,r12b
   18027d556:	75 09                	jne    0x18027d561
   18027d558:	41 38 9e ad 04 00 00 	cmp    BYTE PTR [r14+0x4ad],bl
   18027d55f:	74 05                	je     0x18027d566
   18027d561:	40 b6 01             	mov    sil,0x1
   18027d564:	eb 03                	jmp    0x18027d569
   18027d566:	40 32 f6             	xor    sil,sil
   18027d569:	40 3a 35 78 87 1f 00 	cmp    sil,BYTE PTR [rip+0x1f8778]        # 0x180475ce8
   18027d570:	74 21                	je     0x18027d593
   18027d572:	66 89 1d b7 8a 1f 00 	mov    WORD PTR [rip+0x1f8ab7],bx        # 0x180476030
   18027d579:	c6 05 69 87 1f 00 01 	mov    BYTE PTR [rip+0x1f8769],0x1        # 0x180475ce9
   18027d580:	e8 1b 2e ed ff       	call   0x1801503a0
   18027d585:	c6 80 64 02 00 00 01 	mov    BYTE PTR [rax+0x264],0x1
   18027d58c:	40 88 35 55 87 1f 00 	mov    BYTE PTR [rip+0x1f8755],sil        # 0x180475ce8
   18027d593:	40 84 f6             	test   sil,sil
   18027d596:	0f 84 61 17 00 00    	je     0x18027ecfd
   18027d59c:	39 9d 80 02 00 00    	cmp    DWORD PTR [rbp+0x280],ebx
   18027d5a2:	0f 85 55 17 00 00    	jne    0x18027ecfd
   18027d5a8:	38 9d 88 02 00 00    	cmp    BYTE PTR [rbp+0x288],bl
   18027d5ae:	0f 85 49 17 00 00    	jne    0x18027ecfd
   18027d5b4:	41 38 9e 8a 04 00 00 	cmp    BYTE PTR [r14+0x48a],bl
   18027d5bb:	0f 84 3c 17 00 00    	je     0x18027ecfd
   18027d5c1:	48 85 ff             	test   rdi,rdi
   18027d5c4:	0f 84 33 17 00 00    	je     0x18027ecfd
   18027d5ca:	48 8b cf             	mov    rcx,rdi
   18027d5cd:	e8 6e ec f2 ff       	call   0x1801ac240
   18027d5d2:	84 c0                	test   al,al
   18027d5d4:	0f 85 23 17 00 00    	jne    0x18027ecfd
   18027d5da:	4d 85 ff             	test   r15,r15
   18027d5dd:	0f 84 1a 17 00 00    	je     0x18027ecfd
   18027d5e3:	48 8b 74 24 70       	mov    rsi,QWORD PTR [rsp+0x70]
   18027d5e8:	48 85 f6             	test   rsi,rsi
   18027d5eb:	0f 84 0c 17 00 00    	je     0x18027ecfd
   18027d5f1:	4d 85 ed             	test   r13,r13
   18027d5f4:	0f 84 03 17 00 00    	je     0x18027ecfd
   18027d5fa:	41 38 9e d0 03 00 00 	cmp    BYTE PTR [r14+0x3d0],bl
   18027d601:	0f 85 f6 16 00 00    	jne    0x18027ecfd
   18027d607:	41 38 9e 8b 16 00 00 	cmp    BYTE PTR [r14+0x168b],bl
   18027d60e:	0f 85 e9 16 00 00    	jne    0x18027ecfd
   18027d614:	e8 87 57 ed ff       	call   0x180152da0
   18027d619:	48 85 c0             	test   rax,rax
   18027d61c:	74 15                	je     0x18027d633
   18027d61e:	e8 7d 57 ed ff       	call   0x180152da0
   18027d623:	0f b6 88 5c 01 00 00 	movzx  ecx,BYTE PTR [rax+0x15c]
   18027d62a:	90                   	nop
   18027d62b:	84 c9                	test   cl,cl
   18027d62d:	0f 85 ca 16 00 00    	jne    0x18027ecfd
   18027d633:	e8 c8 58 ed ff       	call   0x180152f00
   18027d638:	48 8b c8             	mov    rcx,rax
   18027d63b:	e8 90 17 fe ff       	call   0x18025edd0
   18027d640:	84 c0                	test   al,al
   18027d642:	0f 85 b5 16 00 00    	jne    0x18027ecfd
   18027d648:	48 8d 15 c9 80 16 00 	lea    rdx,[rip+0x1680c9]        # 0x1803e5718
   18027d64f:	48 8b cf             	mov    rcx,rdi
   18027d652:	e8 29 ee f2 ff       	call   0x1801ac480
   18027d657:	84 c0                	test   al,al
   18027d659:	0f 85 9e 16 00 00    	jne    0x18027ecfd
   18027d65f:	48 8d 15 a2 80 16 00 	lea    rdx,[rip+0x1680a2]        # 0x1803e5708
   18027d666:	48 8b cf             	mov    rcx,rdi
   18027d669:	e8 12 ee f2 ff       	call   0x1801ac480
   18027d66e:	84 c0                	test   al,al
   18027d670:	0f 85 87 16 00 00    	jne    0x18027ecfd
   18027d676:	48 8d 15 bb 80 16 00 	lea    rdx,[rip+0x1680bb]        # 0x1803e5738
   18027d67d:	48 8b cf             	mov    rcx,rdi
   18027d680:	e8 fb ed f2 ff       	call   0x1801ac480
   18027d685:	84 c0                	test   al,al
   18027d687:	0f 85 70 16 00 00    	jne    0x18027ecfd
   18027d68d:	48 8b 76 28          	mov    rsi,QWORD PTR [rsi+0x28]
   18027d691:	49 8b cf             	mov    rcx,r15
   18027d694:	49 8b 7d 60          	mov    rdi,QWORD PTR [r13+0x60]
   18027d698:	48 89 74 24 70       	mov    QWORD PTR [rsp+0x70],rsi
   18027d69d:	e8 4e 52 ed ff       	call   0x1801528f0
   18027d6a2:	4c 8b e8             	mov    r13,rax
   18027d6a5:	38 58 18             	cmp    BYTE PTR [rax+0x18],bl
   18027d6a8:	0f 85 20 16 00 00    	jne    0x18027ecce
   18027d6ae:	48 85 ff             	test   rdi,rdi
   18027d6b1:	0f 84 17 16 00 00    	je     0x18027ecce
   18027d6b7:	38 1d 1b 86 1f 00    	cmp    BYTE PTR [rip+0x1f861b],bl        # 0x180475cd8
   18027d6bd:	c5 f8 29 b4 24 20 03 	vmovaps XMMWORD PTR [rsp+0x320],xmm6
   18027d6c4:	00 00 
   18027d6c6:	c5 f8 29 bc 24 10 03 	vmovaps XMMWORD PTR [rsp+0x310],xmm7
   18027d6cd:	00 00 
   18027d6cf:	c5 78 29 84 24 00 03 	vmovaps XMMWORD PTR [rsp+0x300],xmm8
   18027d6d6:	00 00 
   18027d6d8:	c5 78 29 94 24 e0 02 	vmovaps XMMWORD PTR [rsp+0x2e0],xmm10
   18027d6df:	00 00 
   18027d6e1:	75 13                	jne    0x18027d6f6
   18027d6e3:	c6 05 ff 85 1f 00 01 	mov    BYTE PTR [rip+0x1f85ff],0x1        # 0x180475ce9
   18027d6ea:	e8 b1 2c ed ff       	call   0x1801503a0
   18027d6ef:	c6 80 64 02 00 00 01 	mov    BYTE PTR [rax+0x264],0x1
   18027d6f6:	83 3d 03 80 1f 00 02 	cmp    DWORD PTR [rip+0x1f8003],0x2        # 0x180475700
   18027d6fd:	c4 c1 7a 10 8f 8c 00 	vmovss xmm1,DWORD PTR [r15+0x8c]
   18027d704:	00 00 
   18027d706:	c4 c1 7a 10 87 a0 00 	vmovss xmm0,DWORD PTR [r15+0xa0]
   18027d70d:	00 00 
   18027d70f:	c4 41 7a 10 87 a4 00 	vmovss xmm8,DWORD PTR [r15+0xa4]
   18027d716:	00 00 
   18027d718:	c4 c1 7a 10 b7 a8 00 	vmovss xmm6,DWORD PTR [r15+0xa8]
   18027d71f:	00 00 
   18027d721:	c4 c1 7a 10 7f 7c    	vmovss xmm7,DWORD PTR [r15+0x7c]
   18027d727:	c4 c1 7a 10 af 88 00 	vmovss xmm5,DWORD PTR [r15+0x88]
   18027d72e:	00 00 
   18027d730:	c4 c1 7a 10 9f 94 00 	vmovss xmm3,DWORD PTR [r15+0x94]
   18027d737:	00 00 
   18027d739:	c4 c1 7a 10 97 80 00 	vmovss xmm2,DWORD PTR [r15+0x80]
   18027d740:	00 00 
   18027d742:	c4 c1 7a 10 a7 98 00 	vmovss xmm4,DWORD PTR [r15+0x98]
   18027d749:	00 00 
   18027d74b:	c5 f2 5a c9          	vcvtss2sd xmm1,xmm1,xmm1
   18027d74f:	c5 fa 5a c0          	vcvtss2sd xmm0,xmm0,xmm0
   18027d753:	c5 da 5a e4          	vcvtss2sd xmm4,xmm4,xmm4
   18027d757:	c4 41 3a 5a c0       	vcvtss2sd xmm8,xmm8,xmm8
   18027d75c:	c5 ca 5a f6          	vcvtss2sd xmm6,xmm6,xmm6
   18027d760:	c5 c2 5a ff          	vcvtss2sd xmm7,xmm7,xmm7
   18027d764:	c5 d2 5a ed          	vcvtss2sd xmm5,xmm5,xmm5
   18027d768:	c5 e2 5a db          	vcvtss2sd xmm3,xmm3,xmm3
   18027d76c:	c5 ea 5a d2          	vcvtss2sd xmm2,xmm2,xmm2
   18027d770:	c5 fb 11 45 a0       	vmovsd QWORD PTR [rbp-0x60],xmm0
   18027d775:	c5 7b 11 45 a8       	vmovsd QWORD PTR [rbp-0x58],xmm8
   18027d77a:	c5 fb 11 75 b0       	vmovsd QWORD PTR [rbp-0x50],xmm6
   18027d77f:	c5 fb 11 7d b8       	vmovsd QWORD PTR [rbp-0x48],xmm7
   18027d784:	c5 fc 10 45 a0       	vmovups ymm0,YMMWORD PTR [rbp-0x60]
   18027d789:	c5 fb 11 6d c0       	vmovsd QWORD PTR [rbp-0x40],xmm5
   18027d78e:	c5 fb 11 5d c8       	vmovsd QWORD PTR [rbp-0x38],xmm3
   18027d793:	c5 fb 11 55 d0       	vmovsd QWORD PTR [rbp-0x30],xmm2
   18027d798:	c5 fb 11 4d d8       	vmovsd QWORD PTR [rbp-0x28],xmm1
   18027d79d:	c5 fc 10 4d c0       	vmovups ymm1,YMMWORD PTR [rbp-0x40]
   18027d7a2:	c5 fc 11 05 16 86 1f 	vmovups YMMWORD PTR [rip+0x1f8616],ymm0        # 0x180475dc0
   18027d7a9:	00 
   18027d7aa:	c5 fc 11 0d 2e 86 1f 	vmovups YMMWORD PTR [rip+0x1f862e],ymm1        # 0x180475de0
   18027d7b1:	00 
   18027d7b2:	c5 fb 11 65 e0       	vmovsd QWORD PTR [rbp-0x20],xmm4
   18027d7b7:	c6 05 1a 85 1f 00 01 	mov    BYTE PTR [rip+0x1f851a],0x1        # 0x180475cd8
   18027d7be:	c5 fb 11 25 3a 86 1f 	vmovsd QWORD PTR [rip+0x1f863a],xmm4        # 0x180475e00
   18027d7c5:	00 
   18027d7c6:	48 89 9d 90 02 00 00 	mov    QWORD PTR [rbp+0x290],rbx
   18027d7cd:	48 89 9d 98 02 00 00 	mov    QWORD PTR [rbp+0x298],rbx
   18027d7d4:	75 1d                	jne    0x18027d7f3
   18027d7d6:	48 8d 8d 90 02 00 00 	lea    rcx,[rbp+0x290]
   18027d7dd:	c5 f8 77             	vzeroupper
   18027d7e0:	ff 15 2a 59 04 00    	call   QWORD PTR [rip+0x4592a]        # 0x1802c3110
   18027d7e6:	48 8d 8d 98 02 00 00 	lea    rcx,[rbp+0x298]
   18027d7ed:	ff 15 15 59 04 00    	call   QWORD PTR [rip+0x45915]        # 0x1802c3108
   18027d7f3:	4c 39 3d e6 84 1f 00 	cmp    QWORD PTR [rip+0x1f84e6],r15        # 0x180475ce0
   18027d7fa:	48 8b 85 90 02 00 00 	mov    rax,QWORD PTR [rbp+0x290]
   18027d801:	48 89 05 88 87 1f 00 	mov    QWORD PTR [rip+0x1f8788],rax        # 0x180475f90
   18027d808:	41 8b 86 78 02 00 00 	mov    eax,DWORD PTR [r14+0x278]
   18027d80f:	89 44 24 40          	mov    DWORD PTR [rsp+0x40],eax
   18027d813:	41 8b 86 7c 02 00 00 	mov    eax,DWORD PTR [r14+0x27c]
   18027d81a:	89 44 24 44          	mov    DWORD PTR [rsp+0x44],eax
   18027d81e:	41 8b 86 70 02 00 00 	mov    eax,DWORD PTR [r14+0x270]
   18027d825:	89 44 24 48          	mov    DWORD PTR [rsp+0x48],eax
   18027d829:	41 8b 86 74 02 00 00 	mov    eax,DWORD PTR [r14+0x274]
   18027d830:	89 44 24 4c          	mov    DWORD PTR [rsp+0x4c],eax
   18027d834:	75 66                	jne    0x18027d89c
   18027d836:	48 39 35 8b 84 1f 00 	cmp    QWORD PTR [rip+0x1f848b],rsi        # 0x180475cc8
   18027d83d:	75 5d                	jne    0x18027d89c
   18027d83f:	48 39 3d 8a 84 1f 00 	cmp    QWORD PTR [rip+0x1f848a],rdi        # 0x180475cd0
   18027d846:	75 54                	jne    0x18027d89c
   18027d848:	41 b8 10 00 00 00    	mov    r8d,0x10
   18027d84e:	48 8d 54 24 40       	lea    rdx,[rsp+0x40]
   18027d853:	48 8d 0d 3e 84 1f 00 	lea    rcx,[rip+0x1f843e]        # 0x180475c98
   18027d85a:	c5 f8 77             	vzeroupper
   18027d85d:	e8 b8 cf fb ff       	call   0x18023a81a
   18027d862:	85 c0                	test   eax,eax
   18027d864:	75 36                	jne    0x18027d89c
   18027d866:	41 b8 1c 00 00 00    	mov    r8d,0x1c
   18027d86c:	48 8d 0d 35 84 1f 00 	lea    rcx,[rip+0x1f8435]        # 0x180475ca8
   18027d873:	49 8b d5             	mov    rdx,r13
   18027d876:	e8 9f cf fb ff       	call   0x18023a81a
   18027d87b:	85 c0                	test   eax,eax
   18027d87d:	75 1d                	jne    0x18027d89c
   18027d87f:	49 8d 75 24          	lea    rsi,[r13+0x24]
   18027d883:	41 b8 10 00 00 00    	mov    r8d,0x10
   18027d889:	48 8b d6             	mov    rdx,rsi
   18027d88c:	48 8d 0d 9d 3a c0 00 	lea    rcx,[rip+0xc03a9d]        # 0x180e81330
   18027d893:	e8 82 cf fb ff       	call   0x18023a81a
   18027d898:	85 c0                	test   eax,eax
   18027d89a:	74 2d                	je     0x18027d8c9
   18027d89c:	66 89 1d 8d 87 1f 00 	mov    WORD PTR [rip+0x1f878d],bx        # 0x180476030
   18027d8a3:	c5 f8 77             	vzeroupper
   18027d8a6:	e8 15 1a 00 00       	call   0x18027f2c0
   18027d8ab:	48 89 1d a6 84 1f 00 	mov    QWORD PTR [rip+0x1f84a6],rbx        # 0x180475d58
   18027d8b2:	c6 05 30 84 1f 00 01 	mov    BYTE PTR [rip+0x1f8430],0x1        # 0x180475ce9
   18027d8b9:	e8 e2 2a ed ff       	call   0x1801503a0
   18027d8be:	49 8d 75 24          	lea    rsi,[r13+0x24]
   18027d8c2:	c6 80 64 02 00 00 01 	mov    BYTE PTR [rax+0x264],0x1
   18027d8c9:	c5 f8 10 44 24 40    	vmovups xmm0,XMMWORD PTR [rsp+0x40]
   18027d8cf:	48 8b 44 24 70       	mov    rax,QWORD PTR [rsp+0x70]
   18027d8d4:	c5 f8 11 05 bc 83 1f 	vmovups XMMWORD PTR [rip+0x1f83bc],xmm0        # 0x180475c98
   18027d8db:	00 
   18027d8dc:	c4 c1 78 10 45 00    	vmovups xmm0,XMMWORD PTR [r13+0x0]
   18027d8e2:	c5 f8 11 05 be 83 1f 	vmovups XMMWORD PTR [rip+0x1f83be],xmm0        # 0x180475ca8
   18027d8e9:	00 
   18027d8ea:	c4 c1 7b 10 4d 10    	vmovsd xmm1,QWORD PTR [r13+0x10]
   18027d8f0:	c5 fb 11 0d c0 83 1f 	vmovsd QWORD PTR [rip+0x1f83c0],xmm1        # 0x180475cb8
   18027d8f7:	00 
   18027d8f8:	48 89 05 c9 83 1f 00 	mov    QWORD PTR [rip+0x1f83c9],rax        # 0x180475cc8
   18027d8ff:	41 8b 45 18          	mov    eax,DWORD PTR [r13+0x18]
   18027d903:	89 05 b7 83 1f 00    	mov    DWORD PTR [rip+0x1f83b7],eax        # 0x180475cc0
   18027d909:	0f b6 05 20 87 1f 00 	movzx  eax,BYTE PTR [rip+0x1f8720]        # 0x180476030
   18027d910:	4c 89 3d c9 83 1f 00 	mov    QWORD PTR [rip+0x1f83c9],r15        # 0x180475ce0
   18027d917:	48 89 3d b2 83 1f 00 	mov    QWORD PTR [rip+0x1f83b2],rdi        # 0x180475cd0
   18027d91e:	c5 f8 10 06          	vmovups xmm0,XMMWORD PTR [rsi]
   18027d922:	c5 f8 11 05 06 3a c0 	vmovups XMMWORD PTR [rip+0xc03a06],xmm0        # 0x180e81330
   18027d929:	00 
   18027d92a:	c4 41 29 57 d2       	vxorpd xmm10,xmm10,xmm10
   18027d92f:	84 c0                	test   al,al
   18027d931:	74 6e                	je     0x18027d9a1
   18027d933:	c5 fb 10 45 c0       	vmovsd xmm0,QWORD PTR [rbp-0x40]
   18027d938:	c5 fb 10 4d b8       	vmovsd xmm1,QWORD PTR [rbp-0x48]
   18027d93d:	c5 f3 59 15 73 86 1f 	vmulsd xmm2,xmm1,QWORD PTR [rip+0x1f8673]        # 0x180475fb8
   18027d944:	00 
   18027d945:	c5 fb 59 1d 73 86 1f 	vmulsd xmm3,xmm0,QWORD PTR [rip+0x1f8673]        # 0x180475fc0
   18027d94c:	00 
   18027d94d:	c5 fb 10 45 c8       	vmovsd xmm0,QWORD PTR [rbp-0x38]
   18027d952:	c5 fb 59 0d 6e 86 1f 	vmulsd xmm1,xmm0,QWORD PTR [rip+0x1f866e]        # 0x180475fc8
   18027d959:	00 
   18027d95a:	c5 e3 58 e2          	vaddsd xmm4,xmm3,xmm2
   18027d95e:	c5 db 58 d1          	vaddsd xmm2,xmm4,xmm1
   18027d962:	c5 f9 2f 15 2e e4 18 	vcomisd xmm2,QWORD PTR [rip+0x18e42e]        # 0x18040bd98
   18027d969:	00 
   18027d96a:	73 2d                	jae    0x18027d999
   18027d96c:	66 89 1d bd 86 1f 00 	mov    WORD PTR [rip+0x1f86bd],bx        # 0x180476030
   18027d973:	e8 48 19 00 00       	call   0x18027f2c0
   18027d978:	48 89 1d d9 83 1f 00 	mov    QWORD PTR [rip+0x1f83d9],rbx        # 0x180475d58
   18027d97f:	c6 05 63 83 1f 00 01 	mov    BYTE PTR [rip+0x1f8363],0x1        # 0x180475ce9
   18027d986:	e8 15 2a ed ff       	call   0x1801503a0
   18027d98b:	c6 80 64 02 00 00 01 	mov    BYTE PTR [rax+0x264],0x1
   18027d992:	0f b6 05 97 86 1f 00 	movzx  eax,BYTE PTR [rip+0x1f8697]        # 0x180476030
   18027d999:	84 c0                	test   al,al
   18027d99b:	0f 85 75 01 00 00    	jne    0x18027db16
   18027d9a1:	38 1d 42 83 1f 00    	cmp    BYTE PTR [rip+0x1f8342],bl        # 0x180475ce9
   18027d9a7:	75 1a                	jne    0x18027d9c3
   18027d9a9:	c6 05 39 83 1f 00 01 	mov    BYTE PTR [rip+0x1f8339],0x1        # 0x180475ce9
   18027d9b0:	e8 eb 29 ed ff       	call   0x1801503a0
   18027d9b5:	c6 80 64 02 00 00 01 	mov    BYTE PTR [rax+0x264],0x1
   18027d9bc:	0f b6 05 6d 86 1f 00 	movzx  eax,BYTE PTR [rip+0x1f866d]        # 0x180476030
   18027d9c3:	84 c0                	test   al,al
   18027d9c5:	0f 85 4b 01 00 00    	jne    0x18027db16
   18027d9cb:	88 1d 9f 85 1f 00    	mov    BYTE PTR [rip+0x1f859f],bl        # 0x180475f70
   18027d9d1:	45 84 e4             	test   r12b,r12b
   18027d9d4:	0f 85 3b 08 00 00    	jne    0x18027e215
   18027d9da:	c5 fa 6f 05 1e ee 18 	vmovdqu xmm0,XMMWORD PTR [rip+0x18ee1e]        # 0x18040c800
   18027d9e1:	00 
   18027d9e2:	c5 f8 11 05 0e 37 1e 	vmovups XMMWORD PTR [rip+0x1e370e],xmm0        # 0x1804610f8
   18027d9e9:	00 
   18027d9ea:	49 bc 00 00 00 00 00 	movabs r12,0x7ff0000000000000
   18027d9f1:	00 f0 7f 
   18027d9f4:	c5 7a 10 0d c8 e2 18 	vmovss xmm9,DWORD PTR [rip+0x18e2c8]        # 0x18040bcc4
   18027d9fb:	00 
   18027d9fc:	c5 78 29 9c 24 d0 02 	vmovaps XMMWORD PTR [rsp+0x2d0],xmm11
   18027da03:	00 00 
   18027da05:	c5 78 29 a4 24 c0 02 	vmovaps XMMWORD PTR [rsp+0x2c0],xmm12
   18027da0c:	00 00 
   18027da0e:	c5 78 29 ac 24 b0 02 	vmovaps XMMWORD PTR [rsp+0x2b0],xmm13
   18027da15:	00 00 
   18027da17:	c5 78 29 b4 24 a0 02 	vmovaps XMMWORD PTR [rsp+0x2a0],xmm14
   18027da1e:	00 00 
   18027da20:	c5 78 29 bc 24 90 02 	vmovaps XMMWORD PTR [rsp+0x290],xmm15
   18027da27:	00 00 
   18027da29:	41 38 9e ad 04 00 00 	cmp    BYTE PTR [r14+0x4ad],bl
   18027da30:	0f 84 a1 11 00 00    	je     0x18027ebd7
   18027da36:	83 3d c3 7c 1f 00 02 	cmp    DWORD PTR [rip+0x1f7cc3],0x2        # 0x180475700
   18027da3d:	0f 85 94 11 00 00    	jne    0x18027ebd7
   18027da43:	48 39 9d 90 02 00 00 	cmp    QWORD PTR [rbp+0x290],rbx
   18027da4a:	0f 8e 87 11 00 00    	jle    0x18027ebd7
   18027da50:	48 39 9d 98 02 00 00 	cmp    QWORD PTR [rbp+0x298],rbx
   18027da57:	0f 8e 7a 11 00 00    	jle    0x18027ebd7
   18027da5d:	c4 41 7a 10 65 00    	vmovss xmm12,DWORD PTR [r13+0x0]
   18027da63:	c4 41 7a 10 6d 04    	vmovss xmm13,DWORD PTR [r13+0x4]
   18027da69:	c4 41 7a 10 75 0c    	vmovss xmm14,DWORD PTR [r13+0xc]
   18027da6f:	c4 41 7a 10 7d 08    	vmovss xmm15,DWORD PTR [r13+0x8]
   18027da75:	c5 7a 11 64 24 50    	vmovss DWORD PTR [rsp+0x50],xmm12
   18027da7b:	c5 7a 11 6c 24 54    	vmovss DWORD PTR [rsp+0x54],xmm13
   18027da81:	c5 7a 11 74 24 58    	vmovss DWORD PTR [rsp+0x58],xmm14
   18027da87:	c5 7a 11 7c 24 5c    	vmovss DWORD PTR [rsp+0x5c],xmm15
   18027da8d:	e8 9e e5 ef ff       	call   0x18017c030
   18027da92:	c5 fb 10 3d 26 83 1f 	vmovsd xmm7,QWORD PTR [rip+0x1f8326]        # 0x180475dc0
   18027da99:	00 
   18027da9a:	c5 7b 10 05 26 83 1f 	vmovsd xmm8,QWORD PTR [rip+0x1f8326]        # 0x180475dc8
   18027daa1:	00 
   18027daa2:	c5 7b 10 0d 26 83 1f 	vmovsd xmm9,QWORD PTR [rip+0x1f8326]        # 0x180475dd0
   18027daa9:	00 
   18027daaa:	c5 bb 5c 15 56 84 1f 	vsubsd xmm2,xmm8,QWORD PTR [rip+0x1f8456]        # 0x180475f08
   18027dab1:	00 
   18027dab2:	c5 b3 5c 1d 56 84 1f 	vsubsd xmm3,xmm9,QWORD PTR [rip+0x1f8456]        # 0x180475f10
   18027dab9:	00 
   18027daba:	c5 7a 5a d8          	vcvtss2sd xmm11,xmm0,xmm0
   18027dabe:	c5 c3 5c 05 3a 84 1f 	vsubsd xmm0,xmm7,QWORD PTR [rip+0x1f843a]        # 0x180475f00
   18027dac5:	00 
   18027dac6:	c5 7b 11 5c 24 40    	vmovsd QWORD PTR [rsp+0x40],xmm11
   18027dacc:	48 8b 44 24 40       	mov    rax,QWORD PTR [rsp+0x40]
   18027dad1:	49 23 c4             	and    rax,r12
   18027dad4:	49 3b c4             	cmp    rax,r12
   18027dad7:	0f 84 8b 08 00 00    	je     0x18027e368
   18027dadd:	c4 41 79 2f da       	vcomisd xmm11,xmm10
   18027dae2:	0f 86 80 08 00 00    	jbe    0x18027e368
   18027dae8:	c5 7b 10 15 b0 e2 18 	vmovsd xmm10,QWORD PTR [rip+0x18e2b0]        # 0x18040bda0
   18027daef:	00 
   18027daf0:	c5 fb 59 c8          	vmulsd xmm1,xmm0,xmm0
   18027daf4:	c5 eb 59 c2          	vmulsd xmm0,xmm2,xmm2
   18027daf8:	c5 f3 58 d0          	vaddsd xmm2,xmm1,xmm0
   18027dafc:	c5 e3 59 cb          	vmulsd xmm1,xmm3,xmm3
   18027db00:	c5 eb 58 d1          	vaddsd xmm2,xmm2,xmm1
   18027db04:	c5 eb 51 da          	vsqrtsd xmm3,xmm2,xmm2
   18027db08:	c4 c1 2b 5e c3       	vdivsd xmm0,xmm10,xmm11
   18027db0d:	c5 e3 59 f0          	vmulsd xmm6,xmm3,xmm0
   18027db11:	e9 62 08 00 00       	jmp    0x18027e378
   18027db16:	45 84 e4             	test   r12b,r12b
   18027db19:	0f 84 bb fe ff ff    	je     0x18027d9da
   18027db1f:	83 3d da 7b 1f 00 02 	cmp    DWORD PTR [rip+0x1f7bda],0x2        # 0x180475700
   18027db26:	c4 c1 7a 10 55 00    	vmovss xmm2,DWORD PTR [r13+0x0]
   18027db2c:	c4 c1 7a 10 5d 04    	vmovss xmm3,DWORD PTR [r13+0x4]
   18027db32:	c4 c1 7a 10 65 0c    	vmovss xmm4,DWORD PTR [r13+0xc]
   18027db38:	c4 c1 7a 10 6d 08    	vmovss xmm5,DWORD PTR [r13+0x8]
   18027db3e:	c5 fa 11 54 24 50    	vmovss DWORD PTR [rsp+0x50],xmm2
   18027db44:	c5 fa 11 5c 24 54    	vmovss DWORD PTR [rsp+0x54],xmm3
   18027db4a:	c5 fa 11 64 24 58    	vmovss DWORD PTR [rsp+0x58],xmm4
   18027db50:	c5 fa 11 6c 24 5c    	vmovss DWORD PTR [rsp+0x5c],xmm5
   18027db56:	0f 85 4a 06 00 00    	jne    0x18027e1a6
   18027db5c:	c5 fc 10 05 3c 84 1f 	vmovups ymm0,YMMWORD PTR [rip+0x1f843c]        # 0x180475fa0
   18027db63:	00 
   18027db64:	c5 fc 10 0d 54 84 1f 	vmovups ymm1,YMMWORD PTR [rip+0x1f8454]        # 0x180475fc0
   18027db6b:	00 
   18027db6c:	0f b6 05 b5 7b 1f 00 	movzx  eax,BYTE PTR [rip+0x1f7bb5]        # 0x180475728
   18027db73:	c5 fc 11 85 20 01 00 	vmovups YMMWORD PTR [rbp+0x120],ymm0
   18027db7a:	00 
   18027db7b:	c5 fb 10 05 5d 84 1f 	vmovsd xmm0,QWORD PTR [rip+0x1f845d]        # 0x180475fe0
   18027db82:	00 
   18027db83:	c5 fb 11 85 60 01 00 	vmovsd QWORD PTR [rbp+0x160],xmm0
   18027db8a:	00 
   18027db8b:	c5 fc 11 8d 40 01 00 	vmovups YMMWORD PTR [rbp+0x140],ymm1
   18027db92:	00 
   18027db93:	90                   	nop
   18027db94:	84 c0                	test   al,al
   18027db96:	74 1a                	je     0x18027dbb2
   18027db98:	c5 f8 10 45 a0       	vmovups xmm0,XMMWORD PTR [rbp-0x60]
   18027db9d:	c5 fb 10 4d b0       	vmovsd xmm1,QWORD PTR [rbp-0x50]
   18027dba2:	c5 f8 11 85 20 01 00 	vmovups XMMWORD PTR [rbp+0x120],xmm0
   18027dba9:	00 
   18027dbaa:	c5 fb 11 8d 30 01 00 	vmovsd QWORD PTR [rbp+0x130],xmm1
   18027dbb1:	00 
   18027dbb2:	c4 c1 7a 10 b6 b8 04 	vmovss xmm6,DWORD PTR [r14+0x4b8]
   18027dbb9:	00 00 
   18027dbbb:	c5 fa 11 54 24 40    	vmovss DWORD PTR [rsp+0x40],xmm2
   18027dbc1:	c5 fa 11 5c 24 44    	vmovss DWORD PTR [rsp+0x44],xmm3
   18027dbc7:	c5 fa 11 64 24 48    	vmovss DWORD PTR [rsp+0x48],xmm4
   18027dbcd:	c5 fa 11 6c 24 4c    	vmovss DWORD PTR [rsp+0x4c],xmm5
   18027dbd3:	c5 f8 77             	vzeroupper
   18027dbd6:	e8 55 e4 ef ff       	call   0x18017c030
   18027dbdb:	c5 7a 10 05 6d e2 18 	vmovss xmm8,DWORD PTR [rip+0x18e26d]        # 0x18040be50
   18027dbe2:	00 
   18027dbe3:	48 89 5c 24 38       	mov    QWORD PTR [rsp+0x38],rbx
   18027dbe8:	4c 8d 4c 24 40       	lea    r9,[rsp+0x40]
   18027dbed:	c5 7a 11 44 24 30    	vmovss DWORD PTR [rsp+0x30],xmm8
   18027dbf3:	c5 fa 5a c8          	vcvtss2sd xmm1,xmm0,xmm0
   18027dbf7:	c5 fa 11 74 24 28    	vmovss DWORD PTR [rsp+0x28],xmm6
   18027dbfd:	4c 8d 45 a0          	lea    r8,[rbp-0x60]
   18027dc01:	48 8d 95 20 01 00 00 	lea    rdx,[rbp+0x120]
   18027dc08:	48 8d 4d 80          	lea    rcx,[rbp-0x80]
   18027dc0c:	c5 fb 11 4c 24 20    	vmovsd QWORD PTR [rsp+0x20],xmm1
   18027dc12:	e8 a9 d6 ef ff       	call   0x18017b2c0
   18027dc17:	48 8b 8d 98 02 00 00 	mov    rcx,QWORD PTR [rbp+0x298]
   18027dc1e:	48 8b bd 90 02 00 00 	mov    rdi,QWORD PTR [rbp+0x290]
   18027dc25:	48 6b d1 4b          	imul   rdx,rcx,0x4b
   18027dc29:	c5 f8 10 00          	vmovups xmm0,XMMWORD PTR [rax]
   18027dc2d:	48 b8 cf f7 53 e3 a5 	movabs rax,0x20c49ba5e353f7cf
   18027dc34:	9b c4 20 
   18027dc37:	4c 8b c7             	mov    r8,rdi
   18027dc3a:	4c 2b 05 f7 83 1f 00 	sub    r8,QWORD PTR [rip+0x1f83f7]        # 0x180476038
   18027dc41:	48 f7 ea             	imul   rdx
   18027dc44:	4c 89 45 58          	mov    QWORD PTR [rbp+0x58],r8
   18027dc48:	4c 8b ca             	mov    r9,rdx
   18027dc4b:	49 c1 f9 07          	sar    r9,0x7
   18027dc4f:	49 8b c1             	mov    rax,r9
   18027dc52:	48 c1 e8 3f          	shr    rax,0x3f
   18027dc56:	4c 03 c8             	add    r9,rax
   18027dc59:	48 b8 89 88 88 88 88 	movabs rax,0x8888888888888889
   18027dc60:	88 88 88 
   18027dc63:	48 f7 e9             	imul   rcx
   18027dc66:	4c 89 4d 60          	mov    QWORD PTR [rbp+0x60],r9
   18027dc6a:	48 03 d1             	add    rdx,rcx
   18027dc6d:	48 8d 4d 50          	lea    rcx,[rbp+0x50]
   18027dc71:	48 c1 fa 07          	sar    rdx,0x7
   18027dc75:	48 8b c2             	mov    rax,rdx
   18027dc78:	48 c1 e8 3f          	shr    rax,0x3f
   18027dc7c:	48 03 d0             	add    rdx,rax
   18027dc7f:	48 8d 45 58          	lea    rax,[rbp+0x58]
   18027dc83:	4c 3b c2             	cmp    r8,rdx
   18027dc86:	48 89 55 50          	mov    QWORD PTR [rbp+0x50],rdx
   18027dc8a:	c5 f8 11 05 66 34 1e 	vmovups XMMWORD PTR [rip+0x1e3466],xmm0        # 0x1804610f8
   18027dc91:	00 
   18027dc92:	48 0f 4d c8          	cmovge rcx,rax
   18027dc96:	4d 3b c8             	cmp    r9,r8
   18027dc99:	48 8d 45 60          	lea    rax,[rbp+0x60]
   18027dc9d:	48 0f 4d c1          	cmovge rax,rcx
   18027dca1:	c4 41 78 2f 8e b8 04 	vcomiss xmm9,DWORD PTR [r14+0x4b8]
   18027dca8:	00 00 
   18027dcaa:	48 8b 00             	mov    rax,QWORD PTR [rax]
   18027dcad:	48 89 44 24 70       	mov    QWORD PTR [rsp+0x70],rax
   18027dcb2:	0f 83 5d 05 00 00    	jae    0x18027e215
   18027dcb8:	48 89 45 90          	mov    QWORD PTR [rbp-0x70],rax
   18027dcbc:	48 8b 05 0d f8 1f 00 	mov    rax,QWORD PTR [rip+0x1ff80d]        # 0x18047d4d0
   18027dcc3:	0f b6 88 40 01 00 00 	movzx  ecx,BYTE PTR [rax+0x140]
   18027dcca:	90                   	nop
   18027dccb:	84 c9                	test   cl,cl
   18027dccd:	0f 84 42 05 00 00    	je     0x18027e215
   18027dcd3:	48 8b 05 f6 f7 1f 00 	mov    rax,QWORD PTR [rip+0x1ff7f6]        # 0x18047d4d0
   18027dcda:	0f b6 88 41 01 00 00 	movzx  ecx,BYTE PTR [rax+0x141]
   18027dce1:	90                   	nop
   18027dce2:	84 c9                	test   cl,cl
   18027dce4:	0f 84 2b 05 00 00    	je     0x18027e215
   18027dcea:	4c 8b 25 df f7 1f 00 	mov    r12,QWORD PTR [rip+0x1ff7df]        # 0x18047d4d0
   18027dcf1:	49 81 c4 f0 00 00 00 	add    r12,0xf0
   18027dcf8:	49 8b cc             	mov    rcx,r12
   18027dcfb:	4c 89 64 24 40       	mov    QWORD PTR [rsp+0x40],r12
   18027dd00:	ff 15 d2 59 04 00    	call   QWORD PTR [rip+0x459d2]        # 0x1802c36d8
   18027dd06:	85 c0                	test   eax,eax
   18027dd08:	0f 85 07 05 00 00    	jne    0x18027e215
   18027dd0e:	48 8b 15 bb f7 1f 00 	mov    rdx,QWORD PTR [rip+0x1ff7bb]        # 0x18047d4d0
   18027dd15:	48 39 9a 68 3e 01 00 	cmp    QWORD PTR [rdx+0x13e68],rbx
   18027dd1c:	0f 84 76 04 00 00    	je     0x18027e198
   18027dd22:	48 39 9a 70 3e 01 00 	cmp    QWORD PTR [rdx+0x13e70],rbx
   18027dd29:	0f 8e 69 04 00 00    	jle    0x18027e198
   18027dd2f:	48 8b 82 60 3e 01 00 	mov    rax,QWORD PTR [rdx+0x13e60]
   18027dd36:	48 ff c8             	dec    rax
   18027dd39:	83 e0 1f             	and    eax,0x1f
   18027dd3c:	48 6b c8 68          	imul   rcx,rax,0x68
   18027dd40:	c5 fc 10 8c 11 80 31 	vmovups ymm1,YMMWORD PTR [rcx+rdx*1+0x13180]
   18027dd47:	01 00 
   18027dd49:	c5 fc 10 a4 11 a0 31 	vmovups ymm4,YMMWORD PTR [rcx+rdx*1+0x131a0]
   18027dd50:	01 00 
   18027dd52:	c5 fc 10 94 11 60 31 	vmovups ymm2,YMMWORD PTR [rcx+rdx*1+0x13160]
   18027dd59:	01 00 
   18027dd5b:	c5 fb 10 9c 11 c0 31 	vmovsd xmm3,QWORD PTR [rcx+rdx*1+0x131c0]
   18027dd62:	01 00 
   18027dd64:	c4 e3 7d 19 c8 01    	vextractf128 xmm0,ymm1,0x1
   18027dd6a:	c5 f9 7e c0          	vmovd  eax,xmm0
   18027dd6e:	c5 fc 11 8d 00 01 00 	vmovups YMMWORD PTR [rbp+0x100],ymm1
   18027dd75:	00 
   18027dd76:	c5 fc 11 a5 70 01 00 	vmovups YMMWORD PTR [rbp+0x170],ymm4
   18027dd7d:	00 
   18027dd7e:	84 c0                	test   al,al
   18027dd80:	0f 84 12 04 00 00    	je     0x18027e198
   18027dd86:	48 8b 82 48 01 00 00 	mov    rax,QWORD PTR [rdx+0x148]
   18027dd8d:	90                   	nop
   18027dd8e:	c4 c1 f9 7e d3       	vmovq  r11,xmm2
   18027dd93:	4c 3b d8             	cmp    r11,rax
   18027dd96:	0f 85 fc 03 00 00    	jne    0x18027e198
   18027dd9c:	c4 e3 f9 16 ce 01    	vpextrq rsi,xmm1,0x1
   18027dda2:	48 3b fe             	cmp    rdi,rsi
   18027dda5:	0f 8c ed 03 00 00    	jl     0x18027e198
   18027ddab:	4c 8b 0d 1e f7 1f 00 	mov    r9,QWORD PTR [rip+0x1ff71e]        # 0x18047d4d0
   18027ddb2:	48 b8 67 66 66 66 66 	movabs rax,0x6666666666666667
   18027ddb9:	66 66 66 
   18027ddbc:	48 2b fe             	sub    rdi,rsi
   18027ddbf:	4d 8b 91 70 3e 01 00 	mov    r10,QWORD PTR [r9+0x13e70]
   18027ddc6:	49 f7 ea             	imul   r10
   18027ddc9:	48 c1 fa 03          	sar    rdx,0x3
   18027ddcd:	48 8b ca             	mov    rcx,rdx
   18027ddd0:	48 c1 e9 3f          	shr    rcx,0x3f
   18027ddd4:	48 03 d1             	add    rdx,rcx
   18027ddd7:	48 3b fa             	cmp    rdi,rdx
   18027ddda:	0f 8f b8 03 00 00    	jg     0x18027e198
   18027dde0:	49 8b b9 68 3e 01 00 	mov    rdi,QWORD PTR [r9+0x13e68]
   18027dde7:	48 b8 cf f7 53 e3 a5 	movabs rax,0x20c49ba5e353f7cf
   18027ddee:	9b c4 20 
   18027ddf1:	49 6b ca 4b          	imul   rcx,r10,0x4b
   18027ddf5:	c5 fc 11 55 70       	vmovups YMMWORD PTR [rbp+0x70],ymm2
   18027ddfa:	48 f7 e9             	imul   rcx
   18027ddfd:	48 b8 89 88 88 88 88 	movabs rax,0x8888888888888889
   18027de04:	88 88 88 
   18027de07:	4c 8b c2             	mov    r8,rdx
   18027de0a:	49 f7 ea             	imul   r10
   18027de0d:	49 c1 f8 07          	sar    r8,0x7
   18027de11:	49 03 d2             	add    rdx,r10
   18027de14:	49 8b c8             	mov    rcx,r8
   18027de17:	48 c1 e9 3f          	shr    rcx,0x3f
   18027de1b:	4c 03 c1             	add    r8,rcx
   18027de1e:	48 c1 fa 07          	sar    rdx,0x7
   18027de22:	48 8b c2             	mov    rax,rdx
   18027de25:	4c 89 45 80          	mov    QWORD PTR [rbp-0x80],r8
   18027de29:	48 c1 e8 3f          	shr    rax,0x3f
   18027de2d:	48 8d 4d 68          	lea    rcx,[rbp+0x68]
   18027de31:	48 03 d0             	add    rdx,rax
   18027de34:	48 8d 45 90          	lea    rax,[rbp-0x70]
   18027de38:	48 39 54 24 70       	cmp    QWORD PTR [rsp+0x70],rdx
   18027de3d:	48 89 55 68          	mov    QWORD PTR [rbp+0x68],rdx
   18027de41:	48 0f 4d c8          	cmovge rcx,rax
   18027de45:	48 8d 45 80          	lea    rax,[rbp-0x80]
   18027de49:	4c 3b 44 24 70       	cmp    r8,QWORD PTR [rsp+0x70]
   18027de4e:	41 b8 01 00 00 00    	mov    r8d,0x1
   18027de54:	c5 fc 11 a5 e0 00 00 	vmovups YMMWORD PTR [rbp+0xe0],ymm4
   18027de5b:	00 
   18027de5c:	48 0f 4d c1          	cmovge rax,rcx
   18027de60:	c5 fb 11 9d d0 00 00 	vmovsd QWORD PTR [rbp+0xd0],xmm3
   18027de67:	00 
   18027de68:	4c 8b 20             	mov    r12,QWORD PTR [rax]
   18027de6b:	49 3b f8             	cmp    rdi,r8
   18027de6e:	0f 86 96 00 00 00    	jbe    0x18027df0a
   18027de74:	49 8b 91 60 3e 01 00 	mov    rdx,QWORD PTR [r9+0x13e60]
   18027de7b:	48 83 ea 02          	sub    rdx,0x2
   18027de7f:	90                   	nop
   18027de80:	48 8b c2             	mov    rax,rdx
   18027de83:	83 e0 1f             	and    eax,0x1f
   18027de86:	48 6b c8 68          	imul   rcx,rax,0x68
   18027de8a:	42 38 9c 09 90 31 01 	cmp    BYTE PTR [rcx+r9*1+0x13190],bl
   18027de91:	00 
   18027de92:	74 76                	je     0x18027df0a
   18027de94:	4e 39 9c 09 60 31 01 	cmp    QWORD PTR [rcx+r9*1+0x13160],r11
   18027de9b:	00 
   18027de9c:	75 6c                	jne    0x18027df0a
   18027de9e:	4e 8b 94 09 88 31 01 	mov    r10,QWORD PTR [rcx+r9*1+0x13188]
   18027dea5:	00 
   18027dea6:	4c 3b d6             	cmp    r10,rsi
   18027dea9:	7d 5f                	jge    0x18027df0a
   18027deab:	c4 a1 7c 10 84 09 60 	vmovups ymm0,YMMWORD PTR [rcx+r9*1+0x13160]
   18027deb2:	31 01 00 
   18027deb5:	c5 fc 11 45 70       	vmovups YMMWORD PTR [rbp+0x70],ymm0
   18027deba:	c4 a1 7c 10 84 09 80 	vmovups ymm0,YMMWORD PTR [rcx+r9*1+0x13180]
   18027dec1:	31 01 00 
   18027dec4:	c5 fc 11 85 00 01 00 	vmovups YMMWORD PTR [rbp+0x100],ymm0
   18027decb:	00 
   18027decc:	c4 a1 7c 10 84 09 a0 	vmovups ymm0,YMMWORD PTR [rcx+r9*1+0x131a0]
   18027ded3:	31 01 00 
   18027ded6:	48 8b c6             	mov    rax,rsi
   18027ded9:	c5 fc 11 85 e0 00 00 	vmovups YMMWORD PTR [rbp+0xe0],ymm0
   18027dee0:	00 
   18027dee1:	c4 a1 7b 10 84 09 c0 	vmovsd xmm0,QWORD PTR [rcx+r9*1+0x131c0]
   18027dee8:	31 01 00 
   18027deeb:	49 2b c2             	sub    rax,r10
   18027deee:	c5 fb 11 85 d0 00 00 	vmovsd QWORD PTR [rbp+0xd0],xmm0
   18027def5:	00 
   18027def6:	49 3b c4             	cmp    rax,r12
   18027def9:	7d 0f                	jge    0x18027df0a
   18027defb:	49 ff c0             	inc    r8
   18027defe:	48 ff ca             	dec    rdx
   18027df01:	4c 3b c7             	cmp    r8,rdi
   18027df04:	0f 82 76 ff ff ff    	jb     0x18027de80
   18027df0a:	48 8b 4c 24 40       	mov    rcx,QWORD PTR [rsp+0x40]
   18027df0f:	c5 f8 77             	vzeroupper
   18027df12:	ff 15 50 57 04 00    	call   QWORD PTR [rip+0x45750]        # 0x1802c3668
   18027df18:	c5 fc 10 ad e0 00 00 	vmovups ymm5,YMMWORD PTR [rbp+0xe0]
   18027df1f:	00 
   18027df20:	c5 fb 10 5d b0       	vmovsd xmm3,QWORD PTR [rbp-0x50]
   18027df25:	c5 f8 10 65 a0       	vmovups xmm4,XMMWORD PTR [rbp-0x60]
   18027df2a:	c5 d0 c6 c5 ff       	vshufps xmm0,xmm5,xmm5,0xff
   18027df2f:	c5 fa 5a c0          	vcvtss2sd xmm0,xmm0,xmm0
   18027df33:	c5 fb 11 85 88 00 00 	vmovsd QWORD PTR [rbp+0x88],xmm0
   18027df3a:	00 
   18027df3b:	c4 e3 7d 19 e9 01    	vextractf128 xmm1,ymm5,0x1
   18027df41:	c5 f2 5a c1          	vcvtss2sd xmm0,xmm1,xmm1
   18027df45:	c5 fb 11 85 90 00 00 	vmovsd QWORD PTR [rbp+0x90],xmm0
   18027df4c:	00 
   18027df4d:	c5 d0 c6 cd 55       	vshufps xmm1,xmm5,xmm5,0x55
   18027df52:	c4 e3 7d 19 ea 01    	vextractf128 xmm2,ymm5,0x1
   18027df58:	c5 e8 c6 d2 55       	vshufps xmm2,xmm2,xmm2,0x55
   18027df5d:	c5 ea 5a c2          	vcvtss2sd xmm0,xmm2,xmm2
   18027df61:	c5 fb 11 85 98 00 00 	vmovsd QWORD PTR [rbp+0x98],xmm0
   18027df68:	00 
   18027df69:	c5 d2 5a c5          	vcvtss2sd xmm0,xmm5,xmm5
   18027df6d:	c5 fb 11 85 a0 00 00 	vmovsd QWORD PTR [rbp+0xa0],xmm0
   18027df74:	00 
   18027df75:	c5 f2 5a c1          	vcvtss2sd xmm0,xmm1,xmm1
   18027df79:	c5 fb 11 85 a8 00 00 	vmovsd QWORD PTR [rbp+0xa8],xmm0
   18027df80:	00 
   18027df81:	c5 fb 11 9d 80 00 00 	vmovsd QWORD PTR [rbp+0x80],xmm3
   18027df88:	00 
   18027df89:	c5 fb 11 5d 10       	vmovsd QWORD PTR [rbp+0x10],xmm3
   18027df8e:	c5 fc 10 9d 70 01 00 	vmovups ymm3,YMMWORD PTR [rbp+0x170]
   18027df95:	00 
   18027df96:	c4 e3 7d 19 d9 01    	vextractf128 xmm1,ymm3,0x1
   18027df9c:	c4 e3 7d 19 da 01    	vextractf128 xmm2,ymm3,0x1
   18027dfa2:	c5 e8 c6 d2 55       	vshufps xmm2,xmm2,xmm2,0x55
   18027dfa7:	c5 d0 c6 ed aa       	vshufps xmm5,xmm5,xmm5,0xaa
   18027dfac:	c5 d2 5a c5          	vcvtss2sd xmm0,xmm5,xmm5
   18027dfb0:	c5 fb 11 85 b0 00 00 	vmovsd QWORD PTR [rbp+0xb0],xmm0
   18027dfb7:	00 
   18027dfb8:	c5 e0 c6 c3 ff       	vshufps xmm0,xmm3,xmm3,0xff
   18027dfbd:	c5 fa 5a c0          	vcvtss2sd xmm0,xmm0,xmm0
   18027dfc1:	c5 fb 11 45 18       	vmovsd QWORD PTR [rbp+0x18],xmm0
   18027dfc6:	c5 f2 5a c1          	vcvtss2sd xmm0,xmm1,xmm1
   18027dfca:	c5 fb 11 45 20       	vmovsd QWORD PTR [rbp+0x20],xmm0
   18027dfcf:	c5 ea 5a c2          	vcvtss2sd xmm0,xmm2,xmm2
   18027dfd3:	c5 fb 11 45 28       	vmovsd QWORD PTR [rbp+0x28],xmm0
   18027dfd8:	c5 e2 5a c3          	vcvtss2sd xmm0,xmm3,xmm3
   18027dfdc:	c5 fb 11 45 30       	vmovsd QWORD PTR [rbp+0x30],xmm0
   18027dfe1:	c5 e0 c6 cb 55       	vshufps xmm1,xmm3,xmm3,0x55
   18027dfe6:	c5 f2 5a c1          	vcvtss2sd xmm0,xmm1,xmm1
   18027dfea:	c4 c1 7a 10 8e b8 04 	vmovss xmm1,DWORD PTR [r14+0x4b8]
   18027dff1:	00 00 
   18027dff3:	c5 fb 11 45 38       	vmovsd QWORD PTR [rbp+0x38],xmm0
   18027dff8:	c5 f2 5a c9          	vcvtss2sd xmm1,xmm1,xmm1
   18027dffc:	c5 e0 c6 db aa       	vshufps xmm3,xmm3,xmm3,0xaa
   18027e001:	c5 e2 5a c3          	vcvtss2sd xmm0,xmm3,xmm3
   18027e005:	c5 fb 11 45 40       	vmovsd QWORD PTR [rbp+0x40],xmm0
   18027e00a:	c5 fc 10 85 00 01 00 	vmovups ymm0,YMMWORD PTR [rbp+0x100]
   18027e011:	00 
   18027e012:	c4 e3 f9 16 c0 01    	vpextrq rax,xmm0,0x1
   18027e018:	c5 f9 57 c0          	vxorpd xmm0,xmm0,xmm0
   18027e01c:	c4 e1 fb 2a 44 24 70 	vcvtsi2sd xmm0,xmm0,QWORD PTR [rsp+0x70]
   18027e023:	c5 f3 59 d0          	vmulsd xmm2,xmm1,xmm0
   18027e027:	c5 f9 57 c0          	vxorpd xmm0,xmm0,xmm0
   18027e02b:	4c 8b c6             	mov    r8,rsi
   18027e02e:	4c 2b c0             	sub    r8,rax
   18027e031:	48 8b 85 90 02 00 00 	mov    rax,QWORD PTR [rbp+0x290]
   18027e038:	48 2b c6             	sub    rax,rsi
   18027e03b:	c4 e1 fb 2a c0       	vcvtsi2sd xmm0,xmm0,rax
   18027e040:	c5 eb 58 c8          	vaddsd xmm1,xmm2,xmm0
   18027e044:	c5 e9 57 d2          	vxorpd xmm2,xmm2,xmm2
   18027e048:	c4 e1 eb 2a 95 98 02 	vcvtsi2sd xmm2,xmm2,QWORD PTR [rbp+0x298]
   18027e04f:	00 00 
   18027e051:	c5 eb 59 05 0f dd 18 	vmulsd xmm0,xmm2,QWORD PTR [rip+0x18dd0f]        # 0x18040bd68
   18027e058:	00 
   18027e059:	c5 fb 11 45 80       	vmovsd QWORD PTR [rbp-0x80],xmm0
   18027e05e:	c5 fb 11 4c 24 40    	vmovsd QWORD PTR [rsp+0x40],xmm1
   18027e064:	c5 f8 11 65 70       	vmovups XMMWORD PTR [rbp+0x70],xmm4
   18027e069:	c5 f8 11 65 00       	vmovups XMMWORD PTR [rbp+0x0],xmm4
   18027e06e:	4d 85 c0             	test   r8,r8
   18027e071:	7e 28                	jle    0x18027e09b
   18027e073:	48 8d 54 24 40       	lea    rdx,[rsp+0x40]
   18027e078:	48 8d 4d 80          	lea    rcx,[rbp-0x80]
   18027e07c:	c5 f8 77             	vzeroupper
   18027e07f:	e8 dc 95 ed ff       	call   0x180157660
   18027e084:	c5 f1 57 c9          	vxorpd xmm1,xmm1,xmm1
   18027e088:	c4 c1 f3 2a c8       	vcvtsi2sd xmm1,xmm1,r8
   18027e08d:	c5 fb 10 00          	vmovsd xmm0,QWORD PTR [rax]
   18027e091:	c5 fb 5e c9          	vdivsd xmm1,xmm0,xmm1
   18027e095:	c5 f3 5a d1          	vcvtsd2ss xmm2,xmm1,xmm1
   18027e099:	eb 05                	jmp    0x18027e0a0
   18027e09b:	c4 c1 78 28 d1       	vmovaps xmm2,xmm9
   18027e0a0:	c5 f8 10 7c 24 50    	vmovups xmm7,XMMWORD PTR [rsp+0x50]
   18027e0a6:	c5 7a 11 4d f0       	vmovss DWORD PTR [rbp-0x10],xmm9
   18027e0ab:	c5 fa 11 55 f4       	vmovss DWORD PTR [rbp-0xc],xmm2
   18027e0b0:	48 8d 7d f0          	lea    rdi,[rbp-0x10]
   18027e0b4:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
   18027e0b8:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
   18027e0bf:	00 
   18027e0c0:	c5 fa 10 37          	vmovss xmm6,DWORD PTR [rdi]
   18027e0c4:	c5 f9 7f 7c 24 40    	vmovdqa XMMWORD PTR [rsp+0x40],xmm7
   18027e0ca:	c5 f8 77             	vzeroupper
   18027e0cd:	e8 5e df ef ff       	call   0x18017c030
   18027e0d2:	48 8d 45 a0          	lea    rax,[rbp-0x60]
   18027e0d6:	48 89 44 24 38       	mov    QWORD PTR [rsp+0x38],rax
   18027e0db:	4c 8d 4c 24 40       	lea    r9,[rsp+0x40]
   18027e0e0:	c5 7a 11 44 24 30    	vmovss DWORD PTR [rsp+0x30],xmm8
   18027e0e6:	c5 fa 5a c8          	vcvtss2sd xmm1,xmm0,xmm0
   18027e0ea:	c5 fa 11 74 24 28    	vmovss DWORD PTR [rsp+0x28],xmm6
   18027e0f0:	4c 8d 45 00          	lea    r8,[rbp+0x0]
   18027e0f4:	48 8d 55 70          	lea    rdx,[rbp+0x70]
   18027e0f8:	48 8d 4d 80          	lea    rcx,[rbp-0x80]
   18027e0fc:	c5 fb 11 4c 24 20    	vmovsd QWORD PTR [rsp+0x20],xmm1
   18027e102:	e8 b9 d1 ef ff       	call   0x18017b2c0
   18027e107:	48 8d 54 24 50       	lea    rdx,[rsp+0x50]
   18027e10c:	48 8d 0d e5 2f 1e 00 	lea    rcx,[rip+0x1e2fe5]        # 0x1804610f8
   18027e113:	c5 f8 10 00          	vmovups xmm0,XMMWORD PTR [rax]
   18027e117:	c5 f8 11 44 24 50    	vmovups XMMWORD PTR [rsp+0x50],xmm0
   18027e11d:	e8 3e 2c eb ff       	call   0x180130d60
   18027e122:	48 8d 54 24 54       	lea    rdx,[rsp+0x54]
   18027e127:	48 8d 0d ce 2f 1e 00 	lea    rcx,[rip+0x1e2fce]        # 0x1804610fc
   18027e12e:	c5 fa 10 00          	vmovss xmm0,DWORD PTR [rax]
   18027e132:	c5 fa 11 05 be 2f 1e 	vmovss DWORD PTR [rip+0x1e2fbe],xmm0        # 0x1804610f8
   18027e139:	00 
   18027e13a:	e8 21 2c eb ff       	call   0x180130d60
   18027e13f:	48 8d 54 24 58       	lea    rdx,[rsp+0x58]
   18027e144:	48 8d 0d b5 2f 1e 00 	lea    rcx,[rip+0x1e2fb5]        # 0x180461100
   18027e14b:	c5 fa 10 00          	vmovss xmm0,DWORD PTR [rax]
   18027e14f:	c5 fa 11 05 a5 2f 1e 	vmovss DWORD PTR [rip+0x1e2fa5],xmm0        # 0x1804610fc
   18027e156:	00 
   18027e157:	e8 f4 2b eb ff       	call   0x180130d50
   18027e15c:	48 8d 54 24 5c       	lea    rdx,[rsp+0x5c]
   18027e161:	48 8d 0d 9c 2f 1e 00 	lea    rcx,[rip+0x1e2f9c]        # 0x180461104
   18027e168:	c5 fa 10 00          	vmovss xmm0,DWORD PTR [rax]
   18027e16c:	c5 fa 11 05 8c 2f 1e 	vmovss DWORD PTR [rip+0x1e2f8c],xmm0        # 0x180461100
   18027e173:	00 
   18027e174:	e8 d7 2b eb ff       	call   0x180130d50
   18027e179:	48 83 c7 04          	add    rdi,0x4
   18027e17d:	c5 fa 10 00          	vmovss xmm0,DWORD PTR [rax]
   18027e181:	48 8d 45 f8          	lea    rax,[rbp-0x8]
   18027e185:	c5 fa 11 05 77 2f 1e 	vmovss DWORD PTR [rip+0x1e2f77],xmm0        # 0x180461104
   18027e18c:	00 
   18027e18d:	48 3b f8             	cmp    rdi,rax
   18027e190:	0f 85 2a ff ff ff    	jne    0x18027e0c0
   18027e196:	eb 7d                	jmp    0x18027e215
   18027e198:	49 8b cc             	mov    rcx,r12
   18027e19b:	c5 f8 77             	vzeroupper
   18027e19e:	ff 15 c4 54 04 00    	call   QWORD PTR [rip+0x454c4]        # 0x1802c3668
   18027e1a4:	eb 6f                	jmp    0x18027e215
   18027e1a6:	c4 c1 7a 10 b6 b8 04 	vmovss xmm6,DWORD PTR [r14+0x4b8]
   18027e1ad:	00 00 
   18027e1af:	c5 fa 11 54 24 50    	vmovss DWORD PTR [rsp+0x50],xmm2
   18027e1b5:	c5 fa 11 5c 24 54    	vmovss DWORD PTR [rsp+0x54],xmm3
   18027e1bb:	c5 fa 11 64 24 58    	vmovss DWORD PTR [rsp+0x58],xmm4
   18027e1c1:	c5 fa 11 6c 24 5c    	vmovss DWORD PTR [rsp+0x5c],xmm5
   18027e1c7:	e8 64 de ef ff       	call   0x18017c030
   18027e1cc:	c5 fa 10 0d 8c db 18 	vmovss xmm1,DWORD PTR [rip+0x18db8c]        # 0x18040bd60
   18027e1d3:	00 
   18027e1d4:	48 89 5c 24 38       	mov    QWORD PTR [rsp+0x38],rbx
   18027e1d9:	4c 8d 4c 24 50       	lea    r9,[rsp+0x50]
   18027e1de:	c5 fa 11 4c 24 30    	vmovss DWORD PTR [rsp+0x30],xmm1
   18027e1e4:	c5 fa 5a d0          	vcvtss2sd xmm2,xmm0,xmm0
   18027e1e8:	c5 fa 11 74 24 28    	vmovss DWORD PTR [rsp+0x28],xmm6
   18027e1ee:	4c 8d 45 a0          	lea    r8,[rbp-0x60]
   18027e1f2:	48 8d 15 a7 7d 1f 00 	lea    rdx,[rip+0x1f7da7]        # 0x180475fa0
   18027e1f9:	48 8d 4c 24 40       	lea    rcx,[rsp+0x40]
   18027e1fe:	c5 fb 11 54 24 20    	vmovsd QWORD PTR [rsp+0x20],xmm2
   18027e204:	e8 b7 d0 ef ff       	call   0x18017b2c0
   18027e209:	c5 f8 10 00          	vmovups xmm0,XMMWORD PTR [rax]
   18027e20d:	c5 f8 11 05 e3 2e 1e 	vmovups XMMWORD PTR [rip+0x1e2ee3],xmm0        # 0x1804610f8
   18027e214:	00 
   18027e215:	83 3d e4 74 1f 00 02 	cmp    DWORD PTR [rip+0x1f74e4],0x2        # 0x180475700
   18027e21c:	0f 85 c8 f7 ff ff    	jne    0x18027d9ea
   18027e222:	41 38 9e f8 04 00 00 	cmp    BYTE PTR [r14+0x4f8],bl
   18027e229:	0f 84 bb f7 ff ff    	je     0x18027d9ea
   18027e22f:	41 39 9e dc 03 00 00 	cmp    DWORD PTR [r14+0x3dc],ebx
   18027e236:	0f 8e ae f7 ff ff    	jle    0x18027d9ea
   18027e23c:	38 1d 2e 7d 1f 00    	cmp    BYTE PTR [rip+0x1f7d2e],bl        # 0x180475f70
   18027e242:	0f 84 a2 f7 ff ff    	je     0x18027d9ea
   18027e248:	48 8b 05 39 7d 1f 00 	mov    rax,QWORD PTR [rip+0x1f7d39]        # 0x180475f88
   18027e24f:	48 8b bd 90 02 00 00 	mov    rdi,QWORD PTR [rbp+0x290]
   18027e256:	48 3b f8             	cmp    rdi,rax
   18027e259:	0f 8e 8b f7 ff ff    	jle    0x18027d9ea
   18027e25f:	48 8b 8d 98 02 00 00 	mov    rcx,QWORD PTR [rbp+0x298]
   18027e266:	49 bc 00 00 00 00 00 	movabs r12,0x7ff0000000000000
   18027e26d:	00 f0 7f 
   18027e270:	48 85 c9             	test   rcx,rcx
   18027e273:	0f 8e 7b f7 ff ff    	jle    0x18027d9f4
   18027e279:	c5 f8 10 05 77 2e 1e 	vmovups xmm0,XMMWORD PTR [rip+0x1e2e77]        # 0x1804610f8
   18027e280:	00 
   18027e281:	48 2b f8             	sub    rdi,rax
   18027e284:	c5 f9 7f 44 24 50    	vmovdqa XMMWORD PTR [rsp+0x50],xmm0
   18027e28a:	c5 f8 57 c0          	vxorps xmm0,xmm0,xmm0
   18027e28e:	c5 f0 57 c9          	vxorps xmm1,xmm1,xmm1
   18027e292:	c4 e1 fb 2a c1       	vcvtsi2sd xmm0,xmm0,rcx
   18027e297:	c4 e1 f3 2a cf       	vcvtsi2sd xmm1,xmm1,rdi
   18027e29c:	c5 f3 5e d0          	vdivsd xmm2,xmm1,xmm0
   18027e2a0:	c5 fb 11 54 24 40    	vmovsd QWORD PTR [rsp+0x40],xmm2
   18027e2a6:	48 8b 44 24 40       	mov    rax,QWORD PTR [rsp+0x40]
   18027e2ab:	49 23 c4             	and    rax,r12
   18027e2ae:	49 3b c4             	cmp    rax,r12
   18027e2b1:	0f 84 9c 00 00 00    	je     0x18027e353
   18027e2b7:	c4 c1 79 2f d2       	vcomisd xmm2,xmm10
   18027e2bc:	0f 82 91 00 00 00    	jb     0x18027e353
   18027e2c2:	c5 eb 59 05 76 dc 18 	vmulsd xmm0,xmm2,QWORD PTR [rip+0x18dc76]        # 0x18040bf40
   18027e2c9:	00 
   18027e2ca:	ff 15 c8 59 04 00    	call   QWORD PTR [rip+0x459c8]        # 0x1802c3c98
   18027e2d0:	8b f3                	mov    esi,ebx
   18027e2d2:	48 8d 05 0f 2e 1e 00 	lea    rax,[rip+0x1e2e0f]        # 0x1804610e8
   18027e2d9:	c5 7b 5a c0          	vcvtsd2ss xmm8,xmm0,xmm0
   18027e2dd:	48 8b fb             	mov    rdi,rbx
   18027e2e0:	c5 fa 10 3c 07       	vmovss xmm7,DWORD PTR [rdi+rax*1]
   18027e2e5:	c5 f8 28 c7          	vmovaps xmm0,xmm7
   18027e2e9:	e8 22 2a eb ff       	call   0x180130d10
   18027e2ee:	84 c0                	test   al,al
   18027e2f0:	74 4e                	je     0x18027e340
   18027e2f2:	c5 fa 10 74 3c 50    	vmovss xmm6,DWORD PTR [rsp+rdi*1+0x50]
   18027e2f8:	c5 f8 28 c6          	vmovaps xmm0,xmm6
   18027e2fc:	e8 0f 2a eb ff       	call   0x180130d10
   18027e301:	84 c0                	test   al,al
   18027e303:	74 3b                	je     0x18027e340
   18027e305:	83 fe 02             	cmp    esi,0x2
   18027e308:	73 06                	jae    0x18027e310
   18027e30a:	c5 f8 2f f7          	vcomiss xmm6,xmm7
   18027e30e:	eb 04                	jmp    0x18027e314
   18027e310:	c5 f8 2f fe          	vcomiss xmm7,xmm6
   18027e314:	0f 97 c0             	seta   al
   18027e317:	84 c0                	test   al,al
   18027e319:	74 13                	je     0x18027e32e
   18027e31b:	c5 c2 5c c6          	vsubss xmm0,xmm7,xmm6
   18027e31f:	c4 c1 7a 59 c8       	vmulss xmm1,xmm0,xmm8
   18027e324:	c5 f2 58 d6          	vaddss xmm2,xmm1,xmm6
   18027e328:	c5 fa 11 54 3c 50    	vmovss DWORD PTR [rsp+rdi*1+0x50],xmm2
   18027e32e:	ff c6                	inc    esi
   18027e330:	48 8d 05 b1 2d 1e 00 	lea    rax,[rip+0x1e2db1]        # 0x1804610e8
   18027e337:	48 83 c7 04          	add    rdi,0x4
   18027e33b:	83 fe 04             	cmp    esi,0x4
   18027e33e:	72 a0                	jb     0x18027e2e0
   18027e340:	c5 f9 6f 44 24 50    	vmovdqa xmm0,XMMWORD PTR [rsp+0x50]
   18027e346:	c5 f8 11 05 aa 2d 1e 	vmovups XMMWORD PTR [rip+0x1e2daa],xmm0        # 0x1804610f8
   18027e34d:	00 
   18027e34e:	e9 a1 f6 ff ff       	jmp    0x18027d9f4
   18027e353:	c5 f8 10 05 9d 2d 1e 	vmovups xmm0,XMMWORD PTR [rip+0x1e2d9d]        # 0x1804610f8
   18027e35a:	00 
   18027e35b:	c5 f8 11 05 95 2d 1e 	vmovups XMMWORD PTR [rip+0x1e2d95],xmm0        # 0x1804610f8
   18027e362:	00 
   18027e363:	e9 8c f6 ff ff       	jmp    0x18027d9f4
   18027e368:	c5 fb 10 35 28 db 18 	vmovsd xmm6,QWORD PTR [rip+0x18db28]        # 0x18040be98
   18027e36f:	00 
   18027e370:	c5 7b 10 15 28 da 18 	vmovsd xmm10,QWORD PTR [rip+0x18da28]        # 0x18040bda0
   18027e377:	00 
   18027e378:	0f b6 35 c7 79 1f 00 	movzx  esi,BYTE PTR [rip+0x1f79c7]        # 0x180475d46
   18027e37f:	40 84 f6             	test   sil,sil
   18027e382:	74 0e                	je     0x18027e392
   18027e384:	c5 f9 2f 35 94 d9 18 	vcomisd xmm6,QWORD PTR [rip+0x18d994]        # 0x18040bd20
   18027e38b:	00 
   18027e38c:	72 04                	jb     0x18027e392
   18027e38e:	b0 01                	mov    al,0x1
   18027e390:	eb 02                	jmp    0x18027e394
   18027e392:	32 c0                	xor    al,al
   18027e394:	88 05 a7 79 1f 00    	mov    BYTE PTR [rip+0x1f79a7],al        # 0x180475d41
   18027e39a:	40 84 f6             	test   sil,sil
   18027e39d:	74 6f                	je     0x18027e40e
   18027e39f:	c5 fb 10 05 89 db 18 	vmovsd xmm0,QWORD PTR [rip+0x18db89]        # 0x18040bf30
   18027e3a6:	00 
   18027e3a7:	c5 fb 10 4d c0       	vmovsd xmm1,QWORD PTR [rbp-0x40]
   18027e3ac:	c5 f3 59 1d 1c 7b 1f 	vmulsd xmm3,xmm1,QWORD PTR [rip+0x1f7b1c]        # 0x180475ed0
   18027e3b3:	00 
   18027e3b4:	c5 fb 10 4d c8       	vmovsd xmm1,QWORD PTR [rbp-0x38]
   18027e3b9:	c5 fb 11 45 80       	vmovsd QWORD PTR [rbp-0x80],xmm0
   18027e3be:	c5 fb 10 45 b8       	vmovsd xmm0,QWORD PTR [rbp-0x48]
   18027e3c3:	c5 fb 59 15 fd 7a 1f 	vmulsd xmm2,xmm0,QWORD PTR [rip+0x1f7afd]        # 0x180475ec8
   18027e3ca:	00 
   18027e3cb:	c5 f3 59 05 05 7b 1f 	vmulsd xmm0,xmm1,QWORD PTR [rip+0x1f7b05]        # 0x180475ed8
   18027e3d2:	00 
   18027e3d3:	c5 e3 58 e2          	vaddsd xmm4,xmm3,xmm2
   18027e3d7:	c5 db 58 d0          	vaddsd xmm2,xmm4,xmm0
   18027e3db:	4c 8d 44 24 40       	lea    r8,[rsp+0x40]
   18027e3e0:	48 8d 55 80          	lea    rdx,[rbp-0x80]
   18027e3e4:	48 8d 4d 90          	lea    rcx,[rbp-0x70]
   18027e3e8:	c5 fb 11 55 90       	vmovsd QWORD PTR [rbp-0x70],xmm2
   18027e3ed:	c5 7b 11 54 24 40    	vmovsd QWORD PTR [rsp+0x40],xmm10
   18027e3f3:	e8 78 29 eb ff       	call   0x180130d70
   18027e3f8:	c5 fb 10 00          	vmovsd xmm0,QWORD PTR [rax]
   18027e3fc:	e8 aa 15 03 00       	call   0x1802af9ab
   18027e401:	c5 f9 2f 05 6f d9 18 	vcomisd xmm0,QWORD PTR [rip+0x18d96f]        # 0x18040bd78
   18027e408:	00 
   18027e409:	0f 93 c0             	setae  al
   18027e40c:	eb 02                	jmp    0x18027e410
   18027e40e:	32 c0                	xor    al,al
   18027e410:	c5 c3 5c 05 48 7a 1f 	vsubsd xmm0,xmm7,QWORD PTR [rip+0x1f7a48]        # 0x180475e60
   18027e417:	00 
   18027e418:	c5 bb 5c 15 48 7a 1f 	vsubsd xmm2,xmm8,QWORD PTR [rip+0x1f7a48]        # 0x180475e68
   18027e41f:	00 
   18027e420:	c5 b3 5c 1d 48 7a 1f 	vsubsd xmm3,xmm9,QWORD PTR [rip+0x1f7a48]        # 0x180475e70
   18027e427:	00 
   18027e428:	88 05 14 79 1f 00    	mov    BYTE PTR [rip+0x1f7914],al        # 0x180475d42
   18027e42e:	c5 7b 11 5c 24 40    	vmovsd QWORD PTR [rsp+0x40],xmm11
   18027e434:	48 8b 44 24 40       	mov    rax,QWORD PTR [rsp+0x40]
   18027e439:	49 23 c4             	and    rax,r12
   18027e43c:	c5 c1 57 ff          	vxorpd xmm7,xmm7,xmm7
   18027e440:	49 3b c4             	cmp    rax,r12
   18027e443:	74 3f                	je     0x18027e484
   18027e445:	c5 79 2f df          	vcomisd xmm11,xmm7
   18027e449:	76 39                	jbe    0x18027e484
   18027e44b:	c5 fb 59 c8          	vmulsd xmm1,xmm0,xmm0
   18027e44f:	c5 eb 59 c2          	vmulsd xmm0,xmm2,xmm2
   18027e453:	c5 f3 58 d0          	vaddsd xmm2,xmm1,xmm0
   18027e457:	c5 e3 59 cb          	vmulsd xmm1,xmm3,xmm3
   18027e45b:	c5 eb 58 d1          	vaddsd xmm2,xmm2,xmm1
   18027e45f:	c5 eb 51 da          	vsqrtsd xmm3,xmm2,xmm2
   18027e463:	c5 fb 10 15 d5 d8 18 	vmovsd xmm2,QWORD PTR [rip+0x18d8d5]        # 0x18040bd40
   18027e46a:	00 
   18027e46b:	c4 c1 2b 5e c3       	vdivsd xmm0,xmm10,xmm11
   18027e470:	c5 e3 59 c8          	vmulsd xmm1,xmm3,xmm0
   18027e474:	c5 f9 2f ca          	vcomisd xmm1,xmm2
   18027e478:	77 0a                	ja     0x18027e484
   18027e47a:	c5 f9 2f f2          	vcomisd xmm6,xmm2
   18027e47e:	77 04                	ja     0x18027e484
   18027e480:	8b c3                	mov    eax,ebx
   18027e482:	eb 05                	jmp    0x18027e489
   18027e484:	b8 01 00 00 00       	mov    eax,0x1
   18027e489:	85 c0                	test   eax,eax
   18027e48b:	48 8d 0d 1e 7a 1f 00 	lea    rcx,[rip+0x1f7a1e]        # 0x180475eb0
   18027e492:	48 8d 3d 77 79 1f 00 	lea    rdi,[rip+0x1f7977]        # 0x180475e10
   18027e499:	48 0f 45 f9          	cmovne rdi,rcx
   18027e49d:	40 84 f6             	test   sil,sil
   18027e4a0:	0f 84 3d 02 00 00    	je     0x18027e6e3
   18027e4a6:	c5 f8 10 74 24 50    	vmovups xmm6,XMMWORD PTR [rsp+0x50]
   18027e4ac:	c5 f8 10 05 a4 7a 1f 	vmovups xmm0,XMMWORD PTR [rip+0x1f7aa4]        # 0x180475f58
   18027e4b3:	00 
   18027e4b4:	4c 8d 4c 24 50       	lea    r9,[rsp+0x50]
   18027e4b9:	48 8b cf             	mov    rcx,rdi
   18027e4bc:	4c 8d 45 a0          	lea    r8,[rbp-0x60]
   18027e4c0:	48 8d 54 24 40       	lea    rdx,[rsp+0x40]
   18027e4c5:	c5 f9 7f 74 24 50    	vmovdqa XMMWORD PTR [rsp+0x50],xmm6
   18027e4cb:	c5 f9 7f 44 24 40    	vmovdqa XMMWORD PTR [rsp+0x40],xmm0
   18027e4d1:	e8 3a c9 ef ff       	call   0x18017ae10
   18027e4d6:	38 1d 6a 78 1f 00    	cmp    BYTE PTR [rip+0x1f786a],bl        # 0x180475d46
   18027e4dc:	c5 f8 28 d0          	vmovaps xmm2,xmm0
   18027e4e0:	0f 84 fd 01 00 00    	je     0x18027e6e3
   18027e4e6:	c5 7b 11 5c 24 40    	vmovsd QWORD PTR [rsp+0x40],xmm11
   18027e4ec:	48 8b 44 24 40       	mov    rax,QWORD PTR [rsp+0x40]
   18027e4f1:	49 23 c4             	and    rax,r12
   18027e4f4:	49 3b c4             	cmp    rax,r12
   18027e4f7:	0f 84 e6 01 00 00    	je     0x18027e6e3
   18027e4fd:	c5 79 2f df          	vcomisd xmm11,xmm7
   18027e501:	0f 86 dc 01 00 00    	jbe    0x18027e6e3
   18027e507:	c5 f9 2f c7          	vcomisd xmm0,xmm7
   18027e50b:	c6 05 35 78 1f 00 01 	mov    BYTE PTR [rip+0x1f7835],0x1        # 0x180475d47
   18027e512:	73 07                	jae    0x18027e51b
   18027e514:	c6 05 28 78 1f 00 01 	mov    BYTE PTR [rip+0x1f7828],0x1        # 0x180475d43
   18027e51b:	48 8b 0d 36 78 1f 00 	mov    rcx,QWORD PTR [rip+0x1f7836]        # 0x180475d58
   18027e522:	c5 fb 11 55 90       	vmovsd QWORD PTR [rbp-0x70],xmm2
   18027e527:	48 85 c9             	test   rcx,rcx
   18027e52a:	0f 8e 7f 01 00 00    	jle    0x18027e6af
   18027e530:	48 8b 85 90 02 00 00 	mov    rax,QWORD PTR [rbp+0x290]
   18027e537:	48 3b c1             	cmp    rax,rcx
   18027e53a:	0f 8e 6f 01 00 00    	jle    0x18027e6af
   18027e540:	48 2b c1             	sub    rax,rcx
   18027e543:	c5 f8 57 c0          	vxorps xmm0,xmm0,xmm0
   18027e547:	c4 e1 fb 2a 85 98 02 	vcvtsi2sd xmm0,xmm0,QWORD PTR [rbp+0x298]
   18027e54e:	00 00 
   18027e550:	c5 f0 57 c9          	vxorps xmm1,xmm1,xmm1
   18027e554:	c4 e1 f3 2a c8       	vcvtsi2sd xmm1,xmm1,rax
   18027e559:	c5 f3 5e d8          	vdivsd xmm3,xmm1,xmm0
   18027e55d:	c5 f9 2f df          	vcomisd xmm3,xmm7
   18027e561:	c5 f8 28 c2          	vmovaps xmm0,xmm2
   18027e565:	0f 86 44 01 00 00    	jbe    0x18027e6af
   18027e56b:	c5 f9 2f 1d 0d d8 18 	vcomisd xmm3,QWORD PTR [rip+0x18d80d]        # 0x18040bd80
   18027e572:	00 
   18027e573:	0f 87 36 01 00 00    	ja     0x18027e6af
   18027e579:	c5 fc 10 45 a0       	vmovups ymm0,YMMWORD PTR [rbp-0x60]
   18027e57e:	c5 fb 10 0d 4a d8 18 	vmovsd xmm1,QWORD PTR [rip+0x18d84a]        # 0x18040bdd0
   18027e585:	00 
   18027e586:	c5 fc 11 45 00       	vmovups YMMWORD PTR [rbp+0x0],ymm0
   18027e58b:	c5 fc 10 45 c0       	vmovups ymm0,YMMWORD PTR [rbp-0x40]
   18027e590:	c5 fc 11 45 20       	vmovups YMMWORD PTR [rbp+0x20],ymm0
   18027e595:	c5 fb 10 05 cb d7 18 	vmovsd xmm0,QWORD PTR [rip+0x18d7cb]        # 0x18040bd68
   18027e59c:	00 
   18027e59d:	c5 fb 5e c3          	vdivsd xmm0,xmm0,xmm3
   18027e5a1:	c5 fb 11 45 80       	vmovsd QWORD PTR [rbp-0x80],xmm0
   18027e5a6:	c5 fb 11 4c 24 40    	vmovsd QWORD PTR [rsp+0x40],xmm1
   18027e5ac:	48 8d 54 24 40       	lea    rdx,[rsp+0x40]
   18027e5b1:	48 8d 4d 80          	lea    rcx,[rbp-0x80]
   18027e5b5:	c5 f8 77             	vzeroupper
   18027e5b8:	e8 a3 90 ed ff       	call   0x180157660
   18027e5bd:	c5 fc 10 65 c0       	vmovups ymm4,YMMWORD PTR [rbp-0x40]
   18027e5c2:	c5 fc 10 45 a0       	vmovups ymm0,YMMWORD PTR [rbp-0x60]
   18027e5c7:	c4 e3 7d 19 c2 01    	vextractf128 xmm2,ymm0,0x1
   18027e5cd:	c5 fb 10 28          	vmovsd xmm5,QWORD PTR [rax]
   18027e5d1:	c5 e9 15 d2          	vunpckhpd xmm2,xmm2,xmm2
   18027e5d5:	c5 eb 5c 05 ab 77 1f 	vsubsd xmm0,xmm2,QWORD PTR [rip+0x1f77ab]        # 0x180475d88
   18027e5dc:	00 
   18027e5dd:	c5 fb 59 cd          	vmulsd xmm1,xmm0,xmm5
   18027e5e1:	c5 db 5c 05 a7 77 1f 	vsubsd xmm0,xmm4,QWORD PTR [rip+0x1f77a7]        # 0x180475d90
   18027e5e8:	00 
   18027e5e9:	c5 f3 58 d2          	vaddsd xmm2,xmm1,xmm2
   18027e5ed:	c5 fb 59 cd          	vmulsd xmm1,xmm0,xmm5
   18027e5f1:	c5 d9 15 dc          	vunpckhpd xmm3,xmm4,xmm4
   18027e5f5:	c5 e3 5c 05 9b 77 1f 	vsubsd xmm0,xmm3,QWORD PTR [rip+0x1f779b]        # 0x180475d98
   18027e5fc:	00 
   18027e5fd:	c5 fb 11 55 18       	vmovsd QWORD PTR [rbp+0x18],xmm2
   18027e602:	c5 f3 58 d4          	vaddsd xmm2,xmm1,xmm4
   18027e606:	c5 fb 59 cd          	vmulsd xmm1,xmm0,xmm5
   18027e60a:	c5 fb 11 55 20       	vmovsd QWORD PTR [rbp+0x20],xmm2
   18027e60f:	c5 f3 58 d3          	vaddsd xmm2,xmm1,xmm3
   18027e613:	c4 e3 7d 19 e3 01    	vextractf128 xmm3,ymm4,0x1
   18027e619:	c5 e3 5c 05 7f 77 1f 	vsubsd xmm0,xmm3,QWORD PTR [rip+0x1f777f]        # 0x180475da0
   18027e620:	00 
   18027e621:	c5 fb 59 cd          	vmulsd xmm1,xmm0,xmm5
   18027e625:	c5 fb 11 55 28       	vmovsd QWORD PTR [rbp+0x28],xmm2
   18027e62a:	c5 f3 58 d3          	vaddsd xmm2,xmm1,xmm3
   18027e62e:	c5 fb 10 5d e0       	vmovsd xmm3,QWORD PTR [rbp-0x20]
   18027e633:	c5 fb 11 55 30       	vmovsd QWORD PTR [rbp+0x30],xmm2
   18027e638:	c4 e3 7d 19 e4 01    	vextractf128 xmm4,ymm4,0x1
   18027e63e:	c5 d9 15 e4          	vunpckhpd xmm4,xmm4,xmm4
   18027e642:	c5 db 5c 05 5e 77 1f 	vsubsd xmm0,xmm4,QWORD PTR [rip+0x1f775e]        # 0x180475da8
   18027e649:	00 
   18027e64a:	c5 fb 59 cd          	vmulsd xmm1,xmm0,xmm5
   18027e64e:	c5 e3 5c 05 5a 77 1f 	vsubsd xmm0,xmm3,QWORD PTR [rip+0x1f775a]        # 0x180475db0
   18027e655:	00 
   18027e656:	c5 f3 58 d4          	vaddsd xmm2,xmm1,xmm4
   18027e65a:	c5 fb 59 cd          	vmulsd xmm1,xmm0,xmm5
   18027e65e:	c5 f8 10 05 f2 78 1f 	vmovups xmm0,XMMWORD PTR [rip+0x1f78f2]        # 0x180475f58
   18027e665:	00 
   18027e666:	c5 fb 11 55 38       	vmovsd QWORD PTR [rbp+0x38],xmm2
   18027e66b:	c5 f3 58 d3          	vaddsd xmm2,xmm1,xmm3
   18027e66f:	c5 fb 11 55 40       	vmovsd QWORD PTR [rbp+0x40],xmm2
   18027e674:	c5 f9 7f 74 24 50    	vmovdqa XMMWORD PTR [rsp+0x50],xmm6
   18027e67a:	c5 f9 7f 45 80       	vmovdqa XMMWORD PTR [rbp-0x80],xmm0
   18027e67f:	4c 8d 4c 24 50       	lea    r9,[rsp+0x50]
   18027e684:	48 8b cf             	mov    rcx,rdi
   18027e687:	4c 8d 45 00          	lea    r8,[rbp+0x0]
   18027e68b:	48 8d 55 80          	lea    rdx,[rbp-0x80]
   18027e68f:	c5 f8 77             	vzeroupper
   18027e692:	e8 79 c7 ef ff       	call   0x18017ae10
   18027e697:	48 8d 54 24 40       	lea    rdx,[rsp+0x40]
   18027e69c:	48 8d 4d 90          	lea    rcx,[rbp-0x70]
   18027e6a0:	c5 fb 11 44 24 40    	vmovsd QWORD PTR [rsp+0x40],xmm0
   18027e6a6:	e8 b5 8f ed ff       	call   0x180157660
   18027e6ab:	c5 fb 10 00          	vmovsd xmm0,QWORD PTR [rax]
   18027e6af:	c5 f9 2f 05 59 d7 18 	vcomisd xmm0,QWORD PTR [rip+0x18d759]        # 0x18040be10
   18027e6b6:	00 
   18027e6b7:	72 09                	jb     0x18027e6c2
   18027e6b9:	c6 05 24 2a 1e 00 01 	mov    BYTE PTR [rip+0x1e2a24],0x1        # 0x1804610e4
   18027e6c0:	eb 27                	jmp    0x18027e6e9
   18027e6c2:	38 1d 1c 2a 1e 00    	cmp    BYTE PTR [rip+0x1e2a1c],bl        # 0x1804610e4
   18027e6c8:	74 1f                	je     0x18027e6e9
   18027e6ca:	c5 f9 2f 05 36 d7 18 	vcomisd xmm0,QWORD PTR [rip+0x18d736]        # 0x18040be08
   18027e6d1:	00 
   18027e6d2:	73 15                	jae    0x18027e6e9
   18027e6d4:	c6 05 68 76 1f 00 01 	mov    BYTE PTR [rip+0x1f7668],0x1        # 0x180475d43
   18027e6db:	88 1d 03 2a 1e 00    	mov    BYTE PTR [rip+0x1e2a03],bl        # 0x1804610e4
   18027e6e1:	eb 06                	jmp    0x18027e6e9
   18027e6e3:	88 1d 5e 76 1f 00    	mov    BYTE PTR [rip+0x1f765e],bl        # 0x180475d47
   18027e6e9:	4c 8b 0d c8 56 1f 00 	mov    r9,QWORD PTR [rip+0x1f56c8]        # 0x180473db8
   18027e6f0:	90                   	nop
   18027e6f1:	4c 8b 85 90 02 00 00 	mov    r8,QWORD PTR [rbp+0x290]
   18027e6f8:	4d 85 c9             	test   r9,r9
   18027e6fb:	0f 84 b4 01 00 00    	je     0x18027e8b5
   18027e701:	4c 8b 15 40 76 1f 00 	mov    r10,QWORD PTR [rip+0x1f7640]        # 0x180475d48
   18027e708:	32 c9                	xor    cl,cl
   18027e70a:	4d 85 d2             	test   r10,r10
   18027e70d:	0f 84 32 01 00 00    	je     0x18027e845
   18027e713:	c5 7b 11 5c 24 40    	vmovsd QWORD PTR [rsp+0x40],xmm11
   18027e719:	48 8b 44 24 40       	mov    rax,QWORD PTR [rsp+0x40]
   18027e71e:	49 23 c4             	and    rax,r12
   18027e721:	49 3b c4             	cmp    rax,r12
   18027e724:	0f 84 1b 01 00 00    	je     0x18027e845
   18027e72a:	c5 79 2f df          	vcomisd xmm11,xmm7
   18027e72e:	0f 86 11 01 00 00    	jbe    0x18027e845
   18027e734:	c5 fb 10 05 84 76 1f 	vmovsd xmm0,QWORD PTR [rip+0x1f7684]        # 0x180475dc0
   18027e73b:	00 
   18027e73c:	c5 7b 5c 0d 2c 76 1f 	vsubsd xmm9,xmm0,QWORD PTR [rip+0x1f762c]        # 0x180475d70
   18027e743:	00 
   18027e744:	c5 fb 10 05 7c 76 1f 	vmovsd xmm0,QWORD PTR [rip+0x1f767c]        # 0x180475dc8
   18027e74b:	00 
   18027e74c:	c5 7b 5c 05 24 76 1f 	vsubsd xmm8,xmm0,QWORD PTR [rip+0x1f7624]        # 0x180475d78
   18027e753:	00 
   18027e754:	c5 fb 10 05 74 76 1f 	vmovsd xmm0,QWORD PTR [rip+0x1f7674]        # 0x180475dd0
   18027e75b:	00 
   18027e75c:	c5 7b 5c 15 1c 76 1f 	vsubsd xmm10,xmm0,QWORD PTR [rip+0x1f761c]        # 0x180475d80
   18027e763:	00 
   18027e764:	c5 fb 10 45 b8       	vmovsd xmm0,QWORD PTR [rbp-0x48]
   18027e769:	c5 fb 5c 3d 17 76 1f 	vsubsd xmm7,xmm0,QWORD PTR [rip+0x1f7617]        # 0x180475d88
   18027e770:	00 
   18027e771:	c5 fb 10 4d c0       	vmovsd xmm1,QWORD PTR [rbp-0x40]
   18027e776:	c5 f3 5c 2d 12 76 1f 	vsubsd xmm5,xmm1,QWORD PTR [rip+0x1f7612]        # 0x180475d90
   18027e77d:	00 
   18027e77e:	c5 fb 10 45 c8       	vmovsd xmm0,QWORD PTR [rbp-0x38]
   18027e783:	c5 fb 5c 35 0d 76 1f 	vsubsd xmm6,xmm0,QWORD PTR [rip+0x1f760d]        # 0x180475d98
   18027e78a:	00 
   18027e78b:	c5 fb 10 4d d0       	vmovsd xmm1,QWORD PTR [rbp-0x30]
   18027e790:	c5 f3 5c 1d 08 76 1f 	vsubsd xmm3,xmm1,QWORD PTR [rip+0x1f7608]        # 0x180475da0
   18027e797:	00 
   18027e798:	c5 fb 10 45 d8       	vmovsd xmm0,QWORD PTR [rbp-0x28]
   18027e79d:	c5 fb 5c 15 03 76 1f 	vsubsd xmm2,xmm0,QWORD PTR [rip+0x1f7603]        # 0x180475da8
   18027e7a4:	00 
   18027e7a5:	c5 fb 10 4d e0       	vmovsd xmm1,QWORD PTR [rbp-0x20]
   18027e7aa:	c5 f3 5c 25 fe 75 1f 	vsubsd xmm4,xmm1,QWORD PTR [rip+0x1f75fe]        # 0x180475db0
   18027e7b1:	00 
   18027e7b2:	c5 eb 59 d2          	vmulsd xmm2,xmm2,xmm2
   18027e7b6:	c5 e3 59 c3          	vmulsd xmm0,xmm3,xmm3
   18027e7ba:	c5 eb 58 d8          	vaddsd xmm3,xmm2,xmm0
   18027e7be:	c5 db 59 cc          	vmulsd xmm1,xmm4,xmm4
   18027e7c2:	c5 e3 58 d1          	vaddsd xmm2,xmm3,xmm1
   18027e7c6:	c5 eb 51 c2          	vsqrtsd xmm0,xmm2,xmm2
   18027e7ca:	c5 fb 11 44 24 40    	vmovsd QWORD PTR [rsp+0x40],xmm0
   18027e7d0:	c5 d3 59 dd          	vmulsd xmm3,xmm5,xmm5
   18027e7d4:	c5 c3 59 cf          	vmulsd xmm1,xmm7,xmm7
   18027e7d8:	c5 e3 58 d1          	vaddsd xmm2,xmm3,xmm1
   18027e7dc:	c5 cb 59 c6          	vmulsd xmm0,xmm6,xmm6
   18027e7e0:	c5 eb 58 d0          	vaddsd xmm2,xmm2,xmm0
   18027e7e4:	c5 eb 51 ca          	vsqrtsd xmm1,xmm2,xmm2
   18027e7e8:	c4 c1 3b 59 d8       	vmulsd xmm3,xmm8,xmm8
   18027e7ed:	c4 c1 33 59 c1       	vmulsd xmm0,xmm9,xmm9
   18027e7f2:	c5 e3 58 d0          	vaddsd xmm2,xmm3,xmm0
   18027e7f6:	c5 fb 10 05 a2 d5 18 	vmovsd xmm0,QWORD PTR [rip+0x18d5a2]        # 0x18040bda0
   18027e7fd:	00 
   18027e7fe:	c5 fb 11 4d 80       	vmovsd QWORD PTR [rbp-0x80],xmm1
   18027e803:	c4 c1 2b 59 ca       	vmulsd xmm1,xmm10,xmm10
   18027e808:	c5 eb 58 d1          	vaddsd xmm2,xmm2,xmm1
   18027e80c:	c5 eb 51 da          	vsqrtsd xmm3,xmm2,xmm2
   18027e810:	c4 c1 7b 5e c3       	vdivsd xmm0,xmm0,xmm11
   18027e815:	c5 e3 59 c8          	vmulsd xmm1,xmm3,xmm0
   18027e819:	c5 f9 2f 0d f7 d4 18 	vcomisd xmm1,QWORD PTR [rip+0x18d4f7]        # 0x18040bd18
   18027e820:	00 
   18027e821:	77 20                	ja     0x18027e843
   18027e823:	48 8d 54 24 40       	lea    rdx,[rsp+0x40]
   18027e828:	48 8d 4d 80          	lea    rcx,[rbp-0x80]
   18027e82c:	e8 3f e6 ec ff       	call   0x18014ce70
   18027e831:	c5 fb 10 05 b7 d4 18 	vmovsd xmm0,QWORD PTR [rip+0x18d4b7]        # 0x18040bcf0
   18027e838:	00 
   18027e839:	c5 f9 2f 00          	vcomisd xmm0,QWORD PTR [rax]
   18027e83d:	72 04                	jb     0x18027e843
   18027e83f:	b1 01                	mov    cl,0x1
   18027e841:	eb 02                	jmp    0x18027e845
   18027e843:	32 c9                	xor    cl,cl
   18027e845:	4d 3b ca             	cmp    r9,r10
   18027e848:	0f 95 c2             	setne  dl
   18027e84b:	74 1f                	je     0x18027e86c
   18027e84d:	84 c9                	test   cl,cl
   18027e84f:	74 12                	je     0x18027e863
   18027e851:	8b 1d f9 74 1f 00    	mov    ebx,DWORD PTR [rip+0x1f74f9]        # 0x180475d50
   18027e857:	b8 03 00 00 00       	mov    eax,0x3
   18027e85c:	ff c3                	inc    ebx
   18027e85e:	3b d8                	cmp    ebx,eax
   18027e860:	0f 47 d8             	cmova  ebx,eax
   18027e863:	4c 89 0d de 74 1f 00 	mov    QWORD PTR [rip+0x1f74de],r9        # 0x180475d48
   18027e86a:	eb 04                	jmp    0x18027e870
   18027e86c:	84 c9                	test   cl,cl
   18027e86e:	75 06                	jne    0x18027e876
   18027e870:	89 1d da 74 1f 00    	mov    DWORD PTR [rip+0x1f74da],ebx        # 0x180475d50
   18027e876:	84 d2                	test   dl,dl
   18027e878:	75 04                	jne    0x18027e87e
   18027e87a:	84 c9                	test   cl,cl
   18027e87c:	75 37                	jne    0x18027e8b5
   18027e87e:	c5 fc 10 0d 5a 75 1f 	vmovups ymm1,YMMWORD PTR [rip+0x1f755a]        # 0x180475de0
   18027e885:	00 
   18027e886:	c5 fc 10 05 32 75 1f 	vmovups ymm0,YMMWORD PTR [rip+0x1f7532]        # 0x180475dc0
   18027e88d:	00 
   18027e88e:	c5 fc 11 0d fa 74 1f 	vmovups YMMWORD PTR [rip+0x1f74fa],ymm1        # 0x180475d90
   18027e895:	00 
   18027e896:	c5 fb 10 0d 62 75 1f 	vmovsd xmm1,QWORD PTR [rip+0x1f7562]        # 0x180475e00
   18027e89d:	00 
   18027e89e:	c5 fb 11 0d 0a 75 1f 	vmovsd QWORD PTR [rip+0x1f750a],xmm1        # 0x180475db0
   18027e8a5:	00 
   18027e8a6:	c5 fc 11 05 c2 74 1f 	vmovups YMMWORD PTR [rip+0x1f74c2],ymm0        # 0x180475d70
   18027e8ad:	00 
   18027e8ae:	4c 89 05 a3 74 1f 00 	mov    QWORD PTR [rip+0x1f74a3],r8        # 0x180475d58
   18027e8b5:	80 3d 8b 74 1f 00 00 	cmp    BYTE PTR [rip+0x1f748b],0x0        # 0x180475d47
   18027e8bc:	74 28                	je     0x18027e8e6
   18027e8be:	80 3d 7e 74 1f 00 00 	cmp    BYTE PTR [rip+0x1f747e],0x0        # 0x180475d43
   18027e8c5:	75 1f                	jne    0x18027e8e6
   18027e8c7:	80 3d 73 74 1f 00 00 	cmp    BYTE PTR [rip+0x1f7473],0x0        # 0x180475d41
   18027e8ce:	75 16                	jne    0x18027e8e6
   18027e8d0:	80 3d 6b 74 1f 00 00 	cmp    BYTE PTR [rip+0x1f746b],0x0        # 0x180475d42
   18027e8d7:	75 0d                	jne    0x18027e8e6
   18027e8d9:	80 3d 65 74 1f 00 00 	cmp    BYTE PTR [rip+0x1f7465],0x0        # 0x180475d45
   18027e8e0:	0f 84 e7 02 00 00    	je     0x18027ebcd
   18027e8e6:	83 3d 63 74 1f 00 03 	cmp    DWORD PTR [rip+0x1f7463],0x3        # 0x180475d50
   18027e8ed:	0f 82 da 02 00 00    	jb     0x18027ebcd
   18027e8f3:	4c 3b 05 66 74 1f 00 	cmp    r8,QWORD PTR [rip+0x1f7466]        # 0x180475d60
   18027e8fa:	0f 8c cd 02 00 00    	jl     0x18027ebcd
   18027e900:	49 8b ce             	mov    rcx,r14
   18027e903:	c5 f8 77             	vzeroupper
   18027e906:	e8 e5 98 fe ff       	call   0x1802681f0
   18027e90b:	84 c0                	test   al,al
   18027e90d:	0f 84 ba 02 00 00    	je     0x18027ebcd
   18027e913:	c4 41 1a 5a dc       	vcvtss2sd xmm11,xmm12,xmm12
   18027e918:	c5 7b 11 5c 24 40    	vmovsd QWORD PTR [rsp+0x40],xmm11
   18027e91e:	48 8b 44 24 40       	mov    rax,QWORD PTR [rsp+0x40]
   18027e923:	49 23 c4             	and    rax,r12
   18027e926:	49 3b c4             	cmp    rax,r12
   18027e929:	0f 84 9e 02 00 00    	je     0x18027ebcd
   18027e92f:	c4 c1 12 5a f5       	vcvtss2sd xmm6,xmm13,xmm13
   18027e934:	c5 fb 11 74 24 40    	vmovsd QWORD PTR [rsp+0x40],xmm6
   18027e93a:	48 8b 44 24 40       	mov    rax,QWORD PTR [rsp+0x40]
   18027e93f:	49 23 c4             	and    rax,r12
   18027e942:	49 3b c4             	cmp    rax,r12
   18027e945:	0f 84 82 02 00 00    	je     0x18027ebcd
   18027e94b:	c4 41 0a 5a d6       	vcvtss2sd xmm10,xmm14,xmm14
   18027e950:	c5 7b 11 54 24 40    	vmovsd QWORD PTR [rsp+0x40],xmm10
   18027e956:	48 8b 44 24 40       	mov    rax,QWORD PTR [rsp+0x40]
   18027e95b:	49 23 c4             	and    rax,r12
   18027e95e:	49 3b c4             	cmp    rax,r12
   18027e961:	0f 84 66 02 00 00    	je     0x18027ebcd
   18027e967:	c4 41 02 5a cf       	vcvtss2sd xmm9,xmm15,xmm15
   18027e96c:	c5 7b 11 4c 24 40    	vmovsd QWORD PTR [rsp+0x40],xmm9
   18027e972:	48 8b 44 24 40       	mov    rax,QWORD PTR [rsp+0x40]
   18027e977:	49 23 c4             	and    rax,r12
   18027e97a:	49 3b c4             	cmp    rax,r12
   18027e97d:	0f 84 4a 02 00 00    	je     0x18027ebcd
   18027e983:	c4 41 78 2f ec       	vcomiss xmm13,xmm12
   18027e988:	0f 86 3f 02 00 00    	jbe    0x18027ebcd
   18027e98e:	c4 41 78 2f fe       	vcomiss xmm15,xmm14
   18027e993:	0f 86 34 02 00 00    	jbe    0x18027ebcd
   18027e999:	c5 fb 10 05 3f d4 18 	vmovsd xmm0,QWORD PTR [rip+0x18d43f]        # 0x18040bde0
   18027e9a0:	00 
   18027e9a1:	c4 c1 4b 58 cb       	vaddsd xmm1,xmm6,xmm11
   18027e9a6:	c5 fb 11 44 24 50    	vmovsd QWORD PTR [rsp+0x50],xmm0
   18027e9ac:	c5 f3 59 05 d4 d3 18 	vmulsd xmm0,xmm1,QWORD PTR [rip+0x18d3d4]        # 0x18040bd88
   18027e9b3:	00 
   18027e9b4:	e8 17 28 eb ff       	call   0x1801311d0
   18027e9b9:	c5 7b 10 05 37 d4 18 	vmovsd xmm8,QWORD PTR [rip+0x18d437]        # 0x18040bdf8
   18027e9c0:	00 
   18027e9c1:	c5 bb 5c d0          	vsubsd xmm2,xmm8,xmm0
   18027e9c5:	c4 c1 4b 5c cb       	vsubsd xmm1,xmm6,xmm11
   18027e9ca:	c5 fb 10 35 ee d3 18 	vmovsd xmm6,QWORD PTR [rip+0x18d3ee]        # 0x18040bdc0
   18027e9d1:	00 
   18027e9d2:	c5 cb 5e c1          	vdivsd xmm0,xmm6,xmm1
   18027e9d6:	c5 eb 59 c8          	vmulsd xmm1,xmm2,xmm0
   18027e9da:	c4 c1 33 58 d2       	vaddsd xmm2,xmm9,xmm10
   18027e9df:	c5 eb 59 05 a1 d3 18 	vmulsd xmm0,xmm2,QWORD PTR [rip+0x18d3a1]        # 0x18040bd88
   18027e9e6:	00 
   18027e9e7:	c5 fb 11 4c 24 58    	vmovsd QWORD PTR [rsp+0x58],xmm1
   18027e9ed:	e8 de 27 eb ff       	call   0x1801311d0
   18027e9f2:	c4 c1 33 5c ca       	vsubsd xmm1,xmm9,xmm10
   18027e9f7:	c5 bb 5c d0          	vsubsd xmm2,xmm8,xmm0
   18027e9fb:	c5 cb 5e c1          	vdivsd xmm0,xmm6,xmm1
   18027e9ff:	c5 eb 59 c8          	vmulsd xmm1,xmm2,xmm0
   18027ea03:	48 8d 54 24 68       	lea    rdx,[rsp+0x68]
   18027ea08:	48 8d 4c 24 50       	lea    rcx,[rsp+0x50]
   18027ea0d:	c5 fb 11 4c 24 60    	vmovsd QWORD PTR [rsp+0x60],xmm1
   18027ea13:	e8 d8 a4 fb ff       	call   0x180238ef0
   18027ea18:	c5 fb 11 44 24 40    	vmovsd QWORD PTR [rsp+0x40],xmm0
   18027ea1e:	48 8b 44 24 40       	mov    rax,QWORD PTR [rsp+0x40]
   18027ea23:	49 23 c4             	and    rax,r12
   18027ea26:	49 3b c4             	cmp    rax,r12
   18027ea29:	0f 84 9e 01 00 00    	je     0x18027ebcd
   18027ea2f:	c5 f9 2f 05 71 d3 18 	vcomisd xmm0,QWORD PTR [rip+0x18d371]        # 0x18040bda8
   18027ea36:	00 
   18027ea37:	0f 86 90 01 00 00    	jbe    0x18027ebcd
   18027ea3d:	c5 7a 10 0d 7f d2 18 	vmovss xmm9,DWORD PTR [rip+0x18d27f]        # 0x18040bcc4
   18027ea44:	00 
   18027ea45:	c5 7b 5a c0          	vcvtsd2ss xmm8,xmm0,xmm0
   18027ea49:	c5 b8 57 3d 8f de 18 	vxorps xmm7,xmm8,XMMWORD PTR [rip+0x18de8f]        # 0x18040c8e0
   18027ea50:	00 
   18027ea51:	c5 fa 11 3d 9f 26 1e 	vmovss DWORD PTR [rip+0x1e269f],xmm7        # 0x1804610f8
   18027ea58:	00 
   18027ea59:	c5 fa 11 3d 9b 26 1e 	vmovss DWORD PTR [rip+0x1e269b],xmm7        # 0x1804610fc
   18027ea60:	00 
   18027ea61:	c5 7a 11 05 97 26 1e 	vmovss DWORD PTR [rip+0x1e2697],xmm8        # 0x180461100
   18027ea68:	00 
   18027ea69:	c5 7a 11 05 93 26 1e 	vmovss DWORD PTR [rip+0x1e2693],xmm8        # 0x180461104
   18027ea70:	00 
   18027ea71:	c4 c1 12 58 c4       	vaddss xmm0,xmm13,xmm12
   18027ea76:	c4 c1 7a 59 e9       	vmulss xmm5,xmm0,xmm9
   18027ea7b:	c4 c1 02 58 ce       	vaddss xmm1,xmm15,xmm14
   18027ea80:	c4 c1 72 59 f1       	vmulss xmm6,xmm1,xmm9
   18027ea85:	c4 c1 12 5c c4       	vsubss xmm0,xmm13,xmm12
   18027ea8a:	c4 c1 7a 59 d9       	vmulss xmm3,xmm0,xmm9
   18027ea8f:	c4 c1 02 5c ce       	vsubss xmm1,xmm15,xmm14
   18027ea94:	c4 c1 72 59 e1       	vmulss xmm4,xmm1,xmm9
   18027ea99:	c5 e2 59 c7          	vmulss xmm0,xmm3,xmm7
   18027ea9d:	c5 fa 58 d5          	vaddss xmm2,xmm0,xmm5
   18027eaa1:	c4 c1 62 59 c8       	vmulss xmm1,xmm3,xmm8
   18027eaa6:	c5 f2 58 c5          	vaddss xmm0,xmm1,xmm5
   18027eaaa:	c5 fa 11 05 aa 74 1f 	vmovss DWORD PTR [rip+0x1f74aa],xmm0        # 0x180475f5c
   18027eab1:	00 
   18027eab2:	c5 fa 11 15 9e 74 1f 	vmovss DWORD PTR [rip+0x1f749e],xmm2        # 0x180475f58
   18027eab9:	00 
   18027eaba:	c5 da 59 d7          	vmulss xmm2,xmm4,xmm7
   18027eabe:	c5 ea 58 ce          	vaddss xmm1,xmm2,xmm6
   18027eac2:	c5 fa 11 0d 96 74 1f 	vmovss DWORD PTR [rip+0x1f7496],xmm1        # 0x180475f60
   18027eac9:	00 
   18027eaca:	c4 c1 5a 59 c0       	vmulss xmm0,xmm4,xmm8
   18027eacf:	c5 fa 58 d6          	vaddss xmm2,xmm0,xmm6
   18027ead3:	c4 c1 7a 10 87 a0 00 	vmovss xmm0,DWORD PTR [r15+0xa0]
   18027eada:	00 00 
   18027eadc:	c5 fa 11 05 44 72 1f 	vmovss DWORD PTR [rip+0x1f7244],xmm0        # 0x180475d28
   18027eae3:	00 
   18027eae4:	c4 c1 7a 10 8f a4 00 	vmovss xmm1,DWORD PTR [r15+0xa4]
   18027eaeb:	00 00 
   18027eaed:	c5 fa 11 0d 37 72 1f 	vmovss DWORD PTR [rip+0x1f7237],xmm1        # 0x180475d2c
   18027eaf4:	00 
   18027eaf5:	c4 c1 7a 10 87 a8 00 	vmovss xmm0,DWORD PTR [r15+0xa8]
   18027eafc:	00 00 
   18027eafe:	c5 fa 11 05 2a 72 1f 	vmovss DWORD PTR [rip+0x1f722a],xmm0        # 0x180475d30
   18027eb05:	00 
   18027eb06:	c4 c1 7a 10 8f 84 00 	vmovss xmm1,DWORD PTR [r15+0x84]
   18027eb0d:	00 00 
   18027eb0f:	c5 fa 11 0d e9 71 1f 	vmovss DWORD PTR [rip+0x1f71e9],xmm1        # 0x180475d00
   18027eb16:	00 
   18027eb17:	c4 c1 7a 10 87 80 00 	vmovss xmm0,DWORD PTR [r15+0x80]
   18027eb1e:	00 00 
   18027eb20:	c5 fa 11 05 e4 71 1f 	vmovss DWORD PTR [rip+0x1f71e4],xmm0        # 0x180475d0c
   18027eb27:	00 
   18027eb28:	c4 c1 7a 10 4f 7c    	vmovss xmm1,DWORD PTR [r15+0x7c]
   18027eb2e:	c5 fa 11 0d e2 71 1f 	vmovss DWORD PTR [rip+0x1f71e2],xmm1        # 0x180475d18
   18027eb35:	00 
   18027eb36:	c4 c1 7a 10 87 90 00 	vmovss xmm0,DWORD PTR [r15+0x90]
   18027eb3d:	00 00 
   18027eb3f:	c5 fa 11 05 bd 71 1f 	vmovss DWORD PTR [rip+0x1f71bd],xmm0        # 0x180475d04
   18027eb46:	00 
   18027eb47:	c4 c1 7a 10 8f 8c 00 	vmovss xmm1,DWORD PTR [r15+0x8c]
   18027eb4e:	00 00 
   18027eb50:	c5 fa 11 0d b8 71 1f 	vmovss DWORD PTR [rip+0x1f71b8],xmm1        # 0x180475d10
   18027eb57:	00 
   18027eb58:	c4 c1 7a 10 87 88 00 	vmovss xmm0,DWORD PTR [r15+0x88]
   18027eb5f:	00 00 
   18027eb61:	c5 fa 11 05 b3 71 1f 	vmovss DWORD PTR [rip+0x1f71b3],xmm0        # 0x180475d1c
   18027eb68:	00 
   18027eb69:	c4 c1 7a 10 8f 9c 00 	vmovss xmm1,DWORD PTR [r15+0x9c]
   18027eb70:	00 00 
   18027eb72:	c5 fa 11 0d 8e 71 1f 	vmovss DWORD PTR [rip+0x1f718e],xmm1        # 0x180475d08
   18027eb79:	00 
   18027eb7a:	c4 c1 7a 10 87 98 00 	vmovss xmm0,DWORD PTR [r15+0x98]
   18027eb81:	00 00 
   18027eb83:	c5 fa 11 05 89 71 1f 	vmovss DWORD PTR [rip+0x1f7189],xmm0        # 0x180475d14
   18027eb8a:	00 
   18027eb8b:	c4 c1 7a 10 8f 94 00 	vmovss xmm1,DWORD PTR [r15+0x94]
   18027eb92:	00 00 
   18027eb94:	c5 fa 11 0d 84 71 1f 	vmovss DWORD PTR [rip+0x1f7184],xmm1        # 0x180475d20
   18027eb9b:	00 
   18027eb9c:	c5 7a 11 25 a4 73 1f 	vmovss DWORD PTR [rip+0x1f73a4],xmm12        # 0x180475f48
   18027eba3:	00 
   18027eba4:	c5 7a 11 2d a0 73 1f 	vmovss DWORD PTR [rip+0x1f73a0],xmm13        # 0x180475f4c
   18027ebab:	00 
   18027ebac:	c5 7a 11 35 9c 73 1f 	vmovss DWORD PTR [rip+0x1f739c],xmm14        # 0x180475f50
   18027ebb3:	00 
   18027ebb4:	c5 7a 11 3d 98 73 1f 	vmovss DWORD PTR [rip+0x1f7398],xmm15        # 0x180475f54
   18027ebbb:	00 
   18027ebbc:	c5 fa 11 15 a0 73 1f 	vmovss DWORD PTR [rip+0x1f73a0],xmm2        # 0x180475f64
   18027ebc3:	00 
   18027ebc4:	c6 05 79 71 1f 00 01 	mov    BYTE PTR [rip+0x1f7179],0x1        # 0x180475d44
   18027ebcb:	eb 16                	jmp    0x18027ebe3
   18027ebcd:	c5 7a 10 0d ef d0 18 	vmovss xmm9,DWORD PTR [rip+0x18d0ef]        # 0x18040bcc4
   18027ebd4:	00 
   18027ebd5:	eb 0c                	jmp    0x18027ebe3
   18027ebd7:	48 89 1d 7a 71 1f 00 	mov    QWORD PTR [rip+0x1f717a],rbx        # 0x180475d58
   18027ebde:	e8 dd 06 00 00       	call   0x18027f2c0
   18027ebe3:	c4 c1 7a 10 4d 08    	vmovss xmm1,DWORD PTR [r13+0x8]
   18027ebe9:	c4 c1 72 58 45 0c    	vaddss xmm0,xmm1,DWORD PTR [r13+0xc]
   18027ebef:	c4 c1 72 5c 4d 0c    	vsubss xmm1,xmm1,DWORD PTR [r13+0xc]
   18027ebf5:	c4 c1 72 59 f9       	vmulss xmm7,xmm1,xmm9
   18027ebfa:	c5 c2 59 15 02 25 1e 	vmulss xmm2,xmm7,DWORD PTR [rip+0x1e2502]        # 0x180461104
   18027ec01:	00 
   18027ec02:	c4 41 7a 59 c1       	vmulss xmm8,xmm0,xmm9
   18027ec07:	c4 c1 6a 58 c0       	vaddss xmm0,xmm2,xmm8
   18027ec0c:	c5 f8 77             	vzeroupper
   18027ec0f:	e8 a9 0d 03 00       	call   0x1802af9bd
   18027ec14:	c5 c2 59 15 e0 24 1e 	vmulss xmm2,xmm7,DWORD PTR [rip+0x1e24e0]        # 0x1804610fc
   18027ec1b:	00 
   18027ec1c:	c5 f8 28 f0          	vmovaps xmm6,xmm0
   18027ec20:	c4 c1 6a 58 c0       	vaddss xmm0,xmm2,xmm8
   18027ec25:	e8 93 0d 03 00       	call   0x1802af9bd
   18027ec2a:	c5 fc 10 55 a0       	vmovups ymm2,YMMWORD PTR [rbp-0x60]
   18027ec2f:	c5 ca 5c c8          	vsubss xmm1,xmm6,xmm0
   18027ec33:	c5 fc 10 45 c0       	vmovups ymm0,YMMWORD PTR [rbp-0x40]
   18027ec38:	c5 fa 11 0d 2c 73 1f 	vmovss DWORD PTR [rip+0x1f732c],xmm1        # 0x180475f6c
   18027ec3f:	00 
   18027ec40:	c5 fb 10 4d e0       	vmovsd xmm1,QWORD PTR [rbp-0x20]
   18027ec45:	c5 fb 11 0d db 73 1f 	vmovsd QWORD PTR [rip+0x1f73db],xmm1        # 0x180476028
   18027ec4c:	00 
   18027ec4d:	c5 fc 11 15 93 73 1f 	vmovups YMMWORD PTR [rip+0x1f7393],ymm2        # 0x180475fe8
   18027ec54:	00 
   18027ec55:	c5 fc 11 05 ab 73 1f 	vmovups YMMWORD PTR [rip+0x1f73ab],ymm0        # 0x180476008
   18027ec5c:	00 
   18027ec5d:	c6 05 cd 73 1f 00 01 	mov    BYTE PTR [rip+0x1f73cd],0x1        # 0x180476031
   18027ec64:	33 d2                	xor    edx,edx
   18027ec66:	c6 05 7e 70 1f 00 01 	mov    BYTE PTR [rip+0x1f707e],0x1        # 0x180475ceb
   18027ec6d:	49 8b cf             	mov    rcx,r15
   18027ec70:	c5 f8 77             	vzeroupper
   18027ec73:	e8 48 03 00 00       	call   0x18027efc0
   18027ec78:	c5 78 28 bc 24 90 02 	vmovaps xmm15,XMMWORD PTR [rsp+0x290]
   18027ec7f:	00 00 
   18027ec81:	c5 78 28 b4 24 a0 02 	vmovaps xmm14,XMMWORD PTR [rsp+0x2a0]
   18027ec88:	00 00 
   18027ec8a:	c5 78 28 ac 24 b0 02 	vmovaps xmm13,XMMWORD PTR [rsp+0x2b0]
   18027ec91:	00 00 
   18027ec93:	c5 78 28 a4 24 c0 02 	vmovaps xmm12,XMMWORD PTR [rsp+0x2c0]
   18027ec9a:	00 00 
   18027ec9c:	c5 78 28 9c 24 d0 02 	vmovaps xmm11,XMMWORD PTR [rsp+0x2d0]
   18027eca3:	00 00 
   18027eca5:	c5 78 28 94 24 e0 02 	vmovaps xmm10,XMMWORD PTR [rsp+0x2e0]
   18027ecac:	00 00 
   18027ecae:	c5 78 28 84 24 00 03 	vmovaps xmm8,XMMWORD PTR [rsp+0x300]
   18027ecb5:	00 00 
   18027ecb7:	c5 f8 28 bc 24 10 03 	vmovaps xmm7,XMMWORD PTR [rsp+0x310]
   18027ecbe:	00 00 
   18027ecc0:	c5 f8 28 b4 24 20 03 	vmovaps xmm6,XMMWORD PTR [rsp+0x320]
   18027ecc7:	00 00 
   18027ecc9:	e9 8b 00 00 00       	jmp    0x18027ed59
   18027ecce:	38 1d 04 70 1f 00    	cmp    BYTE PTR [rip+0x1f7004],bl        # 0x180475cd8
   18027ecd4:	74 13                	je     0x18027ece9
   18027ecd6:	c6 05 0c 70 1f 00 01 	mov    BYTE PTR [rip+0x1f700c],0x1        # 0x180475ce9
   18027ecdd:	e8 be 16 ed ff       	call   0x1801503a0
   18027ece2:	c6 80 64 02 00 00 01 	mov    BYTE PTR [rax+0x264],0x1
   18027ece9:	88 1d e9 6f 1f 00    	mov    BYTE PTR [rip+0x1f6fe9],bl        # 0x180475cd8
   18027ecef:	66 89 1d 3a 73 1f 00 	mov    WORD PTR [rip+0x1f733a],bx        # 0x180476030
   18027ecf6:	e8 c5 05 00 00       	call   0x18027f2c0
   18027ecfb:	eb 55                	jmp    0x18027ed52
   18027ecfd:	38 1d 2d 73 1f 00    	cmp    BYTE PTR [rip+0x1f732d],bl        # 0x180476030
   18027ed03:	74 13                	je     0x18027ed18
   18027ed05:	c6 05 dd 6f 1f 00 01 	mov    BYTE PTR [rip+0x1f6fdd],0x1        # 0x180475ce9
   18027ed0c:	e8 8f 16 ed ff       	call   0x1801503a0
   18027ed11:	c6 80 64 02 00 00 01 	mov    BYTE PTR [rax+0x264],0x1
   18027ed18:	66 89 1d 11 73 1f 00 	mov    WORD PTR [rip+0x1f7311],bx        # 0x180476030
   18027ed1f:	88 1d 22 70 1f 00    	mov    BYTE PTR [rip+0x1f7022],bl        # 0x180475d47
   18027ed25:	88 1d 1b 70 1f 00    	mov    BYTE PTR [rip+0x1f701b],bl        # 0x180475d46
   18027ed2b:	48 89 1d 2e 70 1f 00 	mov    QWORD PTR [rip+0x1f702e],rbx        # 0x180475d60
   18027ed32:	89 1d 18 70 1f 00    	mov    DWORD PTR [rip+0x1f7018],ebx        # 0x180475d50
   18027ed38:	48 89 1d 09 70 1f 00 	mov    QWORD PTR [rip+0x1f7009],rbx        # 0x180475d48
   18027ed3f:	e8 5c 16 ed ff       	call   0x1801503a0
   18027ed44:	48 8b c8             	mov    rcx,rax
   18027ed47:	e8 64 80 fe ff       	call   0x180266db0
   18027ed4c:	88 1d 86 6f 1f 00    	mov    BYTE PTR [rip+0x1f6f86],bl        # 0x180475cd8
   18027ed52:	48 89 1d ff 6f 1f 00 	mov    QWORD PTR [rip+0x1f6fff],rbx        # 0x180475d58
   18027ed59:	4c 8b a4 24 50 03 00 	mov    r12,QWORD PTR [rsp+0x350]
   18027ed60:	00 
   18027ed61:	4c 8b b4 24 40 03 00 	mov    r14,QWORD PTR [rsp+0x340]
   18027ed68:	00 
   18027ed69:	4c 8b ac 24 48 03 00 	mov    r13,QWORD PTR [rsp+0x348]
   18027ed70:	00 
   18027ed71:	4c 8b bc 24 38 03 00 	mov    r15,QWORD PTR [rsp+0x338]
   18027ed78:	00 
   18027ed79:	48 8b bc 24 58 03 00 	mov    rdi,QWORD PTR [rsp+0x358]
   18027ed80:	00 
   18027ed81:	48 8b b4 24 60 03 00 	mov    rsi,QWORD PTR [rsp+0x360]
   18027ed88:	00 
   18027ed89:	48 8b 9c 24 68 03 00 	mov    rbx,QWORD PTR [rsp+0x368]
   18027ed90:	00 
   18027ed91:	c5 78 28 8c 24 f0 02 	vmovaps xmm9,XMMWORD PTR [rsp+0x2f0]
   18027ed98:	00 00 
   18027ed9a:	48 81 c4 70 03 00 00 	add    rsp,0x370
   18027eda1:	5d                   	pop    rbp
   18027eda2:	c3                   	ret
   18027eda3:	cc                   	int3
   18027eda4:	cc                   	int3
   18027eda5:	cc                   	int3
   18027eda6:	cc                   	int3
   18027eda7:	cc                   	int3
   18027eda8:	cc                   	int3
   18027eda9:	cc                   	int3
   18027edaa:	cc                   	int3
   18027edab:	cc                   	int3
   18027edac:	cc                   	int3
   18027edad:	cc                   	int3
   18027edae:	cc                   	int3
   18027edaf:	cc                   	int3
   18027edb0:	c5 fa 10 82 a0 00 00 	vmovss xmm0,DWORD PTR [rdx+0xa0]
   18027edb7:	00 
   18027edb8:	c5 fa 10 8a a4 00 00 	vmovss xmm1,DWORD PTR [rdx+0xa4]
   18027edbf:	00 
   18027edc0:	c5 fa 5a c0          	vcvtss2sd xmm0,xmm0,xmm0
   18027edc4:	c5 fb 11 01          	vmovsd QWORD PTR [rcx],xmm0
   18027edc8:	c5 fa 10 82 a8 00 00 	vmovss xmm0,DWORD PTR [rdx+0xa8]
   18027edcf:	00 
   18027edd0:	c5 fa 5a c0          	vcvtss2sd xmm0,xmm0,xmm0
   18027edd4:	c5 f2 5a c9          	vcvtss2sd xmm1,xmm1,xmm1
   18027edd8:	c5 fb 11 41 10       	vmovsd QWORD PTR [rcx+0x10],xmm0
   18027eddd:	c5 fa 10 82 88 00 00 	vmovss xmm0,DWORD PTR [rdx+0x88]
   18027ede4:	00 
   18027ede5:	c5 fb 11 49 08       	vmovsd QWORD PTR [rcx+0x8],xmm1
   18027edea:	c5 fa 10 4a 7c       	vmovss xmm1,DWORD PTR [rdx+0x7c]
   18027edef:	c5 fa 5a c0          	vcvtss2sd xmm0,xmm0,xmm0
   18027edf3:	c5 f2 5a c9          	vcvtss2sd xmm1,xmm1,xmm1
   18027edf7:	c5 fb 11 41 20       	vmovsd QWORD PTR [rcx+0x20],xmm0
   18027edfc:	c5 fa 10 82 80 00 00 	vmovss xmm0,DWORD PTR [rdx+0x80]
   18027ee03:	00 
   18027ee04:	c5 fb 11 49 18       	vmovsd QWORD PTR [rcx+0x18],xmm1
   18027ee09:	c5 fa 10 8a 94 00 00 	vmovss xmm1,DWORD PTR [rdx+0x94]
   18027ee10:	00 
   18027ee11:	c5 fa 5a c0          	vcvtss2sd xmm0,xmm0,xmm0
   18027ee15:	c5 f2 5a c9          	vcvtss2sd xmm1,xmm1,xmm1
   18027ee19:	c5 fb 11 41 30       	vmovsd QWORD PTR [rcx+0x30],xmm0
   18027ee1e:	c5 fa 10 82 98 00 00 	vmovss xmm0,DWORD PTR [rdx+0x98]
   18027ee25:	00 
   18027ee26:	c5 fb 11 49 28       	vmovsd QWORD PTR [rcx+0x28],xmm1
   18027ee2b:	c5 fa 10 8a 8c 00 00 	vmovss xmm1,DWORD PTR [rdx+0x8c]
   18027ee32:	00 
   18027ee33:	c5 fa 5a c0          	vcvtss2sd xmm0,xmm0,xmm0
   18027ee37:	c5 f2 5a c9          	vcvtss2sd xmm1,xmm1,xmm1
   18027ee3b:	c5 fb 11 41 40       	vmovsd QWORD PTR [rcx+0x40],xmm0
   18027ee40:	c5 fb 11 49 38       	vmovsd QWORD PTR [rcx+0x38],xmm1
   18027ee45:	48 8b c1             	mov    rax,rcx
   18027ee48:	c3                   	ret
   18027ee49:	cc                   	int3
   18027ee4a:	cc                   	int3
   18027ee4b:	cc                   	int3
   18027ee4c:	cc                   	int3
   18027ee4d:	cc                   	int3
   18027ee4e:	cc                   	int3
   18027ee4f:	cc                   	int3
   18027ee50:	48 89 5c 24 18       	mov    QWORD PTR [rsp+0x18],rbx
   18027ee55:	48 89 6c 24 20       	mov    QWORD PTR [rsp+0x20],rbp
   18027ee5a:	56                   	push   rsi
   18027ee5b:	57                   	push   rdi
   18027ee5c:	41 56                	push   r14
   18027ee5e:	48 83 ec 30          	sub    rsp,0x30
   18027ee62:	8b 05 28 6e 1f 00    	mov    eax,DWORD PTR [rip+0x1f6e28]        # 0x180475c90
   18027ee68:	85 c0                	test   eax,eax
   18027ee6a:	0f 84 bd 00 00 00    	je     0x18027ef2d
   18027ee70:	4c 8d 35 c9 24 c0 00 	lea    r14,[rip+0xc024c9]        # 0x180e81340
   18027ee77:	33 ed                	xor    ebp,ebp
   18027ee79:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
   18027ee80:	ff c8                	dec    eax
   18027ee82:	89 05 08 6e 1f 00    	mov    DWORD PTR [rip+0x1f6e08],eax        # 0x180475c90
   18027ee88:	48 8d 04 80          	lea    rax,[rax+rax*4]
   18027ee8c:	c4 c1 78 10 44 c6 08 	vmovups xmm0,XMMWORD PTR [r14+rax*8+0x8]
   18027ee93:	c5 f8 11 44 24 20    	vmovups XMMWORD PTR [rsp+0x20],xmm0
   18027ee99:	c4 c1 7b 10 44 c6 18 	vmovsd xmm0,QWORD PTR [r14+rax*8+0x18]
   18027eea0:	c5 fb 11 44 24 58    	vmovsd QWORD PTR [rsp+0x58],xmm0
   18027eea6:	41 8b 7c c6 20       	mov    edi,DWORD PTR [r14+rax*8+0x20]
   18027eeab:	49 8d 1c c6          	lea    rbx,[r14+rax*8]
   18027eeaf:	48 8b 33             	mov    rsi,QWORD PTR [rbx]
   18027eeb2:	e8 69 6a eb ff       	call   0x180135920
   18027eeb7:	90                   	nop
   18027eeb8:	e8 63 6a eb ff       	call   0x180135920
   18027eebd:	0f b6 88 18 01 00 00 	movzx  ecx,BYTE PTR [rax+0x118]
   18027eec4:	83 e9 01             	sub    ecx,0x1
   18027eec7:	74 0a                	je     0x18027eed3
   18027eec9:	83 f9 03             	cmp    ecx,0x3
   18027eecc:	b8 cc 01 00 00       	mov    eax,0x1cc
   18027eed1:	74 05                	je     0x18027eed8
   18027eed3:	b8 50 01 00 00       	mov    eax,0x150
   18027eed8:	c5 f8 10 44 24 20    	vmovups xmm0,XMMWORD PTR [rsp+0x20]
   18027eede:	c5 f8 11 04 30       	vmovups XMMWORD PTR [rax+rsi*1],xmm0
   18027eee3:	c5 fb 10 44 24 58    	vmovsd xmm0,QWORD PTR [rsp+0x58]
   18027eee9:	c5 fb 11 44 30 10    	vmovsd QWORD PTR [rax+rsi*1+0x10],xmm0
   18027eeef:	89 7c 30 18          	mov    DWORD PTR [rax+rsi*1+0x18],edi
   18027eef3:	48 8b 0b             	mov    rcx,QWORD PTR [rbx]
   18027eef6:	33 c0                	xor    eax,eax
   18027eef8:	48 89 44 24 50       	mov    QWORD PTR [rsp+0x50],rax
   18027eefd:	89 6c 24 54          	mov    DWORD PTR [rsp+0x54],ebp
   18027ef01:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   18027ef04:	48 8d 54 24 50       	lea    rdx,[rsp+0x50]
   18027ef09:	ff 90 80 01 00 00    	call   QWORD PTR [rax+0x180]
   18027ef0f:	48 8b 0b             	mov    rcx,QWORD PTR [rbx]
   18027ef12:	48 85 c9             	test   rcx,rcx
   18027ef15:	74 08                	je     0x18027ef1f
   18027ef17:	e8 d4 17 f3 ff       	call   0x1801b06f0
   18027ef1c:	48 89 2b             	mov    QWORD PTR [rbx],rbp
   18027ef1f:	8b 05 6b 6d 1f 00    	mov    eax,DWORD PTR [rip+0x1f6d6b]        # 0x180475c90
   18027ef25:	85 c0                	test   eax,eax
   18027ef27:	0f 85 53 ff ff ff    	jne    0x18027ee80
   18027ef2d:	c6 05 b7 6d 1f 00 00 	mov    BYTE PTR [rip+0x1f6db7],0x0        # 0x180475ceb
   18027ef34:	48 8b 5c 24 60       	mov    rbx,QWORD PTR [rsp+0x60]
   18027ef39:	48 8b 6c 24 68       	mov    rbp,QWORD PTR [rsp+0x68]
   18027ef3e:	48 83 c4 30          	add    rsp,0x30
   18027ef42:	41 5e                	pop    r14
   18027ef44:	5f                   	pop    rdi
   18027ef45:	5e                   	pop    rsi
   18027ef46:	c3                   	ret
   18027ef47:	cc                   	int3
   18027ef48:	cc                   	int3
   18027ef49:	cc                   	int3
   18027ef4a:	cc                   	int3
   18027ef4b:	cc                   	int3
   18027ef4c:	cc                   	int3
   18027ef4d:	cc                   	int3
   18027ef4e:	cc                   	int3
   18027ef4f:	cc                   	int3
   18027ef50:	48 89 5c 24 08       	mov    QWORD PTR [rsp+0x8],rbx
   18027ef55:	57                   	push   rdi
   18027ef56:	48 83 ec 20          	sub    rsp,0x20
   18027ef5a:	48 8b fa             	mov    rdi,rdx
   18027ef5d:	48 8b d9             	mov    rbx,rcx
   18027ef60:	48 3b ca             	cmp    rcx,rdx
   18027ef63:	74 21                	je     0x18027ef86
   18027ef65:	48 8b 09             	mov    rcx,QWORD PTR [rcx]
   18027ef68:	48 85 c9             	test   rcx,rcx
   18027ef6b:	74 0c                	je     0x18027ef79
   18027ef6d:	e8 7e 17 f3 ff       	call   0x1801b06f0
   18027ef72:	48 c7 03 00 00 00 00 	mov    QWORD PTR [rbx],0x0
   18027ef79:	48 8b 07             	mov    rax,QWORD PTR [rdi]
   18027ef7c:	48 89 03             	mov    QWORD PTR [rbx],rax
   18027ef7f:	48 c7 07 00 00 00 00 	mov    QWORD PTR [rdi],0x0
   18027ef86:	c5 f8 10 47 08       	vmovups xmm0,XMMWORD PTR [rdi+0x8]
   18027ef8b:	c5 f8 11 43 08       	vmovups XMMWORD PTR [rbx+0x8],xmm0
   18027ef90:	c5 fb 10 4f 18       	vmovsd xmm1,QWORD PTR [rdi+0x18]
   18027ef95:	c5 fb 11 4b 18       	vmovsd QWORD PTR [rbx+0x18],xmm1
   18027ef9a:	8b 47 20             	mov    eax,DWORD PTR [rdi+0x20]
   18027ef9d:	89 43 20             	mov    DWORD PTR [rbx+0x20],eax
   18027efa0:	48 8b c3             	mov    rax,rbx
   18027efa3:	48 8b 5c 24 30       	mov    rbx,QWORD PTR [rsp+0x30]
   18027efa8:	48 83 c4 20          	add    rsp,0x20
   18027efac:	5f                   	pop    rdi
   18027efad:	c3                   	ret
   18027efae:	cc                   	int3
   18027efaf:	cc                   	int3
   18027efb0:	cc                   	int3
   18027efb1:	cc                   	int3
   18027efb2:	cc                   	int3
   18027efb3:	cc                   	int3
   18027efb4:	cc                   	int3
   18027efb5:	cc                   	int3
   18027efb6:	cc                   	int3
   18027efb7:	cc                   	int3
   18027efb8:	cc                   	int3
   18027efb9:	cc                   	int3
   18027efba:	cc                   	int3
   18027efbb:	cc                   	int3
   18027efbc:	cc                   	int3
   18027efbd:	cc                   	int3
   18027efbe:	cc                   	int3
   18027efbf:	cc                   	int3
   18027efc0:	48 85 c9             	test   rcx,rcx
   18027efc3:	0f 84 f8 01 00 00    	je     0x18027f1c1
   18027efc9:	48 89 5c 24 10       	mov    QWORD PTR [rsp+0x10],rbx
   18027efce:	48 89 6c 24 18       	mov    QWORD PTR [rsp+0x18],rbp
   18027efd3:	56                   	push   rsi
   18027efd4:	57                   	push   rdi
   18027efd5:	41 56                	push   r14
   18027efd7:	48 83 ec 70          	sub    rsp,0x70
   18027efdb:	c5 f8 29 74 24 60    	vmovaps XMMWORD PTR [rsp+0x60],xmm6
   18027efe1:	c5 f8 29 7c 24 50    	vmovaps XMMWORD PTR [rsp+0x50],xmm7
   18027efe7:	0f b6 f2             	movzx  esi,dl
   18027efea:	48 8b d9             	mov    rbx,rcx
   18027efed:	e8 2e 69 eb ff       	call   0x180135920
   18027eff2:	90                   	nop
   18027eff3:	e8 28 69 eb ff       	call   0x180135920
   18027eff8:	0f b6 88 18 01 00 00 	movzx  ecx,BYTE PTR [rax+0x118]
   18027efff:	83 e9 01             	sub    ecx,0x1
   18027f002:	74 0a                	je     0x18027f00e
   18027f004:	83 f9 03             	cmp    ecx,0x3
   18027f007:	bf cc 01 00 00       	mov    edi,0x1cc
   18027f00c:	74 05                	je     0x18027f013
   18027f00e:	bf 50 01 00 00       	mov    edi,0x150
   18027f013:	80 7c 1f 18 00       	cmp    BYTE PTR [rdi+rbx*1+0x18],0x0
   18027f018:	0f 85 83 01 00 00    	jne    0x18027f1a1
   18027f01e:	33 c0                	xor    eax,eax
   18027f020:	4c 8d 35 19 23 c0 00 	lea    r14,[rip+0xc02319]        # 0x180e81340
   18027f027:	8b 15 63 6c 1f 00    	mov    edx,DWORD PTR [rip+0x1f6c63]        # 0x180475c90
   18027f02d:	85 d2                	test   edx,edx
   18027f02f:	74 16                	je     0x18027f047
   18027f031:	49 8b ce             	mov    rcx,r14
   18027f034:	48 39 19             	cmp    QWORD PTR [rcx],rbx
   18027f037:	0f 84 85 01 00 00    	je     0x18027f1c2
   18027f03d:	ff c0                	inc    eax
   18027f03f:	48 83 c1 28          	add    rcx,0x28
   18027f043:	3b c2                	cmp    eax,edx
   18027f045:	72 ed                	jb     0x18027f034
   18027f047:	83 fa 02             	cmp    edx,0x2
   18027f04a:	0f 84 51 01 00 00    	je     0x18027f1a1
   18027f050:	48 89 5c 24 20       	mov    QWORD PTR [rsp+0x20],rbx
   18027f055:	48 8b cb             	mov    rcx,rbx
   18027f058:	e8 b3 16 f3 ff       	call   0x1801b0710
   18027f05d:	c5 f8 10 34 1f       	vmovups xmm6,XMMWORD PTR [rdi+rbx*1]
   18027f062:	c5 f8 11 74 24 28    	vmovups XMMWORD PTR [rsp+0x28],xmm6
   18027f068:	c5 fb 10 7c 1f 10    	vmovsd xmm7,QWORD PTR [rdi+rbx*1+0x10]
   18027f06e:	c5 fb 11 7c 24 38    	vmovsd QWORD PTR [rsp+0x38],xmm7
   18027f074:	8b 6c 1f 18          	mov    ebp,DWORD PTR [rdi+rbx*1+0x18]
   18027f078:	89 6c 24 40          	mov    DWORD PTR [rsp+0x40],ebp
   18027f07c:	8b 05 0e 6c 1f 00    	mov    eax,DWORD PTR [rip+0x1f6c0e]        # 0x180475c90
   18027f082:	8b c8                	mov    ecx,eax
   18027f084:	ff c0                	inc    eax
   18027f086:	89 05 04 6c 1f 00    	mov    DWORD PTR [rip+0x1f6c04],eax        # 0x180475c90
   18027f08c:	48 8d 04 89          	lea    rax,[rcx+rcx*4]
   18027f090:	49 8d 34 c6          	lea    rsi,[r14+rax*8]
   18027f094:	48 8b cb             	mov    rcx,rbx
   18027f097:	48 8d 44 24 20       	lea    rax,[rsp+0x20]
   18027f09c:	48 3b f0             	cmp    rsi,rax
   18027f09f:	74 12                	je     0x18027f0b3
   18027f0a1:	48 8b 0e             	mov    rcx,QWORD PTR [rsi]
   18027f0a4:	48 85 c9             	test   rcx,rcx
   18027f0a7:	74 05                	je     0x18027f0ae
   18027f0a9:	e8 42 16 f3 ff       	call   0x1801b06f0
   18027f0ae:	48 89 1e             	mov    QWORD PTR [rsi],rbx
   18027f0b1:	33 c9                	xor    ecx,ecx
   18027f0b3:	c5 f8 11 76 08       	vmovups XMMWORD PTR [rsi+0x8],xmm6
   18027f0b8:	c5 fb 11 7e 18       	vmovsd QWORD PTR [rsi+0x18],xmm7
   18027f0bd:	89 6e 20             	mov    DWORD PTR [rsi+0x20],ebp
   18027f0c0:	48 85 c9             	test   rcx,rcx
   18027f0c3:	74 06                	je     0x18027f0cb
   18027f0c5:	e8 26 16 f3 ff       	call   0x1801b06f0
   18027f0ca:	90                   	nop
   18027f0cb:	c5 fa 10 4c 1f 04    	vmovss xmm1,DWORD PTR [rdi+rbx*1+0x4]
   18027f0d1:	c5 fa 10 64 1f 08    	vmovss xmm4,DWORD PTR [rdi+rbx*1+0x8]
   18027f0d7:	c5 f2 58 04 1f       	vaddss xmm0,xmm1,DWORD PTR [rdi+rbx*1]
   18027f0dc:	c5 fa 10 1d e0 cb 18 	vmovss xmm3,DWORD PTR [rip+0x18cbe0]        # 0x18040bcc4
   18027f0e3:	00 
   18027f0e4:	c5 fa 59 f3          	vmulss xmm6,xmm0,xmm3
   18027f0e8:	c5 da 58 44 1f 0c    	vaddss xmm0,xmm4,DWORD PTR [rdi+rbx*1+0xc]
   18027f0ee:	c5 fa 59 fb          	vmulss xmm7,xmm0,xmm3
   18027f0f2:	c5 f2 5c 0c 1f       	vsubss xmm1,xmm1,DWORD PTR [rdi+rbx*1]
   18027f0f7:	c5 f2 59 d3          	vmulss xmm2,xmm1,xmm3
   18027f0fb:	c5 da 5c 44 1f 0c    	vsubss xmm0,xmm4,DWORD PTR [rdi+rbx*1+0xc]
   18027f101:	c5 fa 59 e3          	vmulss xmm4,xmm0,xmm3
   18027f105:	c5 ea 59 0d eb 1f 1e 	vmulss xmm1,xmm2,DWORD PTR [rip+0x1e1feb]        # 0x1804610f8
   18027f10c:	00 
   18027f10d:	c5 f2 58 ee          	vaddss xmm5,xmm1,xmm6
   18027f111:	c5 ea 59 05 e7 1f 1e 	vmulss xmm0,xmm2,DWORD PTR [rip+0x1e1fe7]        # 0x180461100
   18027f118:	00 
   18027f119:	c5 fa 58 de          	vaddss xmm3,xmm0,xmm6
   18027f11d:	c5 da 59 0d d7 1f 1e 	vmulss xmm1,xmm4,DWORD PTR [rip+0x1e1fd7]        # 0x1804610fc
   18027f124:	00 
   18027f125:	c5 f2 58 d7          	vaddss xmm2,xmm1,xmm7
   18027f129:	c5 da 59 05 d3 1f 1e 	vmulss xmm0,xmm4,DWORD PTR [rip+0x1e1fd3]        # 0x180461104
   18027f130:	00 
   18027f131:	c5 fa 58 cf          	vaddss xmm1,xmm0,xmm7
   18027f135:	c5 fa 11 2c 1f       	vmovss DWORD PTR [rdi+rbx*1],xmm5
   18027f13a:	c5 fa 11 5c 1f 04    	vmovss DWORD PTR [rdi+rbx*1+0x4],xmm3
   18027f140:	c5 fa 11 54 1f 0c    	vmovss DWORD PTR [rdi+rbx*1+0xc],xmm2
   18027f146:	c5 fa 11 4c 1f 08    	vmovss DWORD PTR [rdi+rbx*1+0x8],xmm1
   18027f14c:	48 3b 1d 8d 6b 1f 00 	cmp    rbx,QWORD PTR [rip+0x1f6b8d]        # 0x180475ce0
   18027f153:	75 27                	jne    0x18027f17c
   18027f155:	c5 fa 11 2d 1b 6e 1f 	vmovss DWORD PTR [rip+0x1f6e1b],xmm5        # 0x180475f78
   18027f15c:	00 
   18027f15d:	c5 fa 11 1d 17 6e 1f 	vmovss DWORD PTR [rip+0x1f6e17],xmm3        # 0x180475f7c
   18027f164:	00 
   18027f165:	c5 fa 11 15 13 6e 1f 	vmovss DWORD PTR [rip+0x1f6e13],xmm2        # 0x180475f80
   18027f16c:	00 
   18027f16d:	c5 fa 11 0d 0f 6e 1f 	vmovss DWORD PTR [rip+0x1f6e0f],xmm1        # 0x180475f84
   18027f174:	00 
   18027f175:	c6 05 ec 6d 1f 00 01 	mov    BYTE PTR [rip+0x1f6dec],0x1        # 0x180475f68
   18027f17c:	33 c0                	xor    eax,eax
   18027f17e:	48 89 84 24 90 00 00 	mov    QWORD PTR [rsp+0x90],rax
   18027f185:	00 
   18027f186:	89 84 24 94 00 00 00 	mov    DWORD PTR [rsp+0x94],eax
   18027f18d:	48 8b 03             	mov    rax,QWORD PTR [rbx]
   18027f190:	48 8d 94 24 90 00 00 	lea    rdx,[rsp+0x90]
   18027f197:	00 
   18027f198:	48 8b cb             	mov    rcx,rbx
   18027f19b:	ff 90 80 01 00 00    	call   QWORD PTR [rax+0x180]
   18027f1a1:	4c 8d 5c 24 70       	lea    r11,[rsp+0x70]
   18027f1a6:	49 8b 5b 28          	mov    rbx,QWORD PTR [r11+0x28]
   18027f1aa:	49 8b 6b 30          	mov    rbp,QWORD PTR [r11+0x30]
   18027f1ae:	c5 f8 28 74 24 60    	vmovaps xmm6,XMMWORD PTR [rsp+0x60]
   18027f1b4:	c5 f8 28 7c 24 50    	vmovaps xmm7,XMMWORD PTR [rsp+0x50]
   18027f1ba:	49 8b e3             	mov    rsp,r11
   18027f1bd:	41 5e                	pop    r14
   18027f1bf:	5f                   	pop    rdi
   18027f1c0:	5e                   	pop    rsi
   18027f1c1:	c3                   	ret
   18027f1c2:	3b c2                	cmp    eax,edx
   18027f1c4:	0f 83 7d fe ff ff    	jae    0x18027f047
   18027f1ca:	40 84 f6             	test   sil,sil
   18027f1cd:	74 d2                	je     0x18027f1a1
   18027f1cf:	c5 f8 10 0c 1f       	vmovups xmm1,XMMWORD PTR [rdi+rbx*1]
   18027f1d4:	c5 fb 10 44 1f 10    	vmovsd xmm0,QWORD PTR [rdi+rbx*1+0x10]
   18027f1da:	8b 54 1f 18          	mov    edx,DWORD PTR [rdi+rbx*1+0x18]
   18027f1de:	48 8d 0c 80          	lea    rcx,[rax+rax*4]
   18027f1e2:	c4 c1 78 11 4c ce 08 	vmovups XMMWORD PTR [r14+rcx*8+0x8],xmm1
   18027f1e9:	c4 c1 7b 11 44 ce 18 	vmovsd QWORD PTR [r14+rcx*8+0x18],xmm0
   18027f1f0:	41 89 54 ce 20       	mov    DWORD PTR [r14+rcx*8+0x20],edx
   18027f1f5:	e9 d1 fe ff ff       	jmp    0x18027f0cb
   18027f1fa:	cc                   	int3
   18027f1fb:	cc                   	int3
   18027f1fc:	cc                   	int3
   18027f1fd:	cc                   	int3
   18027f1fe:	cc                   	int3
   18027f1ff:	cc                   	int3
   18027f200:	48 83 ec 28          	sub    rsp,0x28
   18027f204:	33 c0                	xor    eax,eax
   18027f206:	48 8d 54 24 30       	lea    rdx,[rsp+0x30]
   18027f20b:	48 89 44 24 30       	mov    QWORD PTR [rsp+0x30],rax
   18027f210:	89 44 24 34          	mov    DWORD PTR [rsp+0x34],eax
   18027f214:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   18027f217:	ff 90 80 01 00 00    	call   QWORD PTR [rax+0x180]
   18027f21d:	48 83 c4 28          	add    rsp,0x28
   18027f221:	c3                   	ret
   18027f222:	cc                   	int3
   18027f223:	cc                   	int3
   18027f224:	cc                   	int3
   18027f225:	cc                   	int3
   18027f226:	cc                   	int3
   18027f227:	cc                   	int3
   18027f228:	cc                   	int3
   18027f229:	cc                   	int3
   18027f22a:	cc                   	int3
   18027f22b:	cc                   	int3
   18027f22c:	cc                   	int3
   18027f22d:	cc                   	int3
   18027f22e:	cc                   	int3
   18027f22f:	cc                   	int3
   18027f230:	40 53                	rex push rbx
   18027f232:	48 83 ec 20          	sub    rsp,0x20
   18027f236:	48 8b d9             	mov    rbx,rcx
   18027f239:	48 8b 09             	mov    rcx,QWORD PTR [rcx]
   18027f23c:	48 85 c9             	test   rcx,rcx
   18027f23f:	74 0c                	je     0x18027f24d
   18027f241:	e8 aa 14 f3 ff       	call   0x1801b06f0
   18027f246:	48 c7 03 00 00 00 00 	mov    QWORD PTR [rbx],0x0
   18027f24d:	48 83 c4 20          	add    rsp,0x20
   18027f251:	5b                   	pop    rbx
   18027f252:	c3                   	ret
   18027f253:	cc                   	int3
   18027f254:	cc                   	int3
   18027f255:	cc                   	int3
   18027f256:	cc                   	int3
   18027f257:	cc                   	int3
   18027f258:	cc                   	int3
   18027f259:	cc                   	int3
   18027f25a:	cc                   	int3
   18027f25b:	cc                   	int3
   18027f25c:	cc                   	int3
   18027f25d:	cc                   	int3
   18027f25e:	cc                   	int3
   18027f25f:	cc                   	int3
   18027f260:	48 83 ec 28          	sub    rsp,0x28
   18027f264:	4c 8d 0d c5 ff ff ff 	lea    r9,[rip+0xffffffffffffffc5]        # 0x18027f230
   18027f26b:	ba 28 00 00 00       	mov    edx,0x28
   18027f270:	41 b8 02 00 00 00    	mov    r8d,0x2
   18027f276:	e8 35 a0 fb ff       	call   0x1802392b0
   18027f27b:	90                   	nop
   18027f27c:	48 83 c4 28          	add    rsp,0x28
   18027f280:	c3                   	ret
   18027f281:	cc                   	int3
   18027f282:	cc                   	int3
   18027f283:	cc                   	int3
   18027f284:	cc                   	int3
   18027f285:	cc                   	int3
   18027f286:	cc                   	int3
   18027f287:	cc                   	int3
   18027f288:	cc                   	int3
   18027f289:	cc                   	int3
   18027f28a:	cc                   	int3
   18027f28b:	cc                   	int3
   18027f28c:	cc                   	int3
   18027f28d:	cc                   	int3
   18027f28e:	cc                   	int3
   18027f28f:	cc                   	int3
   18027f290:	48 83 ec 28          	sub    rsp,0x28
   18027f294:	c6 05 4e 6a 1f 00 01 	mov    BYTE PTR [rip+0x1f6a4e],0x1        # 0x180475ce9
   18027f29b:	e8 00 11 ed ff       	call   0x1801503a0
   18027f2a0:	c6 80 64 02 00 00 01 	mov    BYTE PTR [rax+0x264],0x1
   18027f2a7:	48 83 c4 28          	add    rsp,0x28
   18027f2ab:	c3                   	ret
   18027f2ac:	cc                   	int3
   18027f2ad:	cc                   	int3
   18027f2ae:	cc                   	int3
   18027f2af:	cc                   	int3
   18027f2b0:	cc                   	int3
   18027f2b1:	cc                   	int3
   18027f2b2:	cc                   	int3
   18027f2b3:	cc                   	int3
   18027f2b4:	cc                   	int3
   18027f2b5:	cc                   	int3
   18027f2b6:	cc                   	int3
   18027f2b7:	cc                   	int3
   18027f2b8:	cc                   	int3
   18027f2b9:	cc                   	int3
   18027f2ba:	cc                   	int3
   18027f2bb:	cc                   	int3
   18027f2bc:	cc                   	int3
   18027f2bd:	cc                   	int3
   18027f2be:	cc                   	int3
   18027f2bf:	cc                   	int3
   18027f2c0:	48 83 ec 28          	sub    rsp,0x28
   18027f2c4:	33 c0                	xor    eax,eax
   18027f2c6:	c6 05 7a 6a 1f 00 00 	mov    BYTE PTR [rip+0x1f6a7a],0x0        # 0x180475d47
   18027f2cd:	48 89 05 8c 6a 1f 00 	mov    QWORD PTR [rip+0x1f6a8c],rax        # 0x180475d60
   18027f2d4:	89 05 76 6a 1f 00    	mov    DWORD PTR [rip+0x1f6a76],eax        # 0x180475d50
   18027f2da:	48 89 05 67 6a 1f 00 	mov    QWORD PTR [rip+0x1f6a67],rax        # 0x180475d48
   18027f2e1:	c6 05 5e 6a 1f 00 00 	mov    BYTE PTR [rip+0x1f6a5e],0x0        # 0x180475d46
   18027f2e8:	e8 b3 10 ed ff       	call   0x1801503a0
   18027f2ed:	48 8b c8             	mov    rcx,rax
   18027f2f0:	48 83 c4 28          	add    rsp,0x28
   18027f2f4:	e9 b7 7a fe ff       	jmp    0x180266db0
   18027f2f9:	cc                   	int3
   18027f2fa:	cc                   	int3
   18027f2fb:	cc                   	int3
   18027f2fc:	cc                   	int3
   18027f2fd:	cc                   	int3
   18027f2fe:	cc                   	int3
   18027f2ff:	cc                   	int3
   18027f300:	3b 0d da 1d 1e 00    	cmp    ecx,DWORD PTR [rip+0x1e1dda]        # 0x1804610e0
   18027f306:	75 16                	jne    0x18027f31e
   18027f308:	80 3d 35 6a 1f 00 00 	cmp    BYTE PTR [rip+0x1f6a35],0x0        # 0x180475d44
   18027f30f:	74 0d                	je     0x18027f31e
   18027f311:	c6 05 28 6a 1f 00 01 	mov    BYTE PTR [rip+0x1f6a28],0x1        # 0x180475d40
   18027f318:	88 15 06 6a 1f 00    	mov    BYTE PTR [rip+0x1f6a06],dl        # 0x180475d24
   18027f31e:	c3                   	ret
   18027f31f:	cc                   	int3
   18027f320:	cc                   	int3
   18027f321:	cc                   	int3
   18027f322:	cc                   	int3
   18027f323:	cc                   	int3
   18027f324:	cc                   	int3
   18027f325:	cc                   	int3
   18027f326:	cc                   	int3
   18027f327:	cc                   	int3
   18027f328:	cc                   	int3
   18027f329:	cc                   	int3
   18027f32a:	cc                   	int3
   18027f32b:	cc                   	int3
   18027f32c:	cc                   	int3
   18027f32d:	cc                   	int3
   18027f32e:	cc                   	int3
   18027f32f:	cc                   	int3
   18027f330:	40 53                	rex push rbx
   18027f332:	48 83 ec 20          	sub    rsp,0x20
   18027f336:	e8 15 fb ff ff       	call   0x18027ee50
   18027f33b:	c5 fa 6f 05 bd d4 18 	vmovdqu xmm0,XMMWORD PTR [rip+0x18d4bd]        # 0x18040c800
   18027f342:	00 
   18027f343:	33 db                	xor    ebx,ebx
   18027f345:	66 c7 05 e2 6c 1f 00 	mov    WORD PTR [rip+0x1f6ce2],0x0        # 0x180476030
   18027f34c:	00 00 
   18027f34e:	c5 f8 11 05 a2 1d 1e 	vmovups XMMWORD PTR [rip+0x1e1da2],xmm0        # 0x1804610f8
   18027f355:	00 
   18027f356:	c6 05 ea 69 1f 00 00 	mov    BYTE PTR [rip+0x1f69ea],0x0        # 0x180475d47
   18027f35d:	c6 05 e2 69 1f 00 00 	mov    BYTE PTR [rip+0x1f69e2],0x0        # 0x180475d46
   18027f364:	48 89 1d f5 69 1f 00 	mov    QWORD PTR [rip+0x1f69f5],rbx        # 0x180475d60
   18027f36b:	89 1d df 69 1f 00    	mov    DWORD PTR [rip+0x1f69df],ebx        # 0x180475d50
   18027f371:	48 89 1d d0 69 1f 00 	mov    QWORD PTR [rip+0x1f69d0],rbx        # 0x180475d48
   18027f378:	e8 23 10 ed ff       	call   0x1801503a0
   18027f37d:	48 8b c8             	mov    rcx,rax
   18027f380:	e8 2b 7a fe ff       	call   0x180266db0
   18027f385:	88 1d 99 69 1f 00    	mov    BYTE PTR [rip+0x1f6999],bl        # 0x180475d24
   18027f38b:	88 1d af 69 1f 00    	mov    BYTE PTR [rip+0x1f69af],bl        # 0x180475d40
   18027f391:	88 1d ad 69 1f 00    	mov    BYTE PTR [rip+0x1f69ad],bl        # 0x180475d44
   18027f397:	48 89 1d ba 69 1f 00 	mov    QWORD PTR [rip+0x1f69ba],rbx        # 0x180475d58
   18027f39e:	48 89 1d bb 69 1f 00 	mov    QWORD PTR [rip+0x1f69bb],rbx        # 0x180475d60
   18027f3a5:	89 1d a5 69 1f 00    	mov    DWORD PTR [rip+0x1f69a5],ebx        # 0x180475d50
   18027f3ab:	48 89 1d 96 69 1f 00 	mov    QWORD PTR [rip+0x1f6996],rbx        # 0x180475d48
   18027f3b2:	88 1d 8a 69 1f 00    	mov    BYTE PTR [rip+0x1f698a],bl        # 0x180475d42
   18027f3b8:	88 1d 83 69 1f 00    	mov    BYTE PTR [rip+0x1f6983],bl        # 0x180475d41
   18027f3be:	88 1d 7f 69 1f 00    	mov    BYTE PTR [rip+0x1f697f],bl        # 0x180475d43
   18027f3c4:	c6 05 19 1d 1e 00 01 	mov    BYTE PTR [rip+0x1e1d19],0x1        # 0x1804610e4
   18027f3cb:	88 1d 9f 6b 1f 00    	mov    BYTE PTR [rip+0x1f6b9f],bl        # 0x180475f70
   18027f3d1:	88 1d 91 6b 1f 00    	mov    BYTE PTR [rip+0x1f6b91],bl        # 0x180475f68
   18027f3d7:	c7 05 ff 1c 1e 00 ff 	mov    DWORD PTR [rip+0x1e1cff],0xffffffff        # 0x1804610e0
   18027f3de:	ff ff ff 
   18027f3e1:	88 1d f1 68 1f 00    	mov    BYTE PTR [rip+0x1f68f1],bl        # 0x180475cd8
   18027f3e7:	c6 05 fb 68 1f 00 01 	mov    BYTE PTR [rip+0x1f68fb],0x1        # 0x180475ce9
   18027f3ee:	e8 ad 0f ed ff       	call   0x1801503a0
   18027f3f3:	c6 80 64 02 00 00 01 	mov    BYTE PTR [rax+0x264],0x1
   18027f3fa:	48 83 c4 20          	add    rsp,0x20
   18027f3fe:	5b                   	pop    rbx
   18027f3ff:	c3                   	ret
   18027f400:	cc                   	int3
   18027f401:	cc                   	int3
   18027f402:	cc                   	int3
   18027f403:	cc                   	int3
   18027f404:	cc                   	int3
   18027f405:	cc                   	int3
   18027f406:	cc                   	int3
   18027f407:	cc                   	int3
   18027f408:	cc                   	int3
   18027f409:	cc                   	int3
   18027f40a:	cc                   	int3
   18027f40b:	cc                   	int3
   18027f40c:	cc                   	int3
   18027f40d:	cc                   	int3
   18027f40e:	cc                   	int3
   18027f40f:	cc                   	int3
   18027f410:	80 3d 2f 69 1f 00 00 	cmp    BYTE PTR [rip+0x1f692f],0x0        # 0x180475d46
   18027f417:	74 0c                	je     0x18027f425
   18027f419:	80 3d 27 69 1f 00 00 	cmp    BYTE PTR [rip+0x1f6927],0x0        # 0x180475d47
   18027f420:	74 03                	je     0x18027f425
   18027f422:	b0 01                	mov    al,0x1
   18027f424:	c3                   	ret
   18027f425:	32 c0                	xor    al,al
   18027f427:	c3                   	ret
   18027f428:	cc                   	int3
   18027f429:	cc                   	int3
   18027f42a:	cc                   	int3
   18027f42b:	cc                   	int3
   18027f42c:	cc                   	int3
   18027f42d:	cc                   	int3
   18027f42e:	cc                   	int3
   18027f42f:	cc                   	int3
   18027f430:	48 83 ec 18          	sub    rsp,0x18
   18027f434:	3b 15 a6 1c 1e 00    	cmp    edx,DWORD PTR [rip+0x1e1ca6]        # 0x1804610e0
   18027f43a:	75 13                	jne    0x18027f44f
   18027f43c:	80 3d fd 68 1f 00 00 	cmp    BYTE PTR [rip+0x1f68fd],0x0        # 0x180475d40
   18027f443:	75 0a                	jne    0x18027f44f
   18027f445:	c5 f8 10 05 ab 1c 1e 	vmovups xmm0,XMMWORD PTR [rip+0x1e1cab]        # 0x1804610f8
   18027f44c:	00 
   18027f44d:	eb 08                	jmp    0x18027f457
   18027f44f:	c5 fa 6f 05 a9 d3 18 	vmovdqu xmm0,XMMWORD PTR [rip+0x18d3a9]        # 0x18040c800
   18027f456:	00 
   18027f457:	48 8d 04 24          	lea    rax,[rsp]
   18027f45b:	c5 f8 11 04 24       	vmovups XMMWORD PTR [rsp],xmm0
   18027f460:	c5 f8 10 00          	vmovups xmm0,XMMWORD PTR [rax]
   18027f464:	c5 f8 11 01          	vmovups XMMWORD PTR [rcx],xmm0
   18027f468:	48 8b c1             	mov    rax,rcx
   18027f46b:	48 83 c4 18          	add    rsp,0x18
   18027f46f:	c3                   	ret
   18027f470:	cc                   	int3
   18027f471:	cc                   	int3
   18027f472:	cc                   	int3
   18027f473:	cc                   	int3
   18027f474:	cc                   	int3
   18027f475:	cc                   	int3
   18027f476:	cc                   	int3
   18027f477:	cc                   	int3
   18027f478:	cc                   	int3
   18027f479:	cc                   	int3
   18027f47a:	cc                   	int3
   18027f47b:	cc                   	int3
   18027f47c:	cc                   	int3
   18027f47d:	cc                   	int3
   18027f47e:	cc                   	int3
   18027f47f:	cc                   	int3
   18027f480:	3b 0d 5a 1c 1e 00    	cmp    ecx,DWORD PTR [rip+0x1e1c5a]        # 0x1804610e0
   18027f486:	75 18                	jne    0x18027f4a0
   18027f488:	80 3d d9 6a 1f 00 00 	cmp    BYTE PTR [rip+0x1f6ad9],0x0        # 0x180475f68
   18027f48f:	74 0f                	je     0x18027f4a0
   18027f491:	c5 f8 10 05 df 6a 1f 	vmovups xmm0,XMMWORD PTR [rip+0x1f6adf]        # 0x180475f78
   18027f498:	00 
   18027f499:	c5 f8 11 02          	vmovups XMMWORD PTR [rdx],xmm0
   18027f49d:	b0 01                	mov    al,0x1
   18027f49f:	c3                   	ret
   18027f4a0:	32 c0                	xor    al,al
   18027f4a2:	c3                   	ret
   18027f4a3:	cc                   	int3
   18027f4a4:	cc                   	int3
   18027f4a5:	cc                   	int3
   18027f4a6:	cc                   	int3
   18027f4a7:	cc                   	int3
   18027f4a8:	cc                   	int3
   18027f4a9:	cc                   	int3
   18027f4aa:	cc                   	int3
   18027f4ab:	cc                   	int3
   18027f4ac:	cc                   	int3
   18027f4ad:	cc                   	int3
   18027f4ae:	cc                   	int3
   18027f4af:	cc                   	int3
