#pragma once
#include "Traits/DescriptorHeapTraits.h"

namespace vista
{
	enum class DescriptorViewType : Uint8
	{
		Unknown,
		Uninitialized,
		CBV,
		SRV,
		UAV,
		Sampler,
		RTV,
		DSV
	};
	inline Char const* DescriptorViewTypeToString(DescriptorViewType type)
	{
		switch (type)
		{
		case DescriptorViewType::CBV:           return "CBV";
		case DescriptorViewType::SRV:           return "SRV";
		case DescriptorViewType::UAV:           return "UAV";
		case DescriptorViewType::Sampler:       return "Sampler";
		case DescriptorViewType::RTV:           return "RTV";
		case DescriptorViewType::DSV:           return "DSV";
		case DescriptorViewType::Uninitialized: return "Uninitialized"; 
		case DescriptorViewType::Unknown:       
		default:                                return "Unknown";
		}
	}

	using DescriptorDesc = std::variant<std::monostate, D3D12_CONSTANT_BUFFER_VIEW_DESC, D3D12_SHADER_RESOURCE_VIEW_DESC, D3D12_UNORDERED_ACCESS_VIEW_DESC,
										D3D12_SAMPLER_DESC, D3D12_RENDER_TARGET_VIEW_DESC, D3D12_DEPTH_STENCIL_VIEW_DESC>;

	struct DescriptorInfo
	{
		DescriptorViewType type = DescriptorViewType::Unknown;
		ObjectID resourceId = InvalidObjectID; 
		DescriptorDesc desc = std::monostate{};
	};

	struct TrackedDescriptorHeapInfo
	{
		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(Uint32 index) const 
		{
			VISTA_ASSERT(index < descriptorCount);
			return { cpuStart.ptr + static_cast<SIZE_T>(index) * descriptorSize };
		}

		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(Uint32 index) const 
		{
			VISTA_ASSERT(index < descriptorCount);
			VISTA_ASSERT(isShaderVisible); 
			return { gpuStart.ptr + static_cast<UINT64>(index) * descriptorSize };
		}

		ObjectID heapId = InvalidObjectID;
		D3D12_CPU_DESCRIPTOR_HANDLE cpuStart{};
		D3D12_GPU_DESCRIPTOR_HANDLE gpuStart{};
		D3D12_DESCRIPTOR_HEAP_TYPE type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		Uint32 descriptorCount = 0;
		Uint32 descriptorSize = 0;
		Bool isShaderVisible = false;
		std::vector<DescriptorInfo> descriptors;
	};

	class ObjectTracker;
	class DescriptorTracker
	{
	public:
		explicit DescriptorTracker(ObjectTracker& objManager) : objectManager(objManager) {}
		VISTA_NONCOPYABLE_NONMOVABLE(DescriptorTracker)
		~DescriptorTracker() = default;

		void AddDescriptorHeap(ID3D12DescriptorHeap* heap);

		void OnCreateCBV(D3D12_CPU_DESCRIPTOR_HANDLE destHandle, D3D12_CONSTANT_BUFFER_VIEW_DESC const* pDesc);
		void OnCreateSRV(D3D12_CPU_DESCRIPTOR_HANDLE destHandle, ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC const* pDesc);
		void OnCreateUAV(D3D12_CPU_DESCRIPTOR_HANDLE destHandle, ID3D12Resource* resource, ID3D12Resource* pCounterResource, D3D12_UNORDERED_ACCESS_VIEW_DESC const* pDesc);
		void OnCreateSampler(D3D12_CPU_DESCRIPTOR_HANDLE destHandle, D3D12_SAMPLER_DESC const* pDesc);
		void OnCreateRTV(D3D12_CPU_DESCRIPTOR_HANDLE destHandle, ID3D12Resource* resource, D3D12_RENDER_TARGET_VIEW_DESC const* pDesc);
		void OnCreateDSV(D3D12_CPU_DESCRIPTOR_HANDLE destHandle, ID3D12Resource* resource, D3D12_DEPTH_STENCIL_VIEW_DESC const* pDesc);
		void OnCopyDescriptorsSimple(UINT numDescriptors, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptorRangeStart, D3D12_CPU_DESCRIPTOR_HANDLE srcDescriptorRangeStart, D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapsType);
		void OnCopyDescriptors(UINT numDestDescriptorRanges, const D3D12_CPU_DESCRIPTOR_HANDLE* pDestDescriptorRangeStarts, const UINT* pDestDescriptorRangeSizes,
							   UINT numSrcDescriptorRanges, const D3D12_CPU_DESCRIPTOR_HANDLE* pSrcDescriptorRangeStarts, const UINT* pSrcDescriptorRangeSizes, D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapsType);

		std::pair<TrackedDescriptorHeapInfo const*, Int> FindDescriptorByCpuHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle) const;
		std::pair<TrackedDescriptorHeapInfo const*, Int> FindDescriptorByGpuHandle(D3D12_GPU_DESCRIPTOR_HANDLE handle) const;

		TrackedDescriptorHeapInfo const* GetHeapInfo(ObjectID heapId) const;
		DescriptorInfo const* GetDescriptorInfo(ObjectID heapId, Uint32 index) const;
		std::unordered_map<ObjectID, TrackedDescriptorHeapInfo> const& GetTrackedHeaps() const;

	private:
		ObjectTracker& objectManager; 
		Ref<ID3D12Device> device = nullptr; 

		std::unordered_map<ObjectID, TrackedDescriptorHeapInfo> trackedHeaps;
		mutable std::mutex trackerMutex; 

	private:
		std::pair<TrackedDescriptorHeapInfo*, Int>			FindDescriptorByCpuHandle_Internal(D3D12_CPU_DESCRIPTOR_HANDLE handle);
		std::pair<TrackedDescriptorHeapInfo const*, Int>	FindDescriptorByCpuHandle_Internal(D3D12_CPU_DESCRIPTOR_HANDLE handle) const;

		std::pair<TrackedDescriptorHeapInfo*, Int>			FindDescriptorByGpuHandle_Internal(D3D12_GPU_DESCRIPTOR_HANDLE handle);
		std::pair<TrackedDescriptorHeapInfo const*, Int>	FindDescriptorByGpuHandle_Internal(D3D12_GPU_DESCRIPTOR_HANDLE handle) const;

		template<typename ViewDescT>
		void UpdateDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE destHandle, DescriptorViewType viewType, ID3D12Resource* resource, ViewDescT const& viewDesc);
	};

}