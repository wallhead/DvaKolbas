$ErrorActionPreference='Stop'
$repo=(Get-Location).Path
$baseline='96f05a545ee45b21aa58f2502b9fafcc3b1de61e'
# Historical research fixture: snapshot its headers as well as its sources.
# Current production has a proper INT8 profile and must not receive these edits.
function Get-HistoricalSource([string]$Path) {
 $text=& git -C $repo show ($baseline+':'+$Path)
 if($LASTEXITCODE){throw "Historical probe source unavailable: $Path"}
 return ($text -join "`n")+"`n"
}
$probe=[IO.Path]::GetFullPath('out/research/fsr4-audit/nvidia-int8-probe')
if(Test-Path -LiteralPath $probe){throw 'Probe directory already exists'}
$shadow=Join-Path $probe 'shadow/Upscaling'
[IO.Directory]::CreateDirectory($shadow)|Out-Null
[IO.Directory]::CreateDirectory((Join-Path $probe 'runtime'))|Out-Null
foreach($path in (& git -C $repo ls-tree -r --name-only $baseline -- src/Upscaling)) {
 $relative=$path.Substring('src/Upscaling/'.Length)
 $target=Join-Path $shadow $relative
 [IO.Directory]::CreateDirectory([IO.Path]::GetDirectoryName($target))|Out-Null
 [IO.File]::WriteAllText($target,(Get-HistoricalSource $path),[Text.UTF8Encoding]::new($false))
}
$pins=@(
 @{name='amd_fidelityfx_loader_dx12.dll';sha='2f36843c3bb8c059621c10574e586a883ef337f2a549c67ecf3a82b3959ac238'},
 @{name='amd_fidelityfx_upscaler_dx12.dll';sha='2604c0b392072d715b400b2f89434274de31995a4b6e68ce38250ebbd3f6c5fc'})
foreach($pin in $pins) {
 $source=Join-Path 'out/research/fsr4-audit/original/UpscalerBasePlugin' $pin.name
 if((Get-FileHash -LiteralPath $source).Hash.ToLowerInvariant() -ne $pin.sha){throw 'AIO19 runtime differs from byte-validated evidence'}
 Copy-Item -LiteralPath $source -Destination (Join-Path $probe 'runtime')
}
foreach($name in @('FSRRuntime.cpp','FSRUpscaler.cpp')) {
 $source=Get-HistoricalSource ('src/Upscaling/'+$name)
 # The recovered AIO19 creation/sizing API version is 4.0.3. The independently
 # mapped dispatch descriptor is exactly the current header's 0x1B0 layout.
 $source=$source.Replace('FFX_UPSCALER_VERSION};','FFX_UPSCALER_MAKE_VERSION(4,0,3)};')
 if($name -eq 'FSRUpscaler.cpp') {
  # Research-only exception for this exact hash-pinned INT8 provider. Preserve
  # the production adapter guard for every other provider and all SDK contracts.
  $source=$source.Replace('if(IsFsr4Provider(provider)) {','if(IsFsr4Provider(provider) && provider.name!="4.0.2b") {')
  $source=$source.Replace('create.flags=FFX_UPSCALE_ENABLE_AUTO_EXPOSURE |','create.fpMessage=[](uint32_t type,const wchar_t* message){std::fwprintf(stderr,L"[FSR runtime type=%u] %ls\n",type,message?message:L"(null)");};'+"`n"+'        create.flags=FFX_UPSCALE_ENABLE_AUTO_EXPOSURE | FFX_UPSCALE_ENABLE_DEBUG_CHECKING |')
 }
 [IO.File]::WriteAllText((Join-Path $shadow $name),$source,[Text.UTF8Encoding]::new($false))
}
$smoke=Get-HistoricalSource 'tests/fsr-gpu/FSRGpuSmoke.cpp'
$smoke=$smoke.Replace('SelectProvider(providers,requestedPolicy,adapterDescription.VendorId)','SelectProvider(providers,requestedPolicy)')
$smoke=$smoke.Replace('int main(int argc,char** argv)', 'static_assert(sizeof(ffxDispatchDescUpscale)==0x1B0);'+"`n"+'static_assert(offsetof(ffxDispatchDescUpscale,flags)==0x1AC);'+"`n"+'int main(int argc,char** argv)')
$smoke=$smoke.Replace('"{\n\"sharpnessComparison\":','"{\n\"experimentalAio19Int8\":true,\n\"productionUpscalerUnmodified\":false,\n\"sharpnessComparison\":')
$smoke=$smoke.Replace('Require(allocatedResources>0||allocatedHeaps>0,"official allocation callbacks observed SDK allocations");',
 'if(allocatedResources==0 && allocatedHeaps==0)std::puts("LIMITATION: AIO19 runtime uses private allocations; SDK callbacks are not invoked");')
$smoke=$smoke.Replace('<<",\n\"sdkCommittedResourcesCreated\":',
 '<<",\n\"sdkAllocationCallbacksObserved\":"<<((allocatedResources||allocatedHeaps)?"true":"false")<<",\n\"sdkCommittedResourcesCreated\":')
$smoke=$smoke.Replace('SDK live resources/heaps zero','instrumented callback resources/heaps zero (private allocations uncounted)')
$smoke=$smoke.Replace('auto luid=rig.device12->GetAdapterLuid();std::filesystem::create_directories(report.parent_path());',
 'auto beforeUnload=memory();runtime.reset();rig.context11->Flush();auto afterUnload=memory();std::printf("Runtime unload memory: before=%llu after=%llu initial=%llu\n",beforeUnload,afterUnload,initialMemory);'+"`n"+
 '    auto luid=rig.device12->GetAdapterLuid();std::filesystem::create_directories(report.parent_path());')
$smoke=$smoke.Replace('<<",\n\"initialLocalGpuBytes\":',
 '<<",\n\"beforeRuntimeUnloadLocalGpuBytes\":"<<beforeUnload<<",\"afterRuntimeUnloadLocalGpuBytes\":"<<afterUnload<<",\n\"initialLocalGpuBytes\":')
[IO.File]::WriteAllText((Join-Path $probe 'NvidiaInt8GpuSmoke.cpp'),$smoke,[Text.UTF8Encoding]::new($false))
$cmakeRepo=$repo.Replace('\','/');$cmakeProbe=$probe.Replace('\','/')
$project=@"
cmake_minimum_required(VERSION 3.21)
project(NvidiaInt8Probe LANGUAGES CXX)
add_executable(NvidiaInt8GpuSmoke
  "$cmakeProbe/NvidiaInt8GpuSmoke.cpp"
  "$cmakeProbe/shadow/Upscaling/FSRRuntime.cpp"
  "$cmakeProbe/shadow/Upscaling/FSRUpscaler.cpp"
  "$cmakeProbe/shadow/Upscaling/FSRParameters.cpp"
  "$cmakeProbe/shadow/Upscaling/FSRColorConversion.cpp"
  "$cmakeProbe/shadow/Upscaling/FSRFrameAdapter.cpp"
  "$cmakeRepo/src/Graphics/D3D11D3D12Interop.cpp")
target_compile_features(NvidiaInt8GpuSmoke PRIVATE cxx_std_23)
target_compile_definitions(NvidiaInt8GpuSmoke PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN TRP_ENABLE_FSR)
target_compile_options(NvidiaInt8GpuSmoke PRIVATE /W4 /Zc:__cplusplus)
target_include_directories(NvidiaInt8GpuSmoke PRIVATE
  "$cmakeProbe/shadow" "$cmakeProbe/shadow/Upscaling" "$cmakeRepo/src" "$cmakeRepo/src/Upscaling" "$cmakeRepo/tests"
  "$cmakeRepo/out/research/sdk-v2.3.0/Kits/FidelityFX/api/include"
  "$cmakeRepo/out/research/sdk-v2.3.0/Kits/FidelityFX/upscalers/include")
target_link_libraries(NvidiaInt8GpuSmoke PRIVATE d3d11 d3d12 dxgi d3dcompiler)
"@
[IO.File]::WriteAllText((Join-Path $probe 'CMakeLists.txt'),$project,[Text.UTF8Encoding]::new($false))
$cmake='C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe'
& $cmake -S $probe -B (Join-Path $probe 'build') -G 'Visual Studio 17 2022' -A x64
if($LASTEXITCODE -ne 0){throw 'Probe configure failed'}
& $cmake --build (Join-Path $probe 'build') --config Release
if($LASTEXITCODE -ne 0){throw 'Probe build failed'}
