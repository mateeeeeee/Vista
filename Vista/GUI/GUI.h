#pragma once
#include "ImGuiManager.h"
#include "SelectedCommandInfo.h"
#include "Tracking/State.h"

namespace vista
{
	class ObjectTracker;
	class DescriptorTracker;
	class ResourceAddressTracker;
	class ResourceCopyRequestManager;
	class Command;
	class RecorderManager;
	struct SelectedItem;

	class GUI
	{
	public:
		GUI(ObjectTracker const& objectTracker, 
			DescriptorTracker const& descriptorTracker, 
			ResourceAddressTracker const& addressTracker, 
			ResourceCopyRequestManager& copyRequestManager)
			: objectTracker(objectTracker), descriptorTracker(descriptorTracker), addressTracker(addressTracker), copyRequestManager(copyRequestManager) {}
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
		ResourceCopyRequestManager& copyRequestManager;
		ImGuiManager imguiManager;
		Bool isFreezed = false;

		SelectedCommandInfo selectedCommandInfo{};
		Command const* selectedCommand = nullptr;
		State currentState{};

	private:
		void RenderEventList(RecorderManager const&, std::string const&);
		void UpdateCurrentState(RecorderManager const&);
		void RenderTabs(RecorderManager const&);
		void RenderResourceViewer();

		void RenderObjectsTab();
		void RenderDescriptorHeapTab();
		void RenderResourceTab();
		void RenderStateTab(RecorderManager const&);
	};
}