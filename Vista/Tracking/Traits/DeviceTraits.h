#pragma once
#include "Tracking/ObjectTraits.h"

namespace vista
{
	using DeviceDesc = std::monostate;
	template<>
	struct ObjectTraits<ID3D12Device>
	{
		using Desc = DeviceDesc;
		static constexpr ObjectType Type = ObjectType::Device;
	};
}