#include "D3D12DescriptorAllocator.h"

namespace vista
{
	D3D12DescriptorAllocator::D3D12DescriptorAllocator(ID3D12Device* device, D3D12DescriptorAllocatorDesc const& desc)
		: D3D12DescriptorAllocatorBase(device, desc.type, desc.descriptorCount, desc.shaderVisible),
		tailDescriptor(headDescriptor)
	{
		tailDescriptor.Increment(descriptorHandleSize ,desc.descriptorCount - 1);
		freeDescriptorRanges.emplace_back(headDescriptor, tailDescriptor);
	}

	D3D12DescriptorAllocator::~D3D12DescriptorAllocator() = default;

	D3D12Descriptor D3D12DescriptorAllocator::AllocateDescriptor()
	{
		D3D12DescriptorRange& range = freeDescriptorRanges.front();
		D3D12Descriptor handle = range.begin;
		range.begin.Increment(descriptorHandleSize);
		if (range.begin == range.end) freeDescriptorRanges.pop_front();
		return handle;
	}

	void D3D12DescriptorAllocator::FreeDescriptor(D3D12Descriptor handle)
	{
		D3D12Descriptor incrementedHandle = handle;
		incrementedHandle.Increment(descriptorHandleSize);

		D3D12DescriptorRange rng{
			.begin = handle,
			.end = incrementedHandle
		};

		Bool found = false;
		for (auto range = std::begin(freeDescriptorRanges);
			range != std::end(freeDescriptorRanges) && found == false; ++range)
		{
			if (range->begin == incrementedHandle)
			{
				range->begin = handle;
				found = true;
			}
			else if (range->end == handle)
			{
				range->end.Increment(descriptorHandleSize);
				found = true;
			}
			else if (range->begin.GetIndex() > handle.GetIndex())
			{
				freeDescriptorRanges.insert(range, rng);
				found = true;
			}
		}
		if (!found) freeDescriptorRanges.push_back(rng);
	}

}

