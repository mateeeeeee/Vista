#pragma once
#include "D3D12Descriptor.h"
#include "Utilities/Ref.h"

namespace vista
{
	enum class D3D12DescriptorHeapType : Uint8
	{
		CBV_SRV_UAV = 0,
		Sampler,
		RTV,
		DSV,
		Count,
		Invalid
	};

	class D3D12DescriptorAllocatorBase
	{
	public:
		D3D12Descriptor GetHandle(Uint32 index = 0) const;
		ID3D12DescriptorHeap* GetHeap() const { return heap.Get(); }
		
	protected:
		ID3D12Device* device;
		Ref<ID3D12DescriptorHeap> heap = nullptr;
		D3D12Descriptor headDescriptor;
		Uint32 descriptorHandleSize = 0;
		D3D12DescriptorHeapType type = D3D12DescriptorHeapType::Invalid;
		Uint32 descriptorCount = 0;
		Bool shaderVisible = false;

	protected:
		D3D12DescriptorAllocatorBase(ID3D12Device* device, D3D12DescriptorHeapType type, Uint32 descriptorCount, Bool shaderVisible);
		void CreateHeap();
	};
}