#pragma once
#include "Command/Command.h"
#include "Utilities/LinearAllocator.h"

namespace vista
{
	class CommandRecorder;
	class ObjectTracker;
	class RecorderManager
	{
	public:
		explicit RecorderManager(ObjectTracker&);
		~RecorderManager() = default;

		template<typename ObjectT>
		CommandRecorder& GetOrCreateRecorder(ObjectT* keyObject)
		{
			//some DXGI Internals are ID3D12Resource* but they return nullptr for QueryInterface(ID3D12Resource),
			//they also implement QueryInterface(ID3D12Fence) at the same time... 
			//so get the base category from static type and query for others
			CommandCategory category = GetBaseCategory(keyObject);

			Ref<ID3D12Device> device;
			if (SUCCEEDED(keyObject->QueryInterface(IID_PPV_ARGS(device.GetAddressOf())))) 
			{
				category |= CommandCategory::Device;
			}
			Ref<ID3D12CommandQueue> queue;
			if (SUCCEEDED(keyObject->QueryInterface(IID_PPV_ARGS(queue.GetAddressOf())))) 
			{
				category |= CommandCategory::Queue;
			}
			Ref<ID3D12CommandList> list; 
			if (SUCCEEDED(keyObject->QueryInterface(IID_PPV_ARGS(list.GetAddressOf())))) 
			{
				category |= CommandCategory::List;
			}
			Ref<ID3D12CommandAllocator> allocator;
			if (SUCCEEDED(keyObject->QueryInterface(IID_PPV_ARGS(allocator.GetAddressOf())))) 
			{
				category |= CommandCategory::Allocator;
			}
			Ref<ID3D12Fence> fence;
			if (SUCCEEDED(keyObject->QueryInterface(IID_PPV_ARGS(fence.GetAddressOf())))) 
			{
				category |= CommandCategory::Fence;
			}
			Ref<ID3D12Resource> resource;
			if (SUCCEEDED(keyObject->QueryInterface(IID_PPV_ARGS(resource.GetAddressOf())))) 
			{
				category |= CommandCategory::Resource;
			}

			if(category == CommandCategory::Invalid)
			{
				VISTA_ASSERT(false);
			}
			return GetOrCreateRecorder(keyObject, category);
		}
		CommandRecorder& GetOrCreateRecorder(void* key, CommandCategory category);

		void Clear();

		template<typename CmdT, typename... Args>
		CmdT* AllocateCommand(Args&&... args)
		{
			std::lock_guard lock(allocatorMutex);
			CmdT* cmd = frameAllocator.Allocate<CmdT>(1);
			frameAllocator.Construct(cmd, std::forward<Args>(args)...);
			return cmd;
		}

		Uint64 GetNextCommandIndex()
		{
			return commandIndex++;
		}

		CommandRecorder const* GetRecorder(void* key) const;

		auto const& GetRecorders() const { return recorderMap; }

	private:
		ObjectTracker& objectManager;
		LinearAllocator<16 * 1024> frameAllocator;
		std::unordered_map<void*, CommandRecorder> recorderMap;
		std::atomic_uint64_t commandIndex = 0;
		mutable std::mutex mapMutex;
		mutable std::mutex allocatorMutex;

	private:
		template<typename ObjectT>
		CommandCategory GetBaseCategory(ObjectT* obj)
		{
			using T = std::remove_const_t<ObjectT>;
			if constexpr (std::is_base_of_v<ID3D12Device, T>)
			{
				return CommandCategory::Device;
			}
			else if constexpr (std::is_base_of_v<ID3D12CommandQueue, T>)
			{
				return CommandCategory::Queue;
			}
			else if constexpr (std::is_base_of_v<ID3D12CommandList, T>)
			{
				return CommandCategory::List;
			}
			else if constexpr (std::is_base_of_v<ID3D12CommandAllocator, T>)
			{
				return CommandCategory::Allocator;
			}
			else if constexpr (std::is_base_of_v<ID3D12Resource, T>)
			{
				return CommandCategory::Resource;
			}
			else if constexpr (std::is_base_of_v<ID3D12Fence, T>)
			{
				return CommandCategory::Fence;
			}
			else
			{
				return CommandCategory::Invalid;
			}
		}
	};
}