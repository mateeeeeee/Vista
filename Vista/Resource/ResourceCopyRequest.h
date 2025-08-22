#pragma once

namespace vista
{
	class Command;
	class ResourceCopyRequestManager;

	class ResourceCopyRequest
	{
		friend class ResourceCopyRequestManager;
	public:
		ResourceCopyRequest() {}

		ResourceCopyRequest(Uint64 commandIndex, Uint32 commandType, ID3D12Resource* sourceResource, ID3D12Resource* destinationResource) 
			: commandIndex(commandIndex), commandType(commandType), sourceResource(sourceResource), destinationResource(destinationResource)
		{
		}

		Bool IsReady() const
		{
			return isReady;
		}
		ID3D12Resource* GetDestinationResource() const
		{
			return destinationResource;
		}
		ID3D12Resource* GetSourceResource() const
		{
			return sourceResource;
		}
		Bool IsMatch(Command const& cmd) const;

	private:
		Uint64 commandIndex = 0;
		Uint32 commandType = 0;
		ID3D12Resource* sourceResource = nullptr;
		ID3D12Resource* destinationResource = nullptr;
		Bool isReady = false;

		Uint64 fenceValue = 0; 
	};
}