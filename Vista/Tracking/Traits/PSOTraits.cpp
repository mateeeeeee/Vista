#include "PSOTraits.h"
#include "Tracking/ObjectTracker.h"

namespace vista
{
	GraphicsPSODescStorage::GraphicsPSODescStorage(D3D12_GRAPHICS_PIPELINE_STATE_DESC const& desc, ObjectTracker& objManager) :
		RootSignatureId(desc.pRootSignature ? objManager.GetObjectID(desc.pRootSignature) : InvalidObjectID),
		VS(desc.VS),
		PS(desc.PS),
		DS(desc.DS),
		HS(desc.HS),
		GS(desc.GS),
		StreamOutput(desc.StreamOutput),
		BlendState(desc.BlendState),
		SampleMask(desc.SampleMask),
		RasterizerState(desc.RasterizerState),
		DepthStencilState(desc.DepthStencilState),
		IBStripCutValue(desc.IBStripCutValue),
		PrimitiveTopologyType(desc.PrimitiveTopologyType),
		NumRenderTargets(desc.NumRenderTargets),
		DSVFormat(desc.DSVFormat),
		SampleDesc(desc.SampleDesc),
		NodeMask(desc.NodeMask),
		CachedPSO(desc.CachedPSO),
		Flags(desc.Flags)
	{
		if (desc.InputLayout.pInputElementDescs && desc.InputLayout.NumElements > 0)
		{
			InputLayoutElements.reserve(desc.InputLayout.NumElements);
			for (Uint32 i = 0; i < desc.InputLayout.NumElements; ++i)
			{
				InputLayoutElements.emplace_back(desc.InputLayout.pInputElementDescs[i]);
			}
		}
		for (Uint32 i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		{
			RTVFormats[i] = desc.RTVFormats[i];
		}
	}

	ComputePSODescStorage::ComputePSODescStorage(D3D12_COMPUTE_PIPELINE_STATE_DESC const& desc, ObjectTracker& objManager) :
		RootSignatureId(desc.pRootSignature ? objManager.GetObjectID(desc.pRootSignature) : InvalidObjectID),
		CS(desc.CS),
		NodeMask(desc.NodeMask),
		CachedPSO(desc.CachedPSO),
		Flags(desc.Flags)
	{
	}

	StreamPSODescStorage::StreamPSODescStorage(D3D12_PIPELINE_STATE_STREAM_DESC const& desc, ObjectTracker& tracker)
	{
		if (desc.pPipelineStateSubobjectStream && desc.SizeInBytes > 0)
		{
			StreamBlob.assign(
				static_cast<BYTE const*>(desc.pPipelineStateSubobjectStream),
				static_cast<BYTE const*>(desc.pPipelineStateSubobjectStream) + desc.SizeInBytes
			);
			ParseStream(tracker);
		}
	}

	void StreamPSODescStorage::ParseStream(ObjectTracker& tracker)
	{
		const BYTE* ptr = StreamBlob.data();
		const BYTE* end = ptr + StreamBlob.size();
		while (ptr < end)
		{
			auto type = *reinterpret_cast<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE const*>(ptr);
			ptr += sizeof(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE);
			switch (type)
			{
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE:
				RootSignatureId = tracker.GetObjectID(*reinterpret_cast<ID3D12RootSignature* const*>(ptr));
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS:
				VS = ShaderBytecodeStorage(*reinterpret_cast<D3D12_SHADER_BYTECODE const*>(ptr));
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS:
				PS = ShaderBytecodeStorage(*reinterpret_cast<D3D12_SHADER_BYTECODE const*>(ptr));
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CS:
				CS = ShaderBytecodeStorage(*reinterpret_cast<D3D12_SHADER_BYTECODE const*>(ptr));
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DS:
				DS = ShaderBytecodeStorage(*reinterpret_cast<D3D12_SHADER_BYTECODE const*>(ptr));
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_HS:
				HS = ShaderBytecodeStorage(*reinterpret_cast<D3D12_SHADER_BYTECODE const*>(ptr));
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_GS:
				GS = ShaderBytecodeStorage(*reinterpret_cast<D3D12_SHADER_BYTECODE const*>(ptr));
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_AS:
				AS = ShaderBytecodeStorage(*reinterpret_cast<D3D12_SHADER_BYTECODE const*>(ptr));
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_MS:
				MS = ShaderBytecodeStorage(*reinterpret_cast<D3D12_SHADER_BYTECODE const*>(ptr));
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND:
				BlendState = *reinterpret_cast<D3D12_BLEND_DESC const*>(ptr);
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER:
				RasterizerState = *reinterpret_cast<D3D12_RASTERIZER_DESC const*>(ptr);
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL:
				DepthStencilState = *reinterpret_cast<D3D12_DEPTH_STENCIL_DESC const*>(ptr);
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_INPUT_LAYOUT:
			{
				auto const& layout = *reinterpret_cast<D3D12_INPUT_LAYOUT_DESC const*>(ptr);
				InputLayoutElements.clear();
				for (UINT i = 0; i < layout.NumElements; ++i)
					InputLayoutElements.emplace_back(layout.pInputElementDescs[i]);
				break;
			}
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY:
				PrimitiveTopologyType = *reinterpret_cast<D3D12_PRIMITIVE_TOPOLOGY_TYPE const*>(ptr);
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS:
			{
				auto const& formats = *reinterpret_cast<D3D12_RT_FORMAT_ARRAY const*>(ptr);
				NumRenderTargets = formats.NumRenderTargets;
				std::copy(std::begin(formats.RTFormats), std::end(formats.RTFormats), RTVFormats);
				break;
			}
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT:
				DSVFormat = *reinterpret_cast<DXGI_FORMAT const*>(ptr);
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC:
				SampleDesc = *reinterpret_cast<DXGI_SAMPLE_DESC const*>(ptr);
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_NODE_MASK:
				NodeMask = *reinterpret_cast<UINT const*>(ptr);
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CACHED_PSO:
				CachedPSO = CachedPSOStorage(*reinterpret_cast<D3D12_CACHED_PIPELINE_STATE const*>(ptr));
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_FLAGS:
				Flags = *reinterpret_cast<D3D12_PIPELINE_STATE_FLAGS const*>(ptr);
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_MASK:
				SampleMask = *reinterpret_cast<UINT const*>(ptr);
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_STREAM_OUTPUT:
				StreamOutput = StreamOutputDescStorage(*reinterpret_cast<D3D12_STREAM_OUTPUT_DESC const*>(ptr));
				break;
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_IB_STRIP_CUT_VALUE:
				IBStripCutValue = *reinterpret_cast<D3D12_INDEX_BUFFER_STRIP_CUT_VALUE const*>(ptr);
				break;
			}
			ptr += GetSubobjectSize(type);
		}
	}

}
