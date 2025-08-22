#pragma once
#include "ImGuiManager.h"
#include "SelectedItem.h"
#include "Tracking/State.h"

namespace vista
{
	class ObjectTracker;
	class DescriptorTracker;
	class ResourceAddressTracker;
	class ResourceCopyRequestManager;
	class MappedBufferManager;
	class BindlessAccessCache;
	class Command;
	class RecorderManager;
	class ListCommand;
	struct SelectedItem;
	enum class ShaderType : Uint8;

	struct Globals
	{
		ObjectTracker const& objectTracker;
		DescriptorTracker const& descriptorTracker;
		ResourceAddressTracker const& addressTracker;
		MappedBufferManager const& mappedBufferManager;
		ResourceCopyRequestManager& copyRequestManager;
		BindlessAccessCache& bindlessAccessCache;
	};

	class GUI
	{
	public:
		explicit GUI(Globals globals)
			: objectTracker(globals.objectTracker), descriptorTracker(globals.descriptorTracker), 
			  addressTracker(globals.addressTracker), mappedBufferManager(globals.mappedBufferManager),
			  copyRequestManager(globals.copyRequestManager), bindlessAccessCache(globals.bindlessAccessCache)
		{}
		~GUI() = default;

		Bool Initialize(ID3D12Device*);

		Bool BeginRender();
		void Render(RecorderManager const&);
		void EndRender();

		Bool IsFreezed() const { return isFreezed; }

	private:
		ObjectTracker const& objectTracker;
		DescriptorTracker const& descriptorTracker;
		ResourceAddressTracker const& addressTracker;
		MappedBufferManager const& mappedBufferManager;
		ResourceCopyRequestManager& copyRequestManager;
		BindlessAccessCache& bindlessAccessCache;

		ImGuiManager imguiManager;
		Bool isFreezed = false;

		SelectedCommandInfo selectedCommandInfo{};
		Command const* selectedCommand = nullptr;
		State currentState{};

	private:
		void RenderEventList(RecorderManager const&);
		void UpdateCurrentState(RecorderManager const&);
		void RenderTabs(RecorderManager const&);
		void RenderResourceViewer();

		void RenderObjectsTab();
		void RenderDescriptorHeapTab();
		void RenderResourceTab();
		void RenderStateTab(RecorderManager const&);

		void RenderDrawCommandBoundResources(SelectedItem&);
		void RenderDispatchCommandBoundResources(SelectedItem&);
		void RenderCopyCommandBoundResources(SelectedItem&, ListCommand const*);
		void RenderDispatchMeshCommandBoundResources(SelectedItem&);
		void RenderDispatchRaysCommandBoundResources(SelectedItem&);

		void RenderTexture2DPreview(ID3D12Resource*);
		void RenderTexture3DPreview(ID3D12Resource*);
		void RenderBufferPreview(ID3D12Resource*);

		void RenderBindlessParameters(ShaderType shaderType, SelectedItem* selectedItemInViewer);

		void RenderVertexBindlessParameters(SelectedItem* selectedItemInViewer);
		void RenderPixelBindlessParameters(SelectedItem* selectedItemInViewer);
		void RenderComputeBindlessParameters(SelectedItem* selectedItemInViewer);
		void RenderMeshBindlessParameters(SelectedItem* selectedItemInViewer);
		void RenderAmplificationBindlessParameters(SelectedItem* selectedItemInViewer);
	};
}