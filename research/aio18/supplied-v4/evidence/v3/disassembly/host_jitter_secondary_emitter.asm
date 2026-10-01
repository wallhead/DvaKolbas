; SkyrimUpscaler.dll SHA256=5ded74be9bccefe4cff12cdd93e87cd079c017b0500dd1c5ec3150eaf08f7f81
; ImageBase=0x180000000, RVAs in left column
; range 0x1A46D0..0x1A4720; unnamed
001A46D0: mov       QWORD PTR [rsp+0x8],rbx
001A46D5: push      rdi
001A46D6: sub       rsp,0x20
001A46DA: mov       rdi,rcx
001A46DD: mov       dl,0x1
001A46DF: mov       ecx,0xe
001A46E4: call      0x1801af520
001A46E9: call      0x1801aacc0
001A46EE: movsxd    rdx,DWORD PTR [rdi+0x2]
001A46F2: lea       rbx,[rdi+0x6]
001A46F6: add       rbx,rdx
001A46F9: lea       r8,[rip+0xffffffffffffc160]        # 0x1801a0860
001A4700: mov       rdx,rdi
001A4703: mov       r9b,0x15
001A4706: mov       rcx,rax
001A4709: call      0x1801ab880
001A470E: mov       QWORD PTR [rip+0x2d8b8b],rbx        # 0x18047d2a0
001A4715: mov       rbx,QWORD PTR [rsp+0x30]
001A471A: add       rsp,0x20
001A471E: pop       rdi
001A471F: ret       
