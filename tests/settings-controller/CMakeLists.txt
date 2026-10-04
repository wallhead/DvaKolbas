# Copy bytes unchanged so quoted game-owner includes resolve to explicit facades.
# Controller, draft validation, SourceFrameGeneration and Configuration remain production code.
configure_file("${PROJECT_SOURCE_DIR}/src/RendererSettingsController.cpp" "${CMAKE_CURRENT_BINARY_DIR}/RendererSettingsController.cpp" COPYONLY)
add_library(TRPSettingsControllerFixture STATIC "${CMAKE_CURRENT_BINARY_DIR}/RendererSettingsController.cpp" Services.cpp)
target_compile_features(TRPSettingsControllerFixture PUBLIC cxx_std_23)
target_compile_definitions(TRPSettingsControllerFixture PUBLIC NOMINMAX WIN32_LEAN_AND_MEAN TRP_ENABLE_FSR TRP_ENABLE_FSR_FG)
target_include_directories(TRPSettingsControllerFixture PUBLIC facades "${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/include" "${TRP_SIMPLEINI_INCLUDE_DIR}" "${TRP_STREAMLINE_INCLUDE_DIR}")
add_executable(TRPSettingsControllerTests ControllerTests.cpp)
target_link_libraries(TRPSettingsControllerTests PRIVATE TRPSettingsControllerFixture)
add_test(NAME RendererSettingsController COMMAND TRPSettingsControllerTests)
