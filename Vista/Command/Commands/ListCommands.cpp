#include <format>
#include "ListCommands.h"
#include "Utilities/StringUtil.h"

namespace vista
{

	std::string CopyBufferRegionCommand::GetDesc() const
	{
		return std::format("CopyBufferRegion(obj#{}, ..., obj#{}, ...) {{ this -> CommandList obj#{} }}", dstBufferId, srcBufferId, GetParentId());
	}

	std::string CopyTextureRegionCommand::GetDesc() const
	{
		return std::format("CopyTextureRegion(obj#{}, ..., obj#{}, ...) {{ this -> CommandList obj#{} }}", dstTextureId, srcTextureId, GetParentId());
	}

	std::string CopyResourceCommand::GetDesc() const
	{
		return std::format("CopyResource(obj#{}, obj#{}) {{ this -> CommandList obj#{} }}", dstResourceId, srcResourceId, GetParentId());
	}

	std::string ResolveSubresourceCommand::GetDesc() const
	{
		return std::format("ResolveSubresource(obj#{}, {}, obj#{}, {}, ...) {{ this -> CommandList obj#{} }}", dstResourceId, dstSubresource, srcResourceId, srcSubresource, GetParentId());
	}

	std::string ResolveSubresourceRegionCommand::GetDesc() const
	{
		return std::format("ResolveSubresourceRegion(obj#{}, {}, {}, {}, obj#{}, {}, ...) {{ this -> CommandList obj#{} }}", dstResourceId, dstSubresource, dstX, dstY, srcResourceId, srcSubresource, GetParentId());
	}

	std::string DrawInstancedCommand::GetDesc() const
	{
		return std::format("DrawInstanced({}, {}, {}, {}) {{ this -> CommandList obj#{} }}", vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation, GetParentId());
	}

	std::string DrawIndexedInstancedCommand::GetDesc() const
	{
		return std::format("DrawIndexedInstanced({}, {}, {}, {}, {}) {{ this -> CommandList obj#{} }}", indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation, GetParentId());
	}

	std::string DispatchCommand::GetDesc() const
	{
		return std::format("Dispatch({}, {}, {}) {{ this -> CommandList obj#{} }}", threadGroupCountX, threadGroupCountY, threadGroupCountZ, GetParentId());
	}

	std::string DispatchMeshCommand::GetDesc() const
	{
		return std::format("DispatchMesh({}, {}, {}) {{ this -> CommandList obj#{} }}", threadGroupCountX, threadGroupCountY, threadGroupCountZ, GetParentId());
	}

	std::string ExecuteIndirectCommand::GetDesc() const
	{
		if (countBufferId == InvalidObjectID)
		{
			return std::format("ExecuteIndirect(obj#{}, {}, {}, {}) {{ this -> CommandList obj#{} }}", commandSignatureId, maxCommandCount, argumentBufferId, argumentBufferOffset, GetParentId());
		}
		return std::format("ExecuteIndirect(obj#{}, {}, obj#{}, {}, obj#{}, {}) {{ this -> CommandList obj#{} }}", commandSignatureId, maxCommandCount, argumentBufferId, argumentBufferOffset,																								countBufferId, countBufferOffset, GetParentId());
	}

	std::string DispatchRaysCommand::GetDesc() const
	{
		return std::format("DispatchRays({}, {}, {}) {{ this -> CommandList obj#{} }}", dispatchWidth, dispatchHeight, dispatchDepth, GetParentId());
	}

	std::string RSSetViewportsCommand::GetDesc() const
	{
		return std::format("RSSetViewports({}, ...) {{ this -> CommandList obj#{} }}", viewports.size(), GetParentId());
	}

	std::string RSSetScissorRectsCommand::GetDesc() const
	{
		return std::format("RSSetScissorRects({}, ...) {{ this -> CommandList obj#{} }}", scissorRects.size(), GetParentId());
	}

	std::string RSSetShadingRateCommand::GetDesc() const
	{
		return std::format("RSSetShadingRate(...) {{ this -> CommandList obj#{} }}", GetParentId());
	}

	std::string RSSetShadingRateImageCommand::GetDesc() const
	{
		return std::format("RSSetShadingRateImage({}) {{ this -> CommandList obj#{} }}", shadingRateImageId, GetParentId());
	}

	std::string SetViewInstanceMaskCommand::GetDesc() const
	{
		return std::format("SetViewInstanceMask({}) {{ this -> CommandList obj#{} }}", viewInstanceMask, GetParentId());
	}

	std::string OMSetRenderTargetsCommand::GetDesc() const
	{
		return std::format("OMSetRenderTargets({}, DepthStencil: {}) {{ this -> CommandList obj#{} }}",
			renderTargetDescriptors.size(),
			depthStencilDescriptor.ptr != NULL ? "Set" : "None",
			GetParentId());
	}

	std::string OMSetBlendFactorCommand::GetDesc() const
	{
		return std::format("OMSetBlendFactor({}, {}, {}, {}) {{ this -> CommandList obj#{} }}", blendFactor[0], blendFactor[1],
							blendFactor[2], blendFactor[3], GetParentId());
	}

	std::string OMSetStencilRefCommand::GetDesc() const
	{
		return std::format("OMSetStencilRef({}) {{ this -> CommandList obj#{} }}", stencilRef, GetParentId());
	}

	std::string OMSetDepthBoundsCommand::GetDesc() const
	{
		return std::format("OMSetDepthBounds({}, {}) {{ this -> CommandList obj#{} }}", depthMin, depthMax, GetParentId());
	}

	std::string BeginRenderPassCommand::GetDesc() const
	{
		return depthStencil.has_value() ?
			 std::format("BeginRenderPass({}, ..., nullptr, ...) {{ this -> CommandList obj#{} }}", renderTargets.size(), GetParentId())
		   : std::format("BeginRenderPass({}, ..., ..., ...) {{ this -> CommandList obj#{} }}", renderTargets.size(), GetParentId());
	}

	std::string EndRenderPassCommand::GetDesc() const
	{
		return std::format("EndRenderPass() {{ this -> CommandList obj#{} }}", GetParentId());
	}

	std::string ClearRenderTargetViewCommand::GetDesc() const
	{
		return std::format("ClearRenderTargetView() {{ this -> CommandList obj#{} }}", GetParentId());
	}

	std::string ClearDepthStencilViewCommand::GetDesc() const
	{
		return std::format("ClearDepthStencilView() {{ this -> CommandList obj#{} }}", GetParentId());
	}

	std::string ClearUnorderedAccessViewUintCommand::GetDesc() const
	{
		return std::format("ClearUnorderedAccessViewUint(..., obj#{}, ...) {{ this -> CommandList obj#{} }}", resourceId, GetParentId());
	}

	std::string ClearUnorderedAccessViewFloatCommand::GetDesc() const
	{
		return std::format("ClearUnorderedAccessViewFloat(..., obj#{}, ...) {{ this -> CommandList obj#{} }}", resourceId, GetParentId());
	}

	std::string SetPipelineStateCommand::GetDesc() const
	{
		return std::format("SetPipelineState(obj#{}) {{ this -> CommandList obj#{} }}", psoId, GetParentId());
	}

	std::string SetPipelineState1Command::GetDesc() const
	{
		return std::format("SetPipelineState1(obj#{}) {{ this -> CommandList obj#{} }}", stateObjectId, GetParentId());
	}

	std::string SetGraphicsRootSignatureCommand::GetDesc() const
	{
		return std::format("SetGraphicsRootSignature(obj#{}) {{ this -> CommandList obj#{} }}", rootSignatureId, GetParentId());
	}

	std::string SetComputeRootSignatureCommand::GetDesc() const
	{
		return std::format("SetComputeRootSignature(obj#{}) {{ this -> CommandList obj#{} }}", rootSignatureId, GetParentId());
	}

	std::string IASetPrimitiveTopologyCommand::GetDesc() const
	{
		return std::format("IASetPrimitiveTopology() {{ this -> CommandList obj#{} }}", GetParentId());
	}

	std::string IASetVertexBuffersCommand::GetDesc() const
	{
		return std::format("IASetVertexBuffers({}, ...) {{ this -> CommandList obj#{} }}", vertexBufferViews.size(), GetParentId());
	}

	std::string IASetIndexBufferCommand::GetDesc() const
	{
		return std::format("IASetIndexBuffer(...) {{ this -> CommandList obj#{} }}", GetParentId());
	}

	std::string ResourceBarrierCommand::GetDesc() const
	{
		return std::format("ResourceBarrier({}) {{ this -> CommandList obj#{} }}", barriers.size(), GetParentId());
	}

	std::string SetDescriptorHeapsCommand::GetDesc() const
	{
		return std::format("SetDescriptorHeaps(NumDescriptorHeaps:{}, ...) {{ this -> CommandList obj#{}}}", numDescriptorHeaps,  GetParentId());
	}

	std::string SetGraphicsRootDescriptorTableCommand::GetDesc() const
	{
		return std::format("SetGraphicsRootDescriptorTable({}, ...) {{ this -> CommandList obj#{}}}", rootParameterIndex, GetParentId());
	}

	std::string SetComputeRootDescriptorTableCommand::GetDesc() const
	{
		return std::format("SetComputeRootDescriptorTable({}, ...) {{ this -> CommandList obj#{}}}", rootParameterIndex, GetParentId());
	}

	std::string SetGraphicsRoot32BitConstantCommand::GetDesc() const
	{
		return std::format("SetGraphicsRoot32BitConstant({}, {}, {}) {{ this -> CommandList obj#{} }}", rootParameterIndex, srcData, destOffsetIn32BitValues, GetParentId());
	}

	std::string SetComputeRoot32BitConstantCommand::GetDesc() const
	{
		return std::format("SetComputeRoot32BitConstant({}, {}, {}) {{ this -> CommandList obj#{} }}", rootParameterIndex, srcData, destOffsetIn32BitValues, GetParentId());
	}

	std::string SetGraphicsRoot32BitConstantsCommand::GetDesc() const
	{
		return std::format("SetGraphicsRoot32BitConstants({}, {}, ..., {}) {{ this -> CommandList obj#{} }}", rootParameterIndex, srcData.size(), destOffsetIn32BitValues, GetParentId());
	}

	std::string SetComputeRoot32BitConstantsCommand::GetDesc() const
	{
		return std::format("SetComputeRoot32BitConstants({}, {}, ..., {}) {{ this -> CommandList obj#{} }}", rootParameterIndex, srcData.size(), destOffsetIn32BitValues, GetParentId());
	}

	std::string SetGraphicsRootConstantBufferViewCommand::GetDesc() const
	{
		return std::format("SetGraphicsRootConstantBufferView({}, ...) {{ this -> CommandList obj#{} }}", rootParameterIndex, GetParentId());
	}

	std::string SetComputeRootConstantBufferViewCommand::GetDesc() const
	{
		return std::format("SetComputeRootConstantBufferView({}, ...) {{ this -> CommandList obj#{} }}", rootParameterIndex, GetParentId());
	}

	std::string SetGraphicsRootShaderResourceViewCommand::GetDesc() const
	{
		return std::format("SetGraphicsRootShaderResourceView({}, ...) {{ this -> CommandList obj#{} }}", rootParameterIndex, GetParentId());
	}

	std::string SetComputeRootShaderResourceViewCommand::GetDesc() const
	{
		return std::format("SetComputeRootShaderResourceView({}, ...) {{ this -> CommandList obj#{} }}", rootParameterIndex, GetParentId());
	}

	std::string SetGraphicsRootUnorderedAccessViewCommand::GetDesc() const
	{
		return std::format("SetGraphicsRootUnorderedAccessView({}, ...) {{ this -> CommandList obj#{} }}", rootParameterIndex, GetParentId());
	}

	std::string SetComputeRootUnorderedAccessViewCommand::GetDesc() const
	{
		return std::format("SetComputeRootUnorderedAccessView({}, ...) {{ this -> CommandList obj#{} }}", rootParameterIndex, GetParentId());
	}

	std::string CloseListCommand::GetDesc() const
	{
		return std::format("Close() {{ this -> CommandList obj#{}, return -> {} }}", GetParentId(), HResultToString(hr));
	}

	std::string ResetListCommand::GetDesc() const
	{
		if (psoId != InvalidObjectID)
		{
			return std::format("Reset(obj#{}, obj#{}) {{ this -> CommandList obj#{}, return -> {} }}", allocatorId, psoId, GetParentId(), HResultToString(hr));
		}
		return std::format("Reset(obj#{}, nullptr) {{ this -> CommandList obj#{}, return -> {} }}", allocatorId, psoId, GetParentId(), HResultToString(hr));
	}

	std::string ClearStateCommand::GetDesc() const
	{
		if (psoId != InvalidObjectID)
		{
			return std::format("Reset(obj#{}) {{ this -> CommandList obj#{} }}",psoId, GetParentId());
		}
		return std::format("Reset(nullptr) {{ this -> CommandList obj#{} }}", psoId, GetParentId());
	}


}
