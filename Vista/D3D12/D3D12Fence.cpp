#include "D3D12Fence.h"
#include "Utilities/StringUtil.h"

namespace vista
{

	D3D12Fence::D3D12Fence() {}

	D3D12Fence::~D3D12Fence()
	{
		CloseHandle(event);
	}

	Bool D3D12Fence::Create(ID3D12Device* device, Char const* name)
	{
		HRESULT hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf()));
		if (FAILED(hr)) return false;

		fence->SetName(ToWideString(name).c_str());
		event = CreateEvent(NULL, FALSE, FALSE, NULL);
		return true;
	}

	void D3D12Fence::Wait(Uint64 value)
	{
		if (!IsCompleted(value))
		{
			fence->SetEventOnCompletion(value, event);
			WaitForSingleObjectEx(event, INFINITE, FALSE);
		}
	}

	void D3D12Fence::Signal(Uint64 value)
	{
		fence->Signal(value);
	}

	Bool D3D12Fence::IsCompleted(Uint64 value)
	{
		return GetCompletedValue() >= value;
	}

	Uint64 D3D12Fence::GetCompletedValue() const
	{
		return fence->GetCompletedValue();
	}

}

