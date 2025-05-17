#include <format>
#include "FenceCommands.h"
#include "Utilities/StringUtil.h"

namespace vista
{
	std::string GetCompletedValueCommand::GetDesc() const
	{
		return std::format("GetCompletedValue() {{ this -> Fence obj#{}, return -> {} }}", GetParentId(), completedValue);
	}

	std::string SetEventOnCompletionCommand::GetDesc() const
	{
		return std::format("SetEventOnCompletion({}, {}) {{ this -> Fence obj#{}, result -> {} }}", value, event, GetParentId(), HResultToString(hr));
	}

	std::string FenceSignalCommand::GetDesc() const
	{
		return std::format("Signal({}) {{ this -> Fence obj#{}, result -> {} }}", value, GetParentId(), HResultToString(hr));
	}
}

