#pragma once
#include "Tracking/ObjectTraits.h"

namespace vista
{
	struct FenceDesc
	{
		UINT64 intialValue;
		D3D12_FENCE_FLAGS flags;
	};
	template<>
	struct ObjectTraits<ID3D12Fence>
	{
		using Desc = FenceDesc;
		static constexpr ObjectType Type = ObjectType::Fence;
	};
}