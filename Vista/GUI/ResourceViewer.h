#pragma once


namespace vista
{
	class ImGuiManager;
	class ResourceViewer
	{
	public:
		explicit ResourceViewer(ImGuiManager& imguiManager) : imguiManager(imguiManager) {}
		void RenderResource(ID3D12Resource* resource);

	private:
		ImGuiManager& imguiManager;

	private:
		void RenderTexturePreview(ID3D12Resource*);
		void RenderBufferPreview(ID3D12Resource*);
	};
}