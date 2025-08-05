#pragma once
#include "dxc/DXIL/DxilConstants.h"

namespace vista
{
	struct CBufferOriginInfo 
	{
		std::string ResourceName;
		Uint32 ResourceBinding;
		Uint32 ByteOffset;
	};

	using ResourceHeapIndexInfo = std::variant<Uint64, CBufferOriginInfo>;

	struct BindlessResourceAccess
	{
		hlsl::DXIL::ResourceKind ResourceKind = hlsl::DXIL::ResourceKind::Invalid;
		hlsl::DXIL::ResourceClass ResourceClass = hlsl::DXIL::ResourceClass::Invalid;
		ResourceHeapIndexInfo Index;
	};

	std::vector<BindlessResourceAccess> AnalyzeBindlessAccesses(void const* shaderBytecode, Uint64 bytecodeSize);
}