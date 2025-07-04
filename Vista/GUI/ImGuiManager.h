#pragma once

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

		LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptor() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptor() const;

	private:
		Bool initialized = false;
		HWND window = nullptr;
		Uint32 windowWidth = 1280;
		Uint32 windowHeight = 720;
		Bool needsResize;

		ID3D12Device* device;
		Ref<ID3D12CommandQueue> commandQueue;
		Ref<IDXGISwapChain3> swapChain;
		Ref<ID3D12DescriptorHeap> rtvDescriptorHeap;
		Ref<ID3D12DescriptorHeap> gpuVisibleHeap;
		Ref<ID3D12GraphicsCommandList> commandList;
		std::vector<Ref<ID3D12CommandAllocator>> commandAllocators;
		std::vector<Ref<ID3D12Resource>> backBuffers;
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> backBufferDescriptors;

		Uint32 bufferIndex = 0;
		Uint32 bufferCount = MaxFramesInFlight;

		Ref<ID3D12Fence> fence;
		HANDLE fenceEvent = nullptr;
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
