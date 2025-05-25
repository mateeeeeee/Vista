#pragma once
#include "Core/Logging/Log.h"

namespace vista
{
	using LogCallbackT = void(*)(LogLevel level, Char const* entry, Char const* file, Uint32 line);

	class CallbackSink : public ILogSink
	{
	public:
		CallbackSink(LogCallbackT callback, LogLevel logLevel = LogLevel::LOG_DEBUG);
		virtual ~CallbackSink() override;
		virtual void Log(LogLevel level, Char const* entry, Char const* file, Uint32 line) override;

	private:
		LogCallbackT   logCallback;
		LogLevel const logLevel;
	};

}