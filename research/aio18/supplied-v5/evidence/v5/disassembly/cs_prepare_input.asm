
/mnt/data/aio18_work/extracted/SKSE/Plugins/SkyrimUpscaler.dll:     file format pei-x86-64


Disassembly of section .text:

0000000180265480 <.text+0x264480>:
   180265480:	00 00                	add    BYTE PTR [rax],al
   180265482:	90                   	nop
   180265483:	48 8d 4d d0          	lea    rcx,[rbp-0x30]
   180265487:	e8 04 07 00 00       	call   0x180265b90
   18026548c:	48 8b 9c 24 88 01 00 	mov    rbx,QWORD PTR [rsp+0x188]
   180265493:	00 
   180265494:	48 81 c4 30 01 00 00 	add    rsp,0x130
   18026549b:	41 5f                	pop    r15
   18026549d:	41 5e                	pop    r14
   18026549f:	41 5d                	pop    r13
   1802654a1:	41 5c                	pop    r12
   1802654a3:	5f                   	pop    rdi
   1802654a4:	5e                   	pop    rsi
   1802654a5:	5d                   	pop    rbp
   1802654a6:	c3                   	ret
   1802654a7:	cc                   	int3
   1802654a8:	cc                   	int3
   1802654a9:	cc                   	int3
   1802654aa:	cc                   	int3
   1802654ab:	cc                   	int3
   1802654ac:	cc                   	int3
   1802654ad:	cc                   	int3
   1802654ae:	cc                   	int3
   1802654af:	cc                   	int3
   1802654b0:	48 89 5c 24 18       	mov    QWORD PTR [rsp+0x18],rbx
   1802654b5:	55                   	push   rbp
   1802654b6:	56                   	push   rsi
   1802654b7:	57                   	push   rdi
   1802654b8:	41 54                	push   r12
   1802654ba:	41 55                	push   r13
   1802654bc:	41 56                	push   r14
   1802654be:	41 57                	push   r15
   1802654c0:	48 8d ac 24 60 ff ff 	lea    rbp,[rsp-0xa0]
   1802654c7:	ff 
   1802654c8:	48 81 ec a0 01 00 00 	sub    rsp,0x1a0
   1802654cf:	48 8b d9             	mov    rbx,rcx
   1802654d2:	80 b9 43 03 00 00 00 	cmp    BYTE PTR [rcx+0x343],0x0
   1802654d9:	0f 84 8b 06 00 00    	je     0x180265b6a
   1802654df:	80 b9 54 03 00 00 00 	cmp    BYTE PTR [rcx+0x354],0x0
   1802654e6:	0f 84 7e 06 00 00    	je     0x180265b6a
   1802654ec:	48 83 b9 80 16 00 00 	cmp    QWORD PTR [rcx+0x1680],0x0
   1802654f3:	00 
   1802654f4:	0f 84 70 06 00 00    	je     0x180265b6a
   1802654fa:	48 83 b9 78 16 00 00 	cmp    QWORD PTR [rcx+0x1678],0x0
   180265501:	00 
   180265502:	0f 84 62 06 00 00    	je     0x180265b6a
   180265508:	e8 f3 d9 ee ff       	call   0x180152f00
   18026550d:	48 8b c8             	mov    rcx,rax
   180265510:	e8 bb 98 ff ff       	call   0x18025edd0
   180265515:	84 c0                	test   al,al
   180265517:	0f 85 4d 06 00 00    	jne    0x180265b6a
   18026551d:	8b 83 60 04 00 00    	mov    eax,DWORD PTR [rbx+0x460]
   180265523:	3b 05 b7 bb 1f 00    	cmp    eax,DWORD PTR [rip+0x1fbbb7]        # 0x1804610e0
   180265529:	75 0e                	jne    0x180265539
   18026552b:	80 3d 12 08 21 00 00 	cmp    BYTE PTR [rip+0x210812],0x0        # 0x180475d44
   180265532:	74 05                	je     0x180265539
   180265534:	41 b6 01             	mov    r14b,0x1
   180265537:	eb 03                	jmp    0x18026553c
   180265539:	45 32 f6             	xor    r14b,r14b
   18026553c:	39 83 58 03 00 00    	cmp    DWORD PTR [rbx+0x358],eax
   180265542:	75 0e                	jne    0x180265552
   180265544:	39 83 5c 03 00 00    	cmp    DWORD PTR [rbx+0x35c],eax
   18026554a:	0f 94 c0             	sete   al
   18026554d:	e9 1a 06 00 00       	jmp    0x180265b6c
   180265552:	45 84 f6             	test   r14b,r14b
   180265555:	75 35                	jne    0x18026558c
   180265557:	44 38 b3 11 12 00 00 	cmp    BYTE PTR [rbx+0x1211],r14b
   18026555e:	75 2c                	jne    0x18026558c
   180265560:	44 38 b3 a5 04 00 00 	cmp    BYTE PTR [rbx+0x4a5],r14b
   180265567:	74 09                	je     0x180265572
   180265569:	44 38 b3 ad 04 00 00 	cmp    BYTE PTR [rbx+0x4ad],r14b
   180265570:	75 1a                	jne    0x18026558c
   180265572:	80 bb a8 02 00 00 00 	cmp    BYTE PTR [rbx+0x2a8],0x0
   180265579:	0f 84 eb 05 00 00    	je     0x180265b6a
   18026557f:	80 bb aa 02 00 00 00 	cmp    BYTE PTR [rbx+0x2aa],0x0
   180265586:	0f 84 de 05 00 00    	je     0x180265b6a
   18026558c:	e8 6f ae f4 ff       	call   0x1801b0400
   180265591:	48 8b f8             	mov    rdi,rax
   180265594:	48 85 c0             	test   rax,rax
   180265597:	0f 84 cd 05 00 00    	je     0x180265b6a
   18026559d:	e8 7e 03 ed ff       	call   0x180135920
   1802655a2:	90                   	nop
   1802655a3:	e8 78 03 ed ff       	call   0x180135920
   1802655a8:	0f b6 90 18 01 00 00 	movzx  edx,BYTE PTR [rax+0x118]
   1802655af:	83 ea 01             	sub    edx,0x1
   1802655b2:	74 0a                	je     0x1802655be
   1802655b4:	83 fa 03             	cmp    edx,0x3
   1802655b7:	b8 90 0a 00 00       	mov    eax,0xa90
   1802655bc:	74 05                	je     0x1802655c3
   1802655be:	b8 88 0a 00 00       	mov    eax,0xa88
   1802655c3:	4c 8d 3c 38          	lea    r15,[rax+rdi*1]
   1802655c7:	49 8b 37             	mov    rsi,QWORD PTR [r15]
   1802655ca:	48 85 f6             	test   rsi,rsi
   1802655cd:	0f 84 97 05 00 00    	je     0x180265b6a
   1802655d3:	49 83 7f 10 00       	cmp    QWORD PTR [r15+0x10],0x0
   1802655d8:	0f 84 8c 05 00 00    	je     0x180265b6a
   1802655de:	83 bb 4c 03 00 00 00 	cmp    DWORD PTR [rbx+0x34c],0x0
   1802655e5:	0f 84 7f 05 00 00    	je     0x180265b6a
   1802655eb:	83 bb 50 03 00 00 00 	cmp    DWORD PTR [rbx+0x350],0x0
   1802655f2:	0f 84 72 05 00 00    	je     0x180265b6a
   1802655f8:	c5 f9 ef c0          	vpxor  xmm0,xmm0,xmm0
   1802655fc:	33 c0                	xor    eax,eax
   1802655fe:	c5 fc 11 45 b8       	vmovups YMMWORD PTR [rbp-0x48],ymm0
   180265603:	48 89 45 d8          	mov    QWORD PTR [rbp-0x28],rax
   180265607:	89 45 e0             	mov    DWORD PTR [rbp-0x20],eax
   18026560a:	48 8b 06             	mov    rax,QWORD PTR [rsi]
   18026560d:	48 8d 55 b8          	lea    rdx,[rbp-0x48]
   180265611:	48 8b ce             	mov    rcx,rsi
   180265614:	c5 f8 77             	vzeroupper
   180265617:	ff 50 50             	call   QWORD PTR [rax+0x50]
   18026561a:	83 7d cc 01          	cmp    DWORD PTR [rbp-0x34],0x1
   18026561e:	0f 85 46 05 00 00    	jne    0x180265b6a
   180265624:	83 bb 44 03 00 00 00 	cmp    DWORD PTR [rbx+0x344],0x0
   18026562b:	0f 85 39 05 00 00    	jne    0x180265b6a
   180265631:	83 bb 48 03 00 00 00 	cmp    DWORD PTR [rbx+0x348],0x0
   180265638:	0f 85 2c 05 00 00    	jne    0x180265b6a
   18026563e:	8b 45 b8             	mov    eax,DWORD PTR [rbp-0x48]
   180265641:	39 83 4c 03 00 00    	cmp    DWORD PTR [rbx+0x34c],eax
   180265647:	0f 87 1d 05 00 00    	ja     0x180265b6a
   18026564d:	8b 45 bc             	mov    eax,DWORD PTR [rbp-0x44]
   180265650:	39 83 50 03 00 00    	cmp    DWORD PTR [rbx+0x350],eax
   180265656:	0f 87 0e 05 00 00    	ja     0x180265b6a
   18026565c:	48 8b d3             	mov    rdx,rbx
   18026565f:	48 8d 4d 40          	lea    rcx,[rbp+0x40]
   180265663:	e8 f8 05 00 00       	call   0x180265c60
   180265668:	90                   	nop
   180265669:	80 bd 98 00 00 00 00 	cmp    BYTE PTR [rbp+0x98],0x0
   180265670:	0f 84 e1 04 00 00    	je     0x180265b57
   180265676:	c5 fc 10 45 b8       	vmovups ymm0,YMMWORD PTR [rbp-0x48]
   18026567b:	c5 fc 11 45 00       	vmovups YMMWORD PTR [rbp+0x0],ymm0
   180265680:	c5 fb 10 4d d8       	vmovsd xmm1,QWORD PTR [rbp-0x28]
   180265685:	c5 fb 11 8d e8 00 00 	vmovsd QWORD PTR [rbp+0xe8],xmm1
   18026568c:	00 
   18026568d:	c5 fb 11 4d 20       	vmovsd QWORD PTR [rbp+0x20],xmm1
   180265692:	8b 45 e0             	mov    eax,DWORD PTR [rbp-0x20]
   180265695:	89 85 e0 00 00 00    	mov    DWORD PTR [rbp+0xe0],eax
   18026569b:	89 45 28             	mov    DWORD PTR [rbp+0x28],eax
   18026569e:	44 8b ab 4c 03 00 00 	mov    r13d,DWORD PTR [rbx+0x34c]
   1802656a5:	44 89 6d 00          	mov    DWORD PTR [rbp+0x0],r13d
   1802656a9:	44 8b a3 50 03 00 00 	mov    r12d,DWORD PTR [rbx+0x350]
   1802656b0:	44 89 65 04          	mov    DWORD PTR [rbp+0x4],r12d
   1802656b4:	c5 fc 10 45 00       	vmovups ymm0,YMMWORD PTR [rbp+0x0]
   1802656b9:	c5 fc 11 44 24 70    	vmovups YMMWORD PTR [rsp+0x70],ymm0
   1802656bf:	c5 fb 11 4d 90       	vmovsd QWORD PTR [rbp-0x70],xmm1
   1802656c4:	89 45 98             	mov    DWORD PTR [rbp-0x68],eax
   1802656c7:	48 8d 93 f8 0a 00 00 	lea    rdx,[rbx+0xaf8]
   1802656ce:	41 b9 28 00 00 00    	mov    r9d,0x28
   1802656d4:	4c 8d 44 24 70       	lea    r8,[rsp+0x70]
   1802656d9:	48 8b 8b 78 16 00 00 	mov    rcx,QWORD PTR [rbx+0x1678]
   1802656e0:	c5 f8 77             	vzeroupper
   1802656e3:	e8 18 38 00 00       	call   0x180268f00
   1802656e8:	84 c0                	test   al,al
   1802656ea:	0f 84 67 04 00 00    	je     0x180265b57
   1802656f0:	b2 01                	mov    dl,0x1
   1802656f2:	48 8b cb             	mov    rcx,rbx
   1802656f5:	e8 b6 be 03 00       	call   0x1802a15b0
   1802656fa:	84 c0                	test   al,al
   1802656fc:	0f 84 55 04 00 00    	je     0x180265b57
   180265702:	48 8b 8b 50 0b 00 00 	mov    rcx,QWORD PTR [rbx+0xb50]
   180265709:	48 85 c9             	test   rcx,rcx
   18026570c:	0f 84 45 04 00 00    	je     0x180265b57
   180265712:	48 83 bb a8 0b 00 00 	cmp    QWORD PTR [rbx+0xba8],0x0
   180265719:	00 
   18026571a:	0f 84 37 04 00 00    	je     0x180265b57
   180265720:	48 8d bb 78 0b 00 00 	lea    rdi,[rbx+0xb78]
   180265727:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   18026572a:	48 8b d7             	mov    rdx,rdi
   18026572d:	ff 50 50             	call   QWORD PTR [rax+0x50]
   180265730:	c5 fb 10 47 20       	vmovsd xmm0,QWORD PTR [rdi+0x20]
   180265735:	c5 fb 11 45 90       	vmovsd QWORD PTR [rbp-0x70],xmm0
   18026573a:	8b 47 28             	mov    eax,DWORD PTR [rdi+0x28]
   18026573d:	89 45 98             	mov    DWORD PTR [rbp-0x68],eax
   180265740:	44 39 2f             	cmp    DWORD PTR [rdi],r13d
   180265743:	0f 85 0e 04 00 00    	jne    0x180265b57
   180265749:	48 8b 8b 50 0b 00 00 	mov    rcx,QWORD PTR [rbx+0xb50]
   180265750:	48 85 c9             	test   rcx,rcx
   180265753:	74 09                	je     0x18026575e
   180265755:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   180265758:	48 8b d7             	mov    rdx,rdi
   18026575b:	ff 50 50             	call   QWORD PTR [rax+0x50]
   18026575e:	c5 fb 10 47 20       	vmovsd xmm0,QWORD PTR [rdi+0x20]
   180265763:	c5 fb 11 45 90       	vmovsd QWORD PTR [rbp-0x70],xmm0
   180265768:	8b 47 28             	mov    eax,DWORD PTR [rdi+0x28]
   18026576b:	89 45 98             	mov    DWORD PTR [rbp-0x68],eax
   18026576e:	48 8b 07             	mov    rax,QWORD PTR [rdi]
   180265771:	48 c1 e8 20          	shr    rax,0x20
   180265775:	41 3b c4             	cmp    eax,r12d
   180265778:	0f 85 d9 03 00 00    	jne    0x180265b57
   18026577e:	48 8b 8b a8 0b 00 00 	mov    rcx,QWORD PTR [rbx+0xba8]
   180265785:	48 85 c9             	test   rcx,rcx
   180265788:	74 0d                	je     0x180265797
   18026578a:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   18026578d:	48 8d 93 d0 0b 00 00 	lea    rdx,[rbx+0xbd0]
   180265794:	ff 50 50             	call   QWORD PTR [rax+0x50]
   180265797:	c5 fb 10 83 f0 0b 00 	vmovsd xmm0,QWORD PTR [rbx+0xbf0]
   18026579e:	00 
   18026579f:	c5 fb 11 45 90       	vmovsd QWORD PTR [rbp-0x70],xmm0
   1802657a4:	8b 83 f8 0b 00 00    	mov    eax,DWORD PTR [rbx+0xbf8]
   1802657aa:	89 45 98             	mov    DWORD PTR [rbp-0x68],eax
   1802657ad:	44 39 ab d0 0b 00 00 	cmp    DWORD PTR [rbx+0xbd0],r13d
   1802657b4:	0f 85 9d 03 00 00    	jne    0x180265b57
   1802657ba:	48 8b 8b a8 0b 00 00 	mov    rcx,QWORD PTR [rbx+0xba8]
   1802657c1:	48 85 c9             	test   rcx,rcx
   1802657c4:	74 0d                	je     0x1802657d3
   1802657c6:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   1802657c9:	48 8d 93 d0 0b 00 00 	lea    rdx,[rbx+0xbd0]
   1802657d0:	ff 50 50             	call   QWORD PTR [rax+0x50]
   1802657d3:	c5 fb 10 83 f0 0b 00 	vmovsd xmm0,QWORD PTR [rbx+0xbf0]
   1802657da:	00 
   1802657db:	c5 fb 11 45 90       	vmovsd QWORD PTR [rbp-0x70],xmm0
   1802657e0:	8b 83 f8 0b 00 00    	mov    eax,DWORD PTR [rbx+0xbf8]
   1802657e6:	89 45 98             	mov    DWORD PTR [rbp-0x68],eax
   1802657e9:	48 8b 83 d0 0b 00 00 	mov    rax,QWORD PTR [rbx+0xbd0]
   1802657f0:	48 c1 e8 20          	shr    rax,0x20
   1802657f4:	41 3b c4             	cmp    eax,r12d
   1802657f7:	0f 85 5a 03 00 00    	jne    0x180265b57
   1802657fd:	33 c0                	xor    eax,eax
   1802657ff:	48 89 45 a0          	mov    QWORD PTR [rbp-0x60],rax
   180265803:	89 45 a8             	mov    DWORD PTR [rbp-0x58],eax
   180265806:	44 89 6d ac          	mov    DWORD PTR [rbp-0x54],r13d
   18026580a:	44 89 65 b0          	mov    DWORD PTR [rbp-0x50],r12d
   18026580e:	c7 45 b4 01 00 00 00 	mov    DWORD PTR [rbp-0x4c],0x1
   180265815:	8b 83 60 04 00 00    	mov    eax,DWORD PTR [rbx+0x460]
   18026581b:	89 83 58 03 00 00    	mov    DWORD PTR [rbx+0x358],eax
   180265821:	48 8b cb             	mov    rcx,rbx
   180265824:	e8 47 4f 04 00       	call   0x1802aa770
   180265829:	0f b6 f8             	movzx  edi,al
   18026582c:	45 84 f6             	test   r14b,r14b
   18026582f:	0f 84 c4 00 00 00    	je     0x1802658f9
   180265835:	48 8b 8b 80 16 00 00 	mov    rcx,QWORD PTR [rbx+0x1680]
   18026583c:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   18026583f:	48 8d 55 a0          	lea    rdx,[rbp-0x60]
   180265843:	48 89 54 24 40       	mov    QWORD PTR [rsp+0x40],rdx
   180265848:	33 ff                	xor    edi,edi
   18026584a:	89 7c 24 38          	mov    DWORD PTR [rsp+0x38],edi
   18026584e:	48 89 74 24 30       	mov    QWORD PTR [rsp+0x30],rsi
   180265853:	89 7c 24 28          	mov    DWORD PTR [rsp+0x28],edi
   180265857:	89 7c 24 20          	mov    DWORD PTR [rsp+0x20],edi
   18026585b:	45 33 c9             	xor    r9d,r9d
   18026585e:	45 33 c0             	xor    r8d,r8d
   180265861:	48 8b 93 f8 0a 00 00 	mov    rdx,QWORD PTR [rbx+0xaf8]
   180265868:	ff 90 70 01 00 00    	call   QWORD PTR [rax+0x170]
   18026586e:	48 8d 8b f8 0a 00 00 	lea    rcx,[rbx+0xaf8]
   180265875:	e8 16 6a f0 ff       	call   0x18016c290
   18026587a:	48 89 85 e0 00 00 00 	mov    QWORD PTR [rbp+0xe0],rax
   180265881:	48 85 c0             	test   rax,rax
   180265884:	0f 84 cd 02 00 00    	je     0x180265b57
   18026588a:	48 89 7c 24 68       	mov    QWORD PTR [rsp+0x68],rdi
   18026588f:	40 88 7c 24 60       	mov    BYTE PTR [rsp+0x60],dil
   180265894:	89 7c 24 58          	mov    DWORD PTR [rsp+0x58],edi
   180265898:	89 7c 24 50          	mov    DWORD PTR [rsp+0x50],edi
   18026589c:	8b 45 bc             	mov    eax,DWORD PTR [rbp-0x44]
   18026589f:	89 44 24 48          	mov    DWORD PTR [rsp+0x48],eax
   1802658a3:	8b 45 b8             	mov    eax,DWORD PTR [rbp-0x48]
   1802658a6:	89 44 24 40          	mov    DWORD PTR [rsp+0x40],eax
   1802658aa:	49 8b 47 10          	mov    rax,QWORD PTR [r15+0x10]
   1802658ae:	48 89 44 24 38       	mov    QWORD PTR [rsp+0x38],rax
   1802658b3:	48 89 7c 24 30       	mov    QWORD PTR [rsp+0x30],rdi
   1802658b8:	48 8d 85 e0 00 00 00 	lea    rax,[rbp+0xe0]
   1802658bf:	48 89 44 24 28       	mov    QWORD PTR [rsp+0x28],rax
   1802658c4:	c7 44 24 20 01 00 00 	mov    DWORD PTR [rsp+0x20],0x1
   1802658cb:	00 
   1802658cc:	45 33 c9             	xor    r9d,r9d
   1802658cf:	ba 01 00 00 00       	mov    edx,0x1
   1802658d4:	44 8b c2             	mov    r8d,edx
   1802658d7:	48 8b cb             	mov    rcx,rbx
   1802658da:	e8 61 b9 03 00       	call   0x1802a1240
   1802658df:	8b 83 60 04 00 00    	mov    eax,DWORD PTR [rbx+0x460]
   1802658e5:	89 83 5c 03 00 00    	mov    DWORD PTR [rbx+0x35c],eax
   1802658eb:	c6 83 11 12 00 00 01 	mov    BYTE PTR [rbx+0x1211],0x1
   1802658f2:	b3 01                	mov    bl,0x1
   1802658f4:	e9 60 02 00 00       	jmp    0x180265b59
   1802658f9:	80 bb 11 12 00 00 00 	cmp    BYTE PTR [rbx+0x1211],0x0
   180265900:	0f 84 a6 00 00 00    	je     0x1802659ac
   180265906:	c5 fc 10 45 00       	vmovups ymm0,YMMWORD PTR [rbp+0x0]
   18026590b:	c5 fc 11 44 24 70    	vmovups YMMWORD PTR [rsp+0x70],ymm0
   180265911:	c5 fb 10 8d e8 00 00 	vmovsd xmm1,QWORD PTR [rbp+0xe8]
   180265918:	00 
   180265919:	c5 fb 11 4d 90       	vmovsd QWORD PTR [rbp-0x70],xmm1
   18026591e:	8b 85 e0 00 00 00    	mov    eax,DWORD PTR [rbp+0xe0]
   180265924:	89 45 98             	mov    DWORD PTR [rbp-0x68],eax
   180265927:	48 8d 93 08 0d 00 00 	lea    rdx,[rbx+0xd08]
   18026592e:	41 b9 28 00 00 00    	mov    r9d,0x28
   180265934:	4c 8d 44 24 70       	lea    r8,[rsp+0x70]
   180265939:	48 8b 8b 78 16 00 00 	mov    rcx,QWORD PTR [rbx+0x1678]
   180265940:	c5 f8 77             	vzeroupper
   180265943:	e8 b8 35 00 00       	call   0x180268f00
   180265948:	80 bb 11 12 00 00 00 	cmp    BYTE PTR [rbx+0x1211],0x0
   18026594f:	74 5b                	je     0x1802659ac
   180265951:	40 84 ff             	test   dil,dil
   180265954:	74 56                	je     0x1802659ac
   180265956:	48 8b cb             	mov    rcx,rbx
   180265959:	e8 d2 09 00 00       	call   0x180266330
   18026595e:	45 33 f6             	xor    r14d,r14d
   180265961:	84 c0                	test   al,al
   180265963:	74 4a                	je     0x1802659af
   180265965:	48 8b 8b 80 16 00 00 	mov    rcx,QWORD PTR [rbx+0x1680]
   18026596c:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   18026596f:	4c 8b 90 70 01 00 00 	mov    r10,QWORD PTR [rax+0x170]
   180265976:	48 8d 45 a0          	lea    rax,[rbp-0x60]
   18026597a:	48 89 44 24 40       	mov    QWORD PTR [rsp+0x40],rax
   18026597f:	44 89 74 24 38       	mov    DWORD PTR [rsp+0x38],r14d
   180265984:	48 8b 83 a8 0b 00 00 	mov    rax,QWORD PTR [rbx+0xba8]
   18026598b:	48 89 44 24 30       	mov    QWORD PTR [rsp+0x30],rax
   180265990:	44 89 74 24 28       	mov    DWORD PTR [rsp+0x28],r14d
   180265995:	44 89 74 24 20       	mov    DWORD PTR [rsp+0x20],r14d
   18026599a:	45 33 c9             	xor    r9d,r9d
   18026599d:	45 33 c0             	xor    r8d,r8d
   1802659a0:	48 8b 93 28 08 00 00 	mov    rdx,QWORD PTR [rbx+0x828]
   1802659a7:	41 ff d2             	call   r10
   1802659aa:	eb 03                	jmp    0x1802659af
   1802659ac:	45 33 f6             	xor    r14d,r14d
   1802659af:	80 bb a8 02 00 00 00 	cmp    BYTE PTR [rbx+0x2a8],0x0
   1802659b6:	0f 84 9b 01 00 00    	je     0x180265b57
   1802659bc:	80 bb aa 02 00 00 00 	cmp    BYTE PTR [rbx+0x2aa],0x0
   1802659c3:	0f 84 8e 01 00 00    	je     0x180265b57
   1802659c9:	c5 fc 10 45 00       	vmovups ymm0,YMMWORD PTR [rbp+0x0]
   1802659ce:	c4 e3 7d 19 c0 01    	vextractf128 xmm0,ymm0,0x1
   1802659d4:	c5 f9 7e c7          	vmovd  edi,xmm0
   1802659d8:	39 bb a4 02 00 00    	cmp    DWORD PTR [rbx+0x2a4],edi
   1802659de:	75 1e                	jne    0x1802659fe
   1802659e0:	44 39 ab 64 03 00 00 	cmp    DWORD PTR [rbx+0x364],r13d
   1802659e7:	75 15                	jne    0x1802659fe
   1802659e9:	44 39 a3 68 03 00 00 	cmp    DWORD PTR [rbx+0x368],r12d
   1802659f0:	75 0c                	jne    0x1802659fe
   1802659f2:	48 8d bb a9 02 00 00 	lea    rdi,[rbx+0x2a9]
   1802659f9:	e9 b5 00 00 00       	jmp    0x180265ab3
   1802659fe:	89 bb a4 02 00 00    	mov    DWORD PTR [rbx+0x2a4],edi
   180265a04:	48 8b cb             	mov    rcx,rbx
   180265a07:	c5 f8 77             	vzeroupper
   180265a0a:	e8 11 c7 03 00       	call   0x1802a2120
   180265a0f:	89 bd e0 00 00 00    	mov    DWORD PTR [rbp+0xe0],edi
   180265a15:	e8 36 c6 fb ff       	call   0x180222050
   180265a1a:	48 8d 0d a7 30 18 00 	lea    rcx,[rip+0x1830a7]        # 0x1803e8ac8
   180265a21:	48 89 4d e8          	mov    QWORD PTR [rbp-0x18],rcx
   180265a25:	c7 45 f0 4b 02 00 00 	mov    DWORD PTR [rbp-0x10],0x24b
   180265a2c:	8b 4c 24 7c          	mov    ecx,DWORD PTR [rsp+0x7c]
   180265a30:	89 4d f4             	mov    DWORD PTR [rbp-0xc],ecx
   180265a33:	48 8d 0d fe 32 18 00 	lea    rcx,[rip+0x1832fe]        # 0x1803e8d38
   180265a3a:	48 89 4d f8          	mov    QWORD PTR [rbp-0x8],rcx
   180265a3e:	48 8d 0d ab 32 18 00 	lea    rcx,[rip+0x1832ab]        # 0x1803e8cf0
   180265a45:	48 89 4d 30          	mov    QWORD PTR [rbp+0x30],rcx
   180265a49:	48 c7 45 38 47 00 00 	mov    QWORD PTR [rbp+0x38],0x47
   180265a50:	00 
   180265a51:	c5 f8 10 45 e8       	vmovups xmm0,XMMWORD PTR [rbp-0x18]
   180265a56:	c5 f8 11 44 24 70    	vmovups XMMWORD PTR [rsp+0x70],xmm0
   180265a5c:	c5 fb 10 4d f8       	vmovsd xmm1,QWORD PTR [rbp-0x8]
   180265a61:	c5 fb 11 4d 80       	vmovsd QWORD PTR [rbp-0x80],xmm1
   180265a66:	48 8d bb a9 02 00 00 	lea    rdi,[rbx+0x2a9]
   180265a6d:	48 89 7c 24 48       	mov    QWORD PTR [rsp+0x48],rdi
   180265a72:	48 8d 8d e0 00 00 00 	lea    rcx,[rbp+0xe0]
   180265a79:	48 89 4c 24 40       	mov    QWORD PTR [rsp+0x40],rcx
   180265a7e:	48 8d 4d 04          	lea    rcx,[rbp+0x4]
   180265a82:	48 89 4c 24 38       	mov    QWORD PTR [rsp+0x38],rcx
   180265a87:	48 8d 4d 00          	lea    rcx,[rbp+0x0]
   180265a8b:	48 89 4c 24 30       	mov    QWORD PTR [rsp+0x30],rcx
   180265a90:	48 8d 4d bc          	lea    rcx,[rbp-0x44]
   180265a94:	48 89 4c 24 28       	mov    QWORD PTR [rsp+0x28],rcx
   180265a99:	48 8d 4d b8          	lea    rcx,[rbp-0x48]
   180265a9d:	48 89 4c 24 20       	mov    QWORD PTR [rsp+0x20],rcx
   180265aa2:	4c 8d 4d 30          	lea    r9,[rbp+0x30]
   180265aa6:	48 8d 54 24 70       	lea    rdx,[rsp+0x70]
   180265aab:	48 8b c8             	mov    rcx,rax
   180265aae:	e8 bd 75 f0 ff       	call   0x18016d070
   180265ab3:	80 3f 00             	cmp    BYTE PTR [rdi],0x0
   180265ab6:	0f 84 9b 00 00 00    	je     0x180265b57
   180265abc:	48 8b 8b 80 16 00 00 	mov    rcx,QWORD PTR [rbx+0x1680]
   180265ac3:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   180265ac6:	48 8d 55 a0          	lea    rdx,[rbp-0x60]
   180265aca:	48 89 54 24 40       	mov    QWORD PTR [rsp+0x40],rdx
   180265acf:	44 89 74 24 38       	mov    DWORD PTR [rsp+0x38],r14d
   180265ad4:	48 89 74 24 30       	mov    QWORD PTR [rsp+0x30],rsi
   180265ad9:	44 89 74 24 28       	mov    DWORD PTR [rsp+0x28],r14d
   180265ade:	44 89 74 24 20       	mov    DWORD PTR [rsp+0x20],r14d
   180265ae3:	45 33 c9             	xor    r9d,r9d
   180265ae6:	45 33 c0             	xor    r8d,r8d
   180265ae9:	48 8b 93 f8 0a 00 00 	mov    rdx,QWORD PTR [rbx+0xaf8]
   180265af0:	c5 f8 77             	vzeroupper
   180265af3:	ff 90 70 01 00 00    	call   QWORD PTR [rax+0x170]
   180265af9:	41 b1 01             	mov    r9b,0x1
   180265afc:	45 0f b6 c1          	movzx  r8d,r9b
   180265b00:	48 8d 93 f8 0a 00 00 	lea    rdx,[rbx+0xaf8]
   180265b07:	48 8b cb             	mov    rcx,rbx
   180265b0a:	e8 f1 c7 03 00       	call   0x1802a2300
   180265b0f:	48 8b 8b 80 16 00 00 	mov    rcx,QWORD PTR [rbx+0x1680]
   180265b16:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   180265b19:	4c 8b 90 70 01 00 00 	mov    r10,QWORD PTR [rax+0x170]
   180265b20:	48 8d 45 a0          	lea    rax,[rbp-0x60]
   180265b24:	48 89 44 24 40       	mov    QWORD PTR [rsp+0x40],rax
   180265b29:	44 89 74 24 38       	mov    DWORD PTR [rsp+0x38],r14d
   180265b2e:	48 8b 83 f8 0a 00 00 	mov    rax,QWORD PTR [rbx+0xaf8]
   180265b35:	48 89 44 24 30       	mov    QWORD PTR [rsp+0x30],rax
   180265b3a:	44 89 74 24 28       	mov    DWORD PTR [rsp+0x28],r14d
   180265b3f:	44 89 74 24 20       	mov    DWORD PTR [rsp+0x20],r14d
   180265b44:	45 33 c9             	xor    r9d,r9d
   180265b47:	45 33 c0             	xor    r8d,r8d
   180265b4a:	48 8b d6             	mov    rdx,rsi
   180265b4d:	41 ff d2             	call   r10
   180265b50:	c6 83 8c 04 00 00 01 	mov    BYTE PTR [rbx+0x48c],0x1
   180265b57:	32 db                	xor    bl,bl
   180265b59:	48 8d 4d 40          	lea    rcx,[rbp+0x40]
   180265b5d:	c5 f8 77             	vzeroupper
   180265b60:	e8 2b 00 00 00       	call   0x180265b90
   180265b65:	0f b6 c3             	movzx  eax,bl
   180265b68:	eb 02                	jmp    0x180265b6c
   180265b6a:	32 c0                	xor    al,al
   180265b6c:	48 8b 9c 24 f0 01 00 	mov    rbx,QWORD PTR [rsp+0x1f0]
   180265b73:	00 
   180265b74:	48 81 c4 a0 01 00 00 	add    rsp,0x1a0
   180265b7b:	41 5f                	pop    r15
   180265b7d:	41 5e                	pop    r14
   180265b7f:	41 5d                	pop    r13
   180265b81:	41 5c                	pop    r12
   180265b83:	5f                   	pop    rdi
   180265b84:	5e                   	pop    rsi
   180265b85:	5d                   	pop    rbp
   180265b86:	c3                   	ret
   180265b87:	cc                   	int3
   180265b88:	cc                   	int3
   180265b89:	cc                   	int3
   180265b8a:	cc                   	int3
   180265b8b:	cc                   	int3
   180265b8c:	cc                   	int3
   180265b8d:	cc                   	int3
   180265b8e:	cc                   	int3
   180265b8f:	cc                   	int3
   180265b90:	48 89 5c 24 08       	mov    QWORD PTR [rsp+0x8],rbx
   180265b95:	57                   	push   rdi
   180265b96:	48 83 ec 40          	sub    rsp,0x40
   180265b9a:	48 8b d9             	mov    rbx,rcx
   180265b9d:	4c 8d 41 18          	lea    r8,[rcx+0x18]
   180265ba1:	48 8b 41 10          	mov    rax,QWORD PTR [rcx+0x10]
   180265ba5:	c4 c1 7c 10 00       	vmovups ymm0,YMMWORD PTR [r8]
   180265baa:	c5 fc 11 80 10 16 00 	vmovups YMMWORD PTR [rax+0x1610],ymm0
   180265bb1:	00 
   180265bb2:	c4 c1 7c 10 48 20    	vmovups ymm1,YMMWORD PTR [r8+0x20]
   180265bb8:	c5 fc 11 88 30 16 00 	vmovups YMMWORD PTR [rax+0x1630],ymm1
   180265bbf:	00 
   180265bc0:	33 ff                	xor    edi,edi
   180265bc2:	40 38 79 58          	cmp    BYTE PTR [rcx+0x58],dil
   180265bc6:	74 49                	je     0x180265c11
   180265bc8:	48 8b 41 10          	mov    rax,QWORD PTR [rcx+0x10]
   180265bcc:	48 8b 90 08 16 00 00 	mov    rdx,QWORD PTR [rax+0x1608]
   180265bd3:	48 85 d2             	test   rdx,rdx
   180265bd6:	74 22                	je     0x180265bfa
   180265bd8:	48 8b 09             	mov    rcx,QWORD PTR [rcx]
   180265bdb:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   180265bde:	89 7c 24 30          	mov    DWORD PTR [rsp+0x30],edi
   180265be2:	89 7c 24 28          	mov    DWORD PTR [rsp+0x28],edi
   180265be6:	4c 89 44 24 20       	mov    QWORD PTR [rsp+0x20],r8
   180265beb:	45 33 c9             	xor    r9d,r9d
   180265bee:	45 33 c0             	xor    r8d,r8d
   180265bf1:	c5 f8 77             	vzeroupper
   180265bf4:	ff 90 80 01 00 00    	call   QWORD PTR [rax+0x180]
   180265bfa:	48 8b 0b             	mov    rcx,QWORD PTR [rbx]
   180265bfd:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   180265c00:	45 33 c0             	xor    r8d,r8d
   180265c03:	48 8b 53 08          	mov    rdx,QWORD PTR [rbx+0x8]
   180265c07:	c5 f8 77             	vzeroupper
   180265c0a:	ff 90 18 04 00 00    	call   QWORD PTR [rax+0x418]
   180265c10:	90                   	nop
   180265c11:	48 8b 4b 08          	mov    rcx,QWORD PTR [rbx+0x8]
   180265c15:	48 85 c9             	test   rcx,rcx
   180265c18:	74 0e                	je     0x180265c28
   180265c1a:	48 89 7b 08          	mov    QWORD PTR [rbx+0x8],rdi
   180265c1e:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   180265c21:	c5 f8 77             	vzeroupper
   180265c24:	ff 50 10             	call   QWORD PTR [rax+0x10]
   180265c27:	90                   	nop
   180265c28:	48 8b 0b             	mov    rcx,QWORD PTR [rbx]
   180265c2b:	48 85 c9             	test   rcx,rcx
   180265c2e:	74 0d                	je     0x180265c3d
   180265c30:	48 89 3b             	mov    QWORD PTR [rbx],rdi
   180265c33:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   180265c36:	c5 f8 77             	vzeroupper
   180265c39:	ff 50 10             	call   QWORD PTR [rax+0x10]
   180265c3c:	90                   	nop
   180265c3d:	c5 f8 77             	vzeroupper
   180265c40:	48 8b 5c 24 50       	mov    rbx,QWORD PTR [rsp+0x50]
   180265c45:	48 83 c4 40          	add    rsp,0x40
   180265c49:	5f                   	pop    rdi
   180265c4a:	c3                   	ret
   180265c4b:	cc                   	int3
   180265c4c:	cc                   	int3
   180265c4d:	cc                   	int3
   180265c4e:	cc                   	int3
   180265c4f:	cc                   	int3
   180265c50:	cc                   	int3
   180265c51:	cc                   	int3
   180265c52:	cc                   	int3
   180265c53:	cc                   	int3
   180265c54:	cc                   	int3
   180265c55:	cc                   	int3
   180265c56:	cc                   	int3
   180265c57:	cc                   	int3
   180265c58:	cc                   	int3
   180265c59:	cc                   	int3
   180265c5a:	cc                   	int3
   180265c5b:	cc                   	int3
   180265c5c:	cc                   	int3
   180265c5d:	cc                   	int3
   180265c5e:	cc                   	int3
   180265c5f:	cc                   	int3
