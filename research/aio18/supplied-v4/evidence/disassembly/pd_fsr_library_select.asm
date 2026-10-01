; PDPerfPlugin.dll SHA256=53b5068c01700aa1c6d551fce3827cf23f0cc3cf9f543737e6c6166a2ff441d1
; ImageBase=0x180000000, RVAs in left column
; range 0xCCC80..0xCCDA8; unnamed
000CCC80: mov       QWORD PTR [rsp+0x8],rbx
000CCC85: mov       QWORD PTR [rsp+0x10],rsi
000CCC8A: push      rdi
000CCC8B: sub       rsp,0x20
000CCC8F: cmp       BYTE PTR [rcx+0xca],0x0
000CCC96: mov       edi,r8d
000CCC99: mov       esi,edx
000CCC9B: mov       rbx,rcx
000CCC9E: jne       0x1800ccca5
000CCCA0: call      0x1800cc8c0
000CCCA5: test      edi,edi
000CCCA7: jne       0x1800cccd1
000CCCA9: mov       rax,QWORD PTR [rbx+0x28]
000CCCAD: mov       QWORD PTR [rbx],rax
000CCCB0: mov       rax,QWORD PTR [rbx+0x30]
000CCCB4: mov       QWORD PTR [rbx+0x8],rax
000CCCB8: mov       rax,QWORD PTR [rbx+0x38]
000CCCBC: mov       QWORD PTR [rbx+0x10],rax
000CCCC0: mov       rax,QWORD PTR [rbx+0x40]
000CCCC4: mov       QWORD PTR [rbx+0x18],rax
000CCCC8: mov       rax,QWORD PTR [rbx+0x48]
000CCCCC: jmp       0x1800ccd92
000CCCD1: cmp       edi,0x1
000CCCD4: jne       0x1800ccd5b
000CCCDA: cmp       esi,0x4
000CCCDD: mov       ecx,0x58
000CCCE2: mov       eax,0x80
000CCCE7: mov       r10d,0x50
000CCCED: cmovne    eax,ecx
000CCCF0: mov       ecx,0x60
000CCCF5: mov       r9,QWORD PTR [rax+rbx*1]
000CCCF9: mov       eax,0x88
000CCCFE: cmovne    eax,ecx
000CCD01: mov       ecx,0x68
000CCD06: mov       r8,QWORD PTR [rax+rbx*1]
000CCD0A: mov       eax,0x90
000CCD0F: cmovne    eax,ecx
000CCD12: mov       ecx,0x70
000CCD17: mov       rdx,QWORD PTR [rax+rbx*1]
000CCD1B: mov       eax,0x98
000CCD20: cmovne    eax,ecx
000CCD23: mov       rcx,QWORD PTR [rax+rbx*1]
000CCD27: mov       eax,0x78
000CCD2C: cmovne    eax,r10d
000CCD30: mov       rax,QWORD PTR [rax+rbx*1]
000CCD34: mov       QWORD PTR [rbx],rax
000CCD37: movzx     eax,dil
000CCD3B: mov       QWORD PTR [rbx+0x8],r9
000CCD3F: mov       QWORD PTR [rbx+0x10],r8
000CCD43: mov       QWORD PTR [rbx+0x18],rdx
000CCD47: mov       QWORD PTR [rbx+0x20],rcx
000CCD4B: mov       rbx,QWORD PTR [rsp+0x30]
000CCD50: mov       rsi,QWORD PTR [rsp+0x38]
000CCD55: add       rsp,0x20
000CCD59: pop       rdi
000CCD5A: ret       
000CCD5B: cmp       edi,0x2
000CCD5E: jne       0x1800ccd96
000CCD60: mov       rax,QWORD PTR [rbx+0xa0]
000CCD67: mov       QWORD PTR [rbx],rax
000CCD6A: mov       rax,QWORD PTR [rbx+0xa8]
000CCD71: mov       QWORD PTR [rbx+0x8],rax
000CCD75: mov       rax,QWORD PTR [rbx+0xb0]
000CCD7C: mov       QWORD PTR [rbx+0x10],rax
000CCD80: mov       rax,QWORD PTR [rbx+0xb8]
000CCD87: mov       QWORD PTR [rbx+0x18],rax
000CCD8B: mov       rax,QWORD PTR [rbx+0xc0]
000CCD92: mov       QWORD PTR [rbx+0x20],rax
000CCD96: mov       rbx,QWORD PTR [rsp+0x30]
000CCD9B: mov       al,0x1
000CCD9D: mov       rsi,QWORD PTR [rsp+0x38]
000CCDA2: add       rsp,0x20
000CCDA6: pop       rdi
000CCDA7: ret       
