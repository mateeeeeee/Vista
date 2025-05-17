#pragma once
#include "Tracking/ObjectTraits.h"

namespace vista
{
	struct AllocatorDesc
	{
		D3D12_COMMAND_LIST_TYPE type;
	};
	template<>
	struct ObjectTraits<ID3D12CommandAllocator>
	{
		using Desc = AllocatorDesc;
		static constexpr ObjectType Type = ObjectType::Allocator;
	};

}