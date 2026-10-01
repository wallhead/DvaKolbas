; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x1A4680..0x1A46D0; unnamed
001A4680: mov       QWORD PTR [rsp+0x8],rbx
001A4685: push      rdi
001A4686: sub       rsp,0x20
001A468A: mov       rdi,rcx
001A468D: mov       dl,0x1
001A468F: mov       ecx,0xe
001A4694: call      0x1801af520
001A4699: call      0x1801aacc0
001A469E: movsxd    rdx,DWORD PTR [rdi+0x1]
001A46A2: lea       rbx,[rdi+0x5]
001A46A6: add       rbx,rdx
001A46A9: lea       r8,[rip+0xffffffffffffb6e0]        # 0x18019fd90
001A46B0: mov       rdx,rdi
001A46B3: mov       r9b,0xe8
001A46B6: mov       rcx,rax
001A46B9: call      0x1801ab720
001A46BE: mov       QWORD PTR [rip+0x2d8cf3],rbx        # 0x18047d3b8
001A46C5: mov       rbx,QWORD PTR [rsp+0x30]
001A46CA: add       rsp,0x20
001A46CE: pop       rdi
001A46CF: ret       
