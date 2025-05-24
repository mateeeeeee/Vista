#include <format>
#include "DeviceCommands.h"
#include "Utilities/StringUtil.h"

namespace vista
{
	std::string CreateCommandQueueCommand::GetDesc() const
	{
		return std::format("CreateCommandQueue(..., obj#{}) {{ this -> Device obj#{}, return -> {} }}", commandQueueId, GetParentId(), HResultToString(hr));
	}

	std::string CreateCommandQueue1Command::GetDesc() const
	{
		return std::format("CreateCommandQueue1(..., obj#{}) {{ this -> Device obj#{}, return -> {} }}", commandQueueId, GetParentId(), HResultToString(hr));
	}

	std::string CreateCommandListCommand::GetDesc() const
	{
		return std::format("CreateCommandList(..., obj#{}) {{ this -> Device obj#{}, return -> {} }}", commandListId, GetParentId(), HResultToString(hr));
	}

	std::string CreateCommandList1Command::GetDesc() const
	{
		return std::format("CreateCommandList1(..., obj#{}) {{ this -> Device obj#{}, return -> {} }}", commandListId, GetParentId(), HResultToString(hr));
	}

	std::string CreateCommandAllocatorCommand::GetDesc() const
	{
		return std::format("CreateCommandAllocator(..., obj#{}) {{ this -> Device obj#{}, return -> {} }}", commandAllocatorId, GetParentId(), HResultToString(hr));
	}

	std::string CreateFenceCommand::GetDesc() const
	{
		return std::format("CreateFence(..., obj#{}) {{ this -> Device obj#{}, return -> {} }}", fenceId, GetParentId(), HResultToString(hr));
	}

	std::string CreateHeapCommand::GetDesc() const
	{
		return std::format("CreateHeap(..., obj#{}) {{ this -> Device obj#{}, return -> {} }}", heapId, GetParentId(), HResultToString(hr));
	}

	std::string CreateHeap1Command::GetDesc() const
	{
		return std::format("CreateHeap1(..., obj#{}) {{ this -> Device obj#{}, return -> {} }}", heapId, GetParentId(), HResultToString(hr));
	}

	std::string CreateRootSignatureCommand::GetDesc() const
	{
		return std::format("CreateRootSignature(..., obj#{}) {{ this -> Device obj#{}, return -> {} }}", rootSignatureId, GetParentId(), HResultToString(hr));
	}

	std::string CreateCommandSignatureCommand::GetDesc() const
	{
		return std::format("CreateCommandSignature(..., obj#{}) {{ this -> Device obj#{}, return -> {} }}", cmdSignatureId, GetParentId(), HResultToString(hr));
	}

	std::string CreateGraphicsPipelineStateCommand::GetDesc() const
	{
		return std::format("CreateGraphicsPipelineState(..., obj#{}) {{ this -> Device obj#{}, return -> {} }}", psoId, GetParentId(), HResultToString(hr));
	}

	std::string CreateComputePipelineStateCommand::GetDesc() const
	{
		return std::format("CreateComputePipelineState(..., obj#{}) {{ this -> Device obj#{}, return -> {} }}", psoId, GetParentId(), HResultToString(hr));
	}

	std::string CreatePipelineStateCommand::GetDesc() const
	{
		return std::format("CreatePipelineState(..., obj#{}) {{ this -> Device obj#{}, return -> {} }}", psoId, GetParentId(), HResultToString(hr));
	}

	std::string CreateStateObjectCommand::GetDesc() const
	{
		return std::format("CreateStateObject(..., obj#{}) {{ this -> Device obj#{}, return -> {} }}", stateObjectId, GetParentId(), HResultToString(hr));
	}

	std::string AddToStateObjectCommand::GetDesc() const
	{
		return std::format("AddToStateObject(..., obj#{}) {{ this -> Device obj#{}, return -> {} }}", stateObjectId, GetParentId(), HResultToString(hr));
	}
	std::string CreateCommittedResourceCommand::GetDesc() const
	{
		return std::format("CreateCommittedResource(..., obj#{}) {{ this -> Device obj#{}, return -> {} }}", resourceId, GetParentId(), HResultToString(hr));
	}

	std::string CreateCommittedResource1Command::GetDesc() const
	{
		return std::format("CreateCommittedResource1(..., obj#{}) {{ this -> Device obj#{}, return -> {} }}", resourceId, GetParentId(), HResultToString(hr));
	}

	std::string CreatePlacedResourceCommand::GetDesc() const
	{
		return std::format("CreatePlacedResource(..., obj#{}) {{ this -> Device obj#{}, return -> {} }}", resourceId, GetParentId(), HResultToString(hr));
	}

	std::string CreatePlacedResource1Command::GetDesc() const
	{
		return std::format("CreatePlacedResource1(..., obj#{}) {{ this -> Device obj#{}, return -> {} }}", resourceId, GetParentId(), HResultToString(hr));
	}

	std::string CreateDescriptorHeapCommand::GetDesc() const
	{
		return std::format("CreateDescriptorHeap(..., obj#{}) {{ this -> Device obj#{}, return -> {} }}", descriptorHeapId, GetParentId(), HResultToString(hr));
	}


}

