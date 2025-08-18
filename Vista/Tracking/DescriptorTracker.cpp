#include "DescriptorTracker.h"
#include "ObjectTracker.h"

namespace vista
{
	namespace
	{
		inline Bool IsValidHeapTypeForViewType(D3D12_DESCRIPTOR_HEAP_TYPE heapType, DescriptorViewType viewType)
		{
			switch (viewType)
			{
			case DescriptorViewType::CBV:
			case DescriptorViewType::SRV:
			case DescriptorViewType::UAV:
				return (heapType == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			case DescriptorViewType::Sampler:
				return (heapType == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

			case DescriptorViewType::RTV:
				return (heapType == D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

			case DescriptorViewType::DSV:
				return (heapType == D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
			case DescriptorViewType::Unknown:
			case DescriptorViewType::Uninitialized:
			default:
				return false;
			}
			return false;
		}
	}

	void DescriptorTracker::AddDescriptorHeap(ID3D12DescriptorHeap* heap)
	{
		if (!heap)
		{
			return;
		}

		if (!device)
		{
			heap->GetDevice(IID_PPV_ARGS(device.ReleaseAndGetAddressOf()));
			VISTA_ASSERT_MSG(device != nullptr, "ID3D12DescriptorHeap::GetDevice returned NULL!?");
		}

		ObjectID heapId = objectManager.GetObjectID(heap);
		if (heapId == InvalidObjectID)
		{
			return;
		}

		std::lock_guard lock(trackerMutex);
		if (trackedHeaps.contains(heapId))
		{
			return;
		}

		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = heap->GetDesc();
		TrackedDescriptorHeapInfo heapInfo{};
		heapInfo.heapId = heapId;
		heapInfo.cpuStart = heap->GetCPUDescriptorHandleForHeapStart();
		heapInfo.type = heapDesc.Type;
		heapInfo.descriptorCount = heapDesc.NumDescriptors;
		heapInfo.isShaderVisible = (heapDesc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) != 0;
		if (heapInfo.isShaderVisible)
		{
			heapInfo.gpuStart = heap->GetGPUDescriptorHandleForHeapStart();
		}

		heapInfo.descriptorSize = device->GetDescriptorHandleIncrementSize(heapDesc.Type);
		heapInfo.descriptors.resize(heapInfo.descriptorCount);
		trackedHeaps.emplace(heapId, std::move(heapInfo));
	}

	std::pair<TrackedDescriptorHeapInfo const*, Int> DescriptorTracker::FindDescriptorByCpuHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle) const
	{
		std::lock_guard lock(trackerMutex);
		return FindDescriptorByCpuHandle_Internal(handle);
	}
	std::pair<TrackedDescriptorHeapInfo const*, Int> DescriptorTracker::FindDescriptorByGpuHandle(D3D12_GPU_DESCRIPTOR_HANDLE handle) const
	{
		std::lock_guard lock(trackerMutex);
		return FindDescriptorByGpuHandle_Internal(handle);
	}

	void DescriptorTracker::OnCreateCBV(D3D12_CPU_DESCRIPTOR_HANDLE destHandle, D3D12_CONSTANT_BUFFER_VIEW_DESC const* pDesc)
	{
		UpdateDescriptor(destHandle, DescriptorViewType::CBV, nullptr, pDesc ? *pDesc : D3D12_CONSTANT_BUFFER_VIEW_DESC{});
	}
	void DescriptorTracker::OnCreateSRV(D3D12_CPU_DESCRIPTOR_HANDLE destHandle, ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC const* pDesc)
	{
		UpdateDescriptor(destHandle, DescriptorViewType::SRV, resource, pDesc ? *pDesc : D3D12_SHADER_RESOURCE_VIEW_DESC{});
	}
	void DescriptorTracker::OnCreateUAV(D3D12_CPU_DESCRIPTOR_HANDLE destHandle, ID3D12Resource* resource, ID3D12Resource*, D3D12_UNORDERED_ACCESS_VIEW_DESC const* pDesc)
	{
		UpdateDescriptor(destHandle, DescriptorViewType::UAV, resource, pDesc ? *pDesc : D3D12_UNORDERED_ACCESS_VIEW_DESC{});
	}
	void DescriptorTracker::OnCreateSampler(D3D12_CPU_DESCRIPTOR_HANDLE destHandle, D3D12_SAMPLER_DESC const* pDesc)
	{
		UpdateDescriptor(destHandle, DescriptorViewType::Sampler, nullptr, pDesc ? *pDesc : D3D12_SAMPLER_DESC{});
	}
	void DescriptorTracker::OnCreateRTV(D3D12_CPU_DESCRIPTOR_HANDLE destHandle, ID3D12Resource* resource, D3D12_RENDER_TARGET_VIEW_DESC const* pDesc)
	{
		UpdateDescriptor(destHandle, DescriptorViewType::RTV, resource, pDesc ? *pDesc : D3D12_RENDER_TARGET_VIEW_DESC{});
	}
	void DescriptorTracker::OnCreateDSV(D3D12_CPU_DESCRIPTOR_HANDLE destHandle, ID3D12Resource* resource, D3D12_DEPTH_STENCIL_VIEW_DESC const* pDesc)
	{
		UpdateDescriptor(destHandle, DescriptorViewType::DSV, resource, pDesc ? *pDesc : D3D12_DEPTH_STENCIL_VIEW_DESC{});
	}

	void DescriptorTracker::OnCopyDescriptorsSimple(UINT numDescriptors, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptorRangeStart, D3D12_CPU_DESCRIPTOR_HANDLE srcDescriptorRangeStart, D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapsType)
	{
		if (numDescriptors == 0) 
		{
			return; 
		}
		TrackedDescriptorHeapInfo const* srcHeapInfo = nullptr;
		Int srcStartIndex = -1;
		{ 
			std::lock_guard lock(trackerMutex);
			auto [foundSrcHeap, foundSrcIndex] = FindDescriptorByCpuHandle_Internal(srcDescriptorRangeStart);
			srcHeapInfo = foundSrcHeap; 
			srcStartIndex = foundSrcIndex;
		} 
		if (!srcHeapInfo || srcHeapInfo->isShaderVisible || srcStartIndex < 0)
		{
			return;
		}

		TrackedDescriptorHeapInfo* destHeapInfo = nullptr; 
		Int destStartIndex = -1;
		{ 
			std::lock_guard lock(trackerMutex);
			auto [foundDestHeap, foundDestIndex] = FindDescriptorByCpuHandle_Internal(destDescriptorRangeStart);
			destHeapInfo = foundDestHeap;
			destStartIndex = foundDestIndex;
		} 

		if (!destHeapInfo || destStartIndex < 0) 
		{
			return;
		}

		if (srcHeapInfo->type != descriptorHeapsType || destHeapInfo->type != descriptorHeapsType) 
		{
			return; 
		}

		if (static_cast<Uint32>(srcStartIndex) + numDescriptors > srcHeapInfo->descriptorCount ||
			static_cast<Uint32>(destStartIndex) + numDescriptors > destHeapInfo->descriptorCount) 
		{
			VISTA_ASSERT_MSG(false, "CopyDescriptorsSimple: Copy range exceeds heap bounds.");
			return;
		}

		{ 
			std::lock_guard lock(trackerMutex);
			std::vector<DescriptorInfo> const& srcDescriptors = srcHeapInfo->descriptors;
			std::vector<DescriptorInfo>& destDescriptors = destHeapInfo->descriptors;
			for (Uint32 i = 0; i < numDescriptors; ++i) 
			{
				Uint32 srcIndex = static_cast<Uint32>(srcStartIndex) + i;
				Uint32 destIndex = static_cast<Uint32>(destStartIndex) + i;
				destDescriptors[destIndex] = srcDescriptors[srcIndex];
			}
		} 
	}
	void DescriptorTracker::OnCopyDescriptors(
		UINT numDestDescriptorRanges,
		const D3D12_CPU_DESCRIPTOR_HANDLE* pDestDescriptorRangeStarts,
		const UINT* pDestDescriptorRangeSizes,
		UINT numSrcDescriptorRanges,
		const D3D12_CPU_DESCRIPTOR_HANDLE* pSrcDescriptorRangeStarts,
		const UINT* pSrcDescriptorRangeSizes,
		D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapsType)
	{
		if (numDestDescriptorRanges == 0 || numSrcDescriptorRanges == 0) 
		{
			return;
		}

		if (!pDestDescriptorRangeStarts || !pSrcDescriptorRangeStarts) 
		{
			VISTA_ASSERT_MSG(false, "CopyDescriptors: Invalid pointer or range configuration.");
			return;
		}

		Uint32 totalDestDescriptors = 0;
		Uint32 totalSrcDescriptors = 0;

		for (Uint32 i = 0; i < numDestDescriptorRanges; ++i)
		{
			totalDestDescriptors += pDestDescriptorRangeSizes[i];
		}
		for (Uint32 i = 0; i < numSrcDescriptorRanges; ++i) 
		{
			totalSrcDescriptors += pSrcDescriptorRangeSizes[i];
		}

		if (totalSrcDescriptors != totalDestDescriptors) 
		{
			VISTA_ASSERT_MSG(false, "CopyDescriptors: Total source and destination descriptor counts must match and be > 0.");
			return;
		}

		Uint32 descriptorSize = device->GetDescriptorHandleIncrementSize(descriptorHeapsType);
		if (descriptorSize == 0) 
		{
			VISTA_ASSERT_MSG(false, "CopyDescriptors: Failed to get valid descriptor increment size.");
			return;
		}

		struct RangeInfo 
		{
			D3D12_CPU_DESCRIPTOR_HANDLE startHandle;
			Uint32 size;
		};

		std::vector<RangeInfo> srcRanges(numSrcDescriptorRanges);
		std::vector<RangeInfo> destRanges(numDestDescriptorRanges);

		for (Uint32 i = 0; i < numSrcDescriptorRanges; ++i) 
		{
			srcRanges[i] = { pSrcDescriptorRangeStarts[i], pSrcDescriptorRangeSizes[i] };
		}
		for (Uint32 i = 0; i < numDestDescriptorRanges; ++i) 
		{
			destRanges[i] = { pDestDescriptorRangeStarts[i], pDestDescriptorRangeSizes[i] };
		}

		std::lock_guard lock(trackerMutex);

		Uint32 srcRangeIndex = 0;
		Uint32 srcOffset = 0;
		D3D12_CPU_DESCRIPTOR_HANDLE srcHandle = srcRanges[0].startHandle;

		Uint32 destRangeIndex = 0;
		Uint32 destOffset = 0;
		D3D12_CPU_DESCRIPTOR_HANDLE destHandle = destRanges[0].startHandle;

		Uint32 remainingInCurrentSrcRange = srcRanges[srcRangeIndex].size;
		Uint32 remainingInCurrentDestRange = destRanges[destRangeIndex].size;

		for (Uint32 count = 0; count < totalSrcDescriptors; ++count) 
		{
			auto [srcHeapInfoPtr, srcIndex] = FindDescriptorByCpuHandle_Internal(srcHandle);
			if (!srcHeapInfoPtr || srcIndex < 0 || srcHeapInfoPtr->type != descriptorHeapsType ||
				(Uint32)srcIndex >= srcHeapInfoPtr->descriptorCount || srcHeapInfoPtr->isShaderVisible)
			{
				VISTA_ASSERT_MSG(false, "CopyDescriptors: Invalid source descriptor.");
				return;
			}

			auto [destHeapInfoPtr, destIndex] = FindDescriptorByCpuHandle_Internal(destHandle);
			if (!destHeapInfoPtr || destIndex < 0 || destHeapInfoPtr->type != descriptorHeapsType ||
				(Uint32)destIndex >= destHeapInfoPtr->descriptorCount) 
			{
				VISTA_ASSERT_MSG(false, "CopyDescriptors: Invalid destination descriptor.");
				return;
			}

			destHeapInfoPtr->descriptors[destIndex] = srcHeapInfoPtr->descriptors[srcIndex];

			--remainingInCurrentSrcRange;
			if (remainingInCurrentSrcRange == 0 && srcRangeIndex + 1 < numSrcDescriptorRanges) 
			{
				++srcRangeIndex;
				srcHandle = srcRanges[srcRangeIndex].startHandle;
				remainingInCurrentSrcRange = srcRanges[srcRangeIndex].size;
			}
			else 
			{
				srcHandle.ptr += descriptorSize;
			}

			--remainingInCurrentDestRange;
			if (remainingInCurrentDestRange == 0 && destRangeIndex + 1 < numDestDescriptorRanges) 
			{
				++destRangeIndex;
				destHandle = destRanges[destRangeIndex].startHandle;
				remainingInCurrentDestRange = destRanges[destRangeIndex].size;
			}
			else 
			{
				destHandle.ptr += descriptorSize;
			}
		}
	}

	TrackedDescriptorHeapInfo const* DescriptorTracker::GetHeapInfo(ObjectID heapId) const
	{
		std::lock_guard lock(trackerMutex);
		auto it = trackedHeaps.find(heapId);
		return (it != trackedHeaps.end()) ? &(it->second) : nullptr;
	}
	DescriptorInfo const* DescriptorTracker::GetDescriptorInfo(ObjectID heapId, Uint32 index) const
	{
		std::lock_guard lock(trackerMutex);
		auto it = trackedHeaps.find(heapId);
		if (it != trackedHeaps.end()) 
		{
			TrackedDescriptorHeapInfo const& heapInfo = it->second;
			if (index < heapInfo.descriptorCount) 
			{
				return &(heapInfo.descriptors[index]);
			}
		}
		return nullptr;
	}
	std::unordered_map<ObjectID, TrackedDescriptorHeapInfo> const& DescriptorTracker::GetTrackedHeaps() const
	{
		return trackedHeaps;
	}

	std::pair<TrackedDescriptorHeapInfo*, Int> DescriptorTracker::FindDescriptorByCpuHandle_Internal(D3D12_CPU_DESCRIPTOR_HANDLE handle)
	{
		if (handle.ptr == NULL)
		{
			return { nullptr, -1 };
		}

		for (auto& [id, heapInfo] : trackedHeaps)
		{
			if (heapInfo.descriptorSize == 0) continue;

			Uint64 start = heapInfo.cpuStart.ptr;
			Uint64 end = start + heapInfo.descriptorCount * heapInfo.descriptorSize;
			if (handle.ptr >= start && handle.ptr < end)
			{
				if ((handle.ptr - start) % heapInfo.descriptorSize == 0)
				{
					Int index = static_cast<Int>((handle.ptr - start) / heapInfo.descriptorSize);
					return { &heapInfo, index };
				}
				else
				{
					return { nullptr, -1 };
				}
			}
		}
		return { nullptr, -1 };
	}
	std::pair<TrackedDescriptorHeapInfo const*, Int> DescriptorTracker::FindDescriptorByCpuHandle_Internal(D3D12_CPU_DESCRIPTOR_HANDLE handle) const
	{
		auto nonConstResult = const_cast<DescriptorTracker*>(this)->FindDescriptorByCpuHandle_Internal(handle);
		return { static_cast<TrackedDescriptorHeapInfo const*>(nonConstResult.first), nonConstResult.second };
	}
	std::pair<TrackedDescriptorHeapInfo*, Int> DescriptorTracker::FindDescriptorByGpuHandle_Internal(D3D12_GPU_DESCRIPTOR_HANDLE handle)
	{
		if (handle.ptr == NULL)
		{
			return { nullptr, -1 };
		}

		for (auto& [id, heapInfo] : trackedHeaps)
		{
			if (!heapInfo.isShaderVisible || heapInfo.descriptorSize == 0) continue;

			Uint64 start = heapInfo.gpuStart.ptr;
			Uint64 end = start + heapInfo.descriptorCount * heapInfo.descriptorSize;
			if (handle.ptr >= start && handle.ptr < end)
			{
				if ((handle.ptr - start) % heapInfo.descriptorSize == 0)
				{
					Int index = static_cast<Int>((handle.ptr - start) / heapInfo.descriptorSize);
					return { &heapInfo, index };
				}
				else
				{
					return { nullptr, -1 };
				}
			}
		}
		return { nullptr, -1 };
	}
	std::pair<TrackedDescriptorHeapInfo const*, Int> DescriptorTracker::FindDescriptorByGpuHandle_Internal(D3D12_GPU_DESCRIPTOR_HANDLE handle) const
	{
		auto nonConstResult = const_cast<DescriptorTracker*>(this)->FindDescriptorByGpuHandle_Internal(handle);
		return { static_cast<TrackedDescriptorHeapInfo const*>(nonConstResult.first), nonConstResult.second };
	}

	template<typename ViewDescT>
	void DescriptorTracker::UpdateDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE destHandle, DescriptorViewType viewType, ID3D12Resource* resource, ViewDescT const& viewDesc)
	{
		std::lock_guard lock(trackerMutex); 
		auto [heapInfoPtr, index] = FindDescriptorByCpuHandle_Internal(destHandle);
		
		if (heapInfoPtr && index >= 0) 
		{
			if (!IsValidHeapTypeForViewType(heapInfoPtr->type, viewType))
			{
				VISTA_ASSERT_MSG(false, "Descriptor view type is incompatible with heap type!");
				return;
			}
			DescriptorInfo& descInfo = heapInfoPtr->descriptors[index];
			descInfo.type = viewType;
			descInfo.resourceId = objectManager.GetObjectID(resource);
			descInfo.desc = viewDesc; 
		}
	}
}

