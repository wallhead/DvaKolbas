cmake_minimum_required(VERSION 3.25)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(TRPNrRuntimeResearch LANGUAGES CXX)
    enable_testing()
endif()
get_filename_component(nrRoot "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
option(TRP_NR_PERF_WITH_FSR "Extend isolated NR performance probe with FSR NativeAA" OFF)
set(TRP_NR_PERF_FSR_SDK "${nrRoot}/out/research/sdk-v2.3.0" CACHE PATH "Pinned FSR headers for integrated performance fixture")
find_package(Python3 COMPONENTS Interpreter REQUIRED)
add_test(NAME NrPerformanceComparison COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/test_performance_comparison.py")
find_program(nrPresentMonPowerShell NAMES powershell.exe REQUIRED)
find_program(nrPackagePowerShell NAMES pwsh.exe REQUIRED)
add_test(NAME NrModelPackage COMMAND "${nrPackagePowerShell}" -NoProfile -ExecutionPolicy Bypass
    -File "${nrRoot}/tests/NrModelPackageTests.ps1")
add_test(NAME NrPresentMonComparison COMMAND "${nrPresentMonPowerShell}" -NoProfile -ExecutionPolicy Bypass
    -File "${CMAKE_CURRENT_LIST_DIR}/PresentMonComparisonTests.ps1")
include("${nrRoot}/cmake/NeuralRuntime.cmake")
if(NOT TARGET TRPNrFsrRouteDiagnosticsTests)
    add_executable(TRPNrFsrRouteDiagnosticsTests ../NrFsrRouteDiagnosticsTests.cpp)
    target_compile_features(TRPNrFsrRouteDiagnosticsTests PRIVATE cxx_std_23)
    target_include_directories(TRPNrFsrRouteDiagnosticsTests PRIVATE "${nrRoot}/src")
    add_test(NAME NrFsrRouteDiagnostics COMMAND TRPNrFsrRouteDiagnosticsTests)
endif()
add_executable(TRPNrPerformanceMetricsTests ../NrPerformanceMetricsTests.cpp)
target_compile_features(TRPNrPerformanceMetricsTests PRIVATE cxx_std_23)
target_link_libraries(TRPNrPerformanceMetricsTests PRIVATE TRPNeuralRuntime)
add_test(NAME NrPerformanceMetrics COMMAND TRPNrPerformanceMetricsTests)
add_executable(TRPNrRuntimeCatalogTests ../NrRuntimeCatalogTests.cpp)
target_compile_features(TRPNrRuntimeCatalogTests PRIVATE cxx_std_23)
target_include_directories(TRPNrRuntimeCatalogTests PRIVATE "${nrRoot}/src")
target_link_libraries(TRPNrRuntimeCatalogTests PRIVATE TRPNeuralRuntime)
add_test(NAME NrRuntimeCatalog COMMAND TRPNrRuntimeCatalogTests)
add_executable(TRPNrRuntimeFileLeaseTests ../NrRuntimeFileLeaseTests.cpp)
target_compile_features(TRPNrRuntimeFileLeaseTests PRIVATE cxx_std_23)
target_compile_definitions(TRPNrRuntimeFileLeaseTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
target_include_directories(TRPNrRuntimeFileLeaseTests PRIVATE "${nrRoot}/src")
target_link_libraries(TRPNrRuntimeFileLeaseTests PRIVATE TRPNeuralRuntime)
add_test(NAME NrRuntimeFileLease COMMAND TRPNrRuntimeFileLeaseTests)
set(TRP_NR_RTX40_DLL "" CACHE FILEPATH "Exact local supplied RTX40 NR DLL for standalone shim tests")
set(TRP_NR_DRIVER_CORE "" CACHE FILEPATH "Installed NVIDIA driver core for hardware owner tests")
add_executable(TRPNrCallerIdentityShimTests ../NrCallerIdentityShimTests.cpp)
target_compile_features(TRPNrCallerIdentityShimTests PRIVATE cxx_std_23)
target_compile_definitions(TRPNrCallerIdentityShimTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
target_include_directories(TRPNrCallerIdentityShimTests PRIVATE "${nrRoot}/src")
target_link_libraries(TRPNrCallerIdentityShimTests PRIVATE TRPNeuralRuntime)
foreach(mode IN ITEMS normal conflict)
    add_test(NAME NrCallerIdentityShim-${mode} COMMAND TRPNrCallerIdentityShimTests "${TRP_NR_RTX40_DLL}" ${mode})
    set_tests_properties(NrCallerIdentityShim-${mode} PROPERTIES SKIP_RETURN_CODE 77 TIMEOUT 30)
endforeach()
add_executable(TRPNrImagePacketTests ../NrImagePacketTests.cpp)
target_compile_features(TRPNrImagePacketTests PRIVATE cxx_std_23)
target_compile_definitions(TRPNrImagePacketTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
target_link_libraries(TRPNrImagePacketTests PRIVATE TRPNeuralRuntime d3d11)
add_test(NAME NrImagePacket COMMAND TRPNrImagePacketTests)
set_tests_properties(NrImagePacket PROPERTIES SKIP_RETURN_CODE 77 RUN_SERIAL TRUE LABELS GPU)
add_executable(TRPNrHistoryTests ../NrHistoryTests.cpp)
target_compile_features(TRPNrHistoryTests PRIVATE cxx_std_23)
target_compile_definitions(TRPNrHistoryTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
target_link_libraries(TRPNrHistoryTests PRIVATE TRPNeuralRuntime)
add_test(NAME NrHistory COMMAND TRPNrHistoryTests)
add_executable(TRPNrStartupSettingsTests ../NrStartupSettingsTests.cpp)
target_compile_features(TRPNrStartupSettingsTests PRIVATE cxx_std_23)
target_link_libraries(TRPNrStartupSettingsTests PRIVATE TRPNeuralRuntime)
add_test(NAME NrStartupSettings COMMAND TRPNrStartupSettingsTests)
add_executable(TRPNrSourcePolicyTests ../NrSourcePolicyTests.cpp)
target_compile_features(TRPNrSourcePolicyTests PRIVATE cxx_std_23)
target_compile_definitions(TRPNrSourcePolicyTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
target_link_libraries(TRPNrSourcePolicyTests PRIVATE TRPNeuralRuntime)
add_test(NAME NrSourcePolicy COMMAND TRPNrSourcePolicyTests)
add_executable(TRPNrTicketOwnershipTests ../NrTicketOwnershipTests.cpp)
target_compile_features(TRPNrTicketOwnershipTests PRIVATE cxx_std_23)
target_link_libraries(TRPNrTicketOwnershipTests PRIVATE TRPNeuralRuntime)
add_test(NAME NrTicketOwnership COMMAND TRPNrTicketOwnershipTests)
add_executable(TRPNrRuntimeParameterTests ../NrRuntimeParameterTests.cpp)
target_compile_features(TRPNrRuntimeParameterTests PRIVATE cxx_std_23)
target_compile_definitions(TRPNrRuntimeParameterTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
target_link_libraries(TRPNrRuntimeParameterTests PRIVATE TRPNeuralRuntime)
add_test(NAME NrRuntimeParameters COMMAND TRPNrRuntimeParameterTests)
add_executable(TRPNrMotionDiagnosticTests ../NrMotionDiagnosticTests.cpp)
target_compile_features(TRPNrMotionDiagnosticTests PRIVATE cxx_std_23)
target_compile_definitions(TRPNrMotionDiagnosticTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
target_include_directories(TRPNrMotionDiagnosticTests PRIVATE "${nrRoot}/src")
add_test(NAME NrMotionDiagnostic COMMAND TRPNrMotionDiagnosticTests)
add_executable(TRPNrRuntimeProbeReportTests ../NrRuntimeProbeReportTests.cpp RuntimeProbeReport.cpp)
target_compile_features(TRPNrRuntimeProbeReportTests PRIVATE cxx_std_23)
add_test(NAME NrRuntimeProbeReport COMMAND TRPNrRuntimeProbeReportTests)
set(TRP_NR_NGX_INCLUDE_DIR "" CACHE PATH "Pinned NGX SDK headers for direct NR research probe (no import library)")
if(TRP_NR_NGX_INCLUDE_DIR)
    add_executable(TRPNrAutomaticCoreTests ../NrAutomaticCoreTests.cpp)
    target_compile_features(TRPNrAutomaticCoreTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPNrAutomaticCoreTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_link_libraries(TRPNrAutomaticCoreTests PRIVATE TRPNeuralRuntime d3d11)
    add_test(NAME NrAutomaticCore COMMAND TRPNrAutomaticCoreTests "${TRP_NR_TEST_RUNTIME_ROOT}")
    set_tests_properties(NrAutomaticCore PROPERTIES RUN_SERIAL TRUE LABELS GPU TIMEOUT 30 SKIP_RETURN_CODE 77)
    add_executable(TRPNrInitializationFailureTests ../NrInitializationFailureTests.cpp)
    target_compile_features(TRPNrInitializationFailureTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPNrInitializationFailureTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPNrInitializationFailureTests PRIVATE "${TRP_NR_NGX_INCLUDE_DIR}")
    target_link_libraries(TRPNrInitializationFailureTests PRIVATE TRPNeuralRuntime)
    add_test(NAME NrInitializationFailure COMMAND TRPNrInitializationFailureTests)
    if(TRP_NR_TEST_RUNTIME_ROOT AND TRP_NR_DRIVER_CORE)
        foreach(layer IN ITEMS wrapper host)
            foreach(placement IN ITEMS before after)
                foreach(fault IN ITEMS allocate partial abi destroy create second-create null-feature)
                    add_test(NAME NrInitializationFailure-${layer}-${placement}-${fault} COMMAND TRPNrInitializationFailureTests
                        "${TRP_NR_TEST_RUNTIME_ROOT}" "${TRP_NR_DRIVER_CORE}" ${layer} ${placement} ${fault})
                    set_tests_properties(NrInitializationFailure-${layer}-${placement}-${fault} PROPERTIES RUN_SERIAL TRUE LABELS GPU TIMEOUT 45 SKIP_RETURN_CODE 77)
                endforeach()
            endforeach()
        endforeach()
        foreach(placement IN ITEMS before after)
            add_test(NAME NrInitializationFailure-host-${placement}-shutdown COMMAND TRPNrInitializationFailureTests
                "${TRP_NR_TEST_RUNTIME_ROOT}" "${TRP_NR_DRIVER_CORE}" host ${placement} shutdown)
            set_tests_properties(NrInitializationFailure-host-${placement}-shutdown PROPERTIES RUN_SERIAL TRUE LABELS GPU TIMEOUT 45 SKIP_RETURN_CODE 77)
        endforeach()
    endif()
    add_executable(TRPNrPerformanceQueriesTests ../NrPerformanceQueriesTests.cpp ../D3D11QueryReadinessProbe.cpp)
    target_compile_features(TRPNrPerformanceQueriesTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPNrPerformanceQueriesTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_link_libraries(TRPNrPerformanceQueriesTests PRIVATE TRPNeuralRuntime d3d11)
    add_test(NAME NrPerformanceQueries COMMAND TRPNrPerformanceQueriesTests)
    set_tests_properties(NrPerformanceQueries PROPERTIES RUN_SERIAL TRUE LABELS GPU TIMEOUT 30)
    trp_nr_enable_source_stage("${TRP_NR_NGX_INCLUDE_DIR}")
    # Historical shader utility coverage; the product no longer links this resolve.
    add_executable(TRPNrStableColorTests ../NrStableColorTests.cpp ../../src/NeuralRendering/StableColorResolve.cpp)
    target_compile_features(TRPNrStableColorTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPNrStableColorTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_link_libraries(TRPNrStableColorTests PRIVATE TRPNeuralRuntime)
    add_test(NAME NrStableColors COMMAND TRPNrStableColorTests)
    set_tests_properties(NrStableColors PROPERTIES RUN_SERIAL TRUE LABELS GPU TIMEOUT 30)
    add_executable(TRPNrToneContextProbe ../../research/nr/tone-context/ContextProbe.cpp)
    target_compile_features(TRPNrToneContextProbe PRIVATE cxx_std_23)
    target_compile_definitions(TRPNrToneContextProbe PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPNrToneContextProbe PRIVATE "${nrRoot}/tests")
    target_link_libraries(TRPNrToneContextProbe PRIVATE TRPNeuralRuntime)
    foreach(mode IN ITEMS vendor off)
        add_test(NAME NrToneContext-${mode} COMMAND TRPNrToneContextProbe "${TRP_NR_TEST_RUNTIME_ROOT}" "${TRP_NR_DRIVER_CORE}" "${CMAKE_CURRENT_BINARY_DIR}/tone-context/${mode}" ${mode})
        set_tests_properties(NrToneContext-${mode} PROPERTIES SKIP_RETURN_CODE 77 TIMEOUT 60 RUN_SERIAL TRUE LABELS GPU)
    endforeach()
    add_executable(TRPNrBeforeUpscaleTests ../NrBeforeUpscaleTests.cpp)
    target_compile_features(TRPNrBeforeUpscaleTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPNrBeforeUpscaleTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_link_libraries(TRPNrBeforeUpscaleTests PRIVATE TRPNeuralRuntime d3d11)
    add_test(NAME NrBeforeUpscale COMMAND TRPNrBeforeUpscaleTests "${TRP_NR_RTX40_DLL}" "${TRP_NR_DRIVER_CORE}")
    set_tests_properties(NrBeforeUpscale PROPERTIES SKIP_RETURN_CODE 77 TIMEOUT 180 RUN_SERIAL TRUE LABELS GPU)
    add_test(NAME NrBeforeUpscale-wait-failure COMMAND TRPNrBeforeUpscaleTests "${TRP_NR_RTX40_DLL}" "${TRP_NR_DRIVER_CORE}" wait-failure)
    set_tests_properties(NrBeforeUpscale-wait-failure PROPERTIES SKIP_RETURN_CODE 77 TIMEOUT 30 RUN_SERIAL TRUE LABELS GPU)
    foreach(mode IN ITEMS copy-without-rt chain-two chain-three)
        add_test(NAME NrBeforeUpscale-${mode} COMMAND TRPNrBeforeUpscaleTests "${TRP_NR_RTX40_DLL}" "${TRP_NR_DRIVER_CORE}" ${mode})
        set_tests_properties(NrBeforeUpscale-${mode} PROPERTIES SKIP_RETURN_CODE 77 TIMEOUT 180 RUN_SERIAL TRUE LABELS GPU)
    endforeach()
    add_executable(TRPNrPreparedBeforeTests ../NrPreparedBeforeTests.cpp)
    add_executable(TRPNrDeferredDeliveryTests ../NrDeferredDeliveryTests.cpp)
    add_executable(TRPNrCapacityProgressTests ../NrCapacityProgressTests.cpp)
    target_compile_features(TRPNrCapacityProgressTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPNrCapacityProgressTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_link_libraries(TRPNrCapacityProgressTests PRIVATE TRPNeuralRuntime)
    foreach(mode IN ITEMS before prepared)
        add_test(NAME NrCapacityProgress-${mode} COMMAND TRPNrCapacityProgressTests "${TRP_NR_RTX40_DLL}" "${TRP_NR_DRIVER_CORE}" ${mode})
        set_tests_properties(NrCapacityProgress-${mode} PROPERTIES SKIP_RETURN_CODE 77 TIMEOUT 90 RUN_SERIAL TRUE LABELS GPU)
    endforeach()
    target_compile_features(TRPNrDeferredDeliveryTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPNrDeferredDeliveryTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_link_libraries(TRPNrDeferredDeliveryTests PRIVATE TRPNeuralRuntime)
    foreach(mode IN ITEMS pressure retire timeout style)
        add_test(NAME NrDeferredDelivery-${mode} COMMAND TRPNrDeferredDeliveryTests "${TRP_NR_RTX40_DLL}" "${TRP_NR_DRIVER_CORE}" ${mode})
        set_tests_properties(NrDeferredDelivery-${mode} PROPERTIES SKIP_RETURN_CODE 77 TIMEOUT 90 RUN_SERIAL TRUE LABELS GPU)
    endforeach()
    target_compile_features(TRPNrPreparedBeforeTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPNrPreparedBeforeTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_link_libraries(TRPNrPreparedBeforeTests PRIVATE TRPNeuralRuntime)
    add_test(NAME NrPreparedBefore COMMAND TRPNrPreparedBeforeTests "${TRP_NR_RTX40_DLL}" "${TRP_NR_DRIVER_CORE}")
    set_tests_properties(NrPreparedBefore PROPERTIES SKIP_RETURN_CODE 77 TIMEOUT 180 RUN_SERIAL TRUE LABELS GPU)
    add_test(NAME NrPreparedBefore-sdr-bytes COMMAND TRPNrPreparedBeforeTests "${TRP_NR_RTX40_DLL}" "${TRP_NR_DRIVER_CORE}" --sdr-bytes)
    set_tests_properties(NrPreparedBefore-sdr-bytes PROPERTIES SKIP_RETURN_CODE 77 TIMEOUT 180 RUN_SERIAL TRUE LABELS GPU)
    add_executable(TRPNrBeforeHostTests ../NrBeforeHostTests.cpp)
    target_compile_features(TRPNrBeforeHostTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPNrBeforeHostTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_link_libraries(TRPNrBeforeHostTests PRIVATE TRPNeuralRuntime)
    add_executable(TRPNrBeforeHostFramesTests ../NrBeforeHostFramesTests.cpp)
    target_compile_features(TRPNrBeforeHostFramesTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPNrBeforeHostFramesTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_link_libraries(TRPNrBeforeHostFramesTests PRIVATE TRPNeuralRuntime)
    set(TRP_NR_TEST_RUNTIME_ROOT "" CACHE PATH "Controlled three-profile NR fixture root")
    set(TRP_NR_RESHADE_RUNTIME "" CACHE FILEPATH "Supplied ReShade DLL for isolated NR Before/fence regression")
    if(TRP_NR_TEST_RUNTIME_ROOT)
        add_test(NAME NrBeforeHostFrames COMMAND TRPNrBeforeHostFramesTests "${TRP_NR_TEST_RUNTIME_ROOT}" "${TRP_NR_DRIVER_CORE}")
        set_tests_properties(NrBeforeHostFrames PROPERTIES SKIP_RETURN_CODE 77 TIMEOUT 180 RUN_SERIAL TRUE LABELS GPU)
        add_test(NAME NrBeforeHostFrames-sdr-bytes COMMAND TRPNrBeforeHostFramesTests "${TRP_NR_TEST_RUNTIME_ROOT}" "${TRP_NR_DRIVER_CORE}" "${CMAKE_CURRENT_BINARY_DIR}/sdr-host.json" --sdr-bytes)
        set_tests_properties(NrBeforeHostFrames-sdr-bytes PROPERTIES SKIP_RETURN_CODE 77 TIMEOUT 180 RUN_SERIAL TRUE LABELS GPU)
        add_test(NAME NrPostSrHost-switch COMMAND TRPNrBeforeHostFramesTests "${TRP_NR_TEST_RUNTIME_ROOT}" "${TRP_NR_DRIVER_CORE}" "${CMAKE_CURRENT_BINARY_DIR}/post-sr-host.json" --sdr-bytes --post-sr)
        set_tests_properties(NrPostSrHost-switch PROPERTIES SKIP_RETURN_CODE 77 TIMEOUT 180 RUN_SERIAL TRUE LABELS GPU)
        add_test(NAME NrPostSrHost-live-settings COMMAND TRPNrBeforeHostFramesTests "${TRP_NR_TEST_RUNTIME_ROOT}" "${TRP_NR_DRIVER_CORE}" "${CMAKE_CURRENT_BINARY_DIR}/post-sr-live-settings.json" --sdr-bytes --post-sr --live-settings)
        set_tests_properties(NrPostSrHost-live-settings PROPERTIES SKIP_RETURN_CODE 77 TIMEOUT 180 RUN_SERIAL TRUE LABELS GPU)
        add_test(NAME NrPostSrHost-pass-cycle COMMAND TRPNrBeforeHostFramesTests "${TRP_NR_TEST_RUNTIME_ROOT}" "${TRP_NR_DRIVER_CORE}" "${CMAKE_CURRENT_BINARY_DIR}/post-sr-pass-cycle.json" --sdr-bytes --post-sr --pass-cycle)
        set_tests_properties(NrPostSrHost-pass-cycle PROPERTIES SKIP_RETURN_CODE 77 TIMEOUT 180 RUN_SERIAL TRUE LABELS GPU)
        add_test(NAME NrBeforeHost COMMAND TRPNrBeforeHostTests "${TRP_NR_TEST_RUNTIME_ROOT}" "${TRP_NR_DRIVER_CORE}")
        set_tests_properties(NrBeforeHost PROPERTIES SKIP_RETURN_CODE 77 TIMEOUT 180 RUN_SERIAL TRUE LABELS GPU)
        if(TRP_NR_RESHADE_RUNTIME)
            find_package(Python3 COMPONENTS Interpreter REQUIRED)
            add_test(NAME NrReShadeBefore COMMAND "${Python3_EXECUTABLE}"
                "${CMAKE_CURRENT_LIST_DIR}/run_reshade_before.py"
                --packet-exe "$<TARGET_FILE:TRPNrImagePacketTests>"
                --host-exe "$<TARGET_FILE:TRPNrBeforeHostFramesTests>"
                --deferred-exe "$<TARGET_FILE:TRPNrDeferredDeliveryTests>"
                --runtime "${TRP_NR_RESHADE_RUNTIME}" --runtime-root "${TRP_NR_TEST_RUNTIME_ROOT}"
                --driver-core "${TRP_NR_DRIVER_CORE}" --output "${CMAKE_CURRENT_BINARY_DIR}/reshade-before")
            set_tests_properties(NrReShadeBefore PROPERTIES TIMEOUT 240 RUN_SERIAL TRUE LABELS "GPU;ReShade")
            add_test(NAME NrReShadeSdrBefore COMMAND "${Python3_EXECUTABLE}"
                "${CMAKE_CURRENT_LIST_DIR}/run_reshade_before.py"
                --packet-exe "$<TARGET_FILE:TRPNrImagePacketTests>" --host-exe "$<TARGET_FILE:TRPNrBeforeHostFramesTests>"
                --runtime "${TRP_NR_RESHADE_RUNTIME}" --runtime-root "${TRP_NR_TEST_RUNTIME_ROOT}"
                --driver-core "${TRP_NR_DRIVER_CORE}" --output "${CMAKE_CURRENT_BINARY_DIR}/reshade-sdr-before" --sdr-bytes)
            set_tests_properties(NrReShadeSdrBefore PROPERTIES TIMEOUT 240 RUN_SERIAL TRUE LABELS "GPU;ReShade")
            add_test(NAME NrReShadePostSr-switch COMMAND "${Python3_EXECUTABLE}"
                "${CMAKE_CURRENT_LIST_DIR}/run_reshade_before.py"
                --packet-exe "$<TARGET_FILE:TRPNrImagePacketTests>" --host-exe "$<TARGET_FILE:TRPNrBeforeHostFramesTests>"
                --runtime "${TRP_NR_RESHADE_RUNTIME}" --runtime-root "${TRP_NR_TEST_RUNTIME_ROOT}"
                --driver-core "${TRP_NR_DRIVER_CORE}" --output "${CMAKE_CURRENT_BINARY_DIR}/reshade-post-sr" --sdr-bytes --post-sr)
            set_tests_properties(NrReShadePostSr-switch PROPERTIES TIMEOUT 240 RUN_SERIAL TRUE LABELS "GPU;ReShade")
            add_test(NAME NrReShadePostSr-live-settings COMMAND "${Python3_EXECUTABLE}"
                "${CMAKE_CURRENT_LIST_DIR}/run_reshade_before.py"
                --packet-exe "$<TARGET_FILE:TRPNrImagePacketTests>" --host-exe "$<TARGET_FILE:TRPNrBeforeHostFramesTests>"
                --runtime "${TRP_NR_RESHADE_RUNTIME}" --runtime-root "${TRP_NR_TEST_RUNTIME_ROOT}"
                --driver-core "${TRP_NR_DRIVER_CORE}" --output "${CMAKE_CURRENT_BINARY_DIR}/reshade-post-sr-live-settings" --sdr-bytes --post-sr --live-settings)
            set_tests_properties(NrReShadePostSr-live-settings PROPERTIES TIMEOUT 240 RUN_SERIAL TRUE LABELS "GPU;ReShade")
        endif()
    endif()
    add_executable(TRPNrStageTests ../NrStageTests.cpp)
    add_executable(TRPNrFrameSlotTests ../NrFrameSlotTests.cpp)
    target_compile_features(TRPNrFrameSlotTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPNrFrameSlotTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPNrFrameSlotTests PRIVATE "${TRP_NR_NGX_INCLUDE_DIR}")
    target_link_libraries(TRPNrFrameSlotTests PRIVATE TRPNeuralRuntime)
    foreach(mode IN ITEMS output unsubmitted)
        add_test(NAME NrFrameSlot-${mode} COMMAND TRPNrFrameSlotTests "${TRP_NR_RTX40_DLL}" "${TRP_NR_DRIVER_CORE}" ${mode})
        set_tests_properties(NrFrameSlot-${mode} PROPERTIES SKIP_RETURN_CODE 77 TIMEOUT 90 RUN_SERIAL TRUE LABELS GPU)
    endforeach()
    target_compile_features(TRPNrStageTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPNrStageTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPNrStageTests PRIVATE "${TRP_NR_NGX_INCLUDE_DIR}")
    target_link_libraries(TRPNrStageTests PRIVATE TRPNeuralRuntime)
    foreach(mode IN ITEMS unsubmitted record-failure queued-admission chain-two chain-three chain-failure)
        add_test(NAME NrStage-${mode} COMMAND TRPNrStageTests "${TRP_NR_RTX40_DLL}" "${TRP_NR_DRIVER_CORE}" ${mode})
        set_tests_properties(NrStage-${mode} PROPERTIES SKIP_RETURN_CODE 77 TIMEOUT 30 RUN_SERIAL TRUE LABELS GPU)
    endforeach()
    set(nrProbeSources tests/nr-runtime/RuntimeProbe.cpp tests/nr-runtime/RuntimeProbeReport.cpp
        tests/nr-postfg/SyntheticScene.h src/NeuralRendering/PostSrContract.h
        tests/nr-runtime/ObservedQueue.h tests/NrStageTests.cpp tests/NrFrameSlotTests.cpp tests/NrDeferredDeliveryTests.cpp
        tests/nr-runtime/PerformanceProbe.cpp tests/NrStableColorTests.cpp
        src/NeuralRendering/StableColorResolve.h src/NeuralRendering/StableColorResolve.cpp
        research/nr/tone-context/ContextProbe.cpp tools/nr/Test-NrToneStability.py
        src/NeuralRendering/PerformanceMetrics.h src/NeuralRendering/PerformanceMetrics.cpp
        src/NeuralRendering/PerformanceQueries.h src/NeuralRendering/PerformanceQueries.cpp
        tests/nr-runtime/RuntimeProbeReport.h tests/nr-runtime/GpuProbeGuard.h tests/nr-runtime/run_reshade_before.py tests/NrImagePacketTests.cpp src/NeuralRendering/RuntimeCatalog.cpp src/NeuralRendering/RuntimeCatalog.h
        src/NeuralRendering/RuntimeFileLease.cpp src/NeuralRendering/RuntimeFileLease.h
        src/NeuralRendering/DriverCoreTrust.cpp src/NeuralRendering/DriverCoreTrust.h
        src/NvidiaAppSettings.cpp src/NvidiaAppSettings.h src/NvidiaAppSettingsPolicy.h
        src/NvidiaDriverSettings.h src/NvidiaResolverImports.h cmake/NvidiaAppSettings.cmake
        src/NeuralRendering/RuntimeOwner.cpp src/NeuralRendering/RuntimeOwner.h
        src/NeuralRendering/CallerIdentityShim.cpp src/NeuralRendering/CallerIdentityShim.h
        src/NeuralRendering/Error.h src/NeuralRendering/ImagePacket.h src/NeuralRendering/ImagePacket.cpp
        src/NeuralRendering/History.h src/NeuralRendering/History.cpp src/NeuralRendering/RuntimeParameters.h
        src/NeuralRendering/MotionDiagnosticStats.h tests/NrMotionDiagnosticTests.cpp
        src/NeuralRendering/Stage.h src/NeuralRendering/Stage.cpp src/NeuralRendering/TicketOwnership.h src/NeuralRendering/RetirementEvent.h
        src/NeuralRendering/BeforeUpscale.h src/NeuralRendering/BeforeUpscale.cpp tests/NrBeforeUpscaleTests.cpp
        src/Graphics/D3D11D3D12Interop.h src/Graphics/D3D11D3D12Interop.cpp src/FrameGen/D3D11FrameCopy.h src/FrameGen/NeuralRenderingRuntimeContract.h
        src/FrameGen/NeuralRenderingTuning.h src/FrameGen/NeuralRenderingReconstruction.h
        src/NeuralRendering/PreparedBeforeUpscale.h src/NeuralRendering/PreparedBeforeUpscale.cpp src/NeuralRendering/PreparedFsrInput.h tests/NrPreparedFsrTests.cpp
        src/NeuralRendering/PostSrContract.h src/NeuralRendering/PostUpscale.h src/NeuralRendering/PostUpscale.cpp
        tests/nr-postsr/CMakeLists.txt tests/nr-postsr/PostSrContractTests.cpp tests/nr-postsr/PostSrGuideTests.cpp
        tests/nr-postsr/PostSrNativeGpuTests.cpp tests/nr-postsr/PostUpscaleGpuTests.cpp
        src/FrameGen/SourceFrameEvaluator.h src/FrameGen/SourceNvidiaFrameEvaluator.h
        src/NeuralRendering/BeforeHost.h src/NeuralRendering/BeforeHost.cpp src/NeuralRendering/BeforeSettings.h
        src/NeuralRendering/StartupSettings.h src/NeuralRendering/SourcePolicy.h tests/NrPreparedBeforeTests.cpp
        tests/NrBeforeHostFramesTests.cpp tests/NrBeforeHostTests.cpp tests/NrSourcePolicyTests.cpp
        src/Upscaling/FSRColorConversion.h src/Upscaling/FSRColorConversion.cpp src/Upscaling/FSRColorContract.h
        src/Upscaling/FSRHistoryPolicy.h src/FrameGen/NvidiaHostNeural.cpp src/FrameGen/NvidiaHost.h
        src/FrameGen/NvidiaHostStartup.cpp src/FrameGen/NvidiaHostLifecycle.cpp src/FrameGen/SourceFsrEvaluation.cpp
        src/FrameGen/SourceNvidiaEvaluation.cpp src/FrameGen/SourceFrameGeneration.h
        src/RendererSettings.h src/RendererSettingsController.cpp src/RendererBackendPolicy.h
        src/RendererSettingsController.h src/RendererSettingsAction.h src/NvidiaUpscalerConfiguration.h
        tests/settings-controller/CMakeLists.txt tests/settings-controller/ControllerTests.cpp tests/settings-controller/Services.cpp
        tests/settings-controller/facades/PCH.h tests/settings-controller/facades/RenderPipeline.h
        tests/settings-controller/facades/FrameGen/NvidiaHost.h tests/settings-controller/facades/FrameGen/SourceDLSSGBackend.h
        tests/settings-controller/facades/PerformanceTuning.h tests/settings-controller/facades/CommunityShaderIntegration.h
        tests/settings-controller/facades/WeatherAppearanceRuntime.h
        src/FrameGen/SourceUpscalerConfiguration.cpp
        tests/settings-controller/DeferredUpscalerTests.cpp tests/settings-controller/DeferredUpscalerGpuTests.cpp
        tests/settings-controller/facades/NvidiaHost.h tests/settings-controller/facades/SourceDLSSGBackend.h
        tests/settings-controller/facades/DLSSBackend.h
        src/OverlayNeuralPanel.cpp src/OverlayNeuralTab.h src/OverlayFrameView.cpp src/OverlayUI.cpp
        CMakeLists.txt src/HookSafety.h cmake/NeuralRuntime.cmake tests/nr-runtime/CMakeLists.txt)
    if(TRP_NR_PERF_WITH_FSR)
    set(TRP_NR_PERF_FSR_RUNTIME "" CACHE PATH "Pinned FSR DLL fixture root containing FSR/")
        list(APPEND nrProbeSources tests/nr-runtime/FsrPerformanceRecorder.h
            src/Upscaling/FSRRuntime.h src/Upscaling/FSRRuntime.cpp src/Upscaling/FSRParameters.cpp
            src/Upscaling/FSRUpscaler.h src/Upscaling/FSRUpscaler.cpp src/Upscaling/FSRHostResources.h
            src/Upscaling/FSRHostResources.cpp src/Upscaling/FSRFrameAdapter.h src/Upscaling/FSRFrameAdapter.cpp
            src/Upscaling/FSRParameters.h
            src/Upscaling/UpscalerBackend.h src/Upscaling/FSRPreparedResources.h
            src/Upscaling/FSRProviderPolicy.h src/RendererBackendPolicy.h tools/fsr/runtime-pin.json)
        list(REMOVE_DUPLICATES nrProbeSources)
        file(READ "${nrRoot}/tools/fsr/runtime-pin.json" fsrPin)
        foreach(index RANGE 0 3)
            string(JSON headerPath GET "${fsrPin}" headers ${index} path)
            string(JSON expectedHash GET "${fsrPin}" headers ${index} sha256)
            file(SHA256 "${TRP_NR_PERF_FSR_SDK}/${headerPath}" actualHash)
            set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${TRP_NR_PERF_FSR_SDK}/${headerPath}")
            if(NOT actualHash STREQUAL expectedHash)
                message(FATAL_ERROR "Pinned FSR performance header mismatch: ${headerPath}")
            endif()
        endforeach()
    endif()
    set(nrProbeSourcesJson "{")
    set(nrProbeSeparator "")
    if(TRP_NR_PERF_WITH_FSR)
        foreach(index RANGE 0 3)
            string(JSON headerPath GET "${fsrPin}" headers ${index} path)
            string(JSON expectedHash GET "${fsrPin}" headers ${index} sha256)
            string(APPEND nrProbeSourcesJson "${nrProbeSeparator}\"external/${headerPath}\":\"${expectedHash}\"")
            set(nrProbeSeparator ",")
        endforeach()
    endif()
    foreach(nrProbeSource IN LISTS nrProbeSources)
        file(SHA256 "${nrRoot}/${nrProbeSource}" nrProbeHash)
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${nrRoot}/${nrProbeSource}")
        string(APPEND nrProbeSourcesJson "${nrProbeSeparator}\"${nrProbeSource}\":\"${nrProbeHash}\"")
        set(nrProbeSeparator ",")
    endforeach()
    file(SHA256 "${TRP_NR_NGX_INCLUDE_DIR}/nvsdk_ngx_params.h" nrProbeParamsHash)
    string(APPEND nrProbeSourcesJson ",\"external/nvsdk_ngx_params.h\":\"${nrProbeParamsHash}\"")
    execute_process(COMMAND git rev-parse HEAD WORKING_DIRECTORY "${nrRoot}" OUTPUT_VARIABLE nrProbeRevision OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND git status --porcelain --untracked-files=normal WORKING_DIRECTORY "${nrRoot}" OUTPUT_VARIABLE nrProbeDirty OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(nrProbeClean true)
    if(nrProbeDirty OR NOT nrProbeRevision)
        set(nrProbeClean false)
    endif()
    target_include_directories(TRPNrBeforeUpscaleTests PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
    target_include_directories(TRPNrBeforeHostFramesTests PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
    if(TARGET TRPSettingsControllerFixture)
        add_executable(TRPNrControllerApplyGpuTests ../NrBeforeHostFramesTests.cpp)
        target_compile_definitions(TRPNrControllerApplyGpuTests PRIVATE TRP_CONTROLLER_APPLY_PROBE)
        target_include_directories(TRPNrControllerApplyGpuTests PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
        target_link_libraries(TRPNrControllerApplyGpuTests PRIVATE TRPSettingsControllerFixture TRPNeuralRuntime)
        add_test(NAME NrControllerApplyGpu COMMAND TRPNrControllerApplyGpuTests "${TRP_NR_TEST_RUNTIME_ROOT}" "${TRP_NR_DRIVER_CORE}"
            "${CMAKE_CURRENT_BINARY_DIR}/controller-apply.json" --sdr-bytes --post-sr --live-settings)
        set_tests_properties(NrControllerApplyGpu PROPERTIES SKIP_RETURN_CODE 77 TIMEOUT 180 RUN_SERIAL TRUE LABELS GPU)
        add_test(NAME NrControllerApplyGpu-pass-cycle COMMAND TRPNrControllerApplyGpuTests "${TRP_NR_TEST_RUNTIME_ROOT}" "${TRP_NR_DRIVER_CORE}"
            "${CMAKE_CURRENT_BINARY_DIR}/controller-apply-pass-cycle.json" --sdr-bytes --post-sr --pass-cycle)
        set_tests_properties(NrControllerApplyGpu-pass-cycle PROPERTIES SKIP_RETURN_CODE 77 TIMEOUT 180 RUN_SERIAL TRUE LABELS GPU)
        if(TRP_NR_RESHADE_RUNTIME AND TRP_NR_TEST_RUNTIME_ROOT AND EXISTS "${TRP_NR_RESHADE_RUNTIME}")
            add_test(NAME NrControllerApplyReShadeGpu COMMAND "${Python3_EXECUTABLE}"
                "${CMAKE_CURRENT_LIST_DIR}/run_reshade_before.py"
                --packet-exe "$<TARGET_FILE:TRPNrImagePacketTests>" --host-exe "$<TARGET_FILE:TRPNrControllerApplyGpuTests>"
                --runtime "${TRP_NR_RESHADE_RUNTIME}" --runtime-root "${TRP_NR_TEST_RUNTIME_ROOT}"
                --driver-core "${TRP_NR_DRIVER_CORE}" --output "${CMAKE_CURRENT_BINARY_DIR}/reshade-controller-apply" --sdr-bytes --post-sr --live-settings)
            set_tests_properties(NrControllerApplyReShadeGpu PROPERTIES TIMEOUT 240 RUN_SERIAL TRUE LABELS "GPU;ReShade")
            add_test(NAME NrControllerApplyReShadeGpu-pass-cycle COMMAND "${Python3_EXECUTABLE}"
                "${CMAKE_CURRENT_LIST_DIR}/run_reshade_before.py"
                --packet-exe "$<TARGET_FILE:TRPNrImagePacketTests>" --host-exe "$<TARGET_FILE:TRPNrControllerApplyGpuTests>"
                --runtime "${TRP_NR_RESHADE_RUNTIME}" --runtime-root "${TRP_NR_TEST_RUNTIME_ROOT}"
                --driver-core "${TRP_NR_DRIVER_CORE}" --output "${CMAKE_CURRENT_BINARY_DIR}/reshade-controller-apply-pass-cycle" --sdr-bytes --post-sr --pass-cycle)
            set_tests_properties(NrControllerApplyReShadeGpu-pass-cycle PROPERTIES TIMEOUT 240 RUN_SERIAL TRUE LABELS "GPU;ReShade")
        endif()
    endif()
    add_executable(TRPNrRuntimeProbe RuntimeProbe.cpp RuntimeProbeReport.cpp)
    target_compile_features(TRPNrRuntimeProbe PRIVATE cxx_std_23)
    target_compile_definitions(TRPNrRuntimeProbe PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPNrRuntimeProbe PRIVATE "${nrRoot}/src" "${TRP_NR_NGX_INCLUDE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")
    target_link_libraries(TRPNrRuntimeProbe PRIVATE TRPNeuralRuntime)
    foreach(guideCase IN ITEMS native half nonintegral)
        if(guideCase STREQUAL native)
            set(guideWidth 640)
            set(guideHeight 360)
        elseif(guideCase STREQUAL half)
            set(guideWidth 320)
            set(guideHeight 180)
        else()
            set(guideWidth 427)
            set(guideHeight 203)
        endif()
        add_test(NAME NrScaledContractGpu-${guideCase} COMMAND TRPNrRuntimeProbe
            --dll "${TRP_NR_RTX40_DLL}" --core "${TRP_NR_DRIVER_CORE}" --profile rtx40
            --caller-shim on --frames 30 --moving-scene on --guide-width ${guideWidth} --guide-height ${guideHeight}
            --output "${CMAKE_CURRENT_BINARY_DIR}/scaled-contract-${guideCase}.json")
        set_tests_properties(NrScaledContractGpu-${guideCase} PROPERTIES TIMEOUT 90 RUN_SERIAL TRUE LABELS GPU)
    endforeach()
    add_test(NAME NrPendingProducerOutput COMMAND TRPNrRuntimeProbe
        --dll "${TRP_NR_RTX40_DLL}" --core "${TRP_NR_DRIVER_CORE}" --profile rtx40
        --caller-shim on --frames 6 --queued-producer on --output "${CMAKE_CURRENT_BINARY_DIR}/pending-producer.json")
    set_tests_properties(NrPendingProducerOutput PROPERTIES TIMEOUT 90 RUN_SERIAL TRUE LABELS GPU)
    # Isolated executable only: reproduce the production catalog source with
    # one exact research model with direct/shim policies. Shipping pins stay immutable.
    file(READ "${nrRoot}/src/NeuralRendering/RuntimeCatalog.cpp" nrPerformanceCatalog)
    set(nrPerformanceEntry "RuntimeProfile{\"aio19-rtx40\",\"research/aio19/nvngx_dlssnr.dll\",\"8270b350cd82de5ce89806872cdd6b6a9249b80836b91bbeb3573470744cc206\",165840496,GpuFamily::Rtx40,false,CompatibilityPolicy::SignedDirect},")
    # Direct Init_Ext reproduced BAD00002 in an isolated process. This second
    # exact artifact profile tests the existing ownership-aware caller shim.
    string(APPEND nrPerformanceEntry "\nRuntimeProfile{\"aio19-rtx40-shim\",\"research/aio19/nvngx_dlssnr.dll\",\"8270b350cd82de5ce89806872cdd6b6a9249b80836b91bbeb3573470744cc206\",165840496,GpuFamily::Rtx40,false,CompatibilityPolicy::CallerIdentityProbeRequired},")
    string(REPLACE "constexpr std::array profiles{" "constexpr std::array profiles{\n${nrPerformanceEntry}\n" nrPerformanceCatalog "${nrPerformanceCatalog}")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/PerformanceRuntimeCatalog.cpp" "${nrPerformanceCatalog}")
    file(SHA256 "${CMAKE_CURRENT_BINARY_DIR}/PerformanceRuntimeCatalog.cpp" nrPerformanceCatalogHash)
    string(APPEND nrProbeSourcesJson ",\"generated/PerformanceRuntimeCatalog.cpp\":\"${nrPerformanceCatalogHash}\"}")
    configure_file(ProbeBuildIdentity.h.in "${CMAKE_CURRENT_BINARY_DIR}/ProbeBuildIdentity.h" @ONLY)
    get_target_property(nrPerformanceSources TRPNeuralRuntime SOURCES)
    list(REMOVE_ITEM nrPerformanceSources "${nrRoot}/src/NeuralRendering/RuntimeCatalog.cpp")
    add_library(TRPNrPerformanceRuntime STATIC ${nrPerformanceSources} "${CMAKE_CURRENT_BINARY_DIR}/PerformanceRuntimeCatalog.cpp")
    target_compile_features(TRPNrPerformanceRuntime PUBLIC cxx_std_23)
    target_compile_definitions(TRPNrPerformanceRuntime PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPNrPerformanceRuntime PUBLIC "${nrRoot}/src" PRIVATE "${TRP_NR_NGX_INCLUDE_DIR}" "${nrRoot}/src/NeuralRendering")
    target_link_libraries(TRPNrPerformanceRuntime PUBLIC d3d12 dxgi bcrypt wintrust crypt32 version d3d11 d3dcompiler TRPNvidiaAppSettings)
    add_executable(TRPNrPerformanceProbe PerformanceProbe.cpp)
    target_compile_features(TRPNrPerformanceProbe PRIVATE cxx_std_23)
    target_compile_definitions(TRPNrPerformanceProbe PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPNrPerformanceProbe PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
    target_link_libraries(TRPNrPerformanceProbe PRIVATE TRPNrPerformanceRuntime winmm)
    if(TRP_NR_PERF_WITH_FSR)
    add_library(TRPNrPerformanceFsr STATIC
        "${nrRoot}/src/Upscaling/FSRRuntime.cpp" "${nrRoot}/src/Upscaling/FSRParameters.cpp"
        "${nrRoot}/src/Upscaling/FSRUpscaler.cpp" "${nrRoot}/src/Upscaling/FSRHostResources.cpp"
        "${nrRoot}/src/Upscaling/FSRFrameAdapter.cpp")
    target_compile_features(TRPNrPerformanceFsr PUBLIC cxx_std_23)
    target_compile_definitions(TRPNrPerformanceFsr PUBLIC NOMINMAX WIN32_LEAN_AND_MEAN TRP_ENABLE_FSR)
    target_include_directories(TRPNrPerformanceFsr PUBLIC "${nrRoot}/src"
        "${TRP_NR_PERF_FSR_SDK}/Kits/FidelityFX/api/include" "${TRP_NR_PERF_FSR_SDK}/Kits/FidelityFX/upscalers/include")
    target_link_libraries(TRPNrPerformanceProbe PRIVATE TRPNrPerformanceFsr)
    target_compile_definitions(TRPNrPerformanceProbe PRIVATE TRP_NR_PERF_WITH_FSR)
    add_executable(TRPNrPreparedFsrTests ../NrPreparedFsrTests.cpp)
    target_compile_features(TRPNrPreparedFsrTests PRIVATE cxx_std_23)
    target_link_libraries(TRPNrPreparedFsrTests PRIVATE TRPNrPerformanceRuntime TRPNrPerformanceFsr)
    target_include_directories(TRPNrPreparedFsrTests PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
    foreach(mode IN ITEMS native quality gated abandon sdr-bytes)
        add_test(NAME NrPreparedFsr-${mode} COMMAND TRPNrPreparedFsrTests "${TRP_NR_RTX40_DLL}" "${TRP_NR_DRIVER_CORE}" "${TRP_NR_PERF_FSR_RUNTIME}" ${mode})
        set_tests_properties(NrPreparedFsr-${mode} PROPERTIES RUN_SERIAL TRUE LABELS GPU TIMEOUT 90 SKIP_RETURN_CODE 77)
    endforeach()
    add_library(TRPNrFsrRecoveryDouble SHARED ../fixtures/fsr/RuntimeDouble.cpp)
    target_compile_features(TRPNrFsrRecoveryDouble PRIVATE cxx_std_23)
    target_compile_definitions(TRPNrFsrRecoveryDouble PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN FSR_MISSING_EXPORT=0)
    target_include_directories(TRPNrFsrRecoveryDouble PRIVATE "${TRP_NR_PERF_FSR_SDK}/Kits/FidelityFX/api/include" "${TRP_NR_PERF_FSR_SDK}/Kits/FidelityFX/upscalers/include")
    set(recoveryDir "${CMAKE_CURRENT_BINARY_DIR}/nr-fsr-recovery/FSR")
    set_target_properties(TRPNrFsrRecoveryDouble PROPERTIES OUTPUT_NAME amd_fidelityfx_loader_dx12 RUNTIME_OUTPUT_DIRECTORY_RELEASE "${recoveryDir}")
    add_custom_command(TARGET TRPNrFsrRecoveryDouble POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:TRPNrFsrRecoveryDouble>" "${recoveryDir}/amd_fidelityfx_upscaler_dx12.dll")
    add_dependencies(TRPNrPreparedFsrTests TRPNrFsrRecoveryDouble)
    add_test(NAME NrPreparedFsr-recovery COMMAND TRPNrPreparedFsrTests "${TRP_NR_RTX40_DLL}" "${TRP_NR_DRIVER_CORE}" "${CMAKE_CURRENT_BINARY_DIR}/nr-fsr-recovery" recovery)
    set_tests_properties(NrPreparedFsr-recovery PROPERTIES RUN_SERIAL TRUE LABELS GPU TIMEOUT 90 SKIP_RETURN_CODE 77)
    if(TRP_NR_RESHADE_RUNTIME AND TRP_NR_TEST_RUNTIME_ROOT AND TRP_NR_PERF_FSR_RUNTIME)
        add_test(NAME NrReShadePreparedFsr COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/run_reshade_before.py"
            --packet-exe "$<TARGET_FILE:TRPNrImagePacketTests>" --host-exe "$<TARGET_FILE:TRPNrBeforeHostFramesTests>"
            --prepared-fsr-exe "$<TARGET_FILE:TRPNrPreparedFsrTests>" --fsr-runtime "${TRP_NR_PERF_FSR_RUNTIME}"
            --runtime "${TRP_NR_RESHADE_RUNTIME}" --runtime-root "${TRP_NR_TEST_RUNTIME_ROOT}"
            --driver-core "${TRP_NR_DRIVER_CORE}" --output "${CMAKE_CURRENT_BINARY_DIR}/reshade-prepared-fsr")
        set_tests_properties(NrReShadePreparedFsr PROPERTIES RUN_SERIAL TRUE TIMEOUT 240 LABELS "GPU;ReShade")
        add_test(NAME NrReShadeSdrFsr COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/run_reshade_before.py"
            --packet-exe "$<TARGET_FILE:TRPNrImagePacketTests>" --host-exe "$<TARGET_FILE:TRPNrBeforeHostFramesTests>"
            --prepared-fsr-exe "$<TARGET_FILE:TRPNrPreparedFsrTests>" --fsr-runtime "${TRP_NR_PERF_FSR_RUNTIME}"
            --runtime "${TRP_NR_RESHADE_RUNTIME}" --runtime-root "${TRP_NR_TEST_RUNTIME_ROOT}"
            --driver-core "${TRP_NR_DRIVER_CORE}" --output "${CMAKE_CURRENT_BINARY_DIR}/reshade-sdr-fsr" --sdr-bytes)
        set_tests_properties(NrReShadeSdrFsr PROPERTIES RUN_SERIAL TRUE TIMEOUT 240 LABELS "GPU;ReShade")
    endif()
    endif()
    if(TRP_NR_RTX40_DLL AND TRP_NR_DRIVER_CORE)
        foreach(stable IN ITEMS off)
            add_test(NAME NrLiveStyle-${stable} COMMAND TRPNrPerformanceProbe
                --profile rtx40 --dll "${TRP_NR_RTX40_DLL}" --core "${TRP_NR_DRIVER_CORE}"
                --width 2560 --height 1440 --frames 120 --warmup 3 --readback off
                --style-cycle on --stable-colors ${stable}
                --output "${CMAKE_CURRENT_BINARY_DIR}/live-style-${stable}.json")
            set_tests_properties(NrLiveStyle-${stable} PROPERTIES TIMEOUT 60 RUN_SERIAL TRUE LABELS GPU)
        endforeach()
        add_test(NAME NrPerformancePrepared COMMAND TRPNrPerformanceProbe
            --profile rtx40 --dll "${TRP_NR_RTX40_DLL}" --core "${TRP_NR_DRIVER_CORE}"
            --width 320 --height 180 --frames 16 --warmup 3 --readback on
            --output "${CMAKE_CURRENT_BINARY_DIR}/performance-prepared.json")
        set_tests_properties(NrPerformancePrepared PROPERTIES RUN_SERIAL TRUE LABELS GPU TIMEOUT 90)
        if(TRP_NR_PERF_WITH_FSR AND TRP_NR_PERF_FSR_RUNTIME)
            add_test(NAME NrLiveStyle-prepared-fsr COMMAND TRPNrPerformanceProbe
                --profile rtx40 --dll "${TRP_NR_RTX40_DLL}" --core "${TRP_NR_DRIVER_CORE}"
                --width 2560 --height 1440 --frames 120 --warmup 3 --readback off
                --style-cycle on --stable-colors off --prepared-fsr on
                --fsr-runtime "${TRP_NR_PERF_FSR_RUNTIME}"
                --output "${CMAKE_CURRENT_BINARY_DIR}/live-style-prepared-fsr.json")
            set_tests_properties(NrLiveStyle-prepared-fsr PROPERTIES RUN_SERIAL TRUE LABELS GPU TIMEOUT 60)
            foreach(state IN ITEMS on off)
                add_test(NAME NrPerformanceFsr-${state} COMMAND TRPNrPerformanceProbe
                    --profile rtx40 --dll "${TRP_NR_RTX40_DLL}" --core "${TRP_NR_DRIVER_CORE}"
                    --width 320 --height 180 --frames 16 --warmup 3 --readback on --enabled ${state}
                    --fsr-runtime "${TRP_NR_PERF_FSR_RUNTIME}" --output "${CMAKE_CURRENT_BINARY_DIR}/performance-fsr-${state}.json")
                set_tests_properties(NrPerformanceFsr-${state} PROPERTIES RUN_SERIAL TRUE LABELS GPU TIMEOUT 90)
            endforeach()
        endif()
    endif()
endif()
add_executable(TRPNrRuntimeOwnerTests ../NrRuntimeOwnerTests.cpp)
target_compile_features(TRPNrRuntimeOwnerTests PRIVATE cxx_std_23)
target_compile_definitions(TRPNrRuntimeOwnerTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
target_include_directories(TRPNrRuntimeOwnerTests PRIVATE "${nrRoot}/src")
target_link_libraries(TRPNrRuntimeOwnerTests PRIVATE TRPNeuralRuntime)
add_test(NAME NrRuntimeOwner-missing-export COMMAND TRPNrRuntimeOwnerTests "${TRP_NR_RTX40_DLL}" "${TRP_NR_DRIVER_CORE}" missing-export)
foreach(mode IN ITEMS normal foreign-device resident-conflict shutdown-failure changed-shim)
    add_test(NAME NrRuntimeOwner-${mode} COMMAND TRPNrRuntimeOwnerTests "${TRP_NR_RTX40_DLL}" "${TRP_NR_DRIVER_CORE}" ${mode})
    set_tests_properties(NrRuntimeOwner-${mode} PROPERTIES SKIP_RETURN_CODE 77 TIMEOUT 30 RUN_SERIAL TRUE LABELS "GPU")
endforeach()
