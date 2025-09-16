#include <format>
#include "QueueCommands.h"
#include "Command/CommandRecorder.h"
#include "Utilities/StringConversions.h"

namespace vista
{
	std::string ExecuteCommandListsCommand::GetDesc() const
	{
		return std::format("ExecuteCommandLists ({}, ...)  {{ this -> CommandQueue obj#{} }}", recorders.size(), GetParentId());
	}

	Uint64 ExecuteCommandListsCommand::GetHash() const
	{
		HashState hashState{};
		hashState.Combine(recorders.size());
		return hashState;
	}

	std::string QueueSignalCommand::GetDesc() const
	{
		return std::format("Signal (obj#{}, {})   {{ this -> CommandQueue obj#{}, return -> {} }}", fenceId, fenceValue, GetParentId(), HResultToString(hr));
	}
}

