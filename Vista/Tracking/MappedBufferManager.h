#pragma once


namespace vista
{
	class MappedBufferManager
	{
		struct Mirror
		{
			std::vector<Uint8> data;  
			void* lastMappedPtr = nullptr;
			Uint64 gpuVA = 0;
			Uint64 size = 0;
		};

	public:
		MappedBufferManager() = default;
		~MappedBufferManager() = default;

		void OnResourceCreated(ID3D12Resource* resource, D3D12_RESOURCE_DESC const& desc);
		void OnResourceCreated(ID3D12Resource* resource, D3D12_RESOURCE_DESC1 const& desc);
		void OnResourceReleased(ID3D12Resource* resource);

		void OnMap(ID3D12Resource* resource, UINT subresource, D3D12_RANGE const* readRange, void** data);
		void OnUnmap(ID3D12Resource* resource, UINT subresource, D3D12_RANGE const* writtenRange);

		void OnCopyBuffer(ID3D12Resource* dst, UINT64 dstOffset, ID3D12Resource* src, UINT64 srcOffset, UINT64 numBytes);
		void OnCopyResource(ID3D12Resource* dst, ID3D12Resource* src);

		Bool ReadBytes(ID3D12Resource* resource, Uint64 offset, void* dst, Uint64 size) const;

	private:
		std::unordered_map<ID3D12Resource*, Mirror> mirrorMap;
		mutable std::mutex mirrorMutex;

	};
}