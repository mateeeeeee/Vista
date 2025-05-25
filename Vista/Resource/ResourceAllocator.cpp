#include "ResourceAllocator.h"

namespace vista
{
	ResourceAllocator::ResourceAllocator()  {}
	ResourceAllocator::~ResourceAllocator()
	{
		Cleanup();
	}

	void ResourceAllocator::Initialize(ID3D12Device* d3d12device)
	{
		device = d3d12device;
	}

	ID3D12Resource* ResourceAllocator::AllocateResource(D3D12_RESOURCE_DESC const& desc)
	{
		std::lock_guard<std::mutex> lock(poolMutex);

		Bool useReadback = (desc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER);
		for (auto& pooled : resourcePool) 
		{
			if (!pooled.inUse && pooled.desc.Width == desc.Width &&
				pooled.desc.Height == desc.Height &&
				pooled.desc.DepthOrArraySize == desc.DepthOrArraySize &&
				pooled.desc.MipLevels == desc.MipLevels &&
				pooled.desc.Format == desc.Format &&
				pooled.desc.Dimension == desc.Dimension) 
			{
				pooled.inUse = true;
				return pooled.resource.Get();
			}
		}

		D3D12_RESOURCE_DESC poolResourceDesc = desc;
		poolResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		D3D12_HEAP_PROPERTIES heapProps{};
		heapProps.Type = useReadback ? D3D12_HEAP_TYPE_READBACK : D3D12_HEAP_TYPE_DEFAULT;
		Ref<ID3D12Resource> resource;
		HRESULT hr = device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			useReadback ? D3D12_RESOURCE_STATE_COMMON : D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(resource.GetAddressOf()));
		if (FAILED(hr)) 
		{
			return nullptr;
		}

		PooledResource pooledResource;
		pooledResource.resource = resource;
		pooledResource.desc = desc;
		pooledResource.inUse = true;
		resourcePool.push_back(std::move(pooledResource));
		return resource.Get();
	}

	void ResourceAllocator::ReleaseResource(ID3D12Resource* resource)
	{
		std::lock_guard<std::mutex> lock(poolMutex);
		for (auto& pooled : resourcePool) 
		{
			if (pooled.resource.Get() == resource && pooled.inUse) 
			{
				pooled.inUse = false;
				break;
			}
		}
	}

	void ResourceAllocator::Cleanup()
	{
		std::lock_guard<std::mutex> lock(poolMutex);
		resourcePool.clear();
	}
}

