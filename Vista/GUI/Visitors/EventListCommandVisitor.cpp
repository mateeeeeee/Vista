#include "EventListCommandVisitor.h"
#include "GUI/SelectedItem.h"
#include "Command/Commands.h"
#include "Command/CommandRecorder.h"
#include "ImGui/imgui.h"

namespace vista
{
	Bool EventListCommandVisitor::MatchesFilter(std::string const& desc) const
	{
		if (filterTextLower.empty())
			return true;

		std::string descLower = desc;
		std::transform(descLower.begin(), descLower.end(), descLower.begin(), [](Uchar c) { return std::tolower(c); });
		return descLower.find(filterTextLower) != std::string::npos;
	}

	Bool EventListCommandVisitor::IsRenderingEnabled() const
	{
		if (flatten) return true;
		for (Bool isOpen : openTreeNodes)
		{
			if (!isOpen) return false;
		}
		return true;
	}

	EventListCommandVisitor::EventListCommandVisitor(Bool cpuTimeline, Bool flatten, std::string const& filterText, Command const*& selectedCommand, SelectedCommandInfo& selectedCommandInfo)
		: cpuTimeline(cpuTimeline), flatten(flatten), selectedCommand(selectedCommand), selectedCommandInfo(selectedCommandInfo), oldSelectedCommandInfo(selectedCommandInfo)
	{
		filterTextLower.resize(filterText.size());
		std::transform(filterText.begin(), filterText.end(), filterTextLower.begin(), [](Uchar c) { return std::tolower(c); });
	}

	EventListCommandVisitor::~EventListCommandVisitor()
	{
		HandleOpenNodes();
	}

	void EventListCommandVisitor::HandleOpenNodes()
	{
		while (!eventMatchStack.empty()) eventMatchStack.pop_back();

		if (!flatten)
		{
			while (!openTreeNodes.empty())
			{
				Bool wasOpen = openTreeNodes.back();
				openTreeNodes.pop_back();

				if (wasOpen && IsRenderingEnabled())
				{
					indentLevel--;
					ImGui::TreePop();
				}
			}
		}
	}

	void EventListCommandVisitor::VisitCommon(Command const& cmd)
	{
		Bool isMatchingTimeline = !(cpuTimeline ^ cmd.IsCPUTimeline());
		Bool isMatchingFilter = MatchesFilter(cmd.GetDesc());

		if (isMatchingTimeline && isMatchingFilter)
		{
			if (!flatten && !eventMatchStack.empty())
				eventMatchStack.back() = true;

			if (IsRenderingEnabled())
			{
				ImGui::PushID((Int)cmd.GetCommandIndex());
				Bool isSelected = IsSelected(cmd);
				if (!flatten) ImGui::Indent(indentLevel * 10.0f);
				ImGui::Selectable(cmd.GetDesc().c_str(), isSelected);
				if (!flatten) ImGui::Unindent(indentLevel * 10.0f);
				if (ImGui::IsItemClicked(0))
				{
					FillSelectedCommandInfo(cmd);
				}
				ImGui::PopID();
			}
		}
	}

	void EventListCommandVisitor::Visit(ExecuteCommandListsCommand const& cmd)
	{
		Bool isMatchingTimeline = !(cpuTimeline ^ cmd.IsCPUTimeline());
		Bool isMatchingFilter = MatchesFilter(cmd.GetDesc());

		if (isMatchingTimeline && isMatchingFilter)
		{
			if (!flatten && !eventMatchStack.empty())
				eventMatchStack.back() = true;

			if (IsRenderingEnabled())
			{
				ImGui::PushID(&cmd);
				Bool isSelected = IsSelected(cmd);
				if (!flatten) ImGui::Indent(indentLevel * 10.0f);
				ImGui::Selectable(cmd.GetDesc().c_str(), isSelected);
				if (!flatten) ImGui::Unindent(indentLevel * 10.0f);
				if (ImGui::IsItemClicked(0))
				{
					FillSelectedCommandInfo(cmd);
				}
				ImGui::PopID();
			}
		}

		Int commandIndex = 0;
		for (CommandRecorder* listRecorder : cmd.recorders)
		{
			if (listRecorder)
			{
				ImGui::PushID(commandIndex++);
				listRecorder->ForAllCommands([&](Command const& listCmd)
					{
						listCmd.Accept(*this);
					});
				ImGui::PopID();
			}
		}
	}

	void EventListCommandVisitor::Visit(BeginEventCommand const& cmd)
	{
		Bool isMatchingTimeline = !(cpuTimeline ^ cmd.IsCPUTimeline());

		if (!flatten)
		{
			Bool isMatchingFilter = MatchesFilter(cmd.GetDesc());
			eventMatchStack.push_back(isMatchingFilter);

			if (isMatchingTimeline)
			{
				Bool renderNow = IsRenderingEnabled(); 

				ImGui::PushID((Int)cmd.GetCommandIndex());
				Bool isSelected = IsSelected(cmd);
				ImGui::Indent(indentLevel * 10.0f);

				Float r = ((cmd.eventColor >> 16) & 0xFF) / 255.0f;
				Float g = ((cmd.eventColor >> 8) & 0xFF) / 255.0f;
				Float b = (cmd.eventColor & 0xFF) / 255.0f;
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(r, g, b, 1.0f));

				Bool treeNodeOpened = renderNow ? ImGui::TreeNodeEx(cmd.eventName.c_str(), ImGuiTreeNodeFlags_DefaultOpen | (isSelected ? ImGuiTreeNodeFlags_Selected : 0)) : false;

				ImGui::PopStyleColor();

				if (ImGui::IsItemClicked(0))
				{
					FillSelectedCommandInfo(cmd);
				}

				openTreeNodes.push_back(treeNodeOpened && renderNow);

				ImGui::Unindent(indentLevel * 10.0f);
				ImGui::PopID();

				if (treeNodeOpened && renderNow)
				{
					indentLevel++;
				}
			}
			else
			{
				openTreeNodes.push_back(false);
			}
		}
		else
		{
			Bool isMatchingFilter = MatchesFilter(cmd.GetDesc());
			if (isMatchingTimeline && isMatchingFilter)
			{
				ImGui::PushID((Int)cmd.GetCommandIndex());
				Bool isSelected = IsSelected(cmd);

				Float r = ((cmd.eventColor >> 16) & 0xFF) / 255.0f;
				Float g = ((cmd.eventColor >> 8) & 0xFF) / 255.0f;
				Float b = (cmd.eventColor & 0xFF) / 255.0f;
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(r, g, b, 1.0f));
				ImGui::Selectable(cmd.GetDesc().c_str(), isSelected);
				ImGui::PopStyleColor();

				if (ImGui::IsItemClicked(0))
				{
					FillSelectedCommandInfo(cmd);
				}
				ImGui::PopID();
			}
		}
	}

	void EventListCommandVisitor::Visit(EndEventCommand const& cmd)
	{
		if (!flatten)
		{
			Bool isMatchingTimeline = !(cpuTimeline ^ cmd.IsCPUTimeline());
			Bool isMatchingFilter = MatchesFilter(cmd.GetDesc());

			if (isMatchingTimeline && isMatchingFilter && !eventMatchStack.empty())
			{
				eventMatchStack.back() = true;
			}

			if (!eventMatchStack.empty())
			{
				Bool scopeMatched = eventMatchStack.back();
				eventMatchStack.pop_back();
				if (scopeMatched && !eventMatchStack.empty())
				{
					eventMatchStack.back() = true;
				}
			}

			if (!openTreeNodes.empty())
			{
				Bool wasOpen = openTreeNodes.back();
				openTreeNodes.pop_back();

				if (wasOpen)
				{
					indentLevel--;
					ImGui::TreePop();
				}
			}
		}
	}

	void EventListCommandVisitor::FillSelectedCommandInfo(Command const& cmd)
	{
		selectedCommandInfo.commandHash = cmd.GetHash();
		selectedCommandInfo.commandType = (Uint32)cmd.GetType();
		selectedCommandInfo.commandIndex = cmd.GetCommandIndex();
	}

	Bool EventListCommandVisitor::IsSelected(Command const& cmd) const
	{
		if (cmd.GetHash() == oldSelectedCommandInfo.commandHash &&
			(Uint32)cmd.GetType() == oldSelectedCommandInfo.commandType)
		{
			Int64 absIndexDifference = std::abs((Int64)cmd.GetCommandIndex() - (Int64)oldSelectedCommandInfo.commandIndex);
			if (selectedCommand == nullptr && absIndexDifference < 2)
			{
				selectedCommand = &cmd;
				return true;
			}
		}
		return false;
	}

#define VISIT_IMPL(COMMAND_TYPE) \
    void EventListCommandVisitor::Visit(COMMAND_TYPE const& cmd) \
    { \
        VisitCommon(cmd); \
    }

	VISIT_IMPL(Command)
	VISIT_IMPL(CreateCommandQueueCommand)
	VISIT_IMPL(CreateCommandQueue1Command)
	VISIT_IMPL(CreateCommandListCommand)
	VISIT_IMPL(CreateCommandList1Command)
	VISIT_IMPL(CreateCommandAllocatorCommand)
	VISIT_IMPL(CreateFenceCommand)
	VISIT_IMPL(CreateHeapCommand)
	VISIT_IMPL(CreateHeap1Command)
	VISIT_IMPL(CreateRootSignatureCommand)
	VISIT_IMPL(CreateCommandSignatureCommand)
	VISIT_IMPL(CreateGraphicsPipelineStateCommand)
	VISIT_IMPL(CreateComputePipelineStateCommand)
	VISIT_IMPL(CreatePipelineStateCommand)
	VISIT_IMPL(CreateStateObjectCommand)
	VISIT_IMPL(AddToStateObjectCommand)
	VISIT_IMPL(CreateCommittedResourceCommand)
	VISIT_IMPL(CreateCommittedResource1Command)
	VISIT_IMPL(CreatePlacedResourceCommand)
	VISIT_IMPL(CreatePlacedResource1Command)
	VISIT_IMPL(CreateDescriptorHeapCommand)
	VISIT_IMPL(QueueSignalCommand)
	VISIT_IMPL(CopyBufferRegionCommand)
	VISIT_IMPL(CopyTextureRegionCommand)
	VISIT_IMPL(CopyResourceCommand)
	VISIT_IMPL(ResolveSubresourceCommand)
	VISIT_IMPL(ResolveSubresourceRegionCommand)
	VISIT_IMPL(DrawInstancedCommand)
	VISIT_IMPL(DrawIndexedInstancedCommand)
	VISIT_IMPL(DispatchCommand)
	VISIT_IMPL(DispatchMeshCommand)
	VISIT_IMPL(ExecuteIndirectCommand)
	VISIT_IMPL(DispatchRaysCommand)
	VISIT_IMPL(RSSetViewportsCommand)
	VISIT_IMPL(RSSetScissorRectsCommand)
	VISIT_IMPL(RSSetShadingRateCommand)
	VISIT_IMPL(RSSetShadingRateImageCommand)
	VISIT_IMPL(SetViewInstanceMaskCommand)
	VISIT_IMPL(OMSetRenderTargetsCommand)
	VISIT_IMPL(OMSetBlendFactorCommand)
	VISIT_IMPL(OMSetStencilRefCommand)
	VISIT_IMPL(OMSetDepthBoundsCommand)
	VISIT_IMPL(BeginRenderPassCommand)
	VISIT_IMPL(EndRenderPassCommand)
	VISIT_IMPL(ClearRenderTargetViewCommand)
	VISIT_IMPL(ClearDepthStencilViewCommand)
	VISIT_IMPL(ClearUnorderedAccessViewUintCommand)
	VISIT_IMPL(ClearUnorderedAccessViewFloatCommand)
	VISIT_IMPL(SetPipelineStateCommand)
	VISIT_IMPL(SetPipelineState1Command)
	VISIT_IMPL(SetGraphicsRootSignatureCommand)
	VISIT_IMPL(SetComputeRootSignatureCommand)
	VISIT_IMPL(IASetPrimitiveTopologyCommand)
	VISIT_IMPL(IASetVertexBuffersCommand)
	VISIT_IMPL(IASetIndexBufferCommand)
	VISIT_IMPL(ResourceBarrierCommand)
	VISIT_IMPL(SetDescriptorHeapsCommand)
	VISIT_IMPL(SetGraphicsRootDescriptorTableCommand)
	VISIT_IMPL(SetComputeRootDescriptorTableCommand)
	VISIT_IMPL(SetGraphicsRoot32BitConstantCommand)
	VISIT_IMPL(SetComputeRoot32BitConstantCommand)
	VISIT_IMPL(SetGraphicsRoot32BitConstantsCommand)
	VISIT_IMPL(SetComputeRoot32BitConstantsCommand)
	VISIT_IMPL(SetGraphicsRootConstantBufferViewCommand)
	VISIT_IMPL(SetComputeRootConstantBufferViewCommand)
	VISIT_IMPL(SetGraphicsRootShaderResourceViewCommand)
	VISIT_IMPL(SetComputeRootShaderResourceViewCommand)
	VISIT_IMPL(SetGraphicsRootUnorderedAccessViewCommand)
	VISIT_IMPL(SetComputeRootUnorderedAccessViewCommand)
	VISIT_IMPL(CloseListCommand)
	VISIT_IMPL(ResetListCommand)
	VISIT_IMPL(ClearStateCommand)
	VISIT_IMPL(ResourceSetNameCommand)
	VISIT_IMPL(GetCompletedValueCommand)
	VISIT_IMPL(SetEventOnCompletionCommand)
	VISIT_IMPL(FenceSignalCommand)
#undef VISIT_IMPL

}
