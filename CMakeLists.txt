cmake_minimum_required(VERSION 3.21)

project(
	RaZkolbaS
	VERSION 1.1.0
	LANGUAGES C CXX
)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# One identity for the SKSE plugin, installed files and public project.
set(TRP_PLUGIN_TARGET RaZkolbaS)
set(TRP_DISPLAY_NAME "RaZkolbaS")

add_compile_definitions(SKYRIM)

# IPO variables initialize target properties at creation, including the plugin.
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF)
add_library("${TRP_PLUGIN_TARGET}" SHARED)
include(cmake/ReleaseMetadata.cmake)

target_compile_features("${TRP_PLUGIN_TARGET}" PRIVATE cxx_std_23)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(BUILD_TESTS OFF)
set(SKSE_SUPPORT_XBYAK ON CACHE BOOL "" FORCE)

function(arp_configure_plugin_target target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive- /Zc:__cplusplus)
        if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
            target_compile_options(${target} PRIVATE /MP)
        endif()
        target_link_options(${target} PRIVATE
            "$<$<CONFIG:DEBUG>:/INCREMENTAL;/OPT:NOREF;/OPT:NOICF>"
            "$<$<CONFIG:RELEASE>:/LTCG;/INCREMENTAL:NO;/OPT:REF;/OPT:ICF;/DEBUG:FULL>")
    endif()
endfunction()

include(cmake/Detours.cmake)
include(cmake/NvidiaAppSettings.cmake)
target_link_libraries(RaZkolbaS PRIVATE TRPNvidiaAppSettings)
add_library(TRPReShadeIntegration STATIC src/ReShadeIntegration.cpp)
target_compile_features(TRPReShadeIntegration PUBLIC cxx_std_23)
target_compile_definitions(TRPReShadeIntegration PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
target_include_directories(TRPReShadeIntegration PUBLIC src include)
target_link_libraries(TRPReShadeIntegration PUBLIC d3d11 d3d12 d3dcompiler dxgi)
target_link_libraries(RaZkolbaS PRIVATE TRPReShadeIntegration)
option(TRP_ENABLE_RAZKOLBAS_TAB "Include the private audio easter egg tab" OFF)
add_library(TRPRazkolbasAudio STATIC src/RazkolbasAudio.cpp)
target_compile_features(TRPRazkolbasAudio PUBLIC cxx_std_20)
target_compile_definitions(TRPRazkolbasAudio PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
target_include_directories(TRPRazkolbasAudio PUBLIC src)
target_link_libraries(TRPRazkolbasAudio PRIVATE windowsapp)
if(TRP_ENABLE_RAZKOLBAS_TAB)
    target_link_libraries(RaZkolbaS PRIVATE TRPRazkolbasAudio)
    target_compile_definitions(RaZkolbaS PRIVATE TRP_ENABLE_RAZKOLBAS_TAB)
endif()
include(cmake/BaselineSources.cmake)
if(NOT TRP_ENABLE_RAZKOLBAS_TAB)
    list(REMOVE_ITEM ARP_BASELINE_SOURCES src/OverlayRazkolbasPanel.cpp)
endif()
target_sources(${TRP_PLUGIN_TARGET} PRIVATE ${ARP_BASELINE_SOURCES})
include(cmake/FSR.cmake)

option(TRP_ENABLE_NEURAL_RENDERING "Include Neural Rendering integration and controls" ON)
option(TRP_ENABLE_OPTIONAL_FEATURES "Include optional Ada/Ampere/Turing MFG compatibility" ON)
if(TRP_ENABLE_NEURAL_RENDERING)
    target_sources(RaZkolbaS PRIVATE ${ARP_NEURAL_SOURCES})
    include(cmake/NeuralShaders.cmake)
    trp_target_neural_shaders(RaZkolbaS)
else()
    target_compile_definitions(RaZkolbaS PRIVATE TRP_NO_NEURAL_RENDERING)
    target_sources(RaZkolbaS PRIVATE src/FrameGen/SourceDLSSGNeuralDisabled.cpp)
endif()
if(TRP_ENABLE_OPTIONAL_FEATURES)
    set(TRP_EDITION "Universal")
    target_sources(RaZkolbaS PRIVATE src/FrameGen/SourceDLSSGMFG.cpp)
    add_library(RaZkolbaSRTX40MFG STATIC
        extern/RTX40MFG/midpoint_fix.cpp extern/RTX40MFG/dlssg_provider_policy.cpp
        extern/MFGAmpere/runtime.cpp)
    target_compile_features(RaZkolbaSRTX40MFG PRIVATE cxx_std_23)
    target_compile_definitions(RaZkolbaSRTX40MFG PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_link_libraries(RaZkolbaSRTX40MFG PUBLIC bcrypt version d3d12 TRPNvidiaAppSettings)
    target_link_libraries(RaZkolbaS PRIVATE RaZkolbaSRTX40MFG)
else()
    target_sources(RaZkolbaS PRIVATE src/FrameGen/SourceDLSSGMFGNative.cpp)
    set(TRP_EDITION "Standard")
endif()

# A source archive nested inside another checkout must not report its parent's revision.
execute_process(COMMAND git rev-parse --show-toplevel WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    OUTPUT_VARIABLE TRP_GIT_ROOT OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
set(TRP_SOURCE_REVISION "source-archive")
if(TRP_GIT_ROOT STREQUAL PROJECT_SOURCE_DIR)
    execute_process(COMMAND git rev-parse --short=12 HEAD WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        OUTPUT_VARIABLE TRP_SOURCE_REVISION OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
    execute_process(COMMAND git diff --quiet HEAD WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        RESULT_VARIABLE TRP_SOURCE_DIRTY ERROR_QUIET)
    if(TRP_SOURCE_DIRTY)
        string(APPEND TRP_SOURCE_REVISION "-modified")
    endif()
endif()

foreach(capability IN ITEMS FSR FSR_FG NEURAL_RENDERING)
    set(TRP_BUILD_${capability} 0)
    if(TRP_ENABLE_${capability})
        set(TRP_BUILD_${capability} 1)
    endif()
endforeach()

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/Plugin.h.in
	${CMAKE_CURRENT_BINARY_DIR}/cmake/Plugin.h
	@ONLY
)

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/Version.rc.in
	${CMAKE_CURRENT_BINARY_DIR}/cmake/Version.rc
	@ONLY
)

target_sources(
	"${TRP_PLUGIN_TARGET}"
	PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}/cmake/Plugin.h
	${CMAKE_CURRENT_BINARY_DIR}/cmake/Version.rc
)

target_precompile_headers("${TRP_PLUGIN_TARGET}" PRIVATE include/PCH.h)

# Keep the adapter's Windows macro environment independent of the renderer PCH.
set_source_files_properties(compatibility/ImGuiCompat/ImGuiIntegration.cpp PROPERTIES
    SKIP_PRECOMPILE_HEADERS ON
    COMPILE_DEFINITIONS "UNICODE;_UNICODE;WIN32_LEAN_AND_MEAN;NOMINMAX")

if(MSVC)
	add_compile_definitions(_UNICODE)
	target_compile_definitions(${TRP_PLUGIN_TARGET} PRIVATE "$<$<CONFIG:DEBUG>:DEBUG>")

	arp_configure_plugin_target("${TRP_PLUGIN_TARGET}")
	target_compile_options("${TRP_PLUGIN_TARGET}" PRIVATE /arch:AVX)

endif()

# Dependency paths can be supplied directly or placed under .dependencies/.
set(
	TRP_COMMONLIBSSE_NG_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/.dependencies/CommonLibSSE-NG"
	CACHE PATH
	"Path to a CommonLibSSE-NG checkout"
)

if(NOT EXISTS "${TRP_COMMONLIBSSE_NG_DIR}/CMakeLists.txt")
	message(
		FATAL_ERROR
		"CommonLibSSE-NG was not found at '${TRP_COMMONLIBSSE_NG_DIR}'. "
		"See README.md or set TRP_COMMONLIBSSE_NG_DIR."
	)
endif()

add_subdirectory("${TRP_COMMONLIBSSE_NG_DIR}" CommonLibSSE EXCLUDE_FROM_ALL)
include(cmake/D3D11EntryObservers.cmake)
get_directory_property(TRP_COMMONLIB_VERSION DIRECTORY "${TRP_COMMONLIBSSE_NG_DIR}" DEFINITION PROJECT_VERSION)
if(NOT TRP_COMMONLIB_VERSION VERSION_EQUAL "8.1.0")
    message(FATAL_ERROR "This runtime port requires CommonLibSSE-NG 8.1.0; see README.md for the pinned revision.")
endif()

if(ENABLE_SKYRIM_VR OR NOT ENABLE_SKYRIM_SE OR NOT ENABLE_SKYRIM_AE)
    message(FATAL_ERROR "Enable both Skyrim SE and AE, with VR disabled, for the universal 1.5.97/1.6.640/1.6.1170/1.7.104 build.")
endif()

find_package(spdlog CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

set(
	TRP_NGX_SDK_DIR
	"${CMAKE_CURRENT_SOURCE_DIR}/.dependencies/Streamline/external/ngx-sdk"
	CACHE PATH
	"Path to the NVIDIA NGX SDK root containing include/nvsdk_ngx.h"
)

if(NOT EXISTS "${TRP_NGX_SDK_DIR}/include/nvsdk_ngx.h")
	message(
		FATAL_ERROR
		"NGX SDK headers were not found under '${TRP_NGX_SDK_DIR}/include'. "
		"Install the NGX SDK or configure with -D TRP_NGX_SDK_DIR=<path>."
	)
endif()

if(TRP_ENABLE_NEURAL_RENDERING)
    include(cmake/NeuralRuntime.cmake)
    trp_nr_enable_source_stage("${TRP_NGX_SDK_DIR}/include")
    target_link_libraries(RaZkolbaS PRIVATE TRPNeuralRuntime)
    target_sources(RaZkolbaS PRIVATE src/FrameGen/NvidiaHostNeural.cpp)
endif()

if(TRP_ENABLE_OPTIONAL_FEATURES)
    target_include_directories(RaZkolbaSRTX40MFG PRIVATE "${TRP_NGX_SDK_DIR}/include")
endif()

set(TRP_DEFAULT_NGX_LIB_DIR "${TRP_NGX_SDK_DIR}/lib/Windows_x86_64")
if(NOT EXISTS "${TRP_DEFAULT_NGX_LIB_DIR}/nvsdk_ngx_d.lib")
	set(TRP_DEFAULT_NGX_LIB_DIR "${TRP_NGX_SDK_DIR}/lib/x64")
endif()

set(
	TRP_NGX_LIB
	"${TRP_DEFAULT_NGX_LIB_DIR}/nvsdk_ngx_d.lib"
	CACHE FILEPATH
	"Path to the NGX dynamic import library, usually nvsdk_ngx_d.lib for /MD builds"
)

if(NOT EXISTS "${TRP_NGX_LIB}")
	message(
		FATAL_ERROR
		"NGX import library was not found at '${TRP_NGX_LIB}'. "
		"Configure with -D TRP_NGX_LIB=<path-to-nvsdk_ngx_d.lib>."
	)
endif()

set(TRP_STREAMLINE_INCLUDE_DIR "${TRP_NGX_SDK_DIR}/../../include" CACHE PATH
	"Path to Streamline 2.11.1 public headers")
if(NOT EXISTS "${TRP_STREAMLINE_INCLUDE_DIR}/sl_core_api.h")
	message(FATAL_ERROR "Set TRP_STREAMLINE_INCLUDE_DIR to the Streamline 2.11.1 include directory")
endif()

target_include_directories(${TRP_PLUGIN_TARGET} PRIVATE "${TRP_STREAMLINE_INCLUDE_DIR}")
find_path(TRP_SIMPLEINI_INCLUDE_DIR SimpleIni.h REQUIRED)

target_include_directories(
	${TRP_PLUGIN_TARGET}
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
	PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}/cmake
	${CMAKE_CURRENT_SOURCE_DIR}/src
	"${TRP_NGX_SDK_DIR}/include"
	"${TRP_SIMPLEINI_INCLUDE_DIR}"
)

target_link_libraries(
	${TRP_PLUGIN_TARGET}
	PUBLIC
	CommonLibSSE::CommonLibSSE
	PRIVATE
	ARPDetours
	imgui::imgui
	d3d11.lib
	d3d12.lib
	d3dcompiler.lib
	dxgi.lib
	dxguid.lib
	version.lib
	bcrypt.lib
	windowscodecs.lib
	"${TRP_NGX_LIB}"
)

find_package(xbyak CONFIG REQUIRED)

# Build the plugin without packaging, deployment or game launch.
add_custom_target(ARPBaseline DEPENDS RaZkolbaS)

option(TRP_BUILD_COMPATIBILITY_TESTS "Build offline runtime and frame contract checks" OFF)
if(TRP_BUILD_COMPATIBILITY_TESTS)
    add_executable(TRPBackendSelectionTests tests/BackendSelectionTests.cpp)
    target_compile_features(TRPBackendSelectionTests PRIVATE cxx_std_23)
    target_include_directories(TRPBackendSelectionTests PRIVATE src "${TRP_SIMPLEINI_INCLUDE_DIR}")
    add_test(NAME BackendSelection COMMAND TRPBackendSelectionTests)
    add_executable(TRPNrFsrRouteDiagnosticsTests tests/NrFsrRouteDiagnosticsTests.cpp)
    target_compile_features(TRPNrFsrRouteDiagnosticsTests PRIVATE cxx_std_23)
    target_include_directories(TRPNrFsrRouteDiagnosticsTests PRIVATE src)
    add_test(NAME NrFsrRouteDiagnostics COMMAND TRPNrFsrRouteDiagnosticsTests)
    add_executable(TRPSourceFrameEvaluatorTests tests/SourceFrameEvaluatorTests.cpp)
    target_compile_features(TRPSourceFrameEvaluatorTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPSourceFrameEvaluatorTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPSourceFrameEvaluatorTests PRIVATE src)
    target_link_libraries(TRPSourceFrameEvaluatorTests PRIVATE d3d11)
    add_test(NAME SourceFrameEvaluation COMMAND TRPSourceFrameEvaluatorTests)
    enable_testing()
    file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/plugin-policy-$<CONFIG>.txt"
        CONTENT "IPO=$<TARGET_PROPERTY:RaZkolbaS,INTERPROCEDURAL_OPTIMIZATION>\nDebugIPO=$<TARGET_PROPERTY:RaZkolbaS,INTERPROCEDURAL_OPTIMIZATION_DEBUG>\nCompile=$<TARGET_PROPERTY:RaZkolbaS,COMPILE_OPTIONS>\nLink=$<TARGET_PROPERTY:RaZkolbaS,LINK_OPTIONS>\n")
    if(MSVC)
        add_test(NAME PluginCompilerPolicy COMMAND ${CMAKE_COMMAND}
            "-DPOLICY_FILE=${CMAKE_CURRENT_BINARY_DIR}/plugin-policy-$<CONFIG>.txt" "-DCONFIG=$<CONFIG>"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/PluginCompilerPolicyTests.cmake")
    endif()
    add_subdirectory(tests/nr-module-hook)
    trp_add_fsr_tests()
    add_executable(TRPOrdinaryMacroTests tests/OrdinaryPresentationMacroTests.cpp)
    target_compile_features(TRPOrdinaryMacroTests PRIVATE cxx_std_23)
    target_include_directories(TRPOrdinaryMacroTests PRIVATE src)
    target_link_libraries(TRPOrdinaryMacroTests PRIVATE TRPReShadeIntegration d3d11 d3d12 dxgi)
    add_test(NAME OrdinaryMacroEnvironment COMMAND TRPOrdinaryMacroTests)
    foreach(test IN ITEMS D3D11D3D12Interop InteropRetirement)
        add_executable(TRP${test}Tests tests/${test}Tests.cpp src/Graphics/D3D11D3D12Interop.cpp)
        target_compile_features(TRP${test}Tests PRIVATE cxx_std_23)
        target_compile_definitions(TRP${test}Tests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
        target_include_directories(TRP${test}Tests PRIVATE src)
        target_link_libraries(TRP${test}Tests PRIVATE d3d11 d3d12 dxgi d3dcompiler)
        add_test(NAME ${test} COMMAND TRP${test}Tests)
        set_tests_properties(${test} PROPERTIES TIMEOUT 30)
    endforeach()
    add_subdirectory(tests/ui-blend)
    add_executable(TRPWeatherAppearanceTests tests/WeatherAppearanceTests.cpp)
    target_compile_features(TRPWeatherAppearanceTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPWeatherAppearanceTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPWeatherAppearanceTests PRIVATE src include "${TRP_SIMPLEINI_INCLUDE_DIR}" "${TRP_STREAMLINE_INCLUDE_DIR}")
    add_test(NAME WeatherAppearance COMMAND TRPWeatherAppearanceTests)
    add_executable(TRPRCASParametersTests tests/RCASParametersTests.cpp)
    target_compile_features(TRPRCASParametersTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPRCASParametersTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPRCASParametersTests PRIVATE src)
    target_link_libraries(TRPRCASParametersTests PRIVATE d3d11 d3dcompiler)
    add_test(NAME RCASParameters COMMAND TRPRCASParametersTests "${CMAKE_CURRENT_SOURCE_DIR}/package/SKSE/Plugins/RaZkolbaS/RCAS.hlsl")
    set_tests_properties(WeatherAppearance RCASParameters PROPERTIES TIMEOUT 30)
    add_executable(TRPPerformanceTuningTests tests/PerformanceTuningTests.cpp
        tests/performance/TimingContextAlias.c src/PerformanceTuning.cpp)
    target_compile_features(TRPPerformanceTuningTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPPerformanceTuningTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPPerformanceTuningTests PRIVATE tests/performance src "${TRP_SIMPLEINI_INCLUDE_DIR}")
    target_link_libraries(TRPPerformanceTuningTests PRIVATE d3d11)
    add_test(NAME PerformanceScopesCPU COMMAND TRPPerformanceTuningTests --cpu)
    add_test(NAME PerformanceScopesWARP COMMAND TRPPerformanceTuningTests --warp)
    add_test(NAME PerformancePublicationWARP COMMAND TRPPerformanceTuningTests --publication)
    set_tests_properties(PerformanceScopesCPU PerformanceScopesWARP PerformancePublicationWARP PROPERTIES TIMEOUT 30)
    add_executable(TRPOverlayHotkeysTests tests/OverlayHotkeysTests.cpp src/OverlayHotkeys.cpp)
    add_executable(TRPRazkolbasAudioTests tests/RazkolbasAudioTests.cpp)
    target_link_libraries(TRPRazkolbasAudioTests PRIVATE TRPRazkolbasAudio)
    add_test(NAME RazkolbasAudio COMMAND TRPRazkolbasAudioTests
        "${CMAKE_CURRENT_SOURCE_DIR}/package/SKSE/Plugins/RaZkolbaS/Audio/razkolbas.mp3")
    set_tests_properties(RazkolbasAudio PROPERTIES TIMEOUT 70)
    target_compile_features(TRPOverlayHotkeysTests PRIVATE cxx_std_20)
    target_include_directories(TRPOverlayHotkeysTests PRIVATE src)
    target_link_libraries(TRPOverlayHotkeysTests PRIVATE user32)
    add_test(NAME OverlayHotkeys COMMAND TRPOverlayHotkeysTests)
    set_tests_properties(OverlayHotkeys PROPERTIES TIMEOUT 15)
    add_executable(TRPOverlayLayoutTests tests/OverlayLayoutTests.cpp src/OverlayLayout.cpp src/OverlayPipeline.cpp)
    target_compile_features(TRPOverlayLayoutTests PRIVATE cxx_std_20)
    target_compile_definitions(TRPOverlayLayoutTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPOverlayLayoutTests PRIVATE src "${TRP_SIMPLEINI_INCLUDE_DIR}")
    target_link_libraries(TRPOverlayLayoutTests PRIVATE imgui::imgui)
    add_test(NAME OverlayLayout COMMAND TRPOverlayLayoutTests)
    set_tests_properties(OverlayLayout PROPERTIES TIMEOUT 15)
    add_executable(TRPOverlayTextInputTests tests/OverlayTextInputTests.cpp)
    target_compile_features(TRPOverlayTextInputTests PRIVATE cxx_std_20)
    target_compile_definitions(TRPOverlayTextInputTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPOverlayTextInputTests PRIVATE src)
    target_link_libraries(TRPOverlayTextInputTests PRIVATE imgui::imgui)
    add_test(NAME OverlayTextInput COMMAND TRPOverlayTextInputTests)
    set_tests_properties(OverlayTextInput PROPERTIES TIMEOUT 15)
    add_executable(TRPOverlayNeuralTabTests tests/OverlayNeuralTabTests.cpp)
    target_compile_features(TRPOverlayNeuralTabTests PRIVATE cxx_std_20)
    target_include_directories(TRPOverlayNeuralTabTests PRIVATE src)
    target_link_libraries(TRPOverlayNeuralTabTests PRIVATE imgui::imgui)
    add_test(NAME OverlayNeuralTab COMMAND TRPOverlayNeuralTabTests)
    set_tests_properties(OverlayNeuralTab PROPERTIES TIMEOUT 15)
    add_executable(TRPOverlayFsrGenerationTabTests tests/OverlayFsrGenerationTabTests.cpp)
    target_compile_features(TRPOverlayFsrGenerationTabTests PRIVATE cxx_std_20)
    target_include_directories(TRPOverlayFsrGenerationTabTests PRIVATE src)
    target_link_libraries(TRPOverlayFsrGenerationTabTests PRIVATE imgui::imgui)
    add_test(NAME OverlayFsrGenerationTab COMMAND TRPOverlayFsrGenerationTabTests)
    set_tests_properties(OverlayFsrGenerationTab PROPERTIES TIMEOUT 15)
    add_executable(TRPStartupPreferencesTests tests/StartupPreferencesTests.cpp)
    target_compile_features(TRPStartupPreferencesTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPStartupPreferencesTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPStartupPreferencesTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${TRP_SIMPLEINI_INCLUDE_DIR}")
    add_test(NAME StartupPreferences COMMAND TRPStartupPreferencesTests "${CMAKE_CURRENT_SOURCE_DIR}/package/SKSE/Plugins/RaZkolbaS.ini")
    add_executable(TRPIniLayoutTests tests/IniLayoutTests.cpp)
    target_compile_features(TRPIniLayoutTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPIniLayoutTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPIniLayoutTests PRIVATE src "${TRP_SIMPLEINI_INCLUDE_DIR}")
    add_test(NAME IniLayout COMMAND TRPIniLayoutTests
        "${CMAKE_CURRENT_SOURCE_DIR}/package/SKSE/Plugins/RaZkolbaS.ini"
        "${CMAKE_CURRENT_SOURCE_DIR}/package/examples/FSR-SR/RaZkolbaS.ini"
        "${CMAKE_CURRENT_SOURCE_DIR}/package/examples/FSR-FG/RaZkolbaS.ini")
    add_test(NAME IniPackage COMMAND pwsh.exe -NoProfile -File "${CMAKE_CURRENT_SOURCE_DIR}/tests/IniPackageTests.ps1"
        -Repository "${CMAKE_CURRENT_SOURCE_DIR}" -Output "${CMAKE_CURRENT_BINARY_DIR}/ini-package-tests")
    add_executable(TRPRendererSettingsActionTests tests/RendererSettingsActionTests.cpp)
    target_compile_features(TRPRendererSettingsActionTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPRendererSettingsActionTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPRendererSettingsActionTests PRIVATE src include "${TRP_SIMPLEINI_INCLUDE_DIR}" "${TRP_STREAMLINE_INCLUDE_DIR}")
    add_test(NAME RendererSettingsActions COMMAND TRPRendererSettingsActionTests)

    add_executable(TRPOverlaySettingsCommitTests tests/OverlaySettingsCommitTests.cpp)
    target_compile_features(TRPOverlaySettingsCommitTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPOverlaySettingsCommitTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPOverlaySettingsCommitTests PRIVATE src include "${TRP_SIMPLEINI_INCLUDE_DIR}" "${TRP_STREAMLINE_INCLUDE_DIR}")
    target_link_libraries(TRPOverlaySettingsCommitTests PRIVATE imgui::imgui)
    add_test(NAME OverlaySettingsCommit COMMAND TRPOverlaySettingsCommitTests)
    add_executable(TRPFsrSettingsTests tests/FSRSettingsTests.cpp)
    target_compile_features(TRPFsrSettingsTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPFsrSettingsTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPFsrSettingsTests PRIVATE src include "${TRP_SIMPLEINI_INCLUDE_DIR}" "${TRP_STREAMLINE_INCLUDE_DIR}")
    add_test(NAME FsrSettings COMMAND TRPFsrSettingsTests "${CMAKE_CURRENT_SOURCE_DIR}/package/SKSE/Plugins/RaZkolbaS.ini" "${CMAKE_CURRENT_SOURCE_DIR}/package/examples/FSR-SR/RaZkolbaS.ini")
    add_executable(TRPRuntimeModuleDiagnosticTests tests/RuntimeModuleDiagnosticTests.cpp)
    target_compile_features(TRPRuntimeModuleDiagnosticTests PRIVATE cxx_std_23)
    target_include_directories(TRPRuntimeModuleDiagnosticTests PRIVATE src)
    add_test(NAME RuntimeModuleDiagnostics COMMAND TRPRuntimeModuleDiagnosticTests)
    add_executable(TRPNvidiaAppSettingsTests tests/NvidiaAppSettingsTests.cpp)
    target_compile_features(TRPNvidiaAppSettingsTests PRIVATE cxx_std_20)
    target_include_directories(TRPNvidiaAppSettingsTests PRIVATE src)
    add_test(NAME NvidiaAppSettings COMMAND TRPNvidiaAppSettingsTests)
    add_executable(TRPNvidiaResolverImportsTests tests/NvidiaResolverImportsTests.cpp)
    target_compile_features(TRPNvidiaResolverImportsTests PRIVATE cxx_std_20)
    target_compile_definitions(TRPNvidiaResolverImportsTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPNvidiaResolverImportsTests PRIVATE src)
    add_test(NAME NvidiaResolverImports COMMAND TRPNvidiaResolverImportsTests)
    add_executable(TRPNvidiaDriverSettingsTests tests/NvidiaDriverSettingsTests.cpp)
    target_compile_features(TRPNvidiaDriverSettingsTests PRIVATE cxx_std_20)
    target_include_directories(TRPNvidiaDriverSettingsTests PRIVATE src)
    add_test(NAME NvidiaDriverSettings COMMAND TRPNvidiaDriverSettingsTests)
    add_executable(TRPRendererUpgradeTests tests/RendererUpgradeTests.cpp)
    target_compile_features(TRPRendererUpgradeTests PRIVATE cxx_std_20)
    target_compile_definitions(TRPRendererUpgradeTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPRendererUpgradeTests PRIVATE src)
    add_test(NAME RendererUpgrade COMMAND TRPRendererUpgradeTests)
    add_executable(TRPNvidiaDriverDiagnosticsProbe tests/NvidiaDriverDiagnosticsProbe.cpp)
    target_compile_features(TRPNvidiaDriverDiagnosticsProbe PRIVATE cxx_std_20)
    target_link_libraries(TRPNvidiaDriverDiagnosticsProbe PRIVATE TRPNvidiaAppSettings)
    add_test(NAME NvidiaDriverDiagnostics COMMAND TRPNvidiaDriverDiagnosticsProbe)
    set_tests_properties(NvidiaDriverDiagnostics PROPERTIES SKIP_RETURN_CODE 77)
    find_program(trpPackagePowerShell NAMES pwsh.exe REQUIRED)
    add_test(NAME NrPortablePackage COMMAND "${trpPackagePowerShell}" -NoProfile -File "${CMAKE_CURRENT_SOURCE_DIR}/tests/NrPortablePackageTests.ps1")
    add_executable(TRPGameFacingTargetsTests tests/GameFacingTargetsTests.cpp)
    target_compile_features(TRPGameFacingTargetsTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPGameFacingTargetsTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPGameFacingTargetsTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    target_link_libraries(TRPGameFacingTargetsTests PRIVATE d3d11)
    add_test(NAME GameFacingTargets COMMAND TRPGameFacingTargetsTests)
    set_tests_properties(GameFacingTargets PROPERTIES TIMEOUT 15)
    add_executable(TRPNativeUICopyRouteTests tests/NativeUICopyRouteTests.cpp)
    target_compile_features(TRPNativeUICopyRouteTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPNativeUICopyRouteTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPNativeUICopyRouteTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    target_link_libraries(TRPNativeUICopyRouteTests PRIVATE d3d11)
    add_test(NAME NativeUICopyRoute COMMAND TRPNativeUICopyRouteTests)
    set_tests_properties(NativeUICopyRoute PROPERTIES TIMEOUT 15)
    add_executable(TRPD3D11ContextSlotsTests tests/D3D11ContextSlotsTests.cpp)
    target_compile_features(TRPD3D11ContextSlotsTests PRIVATE cxx_std_23)
    target_include_directories(TRPD3D11ContextSlotsTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    add_test(NAME D3D11ContextSlots COMMAND TRPD3D11ContextSlotsTests)
    add_subdirectory(tests/ampere-runtime)
    add_subdirectory(tests/turing)
    add_executable(TRPSourceDLSSGSessionTests tests/SourceDLSSGSessionTests.cpp src/FrameGen/SourceDLSSGSession.cpp)
    target_compile_features(TRPSourceDLSSGSessionTests PRIVATE cxx_std_23)
    target_include_directories(TRPSourceDLSSGSessionTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src" "${TRP_STREAMLINE_INCLUDE_DIR}")
    add_test(NAME SourceDLSSGSession COMMAND TRPSourceDLSSGSessionTests)
    add_executable(TRPSourceDLSSGCameraTests tests/SourceDLSSGCameraTests.cpp
        src/FrameGen/SourceDLSSGCameraMath.cpp src/FrameGen/CameraMeasurements.cpp src/FrameGen/SourceDLSSGSession.cpp)
    target_compile_features(TRPSourceDLSSGCameraTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPSourceDLSSGCameraTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPSourceDLSSGCameraTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src" "${TRP_STREAMLINE_INCLUDE_DIR}")
    add_test(NAME SourceDLSSGCamera COMMAND TRPSourceDLSSGCameraTests)
    add_executable(TRPMenuRenderStateTests tests/MenuRenderStateTests.cpp)
    target_compile_features(TRPMenuRenderStateTests PRIVATE cxx_std_23)
    target_include_directories(TRPMenuRenderStateTests PRIVATE src)
    add_test(NAME MenuRenderState COMMAND TRPMenuRenderStateTests)
    add_executable(TRPDLSSQualityTests tests/DLSSQualityTests.cpp)
    target_compile_features(TRPDLSSQualityTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPDLSSQualityTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPDLSSQualityTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${TRP_NGX_SDK_DIR}/include" "${TRP_STREAMLINE_INCLUDE_DIR}")
    add_test(NAME DLSSQuality COMMAND TRPDLSSQualityTests)
    add_executable(TRPReShadeIntegrationTests tests/ReShadeIntegrationTests.cpp src/FrameGen/OrdinaryPresentation.cpp
        src/Graphics/D3D11D3D12Interop.cpp src/Upscaling/FSRColorConversion.cpp)
    target_compile_features(TRPReShadeIntegrationTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPReShadeIntegrationTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPReShadeIntegrationTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/include")
    target_link_libraries(TRPReShadeIntegrationTests PRIVATE TRPReShadeIntegration d3d11 d3d12 d3dcompiler dxgi)
    set(TRP_FSR_RESHADE_ARGUMENTS "")
    if(TRP_ENABLE_FSR_FG)
        target_link_libraries(TRPReShadeIntegrationTests PRIVATE TRPFsrRuntime)
        if(TRP_ENABLE_NEURAL_RENDERING)
            target_link_libraries(TRPReShadeIntegrationTests PRIVATE TRPNeuralRuntime)
            target_compile_definitions(TRPReShadeIntegrationTests PRIVATE TRP_TEST_NR_FSR)
        endif()
        list(APPEND TRP_FSR_RESHADE_ARGUMENTS --fsr-runtime-dir "${TRP_FSR_SDK_DIR}/runtime")
    endif()
    add_library(TRPReShadeUnresolvedDouble SHARED tests/ReShadeUnresolvedDouble.cpp)
    add_dependencies(TRPReShadeIntegrationTests TRPReShadeUnresolvedDouble)
    add_test(NAME ReShadeUnresolvedOwnership COMMAND TRPReShadeIntegrationTests --unresolved-reshade "$<TARGET_FILE:TRPReShadeUnresolvedDouble>")
    add_test(NAME ReShadeAbsent COMMAND TRPReShadeIntegrationTests)
    set(TRP_RESHADE_INTEGRATION_RUNTIME "" CACHE FILEPATH "Supplied ReShade DLL for isolated presenter checks")
    if(TRP_RESHADE_INTEGRATION_RUNTIME)
        find_package(Python3 3.10 REQUIRED COMPONENTS Interpreter)
        add_test(NAME ReShadePresentationRuntime COMMAND "${Python3_EXECUTABLE}"
            "${PROJECT_SOURCE_DIR}/tests/run_reshade_integration.py"
            --exe "$<TARGET_FILE:TRPReShadeIntegrationTests>" --runtime "${TRP_RESHADE_INTEGRATION_RUNTIME}"
            --output "${CMAKE_BINARY_DIR}/validation/reshade" ${TRP_FSR_RESHADE_ARGUMENTS})
        set_tests_properties(ReShadePresentationRuntime PROPERTIES TIMEOUT 300 RUN_SERIAL TRUE)
    endif()
    add_subdirectory(tests/reshade-lifecycle)
    add_executable(TRPSourceDLSSGInteropFenceTests tests/SourceDLSSGInteropFenceTests.cpp
        src/FrameGen/SourceDLSSGInterop.cpp src/Graphics/D3D11D3D12Interop.cpp)
    target_compile_features(TRPSourceDLSSGInteropFenceTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPSourceDLSSGInteropFenceTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPSourceDLSSGInteropFenceTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    target_link_libraries(TRPSourceDLSSGInteropFenceTests PRIVATE d3d11 d3d12 dxgi d3dcompiler)
    add_test(NAME SourceDLSSGInteropFence COMMAND TRPSourceDLSSGInteropFenceTests)
    set_tests_properties(SourceDLSSGInteropFence PROPERTIES TIMEOUT 30)
    add_executable(TRPFinalFrameCaptureTests tests/FinalFrameCaptureTests.cpp src/ScreenshotFile.cpp)
    target_compile_features(TRPFinalFrameCaptureTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPFinalFrameCaptureTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPFinalFrameCaptureTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    target_link_libraries(TRPFinalFrameCaptureTests PRIVATE d3d12 dxgi windowscodecs ole32)
    add_test(NAME FinalFrameCapture COMMAND TRPFinalFrameCaptureTests)
    set_tests_properties(FinalFrameCapture PROPERTIES TIMEOUT 60)
    foreach(module IN ITEMS 1 2)
        add_library(TRPModuleIdentityFixture${module} SHARED tests/ModuleIdentityFixture.cpp)
        target_compile_definitions(TRPModuleIdentityFixture${module} PRIVATE TRP_TEST_MODULE_ID=${module})
        set_target_properties(TRPModuleIdentityFixture${module} PROPERTIES
            OUTPUT_NAME "TRPModuleIdentityFixture"
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/module-fixtures/${module}"
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/module-fixtures/${module}")
    endforeach()
    add_executable(TRPPluginPathsTests tests/PluginPathsTests.cpp)
    target_compile_features(TRPPluginPathsTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPPluginPathsTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPPluginPathsTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    target_link_libraries(TRPPluginPathsTests PRIVATE ARPDetours)
    add_dependencies(TRPPluginPathsTests TRPModuleIdentityFixture1 TRPModuleIdentityFixture2)
    add_subdirectory(tests/ampere-loader)
    foreach(order IN ITEMS a-first b-first)
        add_test(NAME PluginPaths-${order} COMMAND TRPPluginPathsTests
            $<TARGET_FILE:TRPModuleIdentityFixture1> $<TARGET_FILE:TRPModuleIdentityFixture2> ${order} direct)
        add_test(NAME PluginPaths-virtual-${order} COMMAND TRPPluginPathsTests
            $<TARGET_FILE:TRPModuleIdentityFixture1> $<TARGET_FILE:TRPModuleIdentityFixture2> ${order} virtual)
    endforeach()
    add_executable(TRPD3D11FrameCopyTests tests/D3D11FrameCopyTests.cpp)
    target_compile_features(TRPD3D11FrameCopyTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPD3D11FrameCopyTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPD3D11FrameCopyTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    target_link_libraries(TRPD3D11FrameCopyTests PRIVATE d3d11 d3dcompiler dxgi)
    add_test(NAME D3D11FrameCopy COMMAND TRPD3D11FrameCopyTests)
    add_executable(TRPNeuralRatioColorTests tests/NeuralRatioColorTests.cpp)
    target_compile_features(TRPNeuralRatioColorTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPNeuralRatioColorTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPNeuralRatioColorTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    target_link_libraries(TRPNeuralRatioColorTests PRIVATE d3d11 d3dcompiler)
    add_test(NAME NeuralRatioColor COMMAND TRPNeuralRatioColorTests)
    add_subdirectory(tests/nr-peripheral)
    add_executable(TRPCommunityShaderFrameTests tests/CommunityShaderFrameTests.cpp)
    target_compile_features(TRPCommunityShaderFrameTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPCommunityShaderFrameTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPCommunityShaderFrameTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    target_link_libraries(TRPCommunityShaderFrameTests PRIVATE d3d11 d3dcompiler)
    add_test(NAME CommunityShaderFrame COMMAND TRPCommunityShaderFrameTests)
    add_executable(TRPLoadingFadeTests tests/LoadingFadeTests.cpp src/FrameGen/PresentationFade.cpp)
    target_compile_features(TRPLoadingFadeTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPLoadingFadeTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPLoadingFadeTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    target_link_libraries(TRPLoadingFadeTests PRIVATE d3d11 d3dcompiler dxgi)
    add_test(NAME LoadingFade COMMAND TRPLoadingFadeTests)
    add_executable(TRPCommunityShaderHDRTests tests/CommunityShaderHDRTests.cpp)
    target_compile_features(TRPCommunityShaderHDRTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPCommunityShaderHDRTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPCommunityShaderHDRTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    target_link_libraries(TRPCommunityShaderHDRTests PRIVATE TRPD3D11EntryObservers d3d11 d3dcompiler)
    add_test(NAME CommunityShaderHDR COMMAND TRPCommunityShaderHDRTests)
    add_executable(TRPHDROutputTests tests/HDROutputTests.cpp)
    target_compile_features(TRPHDROutputTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPHDROutputTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPHDROutputTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src" "${TRP_SIMPLEINI_INCLUDE_DIR}")
    add_test(NAME HDROutput COMMAND TRPHDROutputTests)
    add_executable(TRPHDROutputPassTests tests/HDROutputPassTests.cpp src/FrameGen/SourceDLSSGHDROutput.cpp)
    target_compile_features(TRPHDROutputPassTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPHDROutputPassTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPHDROutputPassTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    target_link_libraries(TRPHDROutputPassTests PRIVATE d3d12 dxgi d3dcompiler)
    add_test(NAME HDROutputPass COMMAND TRPHDROutputPassTests)
    set_tests_properties(HDROutputPass PROPERTIES TIMEOUT 60)
    add_executable(TRPD3D11EntryObserverTests tests/D3D11EntryObserverTests.cpp)
    target_compile_features(TRPD3D11EntryObserverTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPD3D11EntryObserverTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPD3D11EntryObserverTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    target_link_libraries(TRPD3D11EntryObserverTests PRIVATE TRPD3D11EntryObservers d3d11 d3dcompiler)
    add_test(NAME D3D11EntryObserver COMMAND TRPD3D11EntryObserverTests)
    add_executable(TRPNeuralWorldContractTests tests/NeuralWorldContractTests.cpp)
    add_subdirectory(tests/nr-postfg)
    add_subdirectory(tests/settings-controller)
    add_subdirectory(tests/game-swapchain)
    add_subdirectory(tests/nr-runtime)
    add_subdirectory(tests/nr-postsr)
    if(TRP_NR_DRIVER_CORE AND TRP_NR_STREAMLINE_ROOT)
        add_executable(TRPNvidiaAppSettingsGpuTests tests/NvidiaAppSettingsGpuTests.cpp)
        target_compile_features(TRPNvidiaAppSettingsGpuTests PRIVATE cxx_std_20)
        target_compile_definitions(TRPNvidiaAppSettingsGpuTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
        target_include_directories(TRPNvidiaAppSettingsGpuTests PRIVATE "${TRP_NGX_SDK_DIR}/include" "${TRP_STREAMLINE_INCLUDE_DIR}")
        target_link_libraries(TRPNvidiaAppSettingsGpuTests PRIVATE TRPNvidiaAppSettings d3d12 dxgi)
        foreach(mode IN ITEMS control filtered streamline compatibility)
            add_test(NAME NvidiaAppSettingsGpu-${mode} COMMAND TRPNvidiaAppSettingsGpuTests "${TRP_NR_DRIVER_CORE}" "${TRP_NR_STREAMLINE_ROOT}" ${mode})
            set_tests_properties(NvidiaAppSettingsGpu-${mode} PROPERTIES TIMEOUT 60 RUN_SERIAL TRUE LABELS GPU SKIP_RETURN_CODE 77)
        endforeach()
    endif()
    if(TRP_ENABLE_NEURAL_RENDERING AND TRP_ENABLE_FSR_FG AND TRP_NR_RESHADE_RUNTIME AND TRP_NR_TEST_RUNTIME_ROOT)
        find_package(Python3 3.10 REQUIRED COMPONENTS Interpreter)
        add_test(NAME NrFsrReShadeBefore COMMAND "${Python3_EXECUTABLE}"
            "${PROJECT_SOURCE_DIR}/tests/run_reshade_integration.py"
            --exe "$<TARGET_FILE:TRPReShadeIntegrationTests>" --runtime "${TRP_NR_RESHADE_RUNTIME}"
            --fsr-runtime-dir "${TRP_FSR_SDK_DIR}/runtime" --nr-runtime-root "${TRP_NR_TEST_RUNTIME_ROOT}"
            --nr-driver-core "${TRP_NR_DRIVER_CORE}" --output "${CMAKE_BINARY_DIR}/validation/nr-fsr-reshade")
        set_tests_properties(NrFsrReShadeBefore PROPERTIES TIMEOUT 300 RUN_SERIAL TRUE LABELS "GPU;ReShade")
    endif()
    target_compile_features(TRPNeuralWorldContractTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPNeuralWorldContractTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPNeuralWorldContractTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/include" "${TRP_STREAMLINE_INCLUDE_DIR}")
    add_test(NAME NeuralWorldContract COMMAND TRPNeuralWorldContractTests)
    add_executable(TRPSourceCameraSelectionTests tests/SourceCameraSelectionTests.cpp)
    target_compile_features(TRPSourceCameraSelectionTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPSourceCameraSelectionTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPSourceCameraSelectionTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    add_test(NAME SourceCameraSelection COMMAND TRPSourceCameraSelectionTests)
    add_executable(TRPSourceNvidiaFrameEvaluatorTests tests/SourceNvidiaFrameEvaluatorTests.cpp)
    target_compile_features(TRPSourceNvidiaFrameEvaluatorTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPSourceNvidiaFrameEvaluatorTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPSourceNvidiaFrameEvaluatorTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    target_link_libraries(TRPSourceNvidiaFrameEvaluatorTests PRIVATE d3d11)
    add_test(NAME SourceNvidiaFrameEvaluation COMMAND TRPSourceNvidiaFrameEvaluatorTests)
    add_executable(TRPRuntimeProfileTests tests/RuntimeProfileTests.cpp)
    add_executable(TRPHookSafetyTests tests/HookSafetyTests.cpp)
    target_compile_features(TRPHookSafetyTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPHookSafetyTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPHookSafetyTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    target_link_libraries(TRPHookSafetyTests PRIVATE user32)
    add_test(NAME HookSafety COMMAND TRPHookSafetyTests)
    add_executable(TRPHookChainTests tests/HookChainTests.cpp)
    target_compile_features(TRPHookChainTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPHookChainTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPHookChainTests PRIVATE src)
    target_link_libraries(TRPHookChainTests PRIVATE ARPDetours CommonLibSSE::CommonLibSSE)
    add_test(NAME HookChains COMMAND TRPHookChainTests)
    set_tests_properties(HookChains PROPERTIES TIMEOUT 15)
    target_compile_features(TRPRuntimeProfileTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPRuntimeProfileTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPRuntimeProfileTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    target_link_libraries(TRPRuntimeProfileTests PRIVATE CommonLibSSE::CommonLibSSE)
    add_test(NAME RuntimeProfiles COMMAND TRPRuntimeProfileTests)
    add_executable(TRPRuntimeLayoutTests tests/RuntimeLayoutTests.cpp)
    target_compile_features(TRPRuntimeLayoutTests PRIVATE cxx_std_23)
    target_compile_definitions(TRPRuntimeLayoutTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN ENABLE_COMMONLIBSSE_TESTING=1)
    target_include_directories(TRPRuntimeLayoutTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    target_link_libraries(TRPRuntimeLayoutTests PRIVATE CommonLibSSE::CommonLibSSE)
    foreach(runtime IN ITEMS 1.5.97 1.6.640 1.6.1170 1.7.104)
        add_test(NAME RuntimeLayout-${runtime} COMMAND TRPRuntimeLayoutTests ${runtime})
    endforeach()
endif()
