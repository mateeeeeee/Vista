#pragma once
#include "Tracking/ObjectTraits.h"

namespace vista
{
	struct ListDesc
	{
		UINT nodeMask;
		D3D12_COMMAND_LIST_TYPE type;
		ObjectID commandAllocatorId;
		ObjectID initialStateId;
	};
	template<>
	struct ObjectTraits<ID3D12CommandList>
	{
		using Desc = ListDesc;
		static constexpr ObjectType Type = ObjectType::List;
	};

}