#pragma once
#include "Command/CommandVisitor.h"
#include "Tracking/ObjectID.h"

namespace vista
{
	class ObjectTracker;
	class DescriptorTracker;
	class ResourceAddressTracker;

	class EventDetailsCommandVisitor : public CommandVisitor
	{
	public:
		EventDetailsCommandVisitor(ObjectTracker const& objectTracker, DescriptorTracker const& descriptorTracker, ResourceAddressTracker const& addressTracker)
			: objectTracker(objectTracker), descriptorTracker(descriptorTracker), addressTracker(addressTracker)
		{
		}

		virtual void Visit(Command const& cmd) override;

		virtual void Visit(CreateCommandQueueCommand const&) override;
		virtual void Visit(CreateCommandQueue1Command const&) override;
		virtual void Visit(CreateCommandListCommand const&) override;
		virtual void Visit(CreateCommandList1Command const&) override;
		virtual void Visit(CreateCommandAllocatorCommand const&) override;
		virtual void Visit(CreateFenceCommand const&) override;
		virtual void Visit(CreateHeapCommand const&) override;
		virtual void Visit(CreateHeap1Command const&) override;
		virtual void Visit(CreateGraphicsPipelineStateCommand const&) override;
		virtual void Visit(CreateComputePipelineStateCommand const&) override;
		virtual void Visit(CreatePipelineStateCommand const&) override;
		virtual void Visit(CreateStateObjectCommand const&) override;
		virtual void Visit(AddToStateObjectCommand const&) override;
		virtual void Visit(CreateRootSignatureCommand const&) override;
		virtual void Visit(CreateCommandSignatureCommand const&) override;
		virtual void Visit(CreateCommittedResourceCommand const&) override;
		virtual void Visit(CreateCommittedResource1Command const&) override;
		virtual void Visit(CreatePlacedResourceCommand const&) override;
		virtual void Visit(CreatePlacedResource1Command const&) override;
		virtual void Visit(CreateDescriptorHeapCommand const&) override;

		virtual void Visit(ExecuteCommandListsCommand const& cmd) override;
		virtual void Visit(QueueSignalCommand const& cmd) override;

		virtual void Visit(CopyBufferRegionCommand const&) override;
		virtual void Visit(CopyTextureRegionCommand const&) override;
		virtual void Visit(CopyResourceCommand const&) override;
		virtual void Visit(ResolveSubresourceCommand const&) override;
		virtual void Visit(ResolveSubresourceRegionCommand const&) override;
		virtual void Visit(DrawInstancedCommand const& cmd) override;
		virtual void Visit(DrawIndexedInstancedCommand const& cmd) override;
		virtual void Visit(DispatchCommand const& cmd) override;
		virtual void Visit(DispatchMeshCommand const& cmd) override;
		virtual void Visit(ExecuteIndirectCommand const& cmd) override;
		virtual void Visit(DispatchRaysCommand const& cmd) override;
		virtual void Visit(RSSetViewportsCommand const& cmd) override;
		virtual void Visit(RSSetScissorRectsCommand const& cmd) override;
		virtual void Visit(RSSetShadingRateCommand const& cmd) override;
		virtual void Visit(RSSetShadingRateImageCommand const& cmd) override;
		virtual void Visit(SetViewInstanceMaskCommand const& cmd) override;
		virtual void Visit(OMSetRenderTargetsCommand const& cmd) override;
		virtual void Visit(OMSetBlendFactorCommand const& cmd) override;
		virtual void Visit(OMSetStencilRefCommand const& cmd) override;
		virtual void Visit(OMSetDepthBoundsCommand const& cmd) override;
		virtual void Visit(BeginRenderPassCommand const&) override;
		virtual void Visit(EndRenderPassCommand const&) override;
		virtual void Visit(ClearRenderTargetViewCommand const& cmd) override;
		virtual void Visit(ClearDepthStencilViewCommand const& cmd) override;
		virtual void Visit(ClearUnorderedAccessViewUintCommand const&) override;
		virtual void Visit(ClearUnorderedAccessViewFloatCommand const&) override;
		virtual void Visit(SetPipelineStateCommand const& cmd) override;
		virtual void Visit(SetPipelineState1Command const& cmd) override;
		virtual void Visit(SetGraphicsRootSignatureCommand const& cmd) override;
		virtual void Visit(SetComputeRootSignatureCommand const& cmd) override;
		virtual void Visit(IASetPrimitiveTopologyCommand const& cmd) override;
		virtual void Visit(IASetVertexBuffersCommand const& cmd) override;
		virtual void Visit(IASetIndexBufferCommand const& cmd) override;
		virtual void Visit(ResourceBarrierCommand const& cmd) override;
		virtual void Visit(SetDescriptorHeapsCommand const& cmd) override;
		virtual void Visit(SetGraphicsRootDescriptorTableCommand const& cmd) override;
		virtual void Visit(SetComputeRootDescriptorTableCommand const& cmd) override;
		virtual void Visit(SetGraphicsRoot32BitConstantCommand const&) override;
		virtual void Visit(SetComputeRoot32BitConstantCommand const&) override;
		virtual void Visit(SetGraphicsRoot32BitConstantsCommand const&) override;
		virtual void Visit(SetComputeRoot32BitConstantsCommand const&) override;
		virtual void Visit(SetGraphicsRootConstantBufferViewCommand const&) override;
		virtual void Visit(SetComputeRootConstantBufferViewCommand const&) override;
		virtual void Visit(SetGraphicsRootShaderResourceViewCommand const&) override;
		virtual void Visit(SetComputeRootShaderResourceViewCommand const&) override;
		virtual void Visit(SetGraphicsRootUnorderedAccessViewCommand const&) override;
		virtual void Visit(SetComputeRootUnorderedAccessViewCommand const&) override;
		virtual void Visit(CloseListCommand const& cmd) override;
		virtual void Visit(ResetListCommand const& cmd) override;
		virtual void Visit(ClearStateCommand const& cmd) override;
		virtual void Visit(BeginEventCommand const&) override {}
		virtual void Visit(EndEventCommand const&) override {}

		virtual void Visit(ResourceSetNameCommand const& cmd) override;

		virtual void Visit(GetCompletedValueCommand const& cmd) override;
		virtual void Visit(SetEventOnCompletionCommand const& cmd) override;
		virtual void Visit(FenceSignalCommand const& cmd) override;

	private:
		ObjectTracker const& objectTracker;
		DescriptorTracker const& descriptorTracker;
		ResourceAddressTracker const& addressTracker;

	private:
		void VisitSetRootViewCommandCommon(ObjectID parentId, Uint32 rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS gpuVA, Char const* label);
	};
}