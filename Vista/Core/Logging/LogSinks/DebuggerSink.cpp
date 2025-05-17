#include "DebuggerSink.h"

namespace vista
{
	DebuggerSink::DebuggerSink(LogLevel logLevel)
		: logLevel{ logLevel }
	{}

	DebuggerSink::~DebuggerSink()
	{}

	void DebuggerSink::Log(LogLevel level, Char const* entry, Char const* file, Uint32 line)
	{
		if (level < logLevel)
		{
			return;
		}
		std::string log = GetLogTime() + LineInfoToString(file, line) + LevelToString(level) + std::string(entry) + "\n";
		OutputDebugStringA(log.c_str());
	}

	void DebuggerSink::Flush()
	{
		
	}

}

