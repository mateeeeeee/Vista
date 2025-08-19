#pragma once
#include "DirectHook/directhook.h"

using namespace directhook;

namespace vista
{
	struct D3D12PFNs
	{
		d3d12::PFN_DXGISwapChain_Present Present;

		d3d12::PFN_D3D12Device_CreateCommandQueue CreateCommandQueue;
		d3d12::PFN_D3D12Device9_CreateCommandQueue1 CreateCommandQueue1;
		d3d12::PFN_D3D12Device_CreateCommandList CreateCommandList;
		d3d12::PFN_D3D12Device4_CreateCommandList1 CreateCommandList1;
		d3d12::PFN_D3D12Device_CreateCommandAllocator CreateCommandAllocator;
		d3d12::PFN_D3D12Device_CreateFence CreateFence;
		d3d12::PFN_D3D12Device_CreateHeap CreateHeap;
		d3d12::PFN_D3D12Device4_CreateHeap1 CreateHeap1;
		d3d12::PFN_D3D12Device_CreateRootSignature CreateRootSignature;
		d3d12::PFN_D3D12Device_CreateCommandSignature CreateCommandSignature;
		d3d12::PFN_D3D12Device_CreateGraphicsPipelineState CreateGraphicsPipelineState;
		d3d12::PFN_D3D12Device_CreateComputePipelineState CreateComputePipelineState;
		d3d12::PFN_D3D12Device2_CreatePipelineState CreatePipelineState;
		d3d12::PFN_D3D12Device5_CreateStateObject CreateStateObject;
		d3d12::PFN_D3D12Device7_AddToStateObject AddToStateObject;
		d3d12::PFN_D3D12Device_CreateCommittedResource CreateCommittedResource;
		d3d12::PFN_D3D12Device4_CreateCommittedResource1 CreateCommittedResource1;
		d3d12::PFN_D3D12Device_CreatePlacedResource CreatePlacedResource;
		d3d12::PFN_D3D12Device8_CreatePlacedResource1 CreatePlacedResource1;
		d3d12::PFN_D3D12Device_CreateDescriptorHeap CreateDescriptorHeap;
		d3d12::PFN_D3D12Device_CreateRenderTargetView CreateRenderTargetView;
		d3d12::PFN_D3D12Device_CreateDepthStencilView CreateDepthStencilView;
		d3d12::PFN_D3D12Device_CreateShaderResourceView CreateShaderResourceView;
		d3d12::PFN_D3D12Device_CreateUnorderedAccessView CreateUnorderedAccessView;
		d3d12::PFN_D3D12Device_CreateConstantBufferView CreateConstantBufferView;
		d3d12::PFN_D3D12Device_CreateSampler CreateSampler;
		d3d12::PFN_D3D12Device_CopyDescriptorsSimple CopyDescriptorsSimple;
		d3d12::PFN_D3D12Device_CopyDescriptors CopyDescriptors;

		d3d12::PFN_D3D12CommandQueue_ExecuteCommandLists ExecuteCommandLists;
		d3d12::PFN_D3D12CommandQueue_Signal Queue_Signal;

		d3d12::PFN_D3D12CommandList_CopyBufferRegion CopyBufferRegion;
		d3d12::PFN_D3D12CommandList_CopyTextureRegion CopyTextureRegion;
		d3d12::PFN_D3D12CommandList_CopyResource CopyResource;
		d3d12::PFN_D3D12CommandList_ResolveSubresource ResolveSubresource;
		d3d12::PFN_D3D12CommandList1_ResolveSubresourceRegion ResolveSubresourceRegion;
		d3d12::PFN_D3D12CommandList_DrawInstanced DrawInstanced;
		d3d12::PFN_D3D12CommandList_DrawIndexedInstanced DrawIndexedInstanced;
		d3d12::PFN_D3D12CommandList_Dispatch Dispatch;
		d3d12::PFN_D3D12CommandList6_DispatchMesh DispatchMesh;
		d3d12::PFN_D3D12CommandList_ExecuteIndirect ExecuteIndirect;
		d3d12::PFN_D3D12CommandList4_DispatchRays DispatchRays;
		d3d12::PFN_D3D12CommandList_RSSetViewports RSSetViewports;
		d3d12::PFN_D3D12CommandList_RSSetScissorRects RSSetScissorRects;
		d3d12::PFN_D3D12CommandList5_RSSetShadingRate RSSetShadingRate;
		d3d12::PFN_D3D12CommandList5_RSSetShadingRateImage RSSetShadingRateImage;
		d3d12::PFN_D3D12CommandList1_SetViewInstanceMask SetViewInstanceMask;
		d3d12::PFN_D3D12CommandList_OMSetRenderTargets OMSetRenderTargets;
		d3d12::PFN_D3D12CommandList_OMSetBlendFactor OMSetBlendFactor;
		d3d12::PFN_D3D12CommandList_OMSetStencilRef OMSetStencilRef;
		d3d12::PFN_D3D12CommandList1_OMSetDepthBounds OMSetDepthBounds;
		d3d12::PFN_D3D12CommandList4_BeginRenderPass BeginRenderPass;
		d3d12::PFN_D3D12CommandList4_EndRenderPass EndRenderPass;
		d3d12::PFN_D3D12CommandList_ClearRenderTargetView ClearRenderTargetView;
		d3d12::PFN_D3D12CommandList_ClearDepthStencilView ClearDepthStencilView;
		d3d12::PFN_D3D12CommandList_ClearUnorderedAccessViewUint ClearUnorderedAccessViewUint;
		d3d12::PFN_D3D12CommandList_ClearUnorderedAccessViewFloat ClearUnorderedAccessViewFloat;
		d3d12::PFN_D3D12CommandList_SetPipelineState SetPipelineState;
		d3d12::PFN_D3D12CommandList4_SetPipelineState1 SetPipelineState1;
		d3d12::PFN_D3D12CommandList_SetGraphicsRootSignature SetGraphicsRootSignature;
		d3d12::PFN_D3D12CommandList_SetComputeRootSignature SetComputeRootSignature;
		d3d12::PFN_D3D12CommandList_IASetPrimitiveTopology IASetPrimitiveTopology;
		d3d12::PFN_D3D12CommandList_IASetVertexBuffers IASetVertexBuffers;
		d3d12::PFN_D3D12CommandList_IASetIndexBuffer IASetIndexBuffer;
		d3d12::PFN_D3D12CommandList_ResourceBarrier ResourceBarrier;
		d3d12::PFN_D3D12CommandList_SetDescriptorHeaps SetDescriptorHeaps;

		d3d12::PFN_D3D12CommandList_SetGraphicsRootDescriptorTable SetGraphicsRootDescriptorTable;
		d3d12::PFN_D3D12CommandList_SetComputeRootDescriptorTable SetComputeRootDescriptorTable;

		d3d12::PFN_D3D12CommandList_SetComputeRoot32BitConstant SetComputeRoot32BitConstant;
		d3d12::PFN_D3D12CommandList_SetGraphicsRoot32BitConstant SetGraphicsRoot32BitConstant;
		d3d12::PFN_D3D12CommandList_SetComputeRoot32BitConstants SetComputeRoot32BitConstants;
		d3d12::PFN_D3D12CommandList_SetGraphicsRoot32BitConstants SetGraphicsRoot32BitConstants;
		d3d12::PFN_D3D12CommandList_SetComputeRootConstantBufferView SetComputeRootConstantBufferView;
		d3d12::PFN_D3D12CommandList_SetGraphicsRootConstantBufferView SetGraphicsRootConstantBufferView;
		d3d12::PFN_D3D12CommandList_SetComputeRootShaderResourceView SetComputeRootShaderResourceView;
		d3d12::PFN_D3D12CommandList_SetGraphicsRootShaderResourceView SetGraphicsRootShaderResourceView;
		d3d12::PFN_D3D12CommandList_SetComputeRootUnorderedAccessView SetComputeRootUnorderedAccessView;
		d3d12::PFN_D3D12CommandList_SetGraphicsRootUnorderedAccessView SetGraphicsRootUnorderedAccessView;

		d3d12::PFN_D3D12CommandList_Close Close;
		d3d12::PFN_D3D12CommandList_Reset List_Reset;
		d3d12::PFN_D3D12CommandList_ClearState ClearState;
		d3d12::PFN_D3D12CommandList_BeginEvent BeginEvent;
		d3d12::PFN_D3D12CommandList_EndEvent EndEvent;

		d3d12::PFN_D3D12Resource_Release Resource_Release;
		d3d12::PFN_D3D12Resource_SetName Resource_SetName;
		d3d12::PFN_D3D12Resource_Map	 Resource_Map;
		d3d12::PFN_D3D12Resource_Unmap	 Resource_Unmap;

		d3d12::PFN_D3D12Fence_GetCompletedValue GetCompletedValue;
		d3d12::PFN_D3D12Fence_SetEventOnCompletion SetEventOnCompletion;
		d3d12::PFN_D3D12Fence_Signal Fence_Signal;
	};
}