// Public ABI test double; never used in a production package.
#ifdef FIXTURE_LATENCY
#include <xell/xell_d3d12.h>
extern "C" xell_result_t xellDestroyContext(xell_context_handle_t context) { return XELL_RESULT_SUCCESS; }
extern "C" xell_result_t xellSetSleepMode(xell_context_handle_t context, const xell_sleep_params_t* param) { return XELL_RESULT_SUCCESS; }
extern "C" xell_result_t xellGetSleepMode(xell_context_handle_t context, xell_sleep_params_t* param) { return XELL_RESULT_SUCCESS; }
extern "C" xell_result_t xellSleep(xell_context_handle_t context, uint32_t frame_id) { return XELL_RESULT_SUCCESS; }
extern "C" xell_result_t xellAddMarkerData(xell_context_handle_t context, uint32_t frame_id, xell_latency_marker_type_t marker) { return XELL_RESULT_SUCCESS; }
extern "C" xell_result_t xellGetVersion(xell_version_t* pVersion) { *pVersion={1,3,2,0};return XELL_RESULT_SUCCESS; }
extern "C" xell_result_t xellSetLoggingCallback(xell_context_handle_t hContext, xell_logging_level_t loggingLevel, xell_app_log_callback_t loggingCallback) { return XELL_RESULT_SUCCESS; }
extern "C" xell_result_t xellGetFramesReports(xell_context_handle_t context, xell_frame_report_t* outdata) { return XELL_RESULT_SUCCESS; }
extern "C" xell_result_t xellD3D12CreateContext(ID3D12Device* device, xell_context_handle_t* out_context) { return XELL_RESULT_SUCCESS; }
#else
#include <xess_fg/xefg_swapchain_d3d12.h>
extern "C" xefg_swapchain_result_t xefgSwapChainGetVersion(xefg_swapchain_version_t* pVersion) { *pVersion={1,3,1,0};return XEFG_SWAPCHAIN_RESULT_SUCCESS; }
extern "C" xefg_swapchain_result_t xefgSwapChainGetProperties(xefg_swapchain_handle_t hSwapChain, xefg_swapchain_properties_t* pProperties) { return XEFG_SWAPCHAIN_RESULT_SUCCESS; }
extern "C" xefg_swapchain_result_t xefgSwapChainTagFrameConstants(xefg_swapchain_handle_t hSwapChain, uint32_t presentId, const xefg_swapchain_frame_constant_data_t* pConstants) { return XEFG_SWAPCHAIN_RESULT_SUCCESS; }
extern "C" xefg_swapchain_result_t xefgSwapChainSetEnabled(xefg_swapchain_handle_t hSwapChain, uint32_t enable) { return XEFG_SWAPCHAIN_RESULT_SUCCESS; }
extern "C" xefg_swapchain_result_t xefgSwapChainSetPresentId(xefg_swapchain_handle_t hSwapChain, uint32_t presentId) { return XEFG_SWAPCHAIN_RESULT_SUCCESS; }
extern "C" xefg_swapchain_result_t xefgSwapChainGetLastPresentStatus(xefg_swapchain_handle_t hSwapChain, xefg_swapchain_present_status_t* pPresentStatus) { return XEFG_SWAPCHAIN_RESULT_SUCCESS; }
extern "C" xefg_swapchain_result_t xefgSwapChainSetLoggingCallback(xefg_swapchain_handle_t hSwapChain, xefg_swapchain_logging_level_t loggingLevel, xefg_swapchain_app_log_callback_t loggingCallback, void* userData) { return XEFG_SWAPCHAIN_RESULT_SUCCESS; }
extern "C" xefg_swapchain_result_t xefgSwapChainDestroy(xefg_swapchain_handle_t hSwapChain) { return XEFG_SWAPCHAIN_RESULT_SUCCESS; }
extern "C" xefg_swapchain_result_t xefgSwapChainSetLatencyReduction(xefg_swapchain_handle_t hSwapChain, void* hXeLLContext) { return XEFG_SWAPCHAIN_RESULT_SUCCESS; }
extern "C" xefg_swapchain_result_t xefgSwapChainSetSceneChangeThreshold(xefg_swapchain_handle_t hSwapChain, float threshold) { return XEFG_SWAPCHAIN_RESULT_SUCCESS; }
extern "C" xefg_swapchain_result_t xefgSwapChainGetPipelineBuildStatus(xefg_swapchain_handle_t hSwapChain) { return XEFG_SWAPCHAIN_RESULT_SUCCESS; }
extern "C" xefg_swapchain_result_t xefgSwapChainSetNumInterpolatedFrames( xefg_swapchain_handle_t hSwapChain, uint32_t numInterpolatedFrames) { return XEFG_SWAPCHAIN_RESULT_SUCCESS; }
extern "C" xefg_swapchain_result_t xefgSwapChainSetUiCompositionState( xefg_swapchain_handle_t hSwapChain, xefg_swapchain_ui_composition_state_t state) { return XEFG_SWAPCHAIN_RESULT_SUCCESS; }
extern "C" xefg_swapchain_result_t xefgSwapChainD3D12CreateContext(ID3D12Device* pDevice, xefg_swapchain_handle_t* phSwapChain) { return XEFG_SWAPCHAIN_RESULT_SUCCESS; }
extern "C" xefg_swapchain_result_t xefgSwapChainD3D12BuildPipelines(xefg_swapchain_handle_t hSwapChain, ID3D12PipelineLibrary* pPipelineLibrary, uint8_t blocking, uint32_t initFlags) { return XEFG_SWAPCHAIN_RESULT_SUCCESS; }
extern "C" xefg_swapchain_result_t xefgSwapChainD3D12GetProperties( xefg_swapchain_handle_t context, const xefg_swapchain_d3d12_init_params_t* initParams, uint32_t backBufferWidth, uint32_t backBufferHeight, DXGI_FORMAT backBufferFormat, xefg_swapchain_properties_t* properties) { return XEFG_SWAPCHAIN_RESULT_SUCCESS; }
extern "C" xefg_swapchain_result_t xefgSwapChainD3D12InitFromSwapChain(xefg_swapchain_handle_t hSwapChain, ID3D12CommandQueue* pCmdQueue, const xefg_swapchain_d3d12_init_params_t* pInitParams) { return XEFG_SWAPCHAIN_RESULT_SUCCESS; }
extern "C" xefg_swapchain_result_t xefgSwapChainD3D12InitFromSwapChainDesc(xefg_swapchain_handle_t hSwapChain, HWND hWnd, const DXGI_SWAP_CHAIN_DESC1* pSwapChainDesc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc, ID3D12CommandQueue* pCmdQueue, IDXGIFactory2* pDxgiFactory, const xefg_swapchain_d3d12_init_params_t* pInitParams) { return XEFG_SWAPCHAIN_RESULT_SUCCESS; }
extern "C" xefg_swapchain_result_t xefgSwapChainD3D12GetSwapChainPtr(xefg_swapchain_handle_t hSwapChain, REFIID riid, void** ppSwapChain) { return XEFG_SWAPCHAIN_RESULT_SUCCESS; }
extern "C" xefg_swapchain_result_t xefgSwapChainD3D12TagFrameResource(xefg_swapchain_handle_t hSwapChain, ID3D12CommandList* pCmdList, uint32_t presentId, const xefg_swapchain_d3d12_resource_data_t* pResData) { return XEFG_SWAPCHAIN_RESULT_SUCCESS; }
extern "C" xefg_swapchain_result_t xefgSwapChainD3D12SetDescriptorHeap(xefg_swapchain_handle_t hSwapChain, ID3D12DescriptorHeap* pDescriptorHeap, uint32_t descriptorHeapOffsetInBytes) { return XEFG_SWAPCHAIN_RESULT_SUCCESS; }
extern "C" xefg_swapchain_result_t xefgSwapChainD3D12UpdateExternalHeapOnResize( xefg_swapchain_handle_t hSwapChain, ID3D12Heap* tempBufferHeap, uint64_t tempBufferHeapOffset, ID3D12Heap* tempTextureHeap, uint64_t tempTextureHeapOffset) { return XEFG_SWAPCHAIN_RESULT_SUCCESS; }
extern "C" xefg_swapchain_result_t xefgSwapChainD3D12GetInitializationParameters( xefg_swapchain_handle_t hSwapChain, xefg_swapchain_d3d12_init_params_t* pParams) { return XEFG_SWAPCHAIN_RESULT_SUCCESS; }
#endif
