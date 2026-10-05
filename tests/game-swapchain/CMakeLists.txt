# Exercise the unchanged game-facing wrapper with explicit native/host facades.
# This does not emulate the GPU, Streamline, Skyrim or ImGui mouse dispatch.
configure_file("${PROJECT_SOURCE_DIR}/src/FrameGen/GameSwapChain.cpp" "${CMAKE_CURRENT_BINARY_DIR}/GameSwapChain.cpp" COPYONLY)
configure_file("${PROJECT_SOURCE_DIR}/src/FrameGen/GameSwapChain.h" "${CMAKE_CURRENT_BINARY_DIR}/GameSwapChain.h" COPYONLY)
add_executable(TRPGameSwapChainTests GameSwapChainTests.cpp "${CMAKE_CURRENT_BINARY_DIR}/GameSwapChain.cpp")
target_compile_features(TRPGameSwapChainTests PRIVATE cxx_std_23)
target_compile_definitions(TRPGameSwapChainTests PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
target_include_directories(TRPGameSwapChainTests PRIVATE "${CMAKE_CURRENT_BINARY_DIR}" facades "${PROJECT_SOURCE_DIR}/src/FrameGen")
add_test(NAME GameSwapChainPresentBoundary COMMAND TRPGameSwapChainTests)
add_subdirectory("${PROJECT_SOURCE_DIR}/tests/fsr-host-resize" "${CMAKE_CURRENT_BINARY_DIR}/fsr-host-resize")
