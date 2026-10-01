
/mnt/data/aio18_work/extracted/SKSE/Plugins/SkyrimUpscaler.dll:     file format pei-x86-64


Disassembly of section .text:

000000018019e330 <.text+0x19d330>:
   18019e330:	15 fb 2c ce 00       	adc    eax,0xce2cfb
   18019e335:	48 63 93 88 00 00 00 	movsxd rdx,DWORD PTR [rbx+0x88]
   18019e33c:	48 03 cb             	add    rcx,rbx
   18019e33f:	48 89 0d 2a 2d ce 00 	mov    QWORD PTR [rip+0xce2d2a],rcx        # 0x180e81070
   18019e346:	48 81 c2 8c 00 00 00 	add    rdx,0x8c
   18019e34d:	48 63 4b 33          	movsxd rcx,DWORD PTR [rbx+0x33]
   18019e351:	4c 03 c3             	add    r8,rbx
   18019e354:	48 83 c1 37          	add    rcx,0x37
   18019e358:	4c 89 05 e1 2c ce 00 	mov    QWORD PTR [rip+0xce2ce1],r8        # 0x180e81040
   18019e35f:	48 03 cb             	add    rcx,rbx
   18019e362:	48 03 d3             	add    rdx,rbx
   18019e365:	48 89 0d e4 2c ce 00 	mov    QWORD PTR [rip+0xce2ce4],rcx        # 0x180e81050
   18019e36c:	48 63 4b 39          	movsxd rcx,DWORD PTR [rbx+0x39]
   18019e370:	48 83 c1 3e          	add    rcx,0x3e
   18019e374:	48 03 cb             	add    rcx,rbx
   18019e377:	48 89 0d ba 2c ce 00 	mov    QWORD PTR [rip+0xce2cba],rcx        # 0x180e81038
   18019e37e:	48 63 4b 48          	movsxd rcx,DWORD PTR [rbx+0x48]
   18019e382:	48 83 c1 4c          	add    rcx,0x4c
   18019e386:	48 03 cb             	add    rcx,rbx
   18019e389:	48 89 0d 98 2c ce 00 	mov    QWORD PTR [rip+0xce2c98],rcx        # 0x180e81028
   18019e390:	48 63 4b 7c          	movsxd rcx,DWORD PTR [rbx+0x7c]
   18019e394:	48 83 e9 80          	sub    rcx,0xffffffffffffff80
   18019e398:	48 03 cb             	add    rcx,rbx
   18019e39b:	48 89 0d de 2c ce 00 	mov    QWORD PTR [rip+0xce2cde],rcx        # 0x180e81080
   18019e3a2:	49 3b d0             	cmp    rdx,r8
   18019e3a5:	0f 85 f6 01 00 00    	jne    0x18019e5a1
   18019e3ab:	48 8b 09             	mov    rcx,QWORD PTR [rcx]
   18019e3ae:	e8 6d fb ff ff       	call   0x18019df20
   18019e3b3:	84 c0                	test   al,al
   18019e3b5:	0f 84 e6 01 00 00    	je     0x18019e5a1
   18019e3bb:	48 8b 0d a6 2c ce 00 	mov    rcx,QWORD PTR [rip+0xce2ca6]        # 0x180e81068
   18019e3c2:	e8 59 fb ff ff       	call   0x18019df20
   18019e3c7:	84 c0                	test   al,al
   18019e3c9:	0f 84 d2 01 00 00    	je     0x18019e5a1
   18019e3cf:	48 8b 0d 9a 2c ce 00 	mov    rcx,QWORD PTR [rip+0xce2c9a]        # 0x180e81070
   18019e3d6:	e8 45 fb ff ff       	call   0x18019df20
   18019e3db:	84 c0                	test   al,al
   18019e3dd:	0f 84 be 01 00 00    	je     0x18019e5a1
   18019e3e3:	48 8b 0d 66 2c ce 00 	mov    rcx,QWORD PTR [rip+0xce2c66]        # 0x180e81050
   18019e3ea:	e8 31 fb ff ff       	call   0x18019df20
   18019e3ef:	84 c0                	test   al,al
   18019e3f1:	0f 84 aa 01 00 00    	je     0x18019e5a1
   18019e3f7:	48 8b 0d 5a 2c ce 00 	mov    rcx,QWORD PTR [rip+0xce2c5a]        # 0x180e81058
   18019e3fe:	e8 1d fb ff ff       	call   0x18019df20
   18019e403:	84 c0                	test   al,al
   18019e405:	0f 84 96 01 00 00    	je     0x18019e5a1
   18019e40b:	48 8b 0d 16 2c ce 00 	mov    rcx,QWORD PTR [rip+0xce2c16]        # 0x180e81028
   18019e412:	e8 09 fb ff ff       	call   0x18019df20
   18019e417:	84 c0                	test   al,al
   18019e419:	0f 84 82 01 00 00    	je     0x18019e5a1
   18019e41f:	48 8b 0d 0a 2c ce 00 	mov    rcx,QWORD PTR [rip+0xce2c0a]        # 0x180e81030
   18019e426:	e8 f5 fa ff ff       	call   0x18019df20
   18019e42b:	84 c0                	test   al,al
   18019e42d:	0f 84 6e 01 00 00    	je     0x18019e5a1
   18019e433:	4c 8d 05 3e 2c ce 00 	lea    r8,[rip+0xce2c3e]        # 0x180e81078
   18019e43a:	48 8b cf             	mov    rcx,rdi
   18019e43d:	48 8d 15 7c f8 ff ff 	lea    rdx,[rip+0xfffffffffffff87c]        # 0x18019dcc0
   18019e444:	e8 67 3c 01 00       	call   0x1801b20b0
   18019e449:	85 c0                	test   eax,eax
   18019e44b:	0f 85 50 01 00 00    	jne    0x18019e5a1
   18019e451:	48 8b cf             	mov    rcx,rdi
   18019e454:	e8 e7 3e 01 00       	call   0x1801b2340
   18019e459:	85 c0                	test   eax,eax
   18019e45b:	74 0d                	je     0x18019e46a
   18019e45d:	48 8b cf             	mov    rcx,rdi
   18019e460:	e8 9b 3f 01 00       	call   0x1801b2400
   18019e465:	e9 37 01 00 00       	jmp    0x18019e5a1
   18019e46a:	41 c6 87 54 03 00 00 	mov    BYTE PTR [r15+0x354],0x1
   18019e471:	01 
   18019e472:	e8 d9 3b 08 00       	call   0x180222050
   18019e477:	48 8d 0d 3a b6 26 00 	lea    rcx,[rip+0x26b63a]        # 0x180409ab8
   18019e47e:	c7 44 24 58 9b 00 00 	mov    DWORD PTR [rsp+0x58],0x9b
   18019e485:	00 
   18019e486:	48 89 4c 24 50       	mov    QWORD PTR [rsp+0x50],rcx
   18019e48b:	4c 8d 4c 24 40       	lea    r9,[rsp+0x40]
   18019e490:	8b 4c 24 7c          	mov    ecx,DWORD PTR [rsp+0x7c]
   18019e494:	48 8d 54 24 70       	lea    rdx,[rsp+0x70]
   18019e499:	89 4c 24 5c          	mov    DWORD PTR [rsp+0x5c],ecx
   18019e49d:	41 b8 02 00 00 00    	mov    r8d,0x2
   18019e4a3:	c5 f8 10 44 24 50    	vmovups xmm0,XMMWORD PTR [rsp+0x50]
   18019e4a9:	48 8d 0d d8 b5 26 00 	lea    rcx,[rip+0x26b5d8]        # 0x180409a88
   18019e4b0:	48 c7 44 24 48 8b 00 	mov    QWORD PTR [rsp+0x48],0x8b
   18019e4b7:	00 00 
   18019e4b9:	48 89 4c 24 60       	mov    QWORD PTR [rsp+0x60],rcx
   18019e4be:	48 8d 0d 3b b6 26 00 	lea    rcx,[rip+0x26b63b]        # 0x180409b00
   18019e4c5:	c5 fb 10 4c 24 60    	vmovsd xmm1,QWORD PTR [rsp+0x60]
   18019e4cb:	48 89 4c 24 40       	mov    QWORD PTR [rsp+0x40],rcx
   18019e4d0:	48 8d 8d d0 05 00 00 	lea    rcx,[rbp+0x5d0]
   18019e4d7:	48 89 4c 24 20       	mov    QWORD PTR [rsp+0x20],rcx
   18019e4dc:	48 8b c8             	mov    rcx,rax
   18019e4df:	c5 f8 11 44 24 70    	vmovups XMMWORD PTR [rsp+0x70],xmm0
   18019e4e5:	c5 fb 11 4d 80       	vmovsd QWORD PTR [rbp-0x80],xmm1
   18019e4ea:	e8 91 38 fb ff       	call   0x180151d80
   18019e4ef:	e9 ad 00 00 00       	jmp    0x18019e5a1
   18019e4f4:	48 85 d2             	test   rdx,rdx
   18019e4f7:	0f 95 85 d0 05 00 00 	setne  BYTE PTR [rbp+0x5d0]
   18019e4fe:	48 85 db             	test   rbx,rbx
   18019e501:	0f 95 85 d8 05 00 00 	setne  BYTE PTR [rbp+0x5d8]
   18019e508:	48 85 ff             	test   rdi,rdi
   18019e50b:	0f 95 85 e0 05 00 00 	setne  BYTE PTR [rbp+0x5e0]
   18019e512:	e8 39 3b 08 00       	call   0x180222050
   18019e517:	48 8d 0d 9a b5 26 00 	lea    rcx,[rip+0x26b59a]        # 0x180409ab8
   18019e51e:	c7 44 24 58 85 00 00 	mov    DWORD PTR [rsp+0x58],0x85
   18019e525:	00 
   18019e526:	48 89 4c 24 50       	mov    QWORD PTR [rsp+0x50],rcx
   18019e52b:	4c 8d 4c 24 40       	lea    r9,[rsp+0x40]
   18019e530:	8b 4c 24 7c          	mov    ecx,DWORD PTR [rsp+0x7c]
   18019e534:	48 8d 54 24 70       	lea    rdx,[rsp+0x70]
   18019e539:	89 4c 24 5c          	mov    DWORD PTR [rsp+0x5c],ecx
   18019e53d:	48 8d 0d 44 b5 26 00 	lea    rcx,[rip+0x26b544]        # 0x180409a88
   18019e544:	c5 f8 10 44 24 50    	vmovups xmm0,XMMWORD PTR [rsp+0x50]
   18019e54a:	48 89 4c 24 60       	mov    QWORD PTR [rsp+0x60],rcx
   18019e54f:	48 8d 0d 3a b6 26 00 	lea    rcx,[rip+0x26b63a]        # 0x180409b90
   18019e556:	c5 fb 10 4c 24 60    	vmovsd xmm1,QWORD PTR [rsp+0x60]
   18019e55c:	48                   	rex.W
   18019e55d:	89                   	.byte 0x89
   18019e55e:	4c                   	rex.WR
   18019e55f:	24                   	.byte 0x24
