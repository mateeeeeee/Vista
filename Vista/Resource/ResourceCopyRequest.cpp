#include "ResourceCopyRequest.h"
#include "Command/Command.h"

namespace vista
{
	Bool ResourceCopyRequest::IsMatch(Command const& cmd) const
	{
		return commandIndex == cmd.GetCommandIndex() && commandType == (Uint32)cmd.GetType();
	}
}
