#include "StateUpdateVisitor.h"
#include "Command/Commands.h"
#include "Tracking/State.h"
#include "Tracking/ObjectTracker.h"

namespace vista
{
	void StateUpdateVisitor::Visit(SetGraphicsRootSignatureCommand const& cmd)
	{
		state.graphicsRootSignatureId = cmd.rootSignatureId; 

		TrackedObjectInfo const* rsInfo = objectTracker.GetObjectInfo(cmd.rootSignatureId); 
		if (rsInfo && rsInfo->objectType == ObjectType::RootSignature && std::holds_alternative<RootSignatureDesc>(rsInfo->objectDesc))
		{
			RootSignatureDesc const& rsDesc = std::get<RootSignatureDesc>(rsInfo->objectDesc);
			state.graphicsRootArguments.resize(rsDesc.Parameters.size());
			for (Uint32 i = 0; i < rsDesc.Parameters.size(); ++i)
			{
				state.graphicsRootArguments[i].Reset();
				state.graphicsRootArguments[i].type = rsDesc.Parameters[i].ParameterType;
				state.graphicsRootArguments[i].shaderVisibility = rsDesc.Parameters[i].ShaderVisibility;
				switch (state.graphicsRootArguments[i].type)
				{
				case D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS:
					state.graphicsRootArguments[i].value = std::array<Uint32, D3D12_MAX_ROOT_COST / sizeof(Uint32)>{};
					break;
				case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
					state.graphicsRootArguments[i].value = D3D12_GPU_DESCRIPTOR_HANDLE{};
					break;
				case D3D12_ROOT_PARAMETER_TYPE_CBV:
				case D3D12_ROOT_PARAMETER_TYPE_SRV:
				case D3D12_ROOT_PARAMETER_TYPE_UAV:
					state.graphicsRootArguments[i].value = D3D12_GPU_VIRTUAL_ADDRESS{};
					break;
				default:
					state.graphicsRootArguments[i].value = std::monostate{};
					break;
				}
			}
		}
		else
		{
			state.graphicsRootArguments.clear();
		}
	}

	void StateUpdateVisitor::Visit(SetComputeRootSignatureCommand const& cmd)
	{
		state.computeRootSignatureId = cmd.rootSignatureId;

		TrackedObjectInfo const* rsInfo = objectTracker.GetObjectInfo(cmd.rootSignatureId);
		if (rsInfo && rsInfo->objectType == ObjectType::RootSignature && std::holds_alternative<RootSignatureDesc>(rsInfo->objectDesc))
		{
			RootSignatureDesc const& rsDesc = std::get<RootSignatureDesc>(rsInfo->objectDesc);
			state.computeRootArguments.resize(rsDesc.Parameters.size());
			for (Uint32 i = 0; i < rsDesc.Parameters.size(); ++i)
			{
				state.computeRootArguments[i].Reset();
				state.computeRootArguments[i].type = rsDesc.Parameters[i].ParameterType;
				state.computeRootArguments[i].shaderVisibility = rsDesc.Parameters[i].ShaderVisibility;
				switch (state.computeRootArguments[i].type)
				{
				case D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS:
					state.computeRootArguments[i].value = std::array<Uint32, D3D12_MAX_ROOT_COST / sizeof(Uint32)>{};
					break;
				case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
					state.computeRootArguments[i].value = D3D12_GPU_DESCRIPTOR_HANDLE{};
					break;
				case D3D12_ROOT_PARAMETER_TYPE_CBV:
				case D3D12_ROOT_PARAMETER_TYPE_SRV:
				case D3D12_ROOT_PARAMETER_TYPE_UAV:
					state.computeRootArguments[i].value = D3D12_GPU_VIRTUAL_ADDRESS{};
					break;
				default:
					state.computeRootArguments[i].value = std::monostate{};
					break;
				}
			}
		}
		else
		{
			state.computeRootArguments.clear();
		}
	}

	void StateUpdateVisitor::Visit(SetPipelineStateCommand const& cmd)
	{
		state.pipelineStateId = cmd.psoId;
	}

	void StateUpdateVisitor::Visit(SetPipelineState1Command const& cmd)
	{
		state.pipelineStateId = cmd.stateObjectId;
	}

	void StateUpdateVisitor::Visit(IASetPrimitiveTopologyCommand const& cmd)
	{
		state.primitiveTopology = cmd.topology; 
	}

	void StateUpdateVisitor::Visit(IASetVertexBuffersCommand const& cmd)
	{
		state.vertexBufferStartSlot = cmd.startSlot;
		state.vertexBufferViews = cmd.vertexBufferViews;
	}

	void StateUpdateVisitor::Visit(IASetIndexBufferCommand const& cmd)
	{
		state.indexBufferView = cmd.indexBufferView;
	}

	void StateUpdateVisitor::Visit(RSSetViewportsCommand const& cmd)
	{
		state.viewports = cmd.viewports; 
	}

	void StateUpdateVisitor::Visit(RSSetScissorRectsCommand const& cmd)
	{
		state.scissorRects = cmd.scissorRects;  
	}

	void StateUpdateVisitor::Visit(RSSetShadingRateCommand const& cmd)
	{
		state.shadingRate = cmd.shadingRate;
		state.shadingRateCombiners[0] = cmd.shadingRateCombiners[0];
		state.shadingRateCombiners[1] = cmd.shadingRateCombiners[1];
	}

	void StateUpdateVisitor::Visit(RSSetShadingRateImageCommand const& cmd)
	{
		state.shadingRateImageId = cmd.shadingRateImageId;
	}

	void StateUpdateVisitor::Visit(OMSetRenderTargetsCommand const& cmd)
	{
		state.numRenderTargetsSet = (Uint32)cmd.renderTargetDescriptors.size();
		std::copy(cmd.renderTargetDescriptors.begin(), cmd.renderTargetDescriptors.begin() + state.numRenderTargetsSet, state.renderTargetViews.begin());
		std::fill(state.renderTargetViews.begin() + state.numRenderTargetsSet, state.renderTargetViews.end(), D3D12_CPU_DESCRIPTOR_HANDLE{});

		D3D12_CPU_DESCRIPTOR_HANDLE dsv = cmd.depthStencilDescriptor;
		if (dsv.ptr != NULL) 
		{ 
			state.depthStencilView = dsv;
		}
		else 
		{
			state.depthStencilView.reset();
		}
	}

	void StateUpdateVisitor::Visit(OMSetBlendFactorCommand const& cmd)
	{
		state.blendFactor = cmd.blendFactor;
	}

	void StateUpdateVisitor::Visit(OMSetStencilRefCommand const& cmd)
	{
		state.stencilRef = cmd.stencilRef;
	}

	void StateUpdateVisitor::Visit(OMSetDepthBoundsCommand const& cmd)
	{
		state.depthMin = cmd.depthMin;
		state.depthMax = cmd.depthMax;
	}

	void StateUpdateVisitor::Visit(BeginRenderPassCommand const& cmd)
	{
		state.numRenderTargetsSet = (Uint32)cmd.renderTargets.size();
		for (Uint32 i = 0; i < cmd.renderTargets.size() && i < state.renderTargetViews.size(); ++i)
		{
			state.renderTargetViews[i] = cmd.renderTargets[i].cpuDescriptor;
		}
		for (Uint32 i = (Uint32)cmd.renderTargets.size(); i < state.renderTargetViews.size(); ++i)
		{
			state.renderTargetViews[i] = D3D12_CPU_DESCRIPTOR_HANDLE{};
		}

		if (cmd.depthStencil.has_value())
		{
			state.depthStencilView = cmd.depthStencil->cpuDescriptor;
		}
		else
		{
			state.depthStencilView.reset();
		}

		state.renderPassFlags = cmd.flags;
	}

	void StateUpdateVisitor::Visit(EndRenderPassCommand const&)
	{
		state.numRenderTargetsSet = 0;
		std::fill(state.renderTargetViews.begin(), state.renderTargetViews.end(), D3D12_CPU_DESCRIPTOR_HANDLE{});
		state.depthStencilView.reset();
		state.renderPassFlags = D3D12_RENDER_PASS_FLAGS::D3D12_RENDER_PASS_FLAG_NONE;
	}

	void StateUpdateVisitor::Visit(SetDescriptorHeapsCommand const& cmd)
	{
		state.descriptorHeapIds[0] = cmd.descriptorHeapIds[0];
		state.descriptorHeapIds[1] = cmd.descriptorHeapIds[1];
	}

	void StateUpdateVisitor::Visit(SetGraphicsRootDescriptorTableCommand const& cmd)
	{
		if (cmd.rootParameterIndex < state.graphicsRootArguments.size())
		{
			state.graphicsRootArguments[cmd.rootParameterIndex].isSet = true;
			state.graphicsRootArguments[cmd.rootParameterIndex].value = cmd.baseDescriptor;
		}
	}

	void StateUpdateVisitor::Visit(SetComputeRootDescriptorTableCommand const& cmd)
	{
		if (cmd.rootParameterIndex < state.computeRootArguments.size())
		{
			state.computeRootArguments[cmd.rootParameterIndex].isSet = true;
			state.computeRootArguments[cmd.rootParameterIndex].value = cmd.baseDescriptor;
		}
	}

	void StateUpdateVisitor::Visit(SetGraphicsRoot32BitConstantCommand const& cmd)
	{
		if (cmd.rootParameterIndex < state.graphicsRootArguments.size())
		{
			state.graphicsRootArguments[cmd.rootParameterIndex].isSet = true;
			RootParameterBinding& binding = state.graphicsRootArguments[cmd.rootParameterIndex];
			if (std::holds_alternative<RootParameterBinding::RootConstants>(binding.value))
			{
				auto& values = std::get<RootParameterBinding::RootConstants>(binding.value);
				if (cmd.destOffsetIn32BitValues < values.size())
				{
					values[cmd.destOffsetIn32BitValues] = cmd.srcData;
				}
			}
			else
			{
				RootParameterBinding::RootConstants values = {};
				if (cmd.destOffsetIn32BitValues < values.size())
				{
					values[cmd.destOffsetIn32BitValues] = cmd.srcData;
				}
				binding.value = values;
			}
		}
	}

	void StateUpdateVisitor::Visit(SetComputeRoot32BitConstantCommand const& cmd)
	{
		if (cmd.rootParameterIndex < state.computeRootArguments.size())
		{
			state.computeRootArguments[cmd.rootParameterIndex].isSet = true;
			RootParameterBinding& binding = state.computeRootArguments[cmd.rootParameterIndex];
			if (std::holds_alternative<RootParameterBinding::RootConstants>(binding.value))
			{
				auto& values = std::get<RootParameterBinding::RootConstants>(binding.value);
				if (cmd.destOffsetIn32BitValues < values.size())
				{
					values[cmd.destOffsetIn32BitValues] = cmd.srcData;
				}
			}
			else
			{
				RootParameterBinding::RootConstants values = {};
				if (cmd.destOffsetIn32BitValues < values.size())
				{
					values[cmd.destOffsetIn32BitValues] = cmd.srcData;
				}
				binding.value = values;
			}
		}
	}

	void StateUpdateVisitor::Visit(SetGraphicsRoot32BitConstantsCommand const& cmd)
	{
		if (cmd.rootParameterIndex < state.graphicsRootArguments.size())
		{
			state.graphicsRootArguments[cmd.rootParameterIndex].isSet = true;
			RootParameterBinding& binding = state.graphicsRootArguments[cmd.rootParameterIndex];
			if (std::holds_alternative<RootParameterBinding::RootConstants>(binding.value))
			{
				auto& values = std::get<RootParameterBinding::RootConstants>(binding.value);
				for (Uint32 i = 0; i < cmd.srcData.size() && (cmd.destOffsetIn32BitValues + i) < values.size(); ++i)
				{
					values[cmd.destOffsetIn32BitValues + i] = cmd.srcData[i];
				}
			}
			else
			{
				RootParameterBinding::RootConstants values = {};
				for (Uint32 i = 0; i < cmd.srcData.size() && (cmd.destOffsetIn32BitValues + i) < values.size(); ++i)
				{
					values[cmd.destOffsetIn32BitValues + i] = cmd.srcData[i];
				}
				binding.value = values;
			}
		}
	}

	void StateUpdateVisitor::Visit(SetComputeRoot32BitConstantsCommand const& cmd)
	{
		if (cmd.rootParameterIndex < state.computeRootArguments.size())
		{
			state.computeRootArguments[cmd.rootParameterIndex].isSet = true;
			RootParameterBinding& binding = state.computeRootArguments[cmd.rootParameterIndex];
			if (std::holds_alternative<RootParameterBinding::RootConstants>(binding.value))
			{
				auto& values = std::get<RootParameterBinding::RootConstants>(binding.value);
				for (Uint32 i = 0; i < cmd.srcData.size() && (cmd.destOffsetIn32BitValues + i) < values.size(); ++i)
				{
					values[cmd.destOffsetIn32BitValues + i] = cmd.srcData[i];
				}
			}
			else
			{
				RootParameterBinding::RootConstants values = {};
				for (Uint32 i = 0; i < cmd.srcData.size() && (cmd.destOffsetIn32BitValues + i) < values.size(); ++i)
				{
					values[cmd.destOffsetIn32BitValues + i] = cmd.srcData[i];
				}
				binding.value = values;
			}
		}
	}

	void StateUpdateVisitor::Visit(SetGraphicsRootConstantBufferViewCommand const& cmd)
	{
		if (cmd.rootParameterIndex < state.graphicsRootArguments.size())
		{
			state.graphicsRootArguments[cmd.rootParameterIndex].isSet = true;
			state.graphicsRootArguments[cmd.rootParameterIndex].value = cmd.gpuVA;
		}
	}

	void StateUpdateVisitor::Visit(SetComputeRootConstantBufferViewCommand const& cmd)
	{
		if (cmd.rootParameterIndex < state.computeRootArguments.size())
		{
			state.computeRootArguments[cmd.rootParameterIndex].isSet = true;
			state.computeRootArguments[cmd.rootParameterIndex].value = cmd.gpuVA;
		}
	}

	void StateUpdateVisitor::Visit(SetGraphicsRootShaderResourceViewCommand const& cmd)
	{
		if (cmd.rootParameterIndex < state.graphicsRootArguments.size())
		{
			state.graphicsRootArguments[cmd.rootParameterIndex].isSet = true;
			state.graphicsRootArguments[cmd.rootParameterIndex].value = cmd.gpuVA;
		}
	}

	void StateUpdateVisitor::Visit(SetComputeRootShaderResourceViewCommand const& cmd)
	{
		if (cmd.rootParameterIndex < state.computeRootArguments.size())
		{
			state.computeRootArguments[cmd.rootParameterIndex].isSet = true;
			state.computeRootArguments[cmd.rootParameterIndex].value = cmd.gpuVA;
		}
	}

	void StateUpdateVisitor::Visit(SetGraphicsRootUnorderedAccessViewCommand const& cmd)
	{
		if (cmd.rootParameterIndex < state.graphicsRootArguments.size())
		{
			state.graphicsRootArguments[cmd.rootParameterIndex].isSet = true;
			state.graphicsRootArguments[cmd.rootParameterIndex].value = cmd.gpuVA;
		}
	}

	void StateUpdateVisitor::Visit(SetComputeRootUnorderedAccessViewCommand const& cmd)
	{
		if (cmd.rootParameterIndex < state.computeRootArguments.size())
		{
			state.computeRootArguments[cmd.rootParameterIndex].isSet = true;
			state.computeRootArguments[cmd.rootParameterIndex].value = cmd.gpuVA;
		}
	}

	void StateUpdateVisitor::Visit(ResetListCommand const& cmd)
	{
		state.Reset();
	}

	void StateUpdateVisitor::Visit(ClearStateCommand const& cmd)
	{
		state.Reset();
	}

}

