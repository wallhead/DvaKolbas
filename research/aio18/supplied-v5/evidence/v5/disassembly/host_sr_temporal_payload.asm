
/mnt/data/aio18_work/extracted/SKSE/Plugins/SkyrimUpscaler.dll:     file format pei-x86-64


Disassembly of section .text:

00000001802949c0 <.text+0x2939c0>:
   1802949c0:	40 55                	rex push rbp
   1802949c2:	56                   	push   rsi
   1802949c3:	41 54                	push   r12
   1802949c5:	41 56                	push   r14
   1802949c7:	48 8d ac 24 98 fe ff 	lea    rbp,[rsp-0x168]
   1802949ce:	ff 
   1802949cf:	48 81 ec 68 02 00 00 	sub    rsp,0x268
   1802949d6:	4c 8b f1             	mov    r14,rcx
   1802949d9:	49 8b f0             	mov    rsi,r8
   1802949dc:	48 8d 4d 90          	lea    rcx,[rbp-0x70]
   1802949e0:	4c 8b e2             	mov    r12,rdx
   1802949e3:	e8 08 52 ee ff       	call   0x180179bf0
   1802949e8:	49 8b ce             	mov    rcx,r14
   1802949eb:	e8 60 1b 00 00       	call   0x180296550
   1802949f0:	84 c0                	test   al,al
   1802949f2:	75 5a                	jne    0x180294a4e
   1802949f4:	4d 85 e4             	test   r12,r12
   1802949f7:	0f 84 58 08 00 00    	je     0x180295255
   1802949fd:	48 85 f6             	test   rsi,rsi
   180294a00:	0f 84 4f 08 00 00    	je     0x180295255
   180294a06:	49 8b 04 24          	mov    rax,QWORD PTR [r12]
   180294a0a:	48 85 c0             	test   rax,rax
   180294a0d:	0f 84 42 08 00 00    	je     0x180295255
   180294a13:	48 8b 16             	mov    rdx,QWORD PTR [rsi]
   180294a16:	48 85 d2             	test   rdx,rdx
   180294a19:	0f 84 36 08 00 00    	je     0x180295255
   180294a1f:	49 83 be 80 16 00 00 	cmp    QWORD PTR [r14+0x1680],0x0
   180294a26:	00 
   180294a27:	0f 84 28 08 00 00    	je     0x180295255
   180294a2d:	48 3b c2             	cmp    rax,rdx
   180294a30:	0f 84 1f 08 00 00    	je     0x180295255
   180294a36:	4d 8b c4             	mov    r8,r12
   180294a39:	49 8b ce             	mov    rcx,r14
   180294a3c:	48 81 c4 68 02 00 00 	add    rsp,0x268
   180294a43:	41 5e                	pop    r14
   180294a45:	41 5c                	pop    r12
   180294a47:	5e                   	pop    rsi
   180294a48:	5d                   	pop    rbp
   180294a49:	e9 22 3d 00 00       	jmp    0x180298770
   180294a4e:	49 83 be 50 0b 00 00 	cmp    QWORD PTR [r14+0xb50],0x0
   180294a55:	00 
   180294a56:	48 89 9c 24 90 02 00 	mov    QWORD PTR [rsp+0x290],rbx
   180294a5d:	00 
   180294a5e:	48 89 bc 24 98 02 00 	mov    QWORD PTR [rsp+0x298],rdi
   180294a65:	00 
   180294a66:	4c 89 bc 24 60 02 00 	mov    QWORD PTR [rsp+0x260],r15
   180294a6d:	00 
   180294a6e:	0f 84 94 07 00 00    	je     0x180295208
   180294a74:	49 83 be a8 0b 00 00 	cmp    QWORD PTR [r14+0xba8],0x0
   180294a7b:	00 
   180294a7c:	0f 84 86 07 00 00    	je     0x180295208
   180294a82:	49 83 be d0 07 00 00 	cmp    QWORD PTR [r14+0x7d0],0x0
   180294a89:	00 
   180294a8a:	0f 84 78 07 00 00    	je     0x180295208
   180294a90:	49 83 be 28 08 00 00 	cmp    QWORD PTR [r14+0x828],0x0
   180294a97:	00 
   180294a98:	0f 84 6a 07 00 00    	je     0x180295208
   180294a9e:	49 8b 8e 80 16 00 00 	mov    rcx,QWORD PTR [r14+0x1680]
   180294aa5:	4c 8b 06             	mov    r8,QWORD PTR [rsi]
   180294aa8:	49 8b 96 f8 0a 00 00 	mov    rdx,QWORD PTR [r14+0xaf8]
   180294aaf:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   180294ab2:	ff 90 78 01 00 00    	call   QWORD PTR [rax+0x178]
   180294ab8:	49 8b 8e 80 16 00 00 	mov    rcx,QWORD PTR [r14+0x1680]
   180294abf:	4d 8b 86 d0 07 00 00 	mov    r8,QWORD PTR [r14+0x7d0]
   180294ac6:	49 8b 96 50 0b 00 00 	mov    rdx,QWORD PTR [r14+0xb50]
   180294acd:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   180294ad0:	ff 90 78 01 00 00    	call   QWORD PTR [rax+0x178]
   180294ad6:	49 8b 8e 80 16 00 00 	mov    rcx,QWORD PTR [r14+0x1680]
   180294add:	4d 8b 86 28 08 00 00 	mov    r8,QWORD PTR [r14+0x828]
   180294ae4:	49 8b 96 a8 0b 00 00 	mov    rdx,QWORD PTR [r14+0xba8]
   180294aeb:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   180294aee:	ff 90 78 01 00 00    	call   QWORD PTR [rax+0x178]
   180294af4:	41 80 be ad 04 00 00 	cmp    BYTE PTR [r14+0x4ad],0x0
   180294afb:	00 
   180294afc:	49 8d 9e 11 12 00 00 	lea    rbx,[r14+0x1211]
   180294b03:	75 05                	jne    0x180294b0a
   180294b05:	80 3b 00             	cmp    BYTE PTR [rbx],0x0
   180294b08:	74 13                	je     0x180294b1d
   180294b0a:	49 8b ce             	mov    rcx,r14
   180294b0d:	48 8b fb             	mov    rdi,rbx
   180294b10:	e8 5b 5c 01 00       	call   0x1802aa770
   180294b15:	84 c0                	test   al,al
   180294b17:	74 04                	je     0x180294b1d
   180294b19:	b1 01                	mov    cl,0x1
   180294b1b:	eb 05                	jmp    0x180294b22
   180294b1d:	32 c9                	xor    cl,cl
   180294b1f:	48 8b fb             	mov    rdi,rbx
   180294b22:	8b 05 b8 c5 1c 00    	mov    eax,DWORD PTR [rip+0x1cc5b8]        # 0x1804610e0
   180294b28:	49 8d 9e 60 04 00 00 	lea    rbx,[r14+0x460]
   180294b2f:	39 03                	cmp    DWORD PTR [rbx],eax
   180294b31:	0f 85 52 01 00 00    	jne    0x180294c89
   180294b37:	80 3d 06 12 1e 00 00 	cmp    BYTE PTR [rip+0x1e1206],0x0        # 0x180475d44
   180294b3e:	0f 84 45 01 00 00    	je     0x180294c89
   180294b44:	c6 07 01             	mov    BYTE PTR [rdi],0x1
   180294b47:	48 8d 3d e2 15 17 00 	lea    rdi,[rip+0x1715e2]        # 0x180406130
   180294b4e:	48 8d 35 0b 27 17 00 	lea    rsi,[rip+0x17270b]        # 0x180407260
   180294b55:	84 c9                	test   cl,cl
   180294b57:	74 0c                	je     0x180294b65
   180294b59:	49 8b ce             	mov    rcx,r14
   180294b5c:	e8 bf 22 fd ff       	call   0x180266e20
   180294b61:	84 c0                	test   al,al
   180294b63:	75 5a                	jne    0x180294bbf
   180294b65:	e8 e6 d4 f8 ff       	call   0x180222050
   180294b6a:	8b 4d bc             	mov    ecx,DWORD PTR [rbp-0x44]
   180294b6d:	4c 8d 4d 90          	lea    r9,[rbp-0x70]
   180294b71:	89 4c 24 7c          	mov    DWORD PTR [rsp+0x7c],ecx
   180294b75:	48 8d 55 b0          	lea    rdx,[rbp-0x50]
   180294b79:	48 8d 0d 90 26 17 00 	lea    rcx,[rip+0x172690]        # 0x180407210
   180294b80:	48 89 7c 24 70       	mov    QWORD PTR [rsp+0x70],rdi
   180294b85:	48 89 4d 90          	mov    QWORD PTR [rbp-0x70],rcx
   180294b89:	48 8b c8             	mov    rcx,rax
   180294b8c:	c7 44 24 78 00 09 00 	mov    DWORD PTR [rsp+0x78],0x900
   180294b93:	00 
   180294b94:	c5 f8 10 44 24 70    	vmovups xmm0,XMMWORD PTR [rsp+0x70]
   180294b9a:	48 89 75 80          	mov    QWORD PTR [rbp-0x80],rsi
   180294b9e:	c5 fb 10 4d 80       	vmovsd xmm1,QWORD PTR [rbp-0x80]
   180294ba3:	48 c7 45 98 4b 00 00 	mov    QWORD PTR [rbp-0x68],0x4b
   180294baa:	00 
   180294bab:	c5 f8 11 45 b0       	vmovups XMMWORD PTR [rbp-0x50],xmm0
   180294bb0:	c5 fb 11 4d c0       	vmovsd QWORD PTR [rbp-0x40],xmm1
   180294bb5:	48 89 5c 24 20       	mov    QWORD PTR [rsp+0x20],rbx
   180294bba:	e8 11 02 ec ff       	call   0x180154dd0
   180294bbf:	49 8b ce             	mov    rcx,r14
   180294bc2:	e8 59 12 fd ff       	call   0x180265e20
   180294bc7:	84 c0                	test   al,al
   180294bc9:	75 5a                	jne    0x180294c25
   180294bcb:	e8 80 d4 f8 ff       	call   0x180222050
   180294bd0:	8b 4d bc             	mov    ecx,DWORD PTR [rbp-0x44]
   180294bd3:	4c 8d 4d 90          	lea    r9,[rbp-0x70]
   180294bd7:	89 4c 24 7c          	mov    DWORD PTR [rsp+0x7c],ecx
   180294bdb:	48 8d 55 b0          	lea    rdx,[rbp-0x50]
   180294bdf:	48 8d 0d 02 27 17 00 	lea    rcx,[rip+0x172702]        # 0x1804072e8
   180294be6:	48 89 7c 24 70       	mov    QWORD PTR [rsp+0x70],rdi
   180294beb:	48 89 4d 90          	mov    QWORD PTR [rbp-0x70],rcx
   180294bef:	48 8b c8             	mov    rcx,rax
   180294bf2:	c7 44 24 78 04 09 00 	mov    DWORD PTR [rsp+0x78],0x904
   180294bf9:	00 
   180294bfa:	c5 f8 10 44 24 70    	vmovups xmm0,XMMWORD PTR [rsp+0x70]
   180294c00:	48 89 75 80          	mov    QWORD PTR [rbp-0x80],rsi
   180294c04:	c5 fb 10 4d 80       	vmovsd xmm1,QWORD PTR [rbp-0x80]
   180294c09:	48 c7 45 98 2b 00 00 	mov    QWORD PTR [rbp-0x68],0x2b
   180294c10:	00 
   180294c11:	c5 f8 11 45 b0       	vmovups XMMWORD PTR [rbp-0x50],xmm0
   180294c16:	c5 fb 11 4d c0       	vmovsd QWORD PTR [rbp-0x40],xmm1
   180294c1b:	48 89 5c 24 20       	mov    QWORD PTR [rsp+0x20],rbx
   180294c20:	e8 ab 01 ec ff       	call   0x180154dd0
   180294c25:	49 83 be 38 0a 00 00 	cmp    QWORD PTR [r14+0xa38],0x0
   180294c2c:	00 
   180294c2d:	ba 38 0a 00 00       	mov    edx,0xa38
   180294c32:	b9 78 07 00 00       	mov    ecx,0x778
   180294c37:	0f 44 d1             	cmove  edx,ecx
   180294c3a:	49 03 d6             	add    rdx,r14
   180294c3d:	4d 85 e4             	test   r12,r12
   180294c40:	74 30                	je     0x180294c72
   180294c42:	48 85 d2             	test   rdx,rdx
   180294c45:	74 2b                	je     0x180294c72
   180294c47:	49 8b 04 24          	mov    rax,QWORD PTR [r12]
   180294c4b:	48 85 c0             	test   rax,rax
   180294c4e:	74 22                	je     0x180294c72
   180294c50:	48 8b 12             	mov    rdx,QWORD PTR [rdx]
   180294c53:	48 85 d2             	test   rdx,rdx
   180294c56:	74 1a                	je     0x180294c72
   180294c58:	49 83 be 80 16 00 00 	cmp    QWORD PTR [r14+0x1680],0x0
   180294c5f:	00 
   180294c60:	74 10                	je     0x180294c72
   180294c62:	48 3b c2             	cmp    rax,rdx
   180294c65:	74 0b                	je     0x180294c72
   180294c67:	4d 8b c4             	mov    r8,r12
   180294c6a:	49 8b ce             	mov    rcx,r14
   180294c6d:	e8 fe 3a 00 00       	call   0x180298770
   180294c72:	45 33 ff             	xor    r15d,r15d
   180294c75:	41 c6 86 8b 04 00 00 	mov    BYTE PTR [r14+0x48b],0x1
   180294c7c:	01 
   180294c7d:	45 89 be 00 05 00 00 	mov    DWORD PTR [r14+0x500],r15d
   180294c84:	e9 b4 05 00 00       	jmp    0x18029523d
   180294c89:	80 3f 00             	cmp    BYTE PTR [rdi],0x0
   180294c8c:	74 16                	je     0x180294ca4
   180294c8e:	49 8b ce             	mov    rcx,r14
   180294c91:	e8 9a 16 fd ff       	call   0x180266330
   180294c96:	84 c0                	test   al,al
   180294c98:	75 0a                	jne    0x180294ca4
   180294c9a:	41 c6 86 64 02 00 00 	mov    BYTE PTR [r14+0x264],0x1
   180294ca1:	01 
   180294ca2:	88 07                	mov    BYTE PTR [rdi],al
   180294ca4:	41 80 be 84 04 00 00 	cmp    BYTE PTR [r14+0x484],0x0
   180294cab:	00 
   180294cac:	41 c6 86 ec 0a 00 00 	mov    BYTE PTR [r14+0xaec],0x0
   180294cb3:	00 
   180294cb4:	0f 85 26 05 00 00    	jne    0x1802951e0
   180294cba:	49 8b 86 f8 0a 00 00 	mov    rax,QWORD PTR [r14+0xaf8]
   180294cc1:	45 33 ff             	xor    r15d,r15d
   180294cc4:	4c 89 ac 24 a0 02 00 	mov    QWORD PTR [rsp+0x2a0],r13
   180294ccb:	00 
   180294ccc:	c5 f8 29 b4 24 50 02 	vmovaps XMMWORD PTR [rsp+0x250],xmm6
   180294cd3:	00 00 
   180294cd5:	c5 f8 29 bc 24 40 02 	vmovaps XMMWORD PTR [rsp+0x240],xmm7
   180294cdc:	00 00 
   180294cde:	48 89 44 24 70       	mov    QWORD PTR [rsp+0x70],rax
   180294ce3:	45 38 be 72 07 00 00 	cmp    BYTE PTR [r14+0x772],r15b
   180294cea:	74 09                	je     0x180294cf5
   180294cec:	4c 89 bd a8 01 00 00 	mov    QWORD PTR [rbp+0x1a8],r15
   180294cf3:	eb 0e                	jmp    0x180294d03
   180294cf5:	49 8b 86 50 0b 00 00 	mov    rax,QWORD PTR [r14+0xb50]
   180294cfc:	48 89 85 a8 01 00 00 	mov    QWORD PTR [rbp+0x1a8],rax
   180294d03:	49 8b 86 a8 0b 00 00 	mov    rax,QWORD PTR [r14+0xba8]
   180294d0a:	49 8b ce             	mov    rcx,r14
   180294d0d:	48 89 45 88          	mov    QWORD PTR [rbp-0x78],rax
   180294d11:	e8 fa f3 ff ff       	call   0x180294110
   180294d16:	49 8d 96 dc 03 00 00 	lea    rdx,[r14+0x3dc]
   180294d1d:	84 c0                	test   al,al
   180294d1f:	74 17                	je     0x180294d38
   180294d21:	45 39 be fc 04 00 00 	cmp    DWORD PTR [r14+0x4fc],r15d
   180294d28:	74 05                	je     0x180294d2f
   180294d2a:	83 3a 03             	cmp    DWORD PTR [rdx],0x3
   180294d2d:	75 09                	jne    0x180294d38
   180294d2f:	4d 8b ae 88 09 00 00 	mov    r13,QWORD PTR [r14+0x988]
   180294d36:	eb 03                	jmp    0x180294d3b
   180294d38:	4d 8b ef             	mov    r13,r15
   180294d3b:	e8 d0 f3 ff ff       	call   0x180294110
   180294d40:	84 c0                	test   al,al
   180294d42:	74 20                	je     0x180294d64
   180294d44:	45 38 be 8b 16 00 00 	cmp    BYTE PTR [r14+0x168b],r15b
   180294d4b:	75 17                	jne    0x180294d64
   180294d4d:	83 3a 03             	cmp    DWORD PTR [rdx],0x3
   180294d50:	75 09                	jne    0x180294d5b
   180294d52:	45 38 be a5 04 00 00 	cmp    BYTE PTR [r14+0x4a5],r15b
   180294d59:	74 09                	je     0x180294d64
   180294d5b:	49 8b be 38 0a 00 00 	mov    rdi,QWORD PTR [r14+0xa38]
   180294d62:	eb 03                	jmp    0x180294d67
   180294d64:	49 8b ff             	mov    rdi,r15
   180294d67:	45 38 be 65 02 00 00 	cmp    BYTE PTR [r14+0x265],r15b
   180294d6e:	74 0b                	je     0x180294d7b
   180294d70:	c4 c1 7a 10 b6 68 02 	vmovss xmm6,DWORD PTR [r14+0x268]
   180294d77:	00 00 
   180294d79:	eb 04                	jmp    0x180294d7f
   180294d7b:	c5 c8 57 f6          	vxorps xmm6,xmm6,xmm6
   180294d7f:	49 8b 8e f8 0a 00 00 	mov    rcx,QWORD PTR [r14+0xaf8]
   180294d86:	c5 f0 57 c9          	vxorps xmm1,xmm1,xmm1
   180294d8a:	c4 c1 72 2a 8e 74 02 	vcvtsi2ss xmm1,xmm1,DWORD PTR [r14+0x274]
   180294d91:	00 00 
   180294d93:	c5 f8 57 c0          	vxorps xmm0,xmm0,xmm0
   180294d97:	c4 c1 7a 2a 86 70 02 	vcvtsi2ss xmm0,xmm0,DWORD PTR [r14+0x270]
   180294d9e:	00 00 
   180294da0:	c5 f2 5e c8          	vdivss xmm1,xmm1,xmm0
   180294da4:	c4 c1 72 59 96 f0 02 	vmulss xmm2,xmm1,DWORD PTR [r14+0x2f0]
   180294dab:	00 00 
   180294dad:	c5 ea 59 3d a3 6e 17 	vmulss xmm7,xmm2,DWORD PTR [rip+0x176ea3]        # 0x18040bc58
   180294db4:	00 
   180294db5:	48 85 c9             	test   rcx,rcx
   180294db8:	74 0d                	je     0x180294dc7
   180294dba:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   180294dbd:	49 8d 96 20 0b 00 00 	lea    rdx,[r14+0xb20]
   180294dc4:	ff 50 50             	call   QWORD PTR [rax+0x50]
   180294dc7:	41 8b 86 30 0b 00 00 	mov    eax,DWORD PTR [r14+0xb30]
   180294dce:	41 39 86 a4 02 00 00 	cmp    DWORD PTR [r14+0x2a4],eax
   180294dd5:	74 33                	je     0x180294e0a
   180294dd7:	49 8b 8e f8 0a 00 00 	mov    rcx,QWORD PTR [r14+0xaf8]
   180294dde:	48 85 c9             	test   rcx,rcx
   180294de1:	74 0d                	je     0x180294df0
   180294de3:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   180294de6:	49 8d 96 20 0b 00 00 	lea    rdx,[r14+0xb20]
   180294ded:	ff 50 50             	call   QWORD PTR [rax+0x50]
   180294df0:	c4 c1 78 10 86 30 0b 	vmovups xmm0,XMMWORD PTR [r14+0xb30]
   180294df7:	00 00 
   180294df9:	49 8b ce             	mov    rcx,r14
   180294dfc:	c4 c1 79 7e 86 a4 02 	vmovd  DWORD PTR [r14+0x2a4],xmm0
   180294e03:	00 00 
   180294e05:	e8 16 d3 00 00       	call   0x1802a2120
   180294e0a:	45 38 be aa 02 00 00 	cmp    BYTE PTR [r14+0x2aa],r15b
   180294e11:	74 16                	je     0x180294e29
   180294e13:	41 b1 01             	mov    r9b,0x1
   180294e16:	49 8d 96 f8 0a 00 00 	lea    rdx,[r14+0xaf8]
   180294e1d:	45 0f b6 c1          	movzx  r8d,r9b
   180294e21:	49 8b ce             	mov    rcx,r14
   180294e24:	e8 d7 d4 00 00       	call   0x1802a2300
   180294e29:	45 38 be e4 04 00 00 	cmp    BYTE PTR [r14+0x4e4],r15b
   180294e30:	74 07                	je     0x180294e39
   180294e32:	ba 01 00 00 00       	mov    edx,0x1
   180294e37:	eb 09                	jmp    0x180294e42
   180294e39:	41 8b 96 a0 16 00 00 	mov    edx,DWORD PTR [r14+0x16a0]
   180294e40:	ff ca                	dec    edx
   180294e42:	41 8b 8e 60 04 00 00 	mov    ecx,DWORD PTR [r14+0x460]
   180294e49:	33 c0                	xor    eax,eax
   180294e4b:	48 89 45 40          	mov    QWORD PTR [rbp+0x40],rax
   180294e4f:	48 8b 44 24 70       	mov    rax,QWORD PTR [rsp+0x70]
   180294e54:	c5 f9 ef c0          	vpxor  xmm0,xmm0,xmm0
   180294e58:	c5 fc 11 45 10       	vmovups YMMWORD PTR [rbp+0x10],ymm0
   180294e5d:	c5 f8 11 45 30       	vmovups XMMWORD PTR [rbp+0x30],xmm0
   180294e62:	48 89 45 e8          	mov    QWORD PTR [rbp-0x18],rax
   180294e66:	48 8b 45 88          	mov    rax,QWORD PTR [rbp-0x78]
   180294e6a:	48 89 45 f0          	mov    QWORD PTR [rbp-0x10],rax
   180294e6e:	48 8b 85 a8 01 00 00 	mov    rax,QWORD PTR [rbp+0x1a8]
   180294e75:	c5 f8 57 c0          	vxorps xmm0,xmm0,xmm0
   180294e79:	c4 c1 7a 2a 86 78 02 	vcvtsi2ss xmm0,xmm0,DWORD PTR [r14+0x278]
   180294e80:	00 00 
   180294e82:	c5 fa 11 45 18       	vmovss DWORD PTR [rbp+0x18],xmm0
   180294e87:	c4 c1 7a 10 46 08    	vmovss xmm0,DWORD PTR [r14+0x8]
   180294e8d:	48 89 45 f8          	mov    QWORD PTR [rbp-0x8],rax
   180294e91:	c4 c1 7a 2c 46 10    	vcvttss2si eax,DWORD PTR [r14+0x10]
   180294e97:	c5 fa 11 45 24       	vmovss DWORD PTR [rbp+0x24],xmm0
   180294e9c:	c5 f0 57 c9          	vxorps xmm1,xmm1,xmm1
   180294ea0:	c4 c1 72 2a 8e 7c 02 	vcvtsi2ss xmm1,xmm1,DWORD PTR [r14+0x27c]
   180294ea7:	00 00 
   180294ea9:	c5 fa 11 4d 1c       	vmovss DWORD PTR [rbp+0x1c],xmm1
   180294eae:	c4 c1 7a 10 4e 0c    	vmovss xmm1,DWORD PTR [r14+0xc]
   180294eb4:	c5 fa 11 4d 28       	vmovss DWORD PTR [rbp+0x28],xmm1
   180294eb9:	c5 f8 57 c0          	vxorps xmm0,xmm0,xmm0
   180294ebd:	c5 fa 2a c0          	vcvtsi2ss xmm0,xmm0,eax
   180294ec1:	c4 c1 7a 2c 46 14    	vcvttss2si eax,DWORD PTR [r14+0x14]
   180294ec7:	c5 fa 11 45 2c       	vmovss DWORD PTR [rbp+0x2c],xmm0
   180294ecc:	c4 c1 7a 10 86 f8 02 	vmovss xmm0,DWORD PTR [r14+0x2f8]
   180294ed3:	00 00 
   180294ed5:	c5 fa 11 75 20       	vmovss DWORD PTR [rbp+0x20],xmm6
   180294eda:	c5 fa 11 45 38       	vmovss DWORD PTR [rbp+0x38],xmm0
   180294edf:	c5 fa 11 7d 40       	vmovss DWORD PTR [rbp+0x40],xmm7
   180294ee4:	89 95 80 00 00 00    	mov    DWORD PTR [rbp+0x80],edx
   180294eea:	48 8d 95 90 00 00 00 	lea    rdx,[rbp+0x90]
   180294ef1:	4c 89 7d e0          	mov    QWORD PTR [rbp-0x20],r15
   180294ef5:	4c 89 7d 08          	mov    QWORD PTR [rbp+0x8],r15
   180294ef9:	4c 89 7d 50          	mov    QWORD PTR [rbp+0x50],r15
   180294efd:	4c 89 7d 58          	mov    QWORD PTR [rbp+0x58],r15
   180294f01:	4c 89 7d 60          	mov    QWORD PTR [rbp+0x60],r15
   180294f05:	4c 89 7d 48          	mov    QWORD PTR [rbp+0x48],r15
   180294f09:	4c 89 7d 00          	mov    QWORD PTR [rbp+0x0],r15
   180294f0d:	4c 89 7d 10          	mov    QWORD PTR [rbp+0x10],r15
   180294f11:	4c 89 6d 68          	mov    QWORD PTR [rbp+0x68],r13
   180294f15:	48 89 7d 70          	mov    QWORD PTR [rbp+0x70],rdi
   180294f19:	44 88 7d 34          	mov    BYTE PTR [rbp+0x34],r15b
   180294f1d:	89 4d 78             	mov    DWORD PTR [rbp+0x78],ecx
   180294f20:	c6 45 44 01          	mov    BYTE PTR [rbp+0x44],0x1
   180294f24:	c6 45 7c 01          	mov    BYTE PTR [rbp+0x7c],0x1
   180294f28:	4c 89 bd 88 00 00 00 	mov    QWORD PTR [rbp+0x88],r15
   180294f2f:	c5 f0 57 c9          	vxorps xmm1,xmm1,xmm1
   180294f33:	c5 f2 2a c8          	vcvtsi2ss xmm1,xmm1,eax
   180294f37:	c5 fa 11 4d 30       	vmovss DWORD PTR [rbp+0x30],xmm1
   180294f3c:	c4 c1 7a 10 8e f4 02 	vmovss xmm1,DWORD PTR [r14+0x2f4]
   180294f43:	00 00 
   180294f45:	c5 fa 11 4d 3c       	vmovss DWORD PTR [rbp+0x3c],xmm1
   180294f4a:	48 8d 45 e0          	lea    rax,[rbp-0x20]
   180294f4e:	c5 fc 10 00          	vmovups ymm0,YMMWORD PTR [rax]
   180294f52:	c5 fc 10 90 80 00 00 	vmovups ymm2,YMMWORD PTR [rax+0x80]
   180294f59:	00 
   180294f5a:	c5 fc 11 02          	vmovups YMMWORD PTR [rdx],ymm0
   180294f5e:	c5 fc 10 40 20       	vmovups ymm0,YMMWORD PTR [rax+0x20]
   180294f63:	c5 fc 11 42 20       	vmovups YMMWORD PTR [rdx+0x20],ymm0
   180294f68:	c5 fc 10 40 40       	vmovups ymm0,YMMWORD PTR [rax+0x40]
   180294f6d:	c5 fc 11 42 40       	vmovups YMMWORD PTR [rdx+0x40],ymm0
   180294f72:	c5 fc 10 40 60       	vmovups ymm0,YMMWORD PTR [rax+0x60]
   180294f77:	c5 fc 11 42 60       	vmovups YMMWORD PTR [rdx+0x60],ymm0
   180294f7c:	c5 fc 11 92 80 00 00 	vmovups YMMWORD PTR [rdx+0x80],ymm2
   180294f83:	00 
   180294f84:	c5 f8 10 90 a0 00 00 	vmovups xmm2,XMMWORD PTR [rax+0xa0]
   180294f8b:	00 
   180294f8c:	41 0f b6 86 64 02 00 	movzx  eax,BYTE PTR [r14+0x264]
   180294f93:	00 
   180294f94:	c5 f8 11 92 a0 00 00 	vmovups XMMWORD PTR [rdx+0xa0],xmm2
   180294f9b:	00 
   180294f9c:	c5 f8 28 cf          	vmovaps xmm1,xmm7
   180294fa0:	88 85 e4 00 00 00    	mov    BYTE PTR [rbp+0xe4],al
   180294fa6:	c5 f8 77             	vzeroupper
   180294fa9:	e8 32 7d fe ff       	call   0x18027cce0
   180294fae:	48 8d 8d 90 00 00 00 	lea    rcx,[rbp+0x90]
   180294fb5:	c5 fa 11 85 f0 00 00 	vmovss DWORD PTR [rbp+0xf0],xmm0
   180294fbc:	00 
   180294fbd:	ff 15 5d dc 1d 00    	call   QWORD PTR [rip+0x1ddc5d]        # 0x180472c20
   180294fc3:	c5 f8 28 bc 24 40 02 	vmovaps xmm7,XMMWORD PTR [rsp+0x240]
   180294fca:	00 00 
   180294fcc:	c5 f8 28 b4 24 50 02 	vmovaps xmm6,XMMWORD PTR [rsp+0x250]
   180294fd3:	00 00 
   180294fd5:	4c 8b ac 24 a0 02 00 	mov    r13,QWORD PTR [rsp+0x2a0]
   180294fdc:	00 
   180294fdd:	45 88 be 64 02 00 00 	mov    BYTE PTR [r14+0x264],r15b
   180294fe4:	45 38 be 8e 04 00 00 	cmp    BYTE PTR [r14+0x48e],r15b
   180294feb:	0f 85 0f 01 00 00    	jne    0x180295100
   180294ff1:	49 8b 8e 80 16 00 00 	mov    rcx,QWORD PTR [r14+0x1680]
   180294ff8:	4c 8d 85 a8 01 00 00 	lea    r8,[rbp+0x1a8]
   180294fff:	48 8d 15 6a 2b 17 00 	lea    rdx,[rip+0x172b6a]        # 0x180407b70
   180295006:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   180295009:	ff 10                	call   QWORD PTR [rax]
   18029500b:	48 8b 8d a8 01 00 00 	mov    rcx,QWORD PTR [rbp+0x1a8]
   180295012:	48 8d 15 a7 22 17 00 	lea    rdx,[rip+0x1722a7]        # 0x1804072c0
   180295019:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   18029501c:	ff 50 18             	call   QWORD PTR [rax+0x18]
   18029501f:	49 8b 8e 80 16 00 00 	mov    rcx,QWORD PTR [r14+0x1680]
   180295026:	4c 8b 06             	mov    r8,QWORD PTR [rsi]
   180295029:	49 8b 96 f8 0a 00 00 	mov    rdx,QWORD PTR [r14+0xaf8]
   180295030:	48 8b 01             	mov    rax,QWORD PTR [rcx]
   180295033:	ff 90 78 01 00 00    	call   QWORD PTR [rax+0x178]
   180295039:	49 8b be 80 16 00 00 	mov    rdi,QWORD PTR [r14+0x1680]
   180295040:	49 8d 8e 78 07 00 00 	lea    rcx,[r14+0x778]
   180295047:	c5 f8 57 c0          	vxorps xmm0,xmm0,xmm0
   18029504b:	c5                   	.byte 0xc5
   18029504c:	f8                   	clc
   18029504d:	11                   	.byte 0x11
   18029504e:	44                   	rex.R
   18029504f:	24                   	.byte 0x24
