#pragma once
#include <cstdio>
#include "Logging/Log.h"

namespace vista
{
	class FileSink : public ILogSink
	{
	public:
		FileSink(Char const* log_file, LogLevel logLevel = LogLevel::LOG_DEBUG, Bool appendMode = false);
		virtual ~FileSink() override;
		VISTA_NONCOPYABLE_NONMOVABLE(FileSink)

		virtual void Log(LogLevel level, Char const* entry, Char const* file, Uint32 line) override;
		virtual void Flush() override;

	private:
		FILE* logHandle = nullptr;
		LogLevel const logLevel;
	};
}