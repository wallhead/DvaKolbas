
/mnt/data/aio18_work/extracted/SKSE/Plugins/SkyrimUpscaler.dll:     file format pei-x86-64


Disassembly of section .text:

000000018019dc40 <.text+0x19cc40>:
   18019dc40:	c8 77 16 8b          	enter  0x1677,0x8b
   18019dc44:	c6 41 8b cf          	mov    BYTE PTR [rcx-0x75],0xcf
   18019dc48:	48 2b c8             	sub    rcx,rax
   18019dc4b:	8b 45 77             	mov    eax,DWORD PTR [rbp+0x77]
   18019dc4e:	48 03 c8             	add    rcx,rax
   18019dc51:	41 8b c1             	mov    eax,r9d
   18019dc54:	48 3b c8             	cmp    rcx,rax
   18019dc57:	76 08                	jbe    0x18019dc61
   18019dc59:	c7 44 24 20 02 00 00 	mov    DWORD PTR [rsp+0x20],0x2
   18019dc60:	00 
   18019dc61:	c5 fc 10 44 24 20    	vmovups ymm0,YMMWORD PTR [rsp+0x20]
   18019dc67:	c5 fc 11 07          	vmovups YMMWORD PTR [rdi],ymm0
   18019dc6b:	44 89 77 20          	mov    DWORD PTR [rdi+0x20],r14d
   18019dc6f:	48 8b c7             	mov    rax,rdi
   18019dc72:	c5 f8 77             	vzeroupper
   18019dc75:	4c 8d 9c 24 b0 00 00 	lea    r11,[rsp+0xb0]
   18019dc7c:	00 
   18019dc7d:	49 8b 5b 38          	mov    rbx,QWORD PTR [r11+0x38]
   18019dc81:	49 8b 73 40          	mov    rsi,QWORD PTR [r11+0x40]
   18019dc85:	49 8b 7b 48          	mov    rdi,QWORD PTR [r11+0x48]
   18019dc89:	49 8b e3             	mov    rsp,r11
   18019dc8c:	41 5f                	pop    r15
   18019dc8e:	41 5e                	pop    r14
   18019dc90:	41 5d                	pop    r13
   18019dc92:	41 5c                	pop    r12
   18019dc94:	5d                   	pop    rbp
   18019dc95:	c3                   	ret
   18019dc96:	33 c9                	xor    ecx,ecx
   18019dc98:	c5 f9 ef c0          	vpxor  xmm0,xmm0,xmm0
   18019dc9c:	33 c0                	xor    eax,eax
   18019dc9e:	89 4c 24 20          	mov    DWORD PTR [rsp+0x20],ecx
   18019dca2:	48 89 45 93          	mov    QWORD PTR [rbp-0x6d],rax
   18019dca6:	c5 f8 11 45 83       	vmovups XMMWORD PTR [rbp-0x7d],xmm0
   18019dcab:	89 4d 9b             	mov    DWORD PTR [rbp-0x65],ecx
   18019dcae:	c5 fc 10 44 24 20    	vmovups ymm0,YMMWORD PTR [rsp+0x20]
   18019dcb4:	c5 fc 11 07          	vmovups YMMWORD PTR [rdi],ymm0
   18019dcb8:	89 4f 20             	mov    DWORD PTR [rdi+0x20],ecx
   18019dcbb:	eb b2                	jmp    0x18019dc6f
   18019dcbd:	cc                   	int3
   18019dcbe:	cc                   	int3
   18019dcbf:	cc                   	int3
   18019dcc0:	48 89 5c 24 08       	mov    QWORD PTR [rsp+0x8],rbx
   18019dcc5:	48 89 6c 24 10       	mov    QWORD PTR [rsp+0x10],rbp
   18019dcca:	48 89 74 24 18       	mov    QWORD PTR [rsp+0x18],rsi
   18019dccf:	48 89 7c 24 20       	mov    QWORD PTR [rsp+0x20],rdi
   18019dcd4:	41 56                	push   r14
   18019dcd6:	48 83 ec 30          	sub    rsp,0x30
   18019dcda:	44 0f b6 74 24 60    	movzx  r14d,BYTE PTR [rsp+0x60]
   18019dce0:	49 8b d9             	mov    rbx,r9
   18019dce3:	41 8b f8             	mov    edi,r8d
   18019dce6:	8b f2                	mov    esi,edx
   18019dce8:	48 8b e9             	mov    rbp,rcx
   18019dceb:	e8 b0 26 fb ff       	call   0x1801503a0
   18019dcf0:	44 8b 15 e9 33 2c 00 	mov    r10d,DWORD PTR [rip+0x2c33e9]        # 0x1804610e0
   18019dcf7:	44 39 90 60 04 00 00 	cmp    DWORD PTR [rax+0x460],r10d
   18019dcfe:	0f 85 bd 00 00 00    	jne    0x18019ddc1
   18019dd04:	80 3d 39 80 2d 00 00 	cmp    BYTE PTR [rip+0x2d8039],0x0        # 0x180475d44
   18019dd0b:	0f 84 b0 00 00 00    	je     0x18019ddc1
   18019dd11:	48 8b c8             	mov    rcx,rax
   18019dd14:	e8 97 77 0c 00       	call   0x1802654b0
   18019dd19:	ff 15 49 33 ce 00    	call   QWORD PTR [rip+0xce3349]        # 0x180e81068
   18019dd1f:	48 8b 0d 3a 33 ce 00 	mov    rcx,QWORD PTR [rip+0xce333a]        # 0x180e81060
   18019dd26:	48 8b 05 2b 33 ce 00 	mov    rax,QWORD PTR [rip+0xce332b]        # 0x180e81058
   18019dd2d:	48 8b 09             	mov    rcx,QWORD PTR [rcx]
   18019dd30:	ff d0                	call   rax
   18019dd32:	8b 0d 10 33 ce 00    	mov    ecx,DWORD PTR [rip+0xce3310]        # 0x180e81048
   18019dd38:	c7 04 01 01 00 00 00 	mov    DWORD PTR [rcx+rax*1],0x1
   18019dd3f:	ff 15 2b 33 ce 00    	call   QWORD PTR [rip+0xce332b]        # 0x180e81070
   18019dd45:	33 c9                	xor    ecx,ecx
   18019dd47:	ff 15 03 33 ce 00    	call   QWORD PTR [rip+0xce3303]        # 0x180e81050
   18019dd4d:	48 8b 05 e4 32 ce 00 	mov    rax,QWORD PTR [rip+0xce32e4]        # 0x180e81038
   18019dd54:	80 38 00             	cmp    BYTE PTR [rax],0x0
   18019dd57:	74 41                	je     0x18019dd9a
   18019dd59:	48 8b 0d e0 32 ce 00 	mov    rcx,QWORD PTR [rip+0xce32e0]        # 0x180e81040
   18019dd60:	ff 15 c2 32 ce 00    	call   QWORD PTR [rip+0xce32c2]        # 0x180e81028
   18019dd66:	48 8b 05 13 33 ce 00 	mov    rax,QWORD PTR [rip+0xce3313]        # 0x180e81080
   18019dd6d:	4c 8b cb             	mov    r9,rbx
   18019dd70:	44 8b c7             	mov    r8d,edi
   18019dd73:	44 88 74 24 20       	mov    BYTE PTR [rsp+0x20],r14b
   18019dd78:	8b d6                	mov    edx,esi
   18019dd7a:	48 8b cd             	mov    rcx,rbp
   18019dd7d:	4c 8b 10             	mov    r10,QWORD PTR [rax]
   18019dd80:	41 ff d2             	call   r10
   18019dd83:	48 8b 0d b6 32 ce 00 	mov    rcx,QWORD PTR [rip+0xce32b6]        # 0x180e81040
   18019dd8a:	ff 15 a0 32 ce 00    	call   QWORD PTR [rip+0xce32a0]        # 0x180e81030
   18019dd90:	33 c9                	xor    ecx,ecx
   18019dd92:	ff 15 b8 32 ce 00    	call   QWORD PTR [rip+0xce32b8]        # 0x180e81050
   18019dd98:	eb 45                	jmp    0x18019dddf
   18019dd9a:	48 8b 05 df 32 ce 00 	mov    rax,QWORD PTR [rip+0xce32df]        # 0x180e81080
   18019dda1:	4c 8b cb             	mov    r9,rbx
   18019dda4:	44 8b c7             	mov    r8d,edi
   18019dda7:	44 88 74 24 20       	mov    BYTE PTR [rsp+0x20],r14b
   18019ddac:	8b d6                	mov    edx,esi
   18019ddae:	48 8b cd             	mov    rcx,rbp
   18019ddb1:	4c 8b 10             	mov    r10,QWORD PTR [rax]
   18019ddb4:	41 ff d2             	call   r10
   18019ddb7:	33 c9                	xor    ecx,ecx
   18019ddb9:	ff 15 91 32 ce 00    	call   QWORD PTR [rip+0xce3291]        # 0x180e81050
   18019ddbf:	eb 1e                	jmp    0x18019dddf
   18019ddc1:	48 8b c8             	mov    rcx,rax
   18019ddc4:	e8 e7 76 0c 00       	call   0x1802654b0
   18019ddc9:	4c 8b cb             	mov    r9,rbx
   18019ddcc:	44 88 74 24 20       	mov    BYTE PTR [rsp+0x20],r14b
   18019ddd1:	44 8b c7             	mov    r8d,edi
   18019ddd4:	8b d6                	mov    edx,esi
   18019ddd6:	48 8b cd             	mov    rcx,rbp
   18019ddd9:	ff 15 99 32 ce 00    	call   QWORD PTR [rip+0xce3299]        # 0x180e81078
   18019dddf:	48 8b 5c 24 40       	mov    rbx,QWORD PTR [rsp+0x40]
   18019dde4:	48 8b 6c 24 48       	mov    rbp,QWORD PTR [rsp+0x48]
   18019dde9:	48 8b 74 24 50       	mov    rsi,QWORD PTR [rsp+0x50]
   18019ddee:	48 8b 7c 24 58       	mov    rdi,QWORD PTR [rsp+0x58]
   18019ddf3:	48 83 c4 30          	add    rsp,0x30
   18019ddf7:	41 5e                	pop    r14
   18019ddf9:	c3                   	ret
   18019ddfa:	cc                   	int3
   18019ddfb:	cc                   	int3
   18019ddfc:	cc                   	int3
   18019ddfd:	cc                   	int3
   18019ddfe:	cc                   	int3
   18019ddff:	cc                   	int3
   18019de00:	48 89 5c 24 10       	mov    QWORD PTR [rsp+0x10],rbx
   18019de05:	48 89 6c 24 18       	mov    QWORD PTR [rsp+0x18],rbp
   18019de0a:	48 89 74 24 20       	mov    QWORD PTR [rsp+0x20],rsi
   18019de0f:	57                   	push   rdi
   18019de10:	41 54                	push   r12
   18019de12:	41 55                	push   r13
   18019de14:	41 56                	push   r14
   18019de16:	41 57                	push   r15
   18019de18:	4c 8b ea             	mov    r13,rdx
   18019de1b:	4c 8b f9             	mov    r15,rcx
   18019de1e:	48 85 c9             	test   rcx,rcx
   18019de21:	0f 84 dd 00 00 00    	je     0x18019df04
   18019de27:	b8 4d 5a 00 00       	mov    eax,0x5a4d
   18019de2c:	66 39 01             	cmp    WORD PTR [rcx],ax
   18019de2f:	0f                   	.byte 0xf
