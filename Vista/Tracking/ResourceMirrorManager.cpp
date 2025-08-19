#include "ResourceMirrorManager.h"

namespace vista
{
	namespace
	{
		Bool IsBufferDesc(D3D12_RESOURCE_DESC const& d)
		{
			return d.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER;
		}
		Bool IsBufferDesc(D3D12_RESOURCE_DESC1 const& d)
		{
			return d.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER;
		}

		Bool IsBuffer(ID3D12Resource* r)
		{
			return r ? IsBufferDesc(r->GetDesc()) : false;
		}

		Uint64 GetBufferSizeBytes(D3D12_RESOURCE_DESC const& d)
		{
			return IsBufferDesc(d) ? d.Width : 0;
		}
		Uint64 GetBufferSizeBytes(D3D12_RESOURCE_DESC1 const& d)
		{
			return IsBufferDesc(d) ? d.Width : 0;
		}
	}

	void ResourceMirrorManager::OnResourceCreated(ID3D12Resource* resource, D3D12_RESOURCE_DESC const& desc)
	{
		if (!resource || !IsBufferDesc(desc))
		{
			return;
		}

		Mirror m{};
		m.size = GetBufferSizeBytes(desc);
		m.data.resize(static_cast<Uint64>(m.size), 0);
		m.gpuVA = resource->GetGPUVirtualAddress();

		std::lock_guard lock(mirrorMutex);
		mirrorMap[resource] = std::move(m);
	}

	void ResourceMirrorManager::OnResourceCreated(ID3D12Resource* resource, D3D12_RESOURCE_DESC1 const& desc)
	{
		if (!resource || !IsBufferDesc(desc))
		{
			return;
		}

		Mirror m{};
		m.size = GetBufferSizeBytes(desc);
		m.data.resize(static_cast<Uint64>(m.size), 0);
		m.gpuVA = resource->GetGPUVirtualAddress();

		std::lock_guard lock(mirrorMutex);
		mirrorMap[resource] = std::move(m);
	}

	void ResourceMirrorManager::OnResourceReleased(ID3D12Resource* resource)
	{
		if (resource)
		{
			std::lock_guard lock(mirrorMutex);
			mirrorMap.erase(resource);
		}
	}

	void ResourceMirrorManager::OnMap(ID3D12Resource* resource, UINT subresource, D3D12_RANGE const* readRange, void** data)
	{
		if (!resource || !data)
		{
			return;
		}

		std::lock_guard lock(mirrorMutex);
		auto it = mirrorMap.find(resource);
		if (it == mirrorMap.end())
		{
			return;
		}

		it->second.lastMappedPtr = *data;
	}

	void ResourceMirrorManager::OnUnmap(ID3D12Resource* resource, UINT subresource, D3D12_RANGE const* writtenRange)
	{
		if (!resource)
		{
			return;
		}

		std::lock_guard lock(mirrorMutex);
		auto it = mirrorMap.find(resource);
		if (it == mirrorMap.end())
		{
			return;
		}
		Mirror& m = it->second;

		if (!m.lastMappedPtr)
		{
			return;
		}

		Uint64 const begin = writtenRange ? static_cast<Uint64>(writtenRange->Begin) : 0;
		Uint64 const end = writtenRange ? static_cast<Uint64>(writtenRange->End) : static_cast<Uint64>(m.size);
		if (end > m.data.size() || begin > end) 
		{ 
			m.lastMappedPtr = nullptr; 
			return; 
		}

		Uint64 const n = end - begin;
		std::memcpy(m.data.data() + begin,
			static_cast<Uint8 const*>(m.lastMappedPtr) + begin,
			n);

		// We don't keep the pointer after Unmap.
		m.lastMappedPtr = nullptr;
	}

	void ResourceMirrorManager::OnCopyBuffer(ID3D12Resource* dst, UINT64 dstOffset, ID3D12Resource* src, UINT64 srcOffset, UINT64 numBytes)
	{
		if (!dst || !src)
		{
			return;
		}

		std::lock_guard lock(mirrorMutex);
		auto itDst = mirrorMap.find(dst);
		auto itSrc = mirrorMap.find(src);
		if (itDst == mirrorMap.end() || itSrc == mirrorMap.end()) 
		{
			return;
		}
		Mirror& md = itDst->second;
		Mirror& ms = itSrc->second;

		if ((dstOffset + numBytes > md.size) || (srcOffset + numBytes > ms.size))
		{
			return;
		}

		std::memcpy(md.data.data() + static_cast<Uint64>(dstOffset),
			ms.data.data() + static_cast<Uint64>(srcOffset),
			static_cast<Uint64>(numBytes));
	}

	void ResourceMirrorManager::OnCopyResource(ID3D12Resource* dst, ID3D12Resource* src)
	{
		if (!dst || !src || !IsBuffer(dst) || !IsBuffer(src))
		{
			return;
		}

		std::lock_guard lock(mirrorMutex);
		auto itDst = mirrorMap.find(dst);
		auto itSrc = mirrorMap.find(src);
		if (itDst == mirrorMap.end() || itSrc == mirrorMap.end())
		{
			return;
		}

		Mirror& md = itDst->second;
		Mirror& ms = itSrc->second;
		Uint64 const n = std::min<Uint64>(md.size, ms.size);
		if (n == 0)
		{
			return;
		}

		std::memcpy(md.data.data(), ms.data.data(), n);
	}

	Bool ResourceMirrorManager::ReadBytes(ID3D12Resource* resource, Uint64 offset, void* dst, Uint64 size) const
	{
		if (!resource || !dst || size == 0)
		{
			return false;
		}

		std::lock_guard lock(mirrorMutex);
		auto it = mirrorMap.find(const_cast<ID3D12Resource*>(resource));
		if (it == mirrorMap.end())
		{
			return false;
		}

		Mirror& m = const_cast<Mirror&>(it->second);
		if (offset + size > m.size)
		{
			return false;
		}

		//If persistently mapped
		if (m.lastMappedPtr != nullptr) 
		{
			std::memcpy(m.data.data() + offset,
				static_cast<Uint8 const*>(m.lastMappedPtr) + offset,
				size);
		}

		std::memcpy(dst, m.data.data() + static_cast<Uint64>(offset), size);
		return true;
	}

}

