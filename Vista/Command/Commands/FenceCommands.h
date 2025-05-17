#pragma once
#include "Command/Command.h"

namespace vista
{
	class FenceCommand : public Command
	{
	public:
		virtual CommandCategory GetCategory() const override { return CommandCategory::Fence; }
		virtual Bool IsCPUTimeline() const { return true; }

		COMMAND_CLASS_OF_RANGE(FenceCommand)

	protected:
		FenceCommand() {}
	};

	class GetCompletedValueCommand : public FenceCommand
	{
	public:
		GetCompletedValueCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::GetCompletedValue; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(GetType())

		static Bool ClassOf(Command const* C)
		{
			using enum CommandType;
			return C->GetType() == GetCompletedValue;
		}

	public:
		Uint64 completedValue = 0;
	};

	class SetEventOnCompletionCommand : public FenceCommand
	{
	public:
		SetEventOnCompletionCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::SetEventOnCompletion; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(event)

		static Bool ClassOf(Command const* C)
		{
			using enum CommandType;
			return C->GetType() == SetEventOnCompletion;
		}

	public:
		Uint64 value = 0;
		HANDLE event = nullptr;
		HRESULT hr = S_OK;
	};

	class FenceSignalCommand : public FenceCommand
	{
	public:
		FenceSignalCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::Fence_Signal; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(0)

		static Bool ClassOf(Command const* C)
		{
			using enum CommandType;
			return C->GetType() == Fence_Signal;
		}

	public:
		Uint64 value = 0;
		HRESULT hr = S_OK;
	};
}