#pragma once


namespace vista
{
	class ResourceStateTracker
	{
		struct ResourceState
		{
			D3D12_RESOURCE_STATES globalState = D3D12_RESOURCE_STATE_COMMON;
			std::unordered_map<ID3D12GraphicsCommandList*, D3D12_RESOURCE_STATES> commandListStates;
		};

	public:
		ResourceStateTracker() = default;
		~ResourceStateTracker();

		void Initialize(ID3D12Device* device);

		void OnResourceCreated(
			ID3D12Resource* resource,
			D3D12_RESOURCE_STATES initialResourceState);

		void OnResourceReleased(ID3D12Resource* resource);

		void OnResourceBarrier(
			ID3D12GraphicsCommandList* cmdList,
			UINT numBarriers,
			D3D12_RESOURCE_BARRIER const* pBarriers);

		void OnExecuteCommandLists(
			ID3D12CommandQueue* commandQueue,
			UINT commandListCount,
			ID3D12CommandList* const* commandLists);

		void OnReset(
			ID3D12GraphicsCommandList* cmdList,
			ID3D12CommandAllocator* cmdAllocator,
			ID3D12PipelineState* pipelineState);

		D3D12_RESOURCE_STATES QueryResourceState(
			ID3D12GraphicsCommandList* commandList,
			ID3D12Resource* resource) const;

	private:
		ID3D12Device* device = nullptr;

		mutable std::mutex stateMutex;
		std::unordered_map<ID3D12Resource*, ResourceState> resourceStates;

		std::vector<std::pair<Ref<ID3D12CommandAllocator>, Ref<ID3D12GraphicsCommandList>>> availableBarrierCommandLists;
		std::unordered_map<ID3D12CommandList*, Ref<ID3D12CommandAllocator>> submittedBarrierAllocators;
		std::unordered_map<ID3D12CommandList*, Uint64> submittedBarrierListsFences;

		Ref<ID3D12Fence> fence;
		HANDLE fenceEvent = nullptr;
		Uint64 fenceValue = 0;

	private:
		void AnalyzeAndGenerateFixingBarriers(
			UINT numCommandLists,
			ID3D12CommandList* const* commandLists,
			std::vector<std::vector<D3D12_RESOURCE_BARRIER>>& fixingBarriersPerList);

		void CreateBarrierCommandList(
			const std::vector<D3D12_RESOURCE_BARRIER>& barriers,
			ID3D12CommandList** barrierCommandList);

		void TrimCommandListPool(ID3D12CommandQueue* commandQueue);
	};
}