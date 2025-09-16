#pragma once
#include "CommandVisitor.h"
#include "Tracking/ObjectID.h"
#include "Utilities/Hash.h"
#include "Utilities/Enum.h"
#include "Utilities/IntrusiveList.h"

namespace vista
{
	enum class CommandCategory : Uint16
	{
		Invalid = 0x00,
		Queue = 0x01,
		Device = 0x02,
		List = 0x04,
		Allocator = 0x08,
		Fence = 0x10,
		Resource = 0x20,
	};
	ENABLE_ENUM_BIT_OPERATORS(CommandCategory);

	enum class CommandType : Uint16
	{
		Invalid,

		DeviceCommand_Begin,
		CreateCommandQueue = DeviceCommand_Begin,
		CreatorCommandQueue1,
		CreateCommandList,
		CreateCommandList1,
		CreateCommandAllocator,
		CreateHeap,
		CreateHeap1,
		CreateFence,
		CreateRootSignature,
		CreateCommandSignature,
		CreateGraphicsPSO,
		CreateComputePSO,
		CreatePipelineState,
		CreateStateObject,
		AddToStateObject,
		CreateCommittedResource,
		CreateCommittedResource1,
		CreatePlacedResource,
		CreatePlacedResource1,
		CreateDescriptorHeap,
		CreateRenderTargetView,
		DeviceCommand_End = CreateRenderTargetView,

		QueueCommand_Begin,
		ExecuteCmdLists = QueueCommand_Begin,
		Queue_Signal,
		QueueCommand_End = Queue_Signal,

		ListCommand_Begin,
		CopyBufferRegion = ListCommand_Begin,
		CopyTextureRegion,
		CopyResource,
		ResolveSubresource,
		ResolveSubresourceRegion,
		DrawInstanced,
		DrawIndexedInstanced,
		Dispatch,
		DispatchMesh,
		ExecuteIndirect,
		DispatchRays,
		SetViewports,
		SetScissorRects,
		SetShadingRate,
		SetShadingRateImage,
		SetViewInstanceMask,
		SetRenderTargets,
		SetBlendFactor,
		SetStencilRef,
		SetDepthBounds,
		BeginRenderPass,
		EndRenderPass,
		ClearRTView,
		ClearDSView,
		ClearUAViewUint,
		ClearUAViewFloat,
		SetPipelineState,
		SetPipelineState1,
		SetGraphicsRS,
		SetComputeRS,
		SetPrimitiveTopology,
		SetVertexBuffers,
		SetIndexBuffer,
		ResourceBarrier,
		SetDescriptorHeaps,
		SetGraphicsRootDescriptorTable,
		SetComputeRootDescriptorTable,
		SetGraphicsRoot32BitConstant,
		SetComputeRoot32BitConstant,
		SetGraphicsRoot32BitConstants,
		SetComputeRoot32BitConstants,
		SetGraphicsRootConstantBufferView,
		SetComputeRootConstantBufferView,
		SetGraphicsRootShaderResourceView,
		SetComputeRootShaderResourceView,
		SetGraphicsRootUnorderedAccessView,
		SetComputeRootUnorderedAccessView,
		List_Close,
		List_Reset,
		ClearState,
		BeginEvent,
		EndEvent,
		ListCommand_End = EndEvent,

		FenceCommand_Begin,
		GetCompletedValue = FenceCommand_Begin,
		SetEventOnCompletion,
		Fence_Signal,
		FenceCommand_End = Fence_Signal,

		ResourceCommand_Begin,
		Resource_SetName = ResourceCommand_Begin,
		Map,
		Unmap,
		ResourceCommand_End = Unmap
	};

	struct CommandVisitor;

	class CommandRecorder;
	class Command : public IListNode<Command>
	{
		friend class CommandRecorder;
	public:
		Command() = default;
		virtual ~Command() = default;

		virtual std::string GetDesc() const { return "Unknown Command"; }
		virtual CommandType GetType() const { return CommandType::Invalid; }
		virtual CommandCategory GetCategory() const { return CommandCategory::Invalid; }
		virtual Bool IsCPUTimeline() const { return false; }

		virtual void Accept(CommandVisitor& v) const {}
		virtual Uint64 GetHash() const { return 0; }

		ObjectID GetParentId() const { return commandParentId; }
		Uint64   GetCommandIndex() const { return commandIndex; }

		static Bool ClassOf(Command const* C)
		{
			return true;
		}

	private:
		ObjectID commandParentId = InvalidObjectID;
		Uint64 commandIndex = UINT64_MAX;
	};

	template<>
	struct IListTraits<Command> : public IListDefaultTraits<Command>
	{
		static Command* CreateNode(Command const&)
		{
			VISTA_ASSERT_MSG(false, "Should not be called ever!");
			return nullptr;
		}

		static void DeleteNode(Command* V)
		{
		}
	};

	#define COMMAND_CLASS_OF_RANGE(CommandClass) \
	static Bool ClassOf(Command const* C) \
	{ \
		using enum CommandType; \
		return C->GetType() >= CommandClass##_Begin && C->GetType() <= CommandClass##_End; \
	} 

	#define COMMAND_ACCEPT_IMPL() \
	virtual void Accept(CommandVisitor& v) const override \
	{ \
		v.Visit(*this); \
	}

	#define COMMAND_HASH_IMPL_WITH_PARENT_ID(...) \
	virtual Uint64 GetHash() const override \
	{ \
		HashState hashState{}; \
		hashState.Combine(GetParentId()); \
		hashState.Combine((Uint64)GetType()); \
		hashState.Combine(__VA_ARGS__); \
		return hashState; \
	} 

	#define COMMAND_HASH_IMPL(...) \
	virtual Uint64 GetHash() const override \
	{ \
		HashState hashState{}; \
		hashState.Combine((Uint64)GetType()); \
		hashState.Combine(__VA_ARGS__); \
		return hashState; \
	} 
}