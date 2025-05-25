#pragma once
#include "Traits/AllObjectTraits.h"
#include "Utilities/D3D12Util.h"

namespace vista
{
	using ObjectDesc = std::variant<
		std::monostate,
		QueueDesc,
		ListDesc,
		AllocatorDesc,
		FenceDesc,
		PSODesc,
		RootSignatureDesc,
		CommandSignatureDesc,
		ResourceDesc,
		DescriptorHeapDesc,
		HeapDesc,
		StateObjectDesc
	>;

	struct TrackedObjectInfo
	{
		ObjectID	objectId = InvalidObjectID;
		ObjectType	objectType = ObjectType::Unknown;
		ObjectDesc	objectDesc;
		void*		objectPtr = nullptr;
		std::string objectDebugName;
	};

	class ObjectTracker
	{
	public:
		ObjectTracker() = default;
		VISTA_NONCOPYABLE_NONMOVABLE(ObjectTracker)
		~ObjectTracker() = default;

		template<typename ObjectT, typename DescT>
		VISTA_MAYBE_UNUSED ObjectID TrackObject(void* objectPtr, DescT const& desc)
		{
			static_assert(std::is_same_v<DescT, typename ObjectTraits<ObjectT>::Desc>);

			if (!objectPtr)
			{
				return InvalidObjectID;
			}
			void* voidPtr = static_cast<void*>(objectPtr);

			std::lock_guard lock(objectMutex);
			if (trackedObjects.contains(voidPtr))
			{
				return trackedObjects[voidPtr].objectId;
			}

			TrackedObjectInfo info{};
			info.objectId = nextObjectID.fetch_add(1, std::memory_order_relaxed);
			info.objectType = ObjectTraits<ObjectT>::Type;
			info.objectPtr = voidPtr;
			info.objectDesc = desc;

			auto [it, success] = trackedObjects.emplace(voidPtr, std::move(info));
			if (success)
			{
				objectList.push_back(&(it->second));
			}
			return info.objectId;
		}

		template<typename ObjectT, typename DescT> requires (!std::is_same_v<ObjectT, void>)
		VISTA_MAYBE_UNUSED ObjectID TrackObject(ObjectT* objectPtr, DescT const& desc)
		{
			static_assert(std::is_same_v<DescT, typename ObjectTraits<ObjectT>::Desc>);

			void* voidPtr = static_cast<void*>(objectPtr);
			return TrackObject<ObjectT>(voidPtr, desc);
		}

		template<typename ObjectT>
		VISTA_MAYBE_UNUSED ObjectID TrackObject(ObjectT* objectPtr)
		{
			static_assert(std::is_same_v<std::monostate, typename ObjectTraits<ObjectT>::Desc>);

			void* voidPtr = static_cast<void*>(objectPtr);
			return TrackObject<ObjectT>(voidPtr, std::monostate{});
		}

		void SetDebugName(void* objectPtr, std::string const& objectName)
		{
			if (!objectPtr)
			{
				return;
			}
			std::lock_guard lock(objectMutex);
			auto it = trackedObjects.find(objectPtr);
			if (it != trackedObjects.end())
			{
				it->second.objectDebugName = objectName;
			}
		}

		VISTA_NODISCARD ObjectID GetObjectID(void* objectPtr) const
		{
			if (!objectPtr)
			{
				return InvalidObjectID;
			}

			std::lock_guard lock(objectMutex); 
			auto it = trackedObjects.find(objectPtr);
			if (it != trackedObjects.end())
			{
				return it->second.objectId;
			}
			return InvalidObjectID;
		}

		VISTA_NODISCARD TrackedObjectInfo const* GetObjectInfo(void* objectPtr) const
		{
			if (!objectPtr)
			{
				return nullptr;
			}
			std::lock_guard lock(objectMutex); 
			auto it = trackedObjects.find(objectPtr);
			if (it != trackedObjects.end())
			{
				return &(it->second);
			}
			return nullptr;
		}
		VISTA_NODISCARD TrackedObjectInfo const* GetObjectInfo(ObjectID objectId) const
		{
			if (objectId == InvalidObjectID)
			{
				return nullptr;
			}
			std::lock_guard lock(objectMutex);
			return objectList[objectId];
		}
		VISTA_NODISCARD std::vector<TrackedObjectInfo*> const& GetObjectList() const
		{
			return objectList;
		}


	private:
		mutable std::mutex objectMutex;
		std::unordered_map<void*, TrackedObjectInfo> trackedObjects;
		std::vector<TrackedObjectInfo*> objectList;
		std::atomic<ObjectID> nextObjectID = 0;
	};
}