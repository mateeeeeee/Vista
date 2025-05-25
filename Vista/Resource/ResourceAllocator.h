#pragma once

namespace vista
{
	class ResourceAllocator 
	{
		struct PooledResource
		{
			Ref<ID3D12Resource> resource;
			D3D12_RESOURCE_DESC desc;
			Bool inUse;
		};

	public:
		ResourceAllocator();
		~ResourceAllocator();

		void Initialize(ID3D12Device* device);

		ID3D12Resource* AllocateResource(D3D12_RESOURCE_DESC const& desc);
		void ReleaseResource(ID3D12Resource* resource);
		void Cleanup();

	private:
		ID3D12Device* device = nullptr;
		std::vector<PooledResource> resourcePool;
		std::mutex poolMutex;
	};
}