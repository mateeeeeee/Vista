#include "ResourceAddressTracker.h"


namespace vista
{
	void ResourceAddressTracker::Initialize(ID3D12Device* dev)
	{
		device = dev;
		device->QueryInterface(IID_PPV_ARGS(device8.GetAddressOf()));
	}

	void ResourceAddressTracker::OnResourceCreated(ID3D12Resource* resource, D3D12_RESOURCE_DESC const& desc, ID3D12Heap* heap /*= nullptr*/)
	{
		VISTA_ASSERT(device);
		if (desc.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER)
		{
			return;
		}

		if (resource)
		{
			std::lock_guard lock(addressMutex);
			D3D12_RESOURCE_ALLOCATION_INFO info = device->GetResourceAllocationInfo(0, 1, &desc);
			AddressInfo addressInfo;
			addressInfo.startAddress = resource->GetGPUVirtualAddress();
			addressInfo.allocatedSizeInBytes = info.SizeInBytes;
			resourceAddresses[resource] = addressInfo;
			addressToResource[addressInfo.startAddress] = resource;
		}
	}

	void ResourceAddressTracker::OnResourceCreated(ID3D12Resource* resource, D3D12_RESOURCE_DESC1 const& desc, ID3D12Heap* heap /*= nullptr*/)
	{
		VISTA_ASSERT(device8);

		if (desc.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER)
		{
			return;
		}

		if (resource)
		{
			std::lock_guard lock(addressMutex);
			
			D3D12_RESOURCE_ALLOCATION_INFO info = device8->GetResourceAllocationInfo2(0, 1, &desc, nullptr);
			AddressInfo addressInfo;
			addressInfo.startAddress = resource->GetGPUVirtualAddress();
			addressInfo.allocatedSizeInBytes = info.SizeInBytes;
			resourceAddresses[resource] = addressInfo;
			addressToResource[addressInfo.startAddress] = resource;
		}
	}

	void ResourceAddressTracker::OnResourceReleased(ID3D12Resource* resource)
	{
		if (resource)
		{
			std::lock_guard lock(addressMutex);
			auto it = resourceAddresses.find(resource);
			if (it != resourceAddresses.end())
			{
				addressToResource.erase(it->second.startAddress);
				resourceAddresses.erase(it);
			}
		}
	}

	Bool ResourceAddressTracker::QueryResourceAddress(
		ID3D12Resource* resource,
		D3D12_GPU_VIRTUAL_ADDRESS& outStartAddress,
		Uint64& outSizeInBytes) const
	{
		std::lock_guard lock(addressMutex);
		auto it = resourceAddresses.find(resource);
		if (it != resourceAddresses.end())
		{
			outStartAddress = it->second.startAddress;
			outSizeInBytes = it->second.allocatedSizeInBytes;
			return true;
		}
		return false;
	}

	ID3D12Resource* ResourceAddressTracker::QueryResource(D3D12_GPU_VIRTUAL_ADDRESS va) const
	{
		std::lock_guard lock(addressMutex);

		// Find the element whose key (start address) is <= va
		// upper_bound(va) returns iterator to the first element > va
		auto it = addressToResource.upper_bound(va);
		if (it == addressToResource.begin())
		{
			return nullptr;
		}

		--it;
		ID3D12Resource* candidateResource = it->second;

		auto infoIt = resourceAddresses.find(candidateResource);
		if (infoIt != resourceAddresses.end())
		{
			AddressInfo const& info = infoIt->second;
			if (va >= info.startAddress && va < (info.startAddress + info.allocatedSizeInBytes))
			{
				return candidateResource;
			}
		}
		return nullptr;
	}
}
