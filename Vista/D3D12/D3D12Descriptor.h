#pragma once

namespace vista
{
	class D3D12Descriptor
	{
		friend class D3D12DescriptorAllocatorBase;

	public:
		D3D12Descriptor() {}
		VISTA_DEFAULT_COPYABLE_MOVABLE(D3D12Descriptor)

		operator D3D12_CPU_DESCRIPTOR_HANDLE() const { return cpu; }
		operator D3D12_GPU_DESCRIPTOR_HANDLE() const { return gpu; }

		Uint32 GetIndex() const { return index; }
		void Increment(Uint32 increment, Uint32 multiply = 1)
		{
			cpu.ptr += increment * multiply;
			if(gpu.ptr != NULL) gpu.ptr += increment * multiply;
			index += multiply;
		}

		Bool operator==(D3D12Descriptor const& other) const
		{
			return cpu.ptr == other.cpu.ptr && index == other.index;
		}

		Bool IsValid() const
		{
			return cpu.ptr != NULL;
		}

	private:
		D3D12_CPU_DESCRIPTOR_HANDLE cpu = { NULL };
		D3D12_GPU_DESCRIPTOR_HANDLE gpu = { NULL };
		Uint32 index = -1;
	};
}