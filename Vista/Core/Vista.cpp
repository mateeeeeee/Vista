#include "Vista.h"
#include "Hook/Hooks.h"
#include "Hook/GlobalHooks.h"
#include "Command/Commands.h"
#include "Utilities/RTTI.h"
#include "Logging/DebuggerSink.h"
#include "Logging/PipeSink.h"

namespace vista
{
	static thread_local Bool g_isInsideVistaRender = false;
	struct VistaRenderGuard
	{
		VistaRenderGuard() { g_isInsideVistaRender = true; }
		~VistaRenderGuard() { g_isInsideVistaRender = false; }
		VISTA_NONCOPYABLE_NONMOVABLE(VistaRenderGuard)
	};
	#define VISTA_RENDER_GUARD VistaRenderGuard VISTA_CONCAT(__vistaRenderGuard, __COUNTER__)

	void Vista::InitializeD3D12Hooks()
	{
		if (DH_Status dh = DH_Initialize(); dh == DH_Status::Success)
		{
#define VISTA_HOOK(Category, Name) Hook(d3d12::##Category##_##Name, d3d12PFNs.##Name, hooks::##Name)
#define VISTA_HOOK2(Category, Name) Hook(d3d12::##Category##_##Name, d3d12PFNs.##Category##_##Name, hooks::##Name)
			VISTA_HOOK(SwapChain, Present);
			VISTA_HOOK(Device, CreateCommandQueue);
			VISTA_HOOK(Device9, CreateCommandQueue1);
			VISTA_HOOK(Device, CreateCommandList);
			VISTA_HOOK(Device4, CreateCommandList1);
			VISTA_HOOK(Device, CreateCommandAllocator);
			VISTA_HOOK(Device, CreateFence);
			VISTA_HOOK(Device, CreateHeap);
			VISTA_HOOK(Device4, CreateHeap1);
			VISTA_HOOK(Device, CreateRootSignature);
			VISTA_HOOK(Device, CreateCommandSignature);
			VISTA_HOOK(Device, CreateGraphicsPipelineState);
			VISTA_HOOK(Device, CreateComputePipelineState);
			VISTA_HOOK(Device2, CreatePipelineState);
			VISTA_HOOK(Device5, CreateStateObject);
			VISTA_HOOK(Device7, AddToStateObject);
			VISTA_HOOK(Device, CreateCommittedResource);
			VISTA_HOOK(Device4, CreateCommittedResource1);
			VISTA_HOOK(Device, CreatePlacedResource);
			VISTA_HOOK(Device8, CreatePlacedResource1);
			VISTA_HOOK(Device, CreateDescriptorHeap);
			VISTA_HOOK(Device, CreateRenderTargetView);
			VISTA_HOOK(Device, CreateDepthStencilView);
			VISTA_HOOK(Device, CreateShaderResourceView);
			VISTA_HOOK(Device, CreateUnorderedAccessView);
			VISTA_HOOK(Device, CreateConstantBufferView);
			VISTA_HOOK(Device, CreateSampler);
			VISTA_HOOK(Device, CopyDescriptorsSimple);
			VISTA_HOOK(Device, CopyDescriptors);
			VISTA_HOOK(Queue, ExecuteCommandLists);
			VISTA_HOOK2(Queue, Signal);
			VISTA_HOOK(List, CopyBufferRegion);
			VISTA_HOOK(List, CopyTextureRegion);
			VISTA_HOOK(List, CopyResource);
			VISTA_HOOK(List, ResolveSubresource);
			VISTA_HOOK(List1, ResolveSubresourceRegion);
			VISTA_HOOK(List, DrawInstanced);
			VISTA_HOOK(List, DrawIndexedInstanced);
			VISTA_HOOK(List, Dispatch);
			VISTA_HOOK(List6, DispatchMesh);
			VISTA_HOOK(List, ExecuteIndirect);
			VISTA_HOOK(List4, DispatchRays);
			VISTA_HOOK(List, RSSetViewports);
			VISTA_HOOK(List, RSSetScissorRects);
			VISTA_HOOK(List5, RSSetShadingRate);
			VISTA_HOOK(List5, RSSetShadingRateImage);
			VISTA_HOOK(List1, SetViewInstanceMask);
			VISTA_HOOK(List, OMSetRenderTargets);
			VISTA_HOOK(List, OMSetBlendFactor);
			VISTA_HOOK(List, OMSetStencilRef);
			VISTA_HOOK(List1, OMSetDepthBounds);
			VISTA_HOOK(List4, BeginRenderPass);
			VISTA_HOOK(List4, EndRenderPass);
			VISTA_HOOK(List, ClearRenderTargetView);
			VISTA_HOOK(List, ClearDepthStencilView);
			VISTA_HOOK(List, ClearUnorderedAccessViewUint);
			VISTA_HOOK(List, ClearUnorderedAccessViewFloat);
			VISTA_HOOK(List, SetPipelineState);
			VISTA_HOOK(List4, SetPipelineState1);
			VISTA_HOOK(List, SetGraphicsRootSignature);
			VISTA_HOOK(List, SetComputeRootSignature);
			VISTA_HOOK(List, IASetPrimitiveTopology);
			VISTA_HOOK(List, IASetVertexBuffers);
			VISTA_HOOK(List, IASetIndexBuffer);
			VISTA_HOOK(List, ResourceBarrier);
			VISTA_HOOK(List, SetDescriptorHeaps);
			VISTA_HOOK(List, SetGraphicsRootDescriptorTable);
			VISTA_HOOK(List, SetComputeRootDescriptorTable);
			VISTA_HOOK(List, SetGraphicsRoot32BitConstant);
			VISTA_HOOK(List, SetComputeRoot32BitConstant);
			VISTA_HOOK(List, SetGraphicsRoot32BitConstants);
			VISTA_HOOK(List, SetComputeRoot32BitConstants);
			VISTA_HOOK(List, SetGraphicsRootConstantBufferView);
			VISTA_HOOK(List, SetComputeRootConstantBufferView);
			VISTA_HOOK(List, SetGraphicsRootShaderResourceView);
			VISTA_HOOK(List, SetComputeRootShaderResourceView);
			VISTA_HOOK(List, SetGraphicsRootUnorderedAccessView);
			VISTA_HOOK(List, SetComputeRootUnorderedAccessView);
			VISTA_HOOK(List, Close);
			VISTA_HOOK2(List, Reset);
			VISTA_HOOK(List, ClearState);
			VISTA_HOOK(List, BeginEvent);
			VISTA_HOOK(List, EndEvent);
			VISTA_HOOK2(Resource, Release);
			VISTA_HOOK2(Resource, SetName);
			VISTA_HOOK2(Resource, Map);
			VISTA_HOOK2(Resource, Unmap);
			VISTA_HOOK(Fence, GetCompletedValue);
			VISTA_HOOK(Fence, SetEventOnCompletion);
			VISTA_HOOK2(Fence, Signal);
#undef VISTA_HOOK2
#undef VISTA_HOOK
		}
	}

	Bool Vista::Initialize()
	{
		VISTA_SINK(DebuggerSink);
		return InitializeGlobalHooks();
	}

	void Vista::Shutdown()
	{
		DH_Shutdown();
	}

	void Vista::OnDeviceCreated(ID3D12Device* pDevice)
	{
		objectTracker.TrackObject(pDevice);
		stateTracker.Initialize(pDevice);
		addressTracker.Initialize(pDevice);
		copyRequestManager.Initialize(pDevice);
		InitializeD3D12Hooks();
		if (!GUI.Initialize(pDevice))
		{
			VISTA_LOG(ERROR, "GUI initialization failed!");
		}
	}

	HRESULT Vista::OnPresent(IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags)
	{
		if (g_isInsideVistaRender)
		{
			return d3d12PFNs.Present(pSwapChain, syncInterval, flags);
		}

		static Bool backbuffersCaptured = false;
		if (!backbuffersCaptured)
		{
			DXGI_SWAP_CHAIN_DESC swapChainDesc{};
			HRESULT hr = pSwapChain->GetDesc(&swapChainDesc);
			if (SUCCEEDED(hr))
			{
				UINT bufferCount = swapChainDesc.BufferCount;
				for (UINT i = 0; i < bufferCount; ++i)
				{
					Ref<ID3D12Resource> backBuffer;
					hr = pSwapChain->GetBuffer(i, IID_PPV_ARGS(backBuffer.GetAddressOf()));
					if (SUCCEEDED(hr))
					{
						ResourceDesc resourceDesc{};
						resourceDesc.heapProperties = D3D12_HEAP_PROPERTIES{}; 
						resourceDesc.heapFlags = D3D12_HEAP_FLAG_NONE; 
						resourceDesc.resourceDesc = backBuffer->GetDesc(); 
						resourceDesc.initialResourceState = D3D12_RESOURCE_STATE_PRESENT; 
						objectTracker.TrackObject<ID3D12Resource>(backBuffer.Get(), resourceDesc);
					}
				}
			}
			backbuffersCaptured = true;
		}

		VISTA_RENDER_GUARD;
		if (GUI.BeginRender())
		{
			GUI.Render(recorderManager);
			GUI.EndRender();
		}

		if (!GUI.IsFreezed())
		{
			copyRequestManager.RefreshRequestsState();
			recorderManager.Clear();
		}
		return d3d12PFNs.Present(pSwapChain, syncInterval, flags);
	}

	HRESULT Vista::OnCreateCommandQueue(ID3D12Device* pDevice, const D3D12_COMMAND_QUEUE_DESC* pDesc, REFIID riid, void** ppCommandQueue)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CreateCommandQueue(pDevice, pDesc, riid, ppCommandQueue);
		}

		CommandRecorder& queueRecorder = recorderManager.GetOrCreateRecorder(pDevice);
		CreateCommandQueueCommand& cmd = queueRecorder.AddCommand<CreateCommandQueueCommand>();

		HRESULT hr = d3d12PFNs.CreateCommandQueue(pDevice, pDesc, riid, ppCommandQueue);
		cmd.hr = hr;
		if (SUCCEEDED(hr) && ppCommandQueue && *ppCommandQueue)
		{
			QueueDesc desc{ .queueDesc = pDesc ? *pDesc : D3D12_COMMAND_QUEUE_DESC{} };
			cmd.commandQueueId = objectTracker.TrackObject<ID3D12CommandQueue>(*ppCommandQueue, desc);
		}
		return hr;
	}

	HRESULT Vista::OnCreateCommandQueue1(ID3D12Device9* pDevice, const D3D12_COMMAND_QUEUE_DESC* pDesc, REFIID creatorID, REFIID riid, void** ppCommandQueue)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CreateCommandQueue1(pDevice, pDesc, creatorID, riid, ppCommandQueue);
		}

		CommandRecorder& queueRecorder = recorderManager.GetOrCreateRecorder(pDevice);
		CreateCommandQueue1Command& cmd = queueRecorder.AddCommand<CreateCommandQueue1Command>();

		HRESULT hr = d3d12PFNs.CreateCommandQueue1(pDevice, pDesc, creatorID, riid, ppCommandQueue);
		cmd.hr = hr;
		if (SUCCEEDED(hr) && ppCommandQueue && *ppCommandQueue)
		{
			QueueDesc desc{ .queueDesc = pDesc ? *pDesc : D3D12_COMMAND_QUEUE_DESC{} };
			cmd.commandQueueId = objectTracker.TrackObject<ID3D12CommandQueue>(*ppCommandQueue, desc);
		}
		return hr;
	}

	HRESULT Vista::OnCreateCommandList(ID3D12Device* pDevice, UINT nodeMask, D3D12_COMMAND_LIST_TYPE type, ID3D12CommandAllocator* pCommandAllocator, ID3D12PipelineState* pInitialState, REFIID riid, void** ppCommandList)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CreateCommandList(pDevice, nodeMask, type, pCommandAllocator, pInitialState, riid, ppCommandList);
		}

		CommandRecorder& queueRecorder = recorderManager.GetOrCreateRecorder(pDevice);
		CreateCommandListCommand& cmd = queueRecorder.AddCommand<CreateCommandListCommand>();

		HRESULT hr = d3d12PFNs.CreateCommandList(pDevice, nodeMask, type, pCommandAllocator, pInitialState, riid, ppCommandList);
		cmd.hr = hr;
		if (SUCCEEDED(hr) && ppCommandList && *ppCommandList)
		{
			ListDesc listDesc{};
			listDesc.nodeMask = nodeMask;
			listDesc.type = type;
			listDesc.commandAllocatorId = objectTracker.GetObjectID(pCommandAllocator);
			listDesc.initialStateId = objectTracker.GetObjectID(pInitialState);
			cmd.commandListId = objectTracker.TrackObject<ID3D12CommandList>(*ppCommandList, listDesc);
		}
		return hr;
	}

	HRESULT Vista::OnCreateCommandList1(ID3D12Device4* pDevice, UINT nodeMask, D3D12_COMMAND_LIST_TYPE type, D3D12_COMMAND_LIST_FLAGS flags, REFIID riid, void** ppCommandList)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CreateCommandList1(pDevice, nodeMask, type, flags, riid, ppCommandList);
		}

		CommandRecorder& queueRecorder = recorderManager.GetOrCreateRecorder(pDevice);
		CreateCommandList1Command& cmd = queueRecorder.AddCommand<CreateCommandList1Command>();

		HRESULT hr = d3d12PFNs.CreateCommandList1(pDevice, nodeMask, type, flags, riid, ppCommandList);
		cmd.hr = hr;
		if (SUCCEEDED(hr) && ppCommandList && *ppCommandList)
		{
			ListDesc listDesc{};
			listDesc.nodeMask = nodeMask;
			listDesc.type = type;
			listDesc.flags = flags;
			cmd.commandListId = objectTracker.TrackObject<ID3D12CommandList>(*ppCommandList, listDesc);
		}
		return hr;
	}

	HRESULT Vista::OnCreateCommandAllocator(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE type, REFIID riid, void** ppCommandAllocator)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CreateCommandAllocator(pDevice, type, riid, ppCommandAllocator);
		}

		CommandRecorder& queueRecorder = recorderManager.GetOrCreateRecorder(pDevice);
		CreateCommandAllocatorCommand& cmd = queueRecorder.AddCommand<CreateCommandAllocatorCommand>();

		HRESULT hr = d3d12PFNs.CreateCommandAllocator(pDevice, type, riid, ppCommandAllocator);
		cmd.hr = hr;
		if (SUCCEEDED(hr) && ppCommandAllocator && *ppCommandAllocator)
		{
			AllocatorDesc allocatorDesc{};
			allocatorDesc.type = type;
			cmd.commandAllocatorId = objectTracker.TrackObject<ID3D12CommandAllocator>(*ppCommandAllocator, allocatorDesc);
		}
		return hr;
	}

	HRESULT Vista::OnCreateHeap(ID3D12Device* pDevice, const D3D12_HEAP_DESC* pDesc, REFIID riid, void** ppvHeap)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CreateHeap(pDevice, pDesc, riid, ppvHeap);
		}
		CommandRecorder& queueRecorder = recorderManager.GetOrCreateRecorder(pDevice);
		CreateHeapCommand& cmd = queueRecorder.AddCommand<CreateHeapCommand>();
		HRESULT hr = d3d12PFNs.CreateHeap(pDevice, pDesc, riid, ppvHeap);
		cmd.hr = hr;
		if (SUCCEEDED(hr) && ppvHeap && *ppvHeap)
		{
			HeapDesc heapDesc{ .heapDesc = pDesc ? *pDesc : D3D12_HEAP_DESC {} };
			cmd.heapId = objectTracker.TrackObject<ID3D12Heap>(*ppvHeap, heapDesc);
		}
		return hr;
	}

	HRESULT Vista::OnCreateHeap1(ID3D12Device4* pDevice, const D3D12_HEAP_DESC* pDesc, ID3D12ProtectedResourceSession* pProtectedSession, REFIID riid, void** ppvHeap)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CreateHeap1(pDevice, pDesc, pProtectedSession, riid, ppvHeap);
		}
		CommandRecorder& queueRecorder = recorderManager.GetOrCreateRecorder(pDevice);
		CreateHeap1Command& cmd = queueRecorder.AddCommand<CreateHeap1Command>();
		HRESULT hr = d3d12PFNs.CreateHeap1(pDevice, pDesc, pProtectedSession, riid, ppvHeap);
		cmd.hr = hr;
		if (SUCCEEDED(hr) && ppvHeap && *ppvHeap)
		{
			HeapDesc heapDesc{ .heapDesc = pDesc ? *pDesc : D3D12_HEAP_DESC {} };
			cmd.heapId = objectTracker.TrackObject<ID3D12Heap>(*ppvHeap, heapDesc);
		}
		return hr;
	}

	HRESULT Vista::OnCreateFence(ID3D12Device* pDevice, UINT64 intialValue, D3D12_FENCE_FLAGS flags, REFIID riid, void** ppFence)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CreateFence(pDevice, intialValue, flags, riid, ppFence);
		}

		CommandRecorder& queueRecorder = recorderManager.GetOrCreateRecorder(pDevice);
		CreateFenceCommand& cmd = queueRecorder.AddCommand<CreateFenceCommand>();
		HRESULT hr = d3d12PFNs.CreateFence(pDevice, intialValue, flags, riid, ppFence);
		cmd.hr = hr;
		if (SUCCEEDED(hr) && ppFence && *ppFence)
		{
			FenceDesc fenceDesc{};
			fenceDesc.intialValue = intialValue;
			fenceDesc.flags = flags;
			cmd.fenceId = objectTracker.TrackObject<ID3D12Fence>(*ppFence, fenceDesc);
		}
		return hr;
	}

	HRESULT Vista::OnCreateGraphicsPipelineState(ID3D12Device* pDevice, const D3D12_GRAPHICS_PIPELINE_STATE_DESC* pDesc, REFIID riid, void** ppPipelineState)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CreateGraphicsPipelineState(pDevice, pDesc, riid, ppPipelineState);
		}

		CommandRecorder& queueRecorder = recorderManager.GetOrCreateRecorder(pDevice);
		CreateGraphicsPipelineStateCommand& cmd = queueRecorder.AddCommand<CreateGraphicsPipelineStateCommand>();
		HRESULT hr = d3d12PFNs.CreateGraphicsPipelineState(pDevice, pDesc, riid, ppPipelineState);
		cmd.hr = hr;
		if (SUCCEEDED(hr) && ppPipelineState && *ppPipelineState)
		{
			GraphicsPSODescStorage gfxPSODescStorage(*pDesc, objectTracker);
			PSODesc psoDesc = gfxPSODescStorage;
			cmd.psoId = objectTracker.TrackObject<ID3D12PipelineState>(*ppPipelineState, psoDesc);
		}
		return hr;
	}

	HRESULT Vista::OnCreateComputePipelineState(ID3D12Device* pDevice, const D3D12_COMPUTE_PIPELINE_STATE_DESC* pDesc, REFIID riid, void** ppPipelineState)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CreateComputePipelineState(pDevice, pDesc, riid, ppPipelineState);
		}

		CommandRecorder& queueRecorder = recorderManager.GetOrCreateRecorder(pDevice);
		CreateComputePipelineStateCommand& cmd = queueRecorder.AddCommand<CreateComputePipelineStateCommand>();
		HRESULT hr = d3d12PFNs.CreateComputePipelineState(pDevice, pDesc, riid, ppPipelineState);
		cmd.hr = hr;
		if (SUCCEEDED(hr) && ppPipelineState && *ppPipelineState)
		{
			ComputePSODescStorage gfxPSODescStorage(*pDesc, objectTracker);
			PSODesc psoDesc = gfxPSODescStorage;
			cmd.psoId = objectTracker.TrackObject<ID3D12PipelineState>(*ppPipelineState, psoDesc);
		}
		return hr;
	}

	HRESULT Vista::OnCreatePipelineState(ID3D12Device2* pDevice, const D3D12_PIPELINE_STATE_STREAM_DESC* pDesc, REFIID riid, void** ppPipelineState)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CreatePipelineState(pDevice, pDesc, riid, ppPipelineState);
		}

		CommandRecorder& queueRecorder = recorderManager.GetOrCreateRecorder(pDevice);
		CreatePipelineStateCommand& cmd = queueRecorder.AddCommand<CreatePipelineStateCommand>();
		HRESULT hr = d3d12PFNs.CreatePipelineState(pDevice, pDesc, riid, ppPipelineState);
		cmd.hr = hr;
		if (SUCCEEDED(hr) && ppPipelineState && *ppPipelineState)
		{
			StreamPSODescStorage gfxPSODescStorage(*pDesc, objectTracker);
			PSODesc psoDesc = gfxPSODescStorage;
			cmd.psoId = objectTracker.TrackObject<ID3D12PipelineState>(*ppPipelineState, psoDesc);
		}
		return hr;
	}
	
	HRESULT Vista::OnCreateStateObject(ID3D12Device5* pDevice, D3D12_STATE_OBJECT_DESC const* pDesc, REFIID riid, void** ppStateObject)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CreateStateObject(pDevice, pDesc, riid, ppStateObject);
		}

		CommandRecorder& queueRecorder = recorderManager.GetOrCreateRecorder(pDevice);
		CreateStateObjectCommand& cmd = queueRecorder.AddCommand<CreateStateObjectCommand>();
		HRESULT const hr = d3d12PFNs.CreateStateObject(pDevice, pDesc, riid, ppStateObject);
		cmd.hr = hr;

		if (SUCCEEDED(hr) && ppStateObject && *ppStateObject)
		{
			StateObjectDesc stateObjectDesc{};

			auto GetSubobjectDescSize = [](D3D12_STATE_SUBOBJECT_TYPE const type) -> Uint64
				{
					switch (type)
					{
					case D3D12_STATE_SUBOBJECT_TYPE_STATE_OBJECT_CONFIG:
						return sizeof(D3D12_STATE_OBJECT_CONFIG);
					case D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE:
					case D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE:
						return 0;
					case D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY:
						return sizeof(D3D12_DXIL_LIBRARY_DESC);
					case D3D12_STATE_SUBOBJECT_TYPE_EXISTING_COLLECTION:
						return sizeof(D3D12_EXISTING_COLLECTION_DESC);
					case D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION:
						return sizeof(D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION);
					case D3D12_STATE_SUBOBJECT_TYPE_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION:
						return sizeof(D3D12_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION);
					case D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG:
						return sizeof(D3D12_RAYTRACING_SHADER_CONFIG);
					case D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG:
						return sizeof(D3D12_RAYTRACING_PIPELINE_CONFIG);
					case D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG1:
						return sizeof(D3D12_RAYTRACING_PIPELINE_CONFIG1);
					case D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP:
						return sizeof(D3D12_HIT_GROUP_DESC);
					case D3D12_STATE_SUBOBJECT_TYPE_NODE_MASK:
						return sizeof(UINT);
					default:
						return 0;
					}
				};

			stateObjectDesc.type = pDesc->Type;
			stateObjectDesc.subobjects.reserve(pDesc->NumSubobjects);

			for (Uint32 i = 0; i < pDesc->NumSubobjects; ++i)
			{
				D3D12_STATE_SUBOBJECT const& src = pDesc->pSubobjects[i];
				StateObjectDesc::Subobject dst;
				dst.type = src.Type;

				switch (src.Type)
				{
				case D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE:
				case D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE:
				{
					ID3D12RootSignature* const* ppRootSig = static_cast<ID3D12RootSignature* const*>(src.pDesc);
					if (ppRootSig && *ppRootSig)
					{
						dst.interfaceRef = *ppRootSig;
					}
					break;
				}
				case D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP:
				{
					D3D12_HIT_GROUP_DESC const* desc = static_cast<D3D12_HIT_GROUP_DESC const*>(src.pDesc);
					dst.hitGroupType = desc->Type;
					if (desc->HitGroupExport) dst.hitGroupName = desc->HitGroupExport;
					if (desc->AnyHitShaderImport) dst.anyHitShader = desc->AnyHitShaderImport;
					if (desc->ClosestHitShaderImport) dst.closestHitShader = desc->ClosestHitShaderImport;
					if (desc->IntersectionShaderImport) dst.intersectionShader = desc->IntersectionShaderImport;
					break;
				}
				case D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION:
				{
					D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION const* desc = static_cast<D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION const*>(src.pDesc);
					dst.ownedAssociationExportNames.reserve(desc->NumExports);
					dst.ownedAssociationExportPointers.resize(desc->NumExports);
					for (Uint32 j = 0; j < desc->NumExports; ++j)
					{
						dst.ownedAssociationExportNames.push_back(desc->pExports[j] ? desc->pExports[j] : L"");
					}
					dst.associatedSubobjectIndex = -1;
					dst.originalAssociationExportsPtr = desc->pExports;
					dst.originalAssociatedSubobjectPtr = desc->pSubobjectToAssociate;
					break;
				}
				case D3D12_STATE_SUBOBJECT_TYPE_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION:
				{
					D3D12_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION const* desc = static_cast<D3D12_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION const*>(src.pDesc);
					dst.ownedAssociationExportNames.reserve(desc->NumExports);
					dst.ownedAssociationExportPointers.resize(desc->NumExports);
					for (Uint32 j = 0; j < desc->NumExports; ++j)
					{
						dst.ownedAssociationExportNames.push_back(desc->pExports[j] ? desc->pExports[j] : L"");
					}
					if (desc->SubobjectToAssociate) dst.associatedSubobjectName = desc->SubobjectToAssociate;
					dst.originalAssociationExportsPtr = desc->pExports;
					break;
				}
				case D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY:
				{
					D3D12_DXIL_LIBRARY_DESC const* desc = static_cast<D3D12_DXIL_LIBRARY_DESC const*>(src.pDesc);
					dst.ownedExportStructs.resize(desc->NumExports);
					dst.ownedExportNames.reserve(desc->NumExports);
					for (Uint32 j = 0; j < desc->NumExports; ++j)
					{
						D3D12_EXPORT_DESC const& src_exp = desc->pExports[j];
						dst.ownedExportNames.push_back(src_exp.Name ? src_exp.Name : L"");
						dst.ownedExportStructs[j] = src_exp;
					}
					break;
				}
				case D3D12_STATE_SUBOBJECT_TYPE_EXISTING_COLLECTION:
				{
					D3D12_EXISTING_COLLECTION_DESC const* desc = static_cast<D3D12_EXISTING_COLLECTION_DESC const*>(src.pDesc);
					if (desc->pExistingCollection)
					{
						dst.interfaceRef = desc->pExistingCollection;
					}
					dst.ownedExportStructs.resize(desc->NumExports);
					dst.ownedExportNames.reserve(desc->NumExports);
					for (Uint32 j = 0; j < desc->NumExports; ++j)
					{
						D3D12_EXPORT_DESC const& src_exp = desc->pExports[j];
						dst.ownedExportNames.push_back(src_exp.Name ? src_exp.Name : L"");
						dst.ownedExportStructs[j] = src_exp;
					}
					break;
				}
				default:
				{
					Uint64 size = GetSubobjectDescSize(src.Type);
					if (size != 0)
					{
						dst.data.resize(size);
						memcpy(dst.data.data(), src.pDesc, size);
					}
					break;
				}
				}
				stateObjectDesc.subobjects.push_back(std::move(dst));
			}
			for (Uint32 i = 0; i < stateObjectDesc.subobjects.size(); ++i)
			{
				StateObjectDesc::Subobject& dst = stateObjectDesc.subobjects[i];
				switch (dst.type)
				{
				case D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION:
				{
					for (Uint32 j = 0; j < dst.ownedAssociationExportNames.size(); ++j)
					{
						dst.ownedAssociationExportPointers[j] = dst.ownedAssociationExportNames[j].c_str();
					}
					if (dst.originalAssociatedSubobjectPtr)
					{
						for (Uint32 k = 0; k < pDesc->NumSubobjects; ++k)
						{
							if (&pDesc->pSubobjects[k] == dst.originalAssociatedSubobjectPtr)
							{
								dst.associatedSubobjectIndex = static_cast<Int32>(k);
								break;
							}
						}
					}
					dst.originalAssociationExportsPtr = nullptr;
					dst.originalAssociatedSubobjectPtr = nullptr;
					break;
				}
				case D3D12_STATE_SUBOBJECT_TYPE_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION:
				{
					for (Uint32 j = 0; j < dst.ownedAssociationExportNames.size(); ++j)
					{
						dst.ownedAssociationExportPointers[j] = dst.ownedAssociationExportNames[j].c_str();
					}
					dst.originalAssociationExportsPtr = nullptr;
					break;
				}
				case D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY:
				case D3D12_STATE_SUBOBJECT_TYPE_EXISTING_COLLECTION:
				{
					for (Uint32 j = 0; j < dst.ownedExportStructs.size(); ++j)
					{
						dst.ownedExportStructs[j].Name = dst.ownedExportNames[j].c_str();
					}
					break;
				}
				default:
					break;
				}
			}
			cmd.stateObjectId = objectTracker.TrackObject<ID3D12StateObject>(*ppStateObject, stateObjectDesc);
		}
		return hr;
	}
	
	HRESULT Vista::OnAddToStateObject(ID3D12Device7* pDevice, const D3D12_STATE_OBJECT_DESC* pAddition, ID3D12StateObject* pStateObjectToGrowFrom, REFIID riid, void** ppNewStateObject)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.AddToStateObject(pDevice, pAddition, pStateObjectToGrowFrom, riid, ppNewStateObject);
		}

		CommandRecorder& queueRecorder = recorderManager.GetOrCreateRecorder(pDevice);
		AddToStateObjectCommand& cmd = queueRecorder.AddCommand<AddToStateObjectCommand>();

		HRESULT hr = d3d12PFNs.AddToStateObject(pDevice, pAddition, pStateObjectToGrowFrom, riid, ppNewStateObject);
		cmd.hr = hr;
		if (SUCCEEDED(hr) && ppNewStateObject && *ppNewStateObject)
		{
			StateObjectDesc newStateObjectDesc{};
			Int32 baseSubobjectCount = 0;

			TrackedObjectInfo const* baseInfo = objectTracker.GetObjectInfo(pStateObjectToGrowFrom);
			if (baseInfo && std::holds_alternative<StateObjectDesc>(baseInfo->objectDesc))
			{
				auto const& baseDesc = std::get<StateObjectDesc>(baseInfo->objectDesc);
				newStateObjectDesc.type = baseDesc.type; 
				newStateObjectDesc.subobjects = baseDesc.subobjects; 
				baseSubobjectCount = static_cast<Int32>(newStateObjectDesc.subobjects.size());

				cmd.existingStateObjectId = baseInfo->objectId;
			}
			else
			{
				newStateObjectDesc.type = pAddition->Type; 
				baseSubobjectCount = 0; 
			}

			auto GetSubobjectDescSize = [](D3D12_STATE_SUBOBJECT_TYPE const type) -> Uint64
				{
					switch (type)
					{
					case D3D12_STATE_SUBOBJECT_TYPE_STATE_OBJECT_CONFIG:
						return sizeof(D3D12_STATE_OBJECT_CONFIG);
					case D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE:
					case D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE:
						return 0;
					case D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY:
						return sizeof(D3D12_DXIL_LIBRARY_DESC);
					case D3D12_STATE_SUBOBJECT_TYPE_EXISTING_COLLECTION:
						return sizeof(D3D12_EXISTING_COLLECTION_DESC);
					case D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION:
						return sizeof(D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION);
					case D3D12_STATE_SUBOBJECT_TYPE_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION:
						return sizeof(D3D12_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION);
					case D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG:
						return sizeof(D3D12_RAYTRACING_SHADER_CONFIG);
					case D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG:
						return sizeof(D3D12_RAYTRACING_PIPELINE_CONFIG);
					case D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG1:
						return sizeof(D3D12_RAYTRACING_PIPELINE_CONFIG1);
					case D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP:
						return sizeof(D3D12_HIT_GROUP_DESC);
					case D3D12_STATE_SUBOBJECT_TYPE_NODE_MASK:
						return sizeof(UINT);
					default:
						return 0;
					}
				};

			for (Uint32 i = 0; i < pAddition->NumSubobjects; ++i)
			{
				D3D12_STATE_SUBOBJECT const& src = pAddition->pSubobjects[i];
				StateObjectDesc::Subobject dst;
				dst.type = src.Type;

				switch (src.Type)
				{
				case D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE:
				case D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE:
				{
					ID3D12RootSignature* const* ppRootSig = static_cast<ID3D12RootSignature* const*>(src.pDesc);
					if (ppRootSig && *ppRootSig)
					{
						dst.interfaceRef = *ppRootSig;
					}
					break;
				}
				case D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP:
				{
					D3D12_HIT_GROUP_DESC const* desc = static_cast<D3D12_HIT_GROUP_DESC const*>(src.pDesc);
					dst.hitGroupType = desc->Type;
					if (desc->HitGroupExport) dst.hitGroupName = desc->HitGroupExport;
					if (desc->AnyHitShaderImport) dst.anyHitShader = desc->AnyHitShaderImport;
					if (desc->ClosestHitShaderImport) dst.closestHitShader = desc->ClosestHitShaderImport;
					if (desc->IntersectionShaderImport) dst.intersectionShader = desc->IntersectionShaderImport;
					break;
				}
				case D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION:
				{
					D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION const* desc = static_cast<D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION const*>(src.pDesc);
					dst.ownedAssociationExportNames.reserve(desc->NumExports);
					dst.ownedAssociationExportPointers.resize(desc->NumExports);
					for (Uint32 j = 0; j < desc->NumExports; ++j)
					{
						dst.ownedAssociationExportNames.push_back(desc->pExports[j] ? desc->pExports[j] : L"");
					}
					dst.associatedSubobjectIndex = -1;
					dst.originalAssociatedSubobjectPtr = desc->pSubobjectToAssociate; 
					break;
				}
				case D3D12_STATE_SUBOBJECT_TYPE_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION:
				{
					D3D12_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION const* desc = static_cast<D3D12_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION const*>(src.pDesc);
					dst.ownedAssociationExportNames.reserve(desc->NumExports);
					dst.ownedAssociationExportPointers.resize(desc->NumExports);
					for (Uint32 j = 0; j < desc->NumExports; ++j)
					{
						dst.ownedAssociationExportNames.push_back(desc->pExports[j] ? desc->pExports[j] : L"");
					}
					if (desc->SubobjectToAssociate) dst.associatedSubobjectName = desc->SubobjectToAssociate;
					break;
				}
				case D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY:
				{
					D3D12_DXIL_LIBRARY_DESC const* desc = static_cast<D3D12_DXIL_LIBRARY_DESC const*>(src.pDesc);
					dst.ownedExportStructs.resize(desc->NumExports);
					dst.ownedExportNames.reserve(desc->NumExports);
					for (Uint32 j = 0; j < desc->NumExports; ++j)
					{
						D3D12_EXPORT_DESC const& src_exp = desc->pExports[j];
						dst.ownedExportNames.push_back(src_exp.Name ? src_exp.Name : L"");
						dst.ownedExportStructs[j] = src_exp;
					}
					break;
				}
				case D3D12_STATE_SUBOBJECT_TYPE_EXISTING_COLLECTION:
				{
					D3D12_EXISTING_COLLECTION_DESC const* desc = static_cast<D3D12_EXISTING_COLLECTION_DESC const*>(src.pDesc);
					if (desc->pExistingCollection)
					{
						dst.interfaceRef = desc->pExistingCollection;
					}
					dst.ownedExportStructs.resize(desc->NumExports);
					dst.ownedExportNames.reserve(desc->NumExports);
					for (Uint32 j = 0; j < desc->NumExports; ++j)
					{
						D3D12_EXPORT_DESC const& src_exp = desc->pExports[j];
						dst.ownedExportNames.push_back(src_exp.Name ? src_exp.Name : L"");
						dst.ownedExportStructs[j] = src_exp;
					}
					break;
				}
				default:
				{
					Uint64 size = GetSubobjectDescSize(src.Type);
					if (size != 0)
					{
						dst.data.resize(size);
						memcpy(dst.data.data(), src.pDesc, size);
					}
					break;
				}
				}
				newStateObjectDesc.subobjects.push_back(std::move(dst));
			}

			for (Uint32 i = baseSubobjectCount; i < newStateObjectDesc.subobjects.size(); ++i) 
			{
				StateObjectDesc::Subobject& dst = newStateObjectDesc.subobjects[i];
				const D3D12_STATE_SUBOBJECT& originalAddedSrc = pAddition->pSubobjects[i - baseSubobjectCount];

				switch (dst.type)
				{
				case D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION:
				{
					for (Uint32 j = 0; j < dst.ownedAssociationExportNames.size(); ++j)
					{
						dst.ownedAssociationExportPointers[j] = dst.ownedAssociationExportNames[j].c_str();
					}

					if (dst.originalAssociatedSubobjectPtr)
					{
						for (Uint32 k = 0; k < newStateObjectDesc.subobjects.size(); ++k)
						{
							Int32 originalAdditionIndex = -1;
							for (Uint32 l = 0; l < pAddition->NumSubobjects; ++l)
							{
								if (&pAddition->pSubobjects[l] == dst.originalAssociatedSubobjectPtr)
								{
									originalAdditionIndex = static_cast<Int32>(l);
									break;
								}
							}
							if (originalAdditionIndex != -1)
							{
								dst.associatedSubobjectIndex = baseSubobjectCount + originalAdditionIndex;
							}
						}
					}
					dst.originalAssociatedSubobjectPtr = nullptr; 
					break;
				}
				case D3D12_STATE_SUBOBJECT_TYPE_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION:
				{
					for (Uint32 j = 0; j < dst.ownedAssociationExportNames.size(); ++j)
					{
						dst.ownedAssociationExportPointers[j] = dst.ownedAssociationExportNames[j].c_str();
					}
					break;
				}
				case D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY:
				case D3D12_STATE_SUBOBJECT_TYPE_EXISTING_COLLECTION:
				{
					for (Uint32 j = 0; j < dst.ownedExportStructs.size(); ++j)
					{
						dst.ownedExportStructs[j].Name = dst.ownedExportNames[j].c_str();
					}
					break;
				}
				}
			}
			cmd.stateObjectId = objectTracker.TrackObject<ID3D12StateObject>(*ppNewStateObject, newStateObjectDesc);
		}
		return hr;
	}

	HRESULT Vista::OnCreateRootSignature(ID3D12Device* pDevice, UINT nodeMask, const void* pBlobWithRootSignature, SIZE_T blobLengthInBytes, REFIID riid, void** ppvRootSignature)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CreateRootSignature(pDevice, nodeMask, pBlobWithRootSignature, blobLengthInBytes, riid, ppvRootSignature);
		}

		CommandRecorder& queueRecorder = recorderManager.GetOrCreateRecorder(pDevice);
		CreateRootSignatureCommand& cmd = queueRecorder.AddCommand<CreateRootSignatureCommand>();
		HRESULT hr = d3d12PFNs.CreateRootSignature(pDevice, nodeMask, pBlobWithRootSignature, blobLengthInBytes, riid, ppvRootSignature);
		cmd.hr = hr;
		if (SUCCEEDED(hr) && ppvRootSignature && *ppvRootSignature)
		{
			ID3D12RootSignatureDeserializer* rootSignatureDeserializer = nullptr;
			D3D12CreateRootSignatureDeserializer(pBlobWithRootSignature, blobLengthInBytes, IID_PPV_ARGS(&rootSignatureDeserializer));
			D3D12_ROOT_SIGNATURE_DESC const* d3d12RootSignatureDesc = rootSignatureDeserializer->GetRootSignatureDesc();
			RootSignatureDesc rootSignatureDesc(d3d12RootSignatureDesc ? *d3d12RootSignatureDesc : D3D12_ROOT_SIGNATURE_DESC{});
			cmd.rootSignatureId = objectTracker.TrackObject<ID3D12RootSignature>(*ppvRootSignature, rootSignatureDesc);
		}
		return hr;
	}

	HRESULT Vista::OnCreateCommandSignature(ID3D12Device* pDevice, const D3D12_COMMAND_SIGNATURE_DESC* pDesc, ID3D12RootSignature* pRootSignature, REFIID riid, void** ppvCommandSignature)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CreateCommandSignature(pDevice, pDesc, pRootSignature, riid, ppvCommandSignature);
		}

		CommandRecorder& queueRecorder = recorderManager.GetOrCreateRecorder(pDevice);
		CreateCommandSignatureCommand& cmd = queueRecorder.AddCommand<CreateCommandSignatureCommand>();
		HRESULT hr = d3d12PFNs.CreateCommandSignature(pDevice, pDesc, pRootSignature, riid, ppvCommandSignature);
		cmd.hr = hr;
		if (SUCCEEDED(hr) && ppvCommandSignature && *ppvCommandSignature)
		{
			CommandSignatureDesc cmdSignatureDesc{ .cmdSignatureDesc = *pDesc, .rootSignatureId = objectTracker.GetObjectID(pRootSignature) };
			cmd.cmdSignatureId = objectTracker.TrackObject<ID3D12CommandSignature>(*ppvCommandSignature, cmdSignatureDesc);
		}
		return hr;
	}

	HRESULT Vista::OnCreateCommittedResource(ID3D12Device* pDevice, const D3D12_HEAP_PROPERTIES* pHeapProperties, D3D12_HEAP_FLAGS heapFlags, const D3D12_RESOURCE_DESC* pDesc, D3D12_RESOURCE_STATES initialResourceState, const D3D12_CLEAR_VALUE* pOptimizedClearValue, REFIID riid, void** ppvResource)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CreateCommittedResource(pDevice, pHeapProperties, heapFlags, pDesc, initialResourceState, pOptimizedClearValue, riid, ppvResource);
		}

		CommandRecorder& queueRecorder = recorderManager.GetOrCreateRecorder(pDevice);
		CreateCommittedResourceCommand& cmd = queueRecorder.AddCommand<CreateCommittedResourceCommand>();
		HRESULT hr = d3d12PFNs.CreateCommittedResource(pDevice, pHeapProperties, heapFlags, pDesc, initialResourceState, pOptimizedClearValue, riid, ppvResource);
		cmd.hr = hr;
		if (SUCCEEDED(hr) && ppvResource && *ppvResource)
		{
			ResourceDesc resourceDesc{};
			resourceDesc.heapProperties = pHeapProperties ? *pHeapProperties : D3D12_HEAP_PROPERTIES{};
			resourceDesc.heapFlags = heapFlags;
			resourceDesc.resourceDesc = pDesc ? *pDesc : D3D12_RESOURCE_DESC{};
			resourceDesc.initialResourceState = initialResourceState;
			if (pOptimizedClearValue)
			{
				resourceDesc.optimizedClearValue = *pOptimizedClearValue;
			}
			cmd.resourceId = objectTracker.TrackObject<ID3D12Resource>(*ppvResource, resourceDesc);

			stateTracker.OnResourceCreated((ID3D12Resource*)*ppvResource, resourceDesc.initialResourceState);
			addressTracker.OnResourceCreated((ID3D12Resource*)*ppvResource, resourceDesc.resourceDesc);
			mappedBufferManager.OnResourceCreated((ID3D12Resource*)*ppvResource, resourceDesc.resourceDesc);
		}
		return hr;
	}

	HRESULT Vista::OnCreateCommittedResource1(ID3D12Device4* pDevice, const D3D12_HEAP_PROPERTIES* pHeapProperties, D3D12_HEAP_FLAGS heapFlags, const D3D12_RESOURCE_DESC* pDesc, D3D12_RESOURCE_STATES initialResourceState, const D3D12_CLEAR_VALUE* pOptimizedClearValue, ID3D12ProtectedResourceSession* pProtectedSession, REFIID riid, void** ppvResource)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CreateCommittedResource1(pDevice, pHeapProperties, heapFlags, pDesc, initialResourceState, pOptimizedClearValue, pProtectedSession, riid, ppvResource);
		}

		CommandRecorder& queueRecorder = recorderManager.GetOrCreateRecorder(pDevice);
		CreateCommittedResource1Command& cmd = queueRecorder.AddCommand<CreateCommittedResource1Command>();
		HRESULT hr = d3d12PFNs.CreateCommittedResource1(pDevice, pHeapProperties, heapFlags, pDesc, initialResourceState, pOptimizedClearValue, pProtectedSession, riid, ppvResource);
		cmd.hr = hr;
		if (SUCCEEDED(hr) && ppvResource && *ppvResource)
		{
			ResourceDesc resourceDesc{};
			resourceDesc.heapProperties = pHeapProperties ? *pHeapProperties : D3D12_HEAP_PROPERTIES{};
			resourceDesc.heapFlags = heapFlags;
			resourceDesc.resourceDesc = pDesc ? *pDesc : D3D12_RESOURCE_DESC{};
			resourceDesc.initialResourceState = initialResourceState;
			if (pOptimizedClearValue)
			{
				resourceDesc.optimizedClearValue = *pOptimizedClearValue;
			}
			cmd.resourceId = objectTracker.TrackObject<ID3D12Resource>(*ppvResource, resourceDesc);

			stateTracker.OnResourceCreated((ID3D12Resource*)*ppvResource, resourceDesc.initialResourceState);
			addressTracker.OnResourceCreated((ID3D12Resource*)*ppvResource, resourceDesc.resourceDesc);
			mappedBufferManager.OnResourceCreated((ID3D12Resource*)*ppvResource, resourceDesc.resourceDesc);
		}
		return hr;
	}

	HRESULT Vista::OnCreatePlacedResource(ID3D12Device* pDevice, ID3D12Heap* pHeap, UINT64 heapOffset, const D3D12_RESOURCE_DESC* pDesc, D3D12_RESOURCE_STATES initialState, const D3D12_CLEAR_VALUE* pOptimizedClearValue, REFIID riid, void** ppvResource)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CreatePlacedResource(pDevice, pHeap, heapOffset, pDesc, initialState, pOptimizedClearValue, riid, ppvResource);
		}

		CommandRecorder& queueRecorder = recorderManager.GetOrCreateRecorder(pDevice);
		CreatePlacedResourceCommand& cmd = queueRecorder.AddCommand<CreatePlacedResourceCommand>();
		HRESULT hr = d3d12PFNs.CreatePlacedResource(pDevice, pHeap, heapOffset, pDesc, initialState, pOptimizedClearValue, riid, ppvResource);
		cmd.hr = hr;
		if (SUCCEEDED(hr) && ppvResource && *ppvResource)
		{
			ResourceDesc resourceDesc{};
			resourceDesc.resourceDesc = pDesc ? *pDesc : D3D12_RESOURCE_DESC{};
			resourceDesc.initialResourceState = initialState;
			resourceDesc.heapId = InvalidObjectID;
			resourceDesc.heapOffset = heapOffset;
			if (pOptimizedClearValue)
			{
				resourceDesc.optimizedClearValue = *pOptimizedClearValue;
			}
			cmd.resourceId = objectTracker.TrackObject<ID3D12Resource>(*ppvResource, resourceDesc);

			stateTracker.OnResourceCreated((ID3D12Resource*)*ppvResource, resourceDesc.initialResourceState);
			addressTracker.OnResourceCreated((ID3D12Resource*)*ppvResource, resourceDesc.resourceDesc);
			mappedBufferManager.OnResourceCreated((ID3D12Resource*)*ppvResource, resourceDesc.resourceDesc);
		}
		return hr;
	}

	HRESULT Vista::OnCreatePlacedResource1(ID3D12Device8* pDevice, ID3D12Heap* pHeap, UINT64 heapOffset, const D3D12_RESOURCE_DESC1* pDesc, D3D12_RESOURCE_STATES initialState, const D3D12_CLEAR_VALUE* pOptimizedClearValue, REFIID riid, void** ppvResource)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CreatePlacedResource1(pDevice, pHeap, heapOffset, pDesc, initialState, pOptimizedClearValue, riid, ppvResource);
		}

		CommandRecorder& queueRecorder = recorderManager.GetOrCreateRecorder(pDevice);
		CreatePlacedResource1Command& cmd = queueRecorder.AddCommand<CreatePlacedResource1Command>();
		HRESULT hr = d3d12PFNs.CreatePlacedResource1(pDevice, pHeap, heapOffset, pDesc, initialState, pOptimizedClearValue, riid, ppvResource);
		cmd.hr = hr;
		if (SUCCEEDED(hr) && ppvResource && *ppvResource)
		{
			ResourceDesc resourceDesc{};
			if (pDesc)
			{
				resourceDesc.resourceDesc.Dimension = pDesc->Dimension;
				resourceDesc.resourceDesc.Alignment = pDesc->Alignment;
				resourceDesc.resourceDesc.Width = pDesc->Height;
				resourceDesc.resourceDesc.DepthOrArraySize = pDesc->DepthOrArraySize;
				resourceDesc.resourceDesc.MipLevels = pDesc->MipLevels;
				resourceDesc.resourceDesc.Format = pDesc->Format;
				resourceDesc.resourceDesc.SampleDesc = pDesc->SampleDesc;
				resourceDesc.resourceDesc.Layout = pDesc->Layout;
				resourceDesc.resourceDesc.Flags = pDesc->Flags;
				resourceDesc.mipRegion = pDesc->SamplerFeedbackMipRegion;
			}
			resourceDesc.initialResourceState = initialState;
			resourceDesc.heapId = InvalidObjectID;
			resourceDesc.heapOffset = heapOffset;
			if (pOptimizedClearValue)
			{
				resourceDesc.optimizedClearValue = *pOptimizedClearValue;
			}
			cmd.resourceId = objectTracker.TrackObject<ID3D12Resource>(*ppvResource, resourceDesc);

			stateTracker.OnResourceCreated((ID3D12Resource*)*ppvResource, resourceDesc.initialResourceState);
			addressTracker.OnResourceCreated((ID3D12Resource*)*ppvResource, resourceDesc.resourceDesc);
			mappedBufferManager.OnResourceCreated((ID3D12Resource*)*ppvResource, resourceDesc.resourceDesc);
		}
		return hr;
	}

	HRESULT Vista::OnCreateDescriptorHeap(ID3D12Device* pDevice, const D3D12_DESCRIPTOR_HEAP_DESC* pHeapProperties, REFIID riid, void** ppDescriptorHeap)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CreateDescriptorHeap(pDevice, pHeapProperties, riid, ppDescriptorHeap);
		}

		CommandRecorder& queueRecorder = recorderManager.GetOrCreateRecorder(pDevice);
		CreateDescriptorHeapCommand& cmd = queueRecorder.AddCommand<CreateDescriptorHeapCommand>();
		HRESULT hr = d3d12PFNs.CreateDescriptorHeap(pDevice, pHeapProperties, riid, ppDescriptorHeap);
		cmd.hr = hr;
		if (SUCCEEDED(hr) && ppDescriptorHeap && *ppDescriptorHeap)
		{
			DescriptorHeapDesc desc{ .heapDesc = *pHeapProperties };
			cmd.descriptorHeapId = objectTracker.TrackObject<ID3D12DescriptorHeap>(*ppDescriptorHeap, desc);
			descriptorTracker.AddDescriptorHeap((ID3D12DescriptorHeap*)*ppDescriptorHeap);
		}
		return hr;
	}

	void Vista::OnCreateRenderTargetView(ID3D12Device* pDevice, ID3D12Resource* pResource, const D3D12_RENDER_TARGET_VIEW_DESC* pDesc, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CreateRenderTargetView(pDevice, pResource, pDesc, destDescriptor);
		}
		d3d12PFNs.CreateRenderTargetView(pDevice, pResource, pDesc, destDescriptor);
		descriptorTracker.OnCreateRTV(destDescriptor, pResource, pDesc);
	}

	void Vista::OnCreateDepthStencilView(ID3D12Device* pDevice, ID3D12Resource* pResource, const D3D12_DEPTH_STENCIL_VIEW_DESC* pDesc, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CreateDepthStencilView(pDevice, pResource, pDesc, destDescriptor);
		}
		d3d12PFNs.CreateDepthStencilView(pDevice, pResource, pDesc, destDescriptor);
		descriptorTracker.OnCreateDSV(destDescriptor, pResource, pDesc);
	}

	void Vista::OnCreateShaderResourceView(ID3D12Device* pDevice, ID3D12Resource* pResource, const D3D12_SHADER_RESOURCE_VIEW_DESC* pDesc, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CreateShaderResourceView(pDevice, pResource, pDesc, destDescriptor);
		}
		d3d12PFNs.CreateShaderResourceView(pDevice, pResource, pDesc, destDescriptor);
		descriptorTracker.OnCreateSRV(destDescriptor, pResource, pDesc);
	}

	void Vista::OnCreateUnorderedAccessView(ID3D12Device* pDevice, ID3D12Resource* pResource, ID3D12Resource* counterResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* pDesc, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CreateUnorderedAccessView(pDevice, pResource, counterResource, pDesc, destDescriptor);
		}
		d3d12PFNs.CreateUnorderedAccessView(pDevice, pResource, counterResource, pDesc, destDescriptor);
		descriptorTracker.OnCreateUAV(destDescriptor, pResource, counterResource, pDesc);
	}

	void Vista::OnCreateConstantBufferView(ID3D12Device* pDevice, const D3D12_CONSTANT_BUFFER_VIEW_DESC* pDesc, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CreateConstantBufferView(pDevice, pDesc, destDescriptor);
		}
		d3d12PFNs.CreateConstantBufferView(pDevice, pDesc, destDescriptor);
		descriptorTracker.OnCreateCBV(destDescriptor, pDesc);
	}

	void Vista::OnCreateSampler(ID3D12Device* pDevice, const D3D12_SAMPLER_DESC* pDesc, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CreateSampler(pDevice, pDesc, destDescriptor);
		}
		d3d12PFNs.CreateSampler(pDevice, pDesc, destDescriptor);
		descriptorTracker.OnCreateSampler(destDescriptor, pDesc);
	}

	void Vista::OnCopyDescriptorsSimple(ID3D12Device* pDevice, UINT numDescriptors, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptorRangeStart, D3D12_CPU_DESCRIPTOR_HANDLE srcDescriptorRangeStart, D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapsType)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CopyDescriptorsSimple(pDevice, numDescriptors, destDescriptorRangeStart, srcDescriptorRangeStart, descriptorHeapsType);
		}
		d3d12PFNs.CopyDescriptorsSimple(pDevice, numDescriptors, destDescriptorRangeStart, srcDescriptorRangeStart, descriptorHeapsType);
		descriptorTracker.OnCopyDescriptorsSimple(numDescriptors, destDescriptorRangeStart, srcDescriptorRangeStart, descriptorHeapsType);
	}

	void Vista::OnCopyDescriptors(ID3D12Device* pDevice, UINT numDestDescriptorRanges, const D3D12_CPU_DESCRIPTOR_HANDLE* pDestDescriptorRangeStarts, const UINT* pDestDescriptorRangeSizes, UINT numSrcDescriptorRanges, const D3D12_CPU_DESCRIPTOR_HANDLE* pSrcDescriptorRangeStarts, const UINT* pSrcDescriptorRangeSizes, D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapsType)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CopyDescriptors(pDevice, numDestDescriptorRanges, pDestDescriptorRangeStarts, pDestDescriptorRangeSizes, numSrcDescriptorRanges, pSrcDescriptorRangeStarts, pSrcDescriptorRangeSizes, descriptorHeapsType);
		}
		d3d12PFNs.CopyDescriptors(pDevice, numDestDescriptorRanges, pDestDescriptorRangeStarts, pDestDescriptorRangeSizes, numSrcDescriptorRanges, pSrcDescriptorRangeStarts, pSrcDescriptorRangeSizes, descriptorHeapsType);
		descriptorTracker.OnCopyDescriptors(numDestDescriptorRanges, pDestDescriptorRangeStarts, pDestDescriptorRangeSizes, numSrcDescriptorRanges, pSrcDescriptorRangeStarts, pSrcDescriptorRangeSizes, descriptorHeapsType);
	}

	void Vista::OnExecuteCommandLists(ID3D12CommandQueue* pCommandQueue, UINT commandListCount, ID3D12CommandList* const* commandLists)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.ExecuteCommandLists(pCommandQueue, commandListCount, commandLists);
		}

		CommandRecorder& queueRecorder = recorderManager.GetOrCreateRecorder(pCommandQueue);
		ExecuteCommandListsCommand& cmd = queueRecorder.AddCommand<ExecuteCommandListsCommand>();
		cmd.recorders.reserve(commandListCount);
		for (Uint i = 0; i < commandListCount; ++i)
		{
			cmd.recorders.push_back(&recorderManager.GetOrCreateRecorder(commandLists[i]));
		}
		
		VISTA_RENDER_GUARD;
		d3d12PFNs.ExecuteCommandLists(pCommandQueue, commandListCount, commandLists);
		VISTA_TODO(stateTracker.OnExecuteCommandLists(pCommandQueue, commandListCount, commandLists););
		for (Uint i = 0; i < commandListCount; ++i)
		{
			if (commandLists[i] == pRequestCommandList)
			{
				copyRequestManager.SignalOnQueue(pCommandQueue);
				break;
			}
		}
	}

	HRESULT Vista::OnSignal(ID3D12CommandQueue* pCommandQueue, ID3D12Fence* pFence, UINT64 fenceValue)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.Queue_Signal(pCommandQueue, pFence, fenceValue);
		}

		HRESULT hr = d3d12PFNs.Queue_Signal(pCommandQueue, pFence, fenceValue);
		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandQueue);
		QueueSignalCommand& cmd = recorder.AddCommand<QueueSignalCommand>();
		cmd.fenceId = objectTracker.GetObjectID(pFence);
		cmd.fenceValue = fenceValue;
		cmd.hr = hr;
		return d3d12PFNs.Queue_Signal(pCommandQueue, pFence, fenceValue);
	}

	void Vista::OnCopyBufferRegion(ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* pDstBuffer, UINT64 dstOffset, ID3D12Resource* pSrcBuffer, UINT64 srcOffset, UINT64 numBytes)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CopyBufferRegion(pCommandList, pDstBuffer, dstOffset, pSrcBuffer, srcOffset, numBytes);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		CopyBufferRegionCommand& cmd = recorder.AddCommand<CopyBufferRegionCommand>();
		cmd.dstBufferId = objectTracker.GetObjectID(pDstBuffer);
		cmd.dstOffset = dstOffset;
		cmd.srcBufferId = objectTracker.GetObjectID(pSrcBuffer);
		cmd.srcOffset = srcOffset;
		cmd.numBytes = numBytes;

		mappedBufferManager.OnCopyBuffer(pDstBuffer, dstOffset, pSrcBuffer, srcOffset, numBytes);
		return d3d12PFNs.CopyBufferRegion(pCommandList, pDstBuffer, dstOffset, pSrcBuffer, srcOffset, numBytes);
	}

	void Vista::OnCopyTextureRegion(ID3D12GraphicsCommandList* pCommandList, const D3D12_TEXTURE_COPY_LOCATION* pDst, UINT dstX, UINT dstY, UINT dstZ, const D3D12_TEXTURE_COPY_LOCATION* pSrc, const D3D12_BOX* pSrcBox)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CopyTextureRegion(pCommandList, pDst, dstX, dstY, dstZ, pSrc, pSrcBox);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		CopyTextureRegionCommand& cmd = recorder.AddCommand<CopyTextureRegionCommand>();
		VISTA_TODO("pDst has more information than what is captured here");
		cmd.dstTextureId = objectTracker.GetObjectID(pDst->pResource);
		cmd.dstX = dstX;
		cmd.dstY = dstY;
		cmd.dstZ = dstZ;
		cmd.srcTextureId = objectTracker.GetObjectID(pSrc->pResource);
		if (pSrcBox)
		{
			cmd.srcBox = *pSrcBox;
		}
		return d3d12PFNs.CopyTextureRegion(pCommandList, pDst, dstX, dstY, dstZ, pSrc, pSrcBox);
	}

	void Vista::OnCopyResource(ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* pDstResource, ID3D12Resource* pSrcResource)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.CopyResource(pCommandList, pDstResource, pSrcResource);
		}
		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		CopyResourceCommand& cmd = recorder.AddCommand<CopyResourceCommand>();
		cmd.dstResourceId = objectTracker.GetObjectID(pDstResource);
		cmd.srcResourceId = objectTracker.GetObjectID(pSrcResource);
		mappedBufferManager.OnCopyResource(pDstResource, pSrcResource);
		return d3d12PFNs.CopyResource(pCommandList, pDstResource, pSrcResource);
	}

	void Vista::OnResolveSubresource(ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* pDstResource, UINT dstSubresource, ID3D12Resource* pSrcResource, UINT srcSubresource, DXGI_FORMAT format)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.ResolveSubresource(pCommandList, pDstResource, dstSubresource, pSrcResource, srcSubresource, format);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		ResolveSubresourceCommand& cmd = recorder.AddCommand<ResolveSubresourceCommand>();

		cmd.dstResourceId = objectTracker.GetObjectID(pDstResource);
		cmd.dstSubresource = dstSubresource;
		cmd.srcResourceId = objectTracker.GetObjectID(pSrcResource);
		cmd.srcSubresource = srcSubresource;
		cmd.format = format;

		return d3d12PFNs.ResolveSubresource(pCommandList, pDstResource, dstSubresource, pSrcResource, srcSubresource, format);
	}

	void Vista::OnResolveSubresourceRegion(ID3D12GraphicsCommandList1* pCommandList, ID3D12Resource* pDstResource, UINT dstSubresource, UINT dstX, UINT dstY, 
										   ID3D12Resource* pSrcResource, UINT srcSubresource, D3D12_RECT* pSrcRect, DXGI_FORMAT format, D3D12_RESOLVE_MODE resolveMode)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.ResolveSubresourceRegion(pCommandList, pDstResource, dstSubresource, dstX, dstY, pSrcResource, srcSubresource, pSrcRect, format, resolveMode);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		ResolveSubresourceRegionCommand& cmd = recorder.AddCommand<ResolveSubresourceRegionCommand>();
		cmd.dstResourceId = objectTracker.GetObjectID(pDstResource);
		cmd.dstSubresource = dstSubresource;
		cmd.dstX = dstX;
		cmd.dstY = dstY;
		cmd.srcResourceId = objectTracker.GetObjectID(pSrcResource);
		cmd.srcSubresource = srcSubresource;
		if (pSrcRect)
		{
			cmd.srcRect = *pSrcRect;
		}
		cmd.resolveMode = resolveMode;
		cmd.format = format;
		return d3d12PFNs.ResolveSubresourceRegion(pCommandList, pDstResource, dstSubresource, dstX, dstY, pSrcResource, srcSubresource, pSrcRect, format, resolveMode);
	}

	void Vista::OnDrawInstanced(ID3D12GraphicsCommandList* pCommandList, UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation, UINT startInstanceLocation)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.DrawInstanced(pCommandList, vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		DrawInstancedCommand& cmd = recorder.AddCommand<DrawInstancedCommand>();
		cmd.vertexCountPerInstance = vertexCountPerInstance;
		cmd.instanceCount = instanceCount;
		cmd.startVertexLocation = startVertexLocation;
		cmd.startInstanceLocation = startInstanceLocation;

		d3d12PFNs.DrawInstanced(pCommandList, vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
		CheckForCopyRequest(pCommandList, cmd);
	}

	void Vista::OnDrawIndexedInstanced(ID3D12GraphicsCommandList* pCommandList, UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation, INT baseVertexLocation, UINT startInstanceLocation)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.DrawIndexedInstanced(pCommandList, indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		DrawIndexedInstancedCommand& cmd = recorder.AddCommand<DrawIndexedInstancedCommand>();
		cmd.indexCountPerInstance = indexCountPerInstance;
		cmd.instanceCount = instanceCount;
		cmd.startIndexLocation = startIndexLocation;
		cmd.baseVertexLocation = baseVertexLocation;
		cmd.startInstanceLocation = startInstanceLocation;

		d3d12PFNs.DrawIndexedInstanced(pCommandList, indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
		CheckForCopyRequest(pCommandList, cmd);
	}

	void Vista::OnDispatch(ID3D12GraphicsCommandList* pCommandList, UINT threadGroupCountX, UINT threadGroupCountY, UINT threadGroupCountZ)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.Dispatch(pCommandList, threadGroupCountX, threadGroupCountY, threadGroupCountZ);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		DispatchCommand& cmd = recorder.AddCommand<DispatchCommand>();
		cmd.threadGroupCountX = threadGroupCountX;
		cmd.threadGroupCountY = threadGroupCountY;
		cmd.threadGroupCountZ = threadGroupCountZ;

		d3d12PFNs.Dispatch(pCommandList, threadGroupCountX, threadGroupCountY, threadGroupCountZ);
		CheckForCopyRequest(pCommandList, cmd);
	}

	void Vista::OnDispatchMesh(ID3D12GraphicsCommandList6* pCommandList, UINT threadGroupCountX, UINT threadGroupCountY, UINT threadGroupCountZ)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.DispatchMesh(pCommandList, threadGroupCountX, threadGroupCountY, threadGroupCountZ);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		DispatchMeshCommand& cmd = recorder.AddCommand<DispatchMeshCommand>();
		cmd.threadGroupCountX = threadGroupCountX;
		cmd.threadGroupCountY = threadGroupCountY;
		cmd.threadGroupCountZ = threadGroupCountZ;

		return d3d12PFNs.DispatchMesh(pCommandList, threadGroupCountX, threadGroupCountY, threadGroupCountZ);
	}

	void Vista::OnExecuteIndirect(ID3D12GraphicsCommandList* pCommandList, ID3D12CommandSignature* pCommandSignature, UINT maxCommandCount, ID3D12Resource* pArgumentBuffer, UINT64 argumentBufferOffset, ID3D12Resource* pCountBuffer, UINT64 countBufferOffset)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.ExecuteIndirect(pCommandList, pCommandSignature, maxCommandCount, pArgumentBuffer, argumentBufferOffset, pCountBuffer, countBufferOffset);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		ExecuteIndirectCommand& cmd = recorder.AddCommand<ExecuteIndirectCommand>();
		cmd.commandSignatureId = objectTracker.GetObjectID(pCommandSignature);
		cmd.maxCommandCount = maxCommandCount;
		cmd.argumentBufferId = objectTracker.GetObjectID(pArgumentBuffer);
		cmd.argumentBufferOffset = argumentBufferOffset;
		cmd.countBufferId = objectTracker.GetObjectID(pCountBuffer);
		cmd.countBufferOffset = countBufferOffset;
		return d3d12PFNs.ExecuteIndirect(pCommandList, pCommandSignature, maxCommandCount, pArgumentBuffer, argumentBufferOffset, pCountBuffer, countBufferOffset);
	}

	void Vista::OnDispatchRays(ID3D12GraphicsCommandList4* pCommandList, const D3D12_DISPATCH_RAYS_DESC* pDesc)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.DispatchRays(pCommandList, pDesc);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		DispatchRaysCommand& cmd = recorder.AddCommand<DispatchRaysCommand>();
		if (pDesc)
		{
			cmd.dispatchWidth = pDesc->Width;
			cmd.dispatchWidth = pDesc->Height;
			cmd.dispatchWidth = pDesc->Depth;
		}
		return d3d12PFNs.DispatchRays(pCommandList, pDesc);
	}

	void Vista::OnRSSetViewports(ID3D12GraphicsCommandList* pCommandList, UINT viewportCount, const D3D12_VIEWPORT* pViewports)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.RSSetViewports(pCommandList, viewportCount, pViewports);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		RSSetViewportsCommand& cmd = recorder.AddCommand<RSSetViewportsCommand>();
		cmd.viewports.reserve(viewportCount);
		for (Uint i = 0; i < viewportCount; ++i)
		{
			cmd.viewports.push_back(pViewports[i]);
		}
		return d3d12PFNs.RSSetViewports(pCommandList, viewportCount, pViewports);
	}

	void Vista::OnRSSetScissorRects(ID3D12GraphicsCommandList* pCommandList, UINT numRects, const D3D12_RECT* pRects)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.RSSetScissorRects(pCommandList, numRects, pRects);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		RSSetScissorRectsCommand& cmd = recorder.AddCommand<RSSetScissorRectsCommand>();
		cmd.scissorRects.reserve(numRects);
		for (Uint i = 0; i < numRects; ++i)
		{
			cmd.scissorRects.push_back(pRects[i]);
		}
		return d3d12PFNs.RSSetScissorRects(pCommandList, numRects, pRects);
	}

	void Vista::OnRSSetShadingRate(ID3D12GraphicsCommandList5* pCommandList, D3D12_SHADING_RATE baseShadingRate, const D3D12_SHADING_RATE_COMBINER* pCombiners)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.RSSetShadingRate(pCommandList, baseShadingRate, pCombiners);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		RSSetShadingRateCommand& cmd = recorder.AddCommand<RSSetShadingRateCommand>();
		cmd.shadingRate = baseShadingRate;
		if (pCombiners)
		{
			cmd.shadingRateCombiners[0] = pCombiners[0];
			cmd.shadingRateCombiners[1] = pCombiners[1];
		}
		return d3d12PFNs.RSSetShadingRate(pCommandList, baseShadingRate, pCombiners);
	}

	void Vista::OnSetViewInstanceMask(ID3D12GraphicsCommandList1* pCommandList, UINT mask)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.SetViewInstanceMask(pCommandList, mask);
		}
		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		SetViewInstanceMaskCommand& cmd = recorder.AddCommand<SetViewInstanceMaskCommand>();
		cmd.viewInstanceMask = mask;
		return d3d12PFNs.SetViewInstanceMask(pCommandList, mask);
	}

	void Vista::OnRSSetShadingRateImage(ID3D12GraphicsCommandList5* pCommandList, ID3D12Resource* shadingRateImage)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.RSSetShadingRateImage(pCommandList, shadingRateImage);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		RSSetShadingRateImageCommand& cmd = recorder.AddCommand<RSSetShadingRateImageCommand>();
		if (shadingRateImage)
		{
			cmd.shadingRateImageId = objectTracker.GetObjectID(shadingRateImage);
		}
		return d3d12PFNs.RSSetShadingRateImage(pCommandList, shadingRateImage);
	}

	void Vista::OnOMSetRenderTargets(ID3D12GraphicsCommandList* pCommandList, UINT numRenderTargetDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE* pRenderTargetDescriptors, BOOL RTsSingleHandleToDescriptorRange, const D3D12_CPU_DESCRIPTOR_HANDLE* pDepthStencilDescriptor)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.OMSetRenderTargets(pCommandList, numRenderTargetDescriptors, pRenderTargetDescriptors, RTsSingleHandleToDescriptorRange, pDepthStencilDescriptor);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		OMSetRenderTargetsCommand& cmd = recorder.AddCommand<OMSetRenderTargetsCommand>();
		cmd.renderTargetDescriptors.resize(numRenderTargetDescriptors);
		for (Uint i = 0; i < numRenderTargetDescriptors; ++i)
		{
			cmd.renderTargetDescriptors[i] = pRenderTargetDescriptors[i];
		}
		cmd.RTsSingleHandleToDescriptorRange = RTsSingleHandleToDescriptorRange;
		if (pDepthStencilDescriptor)
		{
			cmd.depthStencilDescriptor = *pDepthStencilDescriptor;
		}
		return d3d12PFNs.OMSetRenderTargets(pCommandList, numRenderTargetDescriptors, pRenderTargetDescriptors, RTsSingleHandleToDescriptorRange, pDepthStencilDescriptor);
	}

	void Vista::OnOMSetBlendFactor(ID3D12GraphicsCommandList* pCommandList, const FLOAT* blendFactor)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.OMSetBlendFactor(pCommandList, blendFactor);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		OMSetBlendFactorCommand& cmd = recorder.AddCommand<OMSetBlendFactorCommand>();

		cmd.blendFactor[0] = blendFactor[0];
		cmd.blendFactor[1] = blendFactor[1];
		cmd.blendFactor[2] = blendFactor[2];
		cmd.blendFactor[3] = blendFactor[3];

		return d3d12PFNs.OMSetBlendFactor(pCommandList, blendFactor);
	}

	void Vista::OnOMSetStencilRef(ID3D12GraphicsCommandList* pCommandList, UINT stencilRef)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.OMSetStencilRef(pCommandList, stencilRef);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		OMSetStencilRefCommand& cmd = recorder.AddCommand<OMSetStencilRefCommand>();
		cmd.stencilRef = stencilRef;

		return d3d12PFNs.OMSetStencilRef(pCommandList, stencilRef);
	}

	void Vista::OnOMSetDepthBounds(ID3D12GraphicsCommandList1* pCommandList, FLOAT min, FLOAT max)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.OMSetDepthBounds(pCommandList, min, max);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		OMSetDepthBoundsCommand& cmd = recorder.AddCommand<OMSetDepthBoundsCommand>();
		cmd.depthMin = min;
		cmd.depthMax = max;

		return d3d12PFNs.OMSetDepthBounds(pCommandList, min, max);
	}

	void Vista::OnBeginRenderPass(ID3D12GraphicsCommandList4* pCommandList, UINT numRenderTargets, const D3D12_RENDER_PASS_RENDER_TARGET_DESC* pRenderTargets, const D3D12_RENDER_PASS_DEPTH_STENCIL_DESC* pDepthStencil, D3D12_RENDER_PASS_FLAGS flags)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.BeginRenderPass(pCommandList, numRenderTargets, pRenderTargets, pDepthStencil, flags);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		BeginRenderPassCommand& cmd = recorder.AddCommand<BeginRenderPassCommand>();
		cmd.flags = flags;
		cmd.renderTargets.reserve(numRenderTargets);
		for (Uint32 i = 0; i < numRenderTargets; ++i)
		{
			cmd.renderTargets.push_back(pRenderTargets[i]);
		}
		if (pDepthStencil)
		{
			cmd.depthStencil = *pDepthStencil;
		}
		return d3d12PFNs.BeginRenderPass(pCommandList, numRenderTargets, pRenderTargets, pDepthStencil, flags);
	}

	void Vista::OnEndRenderPass(ID3D12GraphicsCommandList4* pCommandList)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.EndRenderPass(pCommandList);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		EndRenderPassCommand& cmd = recorder.AddCommand<EndRenderPassCommand>();
		return d3d12PFNs.EndRenderPass(pCommandList);
	}

	void Vista::OnClearRenderTargetView(ID3D12GraphicsCommandList* pCommandList, D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView, const FLOAT* colorRGBA, UINT numRects, const D3D12_RECT* pRects)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.ClearRenderTargetView(pCommandList, renderTargetView, colorRGBA, numRects, pRects);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		ClearRenderTargetViewCommand& cmd = recorder.AddCommand<ClearRenderTargetViewCommand>();
		cmd.renderTargetView = renderTargetView;
		cmd.colorRGBA[0] = colorRGBA[0];
		cmd.colorRGBA[1] = colorRGBA[1];
		cmd.colorRGBA[2] = colorRGBA[2];
		cmd.colorRGBA[3] = colorRGBA[3];
		cmd.rects.reserve(numRects);
		for (Uint i = 0; i < numRects; ++i)
		{
			cmd.rects.push_back(pRects[i]);
		}
		return d3d12PFNs.ClearRenderTargetView(pCommandList, renderTargetView, colorRGBA, numRects, pRects);
	}

	void Vista::OnClearDepthStencilView(ID3D12GraphicsCommandList* pCommandList, D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView, D3D12_CLEAR_FLAGS clearFlags, FLOAT depth, UINT8 stencil, UINT numRects, const D3D12_RECT* pRects)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.ClearDepthStencilView(pCommandList, depthStencilView, clearFlags, depth, stencil, numRects, pRects);
		}
		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		ClearDepthStencilViewCommand& cmd = recorder.AddCommand<ClearDepthStencilViewCommand>();
		cmd.depthStencilView = depthStencilView;
		cmd.clearFlags = clearFlags;
		cmd.depth = depth;
		cmd.stencil = stencil;
		for (Uint i = 0; i < numRects; ++i)
		{
			cmd.rects.push_back(pRects[i]);
		}
		return d3d12PFNs.ClearDepthStencilView(pCommandList, depthStencilView, clearFlags, depth, stencil, numRects, pRects);
	}

	void Vista::OnClearUnorderedAccessViewUint(ID3D12GraphicsCommandList* pCommandList, D3D12_GPU_DESCRIPTOR_HANDLE viewGPUHandleInCurrentHeap, D3D12_CPU_DESCRIPTOR_HANDLE viewCPUHandle, ID3D12Resource* pResource, const UINT* values, UINT numRects, const D3D12_RECT* pRects)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.ClearUnorderedAccessViewUint(pCommandList, viewGPUHandleInCurrentHeap, viewCPUHandle, pResource, values, numRects, pRects);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		ClearUnorderedAccessViewUintCommand& cmd = recorder.AddCommand<ClearUnorderedAccessViewUintCommand>();

		cmd.gpuUnorderedAccessView = viewGPUHandleInCurrentHeap;
		cmd.cpuUnorderedAccessView = viewCPUHandle;
		cmd.resourceId = objectTracker.GetObjectID(pResource);
		cmd.values[0] = values[0];
		cmd.values[1] = values[1];
		cmd.values[2] = values[2];
		cmd.values[3] = values[3];
		for (Uint i = 0; i < numRects; ++i)
		{
			cmd.rects.push_back(pRects[i]);
		}
		return d3d12PFNs.ClearUnorderedAccessViewUint(pCommandList, viewGPUHandleInCurrentHeap, viewCPUHandle, pResource, values, numRects, pRects);
	}

	void Vista::OnClearUnorderedAccessViewFloat(ID3D12GraphicsCommandList* pCommandList, D3D12_GPU_DESCRIPTOR_HANDLE viewGPUHandleInCurrentHeap, D3D12_CPU_DESCRIPTOR_HANDLE viewCPUHandle, ID3D12Resource* pResource, const FLOAT* values, UINT numRects, const D3D12_RECT* pRects)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.ClearUnorderedAccessViewFloat(pCommandList, viewGPUHandleInCurrentHeap, viewCPUHandle, pResource, values, numRects, pRects);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		ClearUnorderedAccessViewFloatCommand& cmd = recorder.AddCommand<ClearUnorderedAccessViewFloatCommand>();

		cmd.gpuUnorderedAccessView = viewGPUHandleInCurrentHeap;
		cmd.cpuUnorderedAccessView = viewCPUHandle;
		cmd.resourceId = objectTracker.GetObjectID(pResource);
		cmd.values[0] = values[0];
		cmd.values[1] = values[1];
		cmd.values[2] = values[2];
		cmd.values[3] = values[3];
		for (Uint i = 0; i < numRects; ++i)
		{
			cmd.rects.push_back(pRects[i]);
		}
		return d3d12PFNs.ClearUnorderedAccessViewFloat(pCommandList, viewGPUHandleInCurrentHeap, viewCPUHandle, pResource, values, numRects, pRects);
	}

	void Vista::OnSetPipelineState(ID3D12GraphicsCommandList* pCommandList, ID3D12PipelineState* pPipelineState)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.SetPipelineState(pCommandList, pPipelineState);
		}
		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		SetPipelineStateCommand& cmd = recorder.AddCommand<SetPipelineStateCommand>();
		cmd.psoId = objectTracker.GetObjectID(pPipelineState);
		return d3d12PFNs.SetPipelineState(pCommandList, pPipelineState);
	}

	void Vista::OnSetPipelineState1(ID3D12GraphicsCommandList4* pCommandList, ID3D12StateObject* pStateObject)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.SetPipelineState1(pCommandList, pStateObject);
		}
		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		SetPipelineState1Command& cmd = recorder.AddCommand<SetPipelineState1Command>();
		cmd.stateObjectId = objectTracker.GetObjectID(pStateObject);
		return d3d12PFNs.SetPipelineState1(pCommandList, pStateObject);
	}

	void Vista::OnSetGraphicsRootSignature(ID3D12GraphicsCommandList* pCommandList, ID3D12RootSignature* pRootSignature)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.SetGraphicsRootSignature(pCommandList, pRootSignature);
		}
		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		SetGraphicsRootSignatureCommand& cmd = recorder.AddCommand<SetGraphicsRootSignatureCommand>();
		cmd.rootSignatureId = objectTracker.GetObjectID(pRootSignature);
		return d3d12PFNs.SetGraphicsRootSignature(pCommandList, pRootSignature);
	}

	void Vista::OnSetComputeRootSignature(ID3D12GraphicsCommandList* pCommandList, ID3D12RootSignature* pRootSignature)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.SetComputeRootSignature(pCommandList, pRootSignature);
		}
		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		SetComputeRootSignatureCommand& cmd = recorder.AddCommand<SetComputeRootSignatureCommand>();
		cmd.rootSignatureId = objectTracker.GetObjectID(pRootSignature);
		return d3d12PFNs.SetComputeRootSignature(pCommandList, pRootSignature);
	}

	void Vista::OnIASetPrimitiveTopology(ID3D12GraphicsCommandList* pCommandList, D3D12_PRIMITIVE_TOPOLOGY topology)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.IASetPrimitiveTopology(pCommandList, topology);
		}
		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		IASetPrimitiveTopologyCommand& cmd = recorder.AddCommand<IASetPrimitiveTopologyCommand>();
		cmd.topology = topology;
		return d3d12PFNs.IASetPrimitiveTopology(pCommandList, topology);
	}

	void Vista::OnIASetVertexBuffers(ID3D12GraphicsCommandList* pCommandList, UINT startSlot, UINT numViews, const D3D12_VERTEX_BUFFER_VIEW* pViews)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.IASetVertexBuffers(pCommandList, startSlot, numViews, pViews);
		}
		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		IASetVertexBuffersCommand& cmd = recorder.AddCommand<IASetVertexBuffersCommand>();
		cmd.startSlot = startSlot;
		cmd.vertexBufferViews.resize(numViews);
		for (Uint i = 0; i < numViews; ++i)
		{
			cmd.vertexBufferViews[i] = pViews[i];
		}
		return d3d12PFNs.IASetVertexBuffers(pCommandList, startSlot, numViews, pViews);
	}

	void Vista::OnIASetIndexBuffer(ID3D12GraphicsCommandList* pCommandList, const D3D12_INDEX_BUFFER_VIEW* pView)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.IASetIndexBuffer(pCommandList, pView);
		}
		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		IASetIndexBufferCommand& cmd = recorder.AddCommand<IASetIndexBufferCommand>();
		if (pView)
		{
			cmd.indexBufferView = *pView;
		}
		return d3d12PFNs.IASetIndexBuffer(pCommandList, pView);
	}

	void Vista::OnResourceBarrier(ID3D12GraphicsCommandList* pCommandList, UINT numBarriers, const D3D12_RESOURCE_BARRIER* pBarriers)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.ResourceBarrier(pCommandList, numBarriers, pBarriers);
		}
		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		ResourceBarrierCommand& cmd = recorder.AddCommand<ResourceBarrierCommand>();
		cmd.barriers.resize(numBarriers);
		for (Uint i = 0; i < numBarriers; ++i)
		{
			cmd.barriers[i] = pBarriers[i];
		}
		stateTracker.OnResourceBarrier(pCommandList, numBarriers, pBarriers);
		return d3d12PFNs.ResourceBarrier(pCommandList, numBarriers, pBarriers);
	}

	void Vista::OnSetDescriptorHeaps(ID3D12GraphicsCommandList* pCommandList, UINT numDescriptorHeaps, ID3D12DescriptorHeap* const* ppDescriptorHeaps)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.SetDescriptorHeaps(pCommandList, numDescriptorHeaps, ppDescriptorHeaps);
		}
		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		SetDescriptorHeapsCommand& cmd = recorder.AddCommand<SetDescriptorHeapsCommand>();
		cmd.numDescriptorHeaps = numDescriptorHeaps;
		for (Uint i = 0; i < std::min<Uint>(numDescriptorHeaps, 2); ++i)
		{
			cmd.descriptorHeapIds[i] = objectTracker.GetObjectID(ppDescriptorHeaps[i]);
		}
		return d3d12PFNs.SetDescriptorHeaps(pCommandList, numDescriptorHeaps, ppDescriptorHeaps);
	}

	void Vista::OnSetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE baseDescriptor)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.SetGraphicsRootDescriptorTable(pCommandList, rootParameterIndex, baseDescriptor);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		SetGraphicsRootDescriptorTableCommand& cmd = recorder.AddCommand<SetGraphicsRootDescriptorTableCommand>();
		cmd.rootParameterIndex = rootParameterIndex;
		cmd.baseDescriptor = baseDescriptor;
		return d3d12PFNs.SetGraphicsRootDescriptorTable(pCommandList, rootParameterIndex, baseDescriptor);
	}

	void Vista::OnSetComputeRootDescriptorTable(ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE baseDescriptor)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.SetComputeRootDescriptorTable(pCommandList, rootParameterIndex, baseDescriptor);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		SetComputeRootDescriptorTableCommand& cmd = recorder.AddCommand<SetComputeRootDescriptorTableCommand>();
		cmd.rootParameterIndex = rootParameterIndex;
		cmd.baseDescriptor = baseDescriptor;
		return d3d12PFNs.SetComputeRootDescriptorTable(pCommandList, rootParameterIndex, baseDescriptor);
	}

	void Vista::OnSetGraphicsRoot32BitConstant(ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex, UINT srcData, UINT destOffsetIn32BitValues)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.SetGraphicsRoot32BitConstant(pCommandList, rootParameterIndex, srcData, destOffsetIn32BitValues);
		}
		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		SetGraphicsRoot32BitConstantCommand& cmd = recorder.AddCommand<SetGraphicsRoot32BitConstantCommand>();
		cmd.rootParameterIndex = rootParameterIndex;
		cmd.srcData = srcData;
		cmd.destOffsetIn32BitValues = destOffsetIn32BitValues;
		return d3d12PFNs.SetGraphicsRoot32BitConstant(pCommandList, rootParameterIndex, srcData, destOffsetIn32BitValues);
	}

	void Vista::OnSetComputeRoot32BitConstant(ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex, UINT srcData, UINT destOffsetIn32BitValues)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.SetComputeRoot32BitConstant(pCommandList, rootParameterIndex, srcData, destOffsetIn32BitValues);
		}
		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		SetComputeRoot32BitConstantCommand& cmd = recorder.AddCommand<SetComputeRoot32BitConstantCommand>();
		cmd.rootParameterIndex = rootParameterIndex;
		cmd.srcData = srcData;
		cmd.destOffsetIn32BitValues = destOffsetIn32BitValues;
		return d3d12PFNs.SetComputeRoot32BitConstant(pCommandList, rootParameterIndex, srcData, destOffsetIn32BitValues);
	}

	void Vista::OnSetGraphicsRoot32BitConstants(ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex, UINT num32BitValuesToSet, const void* pSrcData, UINT destOffsetIn32BitValues)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.SetGraphicsRoot32BitConstants(pCommandList, rootParameterIndex, num32BitValuesToSet, pSrcData, destOffsetIn32BitValues);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		SetGraphicsRoot32BitConstantsCommand& cmd = recorder.AddCommand<SetGraphicsRoot32BitConstantsCommand>();
		cmd.rootParameterIndex = rootParameterIndex;
		cmd.destOffsetIn32BitValues = destOffsetIn32BitValues;
		cmd.srcData.reserve(num32BitValuesToSet);
		Uint32 const* srcData32Bit = reinterpret_cast<Uint32 const*>(pSrcData);
		for (Uint32 i = 0; i < num32BitValuesToSet; ++i)
		{
			cmd.srcData.push_back(srcData32Bit[i]);
		}
		return d3d12PFNs.SetGraphicsRoot32BitConstants(pCommandList, rootParameterIndex, num32BitValuesToSet, pSrcData, destOffsetIn32BitValues);
	}

	void Vista::OnSetComputeRoot32BitConstants(ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex, UINT num32BitValuesToSet, const void* pSrcData, UINT destOffsetIn32BitValues)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.SetComputeRoot32BitConstants(pCommandList, rootParameterIndex, num32BitValuesToSet, pSrcData, destOffsetIn32BitValues);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		SetComputeRoot32BitConstantsCommand& cmd = recorder.AddCommand<SetComputeRoot32BitConstantsCommand>();
		cmd.rootParameterIndex = rootParameterIndex;
		cmd.destOffsetIn32BitValues = destOffsetIn32BitValues;
		cmd.srcData.reserve(num32BitValuesToSet);
		Uint32 const* srcData32Bit = reinterpret_cast<Uint32 const*>(pSrcData);
		for (Uint32 i = 0; i < num32BitValuesToSet; ++i)
		{
			cmd.srcData.push_back(srcData32Bit[i]);
		}
		return d3d12PFNs.SetComputeRoot32BitConstants(pCommandList, rootParameterIndex, num32BitValuesToSet, pSrcData, destOffsetIn32BitValues);
	}

	void Vista::OnSetGraphicsRootConstantBufferView(ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.SetGraphicsRootConstantBufferView(pCommandList, rootParameterIndex, bufferLocation);
		}
		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		SetGraphicsRootConstantBufferViewCommand& cmd = recorder.AddCommand<SetGraphicsRootConstantBufferViewCommand>();
		cmd.rootParameterIndex = rootParameterIndex;
		cmd.gpuVA = bufferLocation;
		return d3d12PFNs.SetGraphicsRootConstantBufferView(pCommandList, rootParameterIndex, bufferLocation);
	}

	void Vista::OnSetComputeRootConstantBufferView(ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.SetComputeRootConstantBufferView(pCommandList, rootParameterIndex, bufferLocation);
		}
		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		SetComputeRootConstantBufferViewCommand& cmd = recorder.AddCommand<SetComputeRootConstantBufferViewCommand>();
		cmd.rootParameterIndex = rootParameterIndex;
		cmd.gpuVA = bufferLocation;
		return d3d12PFNs.SetComputeRootConstantBufferView(pCommandList, rootParameterIndex, bufferLocation);
	}

	void Vista::OnSetGraphicsRootShaderResourceView(ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.SetGraphicsRootShaderResourceView(pCommandList, rootParameterIndex, bufferLocation);
		}
		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		SetGraphicsRootShaderResourceViewCommand& cmd = recorder.AddCommand<SetGraphicsRootShaderResourceViewCommand>();
		cmd.rootParameterIndex = rootParameterIndex;
		cmd.gpuVA = bufferLocation;
		return d3d12PFNs.SetGraphicsRootShaderResourceView(pCommandList, rootParameterIndex, bufferLocation);
	}

	void Vista::OnSetComputeRootShaderResourceView(ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.SetComputeRootShaderResourceView(pCommandList, rootParameterIndex, bufferLocation);
		}
		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		SetComputeRootShaderResourceViewCommand& cmd = recorder.AddCommand<SetComputeRootShaderResourceViewCommand>();
		cmd.rootParameterIndex = rootParameterIndex;
		cmd.gpuVA = bufferLocation;
		return d3d12PFNs.SetComputeRootShaderResourceView(pCommandList, rootParameterIndex, bufferLocation);
	}

	void Vista::OnSetGraphicsRootUnorderedAccessView(ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.SetGraphicsRootUnorderedAccessView(pCommandList, rootParameterIndex, bufferLocation);
		}
		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		SetGraphicsRootUnorderedAccessViewCommand& cmd = recorder.AddCommand<SetGraphicsRootUnorderedAccessViewCommand>();
		cmd.rootParameterIndex = rootParameterIndex;
		cmd.gpuVA = bufferLocation;
		return d3d12PFNs.SetGraphicsRootUnorderedAccessView(pCommandList, rootParameterIndex, bufferLocation);
	}

	void Vista::OnSetComputeRootUnorderedAccessView(ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.SetComputeRootUnorderedAccessView(pCommandList, rootParameterIndex, bufferLocation);
		}
		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		SetComputeRootUnorderedAccessViewCommand& cmd = recorder.AddCommand<SetComputeRootUnorderedAccessViewCommand>();
		cmd.rootParameterIndex = rootParameterIndex;
		cmd.gpuVA = bufferLocation;
		return d3d12PFNs.SetComputeRootUnorderedAccessView(pCommandList, rootParameterIndex, bufferLocation);
	}

	HRESULT Vista::OnClose(ID3D12GraphicsCommandList* pCommandList)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.Close(pCommandList);
		}
		HRESULT hr = d3d12PFNs.Close(pCommandList);
		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		CloseListCommand& cmd = recorder.AddCommand<CloseListCommand>();
		cmd.hr = hr;
		return hr;
	}

	HRESULT Vista::OnReset(ID3D12GraphicsCommandList* pCommandList, ID3D12CommandAllocator* pCommandAllocator, ID3D12PipelineState* pPipelineState)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.List_Reset(pCommandList, pCommandAllocator, pPipelineState);
		}
		HRESULT hr = d3d12PFNs.List_Reset(pCommandList, pCommandAllocator, pPipelineState);
		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		ResetListCommand& cmd = recorder.AddCommand<ResetListCommand>();
		cmd.hr = hr;
		cmd.allocatorId = objectTracker.GetObjectID(pCommandAllocator);
		cmd.psoId = objectTracker.GetObjectID(pPipelineState);
		stateTracker.OnReset(pCommandList, pCommandAllocator, pPipelineState);
		return hr;
	}

	void Vista::OnClearState(ID3D12GraphicsCommandList* pCommandList, ID3D12PipelineState* pPipelineState)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.ClearState(pCommandList, pPipelineState);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		ClearStateCommand& cmd = recorder.AddCommand<ClearStateCommand>();
		cmd.psoId = objectTracker.GetObjectID(pPipelineState);
		return d3d12PFNs.ClearState(pCommandList, pPipelineState);
	}

	void Vista::OnBeginEvent(ID3D12GraphicsCommandList* pCommandList, UINT metadata, const void* pData, UINT size)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.BeginEvent(pCommandList, metadata, pData, size);
		}

		VISTA_TODO("Include pix3.h?");
		static constexpr Uint32 D3D12_EVENT_METADATA = 2;

		std::string name;
		Uint64 color = 0xFFFFFFFF;
		if (metadata == D3D12_EVENT_METADATA && pData != nullptr && size >= (2 * sizeof(Uint64)))
		{
			Uint64 const* data64 = reinterpret_cast<Uint64 const*>(pData);
			auto ExtractPixEventName = [](Uint64 const* buffer, Uint sizeInBytes) -> std::string
				{
					std::string name;
					if (sizeInBytes < 4 * sizeof(Uint64))
					{
						return name;
					}
					Uint8 const* bytes = reinterpret_cast<const Uint8*>(&buffer[3]);
					Uint maxBytes = sizeInBytes - 3 * sizeof(Uint64);

					for (Uint i = 0; i < maxBytes; ++i)
					{
						if (bytes[i] == '\0') break;
						name += bytes[i];
					}
					return name;
				};
			name = ExtractPixEventName(data64, size);
			color = data64[1];
		}
		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		BeginEventCommand& cmd = recorder.AddCommand<BeginEventCommand>();
		cmd.eventName = name;
		cmd.eventColor = color;
		return d3d12PFNs.BeginEvent(pCommandList, metadata, pData, size);
	}

	void Vista::OnEndEvent(ID3D12GraphicsCommandList* pCommandList)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.EndEvent(pCommandList);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
		recorder.AddCommand<EndEventCommand>();
		return d3d12PFNs.EndEvent(pCommandList);
	}

	ULONG Vista::OnRelease(ID3D12Resource* pResource)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.Resource_Release(pResource);
		}
		ULONG refCount = d3d12PFNs.Resource_Release(pResource);
		if (refCount == 0)
		{
			stateTracker.OnResourceReleased(pResource);
			addressTracker.OnResourceReleased(pResource);
			mappedBufferManager.OnResourceReleased(pResource);
		}
		return refCount;
	}

	HRESULT Vista::OnSetName(ID3D12Resource* pResource, LPCWSTR name)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.Resource_SetName(pResource, name);
		}
		HRESULT hr = d3d12PFNs.Resource_SetName(pResource, name);
		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pResource);
		ResourceSetNameCommand& cmd = recorder.AddCommand<ResourceSetNameCommand>();
		std::string resourceName = ToString(name);
		cmd.resourceName = resourceName;
		cmd.hr = hr;
		objectTracker.SetDebugName(pResource, resourceName);
		return hr;
	}

	HRESULT Vista::OnMap(ID3D12Resource* pResource, UINT subresource, const D3D12_RANGE* pReadRange, void** ppData)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.Resource_Map(pResource, subresource, pReadRange, ppData);
		}
		HRESULT hr = d3d12PFNs.Resource_Map(pResource, subresource, pReadRange, ppData);
		if (SUCCEEDED(hr))
		{
			mappedBufferManager.OnMap(pResource, subresource, pReadRange, ppData);
		}
		return hr;
	}

	void Vista::OnUnmap(ID3D12Resource* pResource, UINT subresource, const D3D12_RANGE* pWrittenRange)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.Resource_Unmap(pResource, subresource, pWrittenRange);
		}
		mappedBufferManager.OnUnmap(pResource, subresource, pWrittenRange);
		return d3d12PFNs.Resource_Unmap(pResource, subresource, pWrittenRange);
	}

	UINT64 Vista::OnGetCompletedValue(ID3D12Fence* pFence)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.GetCompletedValue(pFence);
		}
		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pFence);
		GetCompletedValueCommand& cmd = recorder.AddCommand<GetCompletedValueCommand>();
		Uint64 completedValue = d3d12PFNs.GetCompletedValue(pFence);
		cmd.completedValue = completedValue;
		return completedValue;
	}

	HRESULT Vista::OnSetEventOnCompletion(ID3D12Fence* pFence, UINT64 value, HANDLE event)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.SetEventOnCompletion(pFence, value, event);
		}
		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pFence);
		SetEventOnCompletionCommand& cmd = recorder.AddCommand<SetEventOnCompletionCommand>();
		HRESULT hr = d3d12PFNs.SetEventOnCompletion(pFence, value, event);
		cmd.value = value;
		cmd.event = event;
		cmd.hr = hr;
		return hr;
	}

	HRESULT Vista::OnSignal(ID3D12Fence* pFence, UINT64 value)
	{
		if (g_isInsideVistaRender || GUI.IsFreezed())
		{
			return d3d12PFNs.Fence_Signal(pFence, value);
		}

		CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pFence);
		FenceSignalCommand& cmd = recorder.AddCommand<FenceSignalCommand>();
		HRESULT hr = d3d12PFNs.Fence_Signal(pFence, value);
		cmd.value = value;
		cmd.hr = hr;
		return hr;
	}

	void Vista::CheckForCopyRequest(ID3D12GraphicsCommandList* pCommandList, Command const& cmd)
	{
		if (std::optional<ResourceCopyRequest> request = copyRequestManager.FindMatchingRequest(cmd))
		{
			pRequestCommandList = pCommandList;

			//we cannot do copy operation and barriers if we are inside a render pass
			CommandRecorder& recorder = recorderManager.GetOrCreateRecorder(pCommandList);
			BeginRenderPassCommand const* beginRenderPassCmd = nullptr;
			Ref<ID3D12GraphicsCommandList4> pCommandList4 = nullptr;
			if (recorder.CommandCount<BeginRenderPassCommand>() > recorder.CommandCount<EndRenderPassCommand>())
			{
				beginRenderPassCmd = recorder.FindMostRecent<BeginRenderPassCommand>();
				pCommandList->QueryInterface(IID_PPV_ARGS(pCommandList4.GetAddressOf()));
				if (pCommandList4)
				{
					d3d12PFNs.EndRenderPass(pCommandList4.Get());
				}
			}

			Bool const isBuffer = (request->GetDestinationResource()->GetDesc().Dimension == D3D12_RESOURCE_DIMENSION_BUFFER);
			D3D12_RESOURCE_STATES currentState = stateTracker.QueryResourceState(pCommandList, request->GetSourceResource());

			if (isBuffer)
			{
				D3D12_RESOURCE_BARRIER barriers[1] = {};
				barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				barriers[0].Transition.pResource = request->GetSourceResource();
				barriers[0].Transition.StateBefore = currentState;
				barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;

				d3d12PFNs.ResourceBarrier(pCommandList, 1, barriers);
				d3d12PFNs.CopyResource(pCommandList, request->GetDestinationResource(), request->GetSourceResource());

				barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
				barriers[0].Transition.StateAfter = currentState;
				d3d12PFNs.ResourceBarrier(pCommandList, 1, barriers);
			}
			else
			{
				D3D12_RESOURCE_BARRIER barriers[2] = {};
				barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				barriers[0].Transition.pResource = request->GetSourceResource();
				barriers[0].Transition.StateBefore = currentState;
				barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
				barriers[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

				barriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				barriers[1].Transition.pResource = request->GetDestinationResource();
				barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
				barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
				barriers[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

				d3d12PFNs.ResourceBarrier(pCommandList, 2, barriers);
				d3d12PFNs.CopyResource(pCommandList, request->GetDestinationResource(), request->GetSourceResource());

				barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
				barriers[0].Transition.StateAfter = currentState;
				barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
				barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

				d3d12PFNs.ResourceBarrier(pCommandList, 2, barriers);
			}

			if (beginRenderPassCmd && pCommandList4)
			{
				std::vector<D3D12_RENDER_PASS_RENDER_TARGET_DESC> renderTargets = beginRenderPassCmd->renderTargets;
				for (D3D12_RENDER_PASS_RENDER_TARGET_DESC& renderTarget : renderTargets)
				{
					if (renderTarget.BeginningAccess.Type == D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR)
					{
						renderTarget.BeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
					}
				}
				if (beginRenderPassCmd->depthStencil)
				{
					D3D12_RENDER_PASS_DEPTH_STENCIL_DESC depthStencilDesc = *beginRenderPassCmd->depthStencil;
					if (depthStencilDesc.DepthBeginningAccess.Type == D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR)
					{
						depthStencilDesc.DepthBeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
					}
					if (depthStencilDesc.StencilBeginningAccess.Type == D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR)
					{
						depthStencilDesc.StencilBeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
					}

					d3d12PFNs.BeginRenderPass(pCommandList4.Get(), (Uint32)renderTargets.size(), renderTargets.data(),
											  &depthStencilDesc, beginRenderPassCmd->flags);
				}
				else
				{
					d3d12PFNs.BeginRenderPass(pCommandList4.Get(), (Uint32)renderTargets.size(), renderTargets.data(),
											  nullptr, beginRenderPassCmd->flags);
				}
			}
		}
	}
}

