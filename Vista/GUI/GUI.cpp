﻿#include <format>
#include "GUI.h"
#include "GUIRenderUtil.h" 
#include "BufferFormatParser.h"
#include "Tracking/State.h"
#include "Tracking/ObjectTracker.h"
#include "Tracking/DescriptorTracker.h"
#include "Tracking/ResourceAddressTracker.h"
#include "Visitors/EventListCommandVisitor.h"
#include "Visitors/EventDetailsCommandVisitor.h"
#include "Visitors/StateUpdateVisitor.h"
#include "Command/Command.h"
#include "Command/CommandRecorder.h"
#include "Command/RecorderManager.h"
#include "Command/RTTI.h"
#include "Command/Commands/ListCommands.h"
#include "Resource/ResourceCopyRequestManager.h"
#include "Utilities/D3D12Util.h"
#include "Utilities/HashUtil.h"
#include "ImGui/imgui.h"

namespace vista
{
	
	Bool GUI::Initialize(ID3D12Device* device)
	{
		return imguiManager.Initialize(device);
	}

	Bool GUI::BeginRender()
	{
		return imguiManager.BeginFrame();
	}

	void GUI::Render(RecorderManager const& recorderManager)
	{
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
			ImGui::PopStyleColor();
		}

		if (ImGui::Begin("Event List"))
		{
			RenderEventList(recorderManager);
		}
		ImGui::End();

		UpdateCurrentState(recorderManager);

		if (ImGui::Begin("Misc"))
		{
			RenderTabs(recorderManager);
		}
		ImGui::End();

		if (ImGui::Begin("Resource Viewer"))
		{
			RenderResourceViewer();
		}
		ImGui::End();
	}

	void GUI::EndRender()
	{
		imguiManager.EndFrame();
	}

	void GUI::RenderEventList(RecorderManager const& recorderManager)
	{
		ImGui::Checkbox("Freeze Capture", &isFreezed);
		ImGui::BeginChild("EventList", ImVec2(0, 0), true);
		{
			static Int selectedTimelineIndex = 1;
			static std::vector<std::string> timelineOptions;
			static std::unordered_map<Uint32, std::vector<void*>> timelineIndexRecorderMap;
			timelineOptions.clear();
			timelineIndexRecorderMap.clear();
			selectedCommand = nullptr;

			timelineOptions.push_back("CPU Timeline");
			for (auto const& [key, recorder] : recorderManager.GetRecorders())
			{
				if (recorder.IsTopLevel())
				{
					timelineIndexRecorderMap[0].push_back(key);
				}

				if (HasFlag(recorder.GetCategory(), CommandCategory::Queue))
				{
					TrackedObjectInfo const* queueInfo = objectTracker.GetObjectInfo(key);
					if (!queueInfo || !std::holds_alternative<QueueDesc>(queueInfo->objectDesc)) continue;

					QueueDesc const& queueDesc = std::get<QueueDesc>(queueInfo->objectDesc);

					timelineIndexRecorderMap[(Uint32)timelineOptions.size()].push_back(key);
					if (queueDesc.queueDesc.Type == D3D12_COMMAND_LIST_TYPE_DIRECT)
					{
						timelineOptions.push_back(std::format("Graphics Queue obj#{}", queueInfo->objectId));
					}
					else if (queueDesc.queueDesc.Type == D3D12_COMMAND_LIST_TYPE_COMPUTE)
					{
						timelineOptions.push_back(std::format("Compute Queue obj#{}", queueInfo->objectId));
					}
					else if (queueDesc.queueDesc.Type == D3D12_COMMAND_LIST_TYPE_COPY)
					{
						timelineOptions.push_back(std::format("Copy Queue obj#{}", queueInfo->objectId));
					}
				}
			}

			if (selectedTimelineIndex >= timelineOptions.size())
			{
				selectedTimelineIndex = 0;
			}

			static Bool flatten = false;
			ImGui::Checkbox("Flatten", &flatten);

			static Char filterBuffer[256] = "";
			ImGui::InputText("Filter", filterBuffer, sizeof(filterBuffer));
			std::string filterText = filterBuffer;

			if (ImGui::BeginCombo("##TimelineSelect", timelineOptions[selectedTimelineIndex].c_str()))
			{
				for (Uint32 i = 0; i < timelineOptions.size(); ++i)
				{
					Bool isSelected = (selectedTimelineIndex == i);
					if (ImGui::Selectable(timelineOptions[i].c_str(), isSelected))
					{
						if (selectedTimelineIndex != i)
						{
							selectedTimelineIndex = i;
							selectedCommandInfo.Reset();
							selectedCommand = nullptr;
						}
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			ImGui::Separator();

			ImGui::BeginChild("EventListContent", ImVec2(0, 0), true);
			{
				Bool cpuTimeline = (selectedTimelineIndex == 0);
				EventListCommandVisitor treeVisitor(cpuTimeline, flatten, filterText, selectedCommand, selectedCommandInfo);
				for (void* recorderKey : timelineIndexRecorderMap[selectedTimelineIndex])
				{
					ImGui::PushID(recorderKey);
					recorderManager.GetRecorder(recorderKey)->Accept(treeVisitor);
					ImGui::PopID();
				}
			}
			ImGui::EndChild();
		}
		ImGui::EndChild();
	}

	void GUI::UpdateCurrentState(RecorderManager const& recorderManager)
	{
		if (selectedCommand && isa<ListCommand>(selectedCommand))
		{
			TrackedObjectInfo const* parentObjectInfo = objectTracker.GetObjectInfo(selectedCommand->GetParentId());
			VISTA_ASSERT(parentObjectInfo->objectType == ObjectType::List);
			ListDesc const& listDesc = std::get<ListDesc>(parentObjectInfo->objectDesc);
			CommandRecorder const* recorder = recorderManager.GetRecorder(parentObjectInfo->objectPtr);
			currentState.pipelineStateId = listDesc.initialStateId;
			StateUpdateVisitor visitor(currentState, objectTracker);
			recorder->ForAllCommandsUntil([&](Command const& cmd)
				{
					cmd.Accept(visitor);
					return &cmd == selectedCommand;
				});
		}
	}

	void GUI::RenderResourceViewer()
	{
		static SelectedItem selectedItemInViewer;
		static Float splitterWidth = 300.0f;
		static Float imageWidth = 200.0f;
		static Float infoWidth = 300.0f;
		static Float zoomFactor = 1.0f;
		static ImVec2 imageScroll = ImVec2(0, 0);
		static Bool isImageHovered = false;
		static ResourceCopyRequest lastValidCopyRequest;
		static constexpr Float MinPanelWidth = 100.0f;
		static constexpr Float SplitterThickness = 4.0f;

		ImVec2 windowSize = ImGui::GetContentRegionAvail();
		ImGui::BeginChild("Resource Viewer", ImVec2(0, 0), true);
		{
			windowSize = ImGui::GetContentRegionAvail();
			windowSize.x = std::max<Float>(windowSize.x, 2.0f * MinPanelWidth + SplitterThickness);
			splitterWidth = std::clamp(splitterWidth, MinPanelWidth, windowSize.x - MinPanelWidth - SplitterThickness);

			ImGui::BeginChild("##ResourceViewer_LeftPanel", ImVec2(splitterWidth, 0), true);
			{
				ImGui::TextUnformatted("Bound Resources");
				ImGui::Separator();

				Bool const isListCommand = selectedCommand && isa<ListCommand>(selectedCommand);
				if (isListCommand)
				{
					ListCommand const* listCommand = cast<ListCommand>(selectedCommand);
					if (listCommand->IsDrawCommand())
					{
						RenderDrawCommandBoundResources(selectedItemInViewer);
					}
					else if (listCommand->IsDispatchCommand())
					{
						RenderDispatchCommandBoundResources(selectedItemInViewer);
					}
					else if (listCommand->IsCopyCommand())
					{
						RenderCopyCommandBoundResources(selectedItemInViewer, listCommand);
					}
					else if (listCommand->IsMeshCommand())
					{
						RenderDispatchMeshCommandBoundResources(selectedItemInViewer);
					}
					else if (listCommand->IsRayTracingCommand())
					{
						RenderDispatchRaysCommandBoundResources(selectedItemInViewer);
					}
					else
					{
						selectedItemInViewer.Reset();
						ImGui::Text("Select a command to view bound resources.");
					}
				}

				if (ImGui::Selectable("##Deselect", false, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(0, 0)))
				{
					selectedItemInViewer.Reset();
				}
			}
			ImGui::EndChild();

			ImGui::SameLine();
			ImGui::InvisibleButton("##Splitter", ImVec2(SplitterThickness, -1));
			if (ImGui::IsItemActive())
			{
				Float delta = ImGui::GetIO().MouseDelta.x;
				splitterWidth += delta;
				splitterWidth = std::clamp(splitterWidth, MinPanelWidth, windowSize.x - MinPanelWidth - SplitterThickness);
			}
			if (ImGui::IsItemHovered())
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
			}
			ImVec2 splitterMin = ImGui::GetItemRectMin();
			ImVec2 splitterMax = ImGui::GetItemRectMax();
			ImGui::GetWindowDrawList()->AddRectFilled(splitterMin, splitterMax, IM_COL32(60, 60, 60, 255));

			ImGui::SameLine();

			ImGui::BeginChild("##ResourceViewer_RightPanel", ImVec2(0, 0), true);
			{
				if (selectedItemInViewer.type != SelectedItem::Type::None)
				{
					if (selectedItemInViewer.type == SelectedItem::Type::Resource && selectedItemInViewer.resource)
					{
						TrackedObjectInfo const* resourceInfo = objectTracker.GetObjectInfo(selectedItemInViewer.resource);
						if (resourceInfo && std::holds_alternative<ResourceDesc>(resourceInfo->objectDesc))
						{
							ResourceDesc const& desc = std::get<ResourceDesc>(resourceInfo->objectDesc);
							Float totalWidth = ImGui::GetContentRegionAvail().x;
							imageWidth = std::clamp(imageWidth, 50.0f, totalWidth - SplitterThickness - MinPanelWidth);
							infoWidth = totalWidth - imageWidth - SplitterThickness;

							ImGui::BeginChild("##ImagePreview", ImVec2(imageWidth, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
							{
								if (ImGui::IsWindowHovered())
								{
									if (ImGui::GetIO().MouseWheel != 0.0f)
									{
										Float zoomDelta = ImGui::GetIO().MouseWheel * 0.1f;
										zoomFactor += zoomDelta;
										zoomFactor = std::clamp(zoomFactor, 0.1f, 5.0f);
									}
									if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
									{
										ImGui::SetScrollX(ImGui::GetScrollX() - ImGui::GetIO().MouseDelta.x);
										ImGui::SetScrollY(ImGui::GetScrollY() - ImGui::GetIO().MouseDelta.y);
									}
								}

								if (!IsFreezed())
								{
									copyRequestManager.RequestCopy(selectedCommandInfo, selectedItemInViewer);
								}

								std::optional<ResourceCopyRequest> copyRequest = copyRequestManager.TryGetReadyRequest();
								if (copyRequest.has_value())
								{
									copyRequestManager.FreeRequest(lastValidCopyRequest);
									lastValidCopyRequest = *copyRequest;
								}

								ID3D12Resource* resource = lastValidCopyRequest.GetDestinationResource();
								Bool const isTexture2D = resource && resource->GetDesc().Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D && resource->GetDesc().DepthOrArraySize == 1;
								Bool const isBuffer = resource && resource->GetDesc().Dimension == D3D12_RESOURCE_DIMENSION_BUFFER;

								if (isTexture2D)
								{
									RenderTexture2DPreview(resource, zoomFactor);
								}
								else if (isBuffer)
								{
									RenderBufferPreview(resource);
								}
								else
								{
									ImGui::Text("No resource preview available");
								}
							}
							ImGui::EndChild();

							ImGui::SameLine();
							ImGui::InvisibleButton("##ImageInfoSplitter", ImVec2(SplitterThickness, -1));
							if (ImGui::IsItemActive())
							{
								Float delta = ImGui::GetIO().MouseDelta.x;
								imageWidth += delta;
								imageWidth = std::clamp(imageWidth, 50.0f, totalWidth - SplitterThickness - MinPanelWidth);
								infoWidth = totalWidth - imageWidth - SplitterThickness;
							}
							if (ImGui::IsItemHovered())
							{
								ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
							}
							ImVec2 splitterMin = ImGui::GetItemRectMin();
							ImVec2 splitterMax = ImGui::GetItemRectMax();
							ImGui::GetWindowDrawList()->AddRectFilled(splitterMin, splitterMax, IM_COL32(60, 60, 60, 255));

							ImGui::SameLine();

							ImGui::BeginChild("##ResourceInfo", ImVec2(infoWidth, 0), true);
							{
								if (ImGui::TreeNode("Resource Properties"))
								{
									ImGui::Indent();
									RenderResourceDesc(desc);
									ImGui::Unindent();
									ImGui::TreePop();
								}

								if (ImGui::TreeNode("View Properties"))
								{
									ImGui::Indent();
									RenderDescriptorInfo(selectedItemInViewer.descriptorInfo);
									ImGui::Unindent();
									ImGui::TreePop();
								}

								if (ImGui::TreeNode("Object Info"))
								{
									ImGui::Indent();
									RenderObjectInfoByID(resourceInfo->objectId, objectTracker);
									ImGui::Unindent();
									ImGui::TreePop();
								}
							}
							ImGui::EndChild();
						}
						else
						{
							ImGui::Text("Selected Resource: Unknown %p", selectedItemInViewer.resource);
						}
					}
					else if (selectedItemInViewer.type == SelectedItem::Type::Sampler)
					{
						ImGui::Text("Selected Sampler: Heap#%llu Index %d", selectedItemInViewer.heapId, selectedItemInViewer.descriptorIndex);
						RenderDescriptorInfo(selectedItemInViewer.descriptorInfo);
					}
				}
				else
				{
					ImGui::Text("Select a resource or sampler from the left panel to view its details.");
				}
			}
			ImGui::EndChild();
		}
		ImGui::EndChild();
	}

	void GUI::RenderTabs(RecorderManager const& recorderManager)
	{
		if (ImGui::BeginTabBar("Tabs"))
		{
			enum Tab
			{
				Tab_EventDetails,
				Tab_State,
				Tab_Objects,
				Tab_Resources,
				Tab_DescriptorHeaps,
				Tab_Count
			};
			static Tab selectedTab = Tab_EventDetails;

			static Char const* TabNames[] =
			{
				"Event Details",
				"State",
				"Objects",
				"Resources",
				"Descriptor Heaps"
			};

			for (Uint32 i = Tab_EventDetails; i < Tab_Count; ++i)
			{
				if (i != Tab_EventDetails) ImGui::SameLine();
				if (ImGui::GetContentRegionAvail().x < 100.0f) ImGui::NewLine();
				if (ImGui::Selectable(TabNames[i], selectedTab == i, 0, ImVec2(150, 0))) selectedTab = (Tab)i;
			}
			ImGui::Separator();

			switch (selectedTab)
			{
			case Tab_EventDetails:
				if (selectedCommand)
				{
					EventDetailsCommandVisitor detailsVisitor(objectTracker, descriptorTracker, addressTracker);
					selectedCommand->Accept(detailsVisitor);
				}
				else
				{
					ImGui::TextWrapped("Select a call from the tree to see details.");
				}
				break;
			case Tab_State:
				RenderStateTab(recorderManager);
				break;
			case Tab_Objects:
				RenderObjectsTab();
				break;
			case Tab_Resources:
				RenderResourceTab();
				break;
			case Tab_DescriptorHeaps:
				RenderDescriptorHeapTab();
				break;
			}
			ImGui::EndTabBar();
		}
	}

	void GUI::RenderObjectsTab()
	{
		std::vector<TrackedObjectInfo*> const& objectList = objectTracker.GetObjectList();

		static Int selectedObjectFilterIndex = 0;
		static std::vector<Char const*> objectFilterOptions;

		objectFilterOptions.clear();
		objectFilterOptions.push_back("All Objects");

		static std::array objectFilterTypes =
		{
			ObjectType::Device, ObjectType::Queue, ObjectType::List, ObjectType::Allocator,
			ObjectType::Fence, ObjectType::PSO, ObjectType::RootSignature, ObjectType::CommandSignature,
			ObjectType::Resource, ObjectType::DescriptorHeap, ObjectType::Heap, ObjectType::StateObject
		};
		static_assert(std::size(objectFilterTypes) == (Uint64)ObjectType::Count - 1);

		for (Uint32 i = 1; i < std::size(ObjectTypeNames); ++i)
		{
			objectFilterOptions.push_back(ObjectTypeNames[i]);
		}

		if (selectedObjectFilterIndex >= (Int)objectFilterOptions.size())
		{
			selectedObjectFilterIndex = 0;
		}

		ImGui::PushItemWidth(200.0f);
		if (ImGui::BeginCombo("##ObjectFilter", objectFilterOptions[selectedObjectFilterIndex]))
		{
			for (Uint32 i = 0; i < (Uint32)objectFilterOptions.size(); ++i)
			{
				Bool const isSelected = (selectedObjectFilterIndex == i);
				if (ImGui::Selectable(objectFilterOptions[i], isSelected))
				{
					selectedObjectFilterIndex = i;
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
		ImGui::Separator();

		if (ImGui::BeginTable("ObjectTable", 2,
			ImGuiTableFlags_RowBg |
			ImGuiTableFlags_BordersInnerV |
			ImGuiTableFlags_Resizable |
			ImGuiTableFlags_ScrollY |
			ImGuiTableFlags_SizingStretchSame))
		{
			ImGui::TableSetupColumn("Object", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableHeadersRow();

			for (TrackedObjectInfo const* info : objectList)
			{
				Bool showObject = false;
				if (selectedObjectFilterIndex == 0)
				{
					showObject = true;
				}
				else
				{
					Int typeMapIndex = selectedObjectFilterIndex - 1;
					if (typeMapIndex >= 0 && typeMapIndex < (Int)objectFilterTypes.size())
					{
						if (info->objectType == objectFilterTypes[typeMapIndex])
						{
							showObject = true;
						}
					}
				}

				if (!showObject)
				{
					continue;
				}

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAllColumns | ImGuiTreeNodeFlags_FramePadding;
				Bool opened = ImGui::TreeNodeEx((void*)info, flags, "obj#%llu", info->objectId);

				ImGui::TableSetColumnIndex(1);
				ImGui::TextUnformatted(ObjectTypeNames[(Uint64)info->objectType]);

				if (opened)
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::TableSetColumnIndex(1);
					ImGui::Dummy(ImVec2(0, 0));
					ImGui::TableSetColumnIndex(0);
					ImGui::Indent();
					ImGui::SeparatorText("Details");

					switch (info->objectType)
					{
					case ObjectType::Device:
						ImGui::TextUnformatted("Device (no additional properties)");
						break;
					case ObjectType::Queue:
						if (std::holds_alternative<QueueDesc>(info->objectDesc)) RenderQueueDesc(std::get<QueueDesc>(info->objectDesc));
						break;
					case ObjectType::List:
						if (std::holds_alternative<ListDesc>(info->objectDesc)) RenderListDesc(std::get<ListDesc>(info->objectDesc));
						break;
					case ObjectType::Allocator:
						if (std::holds_alternative<AllocatorDesc>(info->objectDesc)) RenderAllocatorDesc(std::get<AllocatorDesc>(info->objectDesc));
						break;
					case ObjectType::Fence:
						if (std::holds_alternative<FenceDesc>(info->objectDesc)) RenderFenceDesc(std::get<FenceDesc>(info->objectDesc));
						break;
					case ObjectType::Resource:
						if (std::holds_alternative<ResourceDesc>(info->objectDesc)) RenderResourceDesc(std::get<ResourceDesc>(info->objectDesc));
						break;
					case ObjectType::DescriptorHeap:
						if (std::holds_alternative<DescriptorHeapDesc>(info->objectDesc)) RenderDescriptorHeapDesc(std::get<DescriptorHeapDesc>(info->objectDesc));
						break;
					case ObjectType::RootSignature:
						if (std::holds_alternative<RootSignatureDesc>(info->objectDesc)) RenderRootSignatureDesc(std::get<RootSignatureDesc>(info->objectDesc));
						break;
					case ObjectType::PSO:
						if (std::holds_alternative<PSODesc>(info->objectDesc))
						{
							PSODesc const& psoDesc = std::get<PSODesc>(info->objectDesc);
							if (std::holds_alternative<GraphicsPSODescStorage>(psoDesc))
							{
								RenderGraphicsPSODetails(std::get<GraphicsPSODescStorage>(psoDesc), objectTracker);
							}
							else if (std::holds_alternative<ComputePSODescStorage>(psoDesc))
							{
								RenderComputePSODetails(std::get<ComputePSODescStorage>(psoDesc), objectTracker);
							}
						}
						break;
					case ObjectType::CommandSignature:
						if (std::holds_alternative<CommandSignatureDesc>(info->objectDesc)) RenderCommandSignatureDesc(std::get<CommandSignatureDesc>(info->objectDesc), objectTracker);
						break;
					case ObjectType::Heap:
						if (std::holds_alternative<HeapDesc>(info->objectDesc)) RenderHeapDesc(std::get<HeapDesc>(info->objectDesc));
						break;
					case ObjectType::StateObject:
						if (std::holds_alternative<StateObjectDesc>(info->objectDesc)) RenderStateObjectDesc(std::get<StateObjectDesc>(info->objectDesc), objectTracker);
						break;
					default:
						ImGui::TextUnformatted("Unknown object type or details not implemented.");
						break;
					}

					ImGui::Unindent();
					ImGui::TreePop();
				}
			}
			ImGui::EndTable();
		}
	}

	void GUI::RenderDescriptorHeapTab()
	{
		std::vector<TrackedObjectInfo*> const& objectList = objectTracker.GetObjectList();
		static ObjectID selectedHeapId = InvalidObjectID;
		std::map<ObjectID, std::string> heapDisplayMap;
		std::vector<ObjectID> heapIds;

		for (TrackedObjectInfo const* info : objectList)
		{
			if (info->objectType == ObjectType::DescriptorHeap)
			{
				if (std::holds_alternative<DescriptorHeapDesc>(info->objectDesc))
				{
					DescriptorHeapDesc const& desc = std::get<DescriptorHeapDesc>(info->objectDesc);
					heapIds.push_back(info->objectId);
					heapDisplayMap[info->objectId] = std::format("obj#{} ({}, Count={})",
						info->objectId,
						D3D12DescriptorHeapTypeToString(desc.heapDesc.Type),
						desc.heapDesc.NumDescriptors);
				}
			}
		}
		std::sort(heapIds.begin(), heapIds.end());

		Int currentComboIndex = -1;
		Char const* comboPreview = "<Select Heap>";
		if (selectedHeapId != InvalidObjectID)
		{
			for (Uint32 i = 0; i < heapIds.size(); ++i)
			{
				if (heapIds[i] == selectedHeapId)
				{
					currentComboIndex = static_cast<Int>(i);
					auto mapIt = heapDisplayMap.find(heapIds[i]);
					if (mapIt != heapDisplayMap.end())
					{
						comboPreview = mapIt->second.c_str();
					}
					break;
				}
			}
		}

		ImGui::PushItemWidth(300.0f);
		if (ImGui::BeginCombo("##HeapSelect", comboPreview))
		{
			for (Uint32 i = 0; i < heapIds.size(); ++i)
			{
				Bool isSelected = (currentComboIndex == static_cast<Int>(i));
				auto mapIt = heapDisplayMap.find(heapIds[i]);
				if (mapIt != heapDisplayMap.end())
				{
					if (ImGui::Selectable(mapIt->second.c_str(), isSelected))
					{
						selectedHeapId = heapIds[i];
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
		ImGui::Separator();

		if (selectedHeapId != InvalidObjectID)
		{
			TrackedObjectInfo const* heapInfo = objectTracker.GetObjectInfo(selectedHeapId);
			if (heapInfo && std::holds_alternative<DescriptorHeapDesc>(heapInfo->objectDesc))
			{
				TrackedDescriptorHeapInfo const* descHeapInfo = descriptorTracker.GetHeapInfo(selectedHeapId);
				if (descHeapInfo)
				{
					if (ImGui::BeginTable("DescriptorTable", 2,
						ImGuiTableFlags_RowBg |
						ImGuiTableFlags_BordersInnerV |
						ImGuiTableFlags_Resizable |
						ImGuiTableFlags_ScrollY |
						ImGuiTableFlags_SizingStretchSame))
					{
						ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthFixed, 80.0f);
						ImGui::TableSetupColumn("Details", ImGuiTableColumnFlags_WidthStretch);
						ImGui::TableHeadersRow();

						for (Int i = 0; i < static_cast<Int>(descHeapInfo->descriptors.size()); ++i)
						{
							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);
							ImGui::Text("[%d]", i);

							ImGui::TableSetColumnIndex(1);
							DescriptorInfo const& descInfo = descHeapInfo->descriptors[i];
							RenderDescriptorInfo(descInfo, i); 
						}
						ImGui::EndTable();
					}
				}
				else
				{
					ImGui::Text("Descriptor heap information not found.");
				}
			}
			else
			{
				ImGui::Text("Selected heap is invalid or no longer exists.");
				selectedHeapId = InvalidObjectID;
			}
		}
		else
		{
			ImGui::Text("Select a descriptor heap to view its contents.");
		}
	}

	void GUI::RenderResourceTab()
	{
		std::vector<TrackedObjectInfo*> const& objectList = objectTracker.GetObjectList();

		if (ImGui::BeginTable("ResourceTable", 3,
			ImGuiTableFlags_RowBg |
			ImGuiTableFlags_BordersInnerV |
			ImGuiTableFlags_Resizable |
			ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX |
			ImGuiTableFlags_SizingFixedFit))
		{
			ImGui::TableSetupColumn("Resource", ImGuiTableColumnFlags_WidthFixed); 
			ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Summary", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableHeadersRow();

			for (TrackedObjectInfo const* info : objectList)
			{
				if (info->objectType != ObjectType::Resource) continue;

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAllColumns | ImGuiTreeNodeFlags_FramePadding;
				Bool opened = ImGui::TreeNodeEx((void*)info, flags, "obj#%llu (%s)", info->objectId, info->objectDebugName.c_str());

				ImGui::TableSetColumnIndex(1);
				ImGui::TextUnformatted("Resource");

				ImGui::TableSetColumnIndex(2);
				if (std::holds_alternative<ResourceDesc>(info->objectDesc))
				{
					ResourceDesc const& desc = std::get<ResourceDesc>(info->objectDesc);
					ImGui::Text("Dimension: %s, Format: %s, Size: %llu",
						D3D12ResourceDimensionToString(desc.resourceDesc.Dimension),
						DXGIFormatToString(desc.resourceDesc.Format),
						desc.resourceDesc.Width); 
				}

				if (opened)
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Indent();
					ImGui::SeparatorText("Details");

					if (std::holds_alternative<ResourceDesc>(info->objectDesc))
					{
						RenderResourceDesc(std::get<ResourceDesc>(info->objectDesc)); 
					}
					ImGui::Unindent();
					ImGui::TreePop();
				}
			}
			ImGui::EndTable();
		}
	}

	void GUI::RenderStateTab(RecorderManager const& recorderManager)
	{
		if (!selectedCommand || !isa<ListCommand>(selectedCommand))
		{
			ImGui::TextWrapped("Select a command from the event list to view pipeline state.");
			return;
		}

		ImGui::Text("Pipeline State ID: obj#%llu", currentState.pipelineStateId);
		if (currentState.pipelineStateId != InvalidObjectID)
		{
			TrackedObjectInfo const* psoInfo = objectTracker.GetObjectInfo(currentState.pipelineStateId);
			if (psoInfo && std::holds_alternative<PSODesc>(psoInfo->objectDesc))
			{
				PSODesc const& psoDesc = std::get<PSODesc>(psoInfo->objectDesc);
				if (std::holds_alternative<GraphicsPSODescStorage>(psoDesc))
				{
					auto const& desc = std::get<GraphicsPSODescStorage>(psoDesc);

					if (ImGui::TreeNode("PSO Details"))
					{
						RenderGraphicsPSODetails(desc, objectTracker);
						ImGui::TreePop();
					}

					RenderRootSignatureDetails(
						desc.RootSignatureId,
						objectTracker,
						descriptorTracker,
						addressTracker,
						currentState.graphicsRootArguments
					);
				}
				else if (std::holds_alternative<ComputePSODescStorage>(psoDesc))
				{
					auto const& desc = std::get<ComputePSODescStorage>(psoDesc);

					if (ImGui::TreeNode("PSO Details"))
					{
						RenderComputePSODetails(desc, objectTracker);
						ImGui::TreePop();
					}

					RenderRootSignatureDetails(
						desc.RootSignatureId,
						objectTracker,
						descriptorTracker,
						addressTracker,
						currentState.computeRootArguments
					);
				}
				else if (std::holds_alternative<StreamPSODescStorage>(psoDesc))
				{
					auto const& desc = std::get<StreamPSODescStorage>(psoDesc);
					if (ImGui::TreeNode("Stream PSO Details"))
					{
						RenderStreamPSODetails(desc, objectTracker);
						ImGui::TreePop();
					}

					auto HasShader = [&desc](D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type) -> Bool 
						{
						switch (type)
						{
						case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS:
							return desc.VS.IsValid();
						case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CS:
							return desc.CS.IsValid();
						case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_MS:
							return desc.MS.IsValid();
						default:
							return false;
						}
						};

					Bool const isGraphics = HasShader(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS);
					Bool const isCompute = HasShader(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CS);
					Bool const isMesh = HasShader(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_MS);

					ObjectID rootSigId = desc.RootSignatureId;
					if (isGraphics)
					{
						RenderRootSignatureDetails(
							rootSigId,
							objectTracker,
							descriptorTracker,
							addressTracker,
							currentState.graphicsRootArguments
						);
					}
					else if (isCompute)
					{
						RenderRootSignatureDetails(
							rootSigId,
							objectTracker,
							descriptorTracker,
							addressTracker,
							currentState.computeRootArguments
						);
					}
					else if (isMesh)
					{
						RenderRootSignatureDetails(
							rootSigId,
							objectTracker,
							descriptorTracker,
							addressTracker,
							currentState.graphicsRootArguments
						);
					}
					else
					{
						ImGui::Text("<Invalid stream - no entry-point shader found>");
					}
				}
			}
		}

		ImGui::PushID("##State_DescriptorHeaps");
		if (ImGui::TreeNode("Bound Descriptor Heaps"))
		{
			RenderObjectInfoByID(currentState.descriptorHeapIds[0], objectTracker, "CBV/SRV/UAV Heap:");
			RenderObjectInfoByID(currentState.descriptorHeapIds[1], objectTracker, "Sampler Heap:");
			ImGui::TreePop();
		}
		ImGui::PopID();

		if (ImGui::TreeNode("Viewports"))
		{
			if (currentState.viewports.empty())
			{
				ImGui::Text("<None set>");
			}
			else
			{
				for (Uint32 i = 0; i < currentState.viewports.size(); ++i)
				{
					RenderViewport(i, currentState.viewports[i]); 
				}
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Scissor Rects"))
		{
			if (currentState.scissorRects.empty())
			{
				ImGui::Text("<None set>");
			}
			else
			{
				for (Uint32 i = 0; i < currentState.scissorRects.size(); ++i)
				{
					RenderScissorRect(i, currentState.scissorRects[i]); 
				}
			}
			ImGui::TreePop();
		}

		if (currentState.shadingRate.has_value())
		{
			ImGui::Text("Shading Rate: %s", D3D12ShadingRateToString(currentState.shadingRate.value()));
			ImGui::Text("Shading Rate Combiner 0: %s", D3D12ShadingRateCombinerToString(currentState.shadingRateCombiners[0]));
			ImGui::Text("Shading Rate Combiner 1: %s", D3D12ShadingRateCombinerToString(currentState.shadingRateCombiners[1]));
		}
		else
		{
			ImGui::Text("Shading Rate: <not set>");
		}

		ImGui::Text("Primitive Topology: %s", D3D12PrimitiveTopologyToString(currentState.primitiveTopology));

		if (ImGui::TreeNode("Render Targets"))
		{
			ImGui::Text("Count: %u", currentState.numRenderTargetsSet);
			for (Uint32 i = 0; i < currentState.numRenderTargetsSet; ++i)
			{
				ImGui::PushID(i);
				RenderDescriptorInfoByHandle(currentState.renderTargetViews[i], std::format("RTV {}", i).c_str(), descriptorTracker); 
				ImGui::PopID();
			}
			if (currentState.numRenderTargetsSet == 0)
			{
				ImGui::Text("<None set>");
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Depth/Stencil Info"))
		{
			if (currentState.depthStencilView)
			{
				RenderDescriptorInfoByHandle(*currentState.depthStencilView, "Bound DSV", descriptorTracker); 
			}
			else
			{
				ImGui::Text("Bound DSV: <None set>");
			}

			if (ImGui::TreeNode("Command List State"))
			{
				if (currentState.stencilRef.has_value())
				{
					ImGui::Text("Stencil Ref: 0x%X", currentState.stencilRef.value());
				}
				else
				{
					ImGui::Text("Stencil Ref: <Not set>");
				}

				if (ImGui::TreeNode("Depth Bounds"))
				{
					ImGui::Text("Depth Min: %f", currentState.depthMin);
					ImGui::Text("Depth Max: %f", currentState.depthMax);
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		ImGui::Separator();
	}

	void GUI::RenderDrawCommandBoundResources(SelectedItem& selectedItemInViewer)
	{
		if (currentState.pipelineStateId != InvalidObjectID)
		{
			TrackedObjectInfo const* psoInfo = objectTracker.GetObjectInfo(currentState.pipelineStateId);
			if (psoInfo && std::holds_alternative<PSODesc>(psoInfo->objectDesc))
			{
				PSODesc const& psoDesc = std::get<PSODesc>(psoInfo->objectDesc);
				if (std::holds_alternative<GraphicsPSODescStorage>(psoDesc))
				{
					GraphicsPSODescStorage const& desc = std::get<GraphicsPSODescStorage>(psoDesc);
					if (ImGui::TreeNode("Graphics Pipeline State"))
					{
						RenderGraphicsPSODetails(desc, objectTracker);
						ImGui::TreePop();
					}
				}
				else if (std::holds_alternative<StreamPSODescStorage>(psoDesc))
				{
					StreamPSODescStorage const& desc = std::get<StreamPSODescStorage>(psoDesc);
					if (ImGui::TreeNode("Graphics Pipeline State"))
					{
						RenderStreamPSODetails(desc, objectTracker);
						ImGui::TreePop();
					}
				}
			}
		}

		if (ImGui::TreeNode("IA"))
		{
			if (ImGui::TreeNode("Vertex Buffers"))
			{
				if (currentState.vertexBufferViews.empty())
				{
					ImGui::Text("<None set>");
				}
				else
				{
					for (Uint32 i = 0; i < currentState.vertexBufferViews.size(); ++i)
					{
						Uint32 slot = currentState.vertexBufferStartSlot + i;
						auto const& vbv = currentState.vertexBufferViews[i];
						std::string vbLabel = GetResourceLabel(std::format("VB {}", slot).c_str(), vbv.BufferLocation, objectTracker, addressTracker);
						ID3D12Resource* resource = addressTracker.QueryResource(vbv.BufferLocation);
						TrackedObjectInfo const* resourceInfo = resource ? objectTracker.GetObjectInfo(resource) : nullptr;
						DescriptorInfo descInfo{ DescriptorViewType::Unknown, resourceInfo ? resourceInfo->objectId : InvalidObjectID };
						RenderSelectableResource(vbLabel, resource, descInfo, selectedItemInViewer, resource);
					}
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Index Buffer"))
			{
				if (!currentState.indexBufferView.has_value())
				{
					ImGui::Text("<None set>");
				}
				else
				{
					D3D12_INDEX_BUFFER_VIEW const& ibv = currentState.indexBufferView.value();
					std::string ibLabel = GetResourceLabel("IB", ibv.BufferLocation, objectTracker, addressTracker);
					ID3D12Resource* resource = addressTracker.QueryResource(ibv.BufferLocation);
					TrackedObjectInfo const* resourceInfo = resource ? objectTracker.GetObjectInfo(resource) : nullptr;
					DescriptorInfo descInfo{ DescriptorViewType::Unknown, resourceInfo ? resourceInfo->objectId : InvalidObjectID };
					RenderSelectableResource(ibLabel, resource, descInfo, selectedItemInViewer, resource);

					ImGui::Text("Format: %s", DXGIFormatToString(ibv.Format));
					ImGui::Text("Size: %u bytes", ibv.SizeInBytes);
				}
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("VS"))
		{
			for (Uint32 i = 0; i < currentState.graphicsRootArguments.size(); ++i)
			{
				RenderRootParameterBinding(i, currentState.graphicsRootArguments[i], D3D12_SHADER_VISIBILITY_VERTEX, objectTracker, descriptorTracker, addressTracker, &selectedItemInViewer);
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("PS"))
		{
			for (Uint32 i = 0; i < currentState.graphicsRootArguments.size(); ++i)
			{
				RenderRootParameterBinding(i, currentState.graphicsRootArguments[i], D3D12_SHADER_VISIBILITY_PIXEL, objectTracker, descriptorTracker, addressTracker, &selectedItemInViewer);
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("OM"))
		{
			if (ImGui::TreeNode("Render Targets"))
			{
				if (currentState.numRenderTargetsSet == 0)
				{
					ImGui::Text("<None set>");
				}
				else
				{
					for (Uint32 i = 0; i < currentState.numRenderTargetsSet; ++i)
					{
						D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = currentState.renderTargetViews[i];
						if (rtvHandle.ptr == NULL) continue;

						auto const& [descHeapInfo, index] = descriptorTracker.FindDescriptorByCpuHandle(rtvHandle);
						if (index < 0 || !descHeapInfo) continue;

						DescriptorInfo const& descInfo = descHeapInfo->descriptors[index];
						if (std::holds_alternative<D3D12_RENDER_TARGET_VIEW_DESC>(descInfo.desc))
						{
							std::string rtvLabel = std::format("RTV {} :", i);
							TrackedObjectInfo const* resourceInfo = objectTracker.GetObjectInfo(descInfo.resourceId);
							ID3D12Resource* resource = resourceInfo ? reinterpret_cast<ID3D12Resource*>(resourceInfo->objectPtr) : nullptr;
							if (resourceInfo)
							{
								rtvLabel += std::format(" obj#{} ({})", resourceInfo->objectId, resourceInfo->objectDebugName);
							}
							else
							{
								rtvLabel += std::format(" obj#{} (<Unknown>)", descInfo.resourceId);
							}
							RenderSelectableResource(rtvLabel, resource, descInfo, selectedItemInViewer, resource);
						}
					}
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Depth Stencil"))
			{
				if (!currentState.depthStencilView)
				{
					ImGui::Text("<None set>");
				}
				else
				{
					D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = *currentState.depthStencilView;
					auto const& [descHeapInfo, index] = descriptorTracker.FindDescriptorByCpuHandle(dsvHandle);
					if (index >= 0 && descHeapInfo)
					{
						DescriptorInfo const& descInfo = descHeapInfo->descriptors[index];
						if (std::holds_alternative<D3D12_DEPTH_STENCIL_VIEW_DESC>(descInfo.desc))
						{
							std::string dsvLabel = "DSV :";
							TrackedObjectInfo const* resourceInfo = objectTracker.GetObjectInfo(descInfo.resourceId);
							ID3D12Resource* resource = resourceInfo ? reinterpret_cast<ID3D12Resource*>(resourceInfo->objectPtr) : nullptr;
							if (resourceInfo)
							{
								dsvLabel += std::format(" obj#{} ({})", resourceInfo->objectId, resourceInfo->objectDebugName);
							}
							else
							{
								dsvLabel += std::format(" obj#{} (<Unknown>)", descInfo.resourceId);
							}
							RenderSelectableResource(dsvLabel, resource, descInfo, selectedItemInViewer, resource);
						}
					}
					else
					{
						ImGui::Text("DSV: <Invalid or Missing Descriptor>");
					}
				}
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
	}

	void GUI::RenderDispatchCommandBoundResources(SelectedItem& selectedItemInViewer)
	{
		if (currentState.pipelineStateId != InvalidObjectID)
		{
			TrackedObjectInfo const* psoInfo = objectTracker.GetObjectInfo(currentState.pipelineStateId);
			if (psoInfo && std::holds_alternative<PSODesc>(psoInfo->objectDesc))
			{
				PSODesc const& psoDesc = std::get<PSODesc>(psoInfo->objectDesc);
				if (std::holds_alternative<ComputePSODescStorage>(psoDesc))
				{
					ComputePSODescStorage const& desc = std::get<ComputePSODescStorage>(psoDesc);
					if (ImGui::TreeNode("Compute Pipeline State"))
					{
						RenderComputePSODetails(desc, objectTracker);
						ImGui::TreePop();
					}
				}
				else if (std::holds_alternative<StreamPSODescStorage>(psoDesc))
				{
					StreamPSODescStorage const& desc = std::get<StreamPSODescStorage>(psoDesc);
					if (ImGui::TreeNode("Compute Pipeline State"))
					{
						RenderStreamPSODetails(desc, objectTracker);
						ImGui::TreePop();
					}
				}
			}
		}

		if (ImGui::TreeNode("CS"))
		{
			for (Uint32 i = 0; i < currentState.computeRootArguments.size(); ++i)
			{
				RenderRootParameterBinding(i, currentState.computeRootArguments[i], D3D12_SHADER_VISIBILITY_ALL, objectTracker, descriptorTracker, addressTracker, &selectedItemInViewer);
			}
			ImGui::TreePop();
		}
	}

	void GUI::RenderCopyCommandBoundResources(SelectedItem& selectedItemInViewer, ListCommand const* listCommand)
	{
		if (CopyBufferRegionCommand const* cmd = dyn_cast<CopyBufferRegionCommand>(listCommand))
		{
			if (ImGui::TreeNode("Copy Buffer Region"))
			{
				TrackedObjectInfo const* srcResourceInfo = objectTracker.GetObjectInfo(cmd->srcBufferId);
				ID3D12Resource* srcResource = srcResourceInfo ? reinterpret_cast<ID3D12Resource*>(srcResourceInfo->objectPtr) : nullptr;
				std::string srcLabel = GetResourceLabel("Copy Source", srcResource, objectTracker, addressTracker);
				DescriptorInfo srcDescInfo{ DescriptorViewType::Unknown, srcResourceInfo ? srcResourceInfo->objectId : InvalidObjectID };
				RenderSelectableResource(srcLabel, srcResource, srcDescInfo, selectedItemInViewer, srcResource);

				TrackedObjectInfo const* dstResourceInfo = objectTracker.GetObjectInfo(cmd->dstBufferId);
				ID3D12Resource* dstResource = dstResourceInfo ? reinterpret_cast<ID3D12Resource*>(dstResourceInfo->objectPtr) : nullptr;
				std::string dstLabel = GetResourceLabel("Copy Dest", dstResource, objectTracker, addressTracker);
				DescriptorInfo dstDescInfo{ DescriptorViewType::Unknown, dstResourceInfo ? dstResourceInfo->objectId : InvalidObjectID };
				RenderSelectableResource(dstLabel, dstResource, dstDescInfo, selectedItemInViewer, dstResource);

				ImGui::Text("Source Offset: %llu bytes", cmd->srcOffset);
				ImGui::Text("Destination Offset: %llu bytes", cmd->dstOffset);
				ImGui::Text("Size: %llu bytes", cmd->numBytes);

				ImGui::TreePop();
			}
		}
		else if (CopyTextureRegionCommand const* cmd = dyn_cast<CopyTextureRegionCommand>(listCommand))
		{
			if (ImGui::TreeNode("Copy Texture Region"))
			{
				TrackedObjectInfo const* srcResourceInfo = objectTracker.GetObjectInfo(cmd->srcTextureId);
				ID3D12Resource* srcResource = srcResourceInfo ? reinterpret_cast<ID3D12Resource*>(srcResourceInfo->objectPtr) : nullptr;
				std::string srcLabel = GetResourceLabel("Copy Source", srcResource, objectTracker, addressTracker);
				DescriptorInfo srcDescInfo{ DescriptorViewType::Unknown, srcResourceInfo ? srcResourceInfo->objectId : InvalidObjectID };
				RenderSelectableResource(srcLabel, srcResource, srcDescInfo, selectedItemInViewer, srcResource);

				TrackedObjectInfo const* dstResourceInfo = objectTracker.GetObjectInfo(cmd->dstTextureId);
				ID3D12Resource* dstResource = dstResourceInfo ? reinterpret_cast<ID3D12Resource*>(dstResourceInfo->objectPtr) : nullptr;
				std::string dstLabel = GetResourceLabel("Copy Dest", dstResource, objectTracker, addressTracker);
				DescriptorInfo dstDescInfo{ DescriptorViewType::Unknown, dstResourceInfo ? dstResourceInfo->objectId : InvalidObjectID };
				RenderSelectableResource(dstLabel, dstResource, dstDescInfo, selectedItemInViewer, dstResource);

				ImGui::Text("Destination (X, Y, Z): (%u, %u, %u)", cmd->dstX, cmd->dstY, cmd->dstZ);
				if (cmd->srcBox.has_value())
				{
					D3D12_BOX const& box = cmd->srcBox.value();
					ImGui::Text("Source Box: (left: %u, top: %u, front: %u, right: %u, bottom: %u, back: %u)",
						box.left, box.top, box.front, box.right, box.bottom, box.back);
				}
				else
				{
					ImGui::Text("Source Box: <Full Resource>");
				}

				ImGui::TreePop();
			}
		}
		else if (CopyResourceCommand const* cmd = dyn_cast<CopyResourceCommand>(listCommand))
		{
			if (ImGui::TreeNode("Copy Resource"))
			{
				TrackedObjectInfo const* srcResourceInfo = objectTracker.GetObjectInfo(cmd->srcResourceId);
				ID3D12Resource* srcResource = srcResourceInfo ? reinterpret_cast<ID3D12Resource*>(srcResourceInfo->objectPtr) : nullptr;
				std::string srcLabel = GetResourceLabel("Copy Source", srcResource, objectTracker, addressTracker);
				DescriptorInfo srcDescInfo{ DescriptorViewType::Unknown, srcResourceInfo ? srcResourceInfo->objectId : InvalidObjectID };
				RenderSelectableResource(srcLabel, srcResource, srcDescInfo, selectedItemInViewer, srcResource);

				TrackedObjectInfo const* dstResourceInfo = objectTracker.GetObjectInfo(cmd->dstResourceId);
				ID3D12Resource* dstResource = dstResourceInfo ? reinterpret_cast<ID3D12Resource*>(dstResourceInfo->objectPtr) : nullptr;
				std::string dstLabel = GetResourceLabel("Copy Dest", dstResource, objectTracker, addressTracker);
				DescriptorInfo dstDescInfo{ DescriptorViewType::Unknown, dstResourceInfo ? dstResourceInfo->objectId : InvalidObjectID };
				RenderSelectableResource(dstLabel, dstResource, dstDescInfo, selectedItemInViewer, dstResource);

				ImGui::TreePop();
			}
		}
	}

	void GUI::RenderDispatchMeshCommandBoundResources(SelectedItem& selectedItemInViewer)
	{
		if (currentState.pipelineStateId != InvalidObjectID)
		{
			TrackedObjectInfo const* psoInfo = objectTracker.GetObjectInfo(currentState.pipelineStateId);
			if (psoInfo && std::holds_alternative<PSODesc>(psoInfo->objectDesc))
			{
				PSODesc const& psoDesc = std::get<PSODesc>(psoInfo->objectDesc);
				if (std::holds_alternative<StreamPSODescStorage>(psoDesc))
				{
					StreamPSODescStorage const& desc = std::get<StreamPSODescStorage>(psoDesc);
					if (ImGui::TreeNode("Mesh Pipeline State"))
					{
						RenderStreamPSODetails(desc, objectTracker);
						ImGui::TreePop();
					}
				}
			}
		}

		if (ImGui::TreeNode("AS"))
		{
			for (Uint32 i = 0; i < currentState.graphicsRootArguments.size(); ++i)
			{
				RenderRootParameterBinding(i, currentState.graphicsRootArguments[i], D3D12_SHADER_VISIBILITY_AMPLIFICATION, objectTracker, descriptorTracker, addressTracker, &selectedItemInViewer);
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("MS"))
		{
			for (Uint32 i = 0; i < currentState.graphicsRootArguments.size(); ++i)
			{
				RenderRootParameterBinding(i, currentState.graphicsRootArguments[i], D3D12_SHADER_VISIBILITY_MESH, objectTracker, descriptorTracker, addressTracker, &selectedItemInViewer);
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("PS"))
		{
			for (Uint32 i = 0; i < currentState.graphicsRootArguments.size(); ++i)
			{
				RenderRootParameterBinding(i, currentState.graphicsRootArguments[i], D3D12_SHADER_VISIBILITY_PIXEL, objectTracker, descriptorTracker, addressTracker, &selectedItemInViewer);
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("OM"))
		{
			if (ImGui::TreeNode("Render Targets"))
			{
				if (currentState.numRenderTargetsSet == 0)
				{
					ImGui::Text("<None set>");
				}
				else
				{
					for (Uint32 i = 0; i < currentState.numRenderTargetsSet; ++i)
					{
						D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = currentState.renderTargetViews[i];
						if (rtvHandle.ptr == NULL) continue;

						auto const& [descHeapInfo, index] = descriptorTracker.FindDescriptorByCpuHandle(rtvHandle);
						if (index < 0 || !descHeapInfo) continue;

						DescriptorInfo const& descInfo = descHeapInfo->descriptors[index];
						if (std::holds_alternative<D3D12_RENDER_TARGET_VIEW_DESC>(descInfo.desc))
						{
							std::string rtvLabel = std::format("RTV {} :", i);
							TrackedObjectInfo const* resourceInfo = objectTracker.GetObjectInfo(descInfo.resourceId);
							ID3D12Resource* resource = resourceInfo ? reinterpret_cast<ID3D12Resource*>(resourceInfo->objectPtr) : nullptr;
							if (resourceInfo)
							{
								rtvLabel += std::format(" obj#{} ({})", resourceInfo->objectId, resourceInfo->objectDebugName);
							}
							else
							{
								rtvLabel += std::format(" obj#{} (<Unknown>)", descInfo.resourceId);
							}
							RenderSelectableResource(rtvLabel, resource, descInfo, selectedItemInViewer, resource);
						}
					}
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Depth Stencil"))
			{
				if (!currentState.depthStencilView)
				{
					ImGui::Text("<None set>");
				}
				else
				{
					D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = *currentState.depthStencilView;
					auto const& [descHeapInfo, index] = descriptorTracker.FindDescriptorByCpuHandle(dsvHandle);
					if (index >= 0 && descHeapInfo)
					{
						DescriptorInfo const& descInfo = descHeapInfo->descriptors[index];
						if (std::holds_alternative<D3D12_DEPTH_STENCIL_VIEW_DESC>(descInfo.desc))
						{
							std::string dsvLabel = "DSV :";
							TrackedObjectInfo const* resourceInfo = objectTracker.GetObjectInfo(descInfo.resourceId);
							ID3D12Resource* resource = resourceInfo ? reinterpret_cast<ID3D12Resource*>(resourceInfo->objectPtr) : nullptr;
							if (resourceInfo)
							{
								dsvLabel += std::format(" obj#{} ({})", resourceInfo->objectId, resourceInfo->objectDebugName);
							}
							else
							{
								dsvLabel += std::format(" obj#{} (<Unknown>)", descInfo.resourceId);
							}
							RenderSelectableResource(dsvLabel, resource, descInfo, selectedItemInViewer, resource);
						}
					}
					else
					{
						ImGui::Text("DSV: <Invalid or Missing Descriptor>");
					}
				}
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
	}

	void GUI::RenderDispatchRaysCommandBoundResources(SelectedItem& selectedItemInViewer)
	{
		if (currentState.pipelineStateId != InvalidObjectID)
		{
			TrackedObjectInfo const* psoInfo = objectTracker.GetObjectInfo(currentState.pipelineStateId);
			if (psoInfo && std::holds_alternative<StateObjectDesc>(psoInfo->objectDesc))
			{
				StateObjectDesc const& stateObjectDesc = std::get<StateObjectDesc>(psoInfo->objectDesc);
				if (ImGui::TreeNode("State Object"))
				{
					RenderStateObjectDesc(stateObjectDesc, objectTracker);
					ImGui::TreePop();
				}
			}
		}
		if (ImGui::TreeNode("Ray Tracing"))
		{
			for (Uint32 i = 0; i < currentState.computeRootArguments.size(); ++i)
			{
				RenderRootParameterBinding(i, currentState.computeRootArguments[i], D3D12_SHADER_VISIBILITY_ALL, objectTracker, descriptorTracker, addressTracker, &selectedItemInViewer);
			}
			ImGui::TreePop();
		}
	}

	void GUI::RenderTexture2DPreview(ID3D12Resource* resource, Float zoomFactor)
	{
		static Bool showR = true, showG = true, showB = true, showA = false;
		ImGui::Text("Select Channels:");
		ImGui::SameLine();
		ImGui::Checkbox("R##ChannelR", &showR);
		ImGui::SameLine();
		ImGui::Checkbox("G##ChannelG", &showG);
		ImGui::SameLine();
		ImGui::Checkbox("B##ChannelB", &showB);
		ImGui::SameLine();
		ImGui::Checkbox("A##ChannelA", &showA);

		static Char const* mipLabels[] = 
		{
			"Mip 0", "Mip 1", "Mip 2", "Mip 3", "Mip 4", "Mip 5", "Mip 6", "Mip 7",
			"Mip 8", "Mip 9", "Mip 10", "Mip 11", "Mip 12", "Mip 13", "Mip 14", "Mip 15"
		};

		static Int selectedMipLevel = 0;
		Uint32 const mipLevels = resource->GetDesc().MipLevels;
		ImGui::Text("Mip Level:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(75.0f);
		ImGui::Combo("##MipLevelCombo", &selectedMipLevel, mipLabels, mipLevels);

		D3D12_RESOURCE_DESC const& resDesc = resource->GetDesc();
		Float const aspectRatio = (Float)resDesc.Height / resDesc.Width;
		Float const availableWidth = ImGui::GetContentRegionAvail().x;
		Float const availableHeight = ImGui::GetContentRegionAvail().y;
		Float defaultWidth = std::min<Float>(availableWidth, (Float)resDesc.Width);
		Float defaultHeight = defaultWidth * aspectRatio;

		if (defaultHeight > availableHeight)
		{
			defaultHeight = availableHeight;
			defaultWidth = defaultHeight / aspectRatio;
		}
		Float zoomedWidth = defaultWidth * zoomFactor;
		Float zoomedHeight = defaultHeight * zoomFactor;

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Texture2D.MostDetailedMip = selectedMipLevel;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Format = resDesc.Format;
		srvDesc.Shader4ComponentMapping = D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(
			showR ? D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_0 : D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_0,
			showG ? D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_1 : D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_0,
			showB ? D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_2 : D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_0,
			showA ? D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_3 : D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_1
		);

		switch (resDesc.Format)
		{
		case DXGI_FORMAT_R16_TYPELESS:
			srvDesc.Format = DXGI_FORMAT_R16_UNORM;
			break;
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT:
			srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
			break;
		case DXGI_FORMAT_R24G8_TYPELESS:
			srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			break;
		case DXGI_FORMAT_R32G8X24_TYPELESS:
			srvDesc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
			break;
		}
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = imguiManager.GetCPUDescriptor();
		imguiManager.GetDevice()->CreateShaderResourceView(resource, &srvDesc, srvHandle);

		ImGui::SetCursorPos(ImVec2(0, ImGui::GetCursorPosY()));
		ImVec2 imagePos = ImGui::GetCursorScreenPos();
		ImVec2 imageSize = ImVec2(zoomedWidth, zoomedHeight);

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->AddRectFilled(imagePos, ImVec2(imagePos.x + imageSize.x, imagePos.y + imageSize.y), IM_COL32(0, 0, 0, 255));

		ImGui::Image((ImTextureID)imguiManager.GetGPUDescriptor().ptr, ImVec2(zoomedWidth, zoomedHeight),
			ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 1));
	}

	void GUI::RenderBufferPreview(ID3D12Resource* resource)
	{
		void* mappedData = nullptr;
		D3D12_RANGE readRange = { 0, static_cast<Uint64>(resource->GetDesc().Width) };
		HRESULT hr = resource->Map(0, &readRange, &mappedData);
		if (SUCCEEDED(hr) && mappedData)
		{
			static std::string userFormat;

			Uint64 byteSize = static_cast<Uint64>(resource->GetDesc().Width);
			BufferFormat format = BufferFormat::Parse(userFormat);

			ImGui::Text("Buffer Size: %llu bytes", byteSize);

			Char formatBuffer[256] = { 0 };
			strncpy_s(formatBuffer, userFormat.c_str(), sizeof(formatBuffer) - 1);
			if (ImGui::InputTextMultiline("Format", formatBuffer, sizeof(formatBuffer)))
			{
				userFormat = formatBuffer;
			}

			ImGui::BeginChild("BufferView", ImVec2(0, 200), true, ImGuiWindowFlags_HorizontalScrollbar);

			if (format.isValid && format.totalStride > 0 && (byteSize % format.totalStride) == 0)
			{
				Uint64 structCount = byteSize / format.totalStride;
				ImGui::Text("Buffer Contents (%s, %llu structs):", userFormat.c_str(), structCount);

				if (ImGui::BeginTable("BufferTable", static_cast<Int>(format.fields.size() + 1)))
				{
					ImGui::TableSetupColumn("Index");
					for (BufferField const& field : format.fields)
					{
						ImGui::TableSetupColumn(field.name.c_str());
					}
					ImGui::TableHeadersRow();

					Uint8 const* data = static_cast<Uint8 const*>(mappedData);
					for (Uint64 i = 0; i < structCount; ++i)
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text("[%llu]", i);

						Uint64 offset = i * format.totalStride;
						for (Uint64 j = 0; j < format.fields.size(); ++j)
						{
							BufferField const& field = format.fields[j];
							ImGui::TableSetColumnIndex(static_cast<Int>(j + 1));
							Uint8 const* fieldData = data + offset;
							offset += field.stride;

							if (field.type == BufferField::DataType::FloatMatrix)
							{
								Float const* floatData = reinterpret_cast<Float const*>(fieldData);
								for (Int r = 0; r < field.rows; ++r)
								{
									ImGui::Text("%s (row%d):", field.name.c_str(), r);
									ImGui::SameLine();
									for (Int c = 0; c < field.columns; ++c)
									{
										for (Int k = 0; k < field.baseComponents; ++k)
										{
											ImGui::Text("%.6f", floatData[(r * field.columns + c) * field.baseComponents + k]);
											if (k < field.baseComponents - 1)
											{
												ImGui::SameLine();
											}
										}
										if (c < field.columns - 1)
										{
											ImGui::SameLine();
											ImGui::Text(",");
											ImGui::SameLine();
										}
									}
								}
							}
							else if (field.type == BufferField::DataType::IntMatrix)
							{
								Int const* intData = reinterpret_cast<Int const*>(fieldData);
								for (Int r = 0; r < field.rows; ++r)
								{
									ImGui::Text("%s (row%d):", field.name.c_str(), r);
									ImGui::SameLine();
									for (Int c = 0; c < field.columns; ++c)
									{
										for (Int k = 0; k < field.baseComponents; ++k)
										{
											ImGui::Text("%d", intData[(r * field.columns + c) * field.baseComponents + k]);
											if (k < field.baseComponents - 1)
											{
												ImGui::SameLine();
											}
										}
										if (c < field.columns - 1)
										{
											ImGui::SameLine();
											ImGui::Text(",");
											ImGui::SameLine();
										}
									}
								}
							}
							else if (field.type == BufferField::DataType::UintMatrix)
							{
								Uint32 const* uintData = reinterpret_cast<Uint32 const*>(fieldData);
								for (Int r = 0; r < field.rows; ++r)
								{
									ImGui::Text("%s (row%d):", field.name.c_str(), r);
									ImGui::SameLine();
									for (Int c = 0; c < field.columns; ++c)
									{
										for (Int k = 0; k < field.baseComponents; ++k)
										{
											ImGui::Text("%u", uintData[(r * field.columns + c) * field.baseComponents + k]);
											if (k < field.baseComponents - 1)
											{
												ImGui::SameLine();
											}
										}
										if (c < field.columns - 1)
										{
											ImGui::SameLine();
											ImGui::Text(",");
											ImGui::SameLine();
										}
									}
								}
							}
							else if (field.type == BufferField::DataType::BoolMatrix)
							{
								Uint32 const* boolData = reinterpret_cast<Uint32 const*>(fieldData);
								for (Int r = 0; r < field.rows; ++r)
								{
									ImGui::Text("%s (row%d):", field.name.c_str(), r);
									ImGui::SameLine();
									for (Int c = 0; c < field.columns; ++c)
									{
										for (Int k = 0; k < field.baseComponents; ++k)
										{
											ImGui::Text("%s", boolData[(r * field.columns + c) * field.baseComponents + k] ? "true" : "false");
											if (k < field.baseComponents - 1)
											{
												ImGui::SameLine();
											}
										}
										if (c < field.columns - 1)
										{
											ImGui::SameLine();
											ImGui::Text(",");
											ImGui::SameLine();
										}
									}
								}
							}
							else
							{
								for (Int k = 0; k < field.rows; ++k)
								{
									switch (field.type)
									{
									case BufferField::DataType::Float:
									{
										Float const* floatData = reinterpret_cast<Float const*>(fieldData);
										ImGui::Text("%.6f", floatData[k]);
									}
									break;
									case BufferField::DataType::Int:
									{
										Int32 const* intData = reinterpret_cast<Int32 const*>(fieldData);
										ImGui::Text("%d", intData[k]);
									}
									break;
									case BufferField::DataType::Uint:
									{
										Uint32 const* uintData = reinterpret_cast<Uint32 const*>(fieldData);
										ImGui::Text("%u", uintData[k]);
									}
									break;
									case BufferField::DataType::Bool:
									{
										Uint32 const* boolData = reinterpret_cast<Uint32 const*>(fieldData);
										ImGui::Text("%s", boolData[k] ? "true" : "false");
									}
									}

									if (k < field.rows - 1)
									{
										ImGui::SameLine();
									}
								}
							}
						}
					}
					ImGui::EndTable();
				}
			}
			else
			{
				ImGui::Text("Invalid format or buffer size mismatch. Displaying hex view:");
				std::string hexView = BufferToHex(mappedData, byteSize);
				ImGui::TextUnformatted(hexView.c_str());
			}
			ImGui::EndChild();
			resource->Unmap(0, nullptr);
		}
		else
		{
			ImGui::Text("Failed to map readback buffer.");
		}
	}

}