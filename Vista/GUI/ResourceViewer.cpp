#include "ResourceViewer.h"
#include "ImGuiManager.h"
#include "BufferFormatParser.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui/imgui.h"

namespace vista
{

	void ResourceViewer::RenderResource(ID3D12Resource* resource)
	{
		D3D12_RESOURCE_DIMENSION const dimension = resource ? resource->GetDesc().Dimension : D3D12_RESOURCE_DIMENSION_UNKNOWN;
		switch (dimension)
		{
		case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
		case D3D12_RESOURCE_DIMENSION_TEXTURE3D:	RenderTexturePreview(resource); break;
		case D3D12_RESOURCE_DIMENSION_BUFFER:		RenderBufferPreview(resource);  break;
		case D3D12_RESOURCE_DIMENSION_TEXTURE1D:	ImGui::Text("Resource preview not yet supported for Texture1D!"); 
		}
	}

	void ResourceViewer::RenderTexturePreview(ID3D12Resource* resource)
	{
		static Bool showR = true, showG = true, showB = true, showA = false;
		static Int selectedMipLevel = 0;
		static Int selectedSlice = 0; 

		static Float zoom = 1.0f;
		static ImVec2 uv0 = ImVec2(0.0f, 0.0f);
		static ImVec2 uv1 = ImVec2(1.0f, 1.0f);

		static Bool hasPickedPixel = false;
		static Int pickedX = -1, pickedY = -1;

		D3D12_RESOURCE_DESC const& resDesc = resource->GetDesc();

		ImGui::Text("Select Channels:");
		ImGui::SameLine(); ImGui::Checkbox("R##ChannelR", &showR);
		ImGui::SameLine(); ImGui::Checkbox("G##ChannelG", &showG);
		ImGui::SameLine(); ImGui::Checkbox("B##ChannelB", &showB);
		ImGui::SameLine(); ImGui::Checkbox("A##ChannelA", &showA);

		Uint16 mipCount = resDesc.MipLevels;
		if (mipCount > 1)
		{
			static Char const* mipLabels[] = {
				"Mip 0","Mip 1","Mip 2","Mip 3","Mip 4","Mip 5","Mip 6","Mip 7",
				"Mip 8","Mip 9","Mip 10","Mip 11","Mip 12","Mip 13","Mip 14","Mip 15"
			};
			ImGui::Text("Mip Level:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(75.0f);
			if (ImGui::Combo("##MipLevelCombo", &selectedMipLevel, mipLabels, mipCount))
			{
				if (resDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D)
				{
					Uint depthAtMip = std::max<Uint>(1, (Uint)resDesc.DepthOrArraySize >> selectedMipLevel);
					selectedSlice = std::clamp(selectedSlice, 0, (Int)depthAtMip - 1);
				}
			}
		}

		if (resDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D && resDesc.DepthOrArraySize > 1)
		{
			ImGui::SameLine();
			ImGui::Text("Array Slice:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(150.0f);
			auto getter = [](void*, Int idx, Char const** out_text) {
				static Char buf[32]; snprintf(buf, sizeof(buf), "Array Slice %d", idx); *out_text = buf; return true; };
			ImGui::Combo("##ArraySliceCombo", &selectedSlice, getter, nullptr, resDesc.DepthOrArraySize);
		}
		else if (resDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D)
		{
			Uint depthAtMip = std::max<Uint>(1, (Uint)resDesc.DepthOrArraySize >> selectedMipLevel);
			if (depthAtMip > 1)
			{
				ImGui::Text("Depth Slice:");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(150.0f);
				auto getter = [](void*, Int idx, Char const** out_text) {
					static Char buf[32]; snprintf(buf, sizeof(buf), "Depth Slice %d", idx); *out_text = buf; return true; };
				ImGui::Combo("##DepthSliceCombo", &selectedSlice, getter, nullptr, depthAtMip);
			}
		}

		ImGui::SameLine();
		Bool resetView = ImGui::Button("Reset View");
		ImGui::Separator();
		if (resetView)
		{
			selectedMipLevel = 0;
			selectedSlice = 0;
			zoom = 1.0f;
			uv0 = { 0,0 }; uv1 = { 1,1 };
		}

		ImGui::Text("Zoom: %.2fx (Scroll to Zoom, Middle-Click to Pan)", zoom);
		ImVec2 canvasSize = ImGui::GetContentRegionAvail();
		ImVec2 canvasTopLeft = ImGui::GetCursorScreenPos();

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->AddRectFilled(canvasTopLeft, canvasTopLeft + canvasSize, IM_COL32(0, 0, 0, 255));

		ImGui::InvisibleButton("##ImageCanvas", canvasSize);
		if (ImGui::IsItemHovered())
		{
			ImVec2 mousePos = ImGui::GetMousePos();
			ImVec2 mouseCanvas = mousePos - canvasTopLeft;

			Float wheel = ImGui::GetIO().MouseWheel;
			if (wheel != 0.0f)
			{
				Float oldZoom = zoom;
				zoom *= (wheel > 0 ? 1.2f : 1.0f / 1.2f);
				zoom = std::clamp(zoom, 0.1f, 100.0f);

				ImVec2 mouseUV = 
				{
					uv0.x + (uv1.x - uv0.x) * (mouseCanvas.x / canvasSize.x),
					uv0.y + (uv1.y - uv0.y) * (mouseCanvas.y / canvasSize.y)
				};

				Float newW = (uv1.x - uv0.x) * (oldZoom / zoom);
				Float newH = (uv1.y - uv0.y) * (oldZoom / zoom);
				Float ratioX = mouseCanvas.x / canvasSize.x;
				Float ratioY = mouseCanvas.y / canvasSize.y;

				uv0.x = mouseUV.x - ratioX * newW;
				uv0.y = mouseUV.y - ratioY * newH;
				uv1.x = uv0.x + newW;
				uv1.y = uv0.y + newH;
			}

			if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
			{
				ImVec2 d = ImGui::GetIO().MouseDelta;
				Float uvW = uv1.x - uv0.x, uvH = uv1.y - uv0.y;
				uv0.x -= (d.x / canvasSize.x) * uvW;
				uv1.x -= (d.x / canvasSize.x) * uvW;
				uv0.y -= (d.y / canvasSize.y) * uvH;
				uv1.y -= (d.y / canvasSize.y) * uvH;
			}

			if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
			{
				Uint width = (Uint)(resDesc.Width >> selectedMipLevel);
				Uint height = (Uint)(resDesc.Height >> selectedMipLevel);
				Float u = uv0.x + (uv1.x - uv0.x) * (mouseCanvas.x / canvasSize.x);
				Float v = uv0.y + (uv1.y - uv0.y) * (mouseCanvas.y / canvasSize.y);
				pickedX = (Int)std::clamp(u * width, 0.0f, (Float)width - 1);
				pickedY = (Int)std::clamp(v * height, 0.0f, (Float)height - 1);
				hasPickedPixel = true;
			}
		}

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		if (resDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D && resDesc.DepthOrArraySize > 1)
		{
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
			srvDesc.Texture2DArray.MostDetailedMip = selectedMipLevel;
			srvDesc.Texture2DArray.MipLevels = 1;
			srvDesc.Texture2DArray.FirstArraySlice = selectedSlice;
			srvDesc.Texture2DArray.ArraySize = 1;
			srvDesc.Texture2DArray.PlaneSlice = 0;
			srvDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
		}
		else if (resDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D)
		{
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
			srvDesc.Texture3D.MostDetailedMip = selectedMipLevel;
			srvDesc.Texture3D.MipLevels = 1;
		}
		else
		{
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = selectedMipLevel;
			srvDesc.Texture2D.MipLevels = 1;
			srvDesc.Texture2D.PlaneSlice = 0;
			srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		}

		srvDesc.Format = resDesc.Format;
		srvDesc.Shader4ComponentMapping = D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(
			showR ? D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_0 : D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_0,
			showG ? D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_1 : D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_0,
			showB ? D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_2 : D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_0,
			showA ? D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_3 : D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_1);

		switch (srvDesc.Format)
		{
		case DXGI_FORMAT_R16_TYPELESS:        srvDesc.Format = DXGI_FORMAT_R16_UNORM; break;
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT:           srvDesc.Format = DXGI_FORMAT_R32_FLOAT; break;
		case DXGI_FORMAT_R24G8_TYPELESS:      srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; break;
		case DXGI_FORMAT_R32G8X24_TYPELESS:   srvDesc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS; break;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = imguiManager.GetDescriptorForThisFrame();
		imguiManager.GetDevice()->CreateShaderResourceView(resource, &srvDesc, srvHandle);

		ImVec2 uvMin(std::max(uv0.x, 0.0f), std::max(uv0.y, 0.0f));
		ImVec2 uvMax(std::min(uv1.x, 1.0f), std::min(uv1.y, 1.0f));
		if (uvMin.x < uvMax.x && uvMin.y < uvMax.y)
		{
			Float fracLeft = (uvMin.x - uv0.x) / (uv1.x - uv0.x);
			Float fracTop = (uvMin.y - uv0.y) / (uv1.y - uv0.y);
			Float fracRight = (uvMax.x - uv0.x) / (uv1.x - uv0.x);
			Float fracBottom = (uvMax.y - uv0.y) / (uv1.y - uv0.y);

			ImVec2 dstMin(
				canvasTopLeft.x + fracLeft * canvasSize.x,
				canvasTopLeft.y + fracTop * canvasSize.y
			);
			ImVec2 dstMax(
				canvasTopLeft.x + fracRight * canvasSize.x,
				canvasTopLeft.y + fracBottom * canvasSize.y
			);

			D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptor = imguiManager.GetDescriptorForThisFrame();
			drawList->AddImage(
				(ImTextureID)gpuDescriptor.ptr,
				dstMin, dstMax,
				uvMin, uvMax
			);
		}

		if (hasPickedPixel)
		{
			Uint width = static_cast<Uint>(resDesc.Width >> selectedMipLevel);
			Uint height = static_cast<Uint>(resDesc.Height >> selectedMipLevel);

			Float pixelUCenter = (Float(pickedX) + 0.5f) / Float(width);
			Float pixelVCenter = (Float(pickedY) + 0.5f) / Float(height);
			Float pixelUSize = 1.0f / Float(width);
			Float pixelVSize = 1.0f / Float(height);

			Float pixelU0 = pixelUCenter - pixelUSize * 0.5f;
			Float pixelV0 = pixelVCenter - pixelVSize * 0.5f;
			Float pixelU1 = pixelUCenter + pixelUSize * 0.5f;
			Float pixelV1 = pixelVCenter + pixelVSize * 0.5f;

			ImVec2 dstMin(
				canvasTopLeft.x + (pixelU0 - uv0.x) / (uv1.x - uv0.x) * canvasSize.x,
				canvasTopLeft.y + (pixelV0 - uv0.y) / (uv1.y - uv0.y) * canvasSize.y
			);
			ImVec2 dstMax(
				canvasTopLeft.x + (pixelU1 - uv0.x) / (uv1.x - uv0.x) * canvasSize.x,
				canvasTopLeft.y + (pixelV1 - uv0.y) / (uv1.y - uv0.y) * canvasSize.y
			);

			drawList->AddRect(dstMin, dstMax, IM_COL32(255, 255, 0, 255), 0.0f, 0, 1.5f);

			if (resDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D)
			{
				ImGui::Text("Picked Pixel: X = %d, Y = %d, Z = %d", pickedX, pickedY, selectedSlice);
			}
			else
			{
				ImGui::Text("Picked Pixel: X = %d, Y = %d", pickedX, pickedY);
			}
		}
	}

	void ResourceViewer::RenderBufferPreview(ID3D12Resource* resource)
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