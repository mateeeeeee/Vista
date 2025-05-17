#pragma once
#include "Command/Command.h"

namespace vista
{
	class DeviceCommand : public Command
	{
	public:
		virtual CommandCategory GetCategory() const override { return CommandCategory::Device; }
		virtual Bool IsCPUTimeline() const { return true; }

		COMMAND_CLASS_OF_RANGE(DeviceCommand)

	protected:
		DeviceCommand() {}
	};

	class CreateCommandQueueCommand : public DeviceCommand
	{
	public:
		CreateCommandQueueCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::CreateCommandQueue; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL_WITH_PARENT_ID(commandQueueId)

		static Bool ClassOf(Command const* C)
		{
			return C->GetType() == CommandType::CreateCommandQueue;
		}

	public:
		ObjectID commandQueueId = InvalidObjectID;
		HRESULT hr = S_OK;
	};


	class CreateCommandListCommand : public DeviceCommand
	{
	public:
		CreateCommandListCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::CreateCommandList; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL_WITH_PARENT_ID(commandListId)

		static Bool ClassOf(Command const* C)
		{
			using enum CommandType;
			return C->GetType() == CreateCommandList;
		}

	public:
		ObjectID commandListId = InvalidObjectID;
		HRESULT hr = S_OK;
	};

	class CreateCommandAllocatorCommand : public DeviceCommand
	{
	public:
		CreateCommandAllocatorCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::CreateCommandAllocator; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL_WITH_PARENT_ID(commandAllocatorId)

		static Bool ClassOf(Command const* C)
		{
			using enum CommandType;
			return C->GetType() == CreateCommandAllocator;
		}

	public:
		ObjectID commandAllocatorId = InvalidObjectID;
		HRESULT hr = S_OK;
	};

	class CreateHeapCommand : public DeviceCommand
	{
	public:
		CreateHeapCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::CreateHeap; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL_WITH_PARENT_ID(heapId)

		static Bool ClassOf(Command const* C)
		{
			using enum CommandType;
			return C->GetType() == CreateHeap;
		}

	public:
		ObjectID heapId = InvalidObjectID;
		HRESULT hr = S_OK;
	};

	class CreateFenceCommand : public DeviceCommand
	{
	public:
		CreateFenceCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::CreateFence; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL_WITH_PARENT_ID(fenceId)

		static Bool ClassOf(Command const* C)
		{
			using enum CommandType;
			return C->GetType() == CreateFence;
		}

	public:
		ObjectID fenceId = InvalidObjectID;
		HRESULT hr = S_OK;
	};

	class CreateGraphicsPipelineStateCommand : public DeviceCommand
	{
	public:
		CreateGraphicsPipelineStateCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::CreateGraphicsPSO; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL_WITH_PARENT_ID(psoId)

		static Bool ClassOf(Command const* C)
		{
			using enum CommandType;
			return C->GetType() == CreateGraphicsPSO;
		}

	public:
		ObjectID psoId = InvalidObjectID;
		HRESULT hr = S_OK;
	};

	class CreateComputePipelineStateCommand : public DeviceCommand
	{
	public:
		CreateComputePipelineStateCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::CreateComputePSO; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL_WITH_PARENT_ID(psoId)

		static Bool ClassOf(Command const* C)
		{
			using enum CommandType;
			return C->GetType() == CreateComputePSO;
		}

	public:
		ObjectID psoId = InvalidObjectID;
		HRESULT hr = S_OK;
	};

	class CreateRootSignatureCommand : public DeviceCommand
	{
	public:
		CreateRootSignatureCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::CreateRootSignature; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL_WITH_PARENT_ID(rootSignatureId)

		static Bool ClassOf(Command const* C)
		{
			using enum CommandType;
			return C->GetType() == CreateRootSignature;
		}

	public:
		ObjectID rootSignatureId = InvalidObjectID;
		HRESULT hr = S_OK;
	};

	class CreateCommandSignatureCommand : public DeviceCommand
	{
	public:
		CreateCommandSignatureCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::CreateCommandSignature; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL_WITH_PARENT_ID(cmdSignatureId)

		static Bool ClassOf(Command const* C)
		{
			using enum CommandType;
			return C->GetType() == CreateCommandSignature;
		}

	public:
		ObjectID cmdSignatureId = InvalidObjectID;
		HRESULT hr = S_OK;
	};

	class CreateCommittedResourceCommand : public DeviceCommand
	{
	public:
		CreateCommittedResourceCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::CreateCommittedResource; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL_WITH_PARENT_ID(resourceId)

		static Bool ClassOf(Command const* C)
		{
			using enum CommandType;
			return C->GetType() == CreateCommittedResource;
		}

	public:
		ObjectID resourceId = InvalidObjectID;
		HRESULT hr = S_OK;
	};

	class CreatePlacedResourceCommand : public DeviceCommand
	{
	public:
		CreatePlacedResourceCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::CreatePlacedResource; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL_WITH_PARENT_ID(resourceId)

		static Bool ClassOf(Command const* C)
		{
			using enum CommandType;
			return C->GetType() == CreatePlacedResource;
		}

	public:
		ObjectID resourceId = InvalidObjectID;
		HRESULT hr = S_OK;
	};


	class CreateDescriptorHeapCommand : public DeviceCommand
	{
	public:
		CreateDescriptorHeapCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::CreateDescriptorHeap; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL_WITH_PARENT_ID(descriptorHeapId)

		static Bool ClassOf(Command const* C)
		{
			using enum CommandType;
			return C->GetType() == CreateDescriptorHeap;
		}

	public:
		ObjectID descriptorHeapId = InvalidObjectID;
		HRESULT hr = S_OK;
	};
}