#pragma once
#include "Hook/PFNs.h"
#include "Command/CommandRecorder.h"
#include "Command/RecorderManager.h"
#include "Tracking/ObjectTracker.h"
#include "Tracking/DescriptorTracker.h"
#include "Tracking/ResourceStateTracker.h"
#include "Tracking/ResourceAddressTracker.h"
#include "Tracking/MappedBufferManager.h"
#include "Resource/ResourceCopyRequestManager.h"
#include "DXIL/BindlessAccessCache.h"
#include "GUI/GUI.h"
#include "Utilities/Singleton.h"

namespace vista
{
	class Vista : public Singleton<Vista>
	{
		friend class Singleton<Vista>;

	public:
		Bool Initialize();
		void Shutdown();

		void    OnDeviceCreated(ID3D12Device* pDevice);
		HRESULT OnPresent(IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags);
		HRESULT OnCreateCommandQueue(ID3D12Device* pDevice, const D3D12_COMMAND_QUEUE_DESC* pDesc, REFIID riid, void** ppCommandQueue);
		HRESULT OnCreateCommandQueue1(ID3D12Device9* pDevice, const D3D12_COMMAND_QUEUE_DESC* pDesc, REFIID creatorID, REFIID riid, void** ppCommandQueue);
		HRESULT OnCreateCommandList(ID3D12Device* pDevice, UINT nodeMask, D3D12_COMMAND_LIST_TYPE type, ID3D12CommandAllocator* pCommandAllocator, ID3D12PipelineState* pInitialState, REFIID riid, void** ppCommandList);
		HRESULT OnCreateCommandList1(ID3D12Device4* pDevice, UINT nodeMask, D3D12_COMMAND_LIST_TYPE type, D3D12_COMMAND_LIST_FLAGS flags, REFIID riid, void** ppCommandList);
		HRESULT OnCreateCommandAllocator(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE Type, REFIID riid, void** ppCommandAllocator);
		HRESULT OnCreateHeap(ID3D12Device* pDevice, const D3D12_HEAP_DESC* pDesc, REFIID riid, void** ppvHeap);
		HRESULT OnCreateHeap1(ID3D12Device4* pDevice, const D3D12_HEAP_DESC* pDesc, ID3D12ProtectedResourceSession* pProtectedSession, REFIID riid, void** ppvHeap);
		HRESULT OnCreateFence(ID3D12Device* pDevice, UINT64 intialValue, D3D12_FENCE_FLAGS flags, REFIID riid, void** ppFence);
		HRESULT OnCreateGraphicsPipelineState(ID3D12Device* pDevice, const D3D12_GRAPHICS_PIPELINE_STATE_DESC* pDesc, REFIID riid, void** ppPipelineState);
		HRESULT OnCreateComputePipelineState(ID3D12Device* pDevice, const D3D12_COMPUTE_PIPELINE_STATE_DESC* pDesc, REFIID riid, void** ppPipelineState);
		HRESULT OnCreatePipelineState(ID3D12Device2* pDevice, const D3D12_PIPELINE_STATE_STREAM_DESC* pDesc, REFIID riid, void** ppPipelineState);
		HRESULT OnCreateStateObject(ID3D12Device5* pDevice, const D3D12_STATE_OBJECT_DESC* pDesc, REFIID riid, void** ppStateObject);
		HRESULT OnAddToStateObject(ID3D12Device7* pDevice, const D3D12_STATE_OBJECT_DESC* pAddition, ID3D12StateObject* pStateObjectToGrowFrom, REFIID riid, void** ppNewStateObject);
		HRESULT OnCreateRootSignature(ID3D12Device* pDevice, UINT nodeMask, const void* pBlobWithRootSignature, SIZE_T blobLengthInBytes, REFIID riid, void** ppvRootSignature);
		HRESULT OnCreateCommandSignature(ID3D12Device* pDevice, const D3D12_COMMAND_SIGNATURE_DESC* pDesc, ID3D12RootSignature* pRootSignature, REFIID riid, void** ppvCommandSignature);
		HRESULT OnCreateCommittedResource(ID3D12Device* pDevice, const D3D12_HEAP_PROPERTIES* pHeapProperties, D3D12_HEAP_FLAGS heapFlags, const D3D12_RESOURCE_DESC* pDesc, D3D12_RESOURCE_STATES initialResourceState, const D3D12_CLEAR_VALUE* pOptimizedClearValue, REFIID riid, void** ppvResource);
		HRESULT OnCreateCommittedResource1(ID3D12Device4* pDevice, const D3D12_HEAP_PROPERTIES* pHeapProperties, D3D12_HEAP_FLAGS heapFlags, const D3D12_RESOURCE_DESC* pDesc, D3D12_RESOURCE_STATES initialResourceState, const D3D12_CLEAR_VALUE* pOptimizedClearValue, ID3D12ProtectedResourceSession* pProtectedSession, REFIID riid, void** ppvResource);
		HRESULT OnCreatePlacedResource(ID3D12Device* pDevice, ID3D12Heap* pHeap, UINT64 heapOffset, const D3D12_RESOURCE_DESC* pDesc, D3D12_RESOURCE_STATES initialState, const D3D12_CLEAR_VALUE* pOptimizedClearValue, REFIID riid, void** ppvResource);
		HRESULT OnCreatePlacedResource1(ID3D12Device8* pDevice, ID3D12Heap* pHeap, UINT64 heapOffset, const D3D12_RESOURCE_DESC1* pDesc, D3D12_RESOURCE_STATES initialState, const D3D12_CLEAR_VALUE* pOptimizedClearValue, REFIID riid, void** ppvResource);
		HRESULT OnCreateDescriptorHeap(ID3D12Device* pDevice, const D3D12_DESCRIPTOR_HEAP_DESC* pHeapProperties, REFIID riid, void** ppDescriptorHeap);
		void    OnCreateRenderTargetView(ID3D12Device* pDevice, ID3D12Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* pDesc, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor);
		void    OnCreateDepthStencilView(ID3D12Device* pDevice, ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* pDesc, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor);
		void    OnCreateShaderResourceView(ID3D12Device* pDevice, ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* pDesc, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor);
		void    OnCreateUnorderedAccessView(ID3D12Device* pDevice, ID3D12Resource* resource, ID3D12Resource* counterResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* pDesc, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor);
		void    OnCreateConstantBufferView(ID3D12Device* pDevice, const D3D12_CONSTANT_BUFFER_VIEW_DESC* pDesc, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor);
		void    OnCreateSampler(ID3D12Device* pDevice, const D3D12_SAMPLER_DESC* pDesc, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor);
		void    OnCopyDescriptorsSimple(ID3D12Device* pDevice, UINT numDescriptors, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptorRangeStart, D3D12_CPU_DESCRIPTOR_HANDLE srcDescriptorRangeStart, D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapsType);
		void	OnCopyDescriptors(ID3D12Device* pDevice, UINT numDestDescriptorRanges, const D3D12_CPU_DESCRIPTOR_HANDLE* pDestDescriptorRangeStarts, const UINT* pDestDescriptorRangeSizes,
								  UINT numSrcDescriptorRanges, const D3D12_CPU_DESCRIPTOR_HANDLE* pSrcDescriptorRangeStarts, const UINT* pSrcDescriptorRangeSizes, D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapsType);

		void	OnExecuteCommandLists(ID3D12CommandQueue* pCommandQueue, UINT commandListCount, ID3D12CommandList* const* commandLists);
		HRESULT OnSignal(ID3D12CommandQueue* pCommandQueue, ID3D12Fence* pFence, UINT64 Value);

		void    OnCopyBufferRegion(ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* pDstBuffer, UINT64 dstOffset, ID3D12Resource* pSrcBuffer, UINT64 srcOffset, UINT64 numBytes);
		void    OnCopyTextureRegion(ID3D12GraphicsCommandList* pCommandList, const D3D12_TEXTURE_COPY_LOCATION* pDst, UINT dstX, UINT dstY, UINT dstZ, const D3D12_TEXTURE_COPY_LOCATION* pSrc, const D3D12_BOX* pSrcBox);
		void    OnCopyResource(ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* pDstResource, ID3D12Resource* pSrcResource);
		void    OnResolveSubresource(ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* pDstResource, UINT dstSubresource, ID3D12Resource* pSrcResource, UINT srcSubresource, DXGI_FORMAT format);
		void    OnResolveSubresourceRegion(ID3D12GraphicsCommandList1* pCommandList, ID3D12Resource* pDstResource, UINT dstSubresource, UINT dstX, UINT dstY,
										   ID3D12Resource* pSrcResource, UINT SrcSubresource, D3D12_RECT* pSrcRect, DXGI_FORMAT format, D3D12_RESOLVE_MODE resolveMode);
		void    OnDrawInstanced(ID3D12GraphicsCommandList* pCommandList, UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation, UINT startInstanceLocation);
		void	OnDrawIndexedInstanced(ID3D12GraphicsCommandList* pCommandList, UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation, INT baseVertexLocation, UINT startInstanceLocation);
		void    OnDispatch(ID3D12GraphicsCommandList* pCommandList, UINT threadGroupCountX, UINT threadGroupCountY, UINT threadGroupCountZ);
		void    OnDispatchMesh(ID3D12GraphicsCommandList6* pCommandList, UINT threadGroupCountX, UINT threadGroupCountY, UINT threadGroupCountZ);
		void    OnExecuteIndirect(ID3D12GraphicsCommandList* pCommandList, ID3D12CommandSignature* pCommandSignature, UINT maxCommandCount, ID3D12Resource* pArgumentBuffer, UINT64 argumentBufferOffset, ID3D12Resource* pCountBuffer, UINT64 countBufferOffset);
		void    OnDispatchRays(ID3D12GraphicsCommandList4* pCommandList, const D3D12_DISPATCH_RAYS_DESC* pDesc);
		void    OnRSSetViewports(ID3D12GraphicsCommandList* pCommandList, UINT viewportCount, const D3D12_VIEWPORT* pViewports);
		void    OnRSSetScissorRects(ID3D12GraphicsCommandList* pCommandList, UINT numRects, const D3D12_RECT* pRects);
		void    OnRSSetShadingRate(ID3D12GraphicsCommandList5* pCommandList, D3D12_SHADING_RATE baseShadingRate, const D3D12_SHADING_RATE_COMBINER* pCombiners);
		void    OnSetViewInstanceMask(ID3D12GraphicsCommandList1* pCommandList, UINT mask);
		void    OnRSSetShadingRateImage(ID3D12GraphicsCommandList5* pCommandList, ID3D12Resource* shadingRateImage);
		void    OnOMSetRenderTargets(ID3D12GraphicsCommandList* pCommandList, UINT numRenderTargetDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE* pRenderTargetDescriptors, BOOL RTsSingleHandleToDescriptorRange, const D3D12_CPU_DESCRIPTOR_HANDLE* pDepthStencilDescriptor);
		void    OnOMSetBlendFactor(ID3D12GraphicsCommandList* pCommandList, const FLOAT* blendFactor);
		void    OnOMSetStencilRef(ID3D12GraphicsCommandList* pCommandList, UINT stencilRef);
		void    OnOMSetDepthBounds(ID3D12GraphicsCommandList1* pCommandList, FLOAT min, FLOAT max);
		void    OnBeginRenderPass(ID3D12GraphicsCommandList4* pCommandList, UINT numRenderTargets, const D3D12_RENDER_PASS_RENDER_TARGET_DESC* pRenderTargets, const D3D12_RENDER_PASS_DEPTH_STENCIL_DESC* pDepthStencil, D3D12_RENDER_PASS_FLAGS flags);
		void	OnEndRenderPass(ID3D12GraphicsCommandList4* pCommandList);
		void    OnClearRenderTargetView(ID3D12GraphicsCommandList* pCommandList, D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView, const FLOAT* colorRGBA, UINT numRects, const D3D12_RECT* pRects);
		void    OnClearDepthStencilView(ID3D12GraphicsCommandList* pCommandList, D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView, D3D12_CLEAR_FLAGS clearFlags, FLOAT depth, UINT8 stencil, UINT numRects, const D3D12_RECT* pRects);
		void    OnClearUnorderedAccessViewUint(ID3D12GraphicsCommandList* pCommandList, D3D12_GPU_DESCRIPTOR_HANDLE viewGPUHandleInCurrentHeap, D3D12_CPU_DESCRIPTOR_HANDLE viewCPUHandle, ID3D12Resource* pResource, const UINT* values, UINT numRects, const D3D12_RECT* pRects);
		void    OnClearUnorderedAccessViewFloat(ID3D12GraphicsCommandList* pCommandList, D3D12_GPU_DESCRIPTOR_HANDLE viewGPUHandleInCurrentHeap, D3D12_CPU_DESCRIPTOR_HANDLE viewCPUHandle, ID3D12Resource* pResource, const FLOAT* values, UINT numRects, const D3D12_RECT* pRects);
		void    OnSetPipelineState(ID3D12GraphicsCommandList* pCommandList, ID3D12PipelineState* pPipelineState);
		void    OnSetPipelineState1(ID3D12GraphicsCommandList4* pCommandList, ID3D12StateObject* pStateObject);
		void    OnSetGraphicsRootSignature(ID3D12GraphicsCommandList* pCommandList, ID3D12RootSignature* pRootSignature);
		void    OnSetComputeRootSignature(ID3D12GraphicsCommandList* pCommandList, ID3D12RootSignature* pRootSignature);
		void    OnIASetPrimitiveTopology(ID3D12GraphicsCommandList* pCommandList, D3D12_PRIMITIVE_TOPOLOGY topology);
		void    OnIASetVertexBuffers(ID3D12GraphicsCommandList* pCommandList, UINT startSlot, UINT numViews, const D3D12_VERTEX_BUFFER_VIEW* pViews);
		void    OnIASetIndexBuffer(ID3D12GraphicsCommandList* pCommandList, const D3D12_INDEX_BUFFER_VIEW* pView);
		void	OnResourceBarrier(ID3D12GraphicsCommandList* pCommandList, UINT numBarriers, const D3D12_RESOURCE_BARRIER* pBarriers);
		void    OnSetDescriptorHeaps(ID3D12GraphicsCommandList* pCommandList, UINT numDescriptorHeaps, ID3D12DescriptorHeap* const* ppDescriptorHeaps);
		void    OnSetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE baseDescriptor);
		void    OnSetComputeRootDescriptorTable(ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE baseDescriptor);
		void	OnSetGraphicsRoot32BitConstant(ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex, UINT srcData, UINT destOffsetIn32BitValues);
		void    OnSetComputeRoot32BitConstant(ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex, UINT srcData, UINT destOffsetIn32BitValues);
		void	OnSetGraphicsRoot32BitConstants(ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex, UINT num32BitValuesToSet, const void* pSrcData, UINT destOffsetIn32BitValues);
		void	OnSetComputeRoot32BitConstants(ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex, UINT num32BitValuesToSet, const void* pSrcData, UINT destOffsetIn32BitValues);
		void	OnSetGraphicsRootConstantBufferView(ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation);
		void	OnSetComputeRootConstantBufferView(ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation);
		void	OnSetGraphicsRootShaderResourceView(ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation);
		void	OnSetComputeRootShaderResourceView(ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation);
		void	OnSetGraphicsRootUnorderedAccessView(ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation);
		void	OnSetComputeRootUnorderedAccessView(ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation);

		HRESULT OnClose(ID3D12GraphicsCommandList* pCommandList);
		HRESULT OnReset(ID3D12GraphicsCommandList* pCommandList, ID3D12CommandAllocator* pCommandAllocator, ID3D12PipelineState* pPipelineState);
		void    OnClearState(ID3D12GraphicsCommandList* pCommandList, ID3D12PipelineState* pPipelineState);
		void	OnBeginEvent(ID3D12GraphicsCommandList* pCommandList, UINT metadata, const void* pData, UINT size);
		void	OnEndEvent(ID3D12GraphicsCommandList* pCommandList);

		ULONG   OnRelease(ID3D12Resource* pResource);
		HRESULT OnSetName(ID3D12Resource* pResource, LPCWSTR name);
		HRESULT OnMap(ID3D12Resource* pResource, UINT subresource, const D3D12_RANGE* pReadRange, void** ppData);
		void    OnUnmap(ID3D12Resource* pResource, UINT subresource, const D3D12_RANGE* pWrittenRange);

		UINT64  OnGetCompletedValue(ID3D12Fence* pFence);
		HRESULT OnSetEventOnCompletion(ID3D12Fence* pFence, UINT64 value, HANDLE event);
		HRESULT OnSignal(ID3D12Fence* pFence, UINT64 value);

		ObjectTracker const& GetObjectTracker() const { return objectTracker; }
		DescriptorTracker const& GetDescriptorTracker() const { return descriptorTracker; }
		ResourceAddressTracker const& GetAddressTracker() const { return addressTracker; }
		MappedBufferManager const& GetMappedBufferManager() const { return mappedBufferManager; }
		ResourceCopyRequestManager& GetCopyRequestManager() { return copyRequestManager; }
		BindlessAccessCache& GetBindlessAccessCache() { return bindlessAccessCache; }

	private:
		D3D12PFNs d3d12PFNs;
		ObjectTracker objectTracker;
		DescriptorTracker descriptorTracker;
		RecorderManager recorderManager;
		ResourceStateTracker stateTracker;
		ResourceAddressTracker addressTracker;
		MappedBufferManager mappedBufferManager;
		ResourceCopyRequestManager copyRequestManager;
		BindlessAccessCache bindlessAccessCache;
		GUI GUI;

		ID3D12GraphicsCommandList* pRequestCommandList = nullptr;

	private:
		Vista() : d3d12PFNs(), objectTracker(), descriptorTracker(objectTracker), recorderManager(objectTracker), 
				  stateTracker(), addressTracker(), mappedBufferManager(), bindlessAccessCache(), GUI() 
		{}
		~Vista() = default;

		void InitializeD3D12Hooks();
		void CheckForCopyRequest(ID3D12GraphicsCommandList* pCommandList, Command const& cmd);
	};
}
#define g_Vista vista::Vista::Get()
