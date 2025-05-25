#include "CallbackSink.h"

namespace vista
{

	CallbackSink::CallbackSink(LogCallbackT callback, LogLevel logLevel) : logLevel(logLevel)
	{
	}

	CallbackSink::~CallbackSink() {}

	void CallbackSink::Log(LogLevel level, Char const* entry, Char const* file, Uint32 line)
	{
		if (level < logLevel || logCallback == nullptr)
		{
			return;
		}
		logCallback(level, entry, file, line);
	}
}

