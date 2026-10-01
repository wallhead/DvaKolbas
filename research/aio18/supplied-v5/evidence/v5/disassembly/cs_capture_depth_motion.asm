
/mnt/data/aio18_work/extracted/SKSE/Plugins/SkyrimUpscaler.dll:     file format pei-x86-64


Disassembly of section .text:

00000001802a15b0 <.text+0x2a05b0>:
   1802a15b0:	48 89 5c 24 10       	mov    QWORD PTR [rsp+0x10],rbx
   1802a15b5:	55                   	push   rbp
   1802a15b6:	56                   	push   rsi
   1802a15b7:	57                   	push   rdi
   1802a15b8:	41 54                	push   r12
   1802a15ba:	41 55                	push   r13
   1802a15bc:	41 56                	push   r14
   1802a15be:	41 57                	push   r15
   1802a15c0:	48 8d ac 24 f0 fc ff 	lea    rbp,[rsp-0x310]
   1802a15c7:	ff 
   1802a15c8:	48 81 ec 10 04 00 00 	sub    rsp,0x410
   1802a15cf:	48 83 b9 80 16 00 00 	cmp    QWORD PTR [rcx+0x1680],0x0
   1802a15d6:	00 
   1802a15d7:	0f b6 f2             	movzx  esi,dl
   1802a15da:	48 8b f9             	mov    rdi,rcx
   1802a15dd:	0f 84 0c 0b 00 00    	je     0x1802a20ef
   1802a15e3:	48 83 b9 78 16 00 00 	cmp    QWORD PTR [rcx+0x1678],0x0
   1802a15ea:	00 
   1802a15eb:	0f 84 fe 0a 00 00    	je     0x1802a20ef
   1802a15f1:	e8 0a df ee ff       	call   0x18018f500
   1802a15f6:	48 85 c0             	test   rax,rax
   1802a15f9:	0f 84 f0 0a 00 00    	je     0x1802a20ef
   1802a15ff:	48 83 bf 28 08 00 00 	cmp    QWORD PTR [rdi+0x828],0x0
   1802a1606:	00 
   1802a1607:	0f 84 e2 0a 00 00    	je     0x1802a20ef
   1802a160d:	b9 01 00 00 00       	mov    ecx,0x1
   1802a1612:	e8 f9 2d 00 00       	call   0x1802a4410
   1802a1617:	33 c9                	xor    ecx,ecx
   1802a1619:	48 8b d8             	mov    rbx,rax
   1802a161c:	e8 ef 2d 00 00       	call   0x1802a4410
   1802a1621:	45 33 ed             	xor    r13d,r13d
   1802a1624:	40 84 f6             	test   sil,sil
   1802a1627:	74 12                	je     0x1802a163b
   1802a1629:	48 85 c0             	test   rax,rax
   1802a162c:	74 0d                	je     0x1802a163b
   1802a162e:	48 8b 30             	mov    rsi,QWORD PTR [rax]
   1802a1631:	48 85 f6             	test   rsi,rsi
   1802a1634:	74 05                	je     0x1802a163b
   1802a1636:	4c 8b e8             	mov    r13,rax
   1802a1639:	eb 34                	jmp    0x1802a166f
   1802a163b:	48 85 db             	test   rbx,rbx
   1802a163e:	74 0d                	je     0x1802a164d
   1802a1640:	48 8b 33             	mov    rsi,QWORD PTR [rbx]
   1802a1643:	48 85 f6             	test   rsi,rsi
   1802a1646:	74 05                	je     0x1802a164d
   1802a1648:	4c 8b eb             	mov    r13,rbx
   1802a164b:	eb 22                	jmp    0x1802a166f
   1802a164d:	48 85 c0             	test   rax,rax
   1802a1650:	74 0d                	je     0x1802a165f
   1802a1652:	48 8b 30             	mov    rsi,QWORD PTR [rax]
   1802a1655:	48 85 f6             	test   rsi,rsi
   1802a1658:	74 05                	je     0x1802a165f
   1802a165a:	4c 8b e8             	mov    r13,rax
   1802a165d:	eb 10                	jmp    0x1802a166f
   1802a165f:	48 8b b7 d0 07 00 00 	mov    rsi,QWORD PTR [rdi+0x7d0]
   1802a1666:	48 85 f6             	test   rsi,rsi
   1802a1669:	0f 84 80 0a 00 00    	je     0x1802a20ef
   1802a166f:	44 8b bf 4c 03 00 00 	mov    r15d,DWORD PTR [rdi+0x34c]
   1802a1676:	44 8b b7 50 03 00 00 	mov    r14d,DWORD PTR [rdi+0x350]
   1802a167d:	45 85 ff             	test   r15d,r15d
   1802a1680:	74 05                	je     0x1802a1687
   1802a1682:	45 85 f6             	test   r14d,r14d
   1802a1685:	75 30                	jne    0x1802a16b7
   1802a1687:	8b 87 70 02 00 00    	mov    eax,DWORD PTR [rdi+0x270]
   1802a168d:	45 33 ff             	xor    r15d,r15d
   1802a1690:	85 c0                	test   eax,eax
   1802a1692:	44 0f 4f f8          	cmovg  r15d,eax
   1802a1696:	8b 87 74 02 00 00    	mov    eax,DWORD PTR [rdi+0x274]
   1802a169c:	45 33 f6             	xor    r14d,r14d
   1802a169f:	85 c0                	test   eax,eax
   1802a16a1:	44 0f 4f f0          	cmovg  r14d,eax
   1802a16a5:	45 85 ff             	test   r15d,r15d
   1802a16a8:	0f 84 41 0a 00 00    	je     0x1802a20ef
   1802a16ae:	45 85 f6             	test   r14d,r14d
   1802a16b1:	0f 84 38 0a 00 00    	je     0x1802a20ef
   1802a16b7:	33 c0                	xor    eax,eax
   1802a16b9:	48 8d 55 28          	lea    rdx,[rbp+0x28]
   1802a16bd:	c5 f9 ef c0          	vpxor  xmm0,xmm0,xmm0
   1802a16c1:	c5 fc 11 45 28       	vmovups YMMWORD PTR [rbp+0x28],ymm0
   1802a16c6:	c5 fc 11 45 58       	vmovups YMMWORD PTR [rbp+0x58],ymm0
   1802a16cb:	48 89 45 48          	mov    QWORD PTR [rbp+0x48],rax
   1802a16cf:	48 8b ce             	mov    rcx,rsi
   1802a16d2:	89 45 50             	mov    DWORD PTR [rbp+0x50],eax
   1802a16d5:	48 89 45 78          	mov    QWORD PTR [rbp+0x78],rax
   1802a16d9:	89 85 80 00 00 00    	mov    DWORD PTR [rbp+0x80],eax
   1802a16df:	48 8b 06             	mov    rax,QWORD PTR [rsi]
   1802a16e2:	c5 f8 77             	vzeroupper
   1802a16e5:	ff 50 50             	call   QWORD PTR [rax+0x50]
   1802a16e8:	48 8b 8f 28 08 00 00 	mov    rcx,QWORD PTR [rdi+0x828]
   1802a16ef:	48 8d 55 58          	lea    rdx,[rbp+0x58]
   1802a16f3:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   1802a16f6:	ff 50 50             	call   QWORD PTR [rax+0x50]
   1802a16f9:	44 3b 7d 28          	cmp    r15d,DWORD PTR [rbp+0x28]
   1802a16fd:	0f 87 ec 09 00 00    	ja     0x1802a20ef
   1802a1703:	44 3b 75 2c          	cmp    r14d,DWORD PTR [rbp+0x2c]
   1802a1707:	0f 87 e2 09 00 00    	ja     0x1802a20ef
   1802a170d:	44 3b 7d 58          	cmp    r15d,DWORD PTR [rbp+0x58]
   1802a1711:	0f 87 d8 09 00 00    	ja     0x1802a20ef
   1802a1717:	44 3b 75 5c          	cmp    r14d,DWORD PTR [rbp+0x5c]
   1802a171b:	0f 87 ce 09 00 00    	ja     0x1802a20ef
   1802a1721:	83 7d 3c 01          	cmp    DWORD PTR [rbp+0x3c],0x1
   1802a1725:	0f 85 c4 09 00 00    	jne    0x1802a20ef
   1802a172b:	83 7d 6c 01          	cmp    DWORD PTR [rbp+0x6c],0x1
   1802a172f:	0f 85 ba 09 00 00    	jne    0x1802a20ef
   1802a1735:	4c 8d a7 50 0b 00 00 	lea    r12,[rdi+0xb50]
   1802a173c:	49 8b 0c 24          	mov    rcx,QWORD PTR [r12]
   1802a1740:	48 85 c9             	test   rcx,rcx
   1802a1743:	74 2e                	je     0x1802a1773
   1802a1745:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   1802a1748:	48 8d 97 78 0b 00 00 	lea    rdx,[rdi+0xb78]
   1802a174f:	ff 50 50             	call   QWORD PTR [rax+0x50]
   1802a1752:	c5 fc 10 87 78 0b 00 	vmovups ymm0,YMMWORD PTR [rdi+0xb78]
   1802a1759:	00 
   1802a175a:	8b 87 a0 0b 00 00    	mov    eax,DWORD PTR [rdi+0xba0]
   1802a1760:	49 8b 0c 24          	mov    rcx,QWORD PTR [r12]
   1802a1764:	c5 fc 11 45 90       	vmovups YMMWORD PTR [rbp-0x70],ymm0
   1802a1769:	c5 fb 10 87 98 0b 00 	vmovsd xmm0,QWORD PTR [rdi+0xb98]
   1802a1770:	00 
   1802a1771:	eb 1a                	jmp    0x1802a178d
   1802a1773:	c5 f9 ef c0          	vpxor  xmm0,xmm0,xmm0
   1802a1777:	33 c0                	xor    eax,eax
   1802a1779:	c5 fc 11 45 90       	vmovups YMMWORD PTR [rbp-0x70],ymm0
   1802a177e:	48 89 85 50 01 00 00 	mov    QWORD PTR [rbp+0x150],rax
   1802a1785:	c5 fb 10 85 50 01 00 	vmovsd xmm0,QWORD PTR [rbp+0x150]
   1802a178c:	00 
   1802a178d:	89 45 e8             	mov    DWORD PTR [rbp-0x18],eax
   1802a1790:	45 8b c6             	mov    r8d,r14d
   1802a1793:	c5 fb 11 45 e0       	vmovsd QWORD PTR [rbp-0x20],xmm0
   1802a1798:	41 8b d7             	mov    edx,r15d
   1802a179b:	c5 f8 77             	vzeroupper
   1802a179e:	e8 ed 77 ff ff       	call   0x180298f90
   1802a17a3:	48 8d 1d 86 49 16 00 	lea    rbx,[rip+0x164986]        # 0x180406130
   1802a17aa:	84 c0                	test   al,al
   1802a17ac:	74 18                	je     0x1802a17c6
   1802a17ae:	c5 fc 10 45 90       	vmovups ymm0,YMMWORD PTR [rbp-0x70]
   1802a17b3:	c4 e3 7d 19 c0 01    	vextractf128 xmm0,ymm0,0x1
   1802a17b9:	c5 f9 7e c0          	vmovd  eax,xmm0
   1802a17bd:	3b 45 38             	cmp    eax,DWORD PTR [rbp+0x38]
   1802a17c0:	0f 84 db 00 00 00    	je     0x1802a18a1
   1802a17c6:	b2 01                	mov    dl,0x1
   1802a17c8:	49 8b cc             	mov    rcx,r12
   1802a17cb:	c5 f8 77             	vzeroupper
   1802a17ce:	e8 fd 14 eb ff       	call   0x180152cd0
   1802a17d3:	c5 fc 10 45 28       	vmovups ymm0,YMMWORD PTR [rbp+0x28]
   1802a17d8:	c5 fb 10 4d 48       	vmovsd xmm1,QWORD PTR [rbp+0x48]
   1802a17dd:	48 8b 8f 78 16 00 00 	mov    rcx,QWORD PTR [rdi+0x1678]
   1802a17e4:	48 8d 55 90          	lea    rdx,[rbp-0x70]
   1802a17e8:	c5 fc 11 45 90       	vmovups YMMWORD PTR [rbp-0x70],ymm0
   1802a17ed:	c5 fb 11 4d b0       	vmovsd QWORD PTR [rbp-0x50],xmm1
   1802a17f2:	33 c0                	xor    eax,eax
   1802a17f4:	44 89 7d 90          	mov    DWORD PTR [rbp-0x70],r15d
   1802a17f8:	44 89 75 94          	mov    DWORD PTR [rbp-0x6c],r14d
   1802a17fc:	4d 8b cc             	mov    r9,r12
   1802a17ff:	c7 45 98 01 00 00 00 	mov    DWORD PTR [rbp-0x68],0x1
   1802a1806:	45 33 c0             	xor    r8d,r8d
   1802a1809:	c7 45 9c 01 00 00 00 	mov    DWORD PTR [rbp-0x64],0x1
   1802a1810:	48 c7 45 a4 01 00 00 	mov    QWORD PTR [rbp-0x5c],0x1
   1802a1817:	00 
   1802a1818:	89 45 ac             	mov    DWORD PTR [rbp-0x54],eax
   1802a181b:	48 89 45 b4          	mov    QWORD PTR [rbp-0x4c],rax
   1802a181f:	c7 45 b0 48 00 00 00 	mov    DWORD PTR [rbp-0x50],0x48
   1802a1826:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   1802a1829:	c5 f8 77             	vzeroupper
   1802a182c:	ff 50 28             	call   QWORD PTR [rax+0x28]
   1802a182f:	89 85 60 03 00 00    	mov    DWORD PTR [rbp+0x360],eax
   1802a1835:	85 c0                	test   eax,eax
   1802a1837:	79 68                	jns    0x1802a18a1
   1802a1839:	e8 12 08 f8 ff       	call   0x180222050
   1802a183e:	8b 4d 9c             	mov    ecx,DWORD PTR [rbp-0x64]
   1802a1841:	4c 8d 4d 00          	lea    r9,[rbp+0x0]
   1802a1845:	89 4c 24 7c          	mov    DWORD PTR [rsp+0x7c],ecx
   1802a1849:	48 8d 55 c0          	lea    rdx,[rbp-0x40]
   1802a184d:	48 8d 0d ac 57 16 00 	lea    rcx,[rip+0x1657ac]        # 0x180407000
   1802a1854:	48 89 5c 24 70       	mov    QWORD PTR [rsp+0x70],rbx
   1802a1859:	48 89 4d 80          	mov    QWORD PTR [rbp-0x80],rcx
   1802a185d:	48 8d 0d 64 57 16 00 	lea    rcx,[rip+0x165764]        # 0x180406fc8
   1802a1864:	c5 fb 10 4d 80       	vmovsd xmm1,QWORD PTR [rbp-0x80]
   1802a1869:	48 89 4d 00          	mov    QWORD PTR [rbp+0x0],rcx
   1802a186d:	48 8d 8d 60 03 00 00 	lea    rcx,[rbp+0x360]
   1802a1874:	48 89 4c 24 20       	mov    QWORD PTR [rsp+0x20],rcx
   1802a1879:	48 8b c8             	mov    rcx,rax
   1802a187c:	c7 44 24 78 40 05 00 	mov    DWORD PTR [rsp+0x78],0x540
   1802a1883:	00 
   1802a1884:	c5 f8 10 44 24 70    	vmovups xmm0,XMMWORD PTR [rsp+0x70]
   1802a188a:	48 c7 45 08 32 00 00 	mov    QWORD PTR [rbp+0x8],0x32
   1802a1891:	00 
   1802a1892:	c5 f8 11 45 c0       	vmovups XMMWORD PTR [rbp-0x40],xmm0
   1802a1897:	c5 fb 11 4d d0       	vmovsd QWORD PTR [rbp-0x30],xmm1
   1802a189c:	e8 6f 55 ef ff       	call   0x180196e10
   1802a18a1:	48 8d 9f a8 0b 00 00 	lea    rbx,[rdi+0xba8]
   1802a18a8:	45 8b c6             	mov    r8d,r14d
   1802a18ab:	48 8b 0b             	mov    rcx,QWORD PTR [rbx]
   1802a18ae:	41 8b d7             	mov    edx,r15d
   1802a18b1:	c5 f8 77             	vzeroupper
   1802a18b4:	e8 d7 76 ff ff       	call   0x180298f90
   1802a18b9:	84 c0                	test   al,al
   1802a18bb:	0f 85 df 00 00 00    	jne    0x1802a19a0
   1802a18c1:	b2 01                	mov    dl,0x1
   1802a18c3:	48 8b cb             	mov    rcx,rbx
   1802a18c6:	e8 05 14 eb ff       	call   0x180152cd0
   1802a18cb:	c5 fc 10 45 58       	vmovups ymm0,YMMWORD PTR [rbp+0x58]
   1802a18d0:	c5 fb 10 4d 78       	vmovsd xmm1,QWORD PTR [rbp+0x78]
   1802a18d5:	48 8b 8f 78 16 00 00 	mov    rcx,QWORD PTR [rdi+0x1678]
   1802a18dc:	48 8d 55 90          	lea    rdx,[rbp-0x70]
   1802a18e0:	c5 fc 11 45 90       	vmovups YMMWORD PTR [rbp-0x70],ymm0
   1802a18e5:	c5 fb 11 4d b0       	vmovsd QWORD PTR [rbp-0x50],xmm1
   1802a18ea:	33 c0                	xor    eax,eax
   1802a18ec:	44 89 7d 90          	mov    DWORD PTR [rbp-0x70],r15d
   1802a18f0:	44 89 75 94          	mov    DWORD PTR [rbp-0x6c],r14d
   1802a18f4:	4c 8b cb             	mov    r9,rbx
   1802a18f7:	c7 45 98 01 00 00 00 	mov    DWORD PTR [rbp-0x68],0x1
   1802a18fe:	45 33 c0             	xor    r8d,r8d
   1802a1901:	c7 45 9c 01 00 00 00 	mov    DWORD PTR [rbp-0x64],0x1
   1802a1908:	48 c7 45 a4 01 00 00 	mov    QWORD PTR [rbp-0x5c],0x1
   1802a190f:	00 
   1802a1910:	89 45 ac             	mov    DWORD PTR [rbp-0x54],eax
   1802a1913:	48 89 45 b4          	mov    QWORD PTR [rbp-0x4c],rax
   1802a1917:	c7 45 b0 a8 00 00 00 	mov    DWORD PTR [rbp-0x50],0xa8
   1802a191e:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   1802a1921:	c5 f8 77             	vzeroupper
   1802a1924:	ff 50 28             	call   QWORD PTR [rax+0x28]
   1802a1927:	89 85 60 03 00 00    	mov    DWORD PTR [rbp+0x360],eax
   1802a192d:	85 c0                	test   eax,eax
   1802a192f:	79 6f                	jns    0x1802a19a0
   1802a1931:	e8 1a 07 f8 ff       	call   0x180222050
   1802a1936:	48 8d 0d f3 47 16 00 	lea    rcx,[rip+0x1647f3]        # 0x180406130
   1802a193d:	c7 44 24 78 51 05 00 	mov    DWORD PTR [rsp+0x78],0x551
   1802a1944:	00 
   1802a1945:	48 89 4c 24 70       	mov    QWORD PTR [rsp+0x70],rcx
   1802a194a:	4c 8d 4d 00          	lea    r9,[rbp+0x0]
   1802a194e:	8b 4d 9c             	mov    ecx,DWORD PTR [rbp-0x64]
   1802a1951:	48 8d 55 c0          	lea    rdx,[rbp-0x40]
   1802a1955:	89 4c 24 7c          	mov    DWORD PTR [rsp+0x7c],ecx
   1802a1959:	48 8d 0d a0 56 16 00 	lea    rcx,[rip+0x1656a0]        # 0x180407000
   1802a1960:	c5 f8 10 44 24 70    	vmovups xmm0,XMMWORD PTR [rsp+0x70]
   1802a1966:	48 89 4d 80          	mov    QWORD PTR [rbp-0x80],rcx
   1802a196a:	48 8d 0d f7 56 16 00 	lea    rcx,[rip+0x1656f7]        # 0x180407068
   1802a1971:	c5 fb 10 4d 80       	vmovsd xmm1,QWORD PTR [rbp-0x80]
   1802a1976:	48 89 4d 00          	mov    QWORD PTR [rbp+0x0],rcx
   1802a197a:	48 8d 8d 60 03 00 00 	lea    rcx,[rbp+0x360]
   1802a1981:	48 89 4c 24 20       	mov    QWORD PTR [rsp+0x20],rcx
   1802a1986:	48 8b c8             	mov    rcx,rax
   1802a1989:	48 c7 45 08 3a 00 00 	mov    QWORD PTR [rbp+0x8],0x3a
   1802a1990:	00 
   1802a1991:	c5 f8 11 45 c0       	vmovups XMMWORD PTR [rbp-0x40],xmm0
   1802a1996:	c5 fb 11 4d d0       	vmovsd QWORD PTR [rbp-0x30],xmm1
   1802a199b:	e8 70 54 ef ff       	call   0x180196e10
   1802a19a0:	49 83 3c 24 00       	cmp    QWORD PTR [r12],0x0
   1802a19a5:	0f 84 44 07 00 00    	je     0x1802a20ef
   1802a19ab:	48 83 3b 00          	cmp    QWORD PTR [rbx],0x0
   1802a19af:	0f 84 3a 07 00 00    	je     0x1802a20ef
   1802a19b5:	c5 f8 29 b4 24 00 04 	vmovaps XMMWORD PTR [rsp+0x400],xmm6
   1802a19bc:	00 00 
   1802a19be:	c5 f8 29 bc 24 f0 03 	vmovaps XMMWORD PTR [rsp+0x3f0],xmm7
   1802a19c5:	00 00 
   1802a19c7:	c5 78 29 84 24 e0 03 	vmovaps XMMWORD PTR [rsp+0x3e0],xmm8
   1802a19ce:	00 00 
   1802a19d0:	c5 f9 ef c0          	vpxor  xmm0,xmm0,xmm0
   1802a19d4:	c5 c0 57 ff          	vxorps xmm7,xmm7,xmm7
   1802a19d8:	41 8b c7             	mov    eax,r15d
   1802a19db:	4c 8d 4d f8          	lea    r9,[rbp-0x8]
   1802a19df:	c4 e1 c2 2a f8       	vcvtsi2ss xmm7,xmm7,rax
   1802a19e4:	c5 fa 11 7f 10       	vmovss DWORD PTR [rdi+0x10],xmm7
   1802a19e9:	41 8b c6             	mov    eax,r14d
   1802a19ec:	4c 8d 85 f0 00 00 00 	lea    r8,[rbp+0xf0]
   1802a19f3:	c4 41 38 57 c0       	vxorps xmm8,xmm8,xmm8
   1802a19f8:	c4 61 ba 2a c0       	vcvtsi2ss xmm8,xmm8,rax
   1802a19fd:	c5 7a 11 47 14       	vmovss DWORD PTR [rdi+0x14],xmm8
   1802a1a02:	48 8b 8f 80 16 00 00 	mov    rcx,QWORD PTR [rdi+0x1680]
   1802a1a09:	ba 08 00 00 00       	mov    edx,0x8
   1802a1a0e:	c5 fc 11 85 f0 00 00 	vmovups YMMWORD PTR [rbp+0xf0],ymm0
   1802a1a15:	00 
   1802a1a16:	c5 fc 11 85 10 01 00 	vmovups YMMWORD PTR [rbp+0x110],ymm0
   1802a1a1d:	00 
   1802a1a1e:	48 c7 45 f8 00 00 00 	mov    QWORD PTR [rbp-0x8],0x0
   1802a1a25:	00 
   1802a1a26:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   1802a1a29:	c5 f8 77             	vzeroupper
   1802a1a2c:	ff 90 c8 02 00 00    	call   QWORD PTR [rax+0x2c8]
   1802a1a32:	48 8b 8f 80 16 00 00 	mov    rcx,QWORD PTR [rdi+0x1680]
   1802a1a39:	48 8d 95 50 03 00 00 	lea    rdx,[rbp+0x350]
   1802a1a40:	c7 85 50 03 00 00 00 	mov    DWORD PTR [rbp+0x350],0x0
   1802a1a47:	00 00 00 
   1802a1a4a:	45 33 c0             	xor    r8d,r8d
   1802a1a4d:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   1802a1a50:	ff 90 f8 02 00 00    	call   QWORD PTR [rax+0x2f8]
   1802a1a56:	33 d2                	xor    edx,edx
   1802a1a58:	48 8d 8d 60 01 00 00 	lea    rcx,[rbp+0x160]
   1802a1a5f:	41 b8 80 01 00 00    	mov    r8d,0x180
   1802a1a65:	e8 c2 8d f9 ff       	call   0x18023a82c
   1802a1a6a:	83 bd 50 03 00 00 00 	cmp    DWORD PTR [rbp+0x350],0x0
   1802a1a71:	76 1e                	jbe    0x1802a1a91
   1802a1a73:	48 8b 8f 80 16 00 00 	mov    rcx,QWORD PTR [rdi+0x1680]
   1802a1a7a:	4c 8d 85 60 01 00 00 	lea    r8,[rbp+0x160]
   1802a1a81:	48 8d 95 50 03 00 00 	lea    rdx,[rbp+0x350]
   1802a1a88:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   1802a1a8b:	ff 90 f8 02 00 00    	call   QWORD PTR [rax+0x2f8]
   1802a1a91:	48 8b 8f 80 16 00 00 	mov    rcx,QWORD PTR [rdi+0x1680]
   1802a1a98:	45 33 c9             	xor    r9d,r9d
   1802a1a9b:	45 33 c0             	xor    r8d,r8d
   1802a1a9e:	33 d2                	xor    edx,edx
   1802a1aa0:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   1802a1aa3:	ff 90 08 01 00 00    	call   QWORD PTR [rax+0x108]
   1802a1aa9:	48 8b 8f 80 16 00 00 	mov    rcx,QWORD PTR [rdi+0x1680]
   1802a1ab0:	45 33 c9             	xor    r9d,r9d
   1802a1ab3:	45 33 c0             	xor    r8d,r8d
   1802a1ab6:	33 d2                	xor    edx,edx
   1802a1ab8:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   1802a1abb:	ff 50 40             	call   QWORD PTR [rax+0x40]
   1802a1abe:	8b 87 44 03 00 00    	mov    eax,DWORD PTR [rdi+0x344]
   1802a1ac4:	8b 8f 48 03 00 00    	mov    ecx,DWORD PTR [rdi+0x348]
   1802a1aca:	c5 fa 10 35 62 a2 16 	vmovss xmm6,DWORD PTR [rip+0x16a262]        # 0x18040bd34
   1802a1ad1:	00 
   1802a1ad2:	89 45 10             	mov    DWORD PTR [rbp+0x10],eax
   1802a1ad5:	41 03 c7             	add    eax,r15d
   1802a1ad8:	89 45 1c             	mov    DWORD PTR [rbp+0x1c],eax
   1802a1adb:	42 8d 04 31          	lea    eax,[rcx+r14*1]
   1802a1adf:	89 4d 14             	mov    DWORD PTR [rbp+0x14],ecx
   1802a1ae2:	33 c9                	xor    ecx,ecx
   1802a1ae4:	89 45 20             	mov    DWORD PTR [rbp+0x20],eax
   1802a1ae7:	8b 45 28             	mov    eax,DWORD PTR [rbp+0x28]
   1802a1aea:	89 4d 18             	mov    DWORD PTR [rbp+0x18],ecx
   1802a1aed:	c7 45 24 01 00 00 00 	mov    DWORD PTR [rbp+0x24],0x1
   1802a1af4:	85 c0                	test   eax,eax
   1802a1af6:	74 0f                	je     0x1802a1b07
   1802a1af8:	c5 f8 57 c0          	vxorps xmm0,xmm0,xmm0
   1802a1afc:	c4 e1 fa 2a c0       	vcvtsi2ss xmm0,xmm0,rax
   1802a1b01:	c5 c2 5e c8          	vdivss xmm1,xmm7,xmm0
   1802a1b05:	eb 04                	jmp    0x1802a1b0b
   1802a1b07:	c5 f8 28 ce          	vmovaps xmm1,xmm6
   1802a1b0b:	8b 45 2c             	mov    eax,DWORD PTR [rbp+0x2c]
   1802a1b0e:	c5 fa 11 8f 18 16 00 	vmovss DWORD PTR [rdi+0x1618],xmm1
   1802a1b15:	00 
   1802a1b16:	85 c0                	test   eax,eax
   1802a1b18:	74 0f                	je     0x1802a1b29
   1802a1b1a:	c5 f8 57 c0          	vxorps xmm0,xmm0,xmm0
   1802a1b1e:	c4 e1 fa 2a c0       	vcvtsi2ss xmm0,xmm0,rax
   1802a1b23:	c5 ba 5e c8          	vdivss xmm1,xmm8,xmm0
   1802a1b27:	eb 04                	jmp    0x1802a1b2d
   1802a1b29:	c5 f8 28 ce          	vmovaps xmm1,xmm6
   1802a1b2d:	48 89 8d 98 00 00 00 	mov    QWORD PTR [rbp+0x98],rcx
   1802a1b34:	48 89 8d b0 00 00 00 	mov    QWORD PTR [rbp+0xb0],rcx
   1802a1b3b:	48 89 b5 90 00 00 00 	mov    QWORD PTR [rbp+0x90],rsi
   1802a1b42:	c5 f9 ef c0          	vpxor  xmm0,xmm0,xmm0
   1802a1b46:	c5 fa 7f 85 a0 00 00 	vmovdqu XMMWORD PTR [rbp+0xa0],xmm0
   1802a1b4d:	00 
   1802a1b4e:	c5 fa 11 8f 1c 16 00 	vmovss DWORD PTR [rdi+0x161c],xmm1
   1802a1b55:	00 
   1802a1b56:	4d 85 ed             	test   r13,r13
   1802a1b59:	74 10                	je     0x1802a1b6b
   1802a1b5b:	49 8b 85 88 00 00 00 	mov    rax,QWORD PTR [r13+0x88]
   1802a1b62:	48 85 c0             	test   rax,rax
   1802a1b65:	0f 85 a0 01 00 00    	jne    0x1802a1d0b
   1802a1b6b:	48 8b 06             	mov    rax,QWORD PTR [rsi]
   1802a1b6e:	48 8d 95 68 03 00 00 	lea    rdx,[rbp+0x368]
   1802a1b75:	48 8b ce             	mov    rcx,rsi
   1802a1b78:	ff 50 18             	call   QWORD PTR [rax+0x18]
   1802a1b7b:	48 8b 8d 90 00 00 00 	mov    rcx,QWORD PTR [rbp+0x90]
   1802a1b82:	48 8d 95 30 01 00 00 	lea    rdx,[rbp+0x130]
   1802a1b89:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   1802a1b8c:	ff 50 50             	call   QWORD PTR [rax+0x50]
   1802a1b8f:	8b 85 40 01 00 00    	mov    eax,DWORD PTR [rbp+0x140]
   1802a1b95:	83 f8 2c             	cmp    eax,0x2c
   1802a1b98:	75 29                	jne    0x1802a1bc3
   1802a1b9a:	33 c0                	xor    eax,eax
   1802a1b9c:	48 c7 44 24 74 04 00 	mov    QWORD PTR [rsp+0x74],0x4
   1802a1ba3:	00 00 
   1802a1ba5:	48 89 45 80          	mov    QWORD PTR [rbp-0x80],rax
   1802a1ba9:	4c 8d 44 24 70       	lea    r8,[rsp+0x70]
   1802a1bae:	c7 44 24 70 2e 00 00 	mov    DWORD PTR [rsp+0x70],0x2e
   1802a1bb5:	00 
   1802a1bb6:	c7 44 24 7c 01 00 00 	mov    DWORD PTR [rsp+0x7c],0x1
   1802a1bbd:	00 
   1802a1bbe:	e9 88 00 00 00       	jmp    0x1802a1c4b
   1802a1bc3:	83 f8 13             	cmp    eax,0x13
   1802a1bc6:	75 26                	jne    0x1802a1bee
   1802a1bc8:	33 c0                	xor    eax,eax
   1802a1bca:	48 c7 44 24 74 04 00 	mov    QWORD PTR [rsp+0x74],0x4
   1802a1bd1:	00 00 
   1802a1bd3:	48 89 45 80          	mov    QWORD PTR [rbp-0x80],rax
   1802a1bd7:	4c 8d 44 24 70       	lea    r8,[rsp+0x70]
   1802a1bdc:	c7 44 24 70 15 00 00 	mov    DWORD PTR [rsp+0x70],0x15
   1802a1be3:	00 
   1802a1be4:	c7 44 24 7c 01 00 00 	mov    DWORD PTR [rsp+0x7c],0x1
   1802a1beb:	00 
   1802a1bec:	eb 5d                	jmp    0x1802a1c4b
   1802a1bee:	83 f8 21             	cmp    eax,0x21
   1802a1bf1:	75 26                	jne    0x1802a1c19
   1802a1bf3:	33 c0                	xor    eax,eax
   1802a1bf5:	48 c7 44 24 74 04 00 	mov    QWORD PTR [rsp+0x74],0x4
   1802a1bfc:	00 00 
   1802a1bfe:	48 89 45 80          	mov    QWORD PTR [rbp-0x80],rax
   1802a1c02:	4c 8d 44 24 70       	lea    r8,[rsp+0x70]
   1802a1c07:	c7 44 24 70 23 00 00 	mov    DWORD PTR [rsp+0x70],0x23
   1802a1c0e:	00 
   1802a1c0f:	c7 44 24 7c 01 00 00 	mov    DWORD PTR [rsp+0x7c],0x1
   1802a1c16:	00 
   1802a1c17:	eb 32                	jmp    0x1802a1c4b
   1802a1c19:	83 f8 5a             	cmp    eax,0x5a
   1802a1c1c:	75 1c                	jne    0x1802a1c3a
   1802a1c1e:	c7 45 c0 57 00 00 00 	mov    DWORD PTR [rbp-0x40],0x57
   1802a1c25:	48 c7 45 c4 04 00 00 	mov    QWORD PTR [rbp-0x3c],0x4
   1802a1c2c:	00 
   1802a1c2d:	4c 8d 45 c0          	lea    r8,[rbp-0x40]
   1802a1c31:	c7 45 cc ff ff ff ff 	mov    DWORD PTR [rbp-0x34],0xffffffff
   1802a1c38:	eb 11                	jmp    0x1802a1c4b
   1802a1c3a:	83 f8 1b             	cmp    eax,0x1b
   1802a1c3d:	75 09                	jne    0x1802a1c48
   1802a1c3f:	c7 45 c0 1c 00 00 00 	mov    DWORD PTR [rbp-0x40],0x1c
   1802a1c46:	eb dd                	jmp    0x1802a1c25
   1802a1c48:	45 33 c0             	xor    r8d,r8d
   1802a1c4b:	48 8b 8d 68 03 00 00 	mov    rcx,QWORD PTR [rbp+0x368]
   1802a1c52:	4c 8d 8d a8 00 00 00 	lea    r9,[rbp+0xa8]
   1802a1c59:	48 8b 95 90 00 00 00 	mov    rdx,QWORD PTR [rbp+0x90]
   1802a1c60:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   1802a1c63:	ff 50 38             	call   QWORD PTR [rax+0x38]
   1802a1c66:	85 c0                	test   eax,eax
   1802a1c68:	0f 89 89 00 00 00    	jns    0x1802a1cf7
   1802a1c6e:	48 98                	cdqe
   1802a1c70:	48 89 45 00          	mov    QWORD PTR [rbp+0x0],rax
   1802a1c74:	8b 85 40 01 00 00    	mov    eax,DWORD PTR [rbp+0x140]
   1802a1c7a:	89 85 60 03 00 00    	mov    DWORD PTR [rbp+0x360],eax
   1802a1c80:	e8 cb 03 f8 ff       	call   0x180222050
   1802a1c85:	48 8d 0d 5c 6c 14 00 	lea    rcx,[rip+0x146c5c]        # 0x1803e88e8
   1802a1c8c:	c7 45 c8 c6 00 00 00 	mov    DWORD PTR [rbp-0x38],0xc6
   1802a1c93:	48 89 4d c0          	mov    QWORD PTR [rbp-0x40],rcx
   1802a1c97:	4c 8d 4c 24 70       	lea    r9,[rsp+0x70]
   1802a1c9c:	8b 4d 9c             	mov    ecx,DWORD PTR [rbp-0x64]
   1802a1c9f:	48 8d 55 90          	lea    rdx,[rbp-0x70]
   1802a1ca3:	89 4d cc             	mov    DWORD PTR [rbp-0x34],ecx
   1802a1ca6:	48 8d 0d f3 6c 14 00 	lea    rcx,[rip+0x146cf3]        # 0x1803e89a0
   1802a1cad:	c5 f8 10 45 c0       	vmovups xmm0,XMMWORD PTR [rbp-0x40]
   1802a1cb2:	48 89 4d d0          	mov    QWORD PTR [rbp-0x30],rcx
   1802a1cb6:	48 8d 0d 5b 6d 14 00 	lea    rcx,[rip+0x146d5b]        # 0x1803e8a18
   1802a1cbd:	c5 fb 10 4d d0       	vmovsd xmm1,QWORD PTR [rbp-0x30]
   1802a1cc2:	48 89 4c 24 70       	mov    QWORD PTR [rsp+0x70],rcx
   1802a1cc7:	48 8d 4d 00          	lea    rcx,[rbp+0x0]
   1802a1ccb:	48 89 4c 24 28       	mov    QWORD PTR [rsp+0x28],rcx
   1802a1cd0:	48 8d 8d 60 03 00 00 	lea    rcx,[rbp+0x360]
   1802a1cd7:	48 89 4c 24 20       	mov    QWORD PTR [rsp+0x20],rcx
   1802a1cdc:	48 8b c8             	mov    rcx,rax
   1802a1cdf:	48 c7 44 24 78 2f 00 	mov    QWORD PTR [rsp+0x78],0x2f
   1802a1ce6:	00 00 
   1802a1ce8:	c5 f8 11 45 90       	vmovups XMMWORD PTR [rbp-0x70],xmm0
   1802a1ced:	c5 fb 11 4d a0       	vmovsd QWORD PTR [rbp-0x60],xmm1
   1802a1cf2:	e8 49 be ec ff       	call   0x18016db40
   1802a1cf7:	48 8b 8d 68 03 00 00 	mov    rcx,QWORD PTR [rbp+0x368]
   1802a1cfe:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   1802a1d01:	ff 50 10             	call   QWORD PTR [rax+0x10]
   1802a1d04:	48 8b 85 a8 00 00 00 	mov    rax,QWORD PTR [rbp+0xa8]
   1802a1d0b:	49 8b cc             	mov    rcx,r12
   1802a1d0e:	48 89 45 f0          	mov    QWORD PTR [rbp-0x10],rax
   1802a1d12:	e8 39 a7 ec ff       	call   0x18016c450
   1802a1d17:	4c 8b e0             	mov    r12,rax
   1802a1d1a:	b9 02 00 00 00       	mov    ecx,0x2
   1802a1d1f:	48 8b 45 f0          	mov    rax,QWORD PTR [rbp-0x10]
   1802a1d23:	48 85 c0             	test   rax,rax
   1802a1d26:	74 56                	je     0x1802a1d7e
   1802a1d28:	4d 85 e4             	test   r12,r12
   1802a1d2b:	74 51                	je     0x1802a1d7e
   1802a1d2d:	45 33 ed             	xor    r13d,r13d
   1802a1d30:	48 8d 45 f0          	lea    rax,[rbp-0x10]
   1802a1d34:	4c 89 6c 24 68       	mov    QWORD PTR [rsp+0x68],r13
   1802a1d39:	8b d1                	mov    edx,ecx
   1802a1d3b:	c6 44 24 60 01       	mov    BYTE PTR [rsp+0x60],0x1
   1802a1d40:	45 33 c9             	xor    r9d,r9d
   1802a1d43:	44 89 6c 24 58       	mov    DWORD PTR [rsp+0x58],r13d
   1802a1d48:	45 33 c0             	xor    r8d,r8d
   1802a1d4b:	44 89 6c 24 50       	mov    DWORD PTR [rsp+0x50],r13d
   1802a1d50:	48 8b cf             	mov    rcx,rdi
   1802a1d53:	44 89 74 24 48       	mov    DWORD PTR [rsp+0x48],r14d
   1802a1d58:	44 89 7c 24 40       	mov    DWORD PTR [rsp+0x40],r15d
   1802a1d5d:	4c 89 6c 24 38       	mov    QWORD PTR [rsp+0x38],r13
   1802a1d62:	4c 89 64 24 30       	mov    QWORD PTR [rsp+0x30],r12
   1802a1d67:	48 89 44 24 28       	mov    QWORD PTR [rsp+0x28],rax
   1802a1d6c:	c7 44 24 20 01 00 00 	mov    DWORD PTR [rsp+0x20],0x1
   1802a1d73:	00 
   1802a1d74:	e8 c7 f4 ff ff       	call   0x1802a1240
   1802a1d79:	e9 91 00 00 00       	jmp    0x1802a1e0f
   1802a1d7e:	4c 89 a5 60 03 00 00 	mov    QWORD PTR [rbp+0x360],r12
   1802a1d85:	48 89 85 68 03 00 00 	mov    QWORD PTR [rbp+0x368],rax
   1802a1d8c:	e8 bf 02 f8 ff       	call   0x180222050
   1802a1d91:	48 8d 0d 98 43 16 00 	lea    rcx,[rip+0x164398]        # 0x180406130
   1802a1d98:	c7 45 c8 77 05 00 00 	mov    DWORD PTR [rbp-0x38],0x577
   1802a1d9f:	48 89 4d c0          	mov    QWORD PTR [rbp-0x40],rcx
   1802a1da3:	4c 8d 4c 24 70       	lea    r9,[rsp+0x70]
   1802a1da8:	8b 4d 9c             	mov    ecx,DWORD PTR [rbp-0x64]
   1802a1dab:	48 8d 55 90          	lea    rdx,[rbp-0x70]
   1802a1daf:	89 4d cc             	mov    DWORD PTR [rbp-0x34],ecx
   1802a1db2:	41 b8 04 00 00 00    	mov    r8d,0x4
   1802a1db8:	c5 f8 10 45 c0       	vmovups xmm0,XMMWORD PTR [rbp-0x40]
   1802a1dbd:	48 8d 0d 3c 52 16 00 	lea    rcx,[rip+0x16523c]        # 0x180407000
   1802a1dc4:	48 c7 44 24 78 23 00 	mov    QWORD PTR [rsp+0x78],0x23
   1802a1dcb:	00 00 
   1802a1dcd:	48 89 4d d0          	mov    QWORD PTR [rbp-0x30],rcx
   1802a1dd1:	48 8d 0d 68 52 16 00 	lea    rcx,[rip+0x165268]        # 0x180407040
   1802a1dd8:	c5 fb 10 4d d0       	vmovsd xmm1,QWORD PTR [rbp-0x30]
   1802a1ddd:	48 89 4c 24 70       	mov    QWORD PTR [rsp+0x70],rcx
   1802a1de2:	48 8d 8d 60 03 00 00 	lea    rcx,[rbp+0x360]
   1802a1de9:	48 89 4c 24 28       	mov    QWORD PTR [rsp+0x28],rcx
   1802a1dee:	48 8d 8d 68 03 00 00 	lea    rcx,[rbp+0x368]
   1802a1df5:	48 89 4c 24 20       	mov    QWORD PTR [rsp+0x20],rcx
   1802a1dfa:	48 8b c8             	mov    rcx,rax
   1802a1dfd:	c5 f8 11 45 90       	vmovups XMMWORD PTR [rbp-0x70],xmm0
   1802a1e02:	c5 fb 11 4d a0       	vmovsd QWORD PTR [rbp-0x60],xmm1
   1802a1e07:	e8 14 4e ef ff       	call   0x180196c20
   1802a1e0c:	45 33 ed             	xor    r13d,r13d
   1802a1e0f:	48 8b 8d a0 00 00 00 	mov    rcx,QWORD PTR [rbp+0xa0]
   1802a1e16:	48 85 c9             	test   rcx,rcx
   1802a1e19:	74 0d                	je     0x1802a1e28
   1802a1e1b:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   1802a1e1e:	ff 50 10             	call   QWORD PTR [rax+0x10]
   1802a1e21:	4c 89 ad a0 00 00 00 	mov    QWORD PTR [rbp+0xa0],r13
   1802a1e28:	48 8b 8d a8 00 00 00 	mov    rcx,QWORD PTR [rbp+0xa8]
   1802a1e2f:	48 85 c9             	test   rcx,rcx
   1802a1e32:	74 0d                	je     0x1802a1e41
   1802a1e34:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   1802a1e37:	ff 50 10             	call   QWORD PTR [rax+0x10]
   1802a1e3a:	4c 89 ad a8 00 00 00 	mov    QWORD PTR [rbp+0xa8],r13
   1802a1e41:	48 8b 8d b0 00 00 00 	mov    rcx,QWORD PTR [rbp+0xb0]
   1802a1e48:	48 85 c9             	test   rcx,rcx
   1802a1e4b:	74 0d                	je     0x1802a1e5a
   1802a1e4d:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   1802a1e50:	ff 50 10             	call   QWORD PTR [rax+0x10]
   1802a1e53:	4c 89 ad b0 00 00 00 	mov    QWORD PTR [rbp+0xb0],r13
   1802a1e5a:	48 8b 0b             	mov    rcx,QWORD PTR [rbx]
   1802a1e5d:	4c 89 ad 90 00 00 00 	mov    QWORD PTR [rbp+0x90],r13
   1802a1e64:	48 85 c9             	test   rcx,rcx
   1802a1e67:	74 0a                	je     0x1802a1e73
   1802a1e69:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   1802a1e6c:	48 8d 53 28          	lea    rdx,[rbx+0x28]
   1802a1e70:	ff 50 50             	call   QWORD PTR [rax+0x50]
   1802a1e73:	8b 55 5c             	mov    edx,DWORD PTR [rbp+0x5c]
   1802a1e76:	b8 28 00 00 00       	mov    eax,0x28
   1802a1e7b:	48 8b cb             	mov    rcx,rbx
   1802a1e7e:	8b 4d 58             	mov    ecx,DWORD PTR [rbp+0x58]
   1802a1e81:	c5 fc 10 0c 03       	vmovups ymm1,YMMWORD PTR [rbx+rax*1]
   1802a1e86:	c4 e3 7d 19 c8 01    	vextractf128 xmm0,ymm1,0x1
   1802a1e8c:	c5 f9 7e c0          	vmovd  eax,xmm0
   1802a1e90:	39 45 68             	cmp    DWORD PTR [rbp+0x68],eax
   1802a1e93:	0f 85 90 01 00 00    	jne    0x1802a2029
   1802a1e99:	39 4d 1c             	cmp    DWORD PTR [rbp+0x1c],ecx
   1802a1e9c:	0f 87 87 01 00 00    	ja     0x1802a2029
   1802a1ea2:	39 55 20             	cmp    DWORD PTR [rbp+0x20],edx
   1802a1ea5:	0f 87 7e 01 00 00    	ja     0x1802a2029
   1802a1eab:	48 8b 8f 80 16 00 00 	mov    rcx,QWORD PTR [rdi+0x1680]
   1802a1eb2:	40 b6 01             	mov    sil,0x1
   1802a1eb5:	48 8b 97 a8 0b 00 00 	mov    rdx,QWORD PTR [rdi+0xba8]
   1802a1ebc:	45 33 c9             	xor    r9d,r9d
   1802a1ebf:	45 33 c0             	xor    r8d,r8d
   1802a1ec2:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   1802a1ec5:	4c 8b 90 70 01 00 00 	mov    r10,QWORD PTR [rax+0x170]
   1802a1ecc:	48 8d 45 10          	lea    rax,[rbp+0x10]
   1802a1ed0:	48 89 44 24 40       	mov    QWORD PTR [rsp+0x40],rax
   1802a1ed5:	48 8b 87 28 08 00 00 	mov    rax,QWORD PTR [rdi+0x828]
   1802a1edc:	44 89 6c 24 38       	mov    DWORD PTR [rsp+0x38],r13d
   1802a1ee1:	48 89 44 24 30       	mov    QWORD PTR [rsp+0x30],rax
   1802a1ee6:	44 89 6c 24 28       	mov    DWORD PTR [rsp+0x28],r13d
   1802a1eeb:	44 89 6c 24 20       	mov    DWORD PTR [rsp+0x20],r13d
   1802a1ef0:	c5 f8 77             	vzeroupper
   1802a1ef3:	41 ff d2             	call   r10
   1802a1ef6:	bb 03 00 00 00       	mov    ebx,0x3
   1802a1efb:	48 83 bd f0 00 00 00 	cmp    QWORD PTR [rbp+0xf0],0x0
   1802a1f02:	00 
   1802a1f03:	4c 8d 85 f0 00 00 00 	lea    r8,[rbp+0xf0]
   1802a1f0a:	48 8b 8f 80 16 00 00 	mov    rcx,QWORD PTR [rdi+0x1680]
   1802a1f11:	41 8b d5             	mov    edx,r13d
   1802a1f14:	4c 8b 4d f8          	mov    r9,QWORD PTR [rbp-0x8]
   1802a1f18:	0f 95 c2             	setne  dl
   1802a1f1b:	48 83 bd f8 00 00 00 	cmp    QWORD PTR [rbp+0xf8],0x0
   1802a1f22:	00 
   1802a1f23:	b8 02 00 00 00       	mov    eax,0x2
   1802a1f28:	0f 45 d0             	cmovne edx,eax
   1802a1f2b:	48 83 bd 00 01 00 00 	cmp    QWORD PTR [rbp+0x100],0x0
   1802a1f32:	00 
   1802a1f33:	b8 04 00 00 00       	mov    eax,0x4
   1802a1f38:	0f 45 d3             	cmovne edx,ebx
   1802a1f3b:	48 83 bd 08 01 00 00 	cmp    QWORD PTR [rbp+0x108],0x0
   1802a1f42:	00 
   1802a1f43:	0f 45 d0             	cmovne edx,eax
   1802a1f46:	48 83 bd 10 01 00 00 	cmp    QWORD PTR [rbp+0x110],0x0
   1802a1f4d:	00 
   1802a1f4e:	b8 05 00 00 00       	mov    eax,0x5
   1802a1f53:	0f 45 d0             	cmovne edx,eax
   1802a1f56:	48 83 bd 18 01 00 00 	cmp    QWORD PTR [rbp+0x118],0x0
   1802a1f5d:	00 
   1802a1f5e:	b8 06 00 00 00       	mov    eax,0x6
   1802a1f63:	0f 45 d0             	cmovne edx,eax
   1802a1f66:	48 83 bd 20 01 00 00 	cmp    QWORD PTR [rbp+0x120],0x0
   1802a1f6d:	00 
   1802a1f6e:	b8 07 00 00 00       	mov    eax,0x7
   1802a1f73:	0f 45 d0             	cmovne edx,eax
   1802a1f76:	48 83 bd 28 01 00 00 	cmp    QWORD PTR [rbp+0x128],0x0
   1802a1f7d:	00 
   1802a1f7e:	b8 08 00 00 00       	mov    eax,0x8
   1802a1f83:	0f 45 d0             	cmovne edx,eax
   1802a1f86:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   1802a1f89:	85 d2                	test   edx,edx
   1802a1f8b:	4d 0f 44 c5          	cmove  r8,r13
   1802a1f8f:	ff 90 08 01 00 00    	call   QWORD PTR [rax+0x108]
   1802a1f95:	8b 95 50 03 00 00    	mov    edx,DWORD PTR [rbp+0x350]
   1802a1f9b:	c5 78 28 84 24 e0 03 	vmovaps xmm8,XMMWORD PTR [rsp+0x3e0]
   1802a1fa2:	00 00 
   1802a1fa4:	c5 f8 28 bc 24 f0 03 	vmovaps xmm7,XMMWORD PTR [rsp+0x3f0]
   1802a1fab:	00 00 
   1802a1fad:	c5 f8 28 b4 24 00 04 	vmovaps xmm6,XMMWORD PTR [rsp+0x400]
   1802a1fb4:	00 00 
   1802a1fb6:	85 d2                	test   edx,edx
   1802a1fb8:	74 17                	je     0x1802a1fd1
   1802a1fba:	48 8b 8f 80 16 00 00 	mov    rcx,QWORD PTR [rdi+0x1680]
   1802a1fc1:	4c 8d 85 60 01 00 00 	lea    r8,[rbp+0x160]
   1802a1fc8:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   1802a1fcb:	ff 90 60 01 00 00    	call   QWORD PTR [rax+0x160]
   1802a1fd1:	48 8d 9d f0 00 00 00 	lea    rbx,[rbp+0xf0]
   1802a1fd8:	48 8b 0b             	mov    rcx,QWORD PTR [rbx]
   1802a1fdb:	48 85 c9             	test   rcx,rcx
   1802a1fde:	74 06                	je     0x1802a1fe6
   1802a1fe0:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   1802a1fe3:	ff 50 10             	call   QWORD PTR [rax+0x10]
   1802a1fe6:	48 83 c3 08          	add    rbx,0x8
   1802a1fea:	48 8d 85 30 01 00 00 	lea    rax,[rbp+0x130]
   1802a1ff1:	48 3b d8             	cmp    rbx,rax
   1802a1ff4:	75 e2                	jne    0x1802a1fd8
   1802a1ff6:	48 8b 4d f8          	mov    rcx,QWORD PTR [rbp-0x8]
   1802a1ffa:	48 85 c9             	test   rcx,rcx
   1802a1ffd:	74 06                	je     0x1802a2005
   1802a1fff:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   1802a2002:	ff 50 10             	call   QWORD PTR [rax+0x10]
   1802a2005:	48 83 7d f0 00       	cmp    QWORD PTR [rbp-0x10],0x0
   1802a200a:	0f 84 df 00 00 00    	je     0x1802a20ef
   1802a2010:	4d 85 e4             	test   r12,r12
   1802a2013:	0f 84 d6 00 00 00    	je     0x1802a20ef
   1802a2019:	40 84 f6             	test   sil,sil
   1802a201c:	0f 84 cd 00 00 00    	je     0x1802a20ef
   1802a2022:	b0 01                	mov    al,0x1
   1802a2024:	e9 c8 00 00 00       	jmp    0x1802a20f1
   1802a2029:	40 32 f6             	xor    sil,sil
   1802a202c:	85 c9                	test   ecx,ecx
   1802a202e:	74 0f                	je     0x1802a203f
   1802a2030:	c5 f8 57 c0          	vxorps xmm0,xmm0,xmm0
   1802a2034:	c4 e1 fa 2a c1       	vcvtsi2ss xmm0,xmm0,rcx
   1802a2039:	c5 c2 5e c8          	vdivss xmm1,xmm7,xmm0
   1802a203d:	eb 04                	jmp    0x1802a2043
   1802a203f:	c5 f8 28 ce          	vmovaps xmm1,xmm6
   1802a2043:	c5 fa 11 8f 18 16 00 	vmovss DWORD PTR [rdi+0x1618],xmm1
   1802a204a:	00 
   1802a204b:	85 d2                	test   edx,edx
   1802a204d:	74 0d                	je     0x1802a205c
   1802a204f:	c5 f8 57 c0          	vxorps xmm0,xmm0,xmm0
   1802a2053:	c4 e1 fa 2a c2       	vcvtsi2ss xmm0,xmm0,rdx
   1802a2058:	c5 ba 5e f0          	vdivss xmm6,xmm8,xmm0
   1802a205c:	c5 fa 11 b7 1c 16 00 	vmovss DWORD PTR [rdi+0x161c],xmm6
   1802a2063:	00 
   1802a2064:	48 8d 8f 28 08 00 00 	lea    rcx,[rdi+0x828]
   1802a206b:	c5 f8 77             	vzeroupper
   1802a206e:	e8 1d a2 ec ff       	call   0x18016c290
   1802a2073:	48 8b cb             	mov    rcx,rbx
   1802a2076:	48 89 85 60 03 00 00 	mov    QWORD PTR [rbp+0x360],rax
   1802a207d:	e8 be a0 ec ff       	call   0x18016c140
   1802a2082:	48 83 bd 60 03 00 00 	cmp    QWORD PTR [rbp+0x360],0x0
   1802a2089:	00 
   1802a208a:	0f 84 66 fe ff ff    	je     0x1802a1ef6
   1802a2090:	bb 03 00 00 00       	mov    ebx,0x3
   1802a2095:	48 85 c0             	test   rax,rax
   1802a2098:	0f 84 5d fe ff ff    	je     0x1802a1efb
   1802a209e:	4c 89 6c 24 68       	mov    QWORD PTR [rsp+0x68],r13
   1802a20a3:	45 33 c9             	xor    r9d,r9d
   1802a20a6:	c6 44 24 60 01       	mov    BYTE PTR [rsp+0x60],0x1
   1802a20ab:	45 33 c0             	xor    r8d,r8d
   1802a20ae:	44 89 6c 24 58       	mov    DWORD PTR [rsp+0x58],r13d
   1802a20b3:	8b d3                	mov    edx,ebx
   1802a20b5:	44 89 6c 24 50       	mov    DWORD PTR [rsp+0x50],r13d
   1802a20ba:	48 8b cf             	mov    rcx,rdi
   1802a20bd:	44 89 74 24 48       	mov    DWORD PTR [rsp+0x48],r14d
   1802a20c2:	44 89 7c 24 40       	mov    DWORD PTR [rsp+0x40],r15d
   1802a20c7:	48 89 44 24 38       	mov    QWORD PTR [rsp+0x38],rax
   1802a20cc:	48 8d 85 60 03 00 00 	lea    rax,[rbp+0x360]
   1802a20d3:	4c 89 6c 24 30       	mov    QWORD PTR [rsp+0x30],r13
   1802a20d8:	48 89 44 24 28       	mov    QWORD PTR [rsp+0x28],rax
   1802a20dd:	c7 44 24 20 01 00 00 	mov    DWORD PTR [rsp+0x20],0x1
   1802a20e4:	00 
   1802a20e5:	e8 56 f1 ff ff       	call   0x1802a1240
   1802a20ea:	e9 0c fe ff ff       	jmp    0x1802a1efb
   1802a20ef:	32 c0                	xor    al,al
   1802a20f1:	48 8b 9c 24 58 04 00 	mov    rbx,QWORD PTR [rsp+0x458]
   1802a20f8:	00 
   1802a20f9:	48 81 c4 10 04 00 00 	add    rsp,0x410
   1802a2100:	41 5f                	pop    r15
   1802a2102:	41 5e                	pop    r14
   1802a2104:	41 5d                	pop    r13
   1802a2106:	41 5c                	pop    r12
   1802a2108:	5f                   	pop    rdi
   1802a2109:	5e                   	pop    rsi
   1802a210a:	5d                   	pop    rbp
   1802a210b:	c3                   	ret
   1802a210c:	cc                   	int3
   1802a210d:	cc                   	int3
   1802a210e:	cc                   	int3
   1802a210f:	cc                   	int3
   1802a2110:	cc                   	int3
   1802a2111:	cc                   	int3
   1802a2112:	cc                   	int3
   1802a2113:	cc                   	int3
   1802a2114:	cc                   	int3
   1802a2115:	cc                   	int3
   1802a2116:	cc                   	int3
   1802a2117:	cc                   	int3
   1802a2118:	cc                   	int3
   1802a2119:	cc                   	int3
   1802a211a:	cc                   	int3
   1802a211b:	cc                   	int3
   1802a211c:	cc                   	int3
   1802a211d:	cc                   	int3
   1802a211e:	cc                   	int3
   1802a211f:	cc                   	int3
