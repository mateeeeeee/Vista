#include "Hooks.h"
#include "Core/Vista.h"

namespace vista::hooks
{
	HRESULT STDMETHODCALLTYPE Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
	{
		return g_Vista.OnPresent(pSwapChain, SyncInterval, Flags);
	}

	HRESULT STDMETHODCALLTYPE CreateCommandQueue(ID3D12Device* pDevice, const D3D12_COMMAND_QUEUE_DESC* pDesc, REFIID riid, void** ppCommandQueue)
	{
		return g_Vista.OnCreateCommandQueue(pDevice, pDesc, riid, ppCommandQueue);
	}

	HRESULT STDMETHODCALLTYPE CreateCommandQueue1(ID3D12Device9* pDevice, const D3D12_COMMAND_QUEUE_DESC* pDesc, REFIID CreatorID, REFIID riid, void** ppCommandQueue)
	{
		return g_Vista.OnCreateCommandQueue1(pDevice, pDesc, CreatorID, riid, ppCommandQueue);
	}

	HRESULT STDMETHODCALLTYPE CreateCommandList(ID3D12Device* pDevice, UINT NodeMask, D3D12_COMMAND_LIST_TYPE Type, ID3D12CommandAllocator* pCommandAllocator, ID3D12PipelineState* pInitialState, REFIID riid, void** ppCommandList)
	{
		return g_Vista.OnCreateCommandList(pDevice, NodeMask, Type, pCommandAllocator, pInitialState, riid, ppCommandList);
	}

	HRESULT STDMETHODCALLTYPE CreateCommandList1(ID3D12Device4* pDevice, UINT NodeMask, D3D12_COMMAND_LIST_TYPE Type, D3D12_COMMAND_LIST_FLAGS Flags, REFIID riid, void** ppCommandList)
	{
		return g_Vista.OnCreateCommandList1(pDevice, NodeMask, Type, Flags, riid, ppCommandList);
	}

	HRESULT STDMETHODCALLTYPE CreateCommandAllocator(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE Type, REFIID riid, void** ppCommandAllocator)
	{
		return g_Vista.OnCreateCommandAllocator(pDevice, Type, riid, ppCommandAllocator);
	}

	HRESULT STDMETHODCALLTYPE CreateHeap(ID3D12Device* pDevice, const D3D12_HEAP_DESC* pDesc, REFIID riid, void** ppvHeap)
	{
		return g_Vista.OnCreateHeap(pDevice, pDesc, riid, ppvHeap);
	}

	HRESULT STDMETHODCALLTYPE CreateHeap1(ID3D12Device4* pDevice, const D3D12_HEAP_DESC* pDesc, ID3D12ProtectedResourceSession* pProtectedSession, REFIID riid, void** ppvHeap)
	{
		return g_Vista.OnCreateHeap1(pDevice, pDesc, pProtectedSession, riid, ppvHeap);
	}

	HRESULT STDMETHODCALLTYPE CreateFence(ID3D12Device* pDevice, UINT64 IntialValue, D3D12_FENCE_FLAGS Flags, REFIID riid, void** ppFence)
	{
		return g_Vista.OnCreateFence(pDevice, IntialValue, Flags, riid, ppFence);
	}

	HRESULT STDMETHODCALLTYPE CreateRootSignature(ID3D12Device* pDevice, UINT NodeMask, const void* pBlobWithRootSignature, SIZE_T BlobLengthInBytes, REFIID riid, void** ppvRootSignature)
	{
		return g_Vista.OnCreateRootSignature(pDevice, NodeMask, pBlobWithRootSignature, BlobLengthInBytes, riid, ppvRootSignature);
	}

	HRESULT STDMETHODCALLTYPE CreateCommandSignature(ID3D12Device* pDevice, const D3D12_COMMAND_SIGNATURE_DESC* pDesc, ID3D12RootSignature* pRootSignature, REFIID riid, void** ppvCommandSignature)
	{
		return g_Vista.OnCreateCommandSignature(pDevice, pDesc, pRootSignature, riid, ppvCommandSignature);
	}

	HRESULT STDMETHODCALLTYPE CreateGraphicsPipelineState(ID3D12Device* pDevice, const D3D12_GRAPHICS_PIPELINE_STATE_DESC* pDesc, REFIID riid, void** ppPipelineState)
	{
		return g_Vista.OnCreateGraphicsPipelineState(pDevice, pDesc, riid, ppPipelineState);
	}

	HRESULT STDMETHODCALLTYPE CreateComputePipelineState(ID3D12Device* pDevice, const D3D12_COMPUTE_PIPELINE_STATE_DESC* pDesc, REFIID riid, void** ppPipelineState)
	{
		return g_Vista.OnCreateComputePipelineState(pDevice, pDesc, riid, ppPipelineState);
	}

	HRESULT STDMETHODCALLTYPE CreatePipelineState(ID3D12Device2* pDevice, const D3D12_PIPELINE_STATE_STREAM_DESC* pDesc, REFIID riid, void** ppPipelineState)
	{
		return g_Vista.OnCreatePipelineState(pDevice, pDesc, riid, ppPipelineState);
	}

	HRESULT STDMETHODCALLTYPE CreateStateObject(ID3D12Device5* pDevice, const D3D12_STATE_OBJECT_DESC* pDesc, REFIID riid, void** ppStateObject)
	{
		return g_Vista.OnCreateStateObject(pDevice, pDesc, riid, ppStateObject);
	}

	HRESULT STDMETHODCALLTYPE AddToStateObject(ID3D12Device7* pDevice, const D3D12_STATE_OBJECT_DESC* pAddition, ID3D12StateObject* pStateObjectToGrowFrom, REFIID riid, void** ppNewStateObject)
	{
		return g_Vista.OnAddToStateObject(pDevice, pAddition, pStateObjectToGrowFrom, riid, ppNewStateObject);
	}

	HRESULT STDMETHODCALLTYPE CreateCommittedResource(ID3D12Device* pDevice, const D3D12_HEAP_PROPERTIES* pHeapProperties, D3D12_HEAP_FLAGS HeapFlags, 
		const D3D12_RESOURCE_DESC* pDesc, D3D12_RESOURCE_STATES InitialResourceState, const D3D12_CLEAR_VALUE* pOptimizedClearValue, REFIID riid, void** ppvResource)
	{
		return g_Vista.OnCreateCommittedResource(pDevice, pHeapProperties, HeapFlags, pDesc, InitialResourceState, pOptimizedClearValue, riid, ppvResource);
	}

	HRESULT STDMETHODCALLTYPE CreateCommittedResource1(ID3D12Device4* pDevice, const D3D12_HEAP_PROPERTIES* pHeapProperties, D3D12_HEAP_FLAGS HeapFlags,
		const D3D12_RESOURCE_DESC* pDesc, D3D12_RESOURCE_STATES InitialResourceState, const D3D12_CLEAR_VALUE* pOptimizedClearValue, ID3D12ProtectedResourceSession* pProtectedSession, REFIID riid, void** ppvResource)
	{
		return g_Vista.OnCreateCommittedResource1(pDevice, pHeapProperties, HeapFlags, pDesc, InitialResourceState, pOptimizedClearValue, pProtectedSession, riid, ppvResource);
	}

	HRESULT STDMETHODCALLTYPE CreatePlacedResource(ID3D12Device* pDevice, ID3D12Heap* pHeap, UINT64 HeapOffset, const D3D12_RESOURCE_DESC* pDesc, D3D12_RESOURCE_STATES InitialState, const D3D12_CLEAR_VALUE* pOptimizedClearValue, REFIID riid, void** ppvResource)
	{
		return g_Vista.OnCreatePlacedResource(pDevice, pHeap, HeapOffset, pDesc, InitialState, pOptimizedClearValue, riid, ppvResource);
	}

	HRESULT STDMETHODCALLTYPE CreatePlacedResource1(ID3D12Device8* pDevice, ID3D12Heap* pHeap, UINT64 HeapOffset, const D3D12_RESOURCE_DESC1* pDesc, D3D12_RESOURCE_STATES InitialState, const D3D12_CLEAR_VALUE* pOptimizedClearValue, REFIID riid, void** ppvResource)
	{
		return g_Vista.OnCreatePlacedResource1(pDevice, pHeap, HeapOffset, pDesc, InitialState, pOptimizedClearValue, riid, ppvResource);
	}

	HRESULT STDMETHODCALLTYPE CreateDescriptorHeap(ID3D12Device* pDevice, const D3D12_DESCRIPTOR_HEAP_DESC* pHeapProperties, REFIID riid, void** ppDescriptorHeap)
	{
		return g_Vista.OnCreateDescriptorHeap(pDevice, pHeapProperties, riid, ppDescriptorHeap);
	}

	void STDMETHODCALLTYPE CreateRenderTargetView(ID3D12Device* pDevice, ID3D12Resource* Resource, const D3D12_RENDER_TARGET_VIEW_DESC* pDesc, D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor)
	{
		return g_Vista.OnCreateRenderTargetView(pDevice, Resource, pDesc, DestDescriptor);
	}

	void STDMETHODCALLTYPE CreateDepthStencilView(ID3D12Device* pDevice, ID3D12Resource* Resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* pDesc, D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor)
	{
		return g_Vista.OnCreateDepthStencilView(pDevice, Resource, pDesc, DestDescriptor);
	}

	void STDMETHODCALLTYPE CreateShaderResourceView(ID3D12Device* pDevice, ID3D12Resource* Resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* pDesc, D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor)
	{
		return g_Vista.OnCreateShaderResourceView(pDevice, Resource, pDesc, DestDescriptor);
	}

	void STDMETHODCALLTYPE CreateUnorderedAccessView(ID3D12Device* pDevice, ID3D12Resource* Resource, ID3D12Resource* CounterResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* pDesc, D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor)
	{
		return g_Vista.OnCreateUnorderedAccessView(pDevice, Resource, CounterResource, pDesc, DestDescriptor);
	}

	void STDMETHODCALLTYPE CreateSampler(ID3D12Device* pDevice, const D3D12_SAMPLER_DESC* pDesc, D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor)
	{
		return g_Vista.OnCreateSampler(pDevice, pDesc, DestDescriptor);
	}

	void STDMETHODCALLTYPE CreateConstantBufferView(ID3D12Device* pDevice, const D3D12_CONSTANT_BUFFER_VIEW_DESC* pDesc, D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptor)
	{
		return g_Vista.OnCreateConstantBufferView(pDevice, pDesc, DestDescriptor);
	}

	void STDMETHODCALLTYPE CopyDescriptorsSimple(ID3D12Device* pDevice, UINT NumDescriptors, D3D12_CPU_DESCRIPTOR_HANDLE DestDescriptorRangeStart, D3D12_CPU_DESCRIPTOR_HANDLE SrcDescriptorRangeStart, D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeapsType)
	{
		return g_Vista.OnCopyDescriptorsSimple(pDevice, NumDescriptors, DestDescriptorRangeStart, SrcDescriptorRangeStart, DescriptorHeapsType);
	}

	void STDMETHODCALLTYPE CopyDescriptors(ID3D12Device* pDevice, UINT NumDestDescriptorRanges, const D3D12_CPU_DESCRIPTOR_HANDLE* pDestDescriptorRangeStarts, const UINT* pDestDescriptorRangeSizes,
		UINT NumSrcDescriptorRanges, const D3D12_CPU_DESCRIPTOR_HANDLE* pSrcDescriptorRangeStarts, const UINT* pSrcDescriptorRangeSizes, D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeapsType)
	{
		return g_Vista.OnCopyDescriptors(pDevice, NumDestDescriptorRanges, pDestDescriptorRangeStarts, pDestDescriptorRangeSizes, NumSrcDescriptorRanges, pSrcDescriptorRangeStarts, pSrcDescriptorRangeSizes, DescriptorHeapsType);
	}

	void STDMETHODCALLTYPE ExecuteCommandLists(ID3D12CommandQueue* pCommandQueue, UINT CommandListCount, ID3D12CommandList* const* CommandLists)
	{
		return g_Vista.OnExecuteCommandLists(pCommandQueue, CommandListCount, CommandLists);
	}

	HRESULT STDMETHODCALLTYPE Signal(ID3D12CommandQueue* pCommandQueue, ID3D12Fence* Fence, UINT64 FenceValue)
	{
		return g_Vista.OnSignal(pCommandQueue, Fence, FenceValue);
	}

	void STDMETHODCALLTYPE CopyBufferRegion(ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* pDstBuffer, UINT64 DstOffset, ID3D12Resource* pSrcBuffer, UINT64 SrcOffset, UINT64 NumBytes)
	{
		g_Vista.OnCopyBufferRegion(pCommandList, pDstBuffer, DstOffset, pSrcBuffer, SrcOffset, NumBytes);
	}

	void STDMETHODCALLTYPE CopyTextureRegion(ID3D12GraphicsCommandList* pCommandList, const D3D12_TEXTURE_COPY_LOCATION* pDst, UINT DstX, UINT DstY, UINT DstZ,
		const D3D12_TEXTURE_COPY_LOCATION* pSrc, const D3D12_BOX* pSrcBox)
	{
		g_Vista.OnCopyTextureRegion(pCommandList, pDst, DstX, DstY, DstZ, pSrc, pSrcBox);
	}

	void STDMETHODCALLTYPE CopyResource(ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* pDstResource, ID3D12Resource* pSrcResource)
	{
		g_Vista.OnCopyResource(pCommandList, pDstResource, pSrcResource);
	}

	void STDMETHODCALLTYPE ResolveSubresource(ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* pDstResource, UINT DstSubresource, ID3D12Resource* pSrcResource, UINT SrcSubresource, DXGI_FORMAT Format)
	{
		g_Vista.OnResolveSubresource(pCommandList, pDstResource, DstSubresource, pSrcResource, SrcSubresource, Format);
	}

	void STDMETHODCALLTYPE ResolveSubresourceRegion(ID3D12GraphicsCommandList1* pCommandList, ID3D12Resource* pDstResource, UINT DstSubresource, UINT DstX, UINT DstY, 
													ID3D12Resource* pSrcResource, UINT SrcSubresource, D3D12_RECT* pSrcRect, DXGI_FORMAT Format, D3D12_RESOLVE_MODE ResolveMode)
	{
		g_Vista.OnResolveSubresourceRegion(pCommandList, pDstResource, DstSubresource, DstX, DstY, pSrcResource, SrcSubresource, pSrcRect, Format, ResolveMode);
	}

	void STDMETHODCALLTYPE DrawInstanced(ID3D12GraphicsCommandList* pCommandList, UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation)
	{
		return g_Vista.OnDrawInstanced(pCommandList, VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
	}

	void STDMETHODCALLTYPE DrawIndexedInstanced(ID3D12GraphicsCommandList* pCommandList, UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation)
	{
		return g_Vista.OnDrawIndexedInstanced(pCommandList, IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
	}

	void STDMETHODCALLTYPE Dispatch(ID3D12GraphicsCommandList* pCommandList, UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ)
	{
		return g_Vista.OnDispatch(pCommandList, ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
	}

	void STDMETHODCALLTYPE DispatchMesh(ID3D12GraphicsCommandList6* pCommandList, UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ)
	{
		return g_Vista.OnDispatchMesh(pCommandList, ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
	}

	void STDMETHODCALLTYPE ExecuteIndirect(ID3D12GraphicsCommandList* pCommandList, ID3D12CommandSignature* pCommandSignature, UINT MaxCommandCount, ID3D12Resource* pArgumentBuffer, UINT64 ArgumentBufferOffset, ID3D12Resource* pCountBuffer, UINT64 CountBufferOffset)
	{
		return g_Vista.OnExecuteIndirect(pCommandList, pCommandSignature, MaxCommandCount, pArgumentBuffer, ArgumentBufferOffset, pCountBuffer, CountBufferOffset);
	}

	void STDMETHODCALLTYPE DispatchRays(ID3D12GraphicsCommandList4* pCommandList, const D3D12_DISPATCH_RAYS_DESC* pDesc)
	{
		return g_Vista.OnDispatchRays(pCommandList, pDesc);
	}

	void STDMETHODCALLTYPE RSSetViewports(ID3D12GraphicsCommandList* pCommandList, UINT ViewportCount, const D3D12_VIEWPORT* Viewports)
	{
		return g_Vista.OnRSSetViewports(pCommandList, ViewportCount, Viewports);
	}

	void STDMETHODCALLTYPE RSSetScissorRects(ID3D12GraphicsCommandList* pCommandList, UINT NumRects, const D3D12_RECT* pRects)
	{
		return g_Vista.OnRSSetScissorRects(pCommandList, NumRects, pRects);
	}

	void STDMETHODCALLTYPE RSSetShadingRate(ID3D12GraphicsCommandList5* pCommandList, D3D12_SHADING_RATE BaseShadingRate, const D3D12_SHADING_RATE_COMBINER* pCombiners)
	{
		return g_Vista.OnRSSetShadingRate(pCommandList, BaseShadingRate, pCombiners);
	}

	void STDMETHODCALLTYPE RSSetShadingRateImage(ID3D12GraphicsCommandList5* pCommandList, ID3D12Resource* ShadingRateImage)
	{
		return g_Vista.OnRSSetShadingRateImage(pCommandList, ShadingRateImage);
	}

	void STDMETHODCALLTYPE SetViewInstanceMask(ID3D12GraphicsCommandList1* pCommandList, UINT Mask)
	{
		return g_Vista.OnSetViewInstanceMask(pCommandList, Mask);
	}
	void STDMETHODCALLTYPE OMSetRenderTargets(ID3D12GraphicsCommandList* pCommandList, UINT NumRenderTargetDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE* pRenderTargetDescriptors, BOOL RTsSingleHandleToDescriptorRange, const D3D12_CPU_DESCRIPTOR_HANDLE* pDepthStencilDescriptor)
	{
		return g_Vista.OnOMSetRenderTargets(pCommandList, NumRenderTargetDescriptors, pRenderTargetDescriptors, RTsSingleHandleToDescriptorRange, pDepthStencilDescriptor);
	}

	void STDMETHODCALLTYPE OMSetBlendFactor(ID3D12GraphicsCommandList* pCommandList, const FLOAT* BlendFactor)
	{
		return g_Vista.OnOMSetBlendFactor(pCommandList, BlendFactor);
	}

	void STDMETHODCALLTYPE OMSetStencilRef(ID3D12GraphicsCommandList* pCommandList, UINT StencilRef)
	{
		return g_Vista.OnOMSetStencilRef(pCommandList, StencilRef);
	}

	void STDMETHODCALLTYPE OMSetDepthBounds(ID3D12GraphicsCommandList1* pCommandList, FLOAT Min, FLOAT Max)
	{
		return g_Vista.OnOMSetDepthBounds(pCommandList, Min, Max);
	}

	void STDMETHODCALLTYPE BeginRenderPass(ID3D12GraphicsCommandList4* pCommandList, UINT NumRenderTargets, const D3D12_RENDER_PASS_RENDER_TARGET_DESC* pRenderTargets, const D3D12_RENDER_PASS_DEPTH_STENCIL_DESC* pDepthStencil, D3D12_RENDER_PASS_FLAGS Flags)
	{
		return g_Vista.OnBeginRenderPass(pCommandList, NumRenderTargets, pRenderTargets, pDepthStencil, Flags);
	}

	void STDMETHODCALLTYPE EndRenderPass(ID3D12GraphicsCommandList4* pCommandList)
	{
		return g_Vista.OnEndRenderPass(pCommandList);
	}

	void STDMETHODCALLTYPE ClearRenderTargetView(ID3D12GraphicsCommandList* pCommandList, D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetView, const FLOAT* ColorRGBA, UINT NumRects, const D3D12_RECT* pRects)
	{
		return g_Vista.OnClearRenderTargetView(pCommandList, RenderTargetView, ColorRGBA, NumRects, pRects);
	}

	void STDMETHODCALLTYPE ClearDepthStencilView(ID3D12GraphicsCommandList* pCommandList, D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView, D3D12_CLEAR_FLAGS ClearFlags, FLOAT Depth, UINT8 Stencil, UINT NumRects, const D3D12_RECT* pRects)
	{
		return g_Vista.OnClearDepthStencilView(pCommandList, DepthStencilView, ClearFlags, Depth, Stencil, NumRects, pRects);
	}

	void STDMETHODCALLTYPE ClearUnorderedAccessViewUint(ID3D12GraphicsCommandList* pCommandList, D3D12_GPU_DESCRIPTOR_HANDLE ViewGPUHandleInCurrentHeap, D3D12_CPU_DESCRIPTOR_HANDLE ViewCPUHandle, ID3D12Resource* pResource, const UINT* Values, UINT NumRects, const D3D12_RECT* pRects)
	{
		return g_Vista.OnClearUnorderedAccessViewUint(pCommandList, ViewGPUHandleInCurrentHeap, ViewCPUHandle, pResource, Values, NumRects, pRects);
	}

	void STDMETHODCALLTYPE ClearUnorderedAccessViewFloat(ID3D12GraphicsCommandList* pCommandList, D3D12_GPU_DESCRIPTOR_HANDLE ViewGPUHandleInCurrentHeap, D3D12_CPU_DESCRIPTOR_HANDLE ViewCPUHandle, ID3D12Resource* pResource, const FLOAT* Values, UINT NumRects, const D3D12_RECT* pRects)
	{
		return g_Vista.OnClearUnorderedAccessViewFloat(pCommandList, ViewGPUHandleInCurrentHeap, ViewCPUHandle, pResource, Values, NumRects, pRects);
	}

	void STDMETHODCALLTYPE SetPipelineState(ID3D12GraphicsCommandList* pCommandList, ID3D12PipelineState* pPipelineState)
	{
		return g_Vista.OnSetPipelineState(pCommandList, pPipelineState);
	}

	void STDMETHODCALLTYPE SetPipelineState1(ID3D12GraphicsCommandList4* pCommandList, ID3D12StateObject* pStateObject)
	{
		return g_Vista.OnSetPipelineState1(pCommandList, pStateObject);
	}

	void STDMETHODCALLTYPE SetGraphicsRootSignature(ID3D12GraphicsCommandList* pCommandList, ID3D12RootSignature* pRootSignature)
	{
		return g_Vista.OnSetGraphicsRootSignature(pCommandList, pRootSignature);
	}

	void STDMETHODCALLTYPE SetComputeRootSignature(ID3D12GraphicsCommandList* pCommandList, ID3D12RootSignature* pRootSignature)
	{
		return g_Vista.OnSetComputeRootSignature(pCommandList, pRootSignature);
	}

	void STDMETHODCALLTYPE IASetPrimitiveTopology(ID3D12GraphicsCommandList* pCommandList, D3D12_PRIMITIVE_TOPOLOGY Topology)
	{
		return g_Vista.OnIASetPrimitiveTopology(pCommandList, Topology);
	}

	void STDMETHODCALLTYPE IASetVertexBuffers(ID3D12GraphicsCommandList* pCommandList, UINT StartSlot, UINT NumViews, const D3D12_VERTEX_BUFFER_VIEW* pViews)
	{
		return g_Vista.OnIASetVertexBuffers(pCommandList, StartSlot, NumViews, pViews);
	}

	void STDMETHODCALLTYPE IASetIndexBuffer(ID3D12GraphicsCommandList* pCommandList, const D3D12_INDEX_BUFFER_VIEW* pView)
	{
		return g_Vista.OnIASetIndexBuffer(pCommandList, pView);
	}

	void STDMETHODCALLTYPE ResourceBarrier(ID3D12GraphicsCommandList* pCommandList, UINT NumBarriers, const D3D12_RESOURCE_BARRIER* pBarriers)
	{
		return g_Vista.OnResourceBarrier(pCommandList, NumBarriers, pBarriers);
	}

	void STDMETHODCALLTYPE SetDescriptorHeaps(ID3D12GraphicsCommandList* pCommandList, UINT NumDescriptorHeaps, ID3D12DescriptorHeap* const* ppDescriptorHeaps)
	{
		return g_Vista.OnSetDescriptorHeaps(pCommandList, NumDescriptorHeaps, ppDescriptorHeaps);
	}

	void STDMETHODCALLTYPE SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* pCommandList, UINT RootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor)
	{
		return g_Vista.OnSetGraphicsRootDescriptorTable(pCommandList, RootParameterIndex, BaseDescriptor);
	}

	void STDMETHODCALLTYPE SetComputeRootDescriptorTable(ID3D12GraphicsCommandList* pCommandList, UINT RootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor)
	{
		return g_Vista.OnSetComputeRootDescriptorTable(pCommandList, RootParameterIndex, BaseDescriptor);
	}

	void STDMETHODCALLTYPE SetComputeRoot32BitConstant(ID3D12GraphicsCommandList* pCommandList, UINT RootParameterIndex, UINT SrcData, UINT DestOffsetIn32BitValues)
	{
		return g_Vista.OnSetComputeRoot32BitConstant(pCommandList, RootParameterIndex, SrcData, DestOffsetIn32BitValues);
	}

	void STDMETHODCALLTYPE SetGraphicsRoot32BitConstant(ID3D12GraphicsCommandList* pCommandList, UINT RootParameterIndex, UINT SrcData, UINT DestOffsetIn32BitValues)
	{
		return g_Vista.OnSetGraphicsRoot32BitConstant(pCommandList, RootParameterIndex, SrcData, DestOffsetIn32BitValues);
	}

	void STDMETHODCALLTYPE SetGraphicsRoot32BitConstants(ID3D12GraphicsCommandList* pCommandList, UINT RootParameterIndex, UINT Num32BitValuesToSet, const void* pSrcData, UINT DestOffsetIn32BitValues)
	{
		return g_Vista.OnSetGraphicsRoot32BitConstants(pCommandList, RootParameterIndex, Num32BitValuesToSet, pSrcData, DestOffsetIn32BitValues);
	}

	void STDMETHODCALLTYPE SetComputeRoot32BitConstants(ID3D12GraphicsCommandList* pCommandList, UINT RootParameterIndex, UINT Num32BitValuesToSet, const void* pSrcData, UINT DestOffsetIn32BitValues)
	{
		return g_Vista.OnSetComputeRoot32BitConstants(pCommandList, RootParameterIndex, Num32BitValuesToSet, pSrcData, DestOffsetIn32BitValues);
	}

	void STDMETHODCALLTYPE SetGraphicsRootConstantBufferView(ID3D12GraphicsCommandList* pCommandList, UINT RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation)
	{
		return g_Vista.OnSetGraphicsRootConstantBufferView(pCommandList, RootParameterIndex, BufferLocation);
	}

	void STDMETHODCALLTYPE SetComputeRootConstantBufferView(ID3D12GraphicsCommandList* pCommandList, UINT RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation)
	{
		return g_Vista.OnSetComputeRootConstantBufferView(pCommandList, RootParameterIndex, BufferLocation);
	}

	void STDMETHODCALLTYPE SetGraphicsRootShaderResourceView(ID3D12GraphicsCommandList* pCommandList, UINT RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation)
	{
		return g_Vista.OnSetGraphicsRootShaderResourceView(pCommandList, RootParameterIndex, BufferLocation);
	}

	void STDMETHODCALLTYPE SetComputeRootShaderResourceView(ID3D12GraphicsCommandList* pCommandList, UINT RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation)
	{
		return g_Vista.OnSetComputeRootShaderResourceView(pCommandList, RootParameterIndex, BufferLocation);
	}

	void STDMETHODCALLTYPE SetGraphicsRootUnorderedAccessView(ID3D12GraphicsCommandList* pCommandList, UINT RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation)
	{
		return g_Vista.OnSetGraphicsRootUnorderedAccessView(pCommandList, RootParameterIndex, BufferLocation);
	}
	
	void STDMETHODCALLTYPE SetComputeRootUnorderedAccessView(ID3D12GraphicsCommandList* pCommandList, UINT RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation)
	{
		return g_Vista.OnSetComputeRootUnorderedAccessView(pCommandList, RootParameterIndex, BufferLocation);
	}

	HRESULT STDMETHODCALLTYPE Close(ID3D12GraphicsCommandList* pCommandList)
	{
		return g_Vista.OnClose(pCommandList);
	}

	HRESULT STDMETHODCALLTYPE Reset(ID3D12GraphicsCommandList* pCommandList, ID3D12CommandAllocator* CmdAllocator, ID3D12PipelineState* PipelineState)
	{
		return g_Vista.OnReset(pCommandList, CmdAllocator, PipelineState);
	}

	void STDMETHODCALLTYPE ClearState(ID3D12GraphicsCommandList* pCommandList, ID3D12PipelineState* pPipelineState)
	{
		return g_Vista.OnClearState(pCommandList, pPipelineState);
	}

	void STDMETHODCALLTYPE BeginEvent(ID3D12GraphicsCommandList* pCommandList, UINT Metadata, const void* pData, UINT Size)
	{
		return g_Vista.OnBeginEvent(pCommandList, Metadata, pData, Size);
	}

	void STDMETHODCALLTYPE EndEvent(ID3D12GraphicsCommandList* pCommandList)
	{
		return g_Vista.OnEndEvent(pCommandList);
	}

	ULONG STDMETHODCALLTYPE Release(ID3D12Resource* pResource)
	{
		return g_Vista.OnRelease(pResource);
	}

	HRESULT STDMETHODCALLTYPE SetName(ID3D12Resource* pResource, LPCWSTR Name)
	{
		return g_Vista.OnSetName(pResource, Name);
	}

	HRESULT STDMETHODCALLTYPE Map(ID3D12Resource* pResource, UINT Subresource, const D3D12_RANGE* pReadRange, void** ppData)
	{
		return g_Vista.OnMap(pResource, Subresource, pReadRange, ppData);
	}

	void STDMETHODCALLTYPE Unmap(ID3D12Resource* pResource, UINT Subresource, const D3D12_RANGE* pWrittenRange)
	{
		return g_Vista.OnUnmap(pResource, Subresource, pWrittenRange);
	}

	UINT64 STDMETHODCALLTYPE GetCompletedValue(ID3D12Fence* pFence)
	{
		return g_Vista.OnGetCompletedValue(pFence);
	}

	HRESULT STDMETHODCALLTYPE SetEventOnCompletion(ID3D12Fence* pFence, UINT64 Value, HANDLE Event)
	{
		return g_Vista.OnSetEventOnCompletion(pFence, Value, Event);
	}

	HRESULT STDMETHODCALLTYPE Signal(ID3D12Fence* pFence, UINT64 Value)
	{
		return g_Vista.OnSignal(pFence, Value);
	}
}

