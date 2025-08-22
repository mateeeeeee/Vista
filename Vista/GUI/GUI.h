#pragma once
#include "ImGuiManager.h"
#include "ResourceViewer.h"
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

	class GUI
	{
	public:
		GUI();
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
		ResourceViewer resourceViewer;
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

		void RenderBindlessParameters(ShaderType shaderType, SelectedItem* selectedItemInViewer);
	};
}