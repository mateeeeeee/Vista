#pragma once
#include "Command/CommandVisitor.h"
#include "GUI/SelectedItem.h"

namespace vista
{
	class EventListCommandVisitor : public CommandVisitor
	{
	public:
		EventListCommandVisitor(Bool cpuTimeline, Bool flatten, std::string const& filterText, Command const*& selectedCommand, SelectedCommandInfo& selectedCommandInfo);
		~EventListCommandVisitor();

	#define VISIT_DECL(COMMAND_TYPE) virtual void Visit(COMMAND_TYPE const& cmd) override
		VISIT_DECL(Command);
		VISIT_DECL(CreateCommandQueueCommand);
		VISIT_DECL(CreateCommandQueue1Command);
		VISIT_DECL(CreateCommandListCommand);
		VISIT_DECL(CreateCommandList1Command);
		VISIT_DECL(CreateCommandAllocatorCommand);
		VISIT_DECL(CreateFenceCommand);
		VISIT_DECL(CreateHeapCommand);
		VISIT_DECL(CreateHeap1Command);
		VISIT_DECL(CreateRootSignatureCommand);
		VISIT_DECL(CreateCommandSignatureCommand);
		VISIT_DECL(CreateGraphicsPipelineStateCommand);
		VISIT_DECL(CreateComputePipelineStateCommand);
		VISIT_DECL(CreatePipelineStateCommand);
		VISIT_DECL(CreateStateObjectCommand);
		VISIT_DECL(AddToStateObjectCommand);
		VISIT_DECL(CreateCommittedResourceCommand);
		VISIT_DECL(CreateCommittedResource1Command);
		VISIT_DECL(CreatePlacedResourceCommand);
		VISIT_DECL(CreatePlacedResource1Command);
		VISIT_DECL(CreateDescriptorHeapCommand);

		VISIT_DECL(ExecuteCommandListsCommand);
		VISIT_DECL(QueueSignalCommand);

		VISIT_DECL(CopyBufferRegionCommand);
		VISIT_DECL(CopyTextureRegionCommand);
		VISIT_DECL(CopyResourceCommand);
		VISIT_DECL(ResolveSubresourceCommand);
		VISIT_DECL(ResolveSubresourceRegionCommand);
		VISIT_DECL(DrawInstancedCommand);
		VISIT_DECL(DrawIndexedInstancedCommand);
		VISIT_DECL(DispatchCommand);
		VISIT_DECL(DispatchMeshCommand);
		VISIT_DECL(ExecuteIndirectCommand);
		VISIT_DECL(DispatchRaysCommand);
		VISIT_DECL(RSSetViewportsCommand);
		VISIT_DECL(RSSetScissorRectsCommand);
		VISIT_DECL(RSSetShadingRateCommand);
		VISIT_DECL(RSSetShadingRateImageCommand);
		VISIT_DECL(SetViewInstanceMaskCommand);
		VISIT_DECL(OMSetRenderTargetsCommand);
		VISIT_DECL(OMSetBlendFactorCommand);
		VISIT_DECL(OMSetStencilRefCommand);
		VISIT_DECL(OMSetDepthBoundsCommand);
		VISIT_DECL(BeginRenderPassCommand);
		VISIT_DECL(EndRenderPassCommand);
		VISIT_DECL(ClearRenderTargetViewCommand);
		VISIT_DECL(ClearDepthStencilViewCommand);
		VISIT_DECL(ClearUnorderedAccessViewUintCommand);
		VISIT_DECL(ClearUnorderedAccessViewFloatCommand);
		VISIT_DECL(SetPipelineStateCommand);
		VISIT_DECL(SetPipelineState1Command);
		VISIT_DECL(SetGraphicsRootSignatureCommand);
		VISIT_DECL(SetComputeRootSignatureCommand);
		VISIT_DECL(IASetPrimitiveTopologyCommand);
		VISIT_DECL(IASetVertexBuffersCommand);
		VISIT_DECL(IASetIndexBufferCommand);
		VISIT_DECL(ResourceBarrierCommand);
		VISIT_DECL(SetDescriptorHeapsCommand);
		VISIT_DECL(SetGraphicsRootDescriptorTableCommand);
		VISIT_DECL(SetComputeRootDescriptorTableCommand);
		VISIT_DECL(SetGraphicsRoot32BitConstantCommand);
		VISIT_DECL(SetComputeRoot32BitConstantCommand);
		VISIT_DECL(SetGraphicsRoot32BitConstantsCommand);
		VISIT_DECL(SetComputeRoot32BitConstantsCommand);
		VISIT_DECL(SetGraphicsRootConstantBufferViewCommand);
		VISIT_DECL(SetComputeRootConstantBufferViewCommand);
		VISIT_DECL(SetGraphicsRootShaderResourceViewCommand);
		VISIT_DECL(SetComputeRootShaderResourceViewCommand);
		VISIT_DECL(SetGraphicsRootUnorderedAccessViewCommand);
		VISIT_DECL(SetComputeRootUnorderedAccessViewCommand);
		VISIT_DECL(CloseListCommand);
		VISIT_DECL(ResetListCommand);
		VISIT_DECL(ClearStateCommand);
		VISIT_DECL(BeginEventCommand);
		VISIT_DECL(EndEventCommand);

		VISIT_DECL(ResourceSetNameCommand);

		VISIT_DECL(GetCompletedValueCommand);
		VISIT_DECL(SetEventOnCompletionCommand);
		VISIT_DECL(FenceSignalCommand);
	#undef VISIT_DECL

	private:
		Bool const cpuTimeline;
		Bool const flatten;
		std::string filterTextLower;
		Command const*& selectedCommand;
		SelectedCommandInfo  oldSelectedCommandInfo;
		SelectedCommandInfo& selectedCommandInfo;
		Int indentLevel = 0;
		std::vector<Bool> openTreeNodes;
		std::vector<Bool> eventMatchStack;

	private:
		void VisitCommon(Command const& cmd);
		void FillSelectedCommandInfo(Command const& cmd);
		Bool IsSelected(Command const& cmd) const;
		Bool MatchesFilter(std::string const& desc) const;
		Bool IsRenderingEnabled() const;
		void HandleOpenNodes();
	};
}