#include "ResourceStateTracker.h"

namespace vista
{

	ResourceStateTracker::~ResourceStateTracker() { CloseHandle(fenceEvent); }
	void ResourceStateTracker::Initialize(ID3D12Device* device)
	{
		if (!device) return;
		this->device = device;
		if (FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf()))))
		{
			return;
		}
		fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (!fenceEvent)
		{
			return;
		}
	}

	void ResourceStateTracker::OnResourceCreated(
		ID3D12Resource* resource,
		D3D12_RESOURCE_STATES initialResourceState)
	{
		if (resource)
		{
			std::lock_guard lock(stateMutex);
			ResourceState& resourceState = resourceStates[resource];
			resourceState.globalState = initialResourceState;
		}
	}

	void ResourceStateTracker::OnResourceReleased(ID3D12Resource* resource)
	{
		if (resource)
		{
			std::lock_guard lock(stateMutex);
			auto it = resourceStates.find(resource);
			if (it != resourceStates.end())
			{
				resourceStates.erase(it);
			}
		}
	}

	void ResourceStateTracker::OnResourceBarrier(
		ID3D12GraphicsCommandList* cmdList,
		UINT numBarriers,
		D3D12_RESOURCE_BARRIER const* pBarriers)
	{
		std::lock_guard lock(stateMutex);
		for (Uint i = 0; i < numBarriers; ++i)
		{
			D3D12_RESOURCE_BARRIER const& barrier = pBarriers[i];
			if (barrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION)
			{
				ResourceState& resourceState = resourceStates[barrier.Transition.pResource];
				resourceState.commandListStates[cmdList] = barrier.Transition.StateAfter;
			}
		}
	}

	void ResourceStateTracker::OnExecuteCommandLists(
		ID3D12CommandQueue* commandQueue,
		UINT commandListCount,
		ID3D12CommandList* const* commandLists)
	{
		std::lock_guard lock(stateMutex);
		TrimCommandListPool(commandQueue);
		std::vector<std::vector<D3D12_RESOURCE_BARRIER>> fixingBarriersPerList(commandListCount);
		AnalyzeAndGenerateFixingBarriers(commandListCount, commandLists, fixingBarriersPerList);
		std::vector<ID3D12CommandList*> rawCommandLists;
		rawCommandLists.reserve(commandListCount + commandListCount);
		std::vector<ID3D12CommandList*> submittedBarrierLists;
		for (Uint i = 0; i < commandListCount; ++i)
		{
			if (!fixingBarriersPerList[i].empty())
			{
				ID3D12CommandList* barrierList = nullptr;
				CreateBarrierCommandList(fixingBarriersPerList[i], &barrierList);
				if (barrierList)
				{
					rawCommandLists.push_back(barrierList);
					submittedBarrierLists.push_back(barrierList);
				}
			}
			rawCommandLists.push_back(commandLists[i]);
		}
		if (!rawCommandLists.empty())
		{
			commandQueue->ExecuteCommandLists(static_cast<UINT>(rawCommandLists.size()), rawCommandLists.data());
			Uint64 currentFenceValue = ++fenceValue;
			if (SUCCEEDED(commandQueue->Signal(fence.Get(), currentFenceValue)))
			{
				for (auto list : submittedBarrierLists)
				{
					submittedBarrierListsFences[list] = currentFenceValue;
				}
			}
		}
	}

	void ResourceStateTracker::OnReset(
		ID3D12GraphicsCommandList* cmdList,
		ID3D12CommandAllocator* cmdAllocator,
		ID3D12PipelineState* pipelineState)
	{
		if (cmdList)
		{
			std::lock_guard lock(stateMutex);
			for (auto& [resource, state] : resourceStates)
			{
				state.commandListStates.erase(cmdList);
			}
		}
	}

	D3D12_RESOURCE_STATES ResourceStateTracker::QueryResourceState(
		ID3D12GraphicsCommandList* commandList,
		ID3D12Resource* resource) const
	{
		std::lock_guard lock(stateMutex);
		auto it = resourceStates.find(resource);
		if (it == resourceStates.end())
		{
			return D3D12_RESOURCE_STATE_COMMON;
		}
		ResourceState const& resourceState = it->second;
		auto clIt = resourceState.commandListStates.find(commandList);
		if (clIt != resourceState.commandListStates.end())
		{
			return clIt->second;
		}
		return resourceState.globalState;
	}

	void ResourceStateTracker::AnalyzeAndGenerateFixingBarriers(
		UINT numCommandLists,
		ID3D12CommandList* const* commandLists,
		std::vector<std::vector<D3D12_RESOURCE_BARRIER>>& fixingBarriersPerList)
	{
		std::unordered_map<ID3D12Resource*, D3D12_RESOURCE_STATES> lastKnownStates;
		for (const auto& [resource, state] : resourceStates)
		{
			lastKnownStates[resource] = state.globalState;
		}
		for (UINT i = 0; i < numCommandLists; ++i)
		{
			ID3D12GraphicsCommandList* commandList = static_cast<ID3D12GraphicsCommandList*>(commandLists[i]);
			for (const auto& [resource, state] : resourceStates)
			{
				auto lastStateIt = lastKnownStates.find(resource);
				D3D12_RESOURCE_STATES stateBeforeList = (lastStateIt != lastKnownStates.end())
					? lastStateIt->second : D3D12_RESOURCE_STATE_COMMON;
				auto clStateIt = state.commandListStates.find(commandList);
				if (clStateIt != state.commandListStates.end())
				{
					D3D12_RESOURCE_STATES stateAfterList = clStateIt->second;
					if (stateBeforeList != stateAfterList)
					{
						D3D12_RESOURCE_BARRIER barrier = {};
						barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
						barrier.Transition.pResource = resource;
						barrier.Transition.StateBefore = stateBeforeList;
						barrier.Transition.StateAfter = stateAfterList;
						barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
						fixingBarriersPerList[i].push_back(barrier);
					}
					lastKnownStates[resource] = stateAfterList;
				}
			}
		}
		for (auto& [resource, state] : resourceStates)
		{
			auto lastStateIt = lastKnownStates.find(resource);
			if (lastStateIt != lastKnownStates.end() && !state.commandListStates.empty())
			{
				state.globalState = lastStateIt->second;
			}
		}
	}

	void ResourceStateTracker::CreateBarrierCommandList(
		const std::vector<D3D12_RESOURCE_BARRIER>& barriers,
		ID3D12CommandList** barrierCommandList)
	{
		if (barriers.empty())
		{
			*barrierCommandList = nullptr;
			return;
		}
		Ref<ID3D12GraphicsCommandList> list;
		Ref<ID3D12CommandAllocator> allocator;
		if (!availableBarrierCommandLists.empty())
		{
			auto& pair = availableBarrierCommandLists.back();
			allocator = std::move(pair.first);
			list = std::move(pair.second);
			availableBarrierCommandLists.pop_back();
			if (SUCCEEDED(allocator->Reset()) &&
				SUCCEEDED(list->Reset(allocator.Get(), nullptr)))
			{
			}
			else
			{
				allocator.Reset();
				list.Reset();
			}
		}
		if (!list)
		{
			if (FAILED(device->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(allocator.GetAddressOf()))))
			{
				*barrierCommandList = nullptr;
				return;
			}
			if (FAILED(device->CreateCommandList(
				0,
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				allocator.Get(),
				nullptr,
				IID_PPV_ARGS(list.GetAddressOf()))))
			{
				*barrierCommandList = nullptr;
				return;
			}
		}
		list->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
		if (SUCCEEDED(list->Close()))
		{
			ID3D12CommandList* rawListPtr = list.Get();
			*barrierCommandList = list.Detach();
			submittedBarrierAllocators[rawListPtr] = std::move(allocator);
		}
		else
		{
			*barrierCommandList = nullptr;
			list.Reset();
			allocator.Reset();
		}
	}

	void ResourceStateTracker::TrimCommandListPool(ID3D12CommandQueue* commandQueue)
	{
		Uint64 completedFenceValue = fence->GetCompletedValue();
		auto it = submittedBarrierListsFences.begin();
		while (it != submittedBarrierListsFences.end())
		{
			ID3D12CommandList* list = it->first;
			Uint64 listFenceValue = it->second;
			if (listFenceValue <= completedFenceValue)
			{
				auto allocIt = submittedBarrierAllocators.find(list);
				if (allocIt != submittedBarrierAllocators.end())
				{
					Ref<ID3D12CommandAllocator> allocator = std::move(allocIt->second);
					Ref<ID3D12GraphicsCommandList> graphicsList;
					graphicsList.Attach(static_cast<ID3D12GraphicsCommandList*>(list));
					availableBarrierCommandLists.push_back({ std::move(allocator), std::move(graphicsList) });
					submittedBarrierAllocators.erase(allocIt);
				}
				it = submittedBarrierListsFences.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}