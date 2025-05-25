#pragma once
#include "Command/CommandVisitor.h"

namespace vista
{
	struct State;
	class ObjectTracker;
	class StateUpdateVisitor : public DummyCommandVisitor
	{
	public:
		explicit StateUpdateVisitor(State& state, ObjectTracker const& objectTracker) : state(state), objectTracker(objectTracker) {}

		virtual void Visit(SetGraphicsRootSignatureCommand const& cmd) override;
		virtual void Visit(SetComputeRootSignatureCommand const& cmd) override;
		virtual void Visit(SetPipelineStateCommand const& cmd) override;
		virtual void Visit(SetPipelineState1Command const& cmd) override;
		virtual void Visit(IASetPrimitiveTopologyCommand const& cmd) override;
		virtual void Visit(IASetVertexBuffersCommand const& cmd) override;
		virtual void Visit(IASetIndexBufferCommand const& cmd) override;
		virtual void Visit(RSSetViewportsCommand const& cmd) override;
		virtual void Visit(RSSetScissorRectsCommand const& cmd) override;
		virtual void Visit(RSSetShadingRateCommand const& cmd) override;
		virtual void Visit(RSSetShadingRateImageCommand const& cmd) override;
		virtual void Visit(OMSetRenderTargetsCommand const& cmd) override;
		virtual void Visit(OMSetBlendFactorCommand const& cmd) override;
		virtual void Visit(OMSetStencilRefCommand const& cmd) override;
		virtual void Visit(OMSetDepthBoundsCommand const& cmd) override;
		virtual void Visit(BeginRenderPassCommand const&) override;
		virtual void Visit(EndRenderPassCommand const&) override;
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
		virtual void Visit(ResetListCommand const& cmd) override;
		virtual void Visit(ClearStateCommand const& cmd) override;

	private:
		State& state;
		ObjectTracker const& objectTracker;
	};
}