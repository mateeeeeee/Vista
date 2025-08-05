#pragma once
#include "Tracking/ObjectTraits.h"

namespace vista
{
	struct ShaderBytecodeStorage
	{
		ShaderBytecodeStorage() = default;
		ShaderBytecodeStorage(D3D12_SHADER_BYTECODE const& desc)
		{
			if (desc.pShaderBytecode && desc.BytecodeLength > 0) 
			{
				Bytecode.assign(
					static_cast<BYTE const*>(desc.pShaderBytecode),
					static_cast<BYTE const*>(desc.pShaderBytecode) + desc.BytecodeLength
				);
			}
		}

		void const* GetBuffer() const { return Bytecode.data(); }
		Uint64 GetSize() const { return Bytecode.size(); }
		Bool IsValid() const { return !Bytecode.empty(); }

		D3D12_SHADER_BYTECODE GetD3D12Bytecode() const 
		{
			return { Bytecode.empty() ? nullptr : Bytecode.data(), Bytecode.size() };
		}

		std::vector<BYTE> Bytecode;
	};

	struct InputElementDescStorage
	{
		InputElementDescStorage() = default;
		InputElementDescStorage(D3D12_INPUT_ELEMENT_DESC const& desc) :
			SemanticName(desc.SemanticName ? desc.SemanticName : ""),
			SemanticIndex(desc.SemanticIndex),
			Format(desc.Format),
			InputSlot(desc.InputSlot),
			AlignedByteOffset(desc.AlignedByteOffset),
			InputSlotClass(desc.InputSlotClass),
			InstanceDataStepRate(desc.InstanceDataStepRate)
		{
		}

		std::string SemanticName;
		UINT SemanticIndex = 0;
		DXGI_FORMAT Format = DXGI_FORMAT_UNKNOWN;
		UINT InputSlot = 0;
		UINT AlignedByteOffset = 0;
		D3D12_INPUT_CLASSIFICATION InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		UINT InstanceDataStepRate = 0;
	};

	struct SODeclarationEntryStorage
	{
		SODeclarationEntryStorage() = default;
		SODeclarationEntryStorage(D3D12_SO_DECLARATION_ENTRY const& desc) :
			Stream(desc.Stream),
			SemanticName(desc.SemanticName ? desc.SemanticName : ""), 
			SemanticIndex(desc.SemanticIndex),
			StartComponent(desc.StartComponent),
			ComponentCount(desc.ComponentCount),
			OutputSlot(desc.OutputSlot)
		{
		}

		UINT Stream = 0;
		std::string SemanticName;
		UINT SemanticIndex = 0;
		BYTE StartComponent = 0;
		BYTE ComponentCount = 0;
		BYTE OutputSlot = 0;
	};

	struct StreamOutputDescStorage
	{
		StreamOutputDescStorage() = default;
		StreamOutputDescStorage(D3D12_STREAM_OUTPUT_DESC const& desc) :
			RasterizedStream(desc.RasterizedStream)
		{
			if (desc.pSODeclaration && desc.NumEntries > 0) 
			{
				SODeclaration.reserve(desc.NumEntries);
				for (Uint32 i = 0; i < desc.NumEntries; ++i) 
				{
					SODeclaration.emplace_back(desc.pSODeclaration[i]);
				}
			}
			if (desc.pBufferStrides && desc.NumStrides > 0) 
			{
				BufferStrides.assign(desc.pBufferStrides, desc.pBufferStrides + desc.NumStrides);
			}
		}

		std::vector<SODeclarationEntryStorage> SODeclaration;
		std::vector<UINT> BufferStrides;
		UINT RasterizedStream = 0;
	};

	struct CachedPSOStorage
	{

		CachedPSOStorage() = default;
		CachedPSOStorage(D3D12_CACHED_PIPELINE_STATE const& desc)
		{
			if (desc.pCachedBlob && desc.CachedBlobSizeInBytes > 0) 
			{
				Blob.assign(
					static_cast<BYTE const*>(desc.pCachedBlob),
					static_cast<BYTE const*>(desc.pCachedBlob) + desc.CachedBlobSizeInBytes
				);
			}
		}

		Bool IsValid() const { return !Blob.empty(); }

		D3D12_CACHED_PIPELINE_STATE GetBlob() const 
		{
			return { Blob.empty() ? nullptr : Blob.data(), Blob.size() };
		}

		std::vector<BYTE> Blob;
	};

	class ObjectTracker;

	struct GraphicsPSODescStorage
	{
		GraphicsPSODescStorage() = default;
		GraphicsPSODescStorage(D3D12_GRAPHICS_PIPELINE_STATE_DESC const& desc, ObjectTracker& objManager);

		ObjectID RootSignatureId = InvalidObjectID;
		ShaderBytecodeStorage VS;
		ShaderBytecodeStorage PS;
		ShaderBytecodeStorage DS;
		ShaderBytecodeStorage HS;
		ShaderBytecodeStorage GS;
		StreamOutputDescStorage StreamOutput;
		D3D12_BLEND_DESC BlendState{}; 
		UINT SampleMask = 0xFFFFFFFF;
		D3D12_RASTERIZER_DESC RasterizerState{}; 
		D3D12_DEPTH_STENCIL_DESC DepthStencilState{};
		std::vector<InputElementDescStorage> InputLayoutElements;
		D3D12_INDEX_BUFFER_STRIP_CUT_VALUE IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
		UINT NumRenderTargets = 0;
		DXGI_FORMAT RTVFormats[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT]{}; 
		DXGI_FORMAT DSVFormat = DXGI_FORMAT_UNKNOWN;
		DXGI_SAMPLE_DESC SampleDesc{}; 
		UINT NodeMask = 0;
		CachedPSOStorage CachedPSO;
		D3D12_PIPELINE_STATE_FLAGS Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	};

	struct ComputePSODescStorage
	{
		ComputePSODescStorage() = default;
		ComputePSODescStorage(D3D12_COMPUTE_PIPELINE_STATE_DESC const& desc, ObjectTracker& objManager);

		ObjectID RootSignatureId = InvalidObjectID;
		ShaderBytecodeStorage CS;
		UINT NodeMask = 0;
		CachedPSOStorage CachedPSO;
		D3D12_PIPELINE_STATE_FLAGS Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	};

	struct StreamPSODescStorage
	{
		StreamPSODescStorage() = default;

		StreamPSODescStorage(D3D12_PIPELINE_STATE_STREAM_DESC const& desc, ObjectTracker& tracker);

		void ParseStream(ObjectTracker& tracker);

		static Uint64 GetSubobjectSize(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type)
		{
			switch (type)
			{
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE:
				return sizeof(ID3D12RootSignature*);

			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS:
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS:
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DS:
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_HS:
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_GS:
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CS:
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_AS:  
			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_MS:  
				return sizeof(D3D12_SHADER_BYTECODE);

			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_STREAM_OUTPUT:
				return sizeof(D3D12_STREAM_OUTPUT_DESC);

			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND:
				return sizeof(D3D12_BLEND_DESC);

			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_MASK:
				return sizeof(UINT);

			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER:
				return sizeof(D3D12_RASTERIZER_DESC);

			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL:
				return sizeof(D3D12_DEPTH_STENCIL_DESC);

			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_INPUT_LAYOUT:
				return sizeof(D3D12_INPUT_LAYOUT_DESC);

			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_IB_STRIP_CUT_VALUE:
				return sizeof(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE);

			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY:
				return sizeof(D3D12_PRIMITIVE_TOPOLOGY_TYPE);

			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS:
				return sizeof(D3D12_RT_FORMAT_ARRAY);

			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT:
				return sizeof(DXGI_FORMAT);

			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC:
				return sizeof(DXGI_SAMPLE_DESC);

			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_NODE_MASK:
				return sizeof(UINT);

			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CACHED_PSO:
				return sizeof(D3D12_CACHED_PIPELINE_STATE);

			case D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_FLAGS:
				return sizeof(D3D12_PIPELINE_STATE_FLAGS);
			}
			return 0;
		}

		std::vector<BYTE> StreamBlob;
		ObjectID RootSignatureId = InvalidObjectID;
		ShaderBytecodeStorage VS, PS, CS, DS, HS, GS, AS, MS;
		StreamOutputDescStorage StreamOutput;
		D3D12_BLEND_DESC BlendState{};
		D3D12_RASTERIZER_DESC RasterizerState{};
		D3D12_DEPTH_STENCIL_DESC DepthStencilState{};
		std::vector<InputElementDescStorage> InputLayoutElements;
		D3D12_INDEX_BUFFER_STRIP_CUT_VALUE IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
		UINT NumRenderTargets = 0;
		DXGI_FORMAT RTVFormats[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
		DXGI_FORMAT DSVFormat = DXGI_FORMAT_UNKNOWN;
		DXGI_SAMPLE_DESC SampleDesc{};
		UINT SampleMask = 0xFFFFFFFF;
		UINT NodeMask = 0;
		CachedPSOStorage CachedPSO;
		D3D12_PIPELINE_STATE_FLAGS Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	};


	using PSODesc = std::variant<GraphicsPSODescStorage, ComputePSODescStorage, StreamPSODescStorage>;
	template<>
	struct ObjectTraits<ID3D12PipelineState>
	{
		using Desc = PSODesc;
		static constexpr ObjectType Type = ObjectType::PSO;
	};
}