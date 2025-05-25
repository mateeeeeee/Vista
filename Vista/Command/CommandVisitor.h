#pragma once

namespace vista
{
	class Command;

	class CreateCommandQueueCommand;
	class CreateCommandQueue1Command;
	class CreateCommandListCommand;
	class CreateCommandList1Command;
	class CreateCommandAllocatorCommand;
	class CreateFenceCommand;
	class CreateHeapCommand;
	class CreateHeap1Command;
	class CreateRootSignatureCommand;
	class CreateCommandSignatureCommand;
	class CreateGraphicsPipelineStateCommand;
	class CreateComputePipelineStateCommand;
	class CreatePipelineStateCommand;
	class CreateStateObjectCommand;
	class AddToStateObjectCommand;
	class CreateCommittedResourceCommand;
	class CreateCommittedResource1Command;
	class CreatePlacedResourceCommand;
	class CreatePlacedResource1Command;
	class CreateDescriptorHeapCommand;

	class ExecuteCommandListsCommand;
	class QueueSignalCommand;

	class CopyBufferRegionCommand;
	class CopyTextureRegionCommand;
	class CopyResourceCommand;
	class ResolveSubresourceCommand;
	class ResolveSubresourceRegionCommand;
	class DrawInstancedCommand;
	class DrawIndexedInstancedCommand;
	class DispatchCommand;
	class DispatchMeshCommand;
	class ExecuteIndirectCommand;
	class DispatchRaysCommand;
	class RSSetViewportsCommand;
	class RSSetScissorRectsCommand;
	class RSSetShadingRateCommand;
	class RSSetShadingRateImageCommand;
	class SetViewInstanceMaskCommand;
	class OMSetRenderTargetsCommand;
	class OMSetBlendFactorCommand;
	class OMSetStencilRefCommand;
	class OMSetDepthBoundsCommand;
	class BeginRenderPassCommand;
	class EndRenderPassCommand;
	class ClearRenderTargetViewCommand;
	class ClearDepthStencilViewCommand;
	class ClearUnorderedAccessViewUintCommand;
	class ClearUnorderedAccessViewFloatCommand;
	class SetPipelineStateCommand;
	class SetPipelineState1Command;
	class SetGraphicsRootSignatureCommand;
	class SetComputeRootSignatureCommand;
	class IASetPrimitiveTopologyCommand;
	class IASetVertexBuffersCommand;
	class IASetIndexBufferCommand;
	class ResourceBarrierCommand;
	class SetDescriptorHeapsCommand;
	class SetGraphicsRootDescriptorTableCommand;
	class SetComputeRootDescriptorTableCommand;
	class SetGraphicsRoot32BitConstantCommand;
	class SetComputeRoot32BitConstantCommand;
	class SetGraphicsRoot32BitConstantsCommand;
	class SetComputeRoot32BitConstantsCommand;
	class SetGraphicsRootConstantBufferViewCommand;
	class SetComputeRootConstantBufferViewCommand;
	class SetGraphicsRootShaderResourceViewCommand;
	class SetComputeRootShaderResourceViewCommand;
	class SetGraphicsRootUnorderedAccessViewCommand;
	class SetComputeRootUnorderedAccessViewCommand;
	class CloseListCommand;
	class ResetListCommand;
	class ClearStateCommand;
	class BeginEventCommand;
	class EndEventCommand;

	class ResourceSetNameCommand;

	class GetCompletedValueCommand;
	class SetEventOnCompletionCommand;
	class FenceSignalCommand;

	struct CommandVisitor
	{
		virtual ~CommandVisitor() = default;

		virtual void Visit(Command const&) = 0;

		virtual void Visit(CreateCommandQueueCommand const&) = 0;
		virtual void Visit(CreateCommandQueue1Command const&) = 0;
		virtual void Visit(CreateCommandListCommand const&) = 0;
		virtual void Visit(CreateCommandList1Command const&) = 0;
		virtual void Visit(CreateCommandAllocatorCommand const&) = 0;
		virtual void Visit(CreateFenceCommand const&) = 0;
		virtual void Visit(CreateHeapCommand const&) = 0;
		virtual void Visit(CreateHeap1Command const&) = 0;
		virtual void Visit(CreateRootSignatureCommand const&) = 0;
		virtual void Visit(CreateCommandSignatureCommand const&) = 0;
		virtual void Visit(CreateGraphicsPipelineStateCommand const&) = 0;
		virtual void Visit(CreateComputePipelineStateCommand const&) = 0;
		virtual void Visit(CreatePipelineStateCommand const&) = 0;
		virtual void Visit(CreateStateObjectCommand const&) = 0;
		virtual void Visit(AddToStateObjectCommand const&) = 0;
		virtual void Visit(CreateCommittedResourceCommand const&) = 0;
		virtual void Visit(CreateCommittedResource1Command const&) = 0;
		virtual void Visit(CreatePlacedResourceCommand const&) = 0;
		virtual void Visit(CreatePlacedResource1Command const&) = 0;
		virtual void Visit(CreateDescriptorHeapCommand const&) = 0;

		virtual void Visit(ExecuteCommandListsCommand const&) = 0;
		virtual void Visit(QueueSignalCommand const&) = 0;

		virtual void Visit(CopyBufferRegionCommand const&) = 0;
		virtual void Visit(CopyTextureRegionCommand const&) = 0;
		virtual void Visit(CopyResourceCommand const&) = 0;
		virtual void Visit(ResolveSubresourceCommand const&) = 0;
		virtual void Visit(ResolveSubresourceRegionCommand const&) = 0;
		virtual void Visit(DrawInstancedCommand const&) = 0;
		virtual void Visit(DrawIndexedInstancedCommand const&) = 0;
		virtual void Visit(DispatchCommand const&) = 0;
		virtual void Visit(DispatchMeshCommand const&) = 0;
		virtual void Visit(ExecuteIndirectCommand const&) = 0;
		virtual void Visit(DispatchRaysCommand const&) = 0;
		virtual void Visit(RSSetViewportsCommand const&) = 0;
		virtual void Visit(RSSetScissorRectsCommand const&) = 0;
		virtual void Visit(RSSetShadingRateCommand const&) = 0;
		virtual void Visit(RSSetShadingRateImageCommand const&) = 0;
		virtual void Visit(SetViewInstanceMaskCommand const&) = 0;
		virtual void Visit(OMSetRenderTargetsCommand const&) = 0;
		virtual void Visit(OMSetBlendFactorCommand const&) = 0;
		virtual void Visit(OMSetStencilRefCommand const&) = 0;
		virtual void Visit(OMSetDepthBoundsCommand const&) = 0;
		virtual void Visit(BeginRenderPassCommand const&) = 0;
		virtual void Visit(EndRenderPassCommand const&) = 0;
		virtual void Visit(ClearRenderTargetViewCommand const&) = 0;
		virtual void Visit(ClearDepthStencilViewCommand const&) = 0;
		virtual void Visit(ClearUnorderedAccessViewUintCommand const&) = 0;
		virtual void Visit(ClearUnorderedAccessViewFloatCommand const&) = 0;
		virtual void Visit(SetPipelineStateCommand const&) = 0;
		virtual void Visit(SetPipelineState1Command const&) = 0;
		virtual void Visit(SetGraphicsRootSignatureCommand const&) = 0;
		virtual void Visit(SetComputeRootSignatureCommand const&) = 0;
		virtual void Visit(IASetPrimitiveTopologyCommand const&) = 0;
		virtual void Visit(IASetVertexBuffersCommand const&) = 0;
		virtual void Visit(IASetIndexBufferCommand const&) = 0;
		virtual void Visit(ResourceBarrierCommand const&) = 0;
		virtual void Visit(SetDescriptorHeapsCommand const&) = 0;
		virtual void Visit(SetGraphicsRootDescriptorTableCommand const&) = 0;
		virtual void Visit(SetComputeRootDescriptorTableCommand const&) = 0;
		virtual void Visit(SetGraphicsRoot32BitConstantCommand const&) = 0;
		virtual void Visit(SetComputeRoot32BitConstantCommand const&) = 0;
		virtual void Visit(SetGraphicsRoot32BitConstantsCommand const&) = 0;
		virtual void Visit(SetComputeRoot32BitConstantsCommand const&) = 0;
		virtual void Visit(SetGraphicsRootConstantBufferViewCommand const&) = 0;
		virtual void Visit(SetComputeRootConstantBufferViewCommand const&) = 0;
		virtual void Visit(SetGraphicsRootShaderResourceViewCommand const&) = 0;
		virtual void Visit(SetComputeRootShaderResourceViewCommand const&) = 0;
		virtual void Visit(SetGraphicsRootUnorderedAccessViewCommand const&) = 0;
		virtual void Visit(SetComputeRootUnorderedAccessViewCommand const&) = 0;

		virtual void Visit(CloseListCommand const&) = 0;
		virtual void Visit(ResetListCommand const&) = 0;
		virtual void Visit(ClearStateCommand const&) = 0;
		virtual void Visit(BeginEventCommand const&) = 0;
		virtual void Visit(EndEventCommand const&) = 0;

		virtual void Visit(ResourceSetNameCommand const&) = 0;

		virtual void Visit(GetCompletedValueCommand const&) = 0;
		virtual void Visit(SetEventOnCompletionCommand const&) = 0;
		virtual void Visit(FenceSignalCommand const&) = 0;
	};

	struct DummyCommandVisitor : CommandVisitor
	{
		virtual ~DummyCommandVisitor() = default;

		virtual void Visit(Command const&) {}

		virtual void Visit(CreateCommandQueueCommand const&) {}
		virtual void Visit(CreateCommandQueue1Command const&) {}
		virtual void Visit(CreateCommandListCommand const&) {}
		virtual void Visit(CreateCommandList1Command const&) {}
		virtual void Visit(CreateCommandAllocatorCommand const&) {}
		virtual void Visit(CreateFenceCommand const&) {}
		virtual void Visit(CreateHeapCommand const&) {}
		virtual void Visit(CreateHeap1Command const&) {}
		virtual void Visit(CreateRootSignatureCommand const&) {}
		virtual void Visit(CreateCommandSignatureCommand const&) {}
		virtual void Visit(CreateGraphicsPipelineStateCommand const&) {}
		virtual void Visit(CreateComputePipelineStateCommand const&) {}
		virtual void Visit(CreatePipelineStateCommand const&) {}
		virtual void Visit(CreateStateObjectCommand const&) {}
		virtual void Visit(AddToStateObjectCommand const&) {}
		virtual void Visit(CreateCommittedResourceCommand const&) {}
		virtual void Visit(CreateCommittedResource1Command const&) {}
		virtual void Visit(CreatePlacedResourceCommand const&) {}
		virtual void Visit(CreatePlacedResource1Command const&) {}
		virtual void Visit(CreateDescriptorHeapCommand const&) {}

		virtual void Visit(ExecuteCommandListsCommand const&) {}
		virtual void Visit(QueueSignalCommand const&) {}

		virtual void Visit(CopyBufferRegionCommand const&) {}
		virtual void Visit(CopyTextureRegionCommand const&) {}
		virtual void Visit(CopyResourceCommand const&) {}
		virtual void Visit(ResolveSubresourceCommand const&) {}
		virtual void Visit(ResolveSubresourceRegionCommand const&) {}
		virtual void Visit(DrawInstancedCommand const&) {}
		virtual void Visit(DrawIndexedInstancedCommand const&) {}
		virtual void Visit(DispatchCommand const&) {}
		virtual void Visit(DispatchMeshCommand const&) {}
		virtual void Visit(ExecuteIndirectCommand const&) {}
		virtual void Visit(DispatchRaysCommand const&) {}
		virtual void Visit(RSSetViewportsCommand const&) {}
		virtual void Visit(RSSetScissorRectsCommand const&) {}
		virtual void Visit(RSSetShadingRateCommand const&) {}
		virtual void Visit(RSSetShadingRateImageCommand const&) {}
		virtual void Visit(SetViewInstanceMaskCommand const&) {}
		virtual void Visit(OMSetRenderTargetsCommand const&) {}
		virtual void Visit(OMSetBlendFactorCommand const&) {}
		virtual void Visit(OMSetStencilRefCommand const&) {}
		virtual void Visit(OMSetDepthBoundsCommand const&) {}
		virtual void Visit(BeginRenderPassCommand const&) {}
		virtual void Visit(EndRenderPassCommand const&) {}
		virtual void Visit(ClearRenderTargetViewCommand const&) {}
		virtual void Visit(ClearDepthStencilViewCommand const&) {}
		virtual void Visit(ClearUnorderedAccessViewUintCommand const&) {}
		virtual void Visit(ClearUnorderedAccessViewFloatCommand const&) {}
		virtual void Visit(SetPipelineStateCommand const&) {}
		virtual void Visit(SetPipelineState1Command const&) {}
		virtual void Visit(SetGraphicsRootSignatureCommand const&) {}
		virtual void Visit(SetComputeRootSignatureCommand const&) {}
		virtual void Visit(IASetPrimitiveTopologyCommand const&) {}
		virtual void Visit(IASetVertexBuffersCommand const&) {}
		virtual void Visit(IASetIndexBufferCommand const&) {}
		virtual void Visit(ResourceBarrierCommand const&) {}
		virtual void Visit(SetDescriptorHeapsCommand const&) {}
		virtual void Visit(SetGraphicsRootDescriptorTableCommand const&) {}
		virtual void Visit(SetComputeRootDescriptorTableCommand const&) {}
		virtual void Visit(SetGraphicsRoot32BitConstantCommand const&) {}
		virtual void Visit(SetComputeRoot32BitConstantCommand const&) {}
		virtual void Visit(SetGraphicsRoot32BitConstantsCommand const&) {}
		virtual void Visit(SetComputeRoot32BitConstantsCommand const&) {}
		virtual void Visit(SetGraphicsRootConstantBufferViewCommand const&) {}
		virtual void Visit(SetComputeRootConstantBufferViewCommand const&) {}
		virtual void Visit(SetGraphicsRootShaderResourceViewCommand const&) {}
		virtual void Visit(SetComputeRootShaderResourceViewCommand const&) {}
		virtual void Visit(SetGraphicsRootUnorderedAccessViewCommand const&) {}
		virtual void Visit(SetComputeRootUnorderedAccessViewCommand const&) {}
		virtual void Visit(CloseListCommand const&) {}
		virtual void Visit(ResetListCommand const&) {}
		virtual void Visit(ClearStateCommand const&) {}
		virtual void Visit(BeginEventCommand const&) {}
		virtual void Visit(EndEventCommand const&) {}

		virtual void Visit(ResourceSetNameCommand const&) {}

		virtual void Visit(GetCompletedValueCommand const&) {}
		virtual void Visit(SetEventOnCompletionCommand const&) {}
		virtual void Visit(FenceSignalCommand const&) {}
	};
}