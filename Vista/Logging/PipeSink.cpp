#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "PipeSink.h"

namespace vista
{
	PipeSink::PipeSink(std::wstring const& pipeName, LogLevel logLevel)
		: logLevel(logLevel)
	{
		pipeHandle = CreateFileW(
			pipeName.c_str(),
			GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			0,
			NULL
		);
	}

	PipeSink::~PipeSink()
	{
		if (pipeHandle != INVALID_HANDLE_VALUE)
		{
			FlushFileBuffers(pipeHandle);
			CloseHandle(pipeHandle);
			pipeHandle = INVALID_HANDLE_VALUE;
		}
	}

	void PipeSink::Log(LogLevel level, Char const* entry, Char const* file, Uint32 line)
	{
		if (level < logLevel || pipeHandle == INVALID_HANDLE_VALUE)
		{
			return;
		}

		std::string log = GetLogTime() + LineInfoToString(file, line) + LevelToString(level) + std::string(entry) + "\n";
		DWORD bytesWritten;
		WriteFile(pipeHandle, log.c_str(), static_cast<DWORD>(log.size()), &bytesWritten, NULL);
	}

	void PipeSink::Flush()
	{
		if (pipeHandle != INVALID_HANDLE_VALUE)
		{
			FlushFileBuffers(pipeHandle);
		}
	}
}

