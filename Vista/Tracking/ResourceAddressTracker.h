#pragma once

namespace vista
{
	class ResourceAddressTracker
	{
		struct AddressInfo
		{
			D3D12_GPU_VIRTUAL_ADDRESS startAddress = 0;
			Uint64 allocatedSizeInBytes = 0;
		};

	public:
		ResourceAddressTracker() = default;
		~ResourceAddressTracker() = default;

		void Initialize(ID3D12Device* device); 

		void OnResourceCreated(ID3D12Resource* resource, D3D12_RESOURCE_DESC const& desc, ID3D12Heap* heap = nullptr);
		void OnResourceCreated(ID3D12Resource* resource, D3D12_RESOURCE_DESC1 const& desc, ID3D12Heap* heap = nullptr);
		void OnResourceReleased(ID3D12Resource* resource);

		Bool QueryResourceAddress(
			ID3D12Resource* resource,
			D3D12_GPU_VIRTUAL_ADDRESS& outStartAddress,
			Uint64& outSizeInBytes) const;

		ID3D12Resource* QueryResource(D3D12_GPU_VIRTUAL_ADDRESS va) const;

	private:
		ID3D12Device* device = nullptr; 
		Ref<ID3D12Device8> device8 = nullptr;
		std::unordered_map<ID3D12Resource*, AddressInfo> resourceAddresses;		
		std::map<D3D12_GPU_VIRTUAL_ADDRESS, ID3D12Resource*> addressToResource; 
		mutable std::mutex addressMutex;
	};
}

