#pragma once
#include "D3D12DescriptorAllocatorBase.h"

namespace vista
{
	struct D3D12DescriptorAllocatorDesc
	{
		D3D12DescriptorHeapType type = D3D12DescriptorHeapType::Invalid;
		Uint32 descriptorCount = 0;
		Bool shaderVisible = false;
	};

	class D3D12DescriptorAllocator : public D3D12DescriptorAllocatorBase
	{
		struct D3D12DescriptorRange
		{
			D3D12Descriptor begin;
			D3D12Descriptor end;
		};

	public:
		D3D12DescriptorAllocator(ID3D12Device* device, D3D12DescriptorAllocatorDesc const& desc);
		~D3D12DescriptorAllocator();

		VISTA_NODISCARD D3D12Descriptor AllocateDescriptor();
		void FreeDescriptor(D3D12Descriptor handle);

	private:
		D3D12Descriptor tailDescriptor;
		std::list<D3D12DescriptorRange> freeDescriptorRanges;
	};
}