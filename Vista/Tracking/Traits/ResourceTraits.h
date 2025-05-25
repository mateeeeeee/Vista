#pragma once
#include "Tracking/ObjectTraits.h"

namespace vista
{
	struct ResourceDesc
	{
		D3D12_HEAP_PROPERTIES heapProperties;
		D3D12_HEAP_FLAGS heapFlags;
		D3D12_RESOURCE_DESC resourceDesc;
		D3D12_RESOURCE_STATES initialResourceState;
		std::optional<D3D12_CLEAR_VALUE> optimizedClearValue;
		ObjectID heapId = InvalidObjectID;
		Uint64 heapOffset = 0;
		D3D12_MIP_REGION mipRegion;
	};
	template<>
	struct ObjectTraits<ID3D12Resource>
	{
		using Desc = ResourceDesc;
		static constexpr ObjectType Type = ObjectType::Resource;
	};
}