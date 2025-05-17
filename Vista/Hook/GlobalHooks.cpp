#include "GlobalHooks.h"
#include "Core/Vista.h"
#include "DirectHook/MinHook/include/MinHook.h"

namespace vista
{
	typedef HRESULT(WINAPI* PFN_D3D12CreateDevice)(IDXGIAdapter*, D3D_FEATURE_LEVEL, REFIID, _COM_Outptr_opt_ void**);

	inline PFN_D3D12CreateDevice				g_originalD3D12CreateDevice = nullptr;

	HRESULT WINAPI D3D12CreateDevice(IDXGIAdapter* pAdapter, D3D_FEATURE_LEVEL MinimumFeatureLevel, REFIID riid, _COM_Outptr_opt_ void** ppDevice)
	{
		static Bool called = false;
		if (!called)
		{
			called = true;
			HRESULT hr = g_originalD3D12CreateDevice(pAdapter, MinimumFeatureLevel, riid, ppDevice);
			if (SUCCEEDED(hr) && ppDevice && *ppDevice)
			{
				ID3D12Device* device = reinterpret_cast<ID3D12Device*>(*ppDevice);
				g_Vista.OnDeviceCreated(device);
			}
			return hr;
		}
		return g_originalD3D12CreateDevice(pAdapter, MinimumFeatureLevel, riid, ppDevice);
	}

	Bool InitializeGlobalHooks()
	{
		MH_STATUS mhStatus = MH_Initialize();
		if (mhStatus != MH_OK)
		{
			return false;
		}

		HMODULE hD3D12 = ::GetModuleHandle(L"d3d12.dll");
		if (!hD3D12)
		{
			return false;
		}

		void* procAddr = GetProcAddress(hD3D12, "D3D12CreateDevice");
		if (!procAddr)
		{
			return false;
		}
		if (MH_CreateHook(procAddr, D3D12CreateDevice, reinterpret_cast<void**>(&g_originalD3D12CreateDevice)) != MH_OK)
		{
			return false;
		}
		if (MH_EnableHook(procAddr) != MH_OK)
		{
			return false;
		}

		return true;
	}

}

