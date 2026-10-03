# Direct NR owner only. No NGX/Streamline import library and no caller shim or
# runtime load merely from linking. The game integration is a later milestone.
get_filename_component(trpNrRoot "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if(NOT TARGET TRPNeuralRuntime)
    add_library(TRPNeuralRuntime STATIC
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
