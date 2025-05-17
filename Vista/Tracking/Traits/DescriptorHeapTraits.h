#pragma once
#include "Tracking/ObjectTraits.h"

namespace vista
{
	struct DescriptorHeapDesc
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
	};

	template<>
	struct ObjectTraits<ID3D12DescriptorHeap>
	{
		using Desc = DescriptorHeapDesc;
		static constexpr ObjectType Type = ObjectType::DescriptorHeap;
	};
}