#include <format>
#include "ResourceCommands.h"
#include "Utilities/StringUtil.h"

namespace vista
{

	std::string ResourceSetNameCommand::GetDesc() const
	{
		return std::format("SetName ({})  {{ this -> Resource obj#{}, return -> {} }}", resourceName, GetParentId(), HResultToString(hr));
	}
}
