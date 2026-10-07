add_executable(TRPFsrGenerationGpuSmoke FSRGenerationGpuSmoke.cpp "${PROJECT_SOURCE_DIR}/src/Graphics/D3D11D3D12Interop.cpp")
target_link_libraries(TRPFsrGenerationGpuSmoke PRIVATE TRPFsrRuntime d3d11 d3d12 dxgi d3dcompiler bcrypt)
target_include_directories(TRPFsrGenerationGpuSmoke PRIVATE "${PROJECT_SOURCE_DIR}/tests")
execute_process(COMMAND git rev-parse HEAD WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}" OUTPUT_VARIABLE fgValidationRevision OUTPUT_STRIP_TRAILING_WHITESPACE)
file(READ "${PROJECT_SOURCE_DIR}/tools/fsr/runtime-pin.json" srPin)
file(READ "${PROJECT_SOURCE_DIR}/tools/fsr/fg-runtime-pin.json" fgPin)
string(JSON fgLoaderSha GET "${srPin}" runtime 0 sha256)
string(JSON fgUpscalerSha GET "${srPin}" runtime 1 sha256)
string(JSON fgModuleSha GET "${fgPin}" runtime 0 sha256)
file(SHA256 "${CMAKE_CURRENT_SOURCE_DIR}/FSRGenerationGpuSmoke.cpp" fgSourceSha)
file(SHA256 "${CMAKE_CURRENT_SOURCE_DIR}/PresentationObserver.h" fgObserverSha)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS FSRGenerationGpuSmoke.cpp PresentationObserver.h)
target_compile_definitions(TRPFsrGenerationGpuSmoke PRIVATE TRP_FG_VALIDATION_REVISION="${fgValidationRevision}"
    TRP_FG_VALIDATION_SOURCE="${CMAKE_CURRENT_SOURCE_DIR}/FSRGenerationGpuSmoke.cpp"
    TRP_FG_OBSERVER_SOURCE="${CMAKE_CURRENT_SOURCE_DIR}/PresentationObserver.h"
    TRP_FG_LOADER_SHA="${fgLoaderSha}" TRP_FG_UPSCALER_SHA="${fgUpscalerSha}" TRP_FG_MODULE_SHA="${fgModuleSha}"
    TRP_FG_SOURCE_SHA="${fgSourceSha}" TRP_FG_OBSERVER_SHA="${fgObserverSha}")
add_test(NAME FsrGenerationGpuSmoke COMMAND TRPFsrGenerationGpuSmoke --frames 1000 --recreate 25 --debug auto
    --runtime "${TRP_FSR_SDK_DIR}/runtime" --output "${CMAKE_BINARY_DIR}/validation/fsr-fg-gpu.json")
set_tests_properties(FsrGenerationGpuSmoke PROPERTIES TIMEOUT 600 SKIP_RETURN_CODE 77)
