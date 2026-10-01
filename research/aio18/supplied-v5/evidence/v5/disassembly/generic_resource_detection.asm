
/mnt/data/aio18_work/extracted/SKSE/Plugins/SkyrimUpscaler.dll:     file format pei-x86-64


Disassembly of section .text:

00000001802a8850 <.text+0x2a7850>:
   1802a8850:	49 0f af d7          	imul   rdx,r15
   1802a8854:	48 23 15 95 90 bd 00 	and    rdx,QWORD PTR [rip+0xbd9095]        # 0x180e818f0
   1802a885b:	48 03 d2             	add    rdx,rdx
   1802a885e:	48 8b 4c d0 08       	mov    rcx,QWORD PTR [rax+rdx*8+0x8]
   1802a8863:	49 3b c9             	cmp    rcx,r9
   1802a8866:	74 1b                	je     0x1802a8883
   1802a8868:	48 8b 04 d0          	mov    rax,QWORD PTR [rax+rdx*8]
   1802a886c:	4c 3b 41 10          	cmp    r8,QWORD PTR [rcx+0x10]
   1802a8870:	74 13                	je     0x1802a8885
   1802a8872:	48 3b c8             	cmp    rcx,rax
   1802a8875:	74 0c                	je     0x1802a8883
   1802a8877:	48 8b 49 08          	mov    rcx,QWORD PTR [rcx+0x8]
   1802a887b:	4c 3b 41 10          	cmp    r8,QWORD PTR [rcx+0x10]
   1802a887f:	75 f1                	jne    0x1802a8872
   1802a8881:	eb 02                	jmp    0x1802a8885
   1802a8883:	33 c9                	xor    ecx,ecx
   1802a8885:	48 85 c9             	test   rcx,rcx
   1802a8888:	49 8b c1             	mov    rax,r9
   1802a888b:	48 0f 45 c1          	cmovne rax,rcx
   1802a888f:	49 3b c1             	cmp    rax,r9
   1802a8892:	0f 85 87 00 00 00    	jne    0x1802a891f
   1802a8898:	49 8b 00             	mov    rax,QWORD PTR [r8]
   1802a889b:	48 8d 55 c7          	lea    rdx,[rbp-0x39]
   1802a889f:	49 8b c8             	mov    rcx,r8
   1802a88a2:	ff 50 38             	call   QWORD PTR [rax+0x38]
   1802a88a5:	c5 fa 10 45 d7       	vmovss xmm0,DWORD PTR [rbp-0x29]
   1802a88aa:	c5 f8 2e c6          	vucomiss xmm0,xmm6
   1802a88ae:	0f 85 bf 00 00 00    	jne    0x1802a8973
   1802a88b4:	83 7d db 01          	cmp    DWORD PTR [rbp-0x25],0x1
   1802a88b8:	0f 86 b5 00 00 00    	jbe    0x1802a8973
   1802a88be:	48 8b 05 1b 85 bd 00 	mov    rax,QWORD PTR [rip+0xbd851b]        # 0x180e80de0
   1802a88c5:	4c 8d 44 24 30       	lea    r8,[rsp+0x30]
   1802a88ca:	c5 fa 10 05 c2 87 bd 	vmovss xmm0,DWORD PTR [rip+0xbd87c2]        # 0x180e81094
   1802a88d1:	00 
   1802a88d2:	c5 fa 11 45 d7       	vmovss DWORD PTR [rbp-0x29],xmm0
   1802a88d7:	48 8d 55 ff          	lea    rdx,[rbp-0x1]
   1802a88db:	48 8b b8 78 16 00 00 	mov    rdi,QWORD PTR [rax+0x1678]
   1802a88e2:	48 8b 07             	mov    rax,QWORD PTR [rdi]
   1802a88e5:	48 8b 98 b8 00 00 00 	mov    rbx,QWORD PTR [rax+0xb8]
   1802a88ec:	e8 1f c0 ef ff       	call   0x1801a4910
   1802a88f1:	48 8d 55 c7          	lea    rdx,[rbp-0x39]
   1802a88f5:	48 8b cf             	mov    rcx,rdi
   1802a88f8:	4c 8b 00             	mov    r8,QWORD PTR [rax]
   1802a88fb:	49 83 c0 18          	add    r8,0x18
   1802a88ff:	ff d3                	call   rbx
   1802a8901:	4c 8d 44 24 30       	lea    r8,[rsp+0x30]
   1802a8906:	48 8d 55 0f          	lea    rdx,[rbp+0xf]
   1802a890a:	e8 01 c0 ef ff       	call   0x1801a4910
   1802a890f:	48 8d 55 1f          	lea    rdx,[rbp+0x1f]
   1802a8913:	4c 8b 00             	mov    r8,QWORD PTR [rax]
   1802a8916:	49 83 c0 18          	add    r8,0x18
   1802a891a:	e8 f1 c2 ef ff       	call   0x1801a4c10
   1802a891f:	4c 8d 44 24 30       	lea    r8,[rsp+0x30]
   1802a8924:	48 8d 55 87          	lea    rdx,[rbp-0x79]
   1802a8928:	e8 e3 bf ef ff       	call   0x1801a4910
   1802a892d:	48 8b 08             	mov    rcx,QWORD PTR [rax]
   1802a8930:	48 8b 41 18          	mov    rax,QWORD PTR [rcx+0x18]
   1802a8934:	48 89 06             	mov    QWORD PTR [rsi],rax
   1802a8937:	48 83 c6 08          	add    rsi,0x8
   1802a893b:	49 83 ee 01          	sub    r14,0x1
   1802a893f:	0f 85 5b fd ff ff    	jne    0x1802a86a0
   1802a8945:	c5 f8 28 b4 24 f0 00 	vmovaps xmm6,XMMWORD PTR [rsp+0xf0]
   1802a894c:	00 00 
   1802a894e:	4c 8b a4 24 20 01 00 	mov    r12,QWORD PTR [rsp+0x120]
   1802a8955:	00 
   1802a8956:	4c 8d 9c 24 00 01 00 	lea    r11,[rsp+0x100]
   1802a895d:	00 
   1802a895e:	49 8b 5b 28          	mov    rbx,QWORD PTR [r11+0x28]
   1802a8962:	49 8b 73 30          	mov    rsi,QWORD PTR [r11+0x30]
   1802a8966:	49 8b 7b 38          	mov    rdi,QWORD PTR [r11+0x38]
   1802a896a:	49 8b e3             	mov    rsp,r11
   1802a896d:	41 5f                	pop    r15
   1802a896f:	41 5e                	pop    r14
   1802a8971:	5d                   	pop    rbp
   1802a8972:	c3                   	ret
   1802a8973:	4c 8d 44 24 30       	lea    r8,[rsp+0x30]
   1802a8978:	48 8d 55 a7          	lea    rdx,[rbp-0x59]
   1802a897c:	e8 8f c2 ef ff       	call   0x1801a4c10
   1802a8981:	eb b4                	jmp    0x1802a8937
   1802a8983:	cc                   	int3
   1802a8984:	cc                   	int3
   1802a8985:	cc                   	int3
   1802a8986:	cc                   	int3
   1802a8987:	cc                   	int3
   1802a8988:	cc                   	int3
   1802a8989:	cc                   	int3
   1802a898a:	cc                   	int3
   1802a898b:	cc                   	int3
   1802a898c:	cc                   	int3
   1802a898d:	cc                   	int3
   1802a898e:	cc                   	int3
   1802a898f:	cc                   	int3
   1802a8990:	48 89 5c 24 08       	mov    QWORD PTR [rsp+0x8],rbx
   1802a8995:	48 89 74 24 18       	mov    QWORD PTR [rsp+0x18],rsi
   1802a899a:	48 89 7c 24 20       	mov    QWORD PTR [rsp+0x20],rdi
   1802a899f:	55                   	push   rbp
   1802a89a0:	41 56                	push   r14
   1802a89a2:	41 57                	push   r15
   1802a89a4:	48 8d 6c 24 b9       	lea    rbp,[rsp-0x47]
   1802a89a9:	48 81 ec a0 00 00 00 	sub    rsp,0xa0
   1802a89b0:	48 8b 05 d1 4a 1d 00 	mov    rax,QWORD PTR [rip+0x1d4ad1]        # 0x18047d488
   1802a89b7:	49 8b f9             	mov    rdi,r9
   1802a89ba:	48 8b f2             	mov    rsi,rdx
   1802a89bd:	ff d0                	call   rax
   1802a89bf:	8b 56 10             	mov    edx,DWORD PTR [rsi+0x10]
   1802a89c2:	44 8b f8             	mov    r15d,eax
   1802a89c5:	8d 4a df             	lea    ecx,[rdx-0x21]
   1802a89c8:	83 f9 05             	cmp    ecx,0x5
   1802a89cb:	0f 87 69 01 00 00    	ja     0x1802a8b3a
   1802a89d1:	4c 8b 05 10 84 bd 00 	mov    r8,QWORD PTR [rip+0xbd8410]        # 0x180e80de8
   1802a89d8:	32 c9                	xor    cl,cl
   1802a89da:	4d 8b 58 20          	mov    r11,QWORD PTR [r8+0x20]
   1802a89de:	4d 8d 48 20          	lea    r9,[r8+0x20]
   1802a89e2:	49 8b 51 08          	mov    rdx,QWORD PTR [r9+0x8]
   1802a89e6:	49 8b c3             	mov    rax,r11
   1802a89e9:	bb 01 00 00 00       	mov    ebx,0x1
   1802a89ee:	4c 3b da             	cmp    r11,rdx
   1802a89f1:	74 28                	je     0x1802a8a1b
   1802a89f3:	4c 8b 17             	mov    r10,QWORD PTR [rdi]
   1802a89f6:	66 66 0f 1f 84 00 00 	data16 nop WORD PTR [rax+rax*1+0x0]
   1802a89fd:	00 00 00 
   1802a8a00:	4c 39 50 08          	cmp    QWORD PTR [rax+0x8],r10
   1802a8a04:	0f b6 c9             	movzx  ecx,cl
   1802a8a07:	0f 44 cb             	cmove  ecx,ebx
   1802a8a0a:	48 83 c0 40          	add    rax,0x40
   1802a8a0e:	48 3b c2             	cmp    rax,rdx
   1802a8a11:	75 ed                	jne    0x1802a8a00
   1802a8a13:	84 c9                	test   cl,cl
   1802a8a15:	0f 85 08 01 00 00    	jne    0x1802a8b23
   1802a8a1b:	48 8b 07             	mov    rax,QWORD PTR [rdi]
   1802a8a1e:	48 89 45 0f          	mov    QWORD PTR [rbp+0xf],rax
   1802a8a22:	8b 46 28             	mov    eax,DWORD PTR [rsi+0x28]
   1802a8a25:	89 45 3f             	mov    DWORD PTR [rbp+0x3f],eax
   1802a8a28:	89 5d 07             	mov    DWORD PTR [rbp+0x7],ebx
   1802a8a2b:	c5 fc 10 06          	vmovups ymm0,YMMWORD PTR [rsi]
   1802a8a2f:	c5 fb 10 4e 20       	vmovsd xmm1,QWORD PTR [rsi+0x20]
   1802a8a34:	c5 fc 11 45 17       	vmovups YMMWORD PTR [rbp+0x17],ymm0
   1802a8a39:	c5 fb 11 4d 37       	vmovsd QWORD PTR [rbp+0x37],xmm1
   1802a8a3e:	49 3b d3             	cmp    rdx,r11
   1802a8a41:	0f 85 dc 00 00 00    	jne    0x1802a8b23
   1802a8a47:	49 3b 51 10          	cmp    rdx,QWORD PTR [r9+0x10]
   1802a8a4b:	74 1a                	je     0x1802a8a67
   1802a8a4d:	c5 fc 10 45 07       	vmovups ymm0,YMMWORD PTR [rbp+0x7]
   1802a8a52:	c5 fc 10 4d 27       	vmovups ymm1,YMMWORD PTR [rbp+0x27]
   1802a8a57:	c5 fc 11 02          	vmovups YMMWORD PTR [rdx],ymm0
   1802a8a5b:	c5 fc 11 4a 20       	vmovups YMMWORD PTR [rdx+0x20],ymm1
   1802a8a60:	49 83 41 08 40       	add    QWORD PTR [r9+0x8],0x40
   1802a8a65:	eb 20                	jmp    0x1802a8a87
   1802a8a67:	4c 8d 45 07          	lea    r8,[rbp+0x7]
   1802a8a6b:	49 8b c9             	mov    rcx,r9
   1802a8a6e:	c5 f8 77             	vzeroupper
   1802a8a71:	e8 ea c8 ef ff       	call   0x1801a5360
   1802a8a76:	c5 fc 10 4d 27       	vmovups ymm1,YMMWORD PTR [rbp+0x27]
   1802a8a7b:	c5 fc 10 45 07       	vmovups ymm0,YMMWORD PTR [rbp+0x7]
   1802a8a80:	4c 8b 05 61 83 bd 00 	mov    r8,QWORD PTR [rip+0xbd8361]        # 0x180e80de8
   1802a8a87:	48 8b 05 52 83 bd 00 	mov    rax,QWORD PTR [rip+0xbd8352]        # 0x180e80de0
   1802a8a8e:	48 83 b8 28 08 00 00 	cmp    QWORD PTR [rax+0x828],0x0
   1802a8a95:	00 
   1802a8a96:	75 18                	jne    0x1802a8ab0
   1802a8a98:	c4 c1 7c 11 40 38    	vmovups YMMWORD PTR [r8+0x38],ymm0
   1802a8a9e:	c4 c1 7c 11 48 58    	vmovups YMMWORD PTR [r8+0x58],ymm1
   1802a8aa4:	49 8b 50 40          	mov    rdx,QWORD PTR [r8+0x40]
   1802a8aa8:	c5 f8 77             	vzeroupper
   1802a8aab:	e8 70 ad fe ff       	call   0x180293820
   1802a8ab0:	c5 f8 77             	vzeroupper
   1802a8ab3:	e8 98 95 f7 ff       	call   0x180222050
   1802a8ab8:	4c 8b c0             	mov    r8,rax
   1802a8abb:	c7 45 ef 9e 03 00 00 	mov    DWORD PTR [rbp-0x11],0x39e
   1802a8ac2:	48 8d 0d 4f 11 16 00 	lea    rcx,[rip+0x16114f]        # 0x180409c18
   1802a8ac9:	48 c7 45 df 1d 00 00 	mov    QWORD PTR [rbp-0x21],0x1d
   1802a8ad0:	00 
   1802a8ad1:	48 89 4d e7          	mov    QWORD PTR [rbp-0x19],rcx
   1802a8ad5:	48 8d 05 8c 15 16 00 	lea    rax,[rip+0x16158c]        # 0x18040a068
   1802a8adc:	8b 4d 13             	mov    ecx,DWORD PTR [rbp+0x13]
   1802a8adf:	4c 8d 4d d7          	lea    r9,[rbp-0x29]
   1802a8ae3:	89 4d f3             	mov    DWORD PTR [rbp-0xd],ecx
   1802a8ae6:	48 8d 55 07          	lea    rdx,[rbp+0x7]
   1802a8aea:	c5 f8 10 45 e7       	vmovups xmm0,XMMWORD PTR [rbp-0x19]
   1802a8aef:	48 8d 0d 8a 14 16 00 	lea    rcx,[rip+0x16148a]        # 0x180409f80
   1802a8af6:	48 89 45 d7          	mov    QWORD PTR [rbp-0x29],rax
   1802a8afa:	48 89 4d f7          	mov    QWORD PTR [rbp-0x9],rcx
   1802a8afe:	48 8d 46 04          	lea    rax,[rsi+0x4]
   1802a8b02:	c5 fb 10 4d f7       	vmovsd xmm1,QWORD PTR [rbp-0x9]
   1802a8b07:	48 89 44 24 28       	mov    QWORD PTR [rsp+0x28],rax
   1802a8b0c:	49 8b c8             	mov    rcx,r8
   1802a8b0f:	c5 fb 11 4d 17       	vmovsd QWORD PTR [rbp+0x17],xmm1
   1802a8b14:	c5 f8 11 45 07       	vmovups XMMWORD PTR [rbp+0x7],xmm0
   1802a8b19:	48 89 74 24 20       	mov    QWORD PTR [rsp+0x20],rsi
   1802a8b1e:	e8 dd d9 ef ff       	call   0x1801a6500
   1802a8b23:	45 85 ff             	test   r15d,r15d
   1802a8b26:	0f 88 d5 01 00 00    	js     0x1802a8d01
   1802a8b2c:	48 85 ff             	test   rdi,rdi
   1802a8b2f:	0f 84 cc 01 00 00    	je     0x1802a8d01
   1802a8b35:	e9 d8 00 00 00       	jmp    0x1802a8c12
   1802a8b3a:	45 85 ff             	test   r15d,r15d
   1802a8b3d:	0f 88 be 01 00 00    	js     0x1802a8d01
   1802a8b43:	48 85 ff             	test   rdi,rdi
   1802a8b46:	0f 84 b5 01 00 00    	je     0x1802a8d01
   1802a8b4c:	48 8b 0f             	mov    rcx,QWORD PTR [rdi]
   1802a8b4f:	48 85 c9             	test   rcx,rcx
   1802a8b52:	0f 84 ba 00 00 00    	je     0x1802a8c12
   1802a8b58:	8d 42 d4             	lea    eax,[rdx-0x2c]
   1802a8b5b:	83 f8 03             	cmp    eax,0x3
   1802a8b5e:	76 0c                	jbe    0x1802a8b6c
   1802a8b60:	8d 42 ed             	lea    eax,[rdx-0x13]
   1802a8b63:	83 f8 03             	cmp    eax,0x3
   1802a8b66:	0f 87 a6 00 00 00    	ja     0x1802a8c12
   1802a8b6c:	48 8b 1d 6d 82 bd 00 	mov    rbx,QWORD PTR [rip+0xbd826d]        # 0x180e80de0
   1802a8b73:	8b 83 78 02 00 00    	mov    eax,DWORD PTR [rbx+0x278]
   1802a8b79:	39 06                	cmp    DWORD PTR [rsi],eax
   1802a8b7b:	0f 85 98 00 00 00    	jne    0x1802a8c19
   1802a8b81:	8b 83 7c 02 00 00    	mov    eax,DWORD PTR [rbx+0x27c]
   1802a8b87:	4c 8d 76 04          	lea    r14,[rsi+0x4]
   1802a8b8b:	41 39 06             	cmp    DWORD PTR [r14],eax
   1802a8b8e:	0f 85 85 00 00 00    	jne    0x1802a8c19
   1802a8b94:	48 83 bb d0 07 00 00 	cmp    QWORD PTR [rbx+0x7d0],0x0
   1802a8b9b:	00 
   1802a8b9c:	75 7b                	jne    0x1802a8c19
   1802a8b9e:	48 8b d1             	mov    rdx,rcx
   1802a8ba1:	48 8b cb             	mov    rcx,rbx
   1802a8ba4:	e8 87 b0 fe ff       	call   0x180293c30
   1802a8ba9:	e8 a2 94 f7 ff       	call   0x180222050
   1802a8bae:	48 8d 0d 63 10 16 00 	lea    rcx,[rip+0x161063]        # 0x180409c18
   1802a8bb5:	c7 45 ef a8 03 00 00 	mov    DWORD PTR [rbp-0x11],0x3a8
   1802a8bbc:	48 89 4d e7          	mov    QWORD PTR [rbp-0x19],rcx
   1802a8bc0:	4c 8d 4d d7          	lea    r9,[rbp-0x29]
   1802a8bc4:	8b 4d 13             	mov    ecx,DWORD PTR [rbp+0x13]
   1802a8bc7:	48 8d 55 07          	lea    rdx,[rbp+0x7]
   1802a8bcb:	89 4d f3             	mov    DWORD PTR [rbp-0xd],ecx
   1802a8bce:	48 8d 0d ab 13 16 00 	lea    rcx,[rip+0x1613ab]        # 0x180409f80
   1802a8bd5:	c5 f8 10 45 e7       	vmovups xmm0,XMMWORD PTR [rbp-0x19]
   1802a8bda:	48 89 4d f7          	mov    QWORD PTR [rbp-0x9],rcx
   1802a8bde:	48 8d 0d 63 14 16 00 	lea    rcx,[rip+0x161463]        # 0x18040a048
   1802a8be5:	c5 fb 10 4d f7       	vmovsd xmm1,QWORD PTR [rbp-0x9]
   1802a8bea:	48 89 4d d7          	mov    QWORD PTR [rbp-0x29],rcx
   1802a8bee:	48 8b c8             	mov    rcx,rax
   1802a8bf1:	4c 89 74 24 28       	mov    QWORD PTR [rsp+0x28],r14
   1802a8bf6:	48 c7 45 df 1c 00 00 	mov    QWORD PTR [rbp-0x21],0x1c
   1802a8bfd:	00 
   1802a8bfe:	c5 f8 11 45 07       	vmovups XMMWORD PTR [rbp+0x7],xmm0
   1802a8c03:	c5 fb 11 4d 17       	vmovsd QWORD PTR [rbp+0x17],xmm1
   1802a8c08:	48 89 74 24 20       	mov    QWORD PTR [rsp+0x20],rsi
   1802a8c0d:	e8 ee d8 ef ff       	call   0x1801a6500
   1802a8c12:	48 8b 1d c7 81 bd 00 	mov    rbx,QWORD PTR [rip+0xbd81c7]        # 0x180e80de0
   1802a8c19:	48 8b 3f             	mov    rdi,QWORD PTR [rdi]
   1802a8c1c:	48 85 ff             	test   rdi,rdi
   1802a8c1f:	0f 84 dc 00 00 00    	je     0x1802a8d01
   1802a8c25:	33 c0                	xor    eax,eax
   1802a8c27:	48 89 7d 6f          	mov    QWORD PTR [rbp+0x6f],rdi
   1802a8c2b:	c5 f9 ef c0          	vpxor  xmm0,xmm0,xmm0
   1802a8c2f:	c5 fc 11 45 07       	vmovups YMMWORD PTR [rbp+0x7],ymm0
   1802a8c34:	48 89 45 27          	mov    QWORD PTR [rbp+0x27],rax
   1802a8c38:	48 8d 55 07          	lea    rdx,[rbp+0x7]
   1802a8c3c:	89 45 2f             	mov    DWORD PTR [rbp+0x2f],eax
   1802a8c3f:	48 8b cf             	mov    rcx,rdi
   1802a8c42:	48 8b 07             	mov    rax,QWORD PTR [rdi]
   1802a8c45:	c5 f8 77             	vzeroupper
   1802a8c48:	ff 50 50             	call   QWORD PTR [rax+0x50]
   1802a8c4b:	8b 45 17             	mov    eax,DWORD PTR [rbp+0x17]
   1802a8c4e:	83 f8 0a             	cmp    eax,0xa
   1802a8c51:	74 0c                	je     0x1802a8c5f
   1802a8c53:	83 c0 e5             	add    eax,0xffffffe5
   1802a8c56:	83 f8 01             	cmp    eax,0x1
   1802a8c59:	0f 87 a2 00 00 00    	ja     0x1802a8d01
   1802a8c5f:	f6 45 27 20          	test   BYTE PTR [rbp+0x27],0x20
   1802a8c63:	0f 84 98 00 00 00    	je     0x1802a8d01
   1802a8c69:	48 8b 83 80 15 00 00 	mov    rax,QWORD PTR [rbx+0x1580]
   1802a8c70:	48 8b 8b 88 15 00 00 	mov    rcx,QWORD PTR [rbx+0x1588]
   1802a8c77:	48 3b c1             	cmp    rax,rcx
   1802a8c7a:	74 12                	je     0x1802a8c8e
   1802a8c7c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
   1802a8c80:	48 39 38             	cmp    QWORD PTR [rax],rdi
   1802a8c83:	74 7c                	je     0x1802a8d01
   1802a8c85:	48 83 c0 08          	add    rax,0x8
   1802a8c89:	48 3b c1             	cmp    rax,rcx
   1802a8c8c:	75 f2                	jne    0x1802a8c80
   1802a8c8e:	48 8b 8b 80 15 00 00 	mov    rcx,QWORD PTR [rbx+0x1580]
   1802a8c95:	48 8b 83 88 15 00 00 	mov    rax,QWORD PTR [rbx+0x1588]
   1802a8c9c:	48 2b c1             	sub    rax,rcx
   1802a8c9f:	48 c1 f8 03          	sar    rax,0x3
   1802a8ca3:	48 83 f8 40          	cmp    rax,0x40
   1802a8ca7:	72 2b                	jb     0x1802a8cd4
   1802a8ca9:	48 8b 09             	mov    rcx,QWORD PTR [rcx]
   1802a8cac:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   1802a8caf:	ff 50 10             	call   QWORD PTR [rax+0x10]
   1802a8cb2:	48 8b 8b 80 15 00 00 	mov    rcx,QWORD PTR [rbx+0x1580]
   1802a8cb9:	4c 8b 83 88 15 00 00 	mov    r8,QWORD PTR [rbx+0x1588]
   1802a8cc0:	48 8d 51 08          	lea    rdx,[rcx+0x8]
   1802a8cc4:	4c 2b c2             	sub    r8,rdx
   1802a8cc7:	e8 66 1b f9 ff       	call   0x18023a832
   1802a8ccc:	48 83 83 88 15 00 00 	add    QWORD PTR [rbx+0x1588],0xfffffffffffffff8
   1802a8cd3:	f8 
   1802a8cd4:	48 8b 07             	mov    rax,QWORD PTR [rdi]
   1802a8cd7:	48 8b cf             	mov    rcx,rdi
   1802a8cda:	ff 50 08             	call   QWORD PTR [rax+0x8]
   1802a8cdd:	48 8d 8b 80 15 00 00 	lea    rcx,[rbx+0x1580]
   1802a8ce4:	48 8b 51 08          	mov    rdx,QWORD PTR [rcx+0x8]
   1802a8ce8:	48 3b 51 10          	cmp    rdx,QWORD PTR [rcx+0x10]
   1802a8cec:	74 0a                	je     0x1802a8cf8
   1802a8cee:	48 89 3a             	mov    QWORD PTR [rdx],rdi
   1802a8cf1:	48 83 41 08 08       	add    QWORD PTR [rcx+0x8],0x8
   1802a8cf6:	eb 09                	jmp    0x1802a8d01
   1802a8cf8:	4c 8d 45 6f          	lea    r8,[rbp+0x6f]
   1802a8cfc:	e8 cf a1 ee ff       	call   0x180192ed0
   1802a8d01:	41 8b c7             	mov    eax,r15d
   1802a8d04:	c5 f8 77             	vzeroupper
   1802a8d07:	4c 8d 9c 24 a0 00 00 	lea    r11,[rsp+0xa0]
   1802a8d0e:	00 
   1802a8d0f:	49 8b 5b 20          	mov    rbx,QWORD PTR [r11+0x20]
   1802a8d13:	49 8b 73 30          	mov    rsi,QWORD PTR [r11+0x30]
   1802a8d17:	49 8b 7b 38          	mov    rdi,QWORD PTR [r11+0x38]
   1802a8d1b:	49 8b e3             	mov    rsp,r11
   1802a8d1e:	41 5f                	pop    r15
   1802a8d20:	41 5e                	pop    r14
   1802a8d22:	5d                   	pop    rbp
   1802a8d23:	c3                   	ret
   1802a8d24:	cc                   	int3
   1802a8d25:	cc                   	int3
   1802a8d26:	cc                   	int3
   1802a8d27:	cc                   	int3
   1802a8d28:	cc                   	int3
   1802a8d29:	cc                   	int3
   1802a8d2a:	cc                   	int3
   1802a8d2b:	cc                   	int3
   1802a8d2c:	cc                   	int3
   1802a8d2d:	cc                   	int3
   1802a8d2e:	cc                   	int3
   1802a8d2f:	cc                   	int3
   1802a8d30:	48 85 d2             	test   rdx,rdx
   1802a8d33:	0f 84 e8 00 00 00    	je     0x1802a8e21
   1802a8d39:	48 89 5c 24 08       	mov    QWORD PTR [rsp+0x8],rbx
   1802a8d3e:	57                   	push   rdi
   1802a8d3f:	48 81 ec 30 02 00 00 	sub    rsp,0x230
   1802a8d46:	48 8b fa             	mov    rdi,rdx
   1802a8d49:	48 8b d9             	mov    rbx,rcx
   1802a8d4c:	ff 15 2e 47 1d 00    	call   QWORD PTR [rip+0x1d472e]        # 0x18047d480
   1802a8d52:	48 3b 1d ff 81 bd 00 	cmp    rbx,QWORD PTR [rip+0xbd81ff]        # 0x180e80f58
   1802a8d59:	0f 85 b2 00 00 00    	jne    0x1802a8e11
   1802a8d5f:	48 8b 05 7a 80 bd 00 	mov    rax,QWORD PTR [rip+0xbd807a]        # 0x180e80de0
   1802a8d66:	80 b8 e4 04 00 00 00 	cmp    BYTE PTR [rax+0x4e4],0x0
   1802a8d6d:	0f 84 9e 00 00 00    	je     0x1802a8e11
   1802a8d73:	48 8b 94 24 38 02 00 	mov    rdx,QWORD PTR [rsp+0x238]
   1802a8d7a:	00 
   1802a8d7b:	4c                   	rex.WR
   1802a8d7c:	8d                   	.byte 0x8d
   1802a8d7d:	84 24 48             	test   BYTE PTR [rax+rcx*2],ah
