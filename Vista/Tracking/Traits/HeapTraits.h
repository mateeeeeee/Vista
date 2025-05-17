#pragma once
#include "Tracking/ObjectTraits.h"

namespace vista
{
	struct HeapDesc
	{
		D3D12_HEAP_DESC heapDesc;
	};

	template<>
	struct ObjectTraits<ID3D12Heap>
	{
		using Desc = HeapDesc;
		static constexpr ObjectType Type = ObjectType::Heap;
	};
}