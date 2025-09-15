#pragma once
#include "Logging/Log.h"

namespace vista
{
	class ConsoleSink : public ILogSink
	{
	public:
		ConsoleSink(Bool useCerr = false, LogLevel logLevel = LogLevel::LOG_DEBUG);
		virtual ~ConsoleSink() override;
		VISTA_NONCOPYABLE_NONMOVABLE(ConsoleSink)

		virtual void Log(LogLevel level, Char const* entry, Char const* file, Uint32 line) override;
		virtual void Flush() override;

	private:
		Bool const useCerr;
		LogLevel const logLevel;
	};

} 