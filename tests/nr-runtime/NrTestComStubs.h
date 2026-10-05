#pragma once
#include <d3d12.h>
#include <atomic>
// CPU-only doubles: unsupported calls fail, no graphics device is opened.
template<class Interface> class NrTestCom : public Interface {
public:
    std::atomic<ULONG> references{1};
    virtual ~NrTestCom()=default;
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID,void** out) override {if(!out)return E_POINTER;*out=static_cast<Interface*>(this);AddRef();return S_OK;}
    ULONG STDMETHODCALLTYPE AddRef() override {return ++references;}
    ULONG STDMETHODCALLTYPE Release() override {const auto n=--references;if(!n)delete this;return n;}
    HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID,UINT*,void*) override {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID,UINT,const void*) override {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(REFGUID,const IUnknown*) override {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE SetName(LPCWSTR) override {return S_OK;}
};
template<class Interface> class NrTestChild : public NrTestCom<Interface> {
public:
    ID3D12Device* device{};
    HRESULT STDMETHODCALLTYPE GetDevice(REFIID id,void** out) override {return device?device->QueryInterface(id,out):E_FAIL;}
};
class NrTestDeviceStub : public NrTestCom<ID3D12Device> {
public:
    UINT STDMETHODCALLTYPE GetNodeCount( void) override {return {};}
    HRESULT STDMETHODCALLTYPE CreateCommandQueue( const D3D12_COMMAND_QUEUE_DESC *pDesc, REFIID riid, void **ppCommandQueue) override {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE type, REFIID riid, void **ppCommandAllocator) override {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE CreateGraphicsPipelineState( const D3D12_GRAPHICS_PIPELINE_STATE_DESC *pDesc, REFIID riid, void **ppPipelineState) override {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE CreateComputePipelineState( const D3D12_COMPUTE_PIPELINE_STATE_DESC *pDesc, REFIID riid, void **ppPipelineState) override {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE CreateCommandList( UINT nodeMask, D3D12_COMMAND_LIST_TYPE type, ID3D12CommandAllocator *pCommandAllocator, ID3D12PipelineState *pInitialState, REFIID riid, void **ppCommandList) override {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE CheckFeatureSupport( D3D12_FEATURE Feature, void *pFeatureSupportData, UINT FeatureSupportDataSize) override {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE CreateDescriptorHeap( const D3D12_DESCRIPTOR_HEAP_DESC *pDescriptorHeapDesc, REFIID riid, void **ppvHeap) override {return E_NOTIMPL;}
    UINT STDMETHODCALLTYPE GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeapType) override {return {};}
    HRESULT STDMETHODCALLTYPE CreateRootSignature( UINT nodeMask, const void *pBlobWithRootSignature, SIZE_T blobLengthInBytes, REFIID riid, void **ppvRootSignature) override {return E_NOTIMPL;}
    void STDMETHODCALLTYPE CreateConstantBufferView( const D3D12_CONSTANT_BUFFER_VIEW_DESC *pDesc, D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor) override {}
    void STDMETHODCALLTYPE CreateShaderResourceView( ID3D12Resource *pResource, const D3D12_SHADER_RESOURCE_VIEW_DESC *pDesc, D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor) override {}
    void STDMETHODCALLTYPE CreateUnorderedAccessView( ID3D12Resource *pResource, ID3D12Resource *pCounterResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC *pDesc, D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor) override {}
    void STDMETHODCALLTYPE CreateRenderTargetView( ID3D12Resource *pResource, const D3D12_RENDER_TARGET_VIEW_DESC *pDesc, D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor) override {}
    void STDMETHODCALLTYPE CreateDepthStencilView( ID3D12Resource *pResource, const D3D12_DEPTH_STENCIL_VIEW_DESC *pDesc, D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor) override {}
    void STDMETHODCALLTYPE CreateSampler( const D3D12_SAMPLER_DESC *pDesc, D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor) override {}
    void STDMETHODCALLTYPE CopyDescriptors( UINT NumDestDescriptorRanges, const D3D12_CPU_DESCRIPTOR_HANDLE *pDestDescriptorRangeStarts, const UINT *pDestDescriptorRangeSizes, UINT NumSrcDescriptorRanges, const D3D12_CPU_DESCRIPTOR_HANDLE *pSrcDescriptorRangeStarts, const UINT *pSrcDescriptorRangeSizes, D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeapsType) override {}
    void STDMETHODCALLTYPE CopyDescriptorsSimple( UINT NumDescriptors, D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptorRangeStart, D3D12_CPU_DESCRIPTOR_HANDLE SrcDescriptorRangeStart, D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeapsType) override {}
    D3D12_RESOURCE_ALLOCATION_INFO STDMETHODCALLTYPE GetResourceAllocationInfo( UINT visibleMask, UINT numResourceDescs, const D3D12_RESOURCE_DESC *pResourceDescs) override {return {};}
    D3D12_HEAP_PROPERTIES STDMETHODCALLTYPE GetCustomHeapProperties( UINT nodeMask, D3D12_HEAP_TYPE heapType) override {return {};}
    HRESULT STDMETHODCALLTYPE CreateCommittedResource( const D3D12_HEAP_PROPERTIES *pHeapProperties, D3D12_HEAP_FLAGS HeapFlags, const D3D12_RESOURCE_DESC *pDesc, D3D12_RESOURCE_STATES InitialResourceState, const D3D12_CLEAR_VALUE *pOptimizedClearValue, REFIID riidResource, void **ppvResource) override {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE CreateHeap( const D3D12_HEAP_DESC *pDesc, REFIID riid, void **ppvHeap) override {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE CreatePlacedResource( ID3D12Heap *pHeap, UINT64 HeapOffset, const D3D12_RESOURCE_DESC *pDesc, D3D12_RESOURCE_STATES InitialState, const D3D12_CLEAR_VALUE *pOptimizedClearValue, REFIID riid, void **ppvResource) override {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE CreateReservedResource( const D3D12_RESOURCE_DESC *pDesc, D3D12_RESOURCE_STATES InitialState, const D3D12_CLEAR_VALUE *pOptimizedClearValue, REFIID riid, void **ppvResource) override {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE CreateSharedHandle( ID3D12DeviceChild *pObject, const SECURITY_ATTRIBUTES *pAttributes, DWORD Access, LPCWSTR Name, HANDLE *pHandle) override {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE OpenSharedHandle( HANDLE NTHandle, REFIID riid, void **ppvObj) override {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE OpenSharedHandleByName( LPCWSTR Name, DWORD Access, HANDLE *pNTHandle) override {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE MakeResident( UINT NumObjects, ID3D12Pageable *const *ppObjects) override {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE Evict( UINT NumObjects, ID3D12Pageable *const *ppObjects) override {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE CreateFence( UINT64 InitialValue, D3D12_FENCE_FLAGS Flags, REFIID riid, void **ppFence) override {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE GetDeviceRemovedReason( void) override {return E_NOTIMPL;}
    void STDMETHODCALLTYPE GetCopyableFootprints( const D3D12_RESOURCE_DESC *pResourceDesc, UINT FirstSubresource, UINT NumSubresources, UINT64 BaseOffset, D3D12_PLACED_SUBRESOURCE_FOOTPRINT *pLayouts, UINT *pNumRows, UINT64 *pRowSizeInBytes, UINT64 *pTotalBytes) override {}
    HRESULT STDMETHODCALLTYPE CreateQueryHeap( const D3D12_QUERY_HEAP_DESC *pDesc, REFIID riid, void **ppvHeap) override {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE SetStablePowerState( BOOL Enable) override {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE CreateCommandSignature( const D3D12_COMMAND_SIGNATURE_DESC *pDesc, ID3D12RootSignature *pRootSignature, REFIID riid, void **ppvCommandSignature) override {return E_NOTIMPL;}
    void STDMETHODCALLTYPE GetResourceTiling( ID3D12Resource *pTiledResource, UINT *pNumTilesForEntireResource, D3D12_PACKED_MIP_INFO *pPackedMipDesc, D3D12_TILE_SHAPE *pStandardTileShapeForNonPackedMips, UINT *pNumSubresourceTilings, UINT FirstSubresourceTilingToGet, D3D12_SUBRESOURCE_TILING *pSubresourceTilingsForNonPackedMips) override {}
    LUID STDMETHODCALLTYPE GetAdapterLuid( void) override {return {};}
};
class NrTestFenceStub : public NrTestChild<ID3D12Fence> {
public:
    UINT64 STDMETHODCALLTYPE GetCompletedValue( void) override {return {};}
    HRESULT STDMETHODCALLTYPE SetEventOnCompletion( UINT64 Value, HANDLE hEvent) override {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE Signal( UINT64 Value) override {return E_NOTIMPL;}
};
class NrTestCommandQueueStub : public NrTestChild<ID3D12CommandQueue> {
public:
    void STDMETHODCALLTYPE UpdateTileMappings( ID3D12Resource *pResource, UINT NumResourceRegions, const D3D12_TILED_RESOURCE_COORDINATE *pResourceRegionStartCoordinates, const D3D12_TILE_REGION_SIZE *pResourceRegionSizes, ID3D12Heap *pHeap, UINT NumRanges, const D3D12_TILE_RANGE_FLAGS *pRangeFlags, const UINT *pHeapRangeStartOffsets, const UINT *pRangeTileCounts, D3D12_TILE_MAPPING_FLAGS Flags) override {}
    void STDMETHODCALLTYPE CopyTileMappings( ID3D12Resource *pDstResource, const D3D12_TILED_RESOURCE_COORDINATE *pDstRegionStartCoordinate, ID3D12Resource *pSrcResource, const D3D12_TILED_RESOURCE_COORDINATE *pSrcRegionStartCoordinate, const D3D12_TILE_REGION_SIZE *pRegionSize, D3D12_TILE_MAPPING_FLAGS Flags) override {}
    void STDMETHODCALLTYPE ExecuteCommandLists( UINT NumCommandLists, ID3D12CommandList *const *ppCommandLists) override {}
    void STDMETHODCALLTYPE SetMarker( UINT Metadata, const void *pData, UINT Size) override {}
    void STDMETHODCALLTYPE BeginEvent( UINT Metadata, const void *pData, UINT Size) override {}
    void STDMETHODCALLTYPE EndEvent( void) override {}
    HRESULT STDMETHODCALLTYPE Signal( ID3D12Fence *pFence, UINT64 Value) override {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE Wait( ID3D12Fence *pFence, UINT64 Value) override {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE GetTimestampFrequency( UINT64 *pFrequency) override {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE GetClockCalibration( UINT64 *pGpuTimestamp, UINT64 *pCpuTimestamp) override {return E_NOTIMPL;}
    D3D12_COMMAND_QUEUE_DESC STDMETHODCALLTYPE GetDesc( void) override {return {};}
};
class NrTestRootSignatureStub : public NrTestChild<ID3D12RootSignature> {
public:
};
class NrTestPipelineStateStub : public NrTestChild<ID3D12PipelineState> {
public:
    HRESULT STDMETHODCALLTYPE GetCachedBlob( ID3DBlob **ppBlob) override {return E_NOTIMPL;}
};
class NrTestCommandAllocatorStub : public NrTestChild<ID3D12CommandAllocator> {
public:
    HRESULT STDMETHODCALLTYPE Reset( void) override {return E_NOTIMPL;}
};
class NrTestGraphicsCommandListStub : public NrTestChild<ID3D12GraphicsCommandList> {
public:
    D3D12_COMMAND_LIST_TYPE STDMETHODCALLTYPE GetType() override {return D3D12_COMMAND_LIST_TYPE_DIRECT;}
    HRESULT STDMETHODCALLTYPE Close( void) override {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE Reset( ID3D12CommandAllocator *pAllocator, ID3D12PipelineState *pInitialState) override {return E_NOTIMPL;}
    void STDMETHODCALLTYPE ClearState( ID3D12PipelineState *pPipelineState) override {}
    void STDMETHODCALLTYPE DrawInstanced( UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation) override {}
    void STDMETHODCALLTYPE DrawIndexedInstanced( UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation) override {}
    void STDMETHODCALLTYPE Dispatch( UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ) override {}
    void STDMETHODCALLTYPE CopyBufferRegion( ID3D12Resource *pDstBuffer, UINT64 DstOffset, ID3D12Resource *pSrcBuffer, UINT64 SrcOffset, UINT64 NumBytes) override {}
    void STDMETHODCALLTYPE CopyTextureRegion( const D3D12_TEXTURE_COPY_LOCATION *pDst, UINT DstX, UINT DstY, UINT DstZ, const D3D12_TEXTURE_COPY_LOCATION *pSrc, const D3D12_BOX *pSrcBox) override {}
    void STDMETHODCALLTYPE CopyResource( ID3D12Resource *pDstResource, ID3D12Resource *pSrcResource) override {}
    void STDMETHODCALLTYPE CopyTiles( ID3D12Resource *pTiledResource, const D3D12_TILED_RESOURCE_COORDINATE *pTileRegionStartCoordinate, const D3D12_TILE_REGION_SIZE *pTileRegionSize, ID3D12Resource *pBuffer, UINT64 BufferStartOffsetInBytes, D3D12_TILE_COPY_FLAGS Flags) override {}
    void STDMETHODCALLTYPE ResolveSubresource( ID3D12Resource *pDstResource, UINT DstSubresource, ID3D12Resource *pSrcResource, UINT SrcSubresource, DXGI_FORMAT Format) override {}
    void STDMETHODCALLTYPE IASetPrimitiveTopology( D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology) override {}
    void STDMETHODCALLTYPE RSSetViewports( UINT NumViewports, const D3D12_VIEWPORT *pViewports) override {}
    void STDMETHODCALLTYPE RSSetScissorRects( UINT NumRects, const D3D12_RECT *pRects) override {}
    void STDMETHODCALLTYPE OMSetBlendFactor( const FLOAT BlendFactor[ 4 ]) override {}
    void STDMETHODCALLTYPE OMSetStencilRef( UINT StencilRef) override {}
    void STDMETHODCALLTYPE SetPipelineState( ID3D12PipelineState *pPipelineState) override {}
    void STDMETHODCALLTYPE ResourceBarrier( UINT NumBarriers, const D3D12_RESOURCE_BARRIER *pBarriers) override {}
    void STDMETHODCALLTYPE ExecuteBundle( ID3D12GraphicsCommandList *pCommandList) override {}
    void STDMETHODCALLTYPE SetDescriptorHeaps( UINT NumDescriptorHeaps, ID3D12DescriptorHeap *const *ppDescriptorHeaps) override {}
    void STDMETHODCALLTYPE SetComputeRootSignature( ID3D12RootSignature *pRootSignature) override {}
    void STDMETHODCALLTYPE SetGraphicsRootSignature( ID3D12RootSignature *pRootSignature) override {}
    void STDMETHODCALLTYPE SetComputeRootDescriptorTable( UINT RootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor) override {}
    void STDMETHODCALLTYPE SetGraphicsRootDescriptorTable( UINT RootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor) override {}
    void STDMETHODCALLTYPE SetComputeRoot32BitConstant( UINT RootParameterIndex, UINT SrcData, UINT DestOffsetIn32BitValues) override {}
    void STDMETHODCALLTYPE SetGraphicsRoot32BitConstant( UINT RootParameterIndex, UINT SrcData, UINT DestOffsetIn32BitValues) override {}
    void STDMETHODCALLTYPE SetComputeRoot32BitConstants( UINT RootParameterIndex, UINT Num32BitValuesToSet, const void *pSrcData, UINT DestOffsetIn32BitValues) override {}
    void STDMETHODCALLTYPE SetGraphicsRoot32BitConstants( UINT RootParameterIndex, UINT Num32BitValuesToSet, const void *pSrcData, UINT DestOffsetIn32BitValues) override {}
    void STDMETHODCALLTYPE SetComputeRootConstantBufferView( UINT RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation) override {}
    void STDMETHODCALLTYPE SetGraphicsRootConstantBufferView( UINT RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation) override {}
    void STDMETHODCALLTYPE SetComputeRootShaderResourceView( UINT RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation) override {}
    void STDMETHODCALLTYPE SetGraphicsRootShaderResourceView( UINT RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation) override {}
    void STDMETHODCALLTYPE SetComputeRootUnorderedAccessView( UINT RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation) override {}
    void STDMETHODCALLTYPE SetGraphicsRootUnorderedAccessView( UINT RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation) override {}
    void STDMETHODCALLTYPE IASetIndexBuffer( const D3D12_INDEX_BUFFER_VIEW *pView) override {}
    void STDMETHODCALLTYPE IASetVertexBuffers( UINT StartSlot, UINT NumViews, const D3D12_VERTEX_BUFFER_VIEW *pViews) override {}
    void STDMETHODCALLTYPE SOSetTargets( UINT StartSlot, UINT NumViews, const D3D12_STREAM_OUTPUT_BUFFER_VIEW *pViews) override {}
    void STDMETHODCALLTYPE OMSetRenderTargets( UINT NumRenderTargetDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE *pRenderTargetDescriptors, BOOL RTsSingleHandleToDescriptorRange, const D3D12_CPU_DESCRIPTOR_HANDLE *pDepthStencilDescriptor) override {}
    void STDMETHODCALLTYPE ClearDepthStencilView( D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView, D3D12_CLEAR_FLAGS ClearFlags, FLOAT Depth, UINT8 Stencil, UINT NumRects, const D3D12_RECT *pRects) override {}
    void STDMETHODCALLTYPE ClearRenderTargetView( D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetView, const FLOAT ColorRGBA[ 4 ], UINT NumRects, const D3D12_RECT *pRects) override {}
    void STDMETHODCALLTYPE ClearUnorderedAccessViewUint( D3D12_GPU_DESCRIPTOR_HANDLE ViewGPUHandleInCurrentHeap, D3D12_CPU_DESCRIPTOR_HANDLE ViewCPUHandle, ID3D12Resource *pResource, const UINT Values[ 4 ], UINT NumRects, const D3D12_RECT *pRects) override {}
    void STDMETHODCALLTYPE ClearUnorderedAccessViewFloat( D3D12_GPU_DESCRIPTOR_HANDLE ViewGPUHandleInCurrentHeap, D3D12_CPU_DESCRIPTOR_HANDLE ViewCPUHandle, ID3D12Resource *pResource, const FLOAT Values[ 4 ], UINT NumRects, const D3D12_RECT *pRects) override {}
    void STDMETHODCALLTYPE DiscardResource( ID3D12Resource *pResource, const D3D12_DISCARD_REGION *pRegion) override {}
    void STDMETHODCALLTYPE BeginQuery( ID3D12QueryHeap *pQueryHeap, D3D12_QUERY_TYPE Type, UINT Index) override {}
    void STDMETHODCALLTYPE EndQuery( ID3D12QueryHeap *pQueryHeap, D3D12_QUERY_TYPE Type, UINT Index) override {}
    void STDMETHODCALLTYPE ResolveQueryData( ID3D12QueryHeap *pQueryHeap, D3D12_QUERY_TYPE Type, UINT StartIndex, UINT NumQueries, ID3D12Resource *pDestinationBuffer, UINT64 AlignedDestinationBufferOffset) override {}
    void STDMETHODCALLTYPE SetPredication( ID3D12Resource *pBuffer, UINT64 AlignedBufferOffset, D3D12_PREDICATION_OP Operation) override {}
    void STDMETHODCALLTYPE SetMarker( UINT Metadata, const void *pData, UINT Size) override {}
    void STDMETHODCALLTYPE BeginEvent( UINT Metadata, const void *pData, UINT Size) override {}
    void STDMETHODCALLTYPE EndEvent( void) override {}
    void STDMETHODCALLTYPE ExecuteIndirect( ID3D12CommandSignature *pCommandSignature, UINT MaxCommandCount, ID3D12Resource *pArgumentBuffer, UINT64 ArgumentBufferOffset, ID3D12Resource *pCountBuffer, UINT64 CountBufferOffset) override {}
};
