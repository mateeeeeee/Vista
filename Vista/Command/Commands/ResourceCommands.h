#pragma once
#include "Command/Command.h"

namespace vista
{
	class ResourceCommand : public Command
	{
	public:
		virtual CommandCategory GetCategory() const override { return CommandCategory::Resource; }
		virtual Bool IsCPUTimeline() const { return true; }

		COMMAND_CLASS_OF_RANGE(ResourceCommand)

	protected:
		ResourceCommand() {}
	};

	class ResourceSetNameCommand : public ResourceCommand
	{
	public:
		ResourceSetNameCommand() = default;

		virtual std::string GetDesc() const override;
		virtual CommandType GetType() const override { return CommandType::Resource_SetName; }

		COMMAND_ACCEPT_IMPL()
		COMMAND_HASH_IMPL(resourceName)

		static Bool ClassOf(Command const* C)
		{
			using enum CommandType;
			return C->GetType() == Resource_SetName;
		}

	public:
		std::string resourceName = "";
		HRESULT hr = S_OK;
	};
}