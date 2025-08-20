#include <format>
#include "GUIRenderUtil.h"
#include "SelectedItem.h"
#include "Tracking/State.h"
#include "Tracking/ObjectTracker.h"
#include "Tracking/ResourceAddressTracker.h"
#include "ImGui/imgui.h"

namespace vista
{
	namespace details
	{
		template<typename PSODescT>
		static void RenderComputePSODetails(PSODescT const& desc, ObjectTracker const& objectTracker)
		{
			if (desc.RootSignatureId != InvalidObjectID)
			{
				TrackedObjectInfo const* rsInfo = objectTracker.GetObjectInfo(desc.RootSignatureId);
				if (rsInfo)
					ImGui::Text("Root Signature: obj#%llu (%s)", rsInfo->objectId, rsInfo->objectDebugName.c_str());
				else
					ImGui::Text("Root Signature: obj#%llu (<Unknown>)", desc.RootSignatureId);
			}
			else
			{
				ImGui::TextUnformatted("Root Signature: NULL");
			}
			ImGui::Text("Compute Shader: %s", desc.CS.IsValid() ? "Set" : "NULL");
			ImGui::Text("NodeMask: 0x%X", desc.NodeMask);
			if (ImGui::TreeNode("Cached Pipeline State"))
			{
				ImGui::Text("Cached Blob: %s", desc.CachedPSO.IsValid() ? "Set" : "NULL");
				ImGui::Text("Size: %zu bytes", desc.CachedPSO.Blob.size());
				ImGui::TreePop();
			}
			ImGui::Text("Flags: %s", D3D12PipelineStateFlagsToString(desc.Flags));
		}

		template<typename PSODescT>
		static void RenderGraphicsPSODetails(PSODescT const& desc, ObjectTracker const& objectTracker)
		{
			if (desc.RootSignatureId != InvalidObjectID)
			{
				TrackedObjectInfo const* rsInfo = objectTracker.GetObjectInfo(desc.RootSignatureId);
				if (rsInfo)
					ImGui::Text("Root Signature: obj#%llu (%s)", rsInfo->objectId, rsInfo->objectDebugName.c_str());
				else
					ImGui::Text("Root Signature: obj#%llu (<Unknown>)", desc.RootSignatureId);
			}
			else
			{
				ImGui::TextUnformatted("Root Signature: NULL");
			}

			ImGui::Text("Vertex Shader: %s", desc.VS.IsValid() ? "Set" : "NULL");
			ImGui::Text("Pixel Shader: %s", desc.PS.IsValid() ? "Set" : "NULL");
			ImGui::Text("Domain Shader: %s", desc.DS.IsValid() ? "Set" : "NULL");
			ImGui::Text("Hull Shader: %s", desc.HS.IsValid() ? "Set" : "NULL");
			ImGui::Text("Geometry Shader: %s", desc.GS.IsValid() ? "Set" : "NULL");

			ImGui::Separator();
			if (ImGui::TreeNodeEx("Stream Output", ImGuiTreeNodeFlags_None))
			{
				StreamOutputDescStorage const& so = desc.StreamOutput;
				ImGui::Text("Num Entries: %u", so.SODeclaration.size());
				ImGui::Text("Num Strides: %u", so.BufferStrides.size());
				ImGui::Text("Rasterized Stream: %u", so.RasterizedStream);
				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("Blend State", ImGuiTreeNodeFlags_None))
			{
				D3D12_BLEND_DESC const& blend = desc.BlendState;
				ImGui::Text("Alpha To Coverage Enable: %s", blend.AlphaToCoverageEnable ? "TRUE" : "FALSE");
				ImGui::Text("Independent Blend Enable: %s", blend.IndependentBlendEnable ? "TRUE" : "FALSE");

				if (ImGui::TreeNodeEx("Render Target Blends", ImGuiTreeNodeFlags_None))
				{
					for (Uint i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
					{
						if (!blend.IndependentBlendEnable && i >= 1)
							break;

						ImGui::PushID(i);
						if (ImGui::TreeNodeEx(std::format("RT[{}]", i).c_str()))
						{
							D3D12_RENDER_TARGET_BLEND_DESC const& rtBlend = blend.RenderTarget[i];
							ImGui::Text("Blend Enable: %s", rtBlend.BlendEnable ? "TRUE" : "FALSE");
							ImGui::Text("Logic Op Enable: %s", rtBlend.LogicOpEnable ? "TRUE" : "FALSE");
							ImGui::Text("Src Blend: %s", D3D12BlendToString(rtBlend.SrcBlend));
							ImGui::Text("Dest Blend: %s", D3D12BlendToString(rtBlend.DestBlend));
							ImGui::Text("Blend Op: %s", D3D12BlendOpToString(rtBlend.BlendOp));
							ImGui::Text("Src Blend Alpha: %s", D3D12BlendToString(rtBlend.SrcBlendAlpha));
							ImGui::Text("Dest Blend Alpha: %s", D3D12BlendToString(rtBlend.DestBlendAlpha));
							ImGui::Text("Blend Op Alpha: %s", D3D12BlendOpToString(rtBlend.BlendOpAlpha));
							ImGui::Text("Logic Op: %s", D3D12LogicOpToString(rtBlend.LogicOp));
							ImGui::Text("Render Target Write Mask: 0x%02X", rtBlend.RenderTargetWriteMask);
							ImGui::TreePop();
						}
						ImGui::PopID();
					}
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}

			ImGui::Text("SampleMask: 0x%X", desc.SampleMask);

			if (ImGui::TreeNodeEx("Rasterizer State", ImGuiTreeNodeFlags_None))
			{
				D3D12_RASTERIZER_DESC const& rasterDesc = desc.RasterizerState;
				ImGui::Text("Fill Mode: %s", D3D12FillModeToString(rasterDesc.FillMode));
				ImGui::Text("Cull Mode: %s", D3D12CullModeToString(rasterDesc.CullMode));
				ImGui::Text("Front Counter Clockwise: %s", rasterDesc.FrontCounterClockwise ? "TRUE" : "FALSE");
				ImGui::Text("Depth Bias: %d", rasterDesc.DepthBias);
				ImGui::Text("Depth Bias Clamp: %.2f", rasterDesc.DepthBiasClamp);
				ImGui::Text("Slope Scaled Depth Bias: %.2f", rasterDesc.SlopeScaledDepthBias);
				ImGui::Text("Depth Clip Enable: %s", rasterDesc.DepthClipEnable ? "TRUE" : "FALSE");
				ImGui::Text("Multisample Enable: %s", rasterDesc.MultisampleEnable ? "TRUE" : "FALSE");
				ImGui::Text("Antialiased Line Enable: %s", rasterDesc.AntialiasedLineEnable ? "TRUE" : "FALSE");
				ImGui::Text("Forced Sample Count: %u", rasterDesc.ForcedSampleCount);
				ImGui::Text("Conservative Raster: %s", D3D12ConservativeRasterizationModeToString(rasterDesc.ConservativeRaster));
				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("Depth Stencil State", ImGuiTreeNodeFlags_None))
			{
				D3D12_DEPTH_STENCIL_DESC const& ds = desc.DepthStencilState;
				ImGui::Text("Depth Enable: %s", ds.DepthEnable ? "TRUE" : "FALSE");
				ImGui::Text("Depth Write Mask: %s", D3D12DepthWriteMaskToString(ds.DepthWriteMask));
				ImGui::Text("Depth Func: %s", D3D12ComparisonFuncToString(ds.DepthFunc));
				ImGui::Separator();
				ImGui::Text("Stencil Enable: %s", ds.StencilEnable ? "TRUE" : "FALSE");
				ImGui::Text("Stencil Read Mask: 0x%02X", ds.StencilReadMask);
				ImGui::Text("Stencil Write Mask: 0x%02X", ds.StencilWriteMask);

				if (ImGui::TreeNode("Front Face Stencil"))
				{
					D3D12_DEPTH_STENCILOP_DESC const& front = ds.FrontFace;
					ImGui::Text("Stencil Fail Op: %s", D3D12StencilOpToString(front.StencilFailOp));
					ImGui::Text("Stencil Depth Fail Op: %s", D3D12StencilOpToString(front.StencilDepthFailOp));
					ImGui::Text("Stencil Pass Op: %s", D3D12StencilOpToString(front.StencilPassOp));
					ImGui::Text("Stencil Func: %s", D3D12ComparisonFuncToString(front.StencilFunc));
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Back Face Stencil"))
				{
					D3D12_DEPTH_STENCILOP_DESC const& back = ds.BackFace;
					ImGui::Text("Stencil Fail Op: %s", D3D12StencilOpToString(back.StencilFailOp));
					ImGui::Text("Stencil Depth Fail Op: %s", D3D12StencilOpToString(back.StencilDepthFailOp));
					ImGui::Text("Stencil Pass Op: %s", D3D12StencilOpToString(back.StencilPassOp));
					ImGui::Text("Stencil Func: %s", D3D12ComparisonFuncToString(back.StencilFunc));
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("Input Layout", ImGuiTreeNodeFlags_None))
			{
				std::vector<InputElementDescStorage> const& inputLayoutElements = desc.InputLayoutElements;
				ImGui::Text("Num Elements: %u", inputLayoutElements.size());
				if (!inputLayoutElements.empty())
				{
					if (ImGui::TreeNodeEx("Elements", ImGuiTreeNodeFlags_None))
					{
						for (Uint32 i = 0; i < inputLayoutElements.size(); ++i)
						{
							ImGui::PushID(i);
							InputElementDescStorage const& elem = inputLayoutElements[i];
							if (ImGui::TreeNodeEx(std::format("[{}] {}", i, !elem.SemanticName.empty() ? elem.SemanticName.c_str() : "<NULL>").c_str()))
							{
								ImGui::Text("Semantic Name: %s", !elem.SemanticName.empty() ? elem.SemanticName.c_str() : "NULL");
								ImGui::Text("Semantic Index: %u", elem.SemanticIndex);
								ImGui::Text("Format: %s", DXGIFormatToString(elem.Format));
								ImGui::Text("Input Slot: %u", elem.InputSlot);
								ImGui::Text("Aligned Byte Offset: %u", elem.AlignedByteOffset);
								ImGui::Text("Input Slot Class: %s", D3D12InputClassificationToString(elem.InputSlotClass));
								ImGui::Text("Instance Data Step Rate: %u", elem.InstanceDataStepRate);
								ImGui::TreePop();
							}
							ImGui::PopID();
						}
						ImGui::TreePop();
					}
				}
				else
					ImGui::TextUnformatted("Elements: (None)");
				ImGui::TreePop();
			}

			ImGui::Text("IB Strip Cut Value: %s", D3D12IndexBufferStripCutValueToString(desc.IBStripCutValue));
			ImGui::Text("Primitive Topology Type: %s", D3D12PrimitiveTopologyTypeToString(desc.PrimitiveTopologyType));

			if (ImGui::TreeNodeEx("PSO Render Target Formats", ImGuiTreeNodeFlags_None))
			{
				ImGui::Text("Num Render Targets: %u", desc.NumRenderTargets);
				for (Uint32 i = 0; i < desc.NumRenderTargets; ++i)
					ImGui::Text("  RTVFormat[%u]: %s", i, DXGIFormatToString(desc.RTVFormats[i]));
				if (desc.NumRenderTargets == 0)
					ImGui::TextUnformatted("  (No Render Targets)");
				ImGui::TreePop();
			}

			ImGui::Text("DSV Format: %s", DXGIFormatToString(desc.DSVFormat));

			if (ImGui::TreeNodeEx("Sample Desc", ImGuiTreeNodeFlags_None))
			{
				ImGui::Text("Count: %u", desc.SampleDesc.Count);
				ImGui::Text("Quality: %u", desc.SampleDesc.Quality);
				ImGui::TreePop();
			}

			ImGui::Text("NodeMask: 0x%X", desc.NodeMask);
			if (ImGui::TreeNode("Cached Pipeline State"))
			{
				ImGui::Text("Cached Blob: %s", desc.CachedPSO.IsValid() ? "Set" : "NULL");
				ImGui::Text("Size: %zu bytes", desc.CachedPSO.Blob.size());
				ImGui::TreePop();
			}
			ImGui::Text("Flags: %s", D3D12PipelineStateFlagsToString(desc.Flags));
		}
	}

	void RenderSamplerDesc(D3D12_SAMPLER_DESC const& samplerDesc, Char const* labelPrefix)
	{
		ImGui::Text("%s Details:", labelPrefix);
		if (ImGui::TreeNode("Properties"))
		{
			ImGui::Text("Filter: %s", D3D12FilterToString(samplerDesc.Filter));
			ImGui::Text("Address U: %s", D3D12TextureAddressModeToString(samplerDesc.AddressU));
			ImGui::Text("Address V: %s", D3D12TextureAddressModeToString(samplerDesc.AddressV));
			ImGui::Text("Address W: %s", D3D12TextureAddressModeToString(samplerDesc.AddressW));
			ImGui::Text("Mip LOD Bias: %.2f", samplerDesc.MipLODBias);
			ImGui::Text("Max Anisotropy: %u", samplerDesc.MaxAnisotropy);
			ImGui::Text("Comparison Func: %s", D3D12ComparisonFuncToString(samplerDesc.ComparisonFunc));
			ImGui::Text("Border Color: %.2f, %.2f, %.2f, %.2f", samplerDesc.BorderColor[0], samplerDesc.BorderColor[1], samplerDesc.BorderColor[2], samplerDesc.BorderColor[3]);
			ImGui::Text("Min LOD: %.2f", samplerDesc.MinLOD);
			ImGui::Text("Max LOD: %.2f", samplerDesc.MaxLOD);
			ImGui::TreePop();
		}
	}

	void RenderSamplerDesc(D3D12_STATIC_SAMPLER_DESC const& staticDesc, Char const* labelPrefix)
	{
		D3D12_SAMPLER_DESC desc{};
		desc.Filter = staticDesc.Filter;
		desc.AddressU = staticDesc.AddressU;
		desc.AddressV = staticDesc.AddressV;
		desc.AddressW = staticDesc.AddressW;
		desc.MipLODBias = staticDesc.MipLODBias;
		desc.MaxAnisotropy = staticDesc.MaxAnisotropy;
		desc.ComparisonFunc = staticDesc.ComparisonFunc;
		desc.MinLOD = staticDesc.MinLOD;
		desc.MaxLOD = staticDesc.MaxLOD;

		switch (staticDesc.BorderColor)
		{
		case D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK:
			desc.BorderColor[0] = 0.0f;
			desc.BorderColor[1] = 0.0f;
			desc.BorderColor[2] = 0.0f;
			desc.BorderColor[3] = 0.0f;
			break;
		case D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK:
			desc.BorderColor[0] = 0.0f;
			desc.BorderColor[1] = 0.0f;
			desc.BorderColor[2] = 0.0f;
			desc.BorderColor[3] = 1.0f;
			break;
		case D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE:
			desc.BorderColor[0] = 1.0f;
			desc.BorderColor[1] = 1.0f;
			desc.BorderColor[2] = 1.0f;
			desc.BorderColor[3] = 1.0f;
			break;
		default:
			desc.BorderColor[0] = 0.0f;
			desc.BorderColor[1] = 0.0f;
			desc.BorderColor[2] = 0.0f;
			desc.BorderColor[3] = 0.0f;
			break;
		}
		RenderSamplerDesc(desc, labelPrefix);
	}
	void RenderDescriptorInfo(DescriptorInfo const& descInfo, Int descriptorIndex)
	{
		if (std::holds_alternative<std::monostate>(descInfo.desc))
		{
			ImGui::TextDisabled("No descriptor details available");
			return;
		}

		std::visit([&](auto&& viewDescSpecific)
			{
				using T = std::decay_t<decltype(viewDescSpecific)>;

				ImGui::PushID(descriptorIndex);
				if constexpr (std::is_same_v<T, D3D12_CONSTANT_BUFFER_VIEW_DESC>)
				{
					ImGui::Text("CBV Details:");
					ImGui::PushID("CBV");
					if (ImGui::TreeNode("CBV Properties"))
					{
						ImGui::Text("Buffer Location: 0x%llX", viewDescSpecific.BufferLocation);
						ImGui::Text("Size In Bytes: %u", viewDescSpecific.SizeInBytes);
						ImGui::TreePop();
					}
					ImGui::PopID();
				}
				else if constexpr (std::is_same_v<T, D3D12_SHADER_RESOURCE_VIEW_DESC>)
				{
					ImGui::Text("SRV Details:");
					ImGui::Text("Format: %s", DXGIFormatToString(viewDescSpecific.Format));
					ImGui::Text("View Dimension: %s", D3D12SrvDimensionToString(viewDescSpecific.ViewDimension));
					ImGui::Text(
						"Shader 4 Component Mapping: %s (0x%X)",
						D3D12ShaderComponentMappingToString(viewDescSpecific.Shader4ComponentMapping).c_str(),
						viewDescSpecific.Shader4ComponentMapping
					);

					ImGui::PushID("SRV");
					switch (viewDescSpecific.ViewDimension)
					{
					case D3D12_SRV_DIMENSION_BUFFER:
						ImGui::PushID("Buffer");
						if (ImGui::TreeNode("Buffer Properties"))
						{
							ImGui::Text("First Element: %llu", viewDescSpecific.Buffer.FirstElement);
							ImGui::Text("Num Elements: %u", viewDescSpecific.Buffer.NumElements);
							ImGui::Text("Structure Byte Stride: %u", viewDescSpecific.Buffer.StructureByteStride);
							ImGui::Text("Flags: %s", D3D12BufferSrvFlagsToString(viewDescSpecific.Buffer.Flags).c_str());
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_SRV_DIMENSION_TEXTURE1D:
						ImGui::PushID("Texture1D");
						if (ImGui::TreeNode("Texture1D Properties"))
						{
							ImGui::Text("Most Detailed Mip: %u", viewDescSpecific.Texture1D.MostDetailedMip);
							ImGui::Text("Mip Levels: %u", viewDescSpecific.Texture1D.MipLevels);
							ImGui::Text("Resource Min LOD Clamp: %.2f", viewDescSpecific.Texture1D.ResourceMinLODClamp);
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_SRV_DIMENSION_TEXTURE1DARRAY:
						ImGui::PushID("Texture1DArray");
						if (ImGui::TreeNode("Texture1DArray Properties"))
						{
							ImGui::Text("Most Detailed Mip: %u", viewDescSpecific.Texture1DArray.MostDetailedMip);
							ImGui::Text("Mip Levels: %u", viewDescSpecific.Texture1DArray.MipLevels);
							ImGui::Text("First Array Slice: %u", viewDescSpecific.Texture1DArray.FirstArraySlice);
							ImGui::Text("Array Size: %u", viewDescSpecific.Texture1DArray.ArraySize);
							ImGui::Text("Resource Min LOD Clamp: %.2f", viewDescSpecific.Texture1DArray.ResourceMinLODClamp);
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_SRV_DIMENSION_TEXTURE2D:
						ImGui::PushID("Texture2D");
						if (ImGui::TreeNode("Texture2D Properties"))
						{
							ImGui::Text("Most Detailed Mip: %u", viewDescSpecific.Texture2D.MostDetailedMip);
							ImGui::Text("Mip Levels: %u", viewDescSpecific.Texture2D.MipLevels);
							ImGui::Text("Plane Slice: %u", viewDescSpecific.Texture2D.PlaneSlice);
							ImGui::Text("Resource Min LOD Clamp: %.2f", viewDescSpecific.Texture2D.ResourceMinLODClamp);
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_SRV_DIMENSION_TEXTURE2DARRAY:
						ImGui::PushID("Texture2DArray");
						if (ImGui::TreeNode("Texture2DArray Properties"))
						{
							ImGui::Text("Most Detailed Mip: %u", viewDescSpecific.Texture2DArray.MostDetailedMip);
							ImGui::Text("Mip Levels: %u", viewDescSpecific.Texture2DArray.MipLevels);
							ImGui::Text("First Array Slice: %u", viewDescSpecific.Texture2DArray.FirstArraySlice);
							ImGui::Text("Array Size: %u", viewDescSpecific.Texture2DArray.ArraySize);
							ImGui::Text("Plane Slice: %u", viewDescSpecific.Texture2DArray.PlaneSlice);
							ImGui::Text("Resource Min LOD Clamp: %.2f", viewDescSpecific.Texture2DArray.ResourceMinLODClamp);
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_SRV_DIMENSION_TEXTURE2DMS:
						ImGui::PushID("Texture2DMS");
						if (ImGui::TreeNode("Texture2DMS Properties"))
						{
							ImGui::TextUnformatted("(No specific fields)");
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY:
						ImGui::PushID("Texture2DMSArray");
						if (ImGui::TreeNode("Texture2DMSArray Properties"))
						{
							ImGui::Text("First Array Slice: %u", viewDescSpecific.Texture2DMSArray.FirstArraySlice);
							ImGui::Text("Array Size: %u", viewDescSpecific.Texture2DMSArray.ArraySize);
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_SRV_DIMENSION_TEXTURE3D:
						ImGui::PushID("Texture3D");
						if (ImGui::TreeNode("Texture3D Properties"))
						{
							ImGui::Text("Most Detailed Mip: %u", viewDescSpecific.Texture3D.MostDetailedMip);
							ImGui::Text("Mip Levels: %u", viewDescSpecific.Texture3D.MipLevels);
							ImGui::Text("Resource Min LOD Clamp: %.2f", viewDescSpecific.Texture3D.ResourceMinLODClamp);
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_SRV_DIMENSION_TEXTURECUBE:
						ImGui::PushID("TextureCube");
						if (ImGui::TreeNode("TextureCube Properties"))
						{
							ImGui::Text("Most Detailed Mip: %u", viewDescSpecific.TextureCube.MostDetailedMip);
							ImGui::Text("Mip Levels: %u", viewDescSpecific.TextureCube.MipLevels);
							ImGui::Text("Resource Min LOD Clamp: %.2f", viewDescSpecific.TextureCube.ResourceMinLODClamp);
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_SRV_DIMENSION_TEXTURECUBEARRAY:
						ImGui::PushID("TextureCubeArray");
						if (ImGui::TreeNode("TextureCubeArray Properties"))
						{
							ImGui::Text("Most Detailed Mip: %u", viewDescSpecific.TextureCubeArray.MostDetailedMip);
							ImGui::Text("Mip Levels: %u", viewDescSpecific.TextureCubeArray.MipLevels);
							ImGui::Text("First 2D Array Face: %u", viewDescSpecific.TextureCubeArray.First2DArrayFace);
							ImGui::Text("Num Cubes: %u", viewDescSpecific.TextureCubeArray.NumCubes);
							ImGui::Text("Resource Min LOD Clamp: %.2f", viewDescSpecific.TextureCubeArray.ResourceMinLODClamp);
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE:
						ImGui::PushID("RaytracingAS");
						if (ImGui::TreeNode("Raytracing AS Properties"))
						{
							ImGui::Text("Location: 0x%llX", viewDescSpecific.RaytracingAccelerationStructure.Location);
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_SRV_DIMENSION_UNKNOWN:
					default:
						ImGui::TextDisabled("(No dimension-specific details)");
						break;
					}
					ImGui::PopID();
				}
				else if constexpr (std::is_same_v<T, D3D12_UNORDERED_ACCESS_VIEW_DESC>)
				{
					ImGui::Text("UAV Details:");
					ImGui::Text("Format: %s", DXGIFormatToString(viewDescSpecific.Format));
					ImGui::Text("View Dimension: %s", D3D12UavDimensionToString(viewDescSpecific.ViewDimension));

					ImGui::PushID("UAV");
					switch (viewDescSpecific.ViewDimension)
					{
					case D3D12_UAV_DIMENSION_BUFFER:
						ImGui::PushID("Buffer");
						if (ImGui::TreeNode("Buffer Properties"))
						{
							ImGui::Text("First Element: %llu", viewDescSpecific.Buffer.FirstElement);
							ImGui::Text("Num Elements: %u", viewDescSpecific.Buffer.NumElements);
							ImGui::Text("Structure Byte Stride: %u", viewDescSpecific.Buffer.StructureByteStride);
							ImGui::Text("Counter Offset In Bytes: %llu", viewDescSpecific.Buffer.CounterOffsetInBytes);
							ImGui::Text("Flags: %s", D3D12BufferUavFlagsToString(viewDescSpecific.Buffer.Flags).c_str());
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_UAV_DIMENSION_TEXTURE1D:
						ImGui::PushID("Texture1D");
						if (ImGui::TreeNode("Texture1D Properties"))
						{
							ImGui::Text("Mip Slice: %u", viewDescSpecific.Texture1D.MipSlice);
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_UAV_DIMENSION_TEXTURE1DARRAY:
						ImGui::PushID("Texture1DArray");
						if (ImGui::TreeNode("Texture1DArray Properties"))
						{
							ImGui::Text("Mip Slice: %u", viewDescSpecific.Texture1DArray.MipSlice);
							ImGui::Text("First Array Slice: %u", viewDescSpecific.Texture1DArray.FirstArraySlice);
							ImGui::Text("Array Size: %u", viewDescSpecific.Texture1DArray.ArraySize);
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_UAV_DIMENSION_TEXTURE2D:
						ImGui::PushID("Texture2D");
						if (ImGui::TreeNode("Texture2D Properties"))
						{
							ImGui::Text("Mip Slice: %u", viewDescSpecific.Texture2D.MipSlice);
							ImGui::Text("Plane Slice: %u", viewDescSpecific.Texture2D.PlaneSlice);
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_UAV_DIMENSION_TEXTURE2DARRAY:
						ImGui::PushID("Texture2DArray");
						if (ImGui::TreeNode("Texture2DArray Properties"))
						{
							ImGui::Text("Mip Slice: %u", viewDescSpecific.Texture2DArray.MipSlice);
							ImGui::Text("First Array Slice: %u", viewDescSpecific.Texture2DArray.FirstArraySlice);
							ImGui::Text("Array Size: %u", viewDescSpecific.Texture2DArray.ArraySize);
							ImGui::Text("Plane Slice: %u", viewDescSpecific.Texture2DArray.PlaneSlice);
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_UAV_DIMENSION_TEXTURE3D:
						ImGui::PushID("Texture3D");
						if (ImGui::TreeNode("Texture3D Properties"))
						{
							ImGui::Text("Mip Slice: %u", viewDescSpecific.Texture3D.MipSlice);
							ImGui::Text("First W Slice: %u", viewDescSpecific.Texture3D.FirstWSlice);
							ImGui::Text("W Size: %u", viewDescSpecific.Texture3D.WSize);
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_UAV_DIMENSION_UNKNOWN:
					default:
						ImGui::TextDisabled("(No dimension-specific details)");
						break;
					}
					ImGui::PopID();
				}
				else if constexpr (std::is_same_v<T, D3D12_SAMPLER_DESC>)
				{
					ImGui::PushID("Sampler");
					RenderSamplerDesc(viewDescSpecific);
					ImGui::PopID();
				}
				else if constexpr (std::is_same_v<T, D3D12_RENDER_TARGET_VIEW_DESC>)
				{
					ImGui::Text("RTV Details:");
					ImGui::Text("Format: %s", DXGIFormatToString(viewDescSpecific.Format));
					ImGui::Text("View Dimension: %s", D3D12RtvDimensionToString(viewDescSpecific.ViewDimension));

					ImGui::PushID("RTV");
					switch (viewDescSpecific.ViewDimension)
					{
					case D3D12_RTV_DIMENSION_BUFFER:
						ImGui::PushID("Buffer");
						if (ImGui::TreeNode("Buffer Properties"))
						{
							ImGui::Text("First Element: %llu", viewDescSpecific.Buffer.FirstElement);
							ImGui::Text("Num Elements: %u", viewDescSpecific.Buffer.NumElements);
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_RTV_DIMENSION_TEXTURE1D:
						ImGui::PushID("Texture1D");
						if (ImGui::TreeNode("Texture1D Properties"))
						{
							ImGui::Text("Mip Slice: %u", viewDescSpecific.Texture1D.MipSlice);
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_RTV_DIMENSION_TEXTURE1DARRAY:
						ImGui::PushID("Texture1DArray");
						if (ImGui::TreeNode("Texture1DArray Properties"))
						{
							ImGui::Text("Mip Slice: %u", viewDescSpecific.Texture1DArray.MipSlice);
							ImGui::Text("First Array Slice: %u", viewDescSpecific.Texture1DArray.FirstArraySlice);
							ImGui::Text("Array Size: %u", viewDescSpecific.Texture1DArray.ArraySize);
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_RTV_DIMENSION_TEXTURE2D:
						ImGui::PushID("Texture2D");
						if (ImGui::TreeNode("Texture2D Properties"))
						{
							ImGui::Text("Mip Slice: %u", viewDescSpecific.Texture2D.MipSlice);
							ImGui::Text("Plane Slice: %u", viewDescSpecific.Texture2D.PlaneSlice);
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_RTV_DIMENSION_TEXTURE2DARRAY:
						ImGui::PushID("Texture2DArray");
						if (ImGui::TreeNode("Texture2DArray Properties"))
						{
							ImGui::Text("Mip Slice: %u", viewDescSpecific.Texture2DArray.MipSlice);
							ImGui::Text("First Array Slice: %u", viewDescSpecific.Texture2DArray.FirstArraySlice);
							ImGui::Text("Array Size: %u", viewDescSpecific.Texture2DArray.ArraySize);
							ImGui::Text("Plane Slice: %u", viewDescSpecific.Texture2DArray.PlaneSlice);
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_RTV_DIMENSION_TEXTURE2DMS:
						ImGui::PushID("Texture2DMS");
						if (ImGui::TreeNode("Texture2DMS Properties"))
						{
							ImGui::TextUnformatted("(No specific fields)");
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY:
						ImGui::PushID("Texture2DMSArray");
						if (ImGui::TreeNode("Texture2DMSArray Properties"))
						{
							ImGui::Text("First Array Slice: %u", viewDescSpecific.Texture2DMSArray.FirstArraySlice);
							ImGui::Text("Array Size: %u", viewDescSpecific.Texture2DMSArray.ArraySize);
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_RTV_DIMENSION_TEXTURE3D:
						ImGui::PushID("Texture3D");
						if (ImGui::TreeNode("Texture3D Properties"))
						{
							ImGui::Text("Mip Slice: %u", viewDescSpecific.Texture3D.MipSlice);
							ImGui::Text("First W Slice: %u", viewDescSpecific.Texture3D.FirstWSlice);
							ImGui::Text("W Size: %u", viewDescSpecific.Texture3D.WSize);
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_RTV_DIMENSION_UNKNOWN:
					default:
						ImGui::TextDisabled("(No dimension-specific details)");
						break;
					}
					ImGui::PopID();
				}
				else if constexpr (std::is_same_v<T, D3D12_DEPTH_STENCIL_VIEW_DESC>)
				{
					ImGui::Text("DSV Details:");
					ImGui::Text("Format: %s", DXGIFormatToString(viewDescSpecific.Format));
					ImGui::Text("View Dimension: %s", D3D12DsvDimensionToString(viewDescSpecific.ViewDimension));
					ImGui::Text("Flags: %s", D3D12DsvFlagsToString(viewDescSpecific.Flags).c_str());

					ImGui::PushID("DSV");
					switch (viewDescSpecific.ViewDimension)
					{
					case D3D12_DSV_DIMENSION_TEXTURE1D:
						ImGui::PushID("Texture1D");
						if (ImGui::TreeNode("Texture1D Properties"))
						{
							ImGui::Text("Mip Slice: %u", viewDescSpecific.Texture1D.MipSlice);
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_DSV_DIMENSION_TEXTURE1DARRAY:
						ImGui::PushID("Texture1DArray");
						if (ImGui::TreeNode("Texture1DArray Properties"))
						{
							ImGui::Text("Mip Slice: %u", viewDescSpecific.Texture1DArray.MipSlice);
							ImGui::Text("First Array Slice: %u", viewDescSpecific.Texture1DArray.FirstArraySlice);
							ImGui::Text("Array Size: %u", viewDescSpecific.Texture1DArray.ArraySize);
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_DSV_DIMENSION_TEXTURE2D:
						ImGui::PushID("Texture2D");
						if (ImGui::TreeNode("Texture2D Properties"))
						{
							ImGui::Text("Mip Slice: %u", viewDescSpecific.Texture2D.MipSlice);
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_DSV_DIMENSION_TEXTURE2DARRAY:
						ImGui::PushID("Texture2DArray");
						if (ImGui::TreeNode("Texture2DArray Properties"))
						{
							ImGui::Text("Mip Slice: %u", viewDescSpecific.Texture2DArray.MipSlice);
							ImGui::Text("First Array Slice: %u", viewDescSpecific.Texture2DArray.FirstArraySlice);
							ImGui::Text("Array Size: %u", viewDescSpecific.Texture2DArray.ArraySize);
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_DSV_DIMENSION_TEXTURE2DMS:
						ImGui::PushID("Texture2DMS");
						if (ImGui::TreeNode("Texture2DMS Properties"))
						{
							ImGui::TextUnformatted("(No specific fields)");
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY:
						ImGui::PushID("Texture2DMSArray");
						if (ImGui::TreeNode("Texture2DMSArray Properties"))
						{
							ImGui::Text("First Array Slice: %u", viewDescSpecific.Texture2DMSArray.FirstArraySlice);
							ImGui::Text("Array Size: %u", viewDescSpecific.Texture2DMSArray.ArraySize);
							ImGui::TreePop();
						}
						ImGui::PopID();
						break;
					case D3D12_DSV_DIMENSION_UNKNOWN:
					default:
						ImGui::TextDisabled("(No dimension-specific details)");
						break;
					}
					ImGui::PopID();
				}
				ImGui::PopID();
			}, descInfo.desc);
	}

	void RenderDescriptorInfoByHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle, Char const* label, DescriptorTracker const& descriptorTracker)
	{
		if (handle.ptr == 0)
		{
			ImGui::Text("%s: <Null Handle>", label);
			return;
		}

		ImGui::Text("%s: 0x%llX", label, handle.ptr);

		auto const& [descHeapInfo, index] = descriptorTracker.FindDescriptorByCpuHandle(handle);
		if (index >= 0 && descHeapInfo)
		{
			DescriptorInfo const& descInfo = descHeapInfo->descriptors[index];
			ImGui::Text("  Heap: obj#%llu", descHeapInfo->heapId);
			ImGui::Text("  Index: %d", index);
			if (ImGui::TreeNode("Descriptor Details"))
			{
				RenderDescriptorInfo(descInfo);
				ImGui::TreePop();
			}
		}
		else
		{
			ImGui::Text("  <Descriptor Info Not Found>");
		}
	}

	void RenderDescriptorInfoByHandle(D3D12_GPU_DESCRIPTOR_HANDLE handle, Char const* label, DescriptorTracker const& descriptorTracker)
	{
		if (handle.ptr == NULL)
		{
			ImGui::Text("%s: <Null Handle>", label);
			return;
		}

		ImGui::Text("%s", label);

		auto const& [descHeapInfo, index] = descriptorTracker.FindDescriptorByGpuHandle(handle);
		if (index >= 0 && descHeapInfo)
		{
			DescriptorInfo const& descInfo = descHeapInfo->descriptors[index];
			ImGui::Text("  Heap: obj#%llu", descHeapInfo->heapId);
			ImGui::Text("  Index: %d", index);
			if (ImGui::TreeNode("Descriptor Details"))
			{
				RenderDescriptorInfo(descInfo);
				ImGui::TreePop();
			}
		}
		else
		{
			ImGui::Text("  <Descriptor Info Not Found>");
		}
	}

	TrackedObjectInfo const* RenderObjectInfoByID(ObjectID id, ObjectTracker const& tracker, Char const* label)
	{
		TrackedObjectInfo const* info = tracker.GetObjectInfo(id);
		if (label) ImGui::TextUnformatted(label);
		if (label) ImGui::SameLine();

		if (info)
		{
			ImGui::Text("obj#%llu (%s)", info->objectId, info->objectDebugName.c_str());
		}
		else if (id == InvalidObjectID)
		{
			ImGui::Text("<Invalid ObjectID>");
		}
		else
		{
			ImGui::Text("obj#%llu (<Unknown>)", id);
		}
		return info;
	}

	void RenderResourceDesc(ResourceDesc const& desc)
	{
		ImGui::SeparatorText("Heap Info");
		if (desc.heapOffset > 0 || desc.heapId != InvalidObjectID)
		{
			if(desc.heapId != InvalidObjectID) ImGui::Text("Heap ID: obj#%llu", desc.heapId);
			else ImGui::Text("Heap ID: <unknown>");

			ImGui::Text("Heap Offset: %llu", desc.heapOffset);
		}
		else
		{
			ImGui::Text("Heap Type: %s", D3D12HeapTypeToString(desc.heapProperties.Type));
			ImGui::Text("Heap Flags: 0x%08X", desc.heapFlags);
		}

		ImGui::SeparatorText("Resource Info");

		ImGui::Text("Dimension: %s", D3D12ResourceDimensionToString(desc.resourceDesc.Dimension));
		ImGui::Text("Width: %llu", desc.resourceDesc.Width);
		ImGui::Text("Height: %u", desc.resourceDesc.Height);
		ImGui::Text("Depth/Array Size: %u", desc.resourceDesc.DepthOrArraySize);
		ImGui::Text("Mip Levels: %u", desc.resourceDesc.MipLevels);
		ImGui::Text("Format: %s", DXGIFormatToString(desc.resourceDesc.Format));
		ImGui::Text("Sample Count: %u", desc.resourceDesc.SampleDesc.Count);
		ImGui::Text("Sample Quality: %u", desc.resourceDesc.SampleDesc.Quality);
		ImGui::Text("Layout: %s", D3D12TextureLayoutToString(desc.resourceDesc.Layout));
		ImGui::Text("Initial State: %s", D3D12ResourceStateToString(desc.initialResourceState).c_str());

		if (desc.optimizedClearValue.has_value())
		{
			D3D12_CLEAR_VALUE const& clearVal = desc.optimizedClearValue.value();
			ImGui::SeparatorText("Optimized Clear Value");
			ImGui::Text("Format: %s", DXGIFormatToString(clearVal.Format));
			if (clearVal.Format == DXGI_FORMAT_D32_FLOAT_S8X24_UINT ||
				clearVal.Format == DXGI_FORMAT_D32_FLOAT ||
				clearVal.Format == DXGI_FORMAT_D24_UNORM_S8_UINT ||
				clearVal.Format == DXGI_FORMAT_D16_UNORM)
			{
				ImGui::Text("Depth/Stencil: %.2f / %u", clearVal.DepthStencil.Depth, clearVal.DepthStencil.Stencil);
			}
			else
			{
				ImGui::Text("Color: %.2f, %.2f, %.2f, %.2f", clearVal.Color[0], clearVal.Color[1], clearVal.Color[2], clearVal.Color[3]);
			}
		}
	}

	void RenderDescriptorHeapDesc(DescriptorHeapDesc const& desc)
	{
		ImGui::Text("Type: %s", D3D12DescriptorHeapTypeToString(desc.heapDesc.Type));
		ImGui::Text("Num Descriptors: %u", desc.heapDesc.NumDescriptors);
		ImGui::Text("Flags: %s", D3D12DescriptorHeapFlagsToString(desc.heapDesc.Flags));
		ImGui::Text("Node Mask: %u", desc.heapDesc.NodeMask);
	}

	void RenderQueueDesc(QueueDesc const& desc)
	{
		ImGui::Text("Type: %s", D3D12CommandListTypeToString(desc.queueDesc.Type));
		ImGui::Text("Priority: %d", desc.queueDesc.Priority);
		ImGui::Text("Flags: 0x%08X", desc.queueDesc.Flags);
		ImGui::Text("Node Mask: %u", desc.queueDesc.NodeMask);
	}

	void RenderListDesc(ListDesc const& desc)
	{
		ImGui::Text("Type: %s", D3D12CommandListTypeToString(desc.type));
		ImGui::Text("Allocator ID: obj#%llu", desc.commandAllocatorId);
		ImGui::Text("Initial State ID: obj#%llu", desc.initialStateId);
		ImGui::Text("Node Mask: %u", desc.nodeMask);
	}

	void RenderAllocatorDesc(AllocatorDesc const& desc)
	{
		ImGui::Text("Type: %s", D3D12CommandListTypeToString(desc.type));
	}

	void RenderFenceDesc(FenceDesc const& desc)
	{
		ImGui::Text("Initial Value: %llu", desc.intialValue);
		ImGui::Text("Flags: 0x%08X", desc.flags);
	}

	void RenderRootSignatureDesc(RootSignatureDesc const& desc)
	{
		ImGui::Text("Num Parameters: %u", (Uint32)desc.Parameters.size());
		ImGui::Text("Num Static Samplers: %u", (Uint32)desc.StaticSamplers.size());
		ImGui::Text("Flags: %s", D3D12RootSignatureFlagsToString(desc.Flags).c_str());
		ImGui::Separator();

		// Root Parameters
		if (!desc.Parameters.empty())
		{
			if (ImGui::TreeNodeEx("Root Parameters", ImGuiTreeNodeFlags_DefaultOpen))
			{
				for (Uint32 i = 0; i < desc.Parameters.size(); ++i)
				{
					ImGui::PushID(i);
					RootParameter const& param = desc.Parameters[i];
					if (ImGui::TreeNodeEx(std::format("[{}]", i).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::Text("Parameter Type: %s", D3D12RootParameterTypeToString(param.ParameterType));
						ImGui::Text("Shader Visibility: %s", D3D12ShaderVisibilityToString(param.ShaderVisibility));
						ImGui::SeparatorText("Details");
						ImGui::Indent();

						switch (param.ParameterType)
						{
						case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
						{
							RootDescriptorTable const& table = param.DescriptorTable;
							ImGui::Text("Num Descriptor Ranges: %u", table.Ranges.size());
							if (ImGui::TreeNodeEx("Descriptor Ranges", ImGuiTreeNodeFlags_DefaultOpen))
							{
								for (Uint32 r = 0; r < table.Ranges.size(); ++r)
								{
									ImGui::PushID(r);
									DescriptorRange const& range = table.Ranges[r];
									if (ImGui::TreeNodeEx(std::format("Range [{}]", r).c_str()))
									{
										ImGui::Text("Range Type: %s", D3D12DescriptorRangeTypeToString(range.RangeType));
										ImGui::Text("Num Descriptors: %u", range.NumDescriptors);
										ImGui::Text("Base Shader Register: %u", range.BaseShaderRegister);
										ImGui::Text("Register Space: %u", range.RegisterSpace);
										ImGui::Text("Offset In Descriptors From Table Start: %u", range.OffsetInDescriptorsFromTableStart);
										ImGui::TreePop();
									}
									ImGui::PopID();
								}
								ImGui::TreePop();
							}
							break;
						}
						case D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS:
						{
							auto const& constants = param.Constants;
							ImGui::Text("Shader Register: %u", constants.ShaderRegister);
							ImGui::Text("Register Space: %u", constants.RegisterSpace);
							ImGui::Text("Num 32Bit Values: %u", constants.Num32BitValues);
							break;
						}
						case D3D12_ROOT_PARAMETER_TYPE_CBV:
						case D3D12_ROOT_PARAMETER_TYPE_SRV:
						case D3D12_ROOT_PARAMETER_TYPE_UAV:
						{
							auto const& descriptor = param.Descriptor;
							ImGui::Text("Shader Register: %u", descriptor.ShaderRegister);
							ImGui::Text("Register Space: %u", descriptor.RegisterSpace);
							break;
						}
						default:
							ImGui::Text("Unknown Parameter Type");
							break;
						}

						ImGui::Unindent();
						ImGui::TreePop();
					}
					ImGui::PopID();
				}
				ImGui::TreePop();
			}
		}

		// Static Samplers
		if (!desc.StaticSamplers.empty())
		{
			ImGui::Separator();
			if (ImGui::TreeNodeEx("Static Samplers", ImGuiTreeNodeFlags_DefaultOpen))
			{
				for (Uint32 i = 0; i < desc.StaticSamplers.size(); ++i)
				{
					ImGui::PushID(1000 + i);
					StaticSamplerDesc const& sampler = desc.StaticSamplers[i];
					if (ImGui::TreeNodeEx(std::format("[{}]", i).c_str()))
					{
						ImGui::Text("Shader Register: %u", sampler.Desc.ShaderRegister);
						ImGui::Text("Register Space: %u", sampler.Desc.RegisterSpace);
						ImGui::Text("Shader Visibility: %s", D3D12ShaderVisibilityToString(sampler.Desc.ShaderVisibility));
						ImGui::Separator();
						RenderSamplerDesc(sampler.Desc, "Static Sampler");
						ImGui::TreePop();
					}
					ImGui::PopID();
				}
				ImGui::TreePop();
			}
		}
	}

	void RenderGraphicsPSODetails(GraphicsPSODescStorage const& desc, ObjectTracker const& objectTracker)
	{
		details::RenderGraphicsPSODetails(desc, objectTracker);
	}

	void RenderComputePSODetails(ComputePSODescStorage const& desc, ObjectTracker const& objectTracker)
	{
		details::RenderComputePSODetails(desc, objectTracker);
	}

	void RenderStreamPSODetails(StreamPSODescStorage const& desc, ObjectTracker const& objectTracker)
	{
		if (desc.CS.IsValid())
		{
			details::RenderComputePSODetails(desc, objectTracker);
		}
		else if (desc.VS.IsValid())
		{
			details::RenderGraphicsPSODetails(desc, objectTracker);
		}
		else
		{
			if (desc.RootSignatureId != InvalidObjectID)
			{
				TrackedObjectInfo const* rsInfo = objectTracker.GetObjectInfo(desc.RootSignatureId);
				if (rsInfo)
					ImGui::Text("Root Signature: obj#%llu (%s)", rsInfo->objectId, rsInfo->objectDebugName.c_str());
				else
					ImGui::Text("Root Signature: obj#%llu (<Unknown>)", desc.RootSignatureId);
			}
			else
			{
				ImGui::TextUnformatted("Root Signature: NULL");
			}

			ImGui::Text("Amplification Shader: %s", desc.AS.IsValid() ? "Set" : "NULL");
			ImGui::Text("Mesh Shader: %s", desc.MS.IsValid() ? "Set" : "NULL");
			ImGui::Text("Pixel Shader: %s", desc.PS.IsValid() ? "Set" : "NULL");

			ImGui::Separator();
			if (ImGui::TreeNodeEx("Stream Output", ImGuiTreeNodeFlags_None))
			{
				StreamOutputDescStorage const& so = desc.StreamOutput;
				ImGui::Text("Num Entries: %u", so.SODeclaration.size());
				ImGui::Text("Num Strides: %u", so.BufferStrides.size());
				ImGui::Text("Rasterized Stream: %u", so.RasterizedStream);
				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("Blend State", ImGuiTreeNodeFlags_None))
			{
				D3D12_BLEND_DESC const& blend = desc.BlendState;
				ImGui::Text("Alpha To Coverage Enable: %s", blend.AlphaToCoverageEnable ? "TRUE" : "FALSE");
				ImGui::Text("Independent Blend Enable: %s", blend.IndependentBlendEnable ? "TRUE" : "FALSE");

				if (ImGui::TreeNodeEx("Render Target Blends", ImGuiTreeNodeFlags_None))
				{
					for (Uint i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
					{
						if (!blend.IndependentBlendEnable && i >= 1)
							break;

						ImGui::PushID(i);
						if (ImGui::TreeNodeEx(std::format("RT[{}]", i).c_str()))
						{
							D3D12_RENDER_TARGET_BLEND_DESC const& rtBlend = blend.RenderTarget[i];
							ImGui::Text("Blend Enable: %s", rtBlend.BlendEnable ? "TRUE" : "FALSE");
							ImGui::Text("Logic Op Enable: %s", rtBlend.LogicOpEnable ? "TRUE" : "FALSE");
							ImGui::Text("Src Blend: %s", D3D12BlendToString(rtBlend.SrcBlend));
							ImGui::Text("Dest Blend: %s", D3D12BlendToString(rtBlend.DestBlend));
							ImGui::Text("Blend Op: %s", D3D12BlendOpToString(rtBlend.BlendOp));
							ImGui::Text("Src Blend Alpha: %s", D3D12BlendToString(rtBlend.SrcBlendAlpha));
							ImGui::Text("Dest Blend Alpha: %s", D3D12BlendToString(rtBlend.DestBlendAlpha));
							ImGui::Text("Blend Op Alpha: %s", D3D12BlendOpToString(rtBlend.BlendOpAlpha));
							ImGui::Text("Logic Op: %s", D3D12LogicOpToString(rtBlend.LogicOp));
							ImGui::Text("Render Target Write Mask: 0x%02X", rtBlend.RenderTargetWriteMask);
							ImGui::TreePop();
						}
						ImGui::PopID();
					}
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}

			ImGui::Text("SampleMask: 0x%X", desc.SampleMask);

			if (ImGui::TreeNodeEx("Rasterizer State", ImGuiTreeNodeFlags_None))
			{
				D3D12_RASTERIZER_DESC const& rasterDesc = desc.RasterizerState;
				ImGui::Text("Fill Mode: %s", D3D12FillModeToString(rasterDesc.FillMode));
				ImGui::Text("Cull Mode: %s", D3D12CullModeToString(rasterDesc.CullMode));
				ImGui::Text("Front Counter Clockwise: %s", rasterDesc.FrontCounterClockwise ? "TRUE" : "FALSE");
				ImGui::Text("Depth Bias: %d", rasterDesc.DepthBias);
				ImGui::Text("Depth Bias Clamp: %.2f", rasterDesc.DepthBiasClamp);
				ImGui::Text("Slope Scaled Depth Bias: %.2f", rasterDesc.SlopeScaledDepthBias);
				ImGui::Text("Depth Clip Enable: %s", rasterDesc.DepthClipEnable ? "TRUE" : "FALSE");
				ImGui::Text("Multisample Enable: %s", rasterDesc.MultisampleEnable ? "TRUE" : "FALSE");
				ImGui::Text("Antialiased Line Enable: %s", rasterDesc.AntialiasedLineEnable ? "TRUE" : "FALSE");
				ImGui::Text("Forced Sample Count: %u", rasterDesc.ForcedSampleCount);
				ImGui::Text("Conservative Raster: %s", D3D12ConservativeRasterizationModeToString(rasterDesc.ConservativeRaster));
				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("Depth Stencil State", ImGuiTreeNodeFlags_None))
			{
				D3D12_DEPTH_STENCIL_DESC const& ds = desc.DepthStencilState;
				ImGui::Text("Depth Enable: %s", ds.DepthEnable ? "TRUE" : "FALSE");
				ImGui::Text("Depth Write Mask: %s", D3D12DepthWriteMaskToString(ds.DepthWriteMask));
				ImGui::Text("Depth Func: %s", D3D12ComparisonFuncToString(ds.DepthFunc));
				ImGui::Separator();
				ImGui::Text("Stencil Enable: %s", ds.StencilEnable ? "TRUE" : "FALSE");
				ImGui::Text("Stencil Read Mask: 0x%02X", ds.StencilReadMask);
				ImGui::Text("Stencil Write Mask: 0x%02X", ds.StencilWriteMask);

				if (ImGui::TreeNode("Front Face Stencil"))
				{
					D3D12_DEPTH_STENCILOP_DESC const& front = ds.FrontFace;
					ImGui::Text("Stencil Fail Op: %s", D3D12StencilOpToString(front.StencilFailOp));
					ImGui::Text("Stencil Depth Fail Op: %s", D3D12StencilOpToString(front.StencilDepthFailOp));
					ImGui::Text("Stencil Pass Op: %s", D3D12StencilOpToString(front.StencilPassOp));
					ImGui::Text("Stencil Func: %s", D3D12ComparisonFuncToString(front.StencilFunc));
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Back Face Stencil"))
				{
					D3D12_DEPTH_STENCILOP_DESC const& back = ds.BackFace;
					ImGui::Text("Stencil Fail Op: %s", D3D12StencilOpToString(back.StencilFailOp));
					ImGui::Text("Stencil Depth Fail Op: %s", D3D12StencilOpToString(back.StencilDepthFailOp));
					ImGui::Text("Stencil Pass Op: %s", D3D12StencilOpToString(back.StencilPassOp));
					ImGui::Text("Stencil Func: %s", D3D12ComparisonFuncToString(back.StencilFunc));
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("Input Layout", ImGuiTreeNodeFlags_None))
			{
				std::vector<InputElementDescStorage> const& inputLayoutElements = desc.InputLayoutElements;
				ImGui::Text("Num Elements: %u", inputLayoutElements.size());
				if (!inputLayoutElements.empty())
				{
					if (ImGui::TreeNodeEx("Elements", ImGuiTreeNodeFlags_None))
					{
						for (Uint32 i = 0; i < inputLayoutElements.size(); ++i)
						{
							ImGui::PushID(i);
							InputElementDescStorage const& elem = inputLayoutElements[i];
							if (ImGui::TreeNodeEx(std::format("[{}] {}", i, !elem.SemanticName.empty() ? elem.SemanticName.c_str() : "<NULL>").c_str()))
							{
								ImGui::Text("Semantic Name: %s", !elem.SemanticName.empty() ? elem.SemanticName.c_str() : "NULL");
								ImGui::Text("Semantic Index: %u", elem.SemanticIndex);
								ImGui::Text("Format: %s", DXGIFormatToString(elem.Format));
								ImGui::Text("Input Slot: %u", elem.InputSlot);
								ImGui::Text("Aligned Byte Offset: %u", elem.AlignedByteOffset);
								ImGui::Text("Input Slot Class: %s", D3D12InputClassificationToString(elem.InputSlotClass));
								ImGui::Text("Instance Data Step Rate: %u", elem.InstanceDataStepRate);
								ImGui::TreePop();
							}
							ImGui::PopID();
						}
						ImGui::TreePop();
					}
				}
				else
					ImGui::TextUnformatted("Elements: (None)");
				ImGui::TreePop();
			}

			ImGui::Text("IB Strip Cut Value: %s", D3D12IndexBufferStripCutValueToString(desc.IBStripCutValue));
			ImGui::Text("Primitive Topology Type: %s", D3D12PrimitiveTopologyTypeToString(desc.PrimitiveTopologyType));

			if (ImGui::TreeNodeEx("PSO Render Target Formats", ImGuiTreeNodeFlags_None))
			{
				ImGui::Text("Num Render Targets: %u", desc.NumRenderTargets);
				for (Uint32 i = 0; i < desc.NumRenderTargets; ++i)
					ImGui::Text("  RTVFormat[%u]: %s", i, DXGIFormatToString(desc.RTVFormats[i]));
				if (desc.NumRenderTargets == 0)
					ImGui::TextUnformatted("  (No Render Targets)");
				ImGui::TreePop();
			}

			ImGui::Text("DSV Format: %s", DXGIFormatToString(desc.DSVFormat));

			if (ImGui::TreeNodeEx("Sample Desc", ImGuiTreeNodeFlags_None))
			{
				ImGui::Text("Count: %u", desc.SampleDesc.Count);
				ImGui::Text("Quality: %u", desc.SampleDesc.Quality);
				ImGui::TreePop();
			}

			ImGui::Text("NodeMask: 0x%X", desc.NodeMask);
			if (ImGui::TreeNode("Cached Pipeline State"))
			{
				ImGui::Text("Cached Blob: %s", desc.CachedPSO.IsValid() ? "Set" : "NULL");
				ImGui::Text("Size: %zu bytes", desc.CachedPSO.Blob.size());
				ImGui::TreePop();
			}
			ImGui::Text("Flags: %s", D3D12PipelineStateFlagsToString(desc.Flags));
		}
	}

	void RenderCommandSignatureDesc(CommandSignatureDesc const& desc, ObjectTracker const& objectTracker)
	{
		D3D12_COMMAND_SIGNATURE_DESC const& sigDesc = desc.cmdSignatureDesc;
		ImGui::SeparatorText("Command Signature Details");
		if (ImGui::BeginTable("CommandSignatureDetailsTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
		{
			ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 150.0f);
			ImGui::TableSetupColumn("Value");
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Byte Stride");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%u", sigDesc.ByteStride);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Number of Arguments");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%u", sigDesc.NumArgumentDescs);

			if (sigDesc.NumArgumentDescs > 0 && sigDesc.pArgumentDescs)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("Arguments");
				ImGui::TableSetColumnIndex(1);
				ImGui::Indent();
				for (Uint32 i = 0; i < sigDesc.NumArgumentDescs; ++i)
				{
					RenderIndirectArgumentDesc(i, sigDesc.pArgumentDescs[i]);
				}
				ImGui::Unindent();
			}

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Node Mask");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("0x%X", sigDesc.NodeMask);

			TrackedObjectInfo const* rootSigInfo = objectTracker.GetObjectInfo(desc.rootSignatureId);
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Root Signature");
			ImGui::TableSetColumnIndex(1);
			if (rootSigInfo)
			{
				ImGui::Text("obj#%llu (%s)", desc.rootSignatureId, rootSigInfo->objectDebugName.c_str());
			}
			else
			{
				ImGui::Text("obj#%llu (<Unknown>)", desc.rootSignatureId);
			}

			ImGui::EndTable();
		}
	}

	void RenderHeapDesc(HeapDesc const& desc)
	{
		ImGui::Text("Size In Bytes: %llu", desc.heapDesc.SizeInBytes);
		ImGui::Text("Alignment: %llu", desc.heapDesc.Alignment);
		ImGui::Text("Flags: %s", D3D12HeapFlagsToString(desc.heapDesc.Flags).c_str());
		ImGui::Text("Heap Type: %s", D3D12HeapTypeToString(desc.heapDesc.Properties.Type));
	}
	
	void RenderStateSubobjectDesc(StateObjectDesc const& desc, StateObjectDesc::Subobject const& subobject, ObjectTracker const& objectTracker)
	{
		switch (subobject.type)
		{
		case D3D12_STATE_SUBOBJECT_TYPE_STATE_OBJECT_CONFIG:
		{
			auto* config = reinterpret_cast<D3D12_STATE_OBJECT_CONFIG const*>(subobject.data.data());
			ImGui::Text("Flags: 0x%X", config->Flags);
			break;
		}
		case D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE:
		case D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE:
		{
			IUnknown* rootSigUnk = subobject.interfaceRef.Get();
			TrackedObjectInfo const* info = objectTracker.GetObjectInfo(rootSigUnk);
			if (info)
			{
				ImGui::Text("Root Signature: obj#%llu (%s)", info->objectId, info->objectDebugName.c_str());
			}
			else
			{
				ImGui::Text("Root Signature: %p <Unknown/Untracked>", rootSigUnk);
			}
			break;
		}
		case D3D12_STATE_SUBOBJECT_TYPE_NODE_MASK:
		{
			auto* nodeMask = reinterpret_cast<UINT const*>(subobject.data.data());
			ImGui::Text("Node Mask: 0x%X", *nodeMask);
			break;
		}
		case D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY:
		{
			ImGui::Text("Num Exports: %zu", subobject.ownedExportStructs.size());
			for (Uint32 i = 0; i < subobject.ownedExportStructs.size(); ++i)
			{
				auto const& exp = subobject.ownedExportStructs[i];
				ImGui::Text("Export[%u]: %s", i, ToString(exp.Name).c_str());
			}
			break;
		}
		case D3D12_STATE_SUBOBJECT_TYPE_EXISTING_COLLECTION:
		{
			IUnknown* collUnk = subobject.interfaceRef.Get();
			ImGui::Text("Existing Collection: %p", collUnk);
			ImGui::Text("Num Exports: %zu", subobject.ownedExportStructs.size());
			for (Uint32 i = 0; i < subobject.ownedExportStructs.size(); ++i)
			{
				auto const& exp = subobject.ownedExportStructs[i];
				ImGui::Text("Export[%u]: %s", i, ToString(exp.Name).c_str());
			}
			break;
		}
		case D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION:
		{
			if (subobject.associatedSubobjectIndex != -1 && subobject.associatedSubobjectIndex < desc.subobjects.size())
			{
				const auto& associatedSubobject = desc.subobjects[subobject.associatedSubobjectIndex];
				ImGui::Text("Subobject Association (Index %d): Type %s",
					subobject.associatedSubobjectIndex,
					D3D12StateSubobjectTypeToString(associatedSubobject.type));
			}
			else
			{
				ImGui::Text("Subobject Association: <None or Invalid>");
			}

			ImGui::Text("Num Exports: %zu", subobject.ownedAssociationExportPointers.size());
			for (Uint32 i = 0; i < subobject.ownedAssociationExportPointers.size(); ++i)
			{
				ImGui::Text("Export[%u]: %s", i, ToString(subobject.ownedAssociationExportPointers[i]).c_str());
			}
			break;
		}
		case D3D12_STATE_SUBOBJECT_TYPE_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION:
		{
			ImGui::Text("Subobject Association (Name): %s", ToString(subobject.associatedSubobjectName.c_str()).c_str());

			ImGui::Text("Num Exports: %zu", subobject.ownedAssociationExportPointers.size());
			for (Uint32 i = 0; i < subobject.ownedAssociationExportPointers.size(); ++i)
			{
				ImGui::Text("Export[%u]: %s", i, ToString(subobject.ownedAssociationExportPointers[i]).c_str());
			}
			break;
		}
		case D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG:
		{
			auto* config = reinterpret_cast<D3D12_RAYTRACING_SHADER_CONFIG const*>(subobject.data.data());
			ImGui::Text("Max Payload Size: %u", config->MaxPayloadSizeInBytes);
			ImGui::Text("Max Attribute Size: %u", config->MaxAttributeSizeInBytes);
			break;
		}
		case D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG:
		{
			auto* config = reinterpret_cast<D3D12_RAYTRACING_PIPELINE_CONFIG const*>(subobject.data.data());
			ImGui::Text("Max Trace Recursion Depth: %u", config->MaxTraceRecursionDepth);
			break;
		}
		case D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP:
		{
			ImGui::Text("Hit Group Type: %s", subobject.hitGroupType == D3D12_HIT_GROUP_TYPE_TRIANGLES ? "Triangles" : "Procedural");
			ImGui::Text("Hit Group Name: %s", ToString(subobject.hitGroupName.c_str()).c_str());
			ImGui::Text("AnyHit Shader: %s", subobject.anyHitShader.empty() ? "NULL" : ToString(subobject.anyHitShader.c_str()).c_str());
			ImGui::Text("ClosestHit Shader: %s", subobject.closestHitShader.empty() ? "NULL" : ToString(subobject.closestHitShader.c_str()).c_str());
			ImGui::Text("Intersection Shader: %s", subobject.intersectionShader.empty() ? "NULL" : ToString(subobject.intersectionShader.c_str()).c_str());
			break;
		}
		case D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG1:
		{
			auto* config1 = reinterpret_cast<D3D12_RAYTRACING_PIPELINE_CONFIG1 const*>(subobject.data.data());
			ImGui::Text("Max Trace Recursion Depth: %u", config1->MaxTraceRecursionDepth);
			ImGui::Text("Flags: 0x%X", config1->Flags);
			break;
		}
		default:
			ImGui::TextUnformatted("Unhandled subobject type.");
			if (!subobject.data.empty())
			{
				ImGui::SameLine(); ImGui::Text("(Data size: %zu bytes)", subobject.data.size());
			}
			break;
		}
	}

	void RenderStateObjectDesc(StateObjectDesc const& desc, ObjectTracker const& objectTracker)
	{
		ImGui::SeparatorText("State Object Details");

		ImGui::Text("Type: %s", D3D12StateObjectTypeToString(desc.type));
		ImGui::Text("Number of Subobjects: %zu", desc.subobjects.size());

		if (ImGui::TreeNodeEx("Subobjects", ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (Uint32 i = 0; i < desc.subobjects.size(); ++i)
			{
				StateObjectDesc::Subobject const& subobject = desc.subobjects[i];
				ImGui::PushID(static_cast<Int>(i));
				if (ImGui::TreeNodeEx(std::format("[{}] {}", i, D3D12StateSubobjectTypeToString(subobject.type)).c_str()))
				{
					RenderStateSubobjectDesc(desc, subobject, objectTracker);
					ImGui::TreePop();
				}
				ImGui::PopID();
			}
			ImGui::TreePop();
		}
	}

	void RenderVertexBufferView(Uint32 slot, D3D12_VERTEX_BUFFER_VIEW const& vbv, ObjectTracker const& objectTracker, ResourceAddressTracker const& addressTracker)
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("%u", slot);

		ImGui::TableSetColumnIndex(1);
		ImGui::Text("0x%llX", (Uint64)vbv.BufferLocation);

		ImGui::TableSetColumnIndex(2);
		ImGui::Text("%u", vbv.StrideInBytes);

		ImGui::TableSetColumnIndex(3);
		ImGui::Text("%u", vbv.SizeInBytes);

		ImGui::TableSetColumnIndex(4);
		ID3D12Resource* resource = addressTracker.QueryResource(vbv.BufferLocation);
		RenderObjectInfoByID(objectTracker.GetObjectID(resource), objectTracker, "Vertex Buffer");
	}

	void RenderIndexBufferView(D3D12_INDEX_BUFFER_VIEW const& ibv, ObjectTracker const& objectTracker, ResourceAddressTracker const& addressTracker)
	{
		if (ImGui::BeginTable("IndexBufferTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
		{
			ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 150.0f);
			ImGui::TableSetupColumn("Value");
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Buffer Location");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("0x%llX", (Uint64)ibv.BufferLocation);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Size (Bytes)");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%u", ibv.SizeInBytes);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Format");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%s", DXGIFormatToString(ibv.Format));

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Resource");
			ImGui::TableSetColumnIndex(1);
			ID3D12Resource* resource = addressTracker.QueryResource(ibv.BufferLocation);
			RenderObjectInfoByID(objectTracker.GetObjectID(resource), objectTracker, "Index Buffer");

			ImGui::EndTable();
		}
	}

	void RenderViewport(Uint32 index, D3D12_VIEWPORT const& vp)
	{
		ImGui::Text("Viewport %u:", index);
		if (ImGui::BeginTable("ViewportTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX))
		{
			ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 100.0f);
			ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, 150.0f);
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("TopLeftX");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%.1f", vp.TopLeftX);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("TopLeftY");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%.1f", vp.TopLeftY);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Width");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%.1f", vp.Width);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Height");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%.1f", vp.Height);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("MinDepth");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%.1f", vp.MinDepth);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("MaxDepth");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%.1f", vp.MaxDepth);

			ImGui::EndTable();
		}
	}

	void RenderScissorRect(Uint32 index, D3D12_RECT const& rect)
	{
		ImGui::Text("Scissor Rect %u:", index);
		if (ImGui::BeginTable("ScissorRectTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX))
		{
			ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 100.0f);
			ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, 150.0f);
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Left");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%d", rect.left);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Top");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%d", rect.top);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Right");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%d", rect.right);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Bottom");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%d", rect.bottom);

			ImGui::EndTable();
		}
	}

	void RenderScissorRect(D3D12_RECT const& rect)
	{
		if (ImGui::BeginTable("ScissorRectTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX))
		{
			ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 100.0f);
			ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, 150.0f);
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Left");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%d", rect.left);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Top");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%d", rect.top);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Right");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%d", rect.right);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Bottom");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%d", rect.bottom);

			ImGui::EndTable();
		}
	}

	void RenderResourceBarrier(Uint32 index, D3D12_RESOURCE_BARRIER const& barrier, ObjectTracker const& objectTracker)
	{
		ImGui::PushID(index);
		if (barrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION)
		{
			auto const& t = barrier.Transition;
			TrackedObjectInfo const* resourceInfo = objectTracker.GetObjectInfo(t.pResource);
			ImGui::Text("Transition: obj#%llu (%s, %s -> %s)",
				resourceInfo ? resourceInfo->objectId : InvalidObjectID,
				resourceInfo ? resourceInfo->objectDebugName.c_str() : "<Unknown>",
				D3D12ResourceStateToString(t.StateBefore).c_str(),
				D3D12ResourceStateToString(t.StateAfter).c_str());
		}
		else if (barrier.Type == D3D12_RESOURCE_BARRIER_TYPE_ALIASING)
		{
			TrackedObjectInfo const* beforeInfo = objectTracker.GetObjectInfo(barrier.Aliasing.pResourceBefore);
			TrackedObjectInfo const* afterInfo = objectTracker.GetObjectInfo(barrier.Aliasing.pResourceAfter);
			ImGui::Text("Aliasing: obj#%llu (%s) -> obj#%llu (%s)",
				beforeInfo ? beforeInfo->objectId : InvalidObjectID,
				beforeInfo ? beforeInfo->objectDebugName.c_str() : "<Unknown>",
				afterInfo ? afterInfo->objectId : InvalidObjectID,
				afterInfo ? afterInfo->objectDebugName.c_str() : "<Unknown>");
		}
		else if (barrier.Type == D3D12_RESOURCE_BARRIER_TYPE_UAV)
		{
			TrackedObjectInfo const* uavInfo = objectTracker.GetObjectInfo(barrier.UAV.pResource);
			ImGui::Text("UAV Barrier: obj#%llu (%s)",
				uavInfo ? uavInfo->objectId : InvalidObjectID,
				uavInfo ? uavInfo->objectDebugName.c_str() : "<Unknown>");
		}
		ImGui::PopID();
	}

	void RenderIndirectArgumentDesc(Uint32 index, D3D12_INDIRECT_ARGUMENT_DESC const& argDesc)
	{
		ImGui::PushID(index);
		if (ImGui::TreeNodeEx(std::format("Argument [{}]", index).c_str()))
		{
			ImGui::Text("Type: %s", D3D12IndirectArgumentTypeToString(argDesc.Type));

			switch (argDesc.Type)
			{
			case D3D12_INDIRECT_ARGUMENT_TYPE_VERTEX_BUFFER_VIEW:
				ImGui::Text("Slot: %u", argDesc.VertexBuffer.Slot);
				break;

			case D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT:
				ImGui::Text("Root Parameter Index: %u", argDesc.Constant.RootParameterIndex);
				ImGui::Text("Dest Offset (32-bit): %u", argDesc.Constant.DestOffsetIn32BitValues);
				ImGui::Text("Num 32-bit Values: %u", argDesc.Constant.Num32BitValuesToSet);
				break;

			case D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT_BUFFER_VIEW:
				ImGui::Text("Root Parameter Index: %u", argDesc.ConstantBufferView.RootParameterIndex);
				break;

			case D3D12_INDIRECT_ARGUMENT_TYPE_SHADER_RESOURCE_VIEW:
				ImGui::Text("Root Parameter Index: %u", argDesc.ShaderResourceView.RootParameterIndex);
				break;

			case D3D12_INDIRECT_ARGUMENT_TYPE_UNORDERED_ACCESS_VIEW:
				ImGui::Text("Root Parameter Index: %u", argDesc.UnorderedAccessView.RootParameterIndex);
				break;

			case D3D12_INDIRECT_ARGUMENT_TYPE_DRAW:
			case D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED:
			case D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH:
			case D3D12_INDIRECT_ARGUMENT_TYPE_INDEX_BUFFER_VIEW:
				break;
			}
			ImGui::TreePop();
		}
		ImGui::PopID();
	}

	void RenderRootSignatureDetails(ObjectID rootSignatureId, ObjectTracker const& objectTracker, DescriptorTracker const& descriptorTracker, ResourceAddressTracker const& addressTracker, std::span<RootParameterBinding const> rootArgs, Char const* rootSignatureLabel /*= "Root Signature Parameters"*/)
	{
		if (rootSignatureId == InvalidObjectID)
			return;

		TrackedObjectInfo const* rsInfo = objectTracker.GetObjectInfo(rootSignatureId);
		if (!rsInfo || !std::holds_alternative<RootSignatureDesc>(rsInfo->objectDesc))
			return;

		RootSignatureDesc const& rsDesc = std::get<RootSignatureDesc>(rsInfo->objectDesc);

		if (ImGui::TreeNode(rootSignatureLabel))
		{
			Bool anySet = false;
			for (Uint32 i = 0; i < rsDesc.Parameters.size(); ++i)
			{
				RootParameterBinding const& binding =
					(i < rootArgs.size() && rootArgs[i].isSet) ? rootArgs[i] : RootParameterBinding{};

				if (!binding.isSet)
					continue;

				anySet = true;
				ImGui::PushID(i);
				std::string paramLabel = std::format("{:02d} - {}", i, D3D12RootParameterTypeToString(rsDesc.Parameters[i].ParameterType));
				if (ImGui::TreeNode(paramLabel.c_str()))
				{
					ImGui::Text("ShaderVisibility: %s", D3D12ShaderVisibilityToString(rsDesc.Parameters[i].ShaderVisibility));

					if (rsDesc.Parameters[i].ParameterType == D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS)
					{
						auto const& constants = rsDesc.Parameters[i].Constants;
						ImGui::Text("ShaderRegister: %u (0x%X)", constants.ShaderRegister, constants.ShaderRegister);
						ImGui::Text("RegisterSpace: %u (0x%X)", constants.RegisterSpace, constants.RegisterSpace);
						ImGui::Text("Num32BitValues: %u (0x%X)", constants.Num32BitValues, constants.Num32BitValues);

						if (std::holds_alternative<RootParameterBinding::RootConstants>(binding.value))
						{
							auto const& values = std::get<RootParameterBinding::RootConstants>(binding.value);
							if (ImGui::TreeNode("SrcData"))
							{
								for (Uint32 j = 0; j < constants.Num32BitValues && j < values.size(); ++j)
								{
									std::string constNodeLabel = std::format("{:02d} - Constant", j);
									ImGui::PushID(j);
									if (ImGui::TreeNode(constNodeLabel.c_str()))
									{
										union { Uint32 u; Float f; } data;
										data.u = values[j];
										if (values[j] != 0)
										{
											ImGui::Text("%.6f (%u) (0x%08X)", data.f, data.u, data.u);
										}
										else
										{
											ImGui::Text("<Not set>");
										}
										ImGui::TreePop();
									}
									ImGui::PopID();
								}
								ImGui::TreePop();
							}
						}
					}
					else
					{
						RenderRootParameterBinding(i, binding, rsDesc.Parameters[i].ShaderVisibility, objectTracker, descriptorTracker, addressTracker, nullptr);
					}
					ImGui::TreePop();
				}
				ImGui::PopID();
			}
			if (!anySet)
			{
				ImGui::Text("<None set>");
			}
			ImGui::TreePop();
		}

		if (!rsDesc.StaticSamplers.empty())
		{
			if (ImGui::TreeNode("Static Samplers"))
			{
				for (Uint32 i = 0; i < rsDesc.StaticSamplers.size(); ++i)
				{
					ImGui::PushID(i);
					StaticSamplerDesc const& sampler = rsDesc.StaticSamplers[i];
					if (ImGui::TreeNode(std::format("{:02d} - Static Sampler", i).c_str()))
					{
						ImGui::Text("Shader Register: %u", sampler.Desc.ShaderRegister);
						ImGui::Text("Register Space: %u", sampler.Desc.RegisterSpace);
						ImGui::Text("Shader Visibility: %s", D3D12ShaderVisibilityToString(sampler.Desc.ShaderVisibility));
						ImGui::Separator();
						RenderSamplerDesc(sampler.Desc, "Static Sampler");
						ImGui::TreePop();
					}
					ImGui::PopID();
				}
				ImGui::TreePop();
			}
		}
	}

	void RenderRootParameterBinding(
		Uint32 index,
		RootParameterBinding const& binding,
		D3D12_SHADER_VISIBILITY expectedVisibility,
		ObjectTracker const& objectTracker,
		DescriptorTracker const& descriptorTracker,
		ResourceAddressTracker const& addressTracker,
		SelectedItem* selectedItemInViewer
	)
	{
		if ((binding.shaderVisibility != D3D12_SHADER_VISIBILITY_ALL && binding.shaderVisibility != expectedVisibility))
		{
			return;
		}

		std::string paramLabel = std::format("Root Parameter {}: ", index);
		ID3D12Resource* resource = nullptr;
		TrackedObjectInfo const* resourceInfo = nullptr;
		ObjectID heapId = InvalidObjectID;
		Int descriptorIndex = -1;
		DescriptorInfo descriptorInfo;

		if (!binding.isSet)
		{
			paramLabel += "<not bound> [Type: ";
			paramLabel += D3D12RootParameterTypeToString(binding.type);
			paramLabel += std::format(", Visibility: {}]", D3D12ShaderVisibilityToString(binding.shaderVisibility));
		}
		else
		{
			std::visit([&](auto&& argValue)
				{
					using T = std::decay_t<decltype(argValue)>;
					if constexpr (std::is_same_v<T, D3D12_GPU_DESCRIPTOR_HANDLE>)
					{
						paramLabel += "Descriptor Table";
						if (argValue.ptr != 0)
						{
							auto const& pair = descriptorTracker.FindDescriptorByGpuHandle(argValue);
							TrackedDescriptorHeapInfo const* heapInfo = pair.first;
							descriptorIndex = pair.second;
							if (heapInfo && descriptorIndex >= 0)
							{
								heapId = heapInfo->heapId;
								descriptorInfo = heapInfo->descriptors[descriptorIndex];
								paramLabel += std::format(" [Type: {}]", DescriptorViewTypeToString(descriptorInfo.type));
								if (descriptorInfo.type != DescriptorViewType::Sampler && descriptorInfo.resourceId != InvalidObjectID)
								{
									resourceInfo = objectTracker.GetObjectInfo(descriptorInfo.resourceId);
									if (resourceInfo)
									{
										resource = reinterpret_cast<ID3D12Resource*>(resourceInfo->objectPtr);
										if (!resourceInfo->objectDebugName.empty())
										{
											paramLabel += std::format(" Name: {}", resourceInfo->objectDebugName);
										}
									}
								}
							}
						}
						else
						{
							paramLabel += " [Null Handle]";
						}
					}
					else if constexpr (std::is_same_v<T, D3D12_GPU_VIRTUAL_ADDRESS>)
					{
						resource = addressTracker.QueryResource(argValue);
						if (resource)
						{
							resourceInfo = objectTracker.GetObjectInfo(resource);
							if (resourceInfo)
							{
								descriptorInfo.resourceId = resourceInfo->objectId;
								descriptorInfo.type = DescriptorViewType::Unknown;
								paramLabel += std::format(" [Type: Unknown]");
								if (!resourceInfo->objectDebugName.empty())
								{
									paramLabel += std::format(" Name: {}", resourceInfo->objectDebugName);
								}
							}
						}
						else
						{
							paramLabel += " [Unmapped VA]";
						}
					}
				}, binding.value);
		}

		ImGui::PushID(static_cast<Int>(index));

		if (selectedItemInViewer)
		{
			if (resource || descriptorInfo.type == DescriptorViewType::Sampler)
			{
				if (resource)
				{
					ImGui::PushID(resource);
				}
				else
				{
					ImGui::PushID(static_cast<int>(heapId) ^ descriptorIndex);
				}

				Bool isSelected = selectedItemInViewer->Matches(descriptorInfo.resourceId, heapId, descriptorIndex);
				if (ImGui::Selectable(paramLabel.c_str(), &isSelected))
				{
					if (isSelected)
					{
						selectedItemInViewer->type = resource ? SelectedItem::Type::Resource : SelectedItem::Type::Sampler;
						selectedItemInViewer->resource = resource;
						selectedItemInViewer->heapId = heapId;
						selectedItemInViewer->descriptorIndex = descriptorIndex;
						selectedItemInViewer->descriptorInfo = descriptorInfo;
					}
					else
					{
						selectedItemInViewer->Reset();
					}
				}
				ImGui::PopID();
			}
			else
			{
				ImGui::Text("%s", paramLabel.c_str());
			}
		}
		else
		{
			ImGui::Text("%s", paramLabel.c_str());
		}

		ImGui::PopID();
	}

	void RenderBindlessParameter(ObjectID heapId, Uint64 resourceDescriptorHeapIndex,
		ObjectTracker const& objectTracker, DescriptorTracker const& descriptorTracker,
		SelectedItem* selectedItemInViewer)
	{
		DescriptorInfo const* descriptorInfo = descriptorTracker.GetDescriptorInfo(heapId, (Uint32)resourceDescriptorHeapIndex);
		TrackedObjectInfo const* resourceInfo = objectTracker.GetObjectInfo(descriptorInfo->resourceId);
		ID3D12Resource* resource = resourceInfo ? reinterpret_cast<ID3D12Resource*>(resourceInfo->objectPtr) : nullptr;

		std::string paramLabel = std::format("Bindless Parameter");
		paramLabel += std::format(" [Type: {}]", DescriptorViewTypeToString(descriptorInfo->type));
		if (resourceInfo)
		{
			if (!resourceInfo->objectDebugName.empty())
			{
				paramLabel += std::format(" Name: {}", resourceInfo->objectDebugName);
			}
		}
		else if (descriptorInfo->resourceId != InvalidObjectID)
		{
			paramLabel += " [No Resource Info]";
		}

		ImGui::PushID(static_cast<Int>(heapId * 1000 + (descriptorInfo->resourceId != InvalidObjectID ? descriptorInfo->resourceId : 0)));

		if (selectedItemInViewer)
		{
			if (resource || descriptorInfo->type == DescriptorViewType::Sampler)
			{
				if (resource)
				{
					ImGui::PushID(resource);
				}
				else
				{
					ImGui::PushID(static_cast<int>(heapId) ^ (Int)resourceDescriptorHeapIndex);
				}

				Bool isSelected = selectedItemInViewer->Matches(descriptorInfo->resourceId, heapId, (Int)resourceDescriptorHeapIndex);
				if (ImGui::Selectable(paramLabel.c_str(), &isSelected))
				{
					if (isSelected)
					{
						selectedItemInViewer->type = resource ? SelectedItem::Type::Resource : SelectedItem::Type::Sampler;
						selectedItemInViewer->resource = resource;
						selectedItemInViewer->heapId = heapId;
						selectedItemInViewer->descriptorIndex = (Int)resourceDescriptorHeapIndex;
						selectedItemInViewer->descriptorInfo = *descriptorInfo;
					}
					else
					{
						selectedItemInViewer->Reset();
					}
				}
				ImGui::PopID();
			}
			else
			{
				ImGui::Text("%s", paramLabel.c_str());
			}
		}
		else
		{
			ImGui::Text("%s", paramLabel.c_str());
		}

		ImGui::PopID();
	}

	std::string GetResourceLabel(Char const* prefix, D3D12_GPU_VIRTUAL_ADDRESS address, ObjectTracker const& objectTracker, ResourceAddressTracker const& addressTracker)
	{
		ID3D12Resource* resource = addressTracker.QueryResource(address);
		return GetResourceLabel(prefix, resource, objectTracker, addressTracker);
	}

	std::string GetResourceLabel(Char const* prefix, ID3D12Resource* resource, ObjectTracker const& objectTracker, ResourceAddressTracker const& addressTracker)
	{
		std::string label = std::format("{} : ", prefix);
		TrackedObjectInfo const* resourceInfo = resource ? objectTracker.GetObjectInfo(resource) : nullptr;
		if (resourceInfo)
		{
			label += std::format(" obj#{} ({})", resourceInfo->objectId, resourceInfo->objectDebugName);
		}
		else
		{
			label += std::format(" Unknown Resource {}", (void*)resource);
		}
		return label;
	}

	void RenderSelectableResource(std::string const& label, ID3D12Resource* resource, DescriptorInfo const& descInfo, SelectedItem& selectedItem, void* id)
	{
		ImGui::PushID(id);
		if (resource)
		{
			ImGui::PushID(resource);
		}
		Bool isSelected = selectedItem.Matches(descInfo.resourceId, InvalidObjectID, -1);
		if (ImGui::Selectable(label.c_str(), &isSelected))
		{
			if (isSelected)
			{
				selectedItem.type = SelectedItem::Type::Resource;
				selectedItem.resource = resource;
				selectedItem.heapId = InvalidObjectID;
				selectedItem.descriptorIndex = -1;
				selectedItem.descriptorInfo = descInfo;
			}
			else
			{
				selectedItem.Reset();
			}
		}
		if (resource)
		{
			ImGui::PopID();
		}
		ImGui::PopID();
	}

}
