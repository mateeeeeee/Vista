#pragma once
#include "Tracking/ObjectID.h"
#include "Tracking/DescriptorTracker.h"

namespace vista
{
	struct SelectedCommandInfo
	{
		Uint64 commandIndex = UINT64_MAX;
		Uint64 commandHash = 0;
		Uint32 commandType = 0;

		Bool IsValid() const
		{
			return commandIndex != UINT64_MAX;
		}

		void Reset()
		{
			commandIndex = UINT64_MAX;
			commandType = 0;
			commandHash = 0;
		}
	};

	struct SelectedItem
	{
		enum class Type
		{
			None,
			Resource,
			Sampler,
		};
		Type type = Type::None;
		ID3D12Resource* resource = nullptr;
		ObjectID heapId = InvalidObjectID;
		Int descriptorIndex = -1;
		DescriptorInfo descriptorInfo;

		Bool Matches(ID3D12Resource* res, ObjectID hId, Int index) const
		{
			if (type == Type::None) return false;
			if (res && type == Type::Resource) return resource == res;
			if (!res && type == Type::Sampler) return heapId == hId && descriptorIndex == index;
			return false;
		}

		void Reset()
		{
			type = Type::None;
			resource = nullptr;
			heapId = InvalidObjectID;
			descriptorIndex = -1;
			descriptorInfo = DescriptorInfo{};
		}
	};
}