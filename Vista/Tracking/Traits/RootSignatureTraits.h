#pragma once
#include "Tracking/ObjectTraits.h"

namespace vista
{
	struct DescriptorRange
	{
		D3D12_DESCRIPTOR_RANGE_TYPE RangeType;
		UINT NumDescriptors;
		UINT BaseShaderRegister;
		UINT RegisterSpace;
		UINT OffsetInDescriptorsFromTableStart;

		DescriptorRange(D3D12_DESCRIPTOR_RANGE const& range)
			: RangeType(range.RangeType),
			NumDescriptors(range.NumDescriptors),
			BaseShaderRegister(range.BaseShaderRegister),
			RegisterSpace(range.RegisterSpace),
			OffsetInDescriptorsFromTableStart(range.OffsetInDescriptorsFromTableStart)
		{
		}
	};

	struct RootDescriptorTable
	{
		std::vector<DescriptorRange> Ranges;

		RootDescriptorTable(D3D12_ROOT_DESCRIPTOR_TABLE const& table)
		{
			for (Uint32 i = 0; i < table.NumDescriptorRanges; ++i)
			{
				Ranges.emplace_back(table.pDescriptorRanges[i]);
			}
		}
	};

	struct RootParameter
	{

		RootParameter(D3D12_ROOT_PARAMETER const& param)
			: ParameterType(param.ParameterType),
			ShaderVisibility(param.ShaderVisibility),
			DescriptorTable(param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE
				? RootDescriptorTable(param.DescriptorTable)
				: RootDescriptorTable({ 0, nullptr })), Constants{}, Descriptor{}
		{
			if (ParameterType == D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS)
			{
				Constants = param.Constants;
			}
			else if (ParameterType == D3D12_ROOT_PARAMETER_TYPE_CBV ||
				ParameterType == D3D12_ROOT_PARAMETER_TYPE_SRV ||
				ParameterType == D3D12_ROOT_PARAMETER_TYPE_UAV)
			{
				Descriptor = param.Descriptor;
			}
		}

		D3D12_ROOT_PARAMETER_TYPE ParameterType;
		D3D12_SHADER_VISIBILITY ShaderVisibility;
		RootDescriptorTable DescriptorTable;
		D3D12_ROOT_CONSTANTS Constants;
		D3D12_ROOT_DESCRIPTOR Descriptor;
	};

	struct StaticSamplerDesc
	{
		StaticSamplerDesc(D3D12_STATIC_SAMPLER_DESC const& desc) : Desc(desc) {}

		D3D12_STATIC_SAMPLER_DESC Desc;
	};

	struct RootSignatureDesc
	{
		RootSignatureDesc(D3D12_ROOT_SIGNATURE_DESC const& desc) : Flags(desc.Flags)
		{
			for (Uint32 i = 0; i < desc.NumParameters; ++i)
			{
				Parameters.emplace_back(desc.pParameters[i]);
			}
			for (Uint32 i = 0; i < desc.NumStaticSamplers; ++i)
			{
				StaticSamplers.emplace_back(desc.pStaticSamplers[i]);
			}
		}

		std::vector<RootParameter> Parameters;
		std::vector<StaticSamplerDesc> StaticSamplers;
		D3D12_ROOT_SIGNATURE_FLAGS Flags;
	};
	template<>
	struct ObjectTraits<ID3D12RootSignature>
	{
		using Desc = RootSignatureDesc;
		static constexpr ObjectType Type = ObjectType::RootSignature;
	};
}