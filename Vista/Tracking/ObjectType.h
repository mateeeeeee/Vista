#pragma once

namespace vista
{
	enum class ObjectType : Uint8
	{
		Unknown,
		Device,
		Queue,
		List,
		Allocator,
		Fence,
		PSO,
		RootSignature,
		CommandSignature,
		Resource,
		DescriptorHeap,
		Heap,
		StateObject,
		Count
	};
	static Char const* ObjectTypeNames[] =
	{
		"Unknown",
		"Device",
		"Queue",
		"List",
		"Allocator",
		"Fence",
		"PSO",
		"RootSignature",
		"CommandSignature",
		"Resource",
		"DescriptorHeap",
		"Heap",
		"StateObject",
	};
	static_assert(std::size(ObjectTypeNames) == (Uint64)ObjectType::Count);
}