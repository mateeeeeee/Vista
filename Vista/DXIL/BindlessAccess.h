#pragma once
#include "dxc/DXIL/DxilConstants.h"

namespace vista
{
	enum class IndexSourceType : Uint8
	{
		Unknown,       
		ImmediateConstant,
		ConstantBuffer,
	};

	struct CBufferSourceInfo 
	{
		std::string ResourceName;
		Uint32 ResourceBinding;
		Uint32 ResourceSpace;
		Uint32 ByteOffset;
	};

	using IndexSourceDetails = std::variant<std::monostate, Uint64, CBufferSourceInfo>;
	struct IndexSource
	{
		IndexSourceType type = IndexSourceType::Unknown;
		IndexSourceDetails details;
	};

	struct BindlessAccess
	{
		hlsl::DXIL::ResourceKind ResourceKind = hlsl::DXIL::ResourceKind::Invalid;
		hlsl::DXIL::ResourceClass ResourceClass = hlsl::DXIL::ResourceClass::Invalid;
		IndexSource Index;
	};
	
	std::vector<BindlessAccess> FindBindlessResourceAccesses(void const* shaderBytecode, Uint64 bytecodeSize);
}