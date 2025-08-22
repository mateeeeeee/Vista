#pragma once
#include <d3dcommon.h>
#include "Utilities/StringUtil.h"

namespace vista
{
	inline Char const* D3D12HeapTypeToString(D3D12_HEAP_TYPE type)
	{
		switch (type)
		{
		case D3D12_HEAP_TYPE_DEFAULT:     return "DEFAULT";
		case D3D12_HEAP_TYPE_UPLOAD:      return "UPLOAD";
		case D3D12_HEAP_TYPE_READBACK:    return "READBACK";
		case D3D12_HEAP_TYPE_CUSTOM:      return "CUSTOM";
		default:                          return "UNKNOWN";
		}
	}
	inline Char const* D3D12ResourceDimensionToString(D3D12_RESOURCE_DIMENSION dim)
	{
		switch (dim)
		{
		case D3D12_RESOURCE_DIMENSION_BUFFER:   return "BUFFER";
		case D3D12_RESOURCE_DIMENSION_TEXTURE1D:return "TEXTURE1D";
		case D3D12_RESOURCE_DIMENSION_TEXTURE2D:return "TEXTURE2D";
		case D3D12_RESOURCE_DIMENSION_TEXTURE3D:return "TEXTURE3D";
		case D3D12_RESOURCE_DIMENSION_UNKNOWN:  return "UNKNOWN";
		default:                                return "INVALID";
		}
	}
	inline Char const* D3D12CommandListTypeToString(D3D12_COMMAND_LIST_TYPE type)
	{
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:		return "DIRECT";
		case D3D12_COMMAND_LIST_TYPE_BUNDLE:		return "BUNDLE";
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:		return "COMPUTE";
		case D3D12_COMMAND_LIST_TYPE_COPY:			return "COPY";
		case D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE:	return "VIDEO_DECODE";
		case D3D12_COMMAND_LIST_TYPE_VIDEO_PROCESS: return "VIDEO_PROCESS";
		case D3D12_COMMAND_LIST_TYPE_VIDEO_ENCODE:	return "VIDEO_ENCODE";
		default:									return "UNKNOWN";
		}
	}
	inline Char const* D3D12DescriptorHeapTypeToString(D3D12_DESCRIPTOR_HEAP_TYPE type)
	{
		switch (type)
		{
		case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV: return "CBV_SRV_UAV";
		case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:     return "SAMPLER";
		case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:         return "RTV";
		case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:         return "DSV";
		default:                                     return "UNKNOWN";
		}
	}
	inline Char const* DXGIFormatToString(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_UNKNOWN: return "UNKNOWN";
		case DXGI_FORMAT_R32G32B32A32_FLOAT: return "R32G32B32A32_FLOAT";
		case DXGI_FORMAT_R32G32B32A32_UINT: return "R32G32B32A32_UINT";
		case DXGI_FORMAT_R32G32B32A32_SINT: return "R32G32B32A32_SINT";
		case DXGI_FORMAT_R32G32B32_FLOAT: return "R32G32B32_FLOAT";
		case DXGI_FORMAT_R32G32B32_UINT: return "R32G32B32_UINT";
		case DXGI_FORMAT_R32G32B32_SINT: return "R32G32B32_SINT";
		case DXGI_FORMAT_R16G16B16A16_FLOAT: return "R16G16B16A16_FLOAT";
		case DXGI_FORMAT_R16G16B16A16_UNORM: return "R16G16B16A16_UNORM";
		case DXGI_FORMAT_R16G16B16A16_UINT: return "R16G16B16A16_UINT";
		case DXGI_FORMAT_R16G16B16A16_SNORM: return "R16G16B16A16_SNORM";
		case DXGI_FORMAT_R16G16B16A16_SINT: return "R16G16B16A16_SINT";
		case DXGI_FORMAT_R32G32_FLOAT: return "R32G32_FLOAT";
		case DXGI_FORMAT_R32G32_UINT: return "R32G32_UINT";
		case DXGI_FORMAT_R32G32_SINT: return "R32G32_SINT";
		case DXGI_FORMAT_R32G8X24_TYPELESS: return "R32G8X24_TYPELESS";
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT: return "D32_FLOAT_S8X24_UINT";
		case DXGI_FORMAT_R10G10B10A2_UNORM: return "R10G10B10A2_UNORM";
		case DXGI_FORMAT_R10G10B10A2_UINT: return "R10G10B10A2_UINT";
		case DXGI_FORMAT_R11G11B10_FLOAT: return "R11G11B10_FLOAT";
		case DXGI_FORMAT_R8G8B8A8_UNORM: return "R8G8B8A8_UNORM";
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: return "R8G8B8A8_UNORM_SRGB";
		case DXGI_FORMAT_R8G8B8A8_UINT: return "R8G8B8A8_UINT";
		case DXGI_FORMAT_R8G8B8A8_SNORM: return "R8G8B8A8_SNORM";
		case DXGI_FORMAT_R8G8B8A8_SINT: return "R8G8B8A8_SINT";
		case DXGI_FORMAT_B8G8R8A8_UNORM: return "B8G8R8A8_UNORM";
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB: return "B8G8R8A8_UNORM_SRGB";
		case DXGI_FORMAT_R16G16_FLOAT: return "R16G16_FLOAT";
		case DXGI_FORMAT_R16G16_UNORM: return "R16G16_UNORM";
		case DXGI_FORMAT_R16G16_UINT: return "R16G16_UINT";
		case DXGI_FORMAT_R16G16_SNORM: return "R16G16_SNORM";
		case DXGI_FORMAT_R16G16_SINT: return "R16G16_SINT";
		case DXGI_FORMAT_R32_TYPELESS: return "R32_TYPELESS";
		case DXGI_FORMAT_D32_FLOAT: return "D32_FLOAT";
		case DXGI_FORMAT_R32_FLOAT: return "R32_FLOAT";
		case DXGI_FORMAT_R32_UINT: return "R32_UINT";
		case DXGI_FORMAT_R32_SINT: return "R32_SINT";
		case DXGI_FORMAT_R24G8_TYPELESS: return "R24G8_TYPELESS";
		case DXGI_FORMAT_D24_UNORM_S8_UINT: return "D24_UNORM_S8_UINT";
		case DXGI_FORMAT_R8G8_UNORM: return "R8G8_UNORM";
		case DXGI_FORMAT_R8G8_UINT: return "R8G8_UINT";
		case DXGI_FORMAT_R8G8_SNORM: return "R8G8_SNORM";
		case DXGI_FORMAT_R8G8_SINT: return "R8G8_SINT";
		case DXGI_FORMAT_R16_TYPELESS: return "R16_TYPELESS";
		case DXGI_FORMAT_R16_FLOAT: return "R16_FLOAT";
		case DXGI_FORMAT_D16_UNORM: return "D16_UNORM";
		case DXGI_FORMAT_R16_UNORM: return "R16_UNORM";
		case DXGI_FORMAT_R16_UINT: return "R16_UINT";
		case DXGI_FORMAT_R16_SNORM: return "R16_SNORM";
		case DXGI_FORMAT_R16_SINT: return "R16_SINT";
		case DXGI_FORMAT_R8_UNORM: return "R8_UNORM";
		case DXGI_FORMAT_R8_UINT: return "R8_UINT";
		case DXGI_FORMAT_R8_SNORM: return "R8_SNORM";
		case DXGI_FORMAT_R8_SINT: return "R8_SINT";
		case DXGI_FORMAT_BC1_UNORM: return "BC1_UNORM";
		case DXGI_FORMAT_BC1_UNORM_SRGB: return "BC1_UNORM_SRGB";
		case DXGI_FORMAT_BC2_UNORM: return "BC2_UNORM";
		case DXGI_FORMAT_BC2_UNORM_SRGB: return "BC2_UNORM_SRGB";
		case DXGI_FORMAT_BC3_UNORM: return "BC3_UNORM";
		case DXGI_FORMAT_BC3_UNORM_SRGB: return "BC3_UNORM_SRGB";
		case DXGI_FORMAT_BC4_UNORM: return "BC4_UNORM";
		case DXGI_FORMAT_BC4_SNORM: return "BC4_SNORM";
		case DXGI_FORMAT_BC5_UNORM: return "BC5_UNORM";
		case DXGI_FORMAT_BC5_SNORM: return "BC5_SNORM";
		case DXGI_FORMAT_BC6H_UF16: return "BC6H_UF16";
		case DXGI_FORMAT_BC6H_SF16: return "BC6H_SF16";
		case DXGI_FORMAT_BC7_UNORM: return "BC7_UNORM";
		case DXGI_FORMAT_BC7_UNORM_SRGB: return "BC7_UNORM_SRGB";
		case DXGI_FORMAT_R9G9B9E5_SHAREDEXP: return "R9G9B9E5_SHAREDEXP";
		default: return "INVALID";
		}
	}
	inline std::string D3D12ResourceStateToString(D3D12_RESOURCE_STATES states)
	{
		std::string result;
		auto AppendState = [&](const char* name)
			{
				if (!result.empty())
					result += " | ";
				result += name;
			};

		if (states == D3D12_RESOURCE_STATE_COMMON)                    return "COMMON";
		if (states == D3D12_RESOURCE_STATE_GENERIC_READ)              return "GENERIC_READ";
		if (states == D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE)       return "ALL_SHADER_RESOURCE";

		if (states & D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER) AppendState("VERTEX_AND_CONSTANT_BUFFER");
		if (states & D3D12_RESOURCE_STATE_INDEX_BUFFER)               AppendState("INDEX_BUFFER");
		if (states & D3D12_RESOURCE_STATE_RENDER_TARGET)              AppendState("RENDER_TARGET");
		if (states & D3D12_RESOURCE_STATE_UNORDERED_ACCESS)           AppendState("UNORDERED_ACCESS");
		if (states & D3D12_RESOURCE_STATE_DEPTH_WRITE)                AppendState("DEPTH_WRITE");
		if (states & D3D12_RESOURCE_STATE_DEPTH_READ)                 AppendState("DEPTH_READ");
		if (states & D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)  AppendState("NON_PIXEL_SHADER_RESOURCE");
		if (states & D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)      AppendState("PIXEL_SHADER_RESOURCE");
		if (states & D3D12_RESOURCE_STATE_STREAM_OUT)                 AppendState("STREAM_OUT");
		if (states & D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT)          AppendState("INDIRECT_ARGUMENT");
		if (states & D3D12_RESOURCE_STATE_COPY_DEST)                  AppendState("COPY_DEST");
		if (states & D3D12_RESOURCE_STATE_COPY_SOURCE)                AppendState("COPY_SOURCE");
		if (states & D3D12_RESOURCE_STATE_RESOLVE_DEST)               AppendState("RESOLVE_DEST");
		if (states & D3D12_RESOURCE_STATE_RESOLVE_SOURCE)             AppendState("RESOLVE_SOURCE");
		if (states & D3D12_RESOURCE_STATE_GENERIC_READ)               AppendState("GENERIC_READ");
		if (states & D3D12_RESOURCE_STATE_PREDICATION)                AppendState("PREDICATION");
		if (states & D3D12_RESOURCE_STATE_VIDEO_DECODE_READ)          AppendState("VIDEO_DECODE_READ");
		if (states & D3D12_RESOURCE_STATE_VIDEO_DECODE_WRITE)         AppendState("VIDEO_DECODE_WRITE");
		if (states & D3D12_RESOURCE_STATE_VIDEO_PROCESS_READ)         AppendState("VIDEO_PROCESS_READ");
		if (states & D3D12_RESOURCE_STATE_VIDEO_PROCESS_WRITE)        AppendState("VIDEO_PROCESS_WRITE");
		if (states & D3D12_RESOURCE_STATE_VIDEO_ENCODE_READ)          AppendState("VIDEO_ENCODE_READ");
		if (states & D3D12_RESOURCE_STATE_VIDEO_ENCODE_WRITE)         AppendState("VIDEO_ENCODE_WRITE");
		return result.empty() ? "UNKNOWN" : result.c_str();
	}
	inline Char const* D3D12RootParameterTypeToString(D3D12_ROOT_PARAMETER_TYPE type)
	{
		switch (type)
		{
		case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE: return "Descriptor Table";
		case D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS:   return "32-bit Constants";
		case D3D12_ROOT_PARAMETER_TYPE_CBV:                return "CBV";
		case D3D12_ROOT_PARAMETER_TYPE_SRV:                return "SRV";
		case D3D12_ROOT_PARAMETER_TYPE_UAV:                return "UAV";
		default: return "Unknown Root Parameter Type";
		}
	}
	inline std::string D3D12RootSignatureFlagsToString(D3D12_ROOT_SIGNATURE_FLAGS flags)
	{
		if (flags == D3D12_ROOT_SIGNATURE_FLAG_NONE)
			return "NONE";

		std::string result;
		auto AppendFlag = [&](const char* name)
			{
				if (!result.empty())
					result += " | ";
				result += name;
			};

		if (flags & D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT) AppendFlag("ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT");
		if (flags & D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS)      AppendFlag("DENY_VERTEX_SHADER_ROOT_ACCESS");
		if (flags & D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS)         AppendFlag("DENY_HULL_SHADER_ROOT_ACCESS");
		if (flags & D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS)       AppendFlag("DENY_DOMAIN_SHADER_ROOT_ACCESS");
		if (flags & D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS)     AppendFlag("DENY_GEOMETRY_SHADER_ROOT_ACCESS");
		if (flags & D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS)        AppendFlag("DENY_PIXEL_SHADER_ROOT_ACCESS");
		if (flags & D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT)                  AppendFlag("ALLOW_STREAM_OUTPUT");
		if (flags & D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE)                 AppendFlag("LOCAL_ROOT_SIGNATURE");
		if (flags & D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS) AppendFlag("DENY_AMPLIFICATION_SHADER_ROOT_ACCESS");
		if (flags & D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS)          AppendFlag("DENY_MESH_SHADER_ROOT_ACCESS");

		return result;
	}
	inline Char const* D3D12FilterToString(D3D12_FILTER filter)
	{
		switch (filter)
		{
		case D3D12_FILTER_MIN_MAG_MIP_POINT: return "MIN_MAG_MIP_POINT";
		case D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR: return "MIN_MAG_POINT_MIP_LINEAR";
		case D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT: return "MIN_POINT_MAG_LINEAR_MIP_POINT";
		case D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR: return "MIN_POINT_MAG_MIP_LINEAR";
		case D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT: return "MIN_LINEAR_MAG_MIP_POINT";
		case D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR: return "MIN_LINEAR_MAG_POINT_MIP_LINEAR";
		case D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT: return "MIN_MAG_LINEAR_MIP_POINT";
		case D3D12_FILTER_MIN_MAG_MIP_LINEAR: return "MIN_MAG_MIP_LINEAR";
		default: return "Unknown Filter";
		}
	}
	inline Char const* D3D12DescriptorHeapFlagsToString(D3D12_DESCRIPTOR_HEAP_FLAGS flags)
	{
		switch (flags)
		{
		case D3D12_DESCRIPTOR_HEAP_FLAG_NONE: return "NONE";
		case D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE: return "SHADER_VISIBLE";
		default:
			return "Unknown";
		}
	}
	inline Char const* D3D12PrimitiveTopologyTypeToString(D3D12_PRIMITIVE_TOPOLOGY_TYPE type)
	{
		switch (type)
		{
		case D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED: return "UNDEFINED";
		case D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT:     return "POINT";
		case D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE:      return "LINE";
		case D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE:  return "TRIANGLE";
		case D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH:     return "PATCH";
		default:                                      return "Unknown";
		}
	}
	inline Char const* D3D12PrimitiveTopologyToString(D3D12_PRIMITIVE_TOPOLOGY topology)
	{
		switch (topology)
		{
		case D3D_PRIMITIVE_TOPOLOGY_UNDEFINED: return "UNDEFINED";
		case D3D_PRIMITIVE_TOPOLOGY_POINTLIST: return "POINTLIST";
		case D3D_PRIMITIVE_TOPOLOGY_LINELIST: return "LINELIST";
		case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP: return "LINESTRIP";
		case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST: return "TRIANGLELIST";
		case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP: return "TRIANGLESTRIP";
		case D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ: return "LINELIST_ADJ";
		case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ: return "LINESTRIP_ADJ";
		case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ: return "TRIANGLELIST_ADJ";
		case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ: return "TRIANGLESTRIP_ADJ";
		default:
			if (topology >= D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST && topology <= D3D_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST) {
				return "PATCHLIST";
			}
		}
		return "Unknown";
	}
	inline Char const* D3D12PipelineStateFlagsToString(D3D12_PIPELINE_STATE_FLAGS flags)
	{
		switch (flags)
		{
		case D3D12_PIPELINE_STATE_FLAG_NONE:       return "NONE";
		case D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG: return "TOOL_DEBUG";
		}
		return "Unknown";
	}
	inline Char const* D3D12TextureLayoutToString(D3D12_TEXTURE_LAYOUT layout)
	{
		switch (layout)
		{
		case D3D12_TEXTURE_LAYOUT_UNKNOWN:              return "UNKNOWN";
		case D3D12_TEXTURE_LAYOUT_ROW_MAJOR:            return "ROW_MAJOR";
		case D3D12_TEXTURE_LAYOUT_64KB_UNDEFINED_SWIZZLE: return "64KB_UNDEFINED_SWIZZLE";
		case D3D12_TEXTURE_LAYOUT_64KB_STANDARD_SWIZZLE:  return "64KB_STANDARD_SWIZZLE";
		default:                                        return "Unknown";
		}
	}
	inline Char const* D3D12ShaderVisibilityToString(D3D12_SHADER_VISIBILITY visibility)
	{
		switch (visibility)
		{
		case D3D12_SHADER_VISIBILITY_ALL:             return "ALL";
		case D3D12_SHADER_VISIBILITY_VERTEX:          return "VERTEX";
		case D3D12_SHADER_VISIBILITY_HULL:            return "HULL";
		case D3D12_SHADER_VISIBILITY_DOMAIN:          return "DOMAIN";
		case D3D12_SHADER_VISIBILITY_GEOMETRY:        return "GEOMETRY";
		case D3D12_SHADER_VISIBILITY_PIXEL:           return "PIXEL";
		case D3D12_SHADER_VISIBILITY_AMPLIFICATION:   return "AMPLIFICATION";
		case D3D12_SHADER_VISIBILITY_MESH:            return "MESH";
		}
		return "Unknown";
	}
	inline Char const* D3D12DescriptorRangeTypeToString(D3D12_DESCRIPTOR_RANGE_TYPE type)
	{
		switch (type)
		{
		case D3D12_DESCRIPTOR_RANGE_TYPE_SRV: return "SRV";
		case D3D12_DESCRIPTOR_RANGE_TYPE_UAV: return "UAV";
		case D3D12_DESCRIPTOR_RANGE_TYPE_CBV: return "CBV";
		case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER: return "SAMPLER";
		}
		return "Unknown";
	}
	inline Char const* D3D12TextureAddressModeToString(D3D12_TEXTURE_ADDRESS_MODE mode)
	{
		switch (mode)
		{
		case D3D12_TEXTURE_ADDRESS_MODE_WRAP:       return "WRAP";
		case D3D12_TEXTURE_ADDRESS_MODE_MIRROR:     return "MIRROR";
		case D3D12_TEXTURE_ADDRESS_MODE_CLAMP:      return "CLAMP";
		case D3D12_TEXTURE_ADDRESS_MODE_BORDER:     return "BORDER";
		case D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE: return "MIRROR_ONCE";
		}
		return "Unknown";
	}
	inline Char const* D3D12ComparisonFuncToString(D3D12_COMPARISON_FUNC func)
	{
		switch (func)
		{
		case D3D12_COMPARISON_FUNC_NEVER:           return "NEVER";
		case D3D12_COMPARISON_FUNC_LESS:            return "LESS";
		case D3D12_COMPARISON_FUNC_EQUAL:           return "EQUAL";
		case D3D12_COMPARISON_FUNC_LESS_EQUAL:      return "LESS_EQUAL";
		case D3D12_COMPARISON_FUNC_GREATER:         return "GREATER";
		case D3D12_COMPARISON_FUNC_NOT_EQUAL:       return "NOT_EQUAL";
		case D3D12_COMPARISON_FUNC_GREATER_EQUAL:   return "GREATER_EQUAL";
		case D3D12_COMPARISON_FUNC_ALWAYS:          return "ALWAYS";
		}
		return "Unknown";
	}
	inline Char const* D3D12StaticBorderColorToString(D3D12_STATIC_BORDER_COLOR color)
	{
		switch (color)
		{
		case D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK: return "TRANSPARENT_BLACK";
		case D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK:      return "OPAQUE_BLACK";
		case D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE:      return "OPAQUE_WHITE";
		}
		return "Unknown";
	}
	inline std::string D3D12RootDescriptorFlagsToString(D3D12_ROOT_DESCRIPTOR_FLAGS flags)
	{
		if (flags == D3D12_ROOT_DESCRIPTOR_FLAG_NONE) return "NONE";

		std::string result;
		auto AppendFlag = [&](Char const* name)
		{
			if (!result.empty())
				result += " | ";
			result += name;
		};

		if (flags & D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE) 
		{
			AppendFlag("DATA_VOLATILE");
		}
		if (flags & D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE) 
		{
			AppendFlag("DATA_STATIC_WHILE_SET_AT_EXECUTE");
		}
		if (flags & D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC) 
		{
			AppendFlag("DATA_STATIC");
		}

		if (result.empty() && flags != D3D12_ROOT_DESCRIPTOR_FLAG_NONE) 
		{
			return "Unknown"; 
		}

		return result;
	}
	inline std::string D3D12DescriptorRangeFlagsToString(D3D12_DESCRIPTOR_RANGE_FLAGS flags)
	{
		if (flags == D3D12_DESCRIPTOR_RANGE_FLAG_NONE) return "NONE";

		std::string result;
		auto AppendFlag = [&](Char const* name)
		{
			if (!result.empty())
				result += " | ";
			result += name;
		};

		if (flags & D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE) 
		{
			AppendFlag("DESCRIPTORS_VOLATILE");
		}
		if (flags & D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE) 
		{
			AppendFlag("DATA_VOLATILE");
		}
		if (flags & D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE) 
		{
			AppendFlag("DATA_STATIC_WHILE_SET_AT_EXECUTE");
		}
		if (flags & D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC) 
		{
			AppendFlag("DATA_STATIC");
		}
		if (flags & D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_STATIC_KEEPING_BUFFER_BOUNDS_CHECKS) 
		{
			AppendFlag("DESCRIPTORS_STATIC_KEEPING_BUFFER_BOUNDS_CHECKS");
		}

		if (result.empty() && flags != D3D12_DESCRIPTOR_RANGE_FLAG_NONE) 
		{
			return "Unknown"; 
		}

		return result;
	}

	inline Char const* D3D12BlendToString(D3D12_BLEND blend)
	{
		switch (blend)
		{
		case D3D12_BLEND_ZERO: return "ZERO";
		case D3D12_BLEND_ONE: return "ONE";
		case D3D12_BLEND_SRC_COLOR: return "SRC_COLOR";
		case D3D12_BLEND_INV_SRC_COLOR: return "INV_SRC_COLOR";
		case D3D12_BLEND_SRC_ALPHA: return "SRC_ALPHA";
		case D3D12_BLEND_INV_SRC_ALPHA: return "INV_SRC_ALPHA";
		case D3D12_BLEND_DEST_ALPHA: return "DEST_ALPHA";
		case D3D12_BLEND_INV_DEST_ALPHA: return "INV_DEST_ALPHA";
		case D3D12_BLEND_DEST_COLOR: return "DEST_COLOR";
		case D3D12_BLEND_INV_DEST_COLOR: return "INV_DEST_COLOR";
		case D3D12_BLEND_SRC_ALPHA_SAT: return "SRC_ALPHA_SAT";
		case D3D12_BLEND_BLEND_FACTOR: return "BLEND_FACTOR";
		case D3D12_BLEND_INV_BLEND_FACTOR: return "INV_BLEND_FACTOR";
		case D3D12_BLEND_SRC1_COLOR: return "SRC1_COLOR";
		case D3D12_BLEND_INV_SRC1_COLOR: return "INV_SRC1_COLOR";
		case D3D12_BLEND_SRC1_ALPHA: return "SRC1_ALPHA";
		case D3D12_BLEND_INV_SRC1_ALPHA: return "INV_SRC1_ALPHA";
		default: return "UNKNOWN";
		}
	}
	inline Char const* D3D12BlendOpToString(D3D12_BLEND_OP blendOp)
	{
		switch (blendOp)
		{
		case D3D12_BLEND_OP_ADD: return "ADD";
		case D3D12_BLEND_OP_SUBTRACT: return "SUBTRACT";
		case D3D12_BLEND_OP_REV_SUBTRACT: return "REV_SUBTRACT";
		case D3D12_BLEND_OP_MIN: return "MIN";
		case D3D12_BLEND_OP_MAX: return "MAX";
		default: return "UNKNOWN";
		}
	}
	inline Char const* D3D12LogicOpToString(D3D12_LOGIC_OP logicOp)
	{
		switch (logicOp)
		{
		case D3D12_LOGIC_OP_CLEAR: return "CLEAR";
		case D3D12_LOGIC_OP_SET: return "SET";
		case D3D12_LOGIC_OP_COPY: return "COPY";
		case D3D12_LOGIC_OP_COPY_INVERTED: return "COPY_INVERTED";
		case D3D12_LOGIC_OP_NOOP: return "NOOP";
		case D3D12_LOGIC_OP_INVERT: return "INVERT";
		case D3D12_LOGIC_OP_AND: return "AND";
		case D3D12_LOGIC_OP_NAND: return "NAND";
		case D3D12_LOGIC_OP_OR: return "OR";
		case D3D12_LOGIC_OP_NOR: return "NOR";
		case D3D12_LOGIC_OP_XOR: return "XOR";
		case D3D12_LOGIC_OP_EQUIV: return "EQUIV";
		case D3D12_LOGIC_OP_AND_REVERSE: return "AND_REVERSE";
		case D3D12_LOGIC_OP_AND_INVERTED: return "AND_INVERTED";
		case D3D12_LOGIC_OP_OR_REVERSE: return "OR_REVERSE";
		case D3D12_LOGIC_OP_OR_INVERTED: return "OR_INVERTED";
		default: return "UNKNOWN";
		}
	}
	inline std::string D3D12ColorWriteEnableToString(UINT8 mask)
	{
		std::string result;
		if (mask == D3D12_COLOR_WRITE_ENABLE_ALL) return "ALL";
		if (mask & D3D12_COLOR_WRITE_ENABLE_RED) result += "RED | ";
		if (mask & D3D12_COLOR_WRITE_ENABLE_GREEN) result += "GREEN | ";
		if (mask & D3D12_COLOR_WRITE_ENABLE_BLUE) result += "BLUE | ";
		if (mask & D3D12_COLOR_WRITE_ENABLE_ALPHA) result += "ALPHA | ";
		if (result.empty()) return "NONE";
		return result.substr(0, result.size() - 3); 
	}
	inline Char const* D3D12FillModeToString(D3D12_FILL_MODE mode)
	{
		switch (mode)
		{
		case D3D12_FILL_MODE_WIREFRAME: return "WIREFRAME";
		case D3D12_FILL_MODE_SOLID: return "SOLID";
		default: return "UNKNOWN";
		}
	}
	inline Char const* D3D12CullModeToString(D3D12_CULL_MODE mode)
	{
		switch (mode)
		{
		case D3D12_CULL_MODE_NONE: return "NONE";
		case D3D12_CULL_MODE_FRONT: return "FRONT";
		case D3D12_CULL_MODE_BACK: return "BACK";
		default: return "UNKNOWN";
		}
	}
	inline Char const* D3D12ConservativeRasterizationModeToString(D3D12_CONSERVATIVE_RASTERIZATION_MODE mode)
	{
		switch (mode)
		{
		case D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF: return "OFF";
		case D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON: return "ON";
		default: return "UNKNOWN";
		}
	}
	inline Char const* D3D12DepthWriteMaskToString(D3D12_DEPTH_WRITE_MASK mask)
	{
		switch (mask)
		{
		case D3D12_DEPTH_WRITE_MASK_ZERO: return "ZERO";
		case D3D12_DEPTH_WRITE_MASK_ALL: return "ALL";
		default: return "UNKNOWN";
		}
	}
	inline Char const* D3D12StencilOpToString(D3D12_STENCIL_OP op)
	{
		switch (op)
		{
		case D3D12_STENCIL_OP_KEEP: return "KEEP";
		case D3D12_STENCIL_OP_ZERO: return "ZERO";
		case D3D12_STENCIL_OP_REPLACE: return "REPLACE";
		case D3D12_STENCIL_OP_INCR_SAT: return "INCR_SAT";
		case D3D12_STENCIL_OP_DECR_SAT: return "DECR_SAT";
		case D3D12_STENCIL_OP_INVERT: return "INVERT";
		case D3D12_STENCIL_OP_INCR: return "INCR";
		case D3D12_STENCIL_OP_DECR: return "DECR";
		default: return "UNKNOWN";
		}
	}
	inline Char const* D3D12InputClassificationToString(D3D12_INPUT_CLASSIFICATION classification)
	{
		switch (classification)
		{
		case D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA: return "PER_VERTEX_DATA";
		case D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA: return "PER_INSTANCE_DATA";
		default: return "UNKNOWN";
		}
	}
	inline Char const* D3D12IndexBufferStripCutValueToString(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE value)
	{
		switch (value)
		{
		case D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED: return "DISABLED";
		case D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFF: return "0xFFFF";
		case D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFFFFFF: return "0xFFFFFFFF";
		default: return "UNKNOWN";
		}
	}

	inline Char const* D3D12SrvDimensionToString(D3D12_SRV_DIMENSION dimension)
	{
		switch (dimension)
		{
		case D3D12_SRV_DIMENSION_UNKNOWN:                     return "Unknown";
		case D3D12_SRV_DIMENSION_BUFFER:                      return "Buffer";
		case D3D12_SRV_DIMENSION_TEXTURE1D:                   return "Texture1D";
		case D3D12_SRV_DIMENSION_TEXTURE1DARRAY:              return "Texture1DArray";
		case D3D12_SRV_DIMENSION_TEXTURE2D:                   return "Texture2D";
		case D3D12_SRV_DIMENSION_TEXTURE2DARRAY:              return "Texture2DArray";
		case D3D12_SRV_DIMENSION_TEXTURE2DMS:                 return "Texture2DMS";
		case D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY:            return "Texture2DMSArray";
		case D3D12_SRV_DIMENSION_TEXTURE3D:                   return "Texture3D";
		case D3D12_SRV_DIMENSION_TEXTURECUBE:                 return "TextureCube";
		case D3D12_SRV_DIMENSION_TEXTURECUBEARRAY:            return "TextureCubeArray";
		case D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE: return "RaytracingAS";
		default:                                              return "Unknown";
		}
	}
	inline Char const* D3D12UavDimensionToString(D3D12_UAV_DIMENSION dimension)
	{
		switch (dimension)
		{
		case D3D12_UAV_DIMENSION_UNKNOWN:         return "Unknown";
		case D3D12_UAV_DIMENSION_BUFFER:          return "Buffer";
		case D3D12_UAV_DIMENSION_TEXTURE1D:       return "Texture1D";
		case D3D12_UAV_DIMENSION_TEXTURE1DARRAY:  return "Texture1DArray";
		case D3D12_UAV_DIMENSION_TEXTURE2D:       return "Texture2D";
		case D3D12_UAV_DIMENSION_TEXTURE2DARRAY:  return "Texture2DArray";
		case D3D12_UAV_DIMENSION_TEXTURE3D:       return "Texture3D";
		default:                                  return "Unknown";
		}
	}
	inline Char const* D3D12RtvDimensionToString(D3D12_RTV_DIMENSION dimension)
	{
		switch (dimension)
		{
		case D3D12_RTV_DIMENSION_UNKNOWN:         return  "Unknown";
		case D3D12_RTV_DIMENSION_BUFFER:          return  "Buffer";
		case D3D12_RTV_DIMENSION_TEXTURE1D:       return  "Texture1D";
		case D3D12_RTV_DIMENSION_TEXTURE1DARRAY:  return  "Texture1DArray";
		case D3D12_RTV_DIMENSION_TEXTURE2D:       return  "Texture2D";
		case D3D12_RTV_DIMENSION_TEXTURE2DARRAY:  return  "Texture2DArray";
		case D3D12_RTV_DIMENSION_TEXTURE2DMS:     return  "Texture2DMS";
		case D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY: return "Texture2DMSArray";
		case D3D12_RTV_DIMENSION_TEXTURE3D:       return  "Texture3D";
		default:                                  return  "Unknown";
		}
	}
	inline Char const* D3D12DsvDimensionToString(D3D12_DSV_DIMENSION dimension)
	{
		switch (dimension)
		{
		case D3D12_DSV_DIMENSION_UNKNOWN:          return "Unknown";
		case D3D12_DSV_DIMENSION_TEXTURE1D:        return "Texture1D";
		case D3D12_DSV_DIMENSION_TEXTURE1DARRAY:   return "Texture1DArray";
		case D3D12_DSV_DIMENSION_TEXTURE2D:        return "Texture2D";
		case D3D12_DSV_DIMENSION_TEXTURE2DARRAY:   return "Texture2DArray";
		case D3D12_DSV_DIMENSION_TEXTURE2DMS:      return "Texture2DMS";
		case D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY: return "Texture2DMSArray";
		default:                                   return "Unknown";
		}
	}

	inline std::string D3D12DsvFlagsToString(D3D12_DSV_FLAGS flags)
	{
		if (flags == D3D12_DSV_FLAG_NONE) return "NONE";

		std::string result;
		auto AppendFlag = [&](Char const* name) 
		{
			if (!result.empty())
				result += " | ";
			result += name;
		};

		if (flags & D3D12_DSV_FLAG_READ_ONLY_DEPTH) 
		{
			AppendFlag("READ_ONLY_DEPTH");
		}
		if (flags & D3D12_DSV_FLAG_READ_ONLY_STENCIL) 
		{
			AppendFlag("READ_ONLY_STENCIL");
		}

		if (result.empty() && flags != D3D12_DSV_FLAG_NONE) 
		{
			return "Unknown";
		}
		return result;
	}
	inline std::string D3D12ShaderComponentMappingToString(UINT mapping)
	{
		Char components[5] = "RGBA"; 
		const D3D12_SHADER_COMPONENT_MAPPING vals[] = 
		{
			D3D12_DECODE_SHADER_4_COMPONENT_MAPPING(0, mapping),
			D3D12_DECODE_SHADER_4_COMPONENT_MAPPING(1, mapping),
			D3D12_DECODE_SHADER_4_COMPONENT_MAPPING(2, mapping),
			D3D12_DECODE_SHADER_4_COMPONENT_MAPPING(3, mapping)
		};

		auto map_char = [](D3D12_SHADER_COMPONENT_MAPPING val) -> Char {
		switch (val) 
			{
			case D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_0: return 'R'; 
			case D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_1: return 'G'; 
			case D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_2: return 'B'; 
			case D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_3: return 'A'; 
			case D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_0:           return '0';
			case D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_1:           return '1';
			default: return '?';
			}
		};

		for (Uint32 i = 0; i < 4; ++i) 
		{
			components[i] = map_char(vals[i]);
		}
		return std::string(components); 
	}
	inline std::string D3D12BufferSrvFlagsToString(D3D12_BUFFER_SRV_FLAGS flags)
	{
		if (flags == D3D12_BUFFER_SRV_FLAG_NONE) return "NONE";
		if (flags == D3D12_BUFFER_SRV_FLAG_RAW)  return "RAW";
		return "NONE";
	}
	inline std::string D3D12BufferUavFlagsToString(D3D12_BUFFER_UAV_FLAGS flags)
	{
		if (flags == D3D12_BUFFER_UAV_FLAG_NONE) return "NONE";
		if (flags == D3D12_BUFFER_UAV_FLAG_RAW)  return "RAW";
		return "NONE";
	}

	inline Char const* D3D12IndirectArgumentTypeToString(D3D12_INDIRECT_ARGUMENT_TYPE type)
	{
		switch (type)
		{
		case D3D12_INDIRECT_ARGUMENT_TYPE_DRAW: return "DRAW";
		case D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED: return "DRAW_INDEXED";
		case D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH: return "DISPATCH";
		case D3D12_INDIRECT_ARGUMENT_TYPE_VERTEX_BUFFER_VIEW: return "VERTEX_BUFFER_VIEW";
		case D3D12_INDIRECT_ARGUMENT_TYPE_INDEX_BUFFER_VIEW: return "INDEX_BUFFER_VIEW";
		case D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT: return "CONSTANT";
		case D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT_BUFFER_VIEW: return "CONSTANT_BUFFER_VIEW";
		case D3D12_INDIRECT_ARGUMENT_TYPE_SHADER_RESOURCE_VIEW: return "SHADER_RESOURCE_VIEW";
		case D3D12_INDIRECT_ARGUMENT_TYPE_UNORDERED_ACCESS_VIEW: return "UNORDERED_ACCESS_VIEW";
		default: return "Unknown";
		}
	}
	inline std::string D3D12HeapFlagsToString(D3D12_HEAP_FLAGS flags)
	{
		if (flags == D3D12_HEAP_FLAG_NONE) return "D3D12_HEAP_FLAG_NONE";

		std::string result;
		auto AppendState = [&](const char* name)
			{
				if (!result.empty())
					result += " | ";
				result += name;
			};

		if (flags & D3D12_HEAP_FLAG_SHARED)
			AppendState("D3D12_HEAP_FLAG_SHARED");
		if (flags & D3D12_HEAP_FLAG_DENY_BUFFERS)
			AppendState("D3D12_HEAP_FLAG_DENY_BUFFERS");
		if (flags & D3D12_HEAP_FLAG_ALLOW_DISPLAY)
			AppendState("D3D12_HEAP_FLAG_ALLOW_DISPLAY");
		if (flags & D3D12_HEAP_FLAG_SHARED_CROSS_ADAPTER)
			AppendState("D3D12_HEAP_FLAG_SHARED_CROSS_ADAPTER");
		if (flags & D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES)
			AppendState("D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES");
		if (flags & D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES)
			AppendState("D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES");
		if (flags & D3D12_HEAP_FLAG_HARDWARE_PROTECTED)
			AppendState("D3D12_HEAP_FLAG_HARDWARE_PROTECTED");
		if (flags & D3D12_HEAP_FLAG_ALLOW_WRITE_WATCH)
			AppendState("D3D12_HEAP_FLAG_ALLOW_WRITE_WATCH");
		if (flags & D3D12_HEAP_FLAG_ALLOW_SHADER_ATOMICS)
			AppendState("D3D12_HEAP_FLAG_ALLOW_SHADER_ATOMICS");
		if (flags & D3D12_HEAP_FLAG_CREATE_NOT_RESIDENT)
			AppendState("D3D12_HEAP_FLAG_CREATE_NOT_RESIDENT");
		if (flags & D3D12_HEAP_FLAG_CREATE_NOT_ZEROED)
			AppendState("D3D12_HEAP_FLAG_CREATE_NOT_ZEROED");

		if (result.empty())
		{
			if (flags == D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES)
				return "D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES";
			else if (flags == D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS)
				return "D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS";
			else if (flags == D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES)
				return "D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES";
			else if (flags == D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES)
				return "D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES";
		}
		return result.empty() ? "Unknown (0x" + std::to_string(flags) + ")" : result;
	}

	inline Char const* D3D12ShadingRateToString(D3D12_SHADING_RATE rate)
	{
		switch (rate)
		{
		case D3D12_SHADING_RATE_1X1: return "1x1";
		case D3D12_SHADING_RATE_1X2: return "1x2";
		case D3D12_SHADING_RATE_2X1: return "2x1";
		case D3D12_SHADING_RATE_2X2: return "2x2";
		case D3D12_SHADING_RATE_2X4: return "2x4";
		case D3D12_SHADING_RATE_4X2: return "4x2";
		case D3D12_SHADING_RATE_4X4: return "4x4";
		default: return "Unknown";
		}
	}
	inline Char const* D3D12ShadingRateCombinerToString(D3D12_SHADING_RATE_COMBINER combiner)
	{
		switch (combiner)
		{
		case D3D12_SHADING_RATE_COMBINER_PASSTHROUGH: return "Passthrough";
		case D3D12_SHADING_RATE_COMBINER_OVERRIDE: return "Override";
		case D3D12_SHADING_RATE_COMBINER_MIN: return "Min";
		case D3D12_SHADING_RATE_COMBINER_MAX: return "Max";
		case D3D12_SHADING_RATE_COMBINER_SUM: return "Sum";
		default: return "Unknown";
		}
	}

	inline Char const* D3D12StateObjectTypeToString(D3D12_STATE_OBJECT_TYPE type)
	{
		switch (type)
		{
		case D3D12_STATE_OBJECT_TYPE_COLLECTION: return "Collection";
		case D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE: return "Raytracing Pipeline";
		case D3D12_STATE_OBJECT_TYPE_EXECUTABLE: return "Executable";
		default: return "<Unknown>";
		}
	}
	inline Char const* D3D12StateSubobjectTypeToString(D3D12_STATE_SUBOBJECT_TYPE type)
	{
		switch (type)
		{
		case D3D12_STATE_SUBOBJECT_TYPE_STATE_OBJECT_CONFIG: return "State Object Config";
		case D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE: return "Global Root Signature";
		case D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE: return "Local Root Signature";
		case D3D12_STATE_SUBOBJECT_TYPE_NODE_MASK: return "Node Mask";
		case D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY: return "DXIL Library";
		case D3D12_STATE_SUBOBJECT_TYPE_EXISTING_COLLECTION: return "Existing Collection";
		case D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION: return "Subobject to Exports Association";
		case D3D12_STATE_SUBOBJECT_TYPE_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION: return "DXIL Subobject to Exports Association";
		case D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG: return "Shader Config";
		case D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG: return "Pipeline Config";
		case D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP: return "Hit Group";
		case D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG1: return "Pipeline Config 1";
		default: return "<Unknown>";
		}
	}

	inline Char const* D3D12ResolveModeToString(D3D12_RESOLVE_MODE mode)
	{
		switch (mode)
		{
		case D3D12_RESOLVE_MODE_DECOMPRESS: return "DECOMPRESS";
		case D3D12_RESOLVE_MODE_MIN: return "MIN";
		case D3D12_RESOLVE_MODE_MAX: return "MAX";
		case D3D12_RESOLVE_MODE_AVERAGE: return "AVERAGE";
		case D3D12_RESOLVE_MODE_ENCODE_SAMPLER_FEEDBACK: return "ENCODE_SAMPLER_FEEDBACK";
		case D3D12_RESOLVE_MODE_DECODE_SAMPLER_FEEDBACK: return "DECODE_SAMPLER_FEEDBACK";
		default: return "UNKNOWN";
		}
	}
}