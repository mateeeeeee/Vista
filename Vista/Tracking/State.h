#pragma once
#include "ObjectID.h"

namespace vista
{
	
	struct RootParameterBinding
	{
		using RootConstants = std::array<Uint32, D3D12_MAX_ROOT_COST / sizeof(Uint32)>; 
		using RootParameterValue = std::variant<std::monostate,
			D3D12_GPU_DESCRIPTOR_HANDLE,
			RootConstants,
			D3D12_GPU_VIRTUAL_ADDRESS>;

		void Reset()
		{
			type = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			shaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
			isSet = false;
			value = std::monostate{};
		}

		D3D12_ROOT_PARAMETER_TYPE type = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		RootParameterValue value = std::monostate{};
		Bool isSet = false;
	};

	struct State
	{
		State()
		{
			descriptorHeapIds.fill(InvalidObjectID);
			renderTargetViews.fill(D3D12_CPU_DESCRIPTOR_HANDLE{}); 
			graphicsRootArguments.clear();
			computeRootArguments.clear();
		}

		void Reset() 
		{
			graphicsRootSignatureId = InvalidObjectID;
			computeRootSignatureId = InvalidObjectID;
			pipelineStateId = InvalidObjectID;
			descriptorHeapIds.fill(InvalidObjectID);
			primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
			viewports.clear();
			scissorRects.clear();
			numRenderTargetsSet = 0;
			renderTargetViews.fill(D3D12_CPU_DESCRIPTOR_HANDLE{});
			depthStencilView.reset();
			blendFactor.reset();
			stencilRef.reset();
			shadingRate.reset();
			shadingRateCombiners[0] = D3D12_SHADING_RATE_COMBINER_PASSTHROUGH;
			shadingRateCombiners[1] = D3D12_SHADING_RATE_COMBINER_PASSTHROUGH;
			graphicsRootArguments.clear();
			computeRootArguments.clear();
			depthMin = 0.0f;
			depthMax = 1.0f;
		}

		ObjectID graphicsRootSignatureId = InvalidObjectID;
		std::vector<RootParameterBinding> graphicsRootArguments;
		ObjectID computeRootSignatureId = InvalidObjectID;
		std::vector<RootParameterBinding> computeRootArguments;

		ObjectID pipelineStateId = InvalidObjectID;
		std::array<ObjectID, 2> descriptorHeapIds;

		D3D12_PRIMITIVE_TOPOLOGY primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		std::vector<D3D12_VERTEX_BUFFER_VIEW> vertexBufferViews;
		Uint32 vertexBufferStartSlot = 0;
		std::optional<D3D12_INDEX_BUFFER_VIEW> indexBufferView = std::nullopt;

		std::vector<D3D12_VIEWPORT> viewports;
		std::vector<D3D12_RECT> scissorRects;

		Uint32 numRenderTargetsSet = 0;
		std::array<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> renderTargetViews{};
		std::optional<D3D12_CPU_DESCRIPTOR_HANDLE> depthStencilView = std::nullopt;
		D3D12_RENDER_PASS_FLAGS renderPassFlags = D3D12_RENDER_PASS_FLAG_NONE;
		std::optional<std::array<Float, 4>> blendFactor = std::nullopt;
		std::optional<Uint32> stencilRef = std::nullopt;
		Float depthMin = 0.0f;
		Float depthMax = 1.0f;

		std::optional<D3D12_SHADING_RATE> shadingRate = std::nullopt;
		D3D12_SHADING_RATE_COMBINER shadingRateCombiners[2] = { D3D12_SHADING_RATE_COMBINER_PASSTHROUGH, D3D12_SHADING_RATE_COMBINER_PASSTHROUGH };
		ObjectID shadingRateImageId = InvalidObjectID;
	};
}