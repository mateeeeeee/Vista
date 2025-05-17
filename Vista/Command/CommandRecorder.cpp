#include "CommandRecorder.h"
#include "CommandVisitor.h"

namespace vista
{

	void CommandRecorder::Accept(CommandVisitor& v) const
	{
		for (Command const& cmd : commands)
		{
			cmd.Accept(v);
		}
	}
}

