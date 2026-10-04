# Direct NR owner only. No NGX/Streamline import library and no caller shim or
# runtime load merely from linking. The game integration is a later milestone.
get_filename_component(trpNrRoot "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if(NOT TARGET TRPNeuralRuntime)
    add_library(TRPNeuralRuntime STATIC
        "${trpNrRoot}/src/NeuralRendering/PerformanceMetrics.cpp"
        "${trpNrRoot}/src/NeuralRendering/RuntimeCatalog.cpp"
        "${trpNrRoot}/src/NeuralRendering/RuntimeFileLease.cpp"
        "${trpNrRoot}/src/NeuralRendering/CallerIdentityShim.cpp"
        "${trpNrRoot}/src/NeuralRendering/RuntimeOwner.cpp"
        "${trpNrRoot}/src/NeuralRendering/ImagePacket.cpp"
        "${trpNrRoot}/src/NeuralRendering/History.cpp")
    target_compile_features(TRPNeuralRuntime PUBLIC cxx_std_23)
    target_compile_definitions(TRPNeuralRuntime PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_include_directories(TRPNeuralRuntime PUBLIC "${trpNrRoot}/src")
    target_link_libraries(TRPNeuralRuntime PUBLIC d3d12 dxgi bcrypt)
endif()

# Shared native source stage is compiled once for the product and research probes.
function(trp_nr_enable_source_stage ngxInclude)
    get_target_property(nrStageAdded TRPNeuralRuntime TRP_NR_SOURCE_STAGE)
    if(NOT nrStageAdded)
        target_sources(TRPNeuralRuntime PRIVATE
            "${trpNrRoot}/src/NeuralRendering/Stage.cpp"
            "${trpNrRoot}/src/NeuralRendering/PerformanceQueries.cpp"
            "${trpNrRoot}/src/NeuralRendering/BeforeUpscale.cpp"
            "${trpNrRoot}/src/NeuralRendering/StableColorResolve.cpp"
            "${trpNrRoot}/src/NeuralRendering/PreparedBeforeUpscale.cpp"
            "${trpNrRoot}/src/NeuralRendering/BeforeHost.cpp"
            "${trpNrRoot}/src/Graphics/D3D11D3D12Interop.cpp"
            "${trpNrRoot}/src/Upscaling/FSRColorConversion.cpp")
        target_include_directories(TRPNeuralRuntime PRIVATE "${ngxInclude}")
        target_link_libraries(TRPNeuralRuntime PUBLIC d3dcompiler d3d11)
        set_target_properties(TRPNeuralRuntime PROPERTIES TRP_NR_SOURCE_STAGE TRUE)
    endif()
endfunction()
