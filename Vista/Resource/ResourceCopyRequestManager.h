#pragma once
#include "ResourceAllocator.h"
#include "ResourceCopyRequest.h"
#include "GUI/SelectedItem.h"

namespace vista
{
	class ResourceCopyRequestManager
	{
	public:
		ResourceCopyRequestManager() = default;
		~ResourceCopyRequestManager() = default;

		void Initialize(ID3D12Device* device)
		{
			resourceAllocator.Initialize(device);
			device->CreateFence(requestFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(requestFence.GetAddressOf()));
		}

		void RequestCopy(SelectedCommandInfo const& commandInfo, SelectedItem const& itemInfo)
		{
			if (!itemInfo.resource)
			{
				return;
			}
			ID3D12Resource* destinationResource = resourceAllocator.AllocateResource(itemInfo.resource->GetDesc());
			ResourceCopyRequest& copyRequest = copyRequestsQueue.emplace_back(commandInfo.commandIndex, commandInfo.commandType, itemInfo.resource, destinationResource);
			copyRequest.fenceValue = ++requestFenceValue;
		}
		void FreeRequest(ResourceCopyRequest const& request)
		{
			if (request.destinationResource)
			{
				resourceAllocator.ReleaseResource(request.destinationResource);
			}
		}
		std::optional<ResourceCopyRequest> TryGetReadyRequest()
		{
			if (copyRequestsQueue.empty() || !std::any_of(copyRequestsQueue.begin(), copyRequestsQueue.end(), [](ResourceCopyRequest const& req) { return req.IsReady(); }))
			{
				return std::nullopt;
			}
			while (!copyRequestsQueue.front().IsReady())
			{
				ResourceCopyRequest copyRequest = copyRequestsQueue.front();
				resourceAllocator.ReleaseResource(copyRequest.GetDestinationResource());
				copyRequestsQueue.pop_front();
			}
			ResourceCopyRequest copyRequest = copyRequestsQueue.front();
			copyRequestsQueue.pop_front();
			return copyRequest;
		}
		std::optional<ResourceCopyRequest> FindMatchingRequest(Command const& cmd)
		{
			for (ResourceCopyRequest const& req : copyRequestsQueue)
			{
				if (req.IsMatch(cmd)) return req;
			}
			return std::nullopt;
		}

		void SignalOnQueue(ID3D12CommandQueue* pCommandQueue)
		{
			pCommandQueue->Signal(requestFence.Get(), requestFenceValue);
		}
		void RefreshRequestsState()
		{
			for (ResourceCopyRequest& req : copyRequestsQueue)
			{
				Uint64 completedFenceValue = requestFence->GetCompletedValue();
				if (completedFenceValue >= req.fenceValue)
				{
					req.isReady = true;
				}
			}
		}

	private:
		ResourceAllocator resourceAllocator;
		Ref<ID3D12Fence> requestFence;
		Uint64 requestFenceValue = 0;
		std::deque<ResourceCopyRequest> copyRequestsQueue;
	};
}

