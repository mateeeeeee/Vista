#pragma once

namespace vista
{
	class D3D12Fence
	{
	public:
		D3D12Fence();
		~D3D12Fence();

		Bool Create(ID3D12Device* device, Char const* name);

		void Wait(Uint64 value);
		void Signal(Uint64 value);

		Bool IsCompleted(Uint64 value);
		Uint64 GetCompletedValue() const;

		operator ID3D12Fence* () const { return fence.Get(); }

	private:
		Ref<ID3D12Fence> fence = nullptr;
		HANDLE event = nullptr;
	};
}