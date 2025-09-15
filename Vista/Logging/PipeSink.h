#pragma once
#include "Logging/Log.h"

typedef void* HANDLE;

namespace vista
{
	class PipeSink : public ILogSink
	{
	public:
		explicit PipeSink(std::wstring const& pipeName, LogLevel logLevel = LogLevel::LOG_DEBUG);
		~PipeSink() override;

		void Log(LogLevel level, Char const* entry, Char const* file, Uint32 line) override;
		void Flush() override;

	private:
		LogLevel const& logLevel;
		HANDLE pipeHandle;
	};
}
