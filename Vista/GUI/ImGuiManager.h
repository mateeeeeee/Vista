#pragma once
#include "D3D12/D3D12Fence.h"
#include "D3D12/D3D12DescriptorAllocator.h"

namespace vista
{
	class ImGuiManager
	{
		static constexpr Uint32 MaxFramesInFlight = 3;

	public:
		Bool Initialize(ID3D12Device*);
		void Shutdown();

		ID3D12Device* GetDevice() const { return device; }

		Bool BeginFrame();
		void EndFrame();

		D3D12Descriptor GetDescriptorForThisFrame() const;

		LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		Bool initialized = false;
		HWND window = nullptr;
		Uint32 windowWidth = 1280;
		Uint32 windowHeight = 720;
		Bool needsResize;


		ID3D12Device* device;
		Ref<ID3D12CommandQueue> commandQueue;
		Ref<IDXGISwapChain3> swapChain;
		Ref<ID3D12GraphicsCommandList> commandList;

		std::vector<Ref<ID3D12CommandAllocator>> commandAllocators;
		std::vector<Ref<ID3D12Resource>> backBuffers;
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> backBufferDescriptors;
		Uint32 bufferIndex = 0;
		Uint32 const bufferCount = MaxFramesInFlight;

		std::unique_ptr<D3D12DescriptorAllocator> rtvDescriptorAllocator;
		std::unique_ptr<D3D12DescriptorAllocator> gpuVisibleAllocator;

		D3D12Fence frameFence;
		Uint64 fenceValues[MaxFramesInFlight] = {};
		Uint64 nextFenceValue = 1;

		std::string iniPath;

	private:
		void SetIniFile();
		void SetWindowIcon(HWND hwnd, Wchar const* iconPath);
		Bool CreateWindowAndSwapChain();
		void DestroyWindowAndSwapChain();
		Bool ResizeSwapChain(Uint32 newWidth, Uint32 newHeight);
		void SetStyle();
		void Flush();
	};
}
