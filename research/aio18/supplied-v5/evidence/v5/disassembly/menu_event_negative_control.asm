
/mnt/data/aio18_work/extracted/SKSE/Plugins/SkyrimUpscaler.dll:     file format pei-x86-64


Disassembly of section .text:

000000018025e890 <.text+0x25d890>:
   18025e890:	48 89 5c 24 08       	mov    QWORD PTR [rsp+0x8],rbx
   18025e895:	48 89 74 24 10       	mov    QWORD PTR [rsp+0x10],rsi
   18025e89a:	57                   	push   rdi
   18025e89b:	48 83 ec 30          	sub    rsp,0x30
   18025e89f:	48 8b 02             	mov    rax,QWORD PTR [rdx]
   18025e8a2:	33 f6                	xor    esi,esi
   18025e8a4:	48 85 c0             	test   rax,rax
   18025e8a7:	48 8b da             	mov    rbx,rdx
   18025e8aa:	48 8d 48 e8          	lea    rcx,[rax-0x18]
   18025e8ae:	48 0f 44 ce          	cmove  rcx,rsi
   18025e8b2:	48 85 c9             	test   rcx,rcx
   18025e8b5:	74 0a                	je     0x18025e8c1
   18025e8b7:	8b 41 10             	mov    eax,DWORD PTR [rcx+0x10]
   18025e8ba:	25 ff ff ff 00       	and    eax,0xffffff
   18025e8bf:	eb 02                	jmp    0x18025e8c3
   18025e8c1:	8b c6                	mov    eax,esi
   18025e8c3:	44 8b c0             	mov    r8d,eax
   18025e8c6:	48 8d 3d 39 6b 18 00 	lea    rdi,[rip+0x186b39]        # 0x1803e5406
   18025e8cd:	49 83 f8 09          	cmp    r8,0x9
   18025e8d1:	75 62                	jne    0x18025e935
   18025e8d3:	48 8b 15 5e 6e 18 00 	mov    rdx,QWORD PTR [rip+0x186e5e]        # 0x1803e5738
   18025e8da:	48 8d 41 18          	lea    rax,[rcx+0x18]
   18025e8de:	48 85 c9             	test   rcx,rcx
   18025e8e1:	48 8b cf             	mov    rcx,rdi
   18025e8e4:	48 0f 44 c6          	cmove  rax,rsi
   18025e8e8:	48 85 c0             	test   rax,rax
   18025e8eb:	48 0f 45 c8          	cmovne rcx,rax
   18025e8ef:	ff 15 73 55 06 00    	call   QWORD PTR [rip+0x65573]        # 0x1802c3e68
   18025e8f5:	85 c0                	test   eax,eax
   18025e8f7:	75 3c                	jne    0x18025e935
   18025e8f9:	40 38 73 08          	cmp    BYTE PTR [rbx+0x8],sil
   18025e8fd:	74 1b                	je     0x18025e91a
   18025e8ff:	e8 fc 45 ef ff       	call   0x180152f00
   18025e904:	c6 40 11 01          	mov    BYTE PTR [rax+0x11],0x1
   18025e908:	33 c0                	xor    eax,eax
   18025e90a:	48 8b 5c 24 40       	mov    rbx,QWORD PTR [rsp+0x40]
   18025e90f:	48 8b 74 24 48       	mov    rsi,QWORD PTR [rsp+0x48]
   18025e914:	48 83 c4 30          	add    rsp,0x30
   18025e918:	5f                   	pop    rdi
   18025e919:	c3                   	ret
   18025e91a:	e8 e1 45 ef ff       	call   0x180152f00
   18025e91f:	40 88 70 11          	mov    BYTE PTR [rax+0x11],sil
   18025e923:	33 c0                	xor    eax,eax
   18025e925:	48 8b 5c 24 40       	mov    rbx,QWORD PTR [rsp+0x40]
   18025e92a:	48 8b 74 24 48       	mov    rsi,QWORD PTR [rsp+0x48]
   18025e92f:	48 83 c4 30          	add    rsp,0x30
   18025e933:	5f                   	pop    rdi
   18025e934:	c3                   	ret
   18025e935:	48 8b 03             	mov    rax,QWORD PTR [rbx]
   18025e938:	48 85 c0             	test   rax,rax
   18025e93b:	48 8d 48 e8          	lea    rcx,[rax-0x18]
   18025e93f:	48 0f 44 ce          	cmove  rcx,rsi
   18025e943:	48 85 c9             	test   rcx,rcx
   18025e946:	74 0a                	je     0x18025e952
   18025e948:	8b 41 10             	mov    eax,DWORD PTR [rcx+0x10]
   18025e94b:	25 ff ff ff 00       	and    eax,0xffffff
   18025e950:	eb 02                	jmp    0x18025e954
   18025e952:	8b c6                	mov    eax,esi
   18025e954:	44 8b c0             	mov    r8d,eax
   18025e957:	49 83 f8 0c          	cmp    r8,0xc
   18025e95b:	75 62                	jne    0x18025e9bf
   18025e95d:	48 8b 15 b4 6d 18 00 	mov    rdx,QWORD PTR [rip+0x186db4]        # 0x1803e5718
   18025e964:	48 8d 41 18          	lea    rax,[rcx+0x18]
   18025e968:	48 85 c9             	test   rcx,rcx
   18025e96b:	48 0f 44 c6          	cmove  rax,rsi
   18025e96f:	48 85 c0             	test   rax,rax
   18025e972:	48 0f 45 f8          	cmovne rdi,rax
   18025e976:	48 8b cf             	mov    rcx,rdi
   18025e979:	ff 15 e9 54 06 00    	call   QWORD PTR [rip+0x654e9]        # 0x1802c3e68
   18025e97f:	85 c0                	test   eax,eax
   18025e981:	75 3c                	jne    0x18025e9bf
   18025e983:	40 38 73 08          	cmp    BYTE PTR [rbx+0x8],sil
   18025e987:	74 1b                	je     0x18025e9a4
   18025e989:	e8 72 45 ef ff       	call   0x180152f00
   18025e98e:	c6 40 12 01          	mov    BYTE PTR [rax+0x12],0x1
   18025e992:	33 c0                	xor    eax,eax
   18025e994:	48 8b 5c 24 40       	mov    rbx,QWORD PTR [rsp+0x40]
   18025e999:	48 8b 74 24 48       	mov    rsi,QWORD PTR [rsp+0x48]
   18025e99e:	48 83 c4 30          	add    rsp,0x30
   18025e9a2:	5f                   	pop    rdi
   18025e9a3:	c3                   	ret
   18025e9a4:	e8 57 45 ef ff       	call   0x180152f00
   18025e9a9:	40 88 70 12          	mov    BYTE PTR [rax+0x12],sil
   18025e9ad:	33 c0                	xor    eax,eax
   18025e9af:	48 8b 5c 24 40       	mov    rbx,QWORD PTR [rsp+0x40]
   18025e9b4:	48 8b 74 24 48       	mov    rsi,QWORD PTR [rsp+0x48]
   18025e9b9:	48 83 c4 30          	add    rsp,0x30
   18025e9bd:	5f                   	pop    rdi
   18025e9be:	c3                   	ret
   18025e9bf:	c5 f8 10 05 61 6d 18 	vmovups xmm0,XMMWORD PTR [rip+0x186d61]        # 0x1803e5728
   18025e9c6:	00 
   18025e9c7:	48 8d 54 24 20       	lea    rdx,[rsp+0x20]
   18025e9cc:	48 8b cb             	mov    rcx,rbx
   18025e9cf:	c5 f8 11 44 24 20    	vmovups XMMWORD PTR [rsp+0x20],xmm0
   18025e9d5:	e8 66 e2 ef ff       	call   0x18015cc40
   18025e9da:	84 c0                	test   al,al
   18025e9dc:	74 3c                	je     0x18025ea1a
   18025e9de:	40 38 73 08          	cmp    BYTE PTR [rbx+0x8],sil
   18025e9e2:	74 1b                	je     0x18025e9ff
   18025e9e4:	e8 17 45 ef ff       	call   0x180152f00
   18025e9e9:	c6 40 14 01          	mov    BYTE PTR [rax+0x14],0x1
   18025e9ed:	33 c0                	xor    eax,eax
   18025e9ef:	48 8b 5c 24 40       	mov    rbx,QWORD PTR [rsp+0x40]
   18025e9f4:	48 8b 74 24 48       	mov    rsi,QWORD PTR [rsp+0x48]
   18025e9f9:	48 83 c4 30          	add    rsp,0x30
   18025e9fd:	5f                   	pop    rdi
   18025e9fe:	c3                   	ret
   18025e9ff:	e8 fc 44 ef ff       	call   0x180152f00
   18025ea04:	40 88 70 14          	mov    BYTE PTR [rax+0x14],sil
   18025ea08:	33 c0                	xor    eax,eax
   18025ea0a:	48 8b 5c 24 40       	mov    rbx,QWORD PTR [rsp+0x40]
   18025ea0f:	48 8b 74 24 48       	mov    rsi,QWORD PTR [rsp+0x48]
   18025ea14:	48 83 c4 30          	add    rsp,0x30
   18025ea18:	5f                   	pop    rdi
   18025ea19:	c3                   	ret
   18025ea1a:	c5 f8 10 05 d6 6c 18 	vmovups xmm0,XMMWORD PTR [rip+0x186cd6]        # 0x1803e56f8
   18025ea21:	00 
   18025ea22:	48 8d 54 24 20       	lea    rdx,[rsp+0x20]
   18025ea27:	48 8b cb             	mov    rcx,rbx
   18025ea2a:	c5 f8 11 44 24 20    	vmovups XMMWORD PTR [rsp+0x20],xmm0
   18025ea30:	e8 0b e2 ef ff       	call   0x18015cc40
   18025ea35:	84 c0                	test   al,al
   18025ea37:	74 3c                	je     0x18025ea75
   18025ea39:	40 38 73 08          	cmp    BYTE PTR [rbx+0x8],sil
   18025ea3d:	74 1b                	je     0x18025ea5a
   18025ea3f:	e8 bc 44 ef ff       	call   0x180152f00
   18025ea44:	c6 40 13 01          	mov    BYTE PTR [rax+0x13],0x1
   18025ea48:	33 c0                	xor    eax,eax
   18025ea4a:	48 8b 5c 24 40       	mov    rbx,QWORD PTR [rsp+0x40]
   18025ea4f:	48 8b 74 24 48       	mov    rsi,QWORD PTR [rsp+0x48]
   18025ea54:	48 83 c4 30          	add    rsp,0x30
   18025ea58:	5f                   	pop    rdi
   18025ea59:	c3                   	ret
   18025ea5a:	e8 a1 44 ef ff       	call   0x180152f00
   18025ea5f:	40 88 70 13          	mov    BYTE PTR [rax+0x13],sil
   18025ea63:	33 c0                	xor    eax,eax
   18025ea65:	48 8b 5c 24 40       	mov    rbx,QWORD PTR [rsp+0x40]
   18025ea6a:	48 8b 74 24 48       	mov    rsi,QWORD PTR [rsp+0x48]
   18025ea6f:	48 83 c4 30          	add    rsp,0x30
   18025ea73:	5f                   	pop    rdi
   18025ea74:	c3                   	ret
   18025ea75:	c5 f8 10 05 8b 6c 18 	vmovups xmm0,XMMWORD PTR [rip+0x186c8b]        # 0x1803e5708
   18025ea7c:	00 
   18025ea7d:	48 8d 54 24 20       	lea    rdx,[rsp+0x20]
   18025ea82:	48 8b cb             	mov    rcx,rbx
   18025ea85:	c5 f8 11 44 24 20    	vmovups XMMWORD PTR [rsp+0x20],xmm0
   18025ea8b:	e8 b0 e1 ef ff       	call   0x18015cc40
   18025ea90:	84 c0                	test   al,al
   18025ea92:	74 2a                	je     0x18025eabe
   18025ea94:	40 38 73 08          	cmp    BYTE PTR [rbx+0x8],sil
   18025ea98:	74 1b                	je     0x18025eab5
   18025ea9a:	e8 61 44 ef ff       	call   0x180152f00
   18025ea9f:	c6 40 15 01          	mov    BYTE PTR [rax+0x15],0x1
   18025eaa3:	33 c0                	xor    eax,eax
   18025eaa5:	48 8b 5c 24 40       	mov    rbx,QWORD PTR [rsp+0x40]
   18025eaaa:	48 8b 74 24 48       	mov    rsi,QWORD PTR [rsp+0x48]
   18025eaaf:	48 83 c4 30          	add    rsp,0x30
   18025eab3:	5f                   	pop    rdi
   18025eab4:	c3                   	ret
   18025eab5:	e8 46 44 ef ff       	call   0x180152f00
   18025eaba:	40 88 70 15          	mov    BYTE PTR [rax+0x15],sil
   18025eabe:	48 8b 5c 24 40       	mov    rbx,QWORD PTR [rsp+0x40]
   18025eac3:	33 c0                	xor    eax,eax
   18025eac5:	48 8b 74 24 48       	mov    rsi,QWORD PTR [rsp+0x48]
   18025eaca:	48 83 c4 30          	add    rsp,0x30
   18025eace:	5f                   	pop    rdi
   18025eacf:	c3                   	ret
   18025ead0:	cc                   	int3
   18025ead1:	cc                   	int3
   18025ead2:	cc                   	int3
   18025ead3:	cc                   	int3
   18025ead4:	cc                   	int3
   18025ead5:	cc                   	int3
   18025ead6:	cc                   	int3
   18025ead7:	cc                   	int3
   18025ead8:	cc                   	int3
   18025ead9:	cc                   	int3
   18025eada:	cc                   	int3
   18025eadb:	cc                   	int3
   18025eadc:	cc                   	int3
   18025eadd:	cc                   	int3
   18025eade:	cc                   	int3
   18025eadf:	cc                   	int3
   18025eae0:	48 89 5c 24 08       	mov    QWORD PTR [rsp+0x8],rbx
   18025eae5:	55                   	push   rbp
   18025eae6:	48 8b ec             	mov    rbp,rsp
   18025eae9:	48 83 ec 70          	sub    rsp,0x70
   18025eaed:	83 7a 08 08          	cmp    DWORD PTR [rdx+0x8],0x8
   18025eaf1:	48 8b d9             	mov    rbx,rcx
   18025eaf4:	0f 85 03 01 00 00    	jne    0x18025ebfd
   18025eafa:	48 8d 0d 97 6a 18 00 	lea    rcx,[rip+0x186a97]        # 0x1803e5598
   18025eb01:	e8 6a 18 f5 ff       	call   0x1801b0370
   18025eb06:	48 89 03             	mov    QWORD PTR [rbx],rax
   18025eb09:	48 85 c0             	test   rax,rax
   18025eb0c:	0f 84 86 00 00 00    	je     0x18025eb98
   18025eb12:	48 8b c8             	mov    rcx,rax
   18025eb15:	e8 d6 18 f5 ff       	call   0x1801b03f0
   18025eb1a:	84 c0                	test   al,al
   18025eb1c:	75 65                	jne    0x18025eb83
   18025eb1e:	e8 2d 35 fc ff       	call   0x180222050
   18025eb23:	48 8d 0d be 6a 18 00 	lea    rcx,[rip+0x186abe]        # 0x1803e55e8
   18025eb2a:	c7 45 c8 0f 00 00 00 	mov    DWORD PTR [rbp-0x38],0xf
   18025eb31:	48 89 4d c0          	mov    QWORD PTR [rbp-0x40],rcx
   18025eb35:	4c 8d 4d b0          	lea    r9,[rbp-0x50]
   18025eb39:	8b 4d ec             	mov    ecx,DWORD PTR [rbp-0x14]
   18025eb3c:	48 8d 55 e0          	lea    rdx,[rbp-0x20]
   18025eb40:	89 4d cc             	mov    DWORD PTR [rbp-0x34],ecx
   18025eb43:	41 b8 02 00 00 00    	mov    r8d,0x2
   18025eb49:	c5 f8 10 45 c0       	vmovups xmm0,XMMWORD PTR [rbp-0x40]
   18025eb4e:	48 8d 0d 6b 6a 18 00 	lea    rcx,[rip+0x186a6b]        # 0x1803e55c0
   18025eb55:	48 c7 45 b8 1d 00 00 	mov    QWORD PTR [rbp-0x48],0x1d
   18025eb5c:	00 
   18025eb5d:	48 89 4d d0          	mov    QWORD PTR [rbp-0x30],rcx
   18025eb61:	48 8d 0d f0 6a 18 00 	lea    rcx,[rip+0x186af0]        # 0x1803e5658
   18025eb68:	c5 fb 10 4d d0       	vmovsd xmm1,QWORD PTR [rbp-0x30]
   18025eb6d:	48 89 4d b0          	mov    QWORD PTR [rbp-0x50],rcx
   18025eb71:	48 8b c8             	mov    rcx,rax
   18025eb74:	c5 f8 11 45 e0       	vmovups XMMWORD PTR [rbp-0x20],xmm0
   18025eb79:	c5 fb 11 4d f0       	vmovsd QWORD PTR [rbp-0x10],xmm1
   18025eb7e:	e8 0d 2e ef ff       	call   0x180151990
   18025eb83:	48 8b 03             	mov    rax,QWORD PTR [rbx]
   18025eb86:	c6 40 08 01          	mov    BYTE PTR [rax+0x8],0x1
   18025eb8a:	48 8b 9c 24 80 00 00 	mov    rbx,QWORD PTR [rsp+0x80]
   18025eb91:	00 
   18025eb92:	48 83 c4 70          	add    rsp,0x70
   18025eb96:	5d                   	pop    rbp
   18025eb97:	c3                   	ret
   18025eb98:	e8 b3 34 fc ff       	call   0x180222050
   18025eb9d:	48 8d 0d 44 6a 18 00 	lea    rcx,[rip+0x186a44]        # 0x1803e55e8
   18025eba4:	c7 45 c8 12 00 00 00 	mov    DWORD PTR [rbp-0x38],0x12
   18025ebab:	48 89 4d c0          	mov    QWORD PTR [rbp-0x40],rcx
   18025ebaf:	4c 8d 4d b0          	lea    r9,[rbp-0x50]
   18025ebb3:	8b 4d ec             	mov    ecx,DWORD PTR [rbp-0x14]
   18025ebb6:	48 8d 55 e0          	lea    rdx,[rbp-0x20]
   18025ebba:	89 4d cc             	mov    DWORD PTR [rbp-0x34],ecx
   18025ebbd:	41 b8 03 00 00 00    	mov    r8d,0x3
   18025ebc3:	c5 f8 10 45 c0       	vmovups xmm0,XMMWORD PTR [rbp-0x40]
   18025ebc8:	48 8d 0d f1 69 18 00 	lea    rcx,[rip+0x1869f1]        # 0x1803e55c0
   18025ebcf:	48 c7 45 b8 3c 00 00 	mov    QWORD PTR [rbp-0x48],0x3c
   18025ebd6:	00 
   18025ebd7:	48 89 4d d0          	mov    QWORD PTR [rbp-0x30],rcx
   18025ebdb:	48 8d 0d 36 6a 18 00 	lea    rcx,[rip+0x186a36]        # 0x1803e5618
   18025ebe2:	c5 fb 10 4d d0       	vmovsd xmm1,QWORD PTR [rbp-0x30]
   18025ebe7:	48 89 4d b0          	mov    QWORD PTR [rbp-0x50],rcx
   18025ebeb:	48 8b c8             	mov    rcx,rax
   18025ebee:	c5 f8 11 45 e0       	vmovups XMMWORD PTR [rbp-0x20],xmm0
   18025ebf3:	c5 fb 11 4d f0       	vmovsd QWORD PTR [rbp-0x10],xmm1
   18025ebf8:	e8 93 2d ef ff       	call   0x180151990
   18025ebfd:	48 8b 9c 24 80 00 00 	mov    rbx,QWORD PTR [rsp+0x80]
   18025ec04:	00 
   18025ec05:	48 83 c4 70          	add    rsp,0x70
   18025ec09:	5d                   	pop    rbp
   18025ec0a:	c3                   	ret
   18025ec0b:	cc                   	int3
   18025ec0c:	cc                   	int3
   18025ec0d:	cc                   	int3
   18025ec0e:	cc                   	int3
   18025ec0f:	cc                   	int3
   18025ec10:	cc                   	int3
   18025ec11:	cc                   	int3
   18025ec12:	cc                   	int3
   18025ec13:	cc                   	int3
   18025ec14:	cc                   	int3
   18025ec15:	cc                   	int3
   18025ec16:	cc                   	int3
   18025ec17:	cc                   	int3
   18025ec18:	cc                   	int3
   18025ec19:	cc                   	int3
   18025ec1a:	cc                   	int3
   18025ec1b:	cc                   	int3
   18025ec1c:	cc                   	int3
   18025ec1d:	cc                   	int3
   18025ec1e:	cc                   	int3
   18025ec1f:	cc                   	int3
   18025ec20:	48 89 5c 24 08       	mov    QWORD PTR [rsp+0x8],rbx
   18025ec25:	57                   	push   rdi
   18025ec26:	48 83 ec 20          	sub    rsp,0x20
   18025ec2a:	48 8b fa             	mov    rdi,rdx
   18025ec2d:	48 8b d9             	mov    rbx,rcx
