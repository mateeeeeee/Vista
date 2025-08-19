#include "Tracking/DescriptorTracker.h"
#include "Tracking/Traits/AllObjectTraits.h"
#include "Utilities/D3D12Util.h"

struct D3D12_SAMPLER_DESC;
struct D3D12_STATIC_SAMPLER_DESC;
struct D3D12_VERTEX_BUFFER_VIEW;
struct D3D12_INDEX_BUFFER_VIEW;
struct D3D12_VIEWPORT;
struct D3D12_RESOURCE_BARRIER;
struct D3D12_INDIRECT_ARGUMENT_DESC;

namespace vista
{
	class ObjectTracker;
	class ResourceAddressTracker;
	struct TrackedObjectInfo;
	struct SelectedItem;
	struct RootParameterBinding;

	void RenderSamplerDesc(D3D12_SAMPLER_DESC const& samplerDesc, Char const* labelPrefix = "Sampler");
	void RenderSamplerDesc(D3D12_STATIC_SAMPLER_DESC const& staticDesc, Char const* labelPrefix = "Sampler");

	void RenderDescriptorInfoByHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle, Char const* label, DescriptorTracker const& descriptorTracker);
	void RenderDescriptorInfoByHandle(D3D12_GPU_DESCRIPTOR_HANDLE handle, Char const* label, DescriptorTracker const& descriptorTracker);

	void RenderDescriptorInfo(DescriptorInfo const& descInfo, Int descriptorIndex = 0);
	void RenderResourceDesc(ResourceDesc const& desc);
	void RenderDescriptorHeapDesc(DescriptorHeapDesc const& desc);
	void RenderQueueDesc(QueueDesc const& desc);
	void RenderListDesc(ListDesc const& desc);
	void RenderAllocatorDesc(AllocatorDesc const& desc);
	void RenderFenceDesc(FenceDesc const& desc);
	void RenderRootSignatureDesc(RootSignatureDesc const& desc);
	void RenderGraphicsPSODetails(GraphicsPSODescStorage const& desc, ObjectTracker const& objectTracker);
	void RenderComputePSODetails(ComputePSODescStorage const& desc, ObjectTracker const& objectTracker);
	void RenderStreamPSODetails(StreamPSODescStorage const& desc, ObjectTracker const& objectTracker);
	void RenderCommandSignatureDesc(CommandSignatureDesc const& desc, ObjectTracker const& objectTracker);
	void RenderHeapDesc(HeapDesc const& desc);
	void RenderStateSubobjectDesc(StateObjectDesc const& desc, StateObjectDesc::Subobject const& subobject, ObjectTracker const& objectTracker);
	void RenderStateObjectDesc(StateObjectDesc const& desc, ObjectTracker const& objectTracker);

	void RenderVertexBufferView(Uint32 slot, D3D12_VERTEX_BUFFER_VIEW const& vbv, ObjectTracker const& objectTracker, ResourceAddressTracker const& addressTracker);
	void RenderIndexBufferView(D3D12_INDEX_BUFFER_VIEW const& ibv, ObjectTracker const& objectTracker, ResourceAddressTracker const& addressTracker);
	void RenderViewport(Uint32 index, D3D12_VIEWPORT const& vp);
	void RenderScissorRect(Uint32 index, D3D12_RECT const& rect);
	void RenderScissorRect(D3D12_RECT const& rect);
	void RenderResourceBarrier(Uint32 index, D3D12_RESOURCE_BARRIER const& barrier, ObjectTracker const& objectTracker);
	void RenderIndirectArgumentDesc(Uint32 index, D3D12_INDIRECT_ARGUMENT_DESC const& argDesc);

	void RenderRootParameterBinding(
		Uint32 index,
		RootParameterBinding const& binding,
		D3D12_SHADER_VISIBILITY expectedVisibility,
		ObjectTracker const& objectTracker,
		DescriptorTracker const& descriptorTracker,
		ResourceAddressTracker const& addressTracker,
		SelectedItem* selectedItemInViewer = nullptr
	);
	void RenderRootSignatureDetails(
		ObjectID rootSignatureId,
		ObjectTracker const& objectTracker,
		DescriptorTracker const& descriptorTracker,
		ResourceAddressTracker const& addressTracker,
		std::span<RootParameterBinding const> rootArgs,
		Char const* rootSignatureLabel = "Root Signature Parameters");

	void RenderBindlessParameter(
		ObjectID heapId, 
		Uint64 resourceDescriptorHeapIndex, 
		ObjectTracker const& objectTracker, 
		DescriptorTracker const& descriptorTracker, 
		SelectedItem* selectedItemInViewer = nullptr);

	TrackedObjectInfo const* RenderObjectInfoByID(ObjectID id, ObjectTracker const& tracker, Char const* label = "");
	std::string GetResourceLabel(Char const* prefix, D3D12_GPU_VIRTUAL_ADDRESS address, ObjectTracker const& objectTracker, ResourceAddressTracker const& addressTracker);
	std::string GetResourceLabel(Char const* prefix, ID3D12Resource* resource, ObjectTracker const& objectTracker, ResourceAddressTracker const& addressTracker);
	void RenderSelectableResource(std::string const& label, ID3D12Resource* resource, DescriptorInfo const& descInfo, SelectedItem& selectedItem, void* id);
}