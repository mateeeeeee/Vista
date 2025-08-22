#include "D3D12DescriptorAllocatorBase.h"

namespace vista
{
	inline constexpr D3D12_DESCRIPTOR_HEAP_TYPE ToD3D12HeapType(D3D12DescriptorHeapType type)
	{
		switch (type)
		{
		case D3D12DescriptorHeapType::CBV_SRV_UAV:
			return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		case D3D12DescriptorHeapType::Sampler:
			return D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		case D3D12DescriptorHeapType::RTV:
			return D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		case D3D12DescriptorHeapType::DSV:
			return D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		}
		return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	}

	D3D12Descriptor D3D12DescriptorAllocatorBase::GetHandle(Uint32 index /*= 0*/) const
	{
		VISTA_ASSERT(heap != nullptr);
		VISTA_ASSERT(index < descriptorCount);

		D3D12Descriptor handle = headDescriptor;
		handle.Increment(descriptorHandleSize, index);
		return handle;
	}

	D3D12DescriptorAllocatorBase::D3D12DescriptorAllocatorBase(ID3D12Device* device, D3D12DescriptorHeapType type, Uint32 descriptorCount, Bool shaderVisible) : device(device),
		type(type),
		descriptorCount(descriptorCount),
		shaderVisible(shaderVisible), headDescriptor{}
	{
		CreateHeap();
		headDescriptor.cpu = heap->GetCPUDescriptorHandleForHeapStart();
		if(shaderVisible) headDescriptor.gpu = heap->GetGPUDescriptorHandleForHeapStart();
		headDescriptor.index = 0;
	}

	void D3D12DescriptorAllocatorBase::CreateHeap()
	{
		VISTA_ASSERT(descriptorCount <= UINT32_MAX && "Too many descriptors");
		D3D12_DESCRIPTOR_HEAP_DESC heap_desc{};
		heap_desc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		heap_desc.NumDescriptors = descriptorCount;
		heap_desc.Type = ToD3D12HeapType(type);
		VISTA_ASSERT(device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(heap.ReleaseAndGetAddressOf())) == S_OK);
		descriptorHandleSize = device->GetDescriptorHandleIncrementSize(heap_desc.Type);
	}

}