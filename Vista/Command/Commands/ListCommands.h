#pragma once
#include "Command/Command.h"

namespace vista
{
	class ListCommand : public Command
	{
	public:
		virtual CommandCategory GetCategory() const override { return CommandCategory::List; }
		
		Bool IsDrawCommand() const
		{
			return GetType() == CommandType::DrawInstanced || GetType() == CommandType::DrawIndexedInstanced;
		}
		Bool IsDispatchCommand() const
		{
			return GetType() == CommandType::Dispatch;
		}
		Bool IsCopyCommand() const
		{
			return GetType() == CommandType::CopyResource || GetType() == CommandType::CopyBufferRegion || GetType() == CommandType::CopyTextureRegion;
		}
		Bool IsMeshCommand() const
		{
			return GetType() == CommandType::DispatchMesh;
		}
		Bool IsRayTracingCommand() const
		{
			return GetType() == CommandType::DispatchRays;
		}

		COMMAND_CLASS_OF_RANGE(ListCommand)

	protected:
		ListCommand() {}
	};

	class CopyBufferRegionCommand : public ListCommand
	{
	public:
		CopyBufferRegionCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::CopyBufferRegion; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(dstBufferId, dstOffset, srcBufferId, srcOffset, numBytes)

		static Bool ClassOf(Command const* C)
		{
			using enum CommandType;
			return C->GetType() == CopyBufferRegion;
		}

	public:
		ObjectID dstBufferId = InvalidObjectID;
		Uint64 dstOffset = 0;
		ObjectID srcBufferId = InvalidObjectID;
		Uint64 srcOffset = 0;
		Uint64 numBytes = 0;
	};

	class CopyTextureRegionCommand : public ListCommand
	{
	public:
		CopyTextureRegionCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::CopyTextureRegion; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(dstTextureId, dstX, dstY, dstZ, srcTextureId)

		static Bool ClassOf(Command const* C)
		{
			using enum CommandType;
			return C->GetType() == CopyTextureRegion;
		}

	public:
		ObjectID dstTextureId = InvalidObjectID;
		Uint32 dstX = 0;
		Uint32 dstY = 0;
		Uint32 dstZ = 0;
		ObjectID srcTextureId = InvalidObjectID;
		std::optional<D3D12_BOX> srcBox = std::nullopt;
	};

	class CopyResourceCommand : public ListCommand
	{
	public:
		CopyResourceCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::CopyResource; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(dstResourceId, srcResourceId)

		static Bool ClassOf(Command const* C)
		{
			using enum CommandType;
			return C->GetType() == CopyResource;
		}

	public:
		ObjectID dstResourceId = InvalidObjectID;
		ObjectID srcResourceId = InvalidObjectID;
	};

	class ResolveSubresourceCommand : public ListCommand
	{
	public:
		ResolveSubresourceCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::ResolveSubresource; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(dstResourceId, dstSubresource, srcResourceId, srcSubresource, format)

		static Bool ClassOf(Command const* C)
		{
			using enum CommandType;
			return C->GetType() == ResolveSubresource;
		}

	public:
		ObjectID dstResourceId = InvalidObjectID;
		Uint32 dstSubresource = 0;
		ObjectID srcResourceId = InvalidObjectID;
		Uint32 srcSubresource = 0;
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
	};

	class ResolveSubresourceRegionCommand : public ListCommand
	{
	public:
		ResolveSubresourceRegionCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::ResolveSubresourceRegion; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(dstResourceId, dstSubresource, dstX, dstY, srcResourceId, srcSubresource, format, resolveMode)

		static Bool ClassOf(Command const* C)
		{
			using enum CommandType;
			return C->GetType() == ResolveSubresourceRegion;
		}

	public:
		ObjectID dstResourceId = InvalidObjectID;
		Uint32 dstSubresource = 0;
		Uint32 dstX = 0;
		Uint32 dstY = 0;
		ObjectID srcResourceId = InvalidObjectID;
		Uint32 srcSubresource = 0;
		std::optional<D3D12_RECT> srcRect = std::nullopt;
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
		D3D12_RESOLVE_MODE resolveMode = D3D12_RESOLVE_MODE_DECOMPRESS;
	};

	class DrawInstancedCommand : public ListCommand
	{
	public:
		DrawInstancedCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::DrawInstanced; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation)

		static Bool ClassOf(Command const* C)
		{
			using enum CommandType;
			return C->GetType() == DrawInstanced;
		}

	public:
		Uint32 vertexCountPerInstance = 0;
		Uint32 instanceCount = 0;
		Uint32 startVertexLocation = 0;
		Uint32 startInstanceLocation = 0;
	};

	class DrawIndexedInstancedCommand : public ListCommand
	{
	public:
		DrawIndexedInstancedCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::DrawIndexedInstanced; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation)

		static Bool ClassOf(Command const* C)
		{
			using enum CommandType;
			return C->GetType() == DrawIndexedInstanced;
		}

	public:
		Uint32 indexCountPerInstance = 0;
		Uint32 instanceCount = 0;
		Uint32 startIndexLocation = 0;
		Int32  baseVertexLocation = 0;
		Uint32 startInstanceLocation = 0;
	};

	class DispatchCommand : public ListCommand
	{
	public:
		DispatchCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::Dispatch; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(threadGroupCountX, threadGroupCountY, threadGroupCountZ)

		static Bool ClassOf(Command const* C)
		{
			using enum CommandType;
			return C->GetType() == Dispatch;
		}

	public:
		Uint32 threadGroupCountX = 0;
		Uint32 threadGroupCountY = 0;
		Uint32 threadGroupCountZ = 0;
	};

	class DispatchMeshCommand : public ListCommand
	{
	public:
		DispatchMeshCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::DispatchMesh; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(threadGroupCountX, threadGroupCountY, threadGroupCountZ)

		static Bool ClassOf(Command const* C)
		{
			using enum CommandType;
			return C->GetType() == DispatchMesh;
		}

	public:
		Uint32 threadGroupCountX = 0;
		Uint32 threadGroupCountY = 0;
		Uint32 threadGroupCountZ = 0;
	};

	class ExecuteIndirectCommand : public ListCommand
	{
	public:
		ExecuteIndirectCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::ExecuteIndirect; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(commandSignatureId, maxCommandCount, argumentBufferId, argumentBufferOffset, countBufferId, countBufferOffset)

		static Bool ClassOf(Command const* C)
		{
			using enum CommandType;
			return C->GetType() == ExecuteIndirect;
		}

	public:
		ObjectID commandSignatureId = InvalidObjectID;
		Uint32   maxCommandCount = 0;
		ObjectID argumentBufferId = InvalidObjectID;
		Uint64 argumentBufferOffset = 0;
		ObjectID countBufferId = InvalidObjectID;
		Uint64 countBufferOffset = 0;
	};

	class DispatchRaysCommand : public ListCommand
	{
	public:
		DispatchRaysCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::DispatchRays; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(dispatchWidth, dispatchHeight, dispatchDepth)

		static Bool ClassOf(Command const* C)
		{
			using enum CommandType;
			return C->GetType() == DispatchRays;
		}

	public:
		Uint32 dispatchWidth = 0;
		Uint32 dispatchHeight = 0;
		Uint32 dispatchDepth = 0;
	};

	class RSSetViewportsCommand : public ListCommand
	{
	public:
		RSSetViewportsCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetViewports; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(viewports.size())

		static Bool ClassOf(Command const* C)
		{
			using enum CommandType;
			return C->GetType() == SetViewports;
		}

	public:
		std::vector<D3D12_VIEWPORT> viewports;
	};

	class RSSetScissorRectsCommand : public ListCommand
	{
	public:
		RSSetScissorRectsCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetScissorRects; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(scissorRects.size())

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::SetScissorRects;
		}

	public:
		std::vector<D3D12_RECT> scissorRects;
	};

	class RSSetShadingRateCommand : public ListCommand
	{
	public:
		RSSetShadingRateCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetShadingRate; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(shadingRate, shadingRateCombiners[0], shadingRateCombiners[1])

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::SetShadingRate;
		}

	public:
		D3D12_SHADING_RATE shadingRate = D3D12_SHADING_RATE_1X1;
		D3D12_SHADING_RATE_COMBINER shadingRateCombiners[2] = { D3D12_SHADING_RATE_COMBINER_PASSTHROUGH, D3D12_SHADING_RATE_COMBINER_PASSTHROUGH };
	};

	class RSSetShadingRateImageCommand : public ListCommand
	{
	public:
		RSSetShadingRateImageCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetShadingRateImage; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(shadingRateImageId)

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::SetShadingRateImage;
		}

	public:
		ObjectID shadingRateImageId = InvalidObjectID;
	};

	class SetViewInstanceMaskCommand : public ListCommand
	{
	public:
		SetViewInstanceMaskCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetViewInstanceMask; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(viewInstanceMask)

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::SetViewInstanceMask;
		}

	public:
		Uint32 viewInstanceMask = 0;
	};

	class OMSetRenderTargetsCommand : public ListCommand
	{
	public:
		OMSetRenderTargetsCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetRenderTargets; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(renderTargetDescriptors.size(), RTsSingleHandleToDescriptorRange, depthStencilDescriptor.ptr != NULL)

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::SetRenderTargets;
		}

	public:
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> renderTargetDescriptors;
		Bool RTsSingleHandleToDescriptorRange = false;
		D3D12_CPU_DESCRIPTOR_HANDLE depthStencilDescriptor = { NULL };
	};

	class OMSetBlendFactorCommand : public ListCommand
	{
	public:
		OMSetBlendFactorCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetBlendFactor; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(blendFactor[0], blendFactor[1], blendFactor[2], blendFactor[3])

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::SetBlendFactor;
		}

	public:
		std::array<Float, 4> blendFactor = {};
	};

	class OMSetStencilRefCommand : public ListCommand
	{
	public:
		OMSetStencilRefCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetStencilRef; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(stencilRef)

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::SetStencilRef;
		}

	public:
		Uint32 stencilRef = 0;
	};

	class OMSetDepthBoundsCommand : public ListCommand
	{
	public:
		OMSetDepthBoundsCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetDepthBounds; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(depthMin, depthMax)

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::SetDepthBounds;
		}

	public:
		Float depthMin = 0.0f;
		Float depthMax = 0.0f;
	};

	class BeginRenderPassCommand : public ListCommand
	{
	public:
		BeginRenderPassCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::BeginRenderPass; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(flags, renderTargets.size(), depthStencil.has_value())

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::BeginRenderPass;
		}

	public:
		std::vector<D3D12_RENDER_PASS_RENDER_TARGET_DESC> renderTargets;
		std::optional<D3D12_RENDER_PASS_DEPTH_STENCIL_DESC> depthStencil;
		D3D12_RENDER_PASS_FLAGS flags;
	};

	class EndRenderPassCommand : public ListCommand
	{
	public:
		EndRenderPassCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::EndRenderPass; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(0)

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::EndRenderPass;
		}
	};

	class ClearRenderTargetViewCommand : public ListCommand
	{
	public:
		ClearRenderTargetViewCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::ClearRTView; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(renderTargetView.ptr != NULL, colorRGBA[0], colorRGBA[1], colorRGBA[2], colorRGBA[3], rects.size())

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::ClearRTView;
		}

	public:
		D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView = { NULL };
		Float colorRGBA[4] = {};
		std::vector<D3D12_RECT> rects;
	};

	class ClearDepthStencilViewCommand : public ListCommand
	{
	public:
		ClearDepthStencilViewCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::ClearDSView; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(depthStencilView.ptr != NULL, clearFlags, depth, stencil, rects.size())

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::ClearDSView;
		}

	public:
		D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = { NULL };
		D3D12_CLEAR_FLAGS clearFlags = D3D12_CLEAR_FLAG_DEPTH;
		Float depth = 0.0f;
		Uint8 stencil = 0;
		std::vector<D3D12_RECT> rects;
	};

	class ClearUnorderedAccessViewUintCommand : public ListCommand
	{
	public:
		ClearUnorderedAccessViewUintCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::ClearUAViewUint; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(resourceId, values[0], values[1], values[2], values[3], rects.size())

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::ClearUAViewUint;
		}

	public:
		D3D12_GPU_DESCRIPTOR_HANDLE gpuUnorderedAccessView = { 0 };
		D3D12_CPU_DESCRIPTOR_HANDLE cpuUnorderedAccessView = { 0 };
		ObjectID resourceId = InvalidObjectID;
		std::array<Uint32, 4> values = {};
		std::vector<D3D12_RECT> rects;
	};

	class ClearUnorderedAccessViewFloatCommand : public ListCommand
	{
	public:
		ClearUnorderedAccessViewFloatCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::ClearUAViewFloat; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(resourceId, values[0], values[1], values[2], values[3], rects.size())

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::ClearUAViewFloat;
		}

	public:
		D3D12_GPU_DESCRIPTOR_HANDLE gpuUnorderedAccessView = { 0 };
		D3D12_CPU_DESCRIPTOR_HANDLE cpuUnorderedAccessView = { 0 };
		ObjectID resourceId = InvalidObjectID;
		std::array<Float, 4> values = {};
		std::vector<D3D12_RECT> rects;
	};

	class SetPipelineStateCommand : public ListCommand
	{
	public:
		SetPipelineStateCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetPipelineState; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(psoId)

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::SetPipelineState;
		}

	public:
		ObjectID psoId = InvalidObjectID;
	};

	class SetPipelineState1Command : public ListCommand
	{
	public:
		SetPipelineState1Command() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetPipelineState1; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(stateObjectId)

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::SetPipelineState1;
		}

	public:
		ObjectID stateObjectId = InvalidObjectID;
	};

	class SetGraphicsRootSignatureCommand : public ListCommand
	{
	public:
		SetGraphicsRootSignatureCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetGraphicsRS; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(rootSignatureId)

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::SetGraphicsRS;
		}

	public:
		ObjectID rootSignatureId = InvalidObjectID;
	};

	class SetComputeRootSignatureCommand : public ListCommand
	{
	public:
		SetComputeRootSignatureCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetComputeRS; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(rootSignatureId)

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::SetComputeRS;
		}

	public:
		ObjectID rootSignatureId = InvalidObjectID;
	};

	class IASetPrimitiveTopologyCommand : public ListCommand
	{
	public:
		IASetPrimitiveTopologyCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetPrimitiveTopology; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL((Int)topology)
		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::SetPrimitiveTopology;
		}

	public:
		D3D12_PRIMITIVE_TOPOLOGY topology;
	};

	class IASetVertexBuffersCommand : public ListCommand
	{
	public:
		IASetVertexBuffersCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetVertexBuffers; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(startSlot, vertexBufferViews.size())

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::SetVertexBuffers;
		}

	public:
		Uint32 startSlot = 0;
		std::vector<D3D12_VERTEX_BUFFER_VIEW> vertexBufferViews;
	};

	class IASetIndexBufferCommand : public ListCommand
	{
	public:
		IASetIndexBufferCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetIndexBuffer; }

		COMMAND_ACCEPT_IMPL()

		virtual Uint64 GetHash() const override 
		{
			HashState hashState{}; hashState.Combine((Uint64)GetType()); 
			if (indexBufferView)
			{
				hashState.Combine(true);
				hashState.Combine(indexBufferView->BufferLocation);
				hashState.Combine((Int)indexBufferView->Format);
				hashState.Combine((Int)indexBufferView->SizeInBytes);
			}
			else
			{
				hashState.Combine(false);
				hashState.Combine(0);
				hashState.Combine(0);
				hashState.Combine(0);
			}
			return hashState;
		}

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::SetIndexBuffer;
		}

	public:
		std::optional<D3D12_INDEX_BUFFER_VIEW> indexBufferView = std::nullopt;
	};

	class ResourceBarrierCommand : public ListCommand
	{
	public:
		ResourceBarrierCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::ResourceBarrier; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(barriers.size())

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::ResourceBarrier;
		}

	public:
		std::vector<D3D12_RESOURCE_BARRIER> barriers;
	};

	class SetDescriptorHeapsCommand : public ListCommand
	{
	public:
		SetDescriptorHeapsCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetDescriptorHeaps; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(descriptorHeapIds[0], descriptorHeapIds[1])

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::SetDescriptorHeaps;
		}

	public:
		ObjectID descriptorHeapIds[2] = {};
		Uint32 numDescriptorHeaps = 0;
	};

	class SetGraphicsRootDescriptorTableCommand : public ListCommand
	{
	public:
		SetGraphicsRootDescriptorTableCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetGraphicsRootDescriptorTable; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(rootParameterIndex)

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::SetGraphicsRootDescriptorTable;
		}

	public:
		Uint32 rootParameterIndex = 0;
		D3D12_GPU_DESCRIPTOR_HANDLE baseDescriptor = { 0 };
	};

	class SetComputeRootDescriptorTableCommand : public ListCommand
	{
	public:
		SetComputeRootDescriptorTableCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetComputeRootDescriptorTable; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(rootParameterIndex)

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::SetComputeRootDescriptorTable;
		}

	public:
		Uint32 rootParameterIndex = 0;
		D3D12_GPU_DESCRIPTOR_HANDLE baseDescriptor = { 0 };
	};

	class SetGraphicsRoot32BitConstantCommand : public ListCommand
	{
	public:
		SetGraphicsRoot32BitConstantCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetGraphicsRoot32BitConstant; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(rootParameterIndex, destOffsetIn32BitValues)

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::SetGraphicsRoot32BitConstant;
		}

	public:
		Uint32 rootParameterIndex = 0;
		Uint32 srcData = 0;
		Uint32 destOffsetIn32BitValues = 0;
	};

	class SetComputeRoot32BitConstantCommand : public ListCommand
	{
	public:
		SetComputeRoot32BitConstantCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetComputeRoot32BitConstant; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(rootParameterIndex, destOffsetIn32BitValues)

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::SetComputeRoot32BitConstant;
		}

	public:
		Uint32 rootParameterIndex = 0;
		Uint32 srcData = 0;
		Uint32 destOffsetIn32BitValues = 0;
	};

	class SetGraphicsRoot32BitConstantsCommand : public ListCommand
	{
	public:
		SetGraphicsRoot32BitConstantsCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetGraphicsRoot32BitConstants; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(rootParameterIndex, destOffsetIn32BitValues)

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::SetGraphicsRoot32BitConstants;
		}

	public:
		Uint32 rootParameterIndex = 0;
		std::vector<Uint32> srcData;
		Uint32 destOffsetIn32BitValues = 0;
	};

	class SetComputeRoot32BitConstantsCommand : public ListCommand
	{
	public:
		SetComputeRoot32BitConstantsCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetComputeRoot32BitConstants; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(rootParameterIndex, destOffsetIn32BitValues)

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::SetComputeRoot32BitConstants;
		}

	public:
		Uint32 rootParameterIndex = 0;
		std::vector<Uint32> srcData;
		Uint32 destOffsetIn32BitValues = 0;
	};

	class SetGraphicsRootConstantBufferViewCommand : public ListCommand
	{
	public:
		SetGraphicsRootConstantBufferViewCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetGraphicsRootConstantBufferView; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(rootParameterIndex)

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::SetGraphicsRootConstantBufferView;
		}

	public:
		Uint32 rootParameterIndex = 0;
		D3D12_GPU_VIRTUAL_ADDRESS gpuVA = { 0 };
	};

	class SetComputeRootConstantBufferViewCommand : public ListCommand
	{
	public:
		SetComputeRootConstantBufferViewCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetComputeRootConstantBufferView; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(rootParameterIndex)

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::SetComputeRootConstantBufferView;
		}

	public:
		Uint32 rootParameterIndex = 0;
		D3D12_GPU_VIRTUAL_ADDRESS gpuVA = { 0 };
	};

	class SetGraphicsRootShaderResourceViewCommand : public ListCommand
	{
	public:
		SetGraphicsRootShaderResourceViewCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetGraphicsRootShaderResourceView; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(rootParameterIndex)

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::SetGraphicsRootShaderResourceView;
		}

	public:
		Uint32 rootParameterIndex = 0;
		D3D12_GPU_VIRTUAL_ADDRESS gpuVA = { 0 };
	};

	class SetComputeRootShaderResourceViewCommand : public ListCommand
	{
	public:
		SetComputeRootShaderResourceViewCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetComputeRootShaderResourceView; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(rootParameterIndex)

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::SetComputeRootShaderResourceView;
		}

	public:
		Uint32 rootParameterIndex = 0;
		D3D12_GPU_VIRTUAL_ADDRESS gpuVA = { 0 };
	};

	class SetGraphicsRootUnorderedAccessViewCommand : public ListCommand
	{
	public:
		SetGraphicsRootUnorderedAccessViewCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetGraphicsRootUnorderedAccessView; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(rootParameterIndex)

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::SetGraphicsRootUnorderedAccessView;
		}

	public:
		Uint32 rootParameterIndex = 0;
		D3D12_GPU_VIRTUAL_ADDRESS gpuVA = { 0 };
	};

	class SetComputeRootUnorderedAccessViewCommand : public ListCommand
	{
	public:
		SetComputeRootUnorderedAccessViewCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetComputeRootUnorderedAccessView; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(rootParameterIndex)

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::SetComputeRootUnorderedAccessView;
		}

	public:
		Uint32 rootParameterIndex = 0;
		D3D12_GPU_VIRTUAL_ADDRESS gpuVA = { 0 };
	};

	class BeginEventCommand : public ListCommand
	{
	public:
		BeginEventCommand() = default;

		virtual std::string GetDesc() const override { return "BeginEvent"; }
		virtual CommandType GetType() const override { return CommandType::BeginEvent; }

		COMMAND_ACCEPT_IMPL()
			COMMAND_HASH_IMPL(eventName, eventColor)

			static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::BeginEvent;
		}

	public:
		std::string eventName;
		Uint64 eventColor = 0xffffffff;
	};

	class EndEventCommand : public ListCommand
	{
	public:
		EndEventCommand() = default;

		virtual std::string GetDesc() const override { return "EndEvent"; }
		virtual CommandType GetType() const override { return CommandType::EndEvent; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(0)

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::EndEvent;
		}
	};

	class CloseListCommand : public ListCommand
	{
	public:
		CloseListCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::List_Close; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(0)

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::List_Close;
		}

	public:
		HRESULT hr = S_OK;
	};

	class ResetListCommand : public ListCommand
	{
	public:
		ResetListCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::List_Reset; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(psoId)

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::List_Reset;
		}

	public:
		HRESULT hr = S_OK;
		ObjectID allocatorId = InvalidObjectID;
		ObjectID psoId = InvalidObjectID;
	};

	class ClearStateCommand : public ListCommand
	{
	public:
		ClearStateCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::ClearState; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(psoId)

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::ClearState;
		}

	public:
		ObjectID psoId = InvalidObjectID;
	};

}