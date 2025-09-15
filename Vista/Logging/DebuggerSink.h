#pragma once
#include "Logging/Log.h"

namespace vista
{
	class DebuggerSink : public ILogSink
	{
	public:
		DebuggerSink(LogLevel logLevel = LogLevel::LOG_DEBUG);
		virtual ~DebuggerSink() override;
		virtual void Log(LogLevel level, Char const* entry, Char const* file, Uint32 line) override;
		virtual void Flush() override;

	private:
		LogLevel const logLevel;
	};

}