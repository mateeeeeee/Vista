
#include "EventDetailsCommandVisitor.h"
#include "GUI/GUIRenderUtil.h" 
#include "Command/Commands.h"
#include "Command/CommandRecorder.h"
#include "Tracking/ObjectTracker.h"
#include "Tracking/DescriptorTracker.h"
#include "Tracking/ResourceAddressTracker.h"
#include "Utilities/D3D12Util.h"
#include "ImGui/imgui.h"
#include <format>

namespace vista
{
	void EventDetailsCommandVisitor::Visit(Command const& cmd)
	{
		ImGui::Text("Unknown Command:");
		ImGui::Text("%s", cmd.GetDesc().c_str());
	}

	void EventDetailsCommandVisitor::Visit(CreateCommandQueueCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("CommandQueue Info");

		if (TrackedObjectInfo const* info = objectTracker.GetObjectInfo(cmd.commandQueueId))
		{
			if (std::holds_alternative<QueueDesc>(info->objectDesc))
			{
				RenderQueueDesc(std::get<QueueDesc>(info->objectDesc)); 
			}
		}
	}
	void EventDetailsCommandVisitor::Visit(CreateCommandQueue1Command const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("CommandQueue1 Info");

		if (TrackedObjectInfo const* info = objectTracker.GetObjectInfo(cmd.commandQueueId))
		{
			if (std::holds_alternative<QueueDesc>(info->objectDesc))
			{
				RenderQueueDesc(std::get<QueueDesc>(info->objectDesc));
			}
		}
	}
	void EventDetailsCommandVisitor::Visit(CreateCommandListCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("CommandList Info");

		if (TrackedObjectInfo const* info = objectTracker.GetObjectInfo(cmd.commandListId))
		{
			if (std::holds_alternative<ListDesc>(info->objectDesc))
			{
				RenderListDesc(std::get<ListDesc>(info->objectDesc)); 
			}
		}
	}
	void EventDetailsCommandVisitor::Visit(CreateCommandList1Command const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("CommandList1 Info");

		if (TrackedObjectInfo const* info = objectTracker.GetObjectInfo(cmd.commandListId))
		{
			if (std::holds_alternative<ListDesc>(info->objectDesc))
			{
				RenderListDesc(std::get<ListDesc>(info->objectDesc));
			}
		}
	}
	void EventDetailsCommandVisitor::Visit(CreateCommandAllocatorCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("CommandAllocator Info");
		if (TrackedObjectInfo const* info = objectTracker.GetObjectInfo(cmd.commandAllocatorId))
		{
			if (std::holds_alternative<AllocatorDesc>(info->objectDesc))
			{
				RenderAllocatorDesc(std::get<AllocatorDesc>(info->objectDesc)); 
			}
		}
	}
	void EventDetailsCommandVisitor::Visit(CreateFenceCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Fence Info");

		if (TrackedObjectInfo const* info = objectTracker.GetObjectInfo(cmd.fenceId))
		{
			if (std::holds_alternative<FenceDesc>(info->objectDesc))
			{
				RenderFenceDesc(std::get<FenceDesc>(info->objectDesc)); 
			}
		}
	}
	void EventDetailsCommandVisitor::Visit(CreateHeapCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Heap Info");

		if (TrackedObjectInfo const* info = objectTracker.GetObjectInfo(cmd.heapId))
		{
			if (std::holds_alternative<HeapDesc>(info->objectDesc))
			{
				RenderHeapDesc(std::get<HeapDesc>(info->objectDesc));
			}
		}
	}
	void EventDetailsCommandVisitor::Visit(CreateHeap1Command const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Heap1 Info");

		if (TrackedObjectInfo const* info = objectTracker.GetObjectInfo(cmd.heapId))
		{
			if (std::holds_alternative<HeapDesc>(info->objectDesc))
			{
				RenderHeapDesc(std::get<HeapDesc>(info->objectDesc));
			}
		}
	}
	void EventDetailsCommandVisitor::Visit(CreateGraphicsPipelineStateCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("GraphicsPipelineState Info");

		if (TrackedObjectInfo const* info = objectTracker.GetObjectInfo(cmd.psoId))
		{
			ImGui::Text("Pipeline State ID: obj#%llu", info->objectId);
		}
		else
		{
			ImGui::TextUnformatted("Pipeline State not found");
		}
	}
	void EventDetailsCommandVisitor::Visit(CreateComputePipelineStateCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("ComputePipelineState Info");

		if (TrackedObjectInfo const* info = objectTracker.GetObjectInfo(cmd.psoId))
		{
			ImGui::Text("Pipeline State ID: obj#%llu", info->objectId);
		}
		else
		{
			ImGui::TextUnformatted("Pipeline State not found");
		}
	}
	void EventDetailsCommandVisitor::Visit(CreatePipelineStateCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("PipelineState Info");
		if (TrackedObjectInfo const* info = objectTracker.GetObjectInfo(cmd.psoId))
		{
			ImGui::Text("Pipeline State ID: obj#%llu", info->objectId);
		}
		else
		{
			ImGui::TextUnformatted("Pipeline State not found");
		}
	}
	void EventDetailsCommandVisitor::Visit(CreateStateObjectCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("StateObject Info");
		if (TrackedObjectInfo const* info = objectTracker.GetObjectInfo(cmd.stateObjectId))
		{
			ImGui::Text("State Object ID: obj#%llu", info->objectId);
		}
		else
		{
			ImGui::TextUnformatted("State Object not found");
		}
	}
	void EventDetailsCommandVisitor::Visit(AddToStateObjectCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("StateObject Info");
		if (TrackedObjectInfo const* info = objectTracker.GetObjectInfo(cmd.stateObjectId))
		{
			ImGui::Text("State Object ID: obj#%llu", info->objectId);
			ImGui::Text("Base State Object ID: obj#%llu", cmd.existingStateObjectId);
		}
		else
		{
			ImGui::TextUnformatted("State Object not found");
		}
	}
	void EventDetailsCommandVisitor::Visit(CreateRootSignatureCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Root Signature Info");

		if (TrackedObjectInfo const* info = objectTracker.GetObjectInfo(cmd.rootSignatureId))
		{
			if (ImGui::BeginTable("RootSigEventTable", 2, ImGuiTableFlags_SizingStretchSame))
			{
				auto DrawRow = [](Char const* name, std::string const& value)
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::TextUnformatted(name);
						ImGui::TableSetColumnIndex(1);
						ImGui::TextUnformatted(value.c_str());
					};

				DrawRow("this", std::format("Device obj#{}", cmd.GetParentId()));
				DrawRow("RootSignature", std::format("obj#{}", cmd.rootSignatureId));
				ImGui::EndTable();
			}
			else
			{
				ImGui::TextUnformatted("Root Signature not found");
			}
		}
	}
	void EventDetailsCommandVisitor::Visit(CreateCommandSignatureCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("CommandSignature Info");

		TrackedObjectInfo const* cmdSigInfo = objectTracker.GetObjectInfo(cmd.cmdSignatureId);

		if (ImGui::BeginTable("CreateCommandSignatureTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
		{
			ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 150.0f);
			ImGui::TableSetupColumn("Value");
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Command Signature ID");
			ImGui::TableSetColumnIndex(1);
			if (cmdSigInfo)
			{
				ImGui::Text("obj#%llu (%s)", cmd.cmdSignatureId, cmdSigInfo->objectDebugName.c_str());
			}
			else
			{
				ImGui::Text("obj#%llu (<Unknown>)", cmd.cmdSignatureId);
			}

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Result");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%s (0x%X)", SUCCEEDED(cmd.hr) ? "S_OK" : "FAILED", cmd.hr);

			ImGui::EndTable();
		}

		if (cmdSigInfo && std::holds_alternative<CommandSignatureDesc>(cmdSigInfo->objectDesc))
		{
			RenderCommandSignatureDesc(std::get<CommandSignatureDesc>(cmdSigInfo->objectDesc), objectTracker); 
		}
		else
		{
			ImGui::SeparatorText("Signature Details");
			ImGui::Text("<Not Available>");
		}
	}
	void EventDetailsCommandVisitor::Visit(CreateCommittedResourceCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("CommitedResource Info");

		if (TrackedObjectInfo const* info = objectTracker.GetObjectInfo(cmd.resourceId))
		{
			if (std::holds_alternative<ResourceDesc>(info->objectDesc))
			{
				RenderResourceDesc(std::get<ResourceDesc>(info->objectDesc)); 
			}
		}
	}
	void EventDetailsCommandVisitor::Visit(CreateCommittedResource1Command const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("CommittedResource1 Info");

		if (TrackedObjectInfo const* info = objectTracker.GetObjectInfo(cmd.resourceId))
		{
			if (std::holds_alternative<ResourceDesc>(info->objectDesc))
			{
				RenderResourceDesc(std::get<ResourceDesc>(info->objectDesc));
			}
		}
	}
	void EventDetailsCommandVisitor::Visit(CreatePlacedResourceCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("PlacedResource Info");

		if (TrackedObjectInfo const* info = objectTracker.GetObjectInfo(cmd.resourceId))
		{
			if (std::holds_alternative<ResourceDesc>(info->objectDesc))
			{
				RenderResourceDesc(std::get<ResourceDesc>(info->objectDesc));
			}
		}
	}
	void EventDetailsCommandVisitor::Visit(CreatePlacedResource1Command const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("PlacedResource1 Info");

		if (TrackedObjectInfo const* info = objectTracker.GetObjectInfo(cmd.resourceId))
		{
			if (std::holds_alternative<ResourceDesc>(info->objectDesc))
			{
				RenderResourceDesc(std::get<ResourceDesc>(info->objectDesc));
			}
		}
	}
	void EventDetailsCommandVisitor::Visit(CreateDescriptorHeapCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Descriptor Heap Info");

		if (TrackedObjectInfo const* info = objectTracker.GetObjectInfo(cmd.descriptorHeapId))
		{
			if (std::holds_alternative<DescriptorHeapDesc>(info->objectDesc))
			{
				RenderDescriptorHeapDesc(std::get<DescriptorHeapDesc>(info->objectDesc)); 
			}
		}
	}

	void EventDetailsCommandVisitor::Visit(ExecuteCommandListsCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Execute Info");

		ImGui::Text("Number of Lists: %zu", cmd.recorders.size());

		for (Uint32 i = 0; i < cmd.recorders.size(); ++i)
		{
			if (TrackedObjectInfo const* info = objectTracker.GetObjectInfo(cmd.recorders[i]->GetParentObjectID()))
			{
				ImGui::BulletText("[%zu] obj#%llu (%s)", i, info->objectId, ObjectTypeNames[(Uint64)info->objectType]);
			}
		}
	}
	void EventDetailsCommandVisitor::Visit(QueueSignalCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Signal Info");

		if (TrackedObjectInfo const* info = objectTracker.GetObjectInfo(cmd.fenceId))
		{
			ImGui::Text("Fence obj#%llu", info->objectId);
		}
		ImGui::Text("Value: %llu", cmd.fenceValue);
	}

	void EventDetailsCommandVisitor::Visit(CopyBufferRegionCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Copy Buffer Region Details");

		if (ImGui::BeginTable("CopyBufferRegionTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
		{
			ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 150.0f);
			ImGui::TableSetupColumn("Value");
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Destination Buffer");
			ImGui::TableSetColumnIndex(1);
			RenderObjectInfoByID(cmd.dstBufferId, objectTracker); 

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Destination Offset");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("0x%llX", cmd.dstOffset);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Source Buffer");
			ImGui::TableSetColumnIndex(1);
			RenderObjectInfoByID(cmd.srcBufferId, objectTracker); 

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Source Offset");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("0x%llX", cmd.srcOffset);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Number of Bytes");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%llu", cmd.numBytes);

			ImGui::EndTable();
		}
	}
	void EventDetailsCommandVisitor::Visit(CopyTextureRegionCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Copy Texture Region Details");

		if (ImGui::BeginTable("CopyTextureRegionTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
		{
			ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 150.0f);
			ImGui::TableSetupColumn("Value");
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Destination Texture");
			ImGui::TableSetColumnIndex(1);
			RenderObjectInfoByID(cmd.dstTextureId, objectTracker); 

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Destination (X, Y, Z)");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%u, %u, %u", cmd.dstX, cmd.dstY, cmd.dstZ);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Source Texture");
			ImGui::TableSetColumnIndex(1);
			RenderObjectInfoByID(cmd.srcTextureId, objectTracker); 

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Source Box");
			ImGui::TableSetColumnIndex(1);
			if (cmd.srcBox.has_value())
			{
				D3D12_BOX const& box = cmd.srcBox.value();
				ImGui::Text("Left=%u, Top=%u, Front=%u, Right=%u, Bottom=%u, Back=%u",
					box.left, box.top, box.front, box.right, box.bottom, box.back);
			}
			else
			{
				ImGui::Text("<Entire Resource>");
			}

			ImGui::EndTable();
		}
	}
	void EventDetailsCommandVisitor::Visit(CopyResourceCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Copy Resource Details");

		if (ImGui::BeginTable("CopyResourceTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
		{
			ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 150.0f);
			ImGui::TableSetupColumn("Value");
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Destination Resource");
			ImGui::TableSetColumnIndex(1);
			RenderObjectInfoByID(cmd.dstResourceId, objectTracker); 

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Source Resource");
			ImGui::TableSetColumnIndex(1);
			RenderObjectInfoByID(cmd.srcResourceId, objectTracker); 

			ImGui::EndTable();
		}
	}
	void EventDetailsCommandVisitor::Visit(ResolveSubresourceCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Resolve Subresource Details");

		if (ImGui::BeginTable("ResolveSubresourceTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
		{
			ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 150.0f);
			ImGui::TableSetupColumn("Value");
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Destination Resource");
			ImGui::TableSetColumnIndex(1);
			RenderObjectInfoByID(cmd.dstResourceId, objectTracker);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Destination Subresource");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%u", cmd.dstSubresource);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Source Resource");
			ImGui::TableSetColumnIndex(1);
			RenderObjectInfoByID(cmd.srcResourceId, objectTracker);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Source Subresource");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%u", cmd.srcSubresource);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Format");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%s", DXGIFormatToString(cmd.format));

			ImGui::EndTable();
		}
	}
	void EventDetailsCommandVisitor::Visit(ResolveSubresourceRegionCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Resolve Subresource Region Details");

		if (ImGui::BeginTable("ResolveSubresourceRegionTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
		{
			ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 150.0f);
			ImGui::TableSetupColumn("Value");
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Destination Resource");
			ImGui::TableSetColumnIndex(1);
			RenderObjectInfoByID(cmd.dstResourceId, objectTracker);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Destination Subresource");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%u", cmd.dstSubresource);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Destination X, Y");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%u, %u", cmd.dstX, cmd.dstY);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Source Resource");
			ImGui::TableSetColumnIndex(1);
			RenderObjectInfoByID(cmd.srcResourceId, objectTracker);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Source Subresource");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%u", cmd.srcSubresource);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Source Rect");
			ImGui::TableSetColumnIndex(1);
			if (cmd.srcRect.has_value())
			{
				RenderScissorRect(cmd.srcRect.value());
			}
			else
			{
				ImGui::Text("<Full Resource>");
			}
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Format");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%s", DXGIFormatToString(cmd.format));

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Resolve Mode");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%s", D3D12ResolveModeToString(cmd.resolveMode));

			ImGui::EndTable();
		}
	}

	void EventDetailsCommandVisitor::Visit(DrawInstancedCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Draw Info");

		ImGui::Text("VertexCountPerInstance: %u", cmd.vertexCountPerInstance);
		ImGui::Text("InstanceCount: %u", cmd.instanceCount);
		ImGui::Text("StartVertexLocation: %u", cmd.startVertexLocation);
		ImGui::Text("StartInstanceLocation: %u", cmd.startInstanceLocation);
	}
	void EventDetailsCommandVisitor::Visit(DrawIndexedInstancedCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Draw Info");

		ImGui::Text("IndexCountPerInstance: %u", cmd.indexCountPerInstance);
		ImGui::Text("InstanceCount: %u", cmd.instanceCount);
		ImGui::Text("StartIndexLocation: %u", cmd.startIndexLocation);
		ImGui::Text("BaseVertexLocation: %d", cmd.baseVertexLocation);
		ImGui::Text("StartInstanceLocation: %u", cmd.startInstanceLocation);
	}
	void EventDetailsCommandVisitor::Visit(DispatchCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Dispatch Info");

		ImGui::Text("ThreadGroupCountX: %u", cmd.threadGroupCountX);
		ImGui::Text("ThreadGroupCountY: %u", cmd.threadGroupCountY);
		ImGui::Text("ThreadGroupCountZ: %u", cmd.threadGroupCountZ);
	}
	void EventDetailsCommandVisitor::Visit(DispatchMeshCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("DispatchMesh Info");

		ImGui::Text("ThreadGroupCountX: %u", cmd.threadGroupCountX);
		ImGui::Text("ThreadGroupCountY: %u", cmd.threadGroupCountY);
		ImGui::Text("ThreadGroupCountZ: %u", cmd.threadGroupCountZ);
	}
	void EventDetailsCommandVisitor::Visit(ExecuteIndirectCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Execute Indirect Info");

		TrackedObjectInfo const* cmdSigInfo = objectTracker.GetObjectInfo(cmd.commandSignatureId);

		if (ImGui::BeginTable("ExecuteIndirectTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
		{
			ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 150.0f);
			ImGui::TableSetupColumn("Value");
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Command Signature");
			ImGui::TableSetColumnIndex(1);
			RenderObjectInfoByID(cmd.commandSignatureId, objectTracker); 

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Max Command Count");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%u", cmd.maxCommandCount);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Argument Buffer");
			ImGui::TableSetColumnIndex(1);
			RenderObjectInfoByID(cmd.argumentBufferId, objectTracker); 

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Argument Buffer Offset");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("0x%llX", cmd.argumentBufferOffset);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Count Buffer");
			ImGui::TableSetColumnIndex(1);
			RenderObjectInfoByID(cmd.countBufferId, objectTracker); 

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Count Buffer Offset");
			ImGui::TableSetColumnIndex(1);
			if (cmd.countBufferId != InvalidObjectID)
			{
				ImGui::Text("0x%llX", cmd.countBufferOffset);
			}
			else
			{
				ImGui::Text("N/A");
			}

			ImGui::EndTable();
		}

		if (cmdSigInfo && std::holds_alternative<CommandSignatureDesc>(cmdSigInfo->objectDesc))
		{
			RenderCommandSignatureDesc(std::get<CommandSignatureDesc>(cmdSigInfo->objectDesc), objectTracker); 
		}
	}
	void EventDetailsCommandVisitor::Visit(DispatchRaysCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("DispatchRays Info");

		ImGui::Text("DispatchWidth: %u", cmd.dispatchWidth);
		ImGui::Text("DispatchHeight: %u", cmd.dispatchHeight);
		ImGui::Text("DispatchDepth: %u", cmd.dispatchDepth);
	}
	void EventDetailsCommandVisitor::Visit(RSSetViewportsCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Viewports");

		for (Uint32 i = 0; i < cmd.viewports.size(); ++i)
		{
			RenderViewport(i, cmd.viewports[i]); 
		}
	}
	void EventDetailsCommandVisitor::Visit(RSSetScissorRectsCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Scissor Rects");

		for (Uint32 i = 0; i < cmd.scissorRects.size(); ++i)
		{
			RenderScissorRect(i, cmd.scissorRects[i]); 
		}
	}
	void EventDetailsCommandVisitor::Visit(RSSetShadingRateCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Shading Rate");

		ImGui::Text("Shading Rate: %s", D3D12ShadingRateToString(cmd.shadingRate));
		Char const* combinerNames[] = { "Combiner 0", "Combiner 1" };

		if (ImGui::BeginTable("ShadingRateCombiners", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
		{
			ImGui::TableSetupColumn("Combiner", ImGuiTableColumnFlags_WidthFixed, 150.0f);
			ImGui::TableSetupColumn("Value");
			ImGui::TableHeadersRow();

			for (Int i = 0; i < 2; ++i)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s", combinerNames[i]);

				ImGui::TableSetColumnIndex(1);
				ImGui::Text("%s", D3D12ShadingRateCombinerToString(cmd.shadingRateCombiners[i]));
			}
			ImGui::EndTable();
		}
	}
	void EventDetailsCommandVisitor::Visit(RSSetShadingRateImageCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Shading Rate Image");
		RenderObjectInfoByID(cmd.shadingRateImageId, objectTracker);
	}
	void EventDetailsCommandVisitor::Visit(SetViewInstanceMaskCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Set View Instance Mask");
		ImGui::Text("Mask: %u", cmd.viewInstanceMask);
	}
	void EventDetailsCommandVisitor::Visit(OMSetRenderTargetsCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Render Targets");

		ImGui::Text("Num RTVs: %zu", cmd.renderTargetDescriptors.size());
		ImGui::Text("DepthStencil Set: %s", cmd.depthStencilDescriptor.ptr != 0 ? "Yes" : "No");

		if (ImGui::TreeNode("Render Target Views"))
		{
			for (Uint32 i = 0; i < cmd.renderTargetDescriptors.size(); ++i)
			{
				RenderDescriptorInfoByHandle(cmd.renderTargetDescriptors[i], std::format("RTV {}", i).c_str(), descriptorTracker); 
			}
			if (cmd.renderTargetDescriptors.empty())
			{
				ImGui::Text("<None set>");
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Depth Stencil View"))
		{
			RenderDescriptorInfoByHandle(cmd.depthStencilDescriptor, "DSV", descriptorTracker); 
			ImGui::TreePop();
		}
	}
	void EventDetailsCommandVisitor::Visit(OMSetBlendFactorCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Blend Factor");

		ImGui::Text("BlendFactor[0]: %f", cmd.blendFactor[0]);
		ImGui::Text("BlendFactor[1]: %f", cmd.blendFactor[1]);
		ImGui::Text("BlendFactor[2]: %f", cmd.blendFactor[2]);
		ImGui::Text("BlendFactor[3]: %f", cmd.blendFactor[3]);
	}
	void EventDetailsCommandVisitor::Visit(OMSetStencilRefCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Stencil Ref");
		ImGui::Text("Stencil Ref: %u", cmd.stencilRef);
	}
	void EventDetailsCommandVisitor::Visit(OMSetDepthBoundsCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Stencil Ref");
		ImGui::Text("Depth Min: %f", cmd.depthMin);
		ImGui::Text("Depth Max: %f", cmd.depthMax);
	}
	void EventDetailsCommandVisitor::Visit(BeginRenderPassCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Begin Render Pass Details");

		if (ImGui::BeginTable("BeginRenderPassTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
		{
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 150.0f);
			ImGui::TableSetupColumn("Value");
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("this");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("ID3D12GraphicsCommandList4 obj#%llu", cmd.GetParentId());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("NumRenderTargets");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%zu", cmd.renderTargets.size());

			if (!cmd.renderTargets.empty())
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				if (ImGui::TreeNode("pRenderTargets"))
				{
					for (Uint32 i = 0; i < cmd.renderTargets.size(); ++i)
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text("cpuDescriptor");
						ImGui::TableSetColumnIndex(1);
						RenderDescriptorInfoByHandle(cmd.renderTargets[i].cpuDescriptor, std::format("RTV {}", i).c_str(), descriptorTracker);

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						if (ImGui::TreeNode(std::format("BeginningAccess{}", i).c_str()))
						{
							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);
							ImGui::Text("Type");
							ImGui::TableSetColumnIndex(1);
							std::string beginType;
							switch (cmd.renderTargets[i].BeginningAccess.Type)
							{
							case D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR:
								beginType = "D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR";
								break;
							case D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE:
								beginType = "D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE";
								break;
							case D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_NO_ACCESS:
								beginType = "D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_NO_ACCESS";
								break;
							default:
								beginType = "UNKNOWN";
							}
							ImGui::Text("%s", beginType.c_str());
							ImGui::TreePop();
						}

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						if (ImGui::TreeNode(std::format("EndingAccess{}", i).c_str()))
						{
							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);
							ImGui::Text("Type");
							ImGui::TableSetColumnIndex(1);
							std::string endType;
							switch (cmd.renderTargets[i].EndingAccess.Type)
							{
							case D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE:
								endType = "D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE";
								break;
							case D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD:
								endType = "D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD";
								break;
							case D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_NO_ACCESS:
								endType = "D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_NO_ACCESS";
								break;
							default:
								endType = "UNKNOWN";
							}
							ImGui::Text("%s", endType.c_str());
							ImGui::TreePop();
						}
					}
					ImGui::TreePop();
				}
			}

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			if (cmd.depthStencil.has_value())
			{
				if (ImGui::TreeNode("pDepthStencil"))
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("cpuDescriptor");
					ImGui::TableSetColumnIndex(1);
					RenderDescriptorInfoByHandle(cmd.depthStencil->cpuDescriptor, "DSV", descriptorTracker);

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					if (ImGui::TreeNode("DepthBeginningAccess"))
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text("Type");
						ImGui::TableSetColumnIndex(1);
						std::string depthBeginType;
						switch (cmd.depthStencil->DepthBeginningAccess.Type)
						{
						case D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR:
							depthBeginType = "D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR";
							break;
						case D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE:
							depthBeginType = "D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE";
							break;
						case D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_NO_ACCESS:
							depthBeginType = "D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_NO_ACCESS";
							break;
						default:
							depthBeginType = "UNKNOWN";
						}
						ImGui::Text("%s", depthBeginType.c_str());
						ImGui::TreePop();
					}

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					if (ImGui::TreeNode("DepthEndingAccess"))
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text("Type");
						ImGui::TableSetColumnIndex(1);
						std::string depthEndType;
						switch (cmd.depthStencil->DepthEndingAccess.Type)
						{
						case D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE:
							depthEndType = "D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE";
							break;
						case D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD:
							depthEndType = "D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD";
							break;
						case D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_NO_ACCESS:
							depthEndType = "D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_NO_ACCESS";
							break;
						default:
							depthEndType = "UNKNOWN";
						}
						ImGui::Text("%s", depthEndType.c_str());
						ImGui::TreePop();
					}

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					if (ImGui::TreeNode("StencilBeginningAccess"))
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text("Type");
						ImGui::TableSetColumnIndex(1);
						std::string stencilBeginType;
						switch (cmd.depthStencil->StencilBeginningAccess.Type)
						{
						case D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR:
							stencilBeginType = "D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR";
							break;
						case D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE:
							stencilBeginType = "D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE";
							break;
						case D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_NO_ACCESS:
							stencilBeginType = "D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_NO_ACCESS";
							break;
						default:
							stencilBeginType = "UNKNOWN";
						}
						ImGui::Text("%s", stencilBeginType.c_str());
						ImGui::TreePop();
					}

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					if (ImGui::TreeNode("StencilEndingAccess"))
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text("Type");
						ImGui::TableSetColumnIndex(1);
						std::string stencilEndType;
						switch (cmd.depthStencil->StencilEndingAccess.Type)
						{
						case D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE:
							stencilEndType = "D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE";
							break;
						case D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD:
							stencilEndType = "D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD";
							break;
						case D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_NO_ACCESS:
							stencilEndType = "D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_NO_ACCESS";
							break;
						default:
							stencilEndType = "UNKNOWN";
						}
						ImGui::Text("%s", stencilEndType.c_str());
						ImGui::TreePop();
					}

					ImGui::TreePop();
				}
			}
			else
			{
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("pDepthStencil");
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("nullptr");
			}

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Flags");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%s", cmd.flags == D3D12_RENDER_PASS_FLAG_NONE ? "D3D12_RENDER_PASS_FLAG_NONE" : "Unknown");

			ImGui::EndTable();
		}
	}
	void EventDetailsCommandVisitor::Visit(EndRenderPassCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("End Render Pass");
		ImGui::Text("No additional details.");
	}
	void EventDetailsCommandVisitor::Visit(ClearRenderTargetViewCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Clear Render Target View");

		if (ImGui::BeginTable("ClearRTVTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
		{
			ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 150.0f);
			ImGui::TableSetupColumn("Value");
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Render Target View");
			ImGui::TableSetColumnIndex(1);
			RenderDescriptorInfoByHandle(cmd.renderTargetView, "RTV Handle", descriptorTracker); 

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Clear Color");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("[%.3f, %.3f, %.3f, %.3f]", cmd.colorRGBA[0], cmd.colorRGBA[1], cmd.colorRGBA[2], cmd.colorRGBA[3]);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Clear Rectangles");
			ImGui::TableSetColumnIndex(1);
			if (cmd.rects.empty())
			{
				ImGui::Text("<Entire View>");
			}
			else
			{
				ImGui::Text("%zu rects", cmd.rects.size());
				ImGui::Indent();
				for (Uint32 i = 0; i < cmd.rects.size(); ++i)
				{
					RenderScissorRect(i, cmd.rects[i]); 
				}
				ImGui::Unindent();
			}

			ImGui::EndTable();
		}
	}
	void EventDetailsCommandVisitor::Visit(ClearDepthStencilViewCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Clear Depth Stencil View");

		if (ImGui::BeginTable("ClearDSVTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
		{
			ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 150.0f);
			ImGui::TableSetupColumn("Value");
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Depth Stencil View");
			ImGui::TableSetColumnIndex(1);
			RenderDescriptorInfoByHandle(cmd.depthStencilView, "DSV Handle", descriptorTracker); 

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Clear Flags");
			ImGui::TableSetColumnIndex(1);
			std::string flagsStr;
			if (cmd.clearFlags & D3D12_CLEAR_FLAG_DEPTH)
				flagsStr += "DEPTH ";
			if (cmd.clearFlags & D3D12_CLEAR_FLAG_STENCIL)
				flagsStr += "STENCIL";
			ImGui::Text("%s", flagsStr.empty() ? "NONE" : flagsStr.c_str());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Depth Value");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%.3f", cmd.depth);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Stencil Value");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("0x%X", cmd.stencil);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Clear Rectangles");
			ImGui::TableSetColumnIndex(1);
			if (cmd.rects.empty())
			{
				ImGui::Text("<Entire View>");
			}
			else
			{
				ImGui::Text("%zu rects", cmd.rects.size());
				ImGui::Indent();
				for (Uint32 i = 0; i < cmd.rects.size(); ++i)
				{
					RenderScissorRect(i, cmd.rects[i]); 
				}
				ImGui::Unindent();
			}
			ImGui::EndTable();
		}
	}
	void EventDetailsCommandVisitor::Visit(ClearUnorderedAccessViewUintCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Clear Unordered Access View (Uint)");

		if (ImGui::BeginTable("ClearUAVUintTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
		{
			ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 150.0f);
			ImGui::TableSetupColumn("Value");
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("GPU Descriptor Handle");
			ImGui::TableSetColumnIndex(1);
			RenderDescriptorInfoByHandle(cmd.gpuUnorderedAccessView, "GPU Handle", descriptorTracker); 

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("CPU Descriptor Handle");
			ImGui::TableSetColumnIndex(1);
			RenderDescriptorInfoByHandle(cmd.cpuUnorderedAccessView, "CPU Handle", descriptorTracker); 

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Resource");
			ImGui::TableSetColumnIndex(1);
			RenderObjectInfoByID(cmd.resourceId, objectTracker); 

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Clear Values");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("[%u, %u, %u, %u]", cmd.values[0], cmd.values[1], cmd.values[2], cmd.values[3]);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Clear Rectangles");
			ImGui::TableSetColumnIndex(1);
			if (cmd.rects.empty())
			{
				ImGui::Text("<Entire View>");
			}
			else
			{
				ImGui::Text("%zu rects", cmd.rects.size());
				ImGui::Indent();
				for (Uint32 i = 0; i < cmd.rects.size(); ++i)
				{
					RenderScissorRect(i, cmd.rects[i]); 
				}
				ImGui::Unindent();
			}

			ImGui::EndTable();
		}
	}
	void EventDetailsCommandVisitor::Visit(ClearUnorderedAccessViewFloatCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Clear Unordered Access View (Float)");

		if (ImGui::BeginTable("ClearUAVFloatTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
		{
			ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 150.0f);
			ImGui::TableSetupColumn("Value");
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("GPU Descriptor Handle");
			ImGui::TableSetColumnIndex(1);
			RenderDescriptorInfoByHandle(cmd.gpuUnorderedAccessView, "GPU Handle", descriptorTracker); 

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("CPU Descriptor Handle");
			ImGui::TableSetColumnIndex(1);
			RenderDescriptorInfoByHandle(cmd.cpuUnorderedAccessView, "CPU Handle", descriptorTracker); 

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Resource");
			ImGui::TableSetColumnIndex(1);
			RenderObjectInfoByID(cmd.resourceId, objectTracker); 

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Clear Values");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("[%.3f, %.3f, %.3f, %.3f]", cmd.values[0], cmd.values[1], cmd.values[2], cmd.values[3]);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Clear Rectangles");
			ImGui::TableSetColumnIndex(1);
			if (cmd.rects.empty())
			{
				ImGui::Text("<Entire View>");
			}
			else
			{
				ImGui::Text("%zu rects", cmd.rects.size());
				ImGui::Indent();
				for (Uint32 i = 0; i < cmd.rects.size(); ++i)
				{
					RenderScissorRect(i, cmd.rects[i]); 
				}
				ImGui::Unindent();
			}

			ImGui::EndTable();
		}
	}
	void EventDetailsCommandVisitor::Visit(SetPipelineStateCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::Text("this:");
		ImGui::SameLine();
		ImGui::Text("obj#%llu", cmd.GetParentId());
		ImGui::Text("PipelineState:");
		ImGui::SameLine();
		RenderObjectInfoByID(cmd.psoId, objectTracker); 
	}
	void EventDetailsCommandVisitor::Visit(SetPipelineState1Command const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::Text("this:");
		ImGui::SameLine();
		ImGui::Text("obj#%llu", cmd.GetParentId());
		ImGui::Text("StateObject:");
		ImGui::SameLine();
		RenderObjectInfoByID(cmd.stateObjectId, objectTracker);
	}
	void EventDetailsCommandVisitor::Visit(SetGraphicsRootSignatureCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::Text("this:");
		ImGui::SameLine();
		ImGui::Text("obj#%llu", cmd.GetParentId());
		ImGui::Text("RootSignature:");
		ImGui::SameLine();
		RenderObjectInfoByID(cmd.rootSignatureId, objectTracker); 
	}
	void EventDetailsCommandVisitor::Visit(SetComputeRootSignatureCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::Text("this:");
		ImGui::SameLine();
		ImGui::Text("obj#%llu", cmd.GetParentId());
		ImGui::Text("RootSignature:");
		ImGui::SameLine();
		RenderObjectInfoByID(cmd.rootSignatureId, objectTracker); 
	}
	void EventDetailsCommandVisitor::Visit(IASetPrimitiveTopologyCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
	}
	void EventDetailsCommandVisitor::Visit(IASetVertexBuffersCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Vertex Buffers");

		ImGui::Text("Views: %zu", cmd.vertexBufferViews.size());
		if (cmd.vertexBufferViews.empty())
		{
			ImGui::Text("<No vertex buffers set>");
			return;
		}

		if (ImGui::BeginTable("VertexBuffersTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY))
		{
			ImGui::TableSetupColumn("Slot", ImGuiTableColumnFlags_WidthFixed, 50.0f);
			ImGui::TableSetupColumn("Buffer Location", ImGuiTableColumnFlags_WidthFixed, 120.0f);
			ImGui::TableSetupColumn("Stride (Bytes)", ImGuiTableColumnFlags_WidthFixed, 100.0f);
			ImGui::TableSetupColumn("Size (Bytes)", ImGuiTableColumnFlags_WidthFixed, 100.0f);
			ImGui::TableSetupColumn("Resource Details");
			ImGui::TableHeadersRow();

			for (Uint32 i = 0; i < cmd.vertexBufferViews.size(); ++i)
			{
				Uint32 slot = cmd.startSlot + static_cast<Uint32>(i);
				RenderVertexBufferView(slot, cmd.vertexBufferViews[i], objectTracker, addressTracker); 
			}
			ImGui::EndTable();
		}
	}
	void EventDetailsCommandVisitor::Visit(IASetIndexBufferCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Index Buffer");

		if (!cmd.indexBufferView.has_value())
		{
			ImGui::Text("<None set>");
			return;
		}

		RenderIndexBufferView(cmd.indexBufferView.value(), objectTracker, addressTracker); 
	}
	void EventDetailsCommandVisitor::Visit(ResourceBarrierCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Barriers");

		for (Uint32 i = 0; i < cmd.barriers.size(); ++i)
		{
			RenderResourceBarrier(i, cmd.barriers[i], objectTracker); 
		}
	}
	void EventDetailsCommandVisitor::Visit(SetDescriptorHeapsCommand const& cmd)
	{
		ImGui::Text("this:");
		ImGui::SameLine();
		ImGui::Text("obj#%llu", cmd.GetParentId());
		ImGui::Text("NumDescriptorHeaps:");
		ImGui::SameLine();
		ImGui::Text("%u", cmd.numDescriptorHeaps);
		if (ImGui::TreeNode("ppDescriptorHeaps"))
		{
			for (Uint32 i = 0; i < cmd.numDescriptorHeaps; ++i)
			{
				RenderObjectInfoByID(cmd.descriptorHeapIds[i], objectTracker, std::format("Heap {}", i).c_str()); 
			}
			ImGui::TreePop();
		}
	}
	void EventDetailsCommandVisitor::Visit(SetGraphicsRootDescriptorTableCommand const& cmd)
	{
		ImGui::Text("this:");
		ImGui::SameLine();
		ImGui::Text("obj#%llu", cmd.GetParentId());
		ImGui::Text("Root Parameter Index:");
		ImGui::SameLine();
		ImGui::Text("%u", cmd.rootParameterIndex);
		RenderDescriptorInfoByHandle(cmd.baseDescriptor, "BaseDescriptor", descriptorTracker); 
	}
	void EventDetailsCommandVisitor::Visit(SetComputeRootDescriptorTableCommand const& cmd)
	{
		ImGui::Text("this:");
		ImGui::SameLine();
		ImGui::Text("obj#%llu", cmd.GetParentId());
		ImGui::Text("Root Parameter Index:");
		ImGui::SameLine();
		ImGui::Text("%u", cmd.rootParameterIndex);
		RenderDescriptorInfoByHandle(cmd.baseDescriptor, "BaseDescriptor", descriptorTracker); 
	}
	void EventDetailsCommandVisitor::Visit(SetGraphicsRoot32BitConstantCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Set Graphics Root 32-Bit Constant Details");

		if (ImGui::BeginTable("SetGraphicsRoot32BitConstantTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
		{
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 150.0f);
			ImGui::TableSetupColumn("Value");
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("this");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("ID3D12GraphicsCommandList obj#%llu", cmd.GetParentId());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("RootParameterIndex");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%u", cmd.rootParameterIndex);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("pSrcData");
			ImGui::TableSetColumnIndex(1);
			union { Uint32 u; Float f; } data;
			data.u = cmd.srcData;
			ImGui::Text("<blob>");
			if (ImGui::TreeNode("Values"))
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("0");
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("(0x%08X) (%.6f)", data.u, data.f);
				ImGui::TreePop();
			}
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("DestOffsetIn32BitValues");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%u", cmd.destOffsetIn32BitValues);

			ImGui::EndTable();
		}
	}
	void EventDetailsCommandVisitor::Visit(SetComputeRoot32BitConstantCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Set Compute Root 32-Bit Constant Details");

		if (ImGui::BeginTable("SetComputeRoot32BitConstantTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
		{
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 150.0f);
			ImGui::TableSetupColumn("Value");
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("this");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("ID3D12GraphicsCommandList obj#%llu", cmd.GetParentId());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("RootParameterIndex");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%u", cmd.rootParameterIndex);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("pSrcData");
			ImGui::TableSetColumnIndex(1);
			union { Uint32 u; Float f; } data;
			data.u = cmd.srcData;
			ImGui::Text("<blob>");
			if (ImGui::TreeNode("Values"))
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("0");
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("(0x%08X) (%.6f)", data.u, data.f);
				ImGui::TreePop();
			}

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("DestOffsetIn32BitValues");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%u", cmd.destOffsetIn32BitValues);

			ImGui::EndTable();
		}
	}
	void EventDetailsCommandVisitor::Visit(SetGraphicsRoot32BitConstantsCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Set Graphics Root 32-Bit Constants Details");

		if (ImGui::BeginTable("SetGraphicsRoot32BitConstantsTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
		{
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 150.0f);
			ImGui::TableSetupColumn("Value");
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("this");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("ID3D12GraphicsCommandList obj#%llu", cmd.GetParentId());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("RootParameterIndex");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%u", cmd.rootParameterIndex);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Num32BitValuesToSet");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%zu", cmd.srcData.size());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("pSrcData");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("<blob>");
			if (ImGui::TreeNode("Values"))
			{
				for (Uint32 i = 0; i < cmd.srcData.size(); ++i)
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%zu", i);
					ImGui::TableSetColumnIndex(1);
					union { Uint32 u; Float f; } data;
					data.u = cmd.srcData[i];
					ImGui::Text("(0x%08X) (%.6f)", data.u, data.f);
				}
				ImGui::TreePop();
			}

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("DestOffsetIn32BitValues");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%u", cmd.destOffsetIn32BitValues);

			ImGui::EndTable();
		}
	}
	void EventDetailsCommandVisitor::Visit(SetComputeRoot32BitConstantsCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("Set Compute Root 32-Bit Constants Details");

		if (ImGui::BeginTable("SetComputeRoot32BitConstantsTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
		{
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 150.0f);
			ImGui::TableSetupColumn("Value");
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("this");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("ID3D12GraphicsCommandList obj#%llu", cmd.GetParentId());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("RootParameterIndex");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%u", cmd.rootParameterIndex);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Num32BitValuesToSet");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%zu", cmd.srcData.size());

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("pSrcData");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("<blob>");
			if (ImGui::TreeNode("Values"))
			{
				for (Uint32 i = 0; i < cmd.srcData.size(); ++i)
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%zu", i);
					ImGui::TableSetColumnIndex(1);
					union { Uint32 u; Float f; } data;
					data.u = cmd.srcData[i];
					ImGui::Text("(0x%08X) (%.6f)", data.u, data.f);
				}
				ImGui::TreePop();
			}

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("DestOffsetIn32BitValues");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%u", cmd.destOffsetIn32BitValues);

			ImGui::EndTable();
		}
	}
	void EventDetailsCommandVisitor::Visit(SetGraphicsRootConstantBufferViewCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("SetGraphicsRootConstantBufferView Details");
		VisitSetRootViewCommandCommon(cmd.GetParentId(), cmd.rootParameterIndex, cmd.gpuVA, "SetGraphicsRootConstantBufferView");
	}
	void EventDetailsCommandVisitor::Visit(SetComputeRootConstantBufferViewCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("SetComputeRootConstantBufferView Details");
		VisitSetRootViewCommandCommon(cmd.GetParentId(), cmd.rootParameterIndex, cmd.gpuVA, "SetComputeRootConstantBufferView");
	}
	void EventDetailsCommandVisitor::Visit(SetGraphicsRootShaderResourceViewCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("SetGraphicsRootShaderResourceView Details");
		VisitSetRootViewCommandCommon(cmd.GetParentId(), cmd.rootParameterIndex, cmd.gpuVA, "SetGraphicsRootShaderResourceView");
	}
	void EventDetailsCommandVisitor::Visit(SetComputeRootShaderResourceViewCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("SetComputeRootShaderResourceView Details");
		VisitSetRootViewCommandCommon(cmd.GetParentId(), cmd.rootParameterIndex, cmd.gpuVA, "SetComputeRootShaderResourceView");
	}
	void EventDetailsCommandVisitor::Visit(SetGraphicsRootUnorderedAccessViewCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("SetGraphicsRootUnorderedAccessView Details");
		VisitSetRootViewCommandCommon(cmd.GetParentId(), cmd.rootParameterIndex, cmd.gpuVA, "SetGraphicsRootUnorderedAccessView");
	}
	void EventDetailsCommandVisitor::Visit(SetComputeRootUnorderedAccessViewCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
		ImGui::SeparatorText("SetComputeRootUnorderedAccessView Details");
		VisitSetRootViewCommandCommon(cmd.GetParentId(), cmd.rootParameterIndex, cmd.gpuVA, "SetComputeRootUnorderedAccessView");
	}
	void EventDetailsCommandVisitor::Visit(CloseListCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
	}
	void EventDetailsCommandVisitor::Visit(ResetListCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
	}
	void EventDetailsCommandVisitor::Visit(ClearStateCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
	}

	void EventDetailsCommandVisitor::Visit(ResourceSetNameCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());

	}

	void EventDetailsCommandVisitor::Visit(GetCompletedValueCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
	}
	void EventDetailsCommandVisitor::Visit(SetEventOnCompletionCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
	}
	void EventDetailsCommandVisitor::Visit(FenceSignalCommand const& cmd)
	{
		ImGui::Text("Command: %s", cmd.GetDesc().c_str());
	}

	void EventDetailsCommandVisitor::VisitSetRootViewCommandCommon(ObjectID parentId, Uint32 rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS gpuVA, Char const* label)
	{
		if (ImGui::BeginTable(label, 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
		{
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 150.0f);
			ImGui::TableSetupColumn("Value");
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("this");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("ID3D12GraphicsCommandList obj#%llu", parentId);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("RootParameterIndex");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%u", rootParameterIndex);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("BufferLocation");
			ImGui::TableSetColumnIndex(1);
			if (ImGui::TreeNode("BufferLocation"))
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("Offset");
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("%llu", gpuVA & 0xFFFFFFFFFFFF);

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("ResourceOrHeap");
				ImGui::TableSetColumnIndex(1);
				ID3D12Resource* resource = addressTracker.QueryResource(gpuVA);
				if (resource)
				{
					TrackedObjectInfo const* resourceInfo = objectTracker.GetObjectInfo(resource);
					if (resourceInfo)
					{
						ImGui::Text("obj#%llu", resourceInfo->objectId);
					}
					else
					{
						ImGui::Text("<Unknown Resource>");
					}
				}
				else
				{
					ImGui::Text("<Unmapped Address>");
				}
				ImGui::TreePop();
			}
			ImGui::EndTable();
		}
	}

}