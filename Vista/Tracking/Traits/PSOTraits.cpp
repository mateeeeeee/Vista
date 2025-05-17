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

}
