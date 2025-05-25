#pragma once
#include "Tracking/ObjectTraits.h"

namespace vista
{
	struct QueueDesc
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc;
	};

	template<>
	struct ObjectTraits<ID3D12CommandQueue>
	{
		using Desc = QueueDesc;
		static constexpr ObjectType Type = ObjectType::Queue;
	};
}