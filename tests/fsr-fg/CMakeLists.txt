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

if(TRP_NR_NGX_INCLUDE_DIR AND TRP_NGX_LIB AND TRP_NR_DLSS_SR_DLL)
    add_executable(TRPDlssFsrGenerationGpuTests DlssFsrGenerationGpuTests.cpp ../nr-postsr/dlss/PerformanceFacade.cpp
        "${PROJECT_SOURCE_DIR}/src/DLSSBackend.cpp" "${PROJECT_SOURCE_DIR}/src/Graphics/D3D11D3D12Interop.cpp")
    target_compile_features(TRPDlssFsrGenerationGpuTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPDlssFsrGenerationGpuTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN
        TRP_FG_VALIDATION_REVISION="${fgValidationRevision}" TRP_FG_LOADER_SHA="${fgLoaderSha}" TRP_FG_MODULE_SHA="${fgModuleSha}")
    target_include_directories(TRPDlssFsrGenerationGpuTests PRIVATE ../nr-postsr/dlss ../ "${TRP_NR_NGX_INCLUDE_DIR}" "${TRP_STREAMLINE_INCLUDE_DIR}")
    target_link_libraries(TRPDlssFsrGenerationGpuTests PRIVATE TRPFsrRuntime TRPNeuralRuntime "${TRP_NGX_LIB}" d3d11 d3d12 dxgi d3dcompiler bcrypt user32)
    add_custom_command(TARGET TRPDlssFsrGenerationGpuTests POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:TRPDlssFsrGenerationGpuTests>/Data/SKSE/Plugins/RaZkolbaS"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${TRP_NR_DLSS_SR_DLL}" "$<TARGET_FILE_DIR:TRPDlssFsrGenerationGpuTests>/Data/SKSE/Plugins/RaZkolbaS/nvngx_dlss.dll")
    foreach(quality IN ITEMS Native Quality Performance)
        string(TOLOWER "${quality}" qualityLower)
        add_test(NAME DlssFsrGenerationGpu-${quality} COMMAND TRPDlssFsrGenerationGpuTests --quality "${qualityLower}"
            --nr "${TRP_NR_RTX40_DLL}" --core "${TRP_NR_DRIVER_CORE}" --runtime "${TRP_FSR_SDK_DIR}/runtime"
            --output "${CMAKE_BINARY_DIR}/validation/dlss-fsr-fg-${qualityLower}.json")
        set_tests_properties(DlssFsrGenerationGpu-${quality} PROPERTIES RUN_SERIAL TRUE LABELS GPU TIMEOUT 180)
    endforeach()
endif()

if(TARGET TRPXessRuntime AND TARGET TRPNeuralRuntime AND TRP_NR_RTX40_DLL AND TRP_NR_DRIVER_CORE)
    file(READ "${PROJECT_SOURCE_DIR}/tools/xess/sdk-pin.json" xessGenerationPin)
    string(JSON xessGenerationSha GET "${xessGenerationPin}" runtime sha256)
    string(JSON xessGenerationBytes GET "${xessGenerationPin}" runtime bytes)
    add_executable(TRPXessFsrGenerationGpuTests XessFsrGenerationGpuTests.cpp
        "${PROJECT_SOURCE_DIR}/src/Graphics/D3D11D3D12Interop.cpp")
    target_compile_features(TRPXessFsrGenerationGpuTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPXessFsrGenerationGpuTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN
        TRP_FG_VALIDATION_REVISION="${fgValidationRevision}" TRP_FG_LOADER_SHA="${fgLoaderSha}" TRP_FG_MODULE_SHA="${fgModuleSha}"
        TRP_XESS_RUNTIME_SHA="${xessGenerationSha}" TRP_XESS_RUNTIME_BYTES=${xessGenerationBytes}
        TRP_TEST_RCAS_SHADER="${PROJECT_SOURCE_DIR}/package/SKSE/Plugins/RaZkolbaS/RCAS.hlsl")
    target_include_directories(TRPXessFsrGenerationGpuTests PRIVATE ../)
    target_link_libraries(TRPXessFsrGenerationGpuTests PRIVATE TRPXessRuntime TRPFsrRuntime TRPNeuralRuntime
        d3d11 d3d12 dxgi d3dcompiler bcrypt user32)
    foreach(quality IN ITEMS Native Performance)
        string(TOLOWER "${quality}" qualityLower)
        set(testName XessFsrGenerationGpu)
        if(NOT quality STREQUAL Native)
            string(APPEND testName "-${quality}")
        endif()
        add_test(NAME ${testName} COMMAND TRPXessFsrGenerationGpuTests --quality "${qualityLower}"
            --xess "${TRP_XESS_SDK_DIR}/SKSE/Plugins" --nr "${TRP_NR_RTX40_DLL}" --core "${TRP_NR_DRIVER_CORE}"
            --runtime "${TRP_FSR_SDK_DIR}/runtime" --output "${CMAKE_BINARY_DIR}/validation/xess-fsr-fg-${qualityLower}.json")
        set_tests_properties(${testName} PROPERTIES RUN_SERIAL TRUE LABELS GPU TIMEOUT 180)
    endforeach()
endif()
