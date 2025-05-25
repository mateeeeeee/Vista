#include "RecorderManager.h"
#include "Command/CommandRecorder.h"
#include "Tracking/ObjectTracker.h"

namespace vista
{

	RecorderManager::RecorderManager(ObjectTracker& objectManager) : objectManager(objectManager)
	{
	}

	CommandRecorder& RecorderManager::GetOrCreateRecorder(void* key, CommandCategory category)
	{
		std::lock_guard lock(mapMutex);
		auto it = recorderMap.find(key);
		if (it == recorderMap.end())
		{
			Uint32 recorderIdx = (Uint32)recorderMap.size();
			it = recorderMap.emplace(
				std::piecewise_construct,
				std::forward_as_tuple(key),
				std::forward_as_tuple(*this, objectManager.GetObjectID(key), category)
			).first;
		}
		return it->second;
	}

	void RecorderManager::Clear()
	{
		recorderMap.clear();
		frameAllocator.Reset();
		commandIndex = 0;
	}

	CommandRecorder const* RecorderManager::GetRecorder(void* key) const
	{
		std::lock_guard lock(mapMutex);
		auto it = recorderMap.find(key);
		if (it == recorderMap.end())
		{
			return nullptr;
		}
		return &(it->second);
	}
}