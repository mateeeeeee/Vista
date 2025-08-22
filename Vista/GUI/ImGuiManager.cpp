#include "ImGuiManager.h"
#include "Fonts/roboto_compressed.cpp"
#include "ImGui/imgui_impl_dx12.h"
#include "ImGui/imgui_impl_win32.h"
#include <filesystem>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace vista
{
	static ImGuiManager* s_ImGuiManagerInstance = nullptr;

	static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
			return true;

		if (s_ImGuiManagerInstance)
			return s_ImGuiManagerInstance->WndProc(hwnd, msg, wParam, lParam);

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	Bool ImGuiManager::Initialize(ID3D12Device* dev)
	{
		if (initialized)
		{
			return true;
		}
		s_ImGuiManagerInstance = this;
		device = dev;
		if (!CreateWindowAndSwapChain())
		{
			return false;
		}
		SetWindowIcon(window, L"Vista.ico");

		ImGui::CreateContext();
		SetIniFile();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.Fonts->AddFontFromMemoryCompressedBase85TTF(roboto_compressed_compressed_data_base85, 16.0f, NULL, io.Fonts->GetGlyphRangesDefault());

		ImGui_ImplWin32_Init(window);

		ImGui_ImplDX12_InitInfo init_info{};
		init_info.Device = device;
		init_info.NumFramesInFlight = bufferCount;
		init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		init_info.SrvDescriptorHeap = gpuVisibleAllocator->GetHeap();
		init_info.LegacySingleSrvCpuDescriptor = gpuVisibleAllocator->GetHandle(0); 
		init_info.LegacySingleSrvGpuDescriptor = gpuVisibleAllocator->GetHandle(0);
		init_info.CommandQueue = commandQueue.Get();
		ImGui_ImplDX12_Init(&init_info);

		initialized = true;
		SetStyle();

		return true;
	}

	void ImGuiManager::SetIniFile()
	{
		HMODULE hModule = GetModuleHandle(L"Vista.dll");
		if (!hModule)
		{
			return;
		}
		Char dllPath[MAX_PATH];
		if (GetModuleFileNameA(hModule, dllPath, MAX_PATH) == 0)
		{
			return;
		}
		std::filesystem::path dllDir = std::filesystem::path(dllPath).parent_path();
		iniPath = dllDir.string() + "/vista.ini";
		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = iniPath.c_str();
	}

	void ImGuiManager::SetWindowIcon(HWND hwnd, Wchar const* iconPath)
	{
		HICON hIcon = static_cast<HICON>(
			LoadImageW(
				nullptr,
				iconPath,
				IMAGE_ICON,
				0, 0,
				LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED
			)
			);

		if (hIcon)
		{
			SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
			SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
			RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
		}
	}

	Bool ImGuiManager::CreateWindowAndSwapChain()
	{
		WNDCLASS wc = {};
		wc.lpfnWndProc = StaticWndProc;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.lpszClassName = L"VistaWindowClass";
		RegisterClass(&wc);

		RECT rect = { 0, 0, (LONG)windowWidth, (LONG)windowHeight };
		AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, 0);

		window = CreateWindowEx(
			0,
			L"VistaWindowClass",
			L"Vista",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			rect.right - rect.left,
			rect.bottom - rect.top,
			nullptr, nullptr,
			wc.hInstance,
			nullptr
		);

		if (!window)
		{
			return false;
		}

		ShowWindow(window, SW_SHOW);

		Ref<IDXGIFactory4> dxgiFactory;
		if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(dxgiFactory.GetAddressOf()))))
		{
			return false;
		}

		Ref<IDXGIAdapter1> adapter;
		dxgiFactory->EnumAdapters1(0, adapter.GetAddressOf());

		D3D12_COMMAND_QUEUE_DESC cqDesc = {};
		cqDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		device->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(commandQueue.GetAddressOf()));

		DXGI_SWAP_CHAIN_DESC1 scDesc = {};
		scDesc.BufferCount = bufferCount;
		scDesc.Width = windowWidth;
		scDesc.Height = windowHeight;
		scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		scDesc.SampleDesc.Count = 1;

		Ref<IDXGISwapChain1> tempSwapChain;
		dxgiFactory->CreateSwapChainForHwnd(commandQueue.Get(), window, &scDesc, nullptr, nullptr, tempSwapChain.GetAddressOf());
		tempSwapChain->QueryInterface(IID_PPV_ARGS(swapChain.GetAddressOf()));

		D3D12DescriptorAllocatorDesc rtvAllocatorDesc{};
		rtvAllocatorDesc.descriptorCount = bufferCount;
		rtvAllocatorDesc.type = D3D12DescriptorHeapType::RTV;
		rtvAllocatorDesc.shaderVisible = false;
		rtvDescriptorAllocator = std::make_unique<D3D12DescriptorAllocator>(device, rtvAllocatorDesc);

		D3D12DescriptorAllocatorDesc gpuVisibleDesc{};
		gpuVisibleDesc.descriptorCount = bufferCount + 1;
		gpuVisibleDesc.type = D3D12DescriptorHeapType::CBV_SRV_UAV;
		gpuVisibleDesc.shaderVisible = true;
		gpuVisibleAllocator = std::make_unique<D3D12DescriptorAllocator>(device, gpuVisibleDesc);
		
		backBuffers.resize(bufferCount);
		backBufferDescriptors.resize(bufferCount);
		for (Uint32 i = 0; i < bufferCount; ++i)
		{
			D3D12Descriptor rtvHandle = rtvDescriptorAllocator->GetHandle(i);
			swapChain->GetBuffer(i, IID_PPV_ARGS(backBuffers[i].GetAddressOf()));
			device->CreateRenderTargetView(backBuffers[i].Get(), nullptr, rtvHandle);
			backBufferDescriptors[i] = rtvHandle;
			backBuffers[i]->SetName(L"VistaBackbuffer");
		}

		commandAllocators.resize(bufferCount);
		for (Uint32 i = 0; i < bufferCount; ++i)
		{
			device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocators[i].GetAddressOf()));
		}
		device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[0].Get(), nullptr, IID_PPV_ARGS(commandList.GetAddressOf()));
		commandList->Close(); 

		frameFence.Create(device, "Frame Fence (Vista)");
		for (Uint32 i = 0; i < bufferCount; ++i)
		{
			fenceValues[i] = 0;
		}
		nextFenceValue = 1;
		bufferIndex = swapChain->GetCurrentBackBufferIndex();
		return true;
	}

	Bool ImGuiManager::ResizeSwapChain(Uint32 newWidth, Uint32 newHeight)
	{
		if (newWidth == 0 || newHeight == 0)
		{
			return false;
		}

		Flush();

		for (auto& buffer : backBuffers)
		{
			buffer.Reset();
		}
		backBuffers.clear();
		backBufferDescriptors.clear();

		DXGI_SWAP_CHAIN_DESC1 scDesc{};
		swapChain->GetDesc1(&scDesc);
		HRESULT hr = swapChain->ResizeBuffers(bufferCount, newWidth, newHeight, scDesc.Format, scDesc.Flags);
		if (FAILED(hr))
		{
			return false;
		}

		windowWidth = newWidth;
		windowHeight = newHeight;

		backBuffers.resize(bufferCount);
		backBufferDescriptors.resize(bufferCount);
		for (Uint32 i = 0; i < bufferCount; ++i)
		{
			D3D12Descriptor rtvHandle = rtvDescriptorAllocator->GetHandle(i);
			swapChain->GetBuffer(i, IID_PPV_ARGS(backBuffers[i].GetAddressOf()));
			device->CreateRenderTargetView(backBuffers[i].Get(), nullptr, rtvHandle);
			backBufferDescriptors[i] = rtvHandle;
			backBuffers[i]->SetName(L"VistaBackbuffer");
		}
		bufferIndex = swapChain->GetCurrentBackBufferIndex(); 

		return true;
	}

	void ImGuiManager::DestroyWindowAndSwapChain()
	{
		if (window)
		{
			DestroyWindow(window);
			window = nullptr;
		}

		UnregisterClass(L"VistaWindowClass", GetModuleHandle(nullptr));

		Flush();
		gpuVisibleAllocator.reset();
		rtvDescriptorAllocator.reset();
		swapChain = nullptr;
		commandQueue = nullptr;
		commandList = nullptr;

		commandAllocators.clear();
		backBuffers.clear();
		backBufferDescriptors.clear();
	}

	void ImGuiManager::SetStyle()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4* colors = style.Colors;

		style.Alpha = 1.0f;
		style.DisabledAlpha = 0.6f;
		style.WindowPadding = ImVec2(5.5f, 8.3f);
		style.WindowRounding = 4.5f;
		style.WindowBorderSize = 1.0f;
		style.WindowMinSize = ImVec2(32.0f, 32.0f);
		style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
		style.WindowMenuButtonPosition = ImGuiDir_Left;
		style.ChildRounding = 3.2f;
		style.ChildBorderSize = 1.0f;
		style.PopupRounding = 2.7f;
		style.PopupBorderSize = 1.0f;
		style.FramePadding = ImVec2(4.0f, 3.0f);
		style.FrameRounding = 2.4f;
		style.FrameBorderSize = 0.0f;
		style.ItemSpacing = ImVec2(8.0f, 4.0f);
		style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
		style.CellPadding = ImVec2(4.0f, 2.0f);
		style.IndentSpacing = 21.0f;
		style.ColumnsMinSpacing = 6.0f;
		style.ScrollbarSize = 14.0f;
		style.ScrollbarRounding = 9.0f;
		style.GrabMinSize = 10.0f;
		style.GrabRounding = 3.2f;
		style.TabRounding = 3.5f;
		style.TabBorderSize = 1.0f;
		style.TabMinWidthForCloseButton = 0.0f;
		style.ColorButtonPosition = ImGuiDir_Right;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.14f, 0.17f, 0.23f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.21f, 0.25f, 0.30f, 0.40f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.04f, 0.05f, 0.05f, 0.67f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.72f, 0.78f, 0.84f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.48f, 0.53f, 0.57f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.29f, 0.32f, 0.35f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.15f, 0.16f, 0.18f, 0.40f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.14f, 0.15f, 0.16f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.08f, 0.09f, 0.09f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.20f, 0.22f, 0.24f, 0.31f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.16f, 0.18f, 0.19f, 0.80f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.07f, 0.08f, 0.09f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.24f, 0.33f, 0.42f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.27f, 0.38f, 0.50f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.29f, 0.33f, 0.38f, 0.20f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.24f, 0.30f, 0.37f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.16f, 0.18f, 0.19f, 0.95f);
		colors[ImGuiCol_Tab] = ImVec4(0.12f, 0.13f, 0.13f, 0.86f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.41f, 0.50f, 0.80f);
		colors[ImGuiCol_TabActive] = ImVec4(0.24f, 0.25f, 0.25f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.26f, 0.42f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	}

	void ImGuiManager::Flush()
	{
		commandQueue->Signal(frameFence, nextFenceValue);
		frameFence.Wait(nextFenceValue);
		nextFenceValue++;
	}

	void ImGuiManager::Shutdown()
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		DestroyWindowAndSwapChain();
		initialized = false;
		s_ImGuiManagerInstance = nullptr; 
	}

	Bool ImGuiManager::BeginFrame()
	{
		if (!initialized)
		{
			return false;
		}

		if (needsResize)
		{
			Bool resizeSuccess = ResizeSwapChain(windowWidth, windowHeight);
			if (resizeSuccess)
			{
				needsResize = false;
			}
			else
			{
				return false;
			}
		}

		frameFence.Wait(fenceValues[bufferIndex]);
		commandAllocators[bufferIndex]->Reset();
		commandList->Reset(commandAllocators[bufferIndex].Get(), nullptr);

		// Transition back buffer to RENDER_TARGET state
		D3D12_RESOURCE_BARRIER barrierToRT = {};
		barrierToRT.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrierToRT.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrierToRT.Transition.pResource = backBuffers[bufferIndex].Get();
		barrierToRT.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrierToRT.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrierToRT.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &barrierToRT);

		FLOAT Black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		commandList->ClearRenderTargetView(backBufferDescriptors[bufferIndex], Black, 0, nullptr);

		ImGui_ImplWin32_NewFrame();
		ImGui_ImplDX12_NewFrame();
		ImGui::NewFrame();
		return true;
	}

	void ImGuiManager::EndFrame()
	{
		if (!initialized)
		{
			return;
		}

		ImGui::Render();
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		commandList->OMSetRenderTargets(1, &backBufferDescriptors[bufferIndex], FALSE, nullptr);
		ID3D12DescriptorHeap* heaps[] = { gpuVisibleAllocator->GetHeap() };
		commandList->SetDescriptorHeaps(_countof(heaps), heaps);

		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());

		D3D12_RESOURCE_BARRIER barrierToPresent = {};
		barrierToPresent.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrierToPresent.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrierToPresent.Transition.pResource = backBuffers[bufferIndex].Get();
		barrierToPresent.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrierToPresent.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		barrierToPresent.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &barrierToPresent);

		commandList->Close();
		ID3D12CommandList* cmdLists[] = { commandList.Get() };
		commandQueue->ExecuteCommandLists(1, cmdLists);

		swapChain->Present(1, 0);

		Uint64 signalValue = nextFenceValue++;
		commandQueue->Signal(frameFence, signalValue);
		fenceValues[bufferIndex] = signalValue;

		bufferIndex = swapChain->GetCurrentBackBufferIndex();
	}

	D3D12Descriptor ImGuiManager::GetDescriptorForThisFrame() const
	{
		return gpuVisibleAllocator->GetHandle(1 + bufferIndex);
	}

	LRESULT ImGuiManager::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_SIZE:
		{
			if (initialized && wParam != SIZE_MINIMIZED)
			{
				Uint32 newWidth = LOWORD(lParam);
				Uint32 newHeight = HIWORD(lParam);
				if (newWidth != windowWidth || newHeight != windowHeight)
				{
					if ((newWidth != 0) && (newHeight != 0))
					{
						needsResize = true;
						windowWidth = newWidth;
						windowHeight = newHeight;
					}
				}
				needsResize = true;
			}
			return 0; 
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}