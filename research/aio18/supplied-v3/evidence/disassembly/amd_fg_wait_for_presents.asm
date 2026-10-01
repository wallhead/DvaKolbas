; amd_fidelityfx_framegeneration_dx12.dll SHA256=3f5e674a59b400756e98ef31fd583a4eb08ad567ec8dee362886bc96e55ed347
; ImageBase=0x180000000, RVAs in left column
; range 0xAAF0..0xAB5E; TFrameInterpolationSwapChainDX12<IFrameInterpolationSwapChainDX12,FrameinterpolationPresentInfoExt,FfxFrameGenerationConfig,ffxCallbackDescFrameGenerationPresent,16>::waitForPresents
0000AAF0: rex       push rbx
0000AAF2: sub       rsp,0x30
0000AAF6: mov       r9,QWORD PTR [rcx+0x1750]
0000AAFD: mov       rbx,rcx
0000AB00: mov       rdx,QWORD PTR [rcx+0x1870]
0000AB07: mov       rcx,QWORD PTR [rcx+0x16e0]
0000AB0E: mov       BYTE PTR [rsp+0x20],0x0
0000AB13: call      0x1800f4c30 ; waitForFenceValue
0000AB18: mov       r9,QWORD PTR [rbx+0x1750]
0000AB1F: mov       rdx,QWORD PTR [rbx+0x1868]
0000AB26: mov       rcx,QWORD PTR [rbx+0x16e8]
0000AB2D: mov       BYTE PTR [rsp+0x20],0x0
0000AB32: call      0x1800f4c30 ; waitForFenceValue
0000AB37: mov       r9,QWORD PTR [rbx+0x1750]
0000AB3E: mov       rdx,QWORD PTR [rbx+0x1a68]
0000AB45: mov       rcx,QWORD PTR [rbx+0x16f0]
0000AB4C: mov       BYTE PTR [rsp+0x20],0x0
0000AB51: call      0x1800f4c30 ; waitForFenceValue
0000AB56: mov       al,0x1
0000AB58: add       rsp,0x30
0000AB5C: pop       rbx
0000AB5D: ret       
