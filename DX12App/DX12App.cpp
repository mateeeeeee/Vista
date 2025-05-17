#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include "d3dx12.h"
#include <stdlib.h>
#include <string>
#include <vector>
#include <algorithm>
#include "WinPixEventRuntime/Include/WinPixEventRuntime/pix3.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

const char* const ShaderString =
"struct VertexShaderOutput\n"
"{\n"
"    float4 position : SV_POSITION;\n"
"    float2 uv : TEXCOORD;\n"
"};\n"
"\n"
"Texture2D myTexture : register(t0);\n"
"SamplerState mySampler : register(s0);\n"
"\n"
"VertexShaderOutput DefaultVertexShader(float4 position : POSITION, float2 uv : TEXCOORD)\n"
"{\n"
"    VertexShaderOutput output;\n"
"    output.position = position;\n"
"    output.uv = uv;\n"
"    return output;\n"
"}\n"
"\n"
"float4 DefaultFragmentShader(VertexShaderOutput input) : SV_TARGET\n"
"{\n"
"    return myTexture.Sample(mySampler, input.uv);\n"
"}\n";

static const wchar_t* const ClassName = L"TEXTURED_TRIANGLE_D3D12";
static const wchar_t* const WindowTitle = L"Hello Textured Triangle D3D12";
static const unsigned int WindowWidth = 960U;
static const unsigned int WindowHeight = 544U;
static const unsigned int BackBufferCount = 3U;
static const unsigned int TextureWidth = 128;
static const unsigned int TextureHeight = 128;
static const unsigned int TexturePixelSize = 4;
static const DXGI_FORMAT TextureFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

LRESULT WINAPI WindowCallback(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			PostMessage(wnd, WM_CLOSE, 0U, 0U);
		return 0;
	}
	return DefWindowProc(wnd, msg, wParam, lParam);
}

std::vector<uint8_t> GenerateCheckerboardTexture(int width, int height, int pixelSize, int squareSize)
{
	std::vector<uint8_t> data(width * height * pixelSize);
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			int checkerX = x / squareSize;
			int checkerY = y / squareSize;
			uint8_t color = (((checkerX + checkerY) % 2) == 0) ? 255 : 0;
			int index = (y * width + x) * pixelSize;
			data[index + 0] = color;
			data[index + 1] = color;
			data[index + 2] = color;
			data[index + 3] = 255;
		}
	}
	return data;
}

int WinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
	LoadLibrary(L"Vista.dll");

	ID3D12Device* device = nullptr;
	IDXGISwapChain* swapChain = nullptr;
	ID3D12Fence* frameFences[BackBufferCount] = {};
	UINT64 fenceValues[BackBufferCount] = {};
	HANDLE frameFenceEvents[BackBufferCount] = {};
	UINT64 currentFenceValue = 1U;
	ID3D12Resource* renderTargets[BackBufferCount] = {};
	ID3D12DescriptorHeap* renderTargetDescriptorHeap = nullptr;
	UINT renderTargetViewDescriptorSize = 0;
	ID3D12CommandQueue* commandQueue = nullptr;
	ID3D12CommandAllocator* commandAllocators[BackBufferCount] = {};
	ID3D12GraphicsCommandList* commandLists[BackBufferCount] = {};
	unsigned int currentBackBuffer = 0U;

	ID3D12RootSignature* rootSignature = nullptr;
	ID3D12PipelineState* pso = nullptr;
	ID3D12DescriptorHeap* resourceDescriptorHeap = nullptr;
	ID3D12DescriptorHeap* samplerDescriptorHeap = nullptr;

	D3D12_VIEWPORT viewport = { 0.0f, 0.0f, (FLOAT)WindowWidth, (FLOAT)WindowHeight, 0.0f, 1.0f };
	D3D12_RECT scissorRect = { 0, 0, (LONG)WindowWidth, (LONG)WindowHeight };

	ID3D12Resource* vertexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
	ID3D12Resource* indexBuffer = nullptr;
	D3D12_INDEX_BUFFER_VIEW indexBufferView = {};

	ID3D12Resource* intermediateRenderTarget = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE intermediateRTVHandle = {};

	HWND hwnd;
	WNDCLASSEX wndClass = {};
	wndClass.cbSize = sizeof(wndClass);
	wndClass.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	wndClass.hbrBackground = NULL;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hInstance = instance;
	wndClass.lpfnWndProc = &WindowCallback;
	wndClass.lpszClassName = ClassName;
	RegisterClassEx(&wndClass);

	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE;
	DWORD exStyle = 0U;
	RECT rect = { 0, 0, (LONG)WindowWidth, (LONG)WindowHeight };
	AdjustWindowRectEx(&rect, style, FALSE, exStyle);

	hwnd = CreateWindowEx(
		exStyle, ClassName, WindowTitle, style,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top,
		NULL, NULL, instance, NULL
	);

	HRESULT result = D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
	if (FAILED(result)) { OutputDebugStringA("Failed to create D3D12 device!\n"); return EXIT_FAILURE; }

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	result = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));
	if (FAILED(result)) { OutputDebugStringA("Failed to create command queue!\n"); return EXIT_FAILURE; }

	IDXGIFactory4* dxgiFactory = nullptr;
	result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = WindowWidth;
	sd.BufferDesc.Height = WindowHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.SampleDesc.Count = 1U;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = BackBufferCount;
	sd.OutputWindow = hwnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
	result = dxgiFactory->CreateSwapChain(commandQueue, &sd, &swapChain);
	dxgiFactory->Release();
	if (FAILED(result)) { OutputDebugStringA("Failed to create swap chain!\n"); return EXIT_FAILURE; }

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = BackBufferCount + 1; // +1 for intermediate render target
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	result = device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&renderTargetDescriptorHeap));
	if (FAILED(result)) { OutputDebugStringA("Failed to create RTV heap!\n"); return EXIT_FAILURE; }

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(renderTargetDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	renderTargetViewDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	for (unsigned int i = 0; i < BackBufferCount; ++i)
	{
		D3D12_RENDER_TARGET_VIEW_DESC viewDesc = {};
		viewDesc.Format = sd.BufferDesc.Format;
		viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
		device->CreateRenderTargetView(renderTargets[i], &viewDesc, rtvHandle);
		rtvHandle.Offset(renderTargetViewDescriptorSize);
	}
	intermediateRTVHandle = rtvHandle; // Store for intermediate render target

	D3D12_DESCRIPTOR_HEAP_DESC resHeapDesc = {};
	resHeapDesc.NumDescriptors = 1;
	resHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	resHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	result = device->CreateDescriptorHeap(&resHeapDesc, IID_PPV_ARGS(&resourceDescriptorHeap));
	if (FAILED(result)) { OutputDebugStringA("Failed to create Resource heap!\n"); return EXIT_FAILURE; }

	D3D12_DESCRIPTOR_HEAP_DESC samplerHeapDesc = {};
	samplerHeapDesc.NumDescriptors = 1;
	samplerHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	samplerHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	result = device->CreateDescriptorHeap(&samplerHeapDesc, IID_PPV_ARGS(&samplerDescriptorHeap));
	if (FAILED(result)) { OutputDebugStringA("Failed to create Sampler heap!\n"); return EXIT_FAILURE; }

	static const D3D12_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

	CD3DX12_DESCRIPTOR_RANGE ranges[2];
	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

	CD3DX12_ROOT_PARAMETER parameters[2];
	parameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);
	parameters[1].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);

	CD3DX12_ROOT_SIGNATURE_DESC descRootSignature;
	descRootSignature.Init(2, parameters, 0, NULL, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ID3DBlob* rootBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	result = D3D12SerializeRootSignature(&descRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &rootBlob, &errorBlob);
	if (FAILED(result))
	{
		if (errorBlob) { OutputDebugStringA((char*)errorBlob->GetBufferPointer()); errorBlob->Release(); }
		OutputDebugStringA("Failed to serialize root signature!\n");
		return EXIT_FAILURE;
	}
	result = device->CreateRootSignature(0, rootBlob->GetBufferPointer(), rootBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	rootBlob->Release();
	if (FAILED(result)) { OutputDebugStringA("Failed to create root signature!\n"); return EXIT_FAILURE; }

	ID3DBlob* vertexShader = nullptr;
	result = D3DCompile(ShaderString, strlen(ShaderString), NULL, NULL, NULL, "DefaultVertexShader", "vs_5_0", 0, 0, &vertexShader, &errorBlob);
	if (FAILED(result))
	{
		if (errorBlob) { OutputDebugStringA((char*)errorBlob->GetBufferPointer()); errorBlob->Release(); }
		OutputDebugStringA("Failed to compile VS!\n"); return EXIT_FAILURE;
	}

	ID3DBlob* pixelShader = nullptr;
	result = D3DCompile(ShaderString, strlen(ShaderString), NULL, NULL, NULL, "DefaultFragmentShader", "ps_5_0", 0, 0, &pixelShader, &errorBlob);
	if (FAILED(result))
	{
		if (errorBlob) { OutputDebugStringA((char*)errorBlob->GetBufferPointer()); errorBlob->Release(); }
		OutputDebugStringA("Failed to compile PS!\n"); return EXIT_FAILURE;
	}

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.VS = { vertexShader->GetBufferPointer(), vertexShader->GetBufferSize() };
	psoDesc.PS = { pixelShader->GetBufferPointer(), pixelShader->GetBufferSize() };
	psoDesc.pRootSignature = rootSignature;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = TextureFormat;
	psoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
	psoDesc.InputLayout = { layout, _countof(layout) };
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.SampleDesc.Count = 1;
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	result = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso));
	if (FAILED(result)) { OutputDebugStringA("Failed to create PSO!\n"); return EXIT_FAILURE; }

	struct Vertex
	{
		float position[3];
		float uv[2];
	};
	static const Vertex vertices[] =
	{
		{{ 0.0f,  0.5f, 0.0f}, { 0.5f, 0.0f}}, // Top
		{{ 0.5f, -0.5f, 0.0f}, { 1.0f, 1.0f}}, // Bottom-right
		{{-0.5f, -0.5f, 0.0f}, { 0.0f, 1.0f}}  // Bottom-left
	};
	static const UINT64 vertexBufferSize = sizeof(vertices);

	static const unsigned int indices[] = { 0, 1, 2 };
	static const UINT64 indexBufferSize = sizeof(indices);

	static const CD3DX12_HEAP_PROPERTIES defaultHeapProperties(D3D12_HEAP_TYPE_DEFAULT);
	static const CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);

	static const CD3DX12_RESOURCE_DESC vertexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
	result = device->CreateCommittedResource(
		&defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &vertexBufferDesc,
		D3D12_RESOURCE_STATE_COPY_DEST, NULL, IID_PPV_ARGS(&vertexBuffer)
	);
	if (FAILED(result)) { OutputDebugStringA("Failed to create vertex buffer!\n"); return EXIT_FAILURE; }
	vertexBuffer->SetName(L"VertexBuffer");

	static const CD3DX12_RESOURCE_DESC indexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);
	result = device->CreateCommittedResource(
		&defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &indexBufferDesc,
		D3D12_RESOURCE_STATE_COPY_DEST, NULL, IID_PPV_ARGS(&indexBuffer)
	);
	if (FAILED(result)) { OutputDebugStringA("Failed to create index buffer!\n"); return EXIT_FAILURE; }
	indexBuffer->SetName(L"IndexBuffer");

	static const CD3DX12_RESOURCE_DESC vertexUploadBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
	ID3D12Resource* vertexUploadHeap = nullptr;
	result = device->CreateCommittedResource(
		&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &vertexUploadBufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&vertexUploadHeap)
	);
	if (FAILED(result)) { OutputDebugStringA("Failed to create vertex upload heap!\n"); return EXIT_FAILURE; }
	vertexUploadHeap->SetName(L"VertexUploadHeap");

	static const CD3DX12_RESOURCE_DESC indexUploadBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);
	ID3D12Resource* indexUploadHeap = nullptr;
	result = device->CreateCommittedResource(
		&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &indexUploadBufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&indexUploadHeap)
	);
	if (FAILED(result)) { OutputDebugStringA("Failed to create index upload heap!\n"); return EXIT_FAILURE; }
	indexUploadHeap->SetName(L"IndexUploadHeap");

	std::vector<uint8_t> textureData = GenerateCheckerboardTexture(TextureWidth, TextureHeight, TexturePixelSize, 16);
	ID3D12Resource* textureResource = nullptr;
	ID3D12Resource* textureUploadHeap = nullptr;
	static const CD3DX12_RESOURCE_DESC textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		TextureFormat, TextureWidth, TextureHeight, 1, 1
	);
	result = device->CreateCommittedResource(
		&defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &textureDesc,
		D3D12_RESOURCE_STATE_COPY_DEST, NULL, IID_PPV_ARGS(&textureResource)
	);
	if (FAILED(result)) { OutputDebugStringA("Failed to create texture resource!\n"); return EXIT_FAILURE; }
	textureResource->SetName(L"TextureResource");

	D3D12_RESOURCE_DESC textureResourceDesc = textureResource->GetDesc();
	UINT64 textureUploadSize;
	device->GetCopyableFootprints(&textureResourceDesc, 0, 1, 0, NULL, NULL, NULL, &textureUploadSize);
	static const CD3DX12_RESOURCE_DESC textureUploadBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(textureUploadSize);
	result = device->CreateCommittedResource(
		&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &textureUploadBufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&textureUploadHeap)
	);
	if (FAILED(result)) { OutputDebugStringA("Failed to create texture upload heap!\n"); return EXIT_FAILURE; }
	textureUploadHeap->SetName(L"TextureUploadHeap");

	static const CD3DX12_RESOURCE_DESC intermediateTextureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		TextureFormat, WindowWidth, WindowHeight, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);
	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = TextureFormat;
	clearValue.Color[0] = 0.1f;
	clearValue.Color[1] = 0.2f;
	clearValue.Color[2] = 0.3f;
	clearValue.Color[3] = 1.0f;
	result = device->CreateCommittedResource(
		&defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &intermediateTextureDesc,
		D3D12_RESOURCE_STATE_RENDER_TARGET, &clearValue, IID_PPV_ARGS(&intermediateRenderTarget)
	);
	if (FAILED(result)) { OutputDebugStringA("Failed to create intermediate render target!\n"); return EXIT_FAILURE; }
	intermediateRenderTarget->SetName(L"IntermediateRenderTarget");

	D3D12_RENDER_TARGET_VIEW_DESC intermediateRTVDesc = {};
	intermediateRTVDesc.Format = TextureFormat;
	intermediateRTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	device->CreateRenderTargetView(intermediateRenderTarget, &intermediateRTVDesc, intermediateRTVHandle);

	ID3D12CommandAllocator* uploadCommandAllocator = nullptr;
	result = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&uploadCommandAllocator));
	if (FAILED(result)) { OutputDebugStringA("Failed to create upload command allocator!\n"); return EXIT_FAILURE; }
	ID3D12GraphicsCommandList* uploadCommandList = nullptr;
	result = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, uploadCommandAllocator, NULL, IID_PPV_ARGS(&uploadCommandList));
	if (FAILED(result)) { OutputDebugStringA("Failed to create upload command list!\n"); return EXIT_FAILURE; }

	PIXBeginEvent(uploadCommandList, 0xFF90EE90, "Initial Resource Upload");

	void* pVertexData = nullptr;
	result = vertexUploadHeap->Map(0, NULL, &pVertexData);
	if (FAILED(result)) { OutputDebugStringA("Failed to map vertex upload heap!\n"); return EXIT_FAILURE; }
	memcpy(pVertexData, vertices, sizeof(vertices));
	vertexUploadHeap->Unmap(0, NULL);

	void* pIndexData = nullptr;
	result = indexUploadHeap->Map(0, NULL, &pIndexData);
	if (FAILED(result)) { OutputDebugStringA("Failed to map index upload heap!\n"); return EXIT_FAILURE; }
	memcpy(pIndexData, indices, sizeof(indices));
	indexUploadHeap->Unmap(0, NULL);

	uploadCommandList->CopyBufferRegion(vertexBuffer, 0, vertexUploadHeap, 0, vertexBufferSize);
	uploadCommandList->CopyBufferRegion(indexBuffer, 0, indexUploadHeap, 0, indexBufferSize);

	D3D12_SUBRESOURCE_DATA subresourceData = {};
	subresourceData.pData = textureData.data();
	subresourceData.RowPitch = TextureWidth * TexturePixelSize;
	subresourceData.SlicePitch = subresourceData.RowPitch * TextureHeight;
	UpdateSubresources(uploadCommandList, textureResource, textureUploadHeap, 0, 0, 1, &subresourceData);

	CD3DX12_RESOURCE_BARRIER barriers[3] = {
		CD3DX12_RESOURCE_BARRIER::Transition(vertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER),
		CD3DX12_RESOURCE_BARRIER::Transition(indexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER),
		CD3DX12_RESOURCE_BARRIER::Transition(textureResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
	};
	uploadCommandList->ResourceBarrier(_countof(barriers), barriers);

	PIXEndEvent(uploadCommandList);
	uploadCommandList->Close();

	ID3D12CommandList* uploadCommandLists[] = { uploadCommandList };
	commandQueue->ExecuteCommandLists(1, uploadCommandLists);

	ID3D12Fence* uploadFence = nullptr;
	result = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&uploadFence));
	if (FAILED(result)) { OutputDebugStringA("Failed to create upload fence!\n"); return EXIT_FAILURE; }
	const UINT64 uploadFenceValue = 1U;
	commandQueue->Signal(uploadFence, uploadFenceValue);

	HANDLE waitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!waitEvent) { OutputDebugStringA("Failed to create wait event!\n"); return EXIT_FAILURE; }
	if (uploadFence->GetCompletedValue() < uploadFenceValue)
	{
		result = uploadFence->SetEventOnCompletion(uploadFenceValue, waitEvent);
		if (FAILED(result)) { OutputDebugStringA("Failed to set upload fence event!\n"); return EXIT_FAILURE; }
		WaitForSingleObject(waitEvent, INFINITE);
	}

	vertexUploadHeap->Release();
	indexUploadHeap->Release();
	textureUploadHeap->Release();
	CloseHandle(waitEvent);
	uploadFence->Release();
	uploadCommandList->Release();
	uploadCommandAllocator->Release();

	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = vertexBufferSize;
	vertexBufferView.StrideInBytes = sizeof(Vertex);

	indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = indexBufferSize;
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = TextureFormat;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(resourceDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	device->CreateShaderResourceView(textureResource, &srvDesc, srvHandle);

	D3D12_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	memcpy(samplerDesc.BorderColor, borderColor, sizeof(borderColor));
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	CD3DX12_CPU_DESCRIPTOR_HANDLE samplerHandle(samplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	device->CreateSampler(&samplerDesc, samplerHandle);

	for (unsigned int i = 0; i < BackBufferCount; ++i)
	{
		frameFenceEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (!frameFenceEvents[i]) { OutputDebugStringA("Failed to create frame fence event!\n"); return EXIT_FAILURE; }
		fenceValues[i] = 0;
		result = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&frameFences[i]));
		if (FAILED(result)) { OutputDebugStringA("Failed to create frame fence!\n"); return EXIT_FAILURE; }
		result = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators[i]));
		if (FAILED(result)) { OutputDebugStringA("Failed to create command allocator!\n"); return EXIT_FAILURE; }
		result = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[i], NULL, IID_PPV_ARGS(&commandLists[i]));
		if (FAILED(result)) { OutputDebugStringA("Failed to create command list!\n"); return EXIT_FAILURE; }
		commandLists[i]->Close();
	}

	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (frameFences[currentBackBuffer]->GetCompletedValue() < fenceValues[currentBackBuffer])
			{
				result = frameFences[currentBackBuffer]->SetEventOnCompletion(fenceValues[currentBackBuffer], frameFenceEvents[currentBackBuffer]);
				if (FAILED(result)) { OutputDebugStringA("Failed to set fence event on completion!\n"); return EXIT_FAILURE; }
				WaitForSingleObject(frameFenceEvents[currentBackBuffer], INFINITE);
			}

			result = commandAllocators[currentBackBuffer]->Reset();
			if (FAILED(result)) { OutputDebugStringA("Failed to reset command allocator!\n"); return EXIT_FAILURE; }
			ID3D12GraphicsCommandList* commandList = commandLists[currentBackBuffer];
			result = commandList->Reset(commandAllocators[currentBackBuffer], pso);
			if (FAILED(result)) { OutputDebugStringA("Failed to reset command list!\n"); return EXIT_FAILURE; }

			PIXScopedEvent(commandList, 0xFF00FF00, "Render Frame");

			// Render to intermediate texture
			{
				PIXScopedEvent(commandList, 0xFFFFA500, "Render to Intermediate Texture");
				commandList->OMSetRenderTargets(1, &intermediateRTVHandle, TRUE, NULL);
				commandList->RSSetViewports(1, &viewport);
				commandList->RSSetScissorRects(1, &scissorRect);

				static const float ClearColor[] = { 0.1f, 0.2f, 0.3f, 1.0f };
				commandList->ClearRenderTargetView(intermediateRTVHandle, ClearColor, 0, NULL);

				commandList->SetPipelineState(pso);
				commandList->SetGraphicsRootSignature(rootSignature);

				ID3D12DescriptorHeap* descriptorHeaps[] = { resourceDescriptorHeap, samplerDescriptorHeap };
				commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

				CD3DX12_GPU_DESCRIPTOR_HANDLE srvGpuHandle(resourceDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
				CD3DX12_GPU_DESCRIPTOR_HANDLE samplerGpuHandle(samplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
				commandList->SetGraphicsRootDescriptorTable(0, srvGpuHandle);
				commandList->SetGraphicsRootDescriptorTable(1, samplerGpuHandle);

				commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
				commandList->IASetIndexBuffer(&indexBufferView);

				PIXScopedEvent(commandList, 0xFF0000FF, "Draw Textured Triangle");
				commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
			}

			// Copy to swapchain
			{
				PIXScopedEvent(commandList, 0xFFFF4500, "Copy to Swapchain");

				D3D12_CPU_DESCRIPTOR_HANDLE renderTargetHandle;
				CD3DX12_CPU_DESCRIPTOR_HANDLE::InitOffsetted(
					renderTargetHandle,
					renderTargetDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
					currentBackBuffer,
					renderTargetViewDescriptorSize
				);

				CD3DX12_RESOURCE_BARRIER barriers[2] = {
					CD3DX12_RESOURCE_BARRIER::Transition(intermediateRenderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE),
					CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[currentBackBuffer], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_DEST)
				};
				commandList->ResourceBarrier(2, barriers);

				commandList->CopyResource(renderTargets[currentBackBuffer], intermediateRenderTarget);

				barriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(intermediateRenderTarget, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
				barriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[currentBackBuffer], D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PRESENT);
				commandList->ResourceBarrier(2, barriers);
			}

			commandList->Close();
			ID3D12CommandList* renderCommandLists[] = { commandList };
			commandQueue->ExecuteCommandLists(1, renderCommandLists);

			swapChain->Present(1, 0);

			const UINT64 fenceValue = currentFenceValue;
			result = commandQueue->Signal(frameFences[currentBackBuffer], fenceValue);
			if (FAILED(result)) { OutputDebugStringA("Failed to signal fence!\n"); return EXIT_FAILURE; }
			fenceValues[currentBackBuffer] = fenceValue;

			++currentFenceValue;
			currentBackBuffer = (currentBackBuffer + 1) % BackBufferCount;
		}
	}

	for (unsigned int i = 0; i < BackBufferCount; ++i)
	{
		if (frameFences[i]->GetCompletedValue() < fenceValues[i])
		{
			result = frameFences[i]->SetEventOnCompletion(fenceValues[i], frameFenceEvents[i]);
			if (FAILED(result)) { OutputDebugStringA("Failed to set final fence event!\n"); }
			WaitForSingleObject(frameFenceEvents[i], INFINITE);
		}
	}

	vertexBuffer->Release();
	indexBuffer->Release();
	textureResource->Release();
	intermediateRenderTarget->Release();
	resourceDescriptorHeap->Release();
	samplerDescriptorHeap->Release();
	rootSignature->Release();
	vertexShader->Release();
	pixelShader->Release();
	commandQueue->Release();
	for (unsigned int i = 0; i < BackBufferCount; ++i)
	{
		renderTargets[i]->Release();
		commandLists[i]->Release();
		commandAllocators[i]->Release();
		CloseHandle(frameFenceEvents[i]);
		frameFences[i]->Release();
	}
	pso->Release();
	renderTargetDescriptorHeap->Release();
	device->Release();
	swapChain->Release();

	return 0;
}