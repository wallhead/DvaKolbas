#include <xess/xess_d3d12.h>
#define EXPORT extern "C" __declspec(dllexport)
EXPORT xess_result_t xessGetVersion(xess_version_t* version) { *version={2,0,1,0};return XESS_RESULT_SUCCESS; }
EXPORT xess_result_t xessGetIntelXeFXVersion(xess_context_handle_t,xess_version_t*) { return XESS_RESULT_SUCCESS; }
EXPORT xess_result_t xessD3D12CreateContext(ID3D12Device*,xess_context_handle_t*) { return XESS_RESULT_SUCCESS; }
EXPORT xess_result_t xessGetOptimalInputResolution(xess_context_handle_t,const xess_2d_t*,xess_quality_settings_t,xess_2d_t*,xess_2d_t*,xess_2d_t*) { return XESS_RESULT_SUCCESS; }
EXPORT xess_result_t xessGetProperties(xess_context_handle_t,const xess_2d_t*,xess_properties_t*) { return XESS_RESULT_SUCCESS; }
EXPORT xess_result_t xessD3D12BuildPipelines(xess_context_handle_t,ID3D12PipelineLibrary*,bool,uint32_t) { return XESS_RESULT_SUCCESS; }
EXPORT xess_result_t xessD3D12Init(xess_context_handle_t,const xess_d3d12_init_params_t*) { return XESS_RESULT_SUCCESS; }
EXPORT xess_result_t xessD3D12GetInitParams(xess_context_handle_t,xess_d3d12_init_params_t*) { return XESS_RESULT_SUCCESS; }
EXPORT xess_result_t xessD3D12Execute(xess_context_handle_t,ID3D12GraphicsCommandList*,const xess_d3d12_execute_params_t*) { return XESS_RESULT_SUCCESS; }
EXPORT xess_result_t xessSetJitterScale(xess_context_handle_t,float,float) { return XESS_RESULT_SUCCESS; }
EXPORT xess_result_t xessSetVelocityScale(xess_context_handle_t,float,float) { return XESS_RESULT_SUCCESS; }
EXPORT xess_result_t xessSetLoggingCallback(xess_context_handle_t,xess_logging_level_t,xess_app_log_callback_t) { return XESS_RESULT_SUCCESS; }
EXPORT xess_result_t xessIsOptimalDriver(xess_context_handle_t) { return XESS_RESULT_SUCCESS; }
#ifndef XESS_MISSING_EXPORT
EXPORT xess_result_t xessDestroyContext(xess_context_handle_t) { return XESS_RESULT_SUCCESS; }
#endif
