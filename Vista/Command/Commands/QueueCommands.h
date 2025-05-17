#pragma once
#include "Command/Command.h"

namespace vista
{

	class QueueCommand : public Command
	{
	public:
		virtual CommandCategory GetCategory() const override { return CommandCategory::Queue; }

		COMMAND_CLASS_OF_RANGE(QueueCommand)

	protected:
		QueueCommand() {}
	};


	class CommandRecorder;
	class ExecuteCommandListsCommand : public QueueCommand
	{
	public:
		ExecuteCommandListsCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::ExecuteCmdLists; }
		virtual Bool IsCPUTimeline() const { return true; }

		COMMAND_ACCEPT_IMPL()
		
		virtual Uint64 GetHash() const override;

		static Bool ClassOf(Command const* C)
		{
			using enum CommandType;
			return C->GetType() == ExecuteCmdLists;
		}

	public:
		std::vector<CommandRecorder*> recorders;
	};

	class QueueSignalCommand : public QueueCommand
	{
	public:
		QueueSignalCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::Queue_Signal; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL_WITH_PARENT_ID(0)

		static Bool ClassOf(Command const* C)
		{
			using enum CommandType;
			return C->GetType() == Queue_Signal;
		}

	public:
		ObjectID fenceId = InvalidObjectID;
		Uint64 fenceValue = 0;
		HRESULT hr = S_OK;
	};
}