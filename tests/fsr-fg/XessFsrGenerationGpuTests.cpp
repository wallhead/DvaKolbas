// Share the real FG callback/pixel/HUD/lifecycle checks with the qualified
// external-DLSS harness. This variant reconstructs every temporal source using
// XessHostResources and xessD3D12Execute; it compiles no NGX SR implementation.
#define TRP_TEST_XESS_PRODUCER
#include "DlssFsrGenerationGpuTests.cpp"
