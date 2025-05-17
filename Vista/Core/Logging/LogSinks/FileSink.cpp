#include "FileSink.h"

namespace vista
{

	FileSink::FileSink(Char const* log_file, LogLevel logLevel, Bool appendMode)
		: logLevel{ logLevel }
	{
		std::string fullPath = log_file;
		Char const* mode = appendMode ? "a" : "w";
		logHandle = fopen(fullPath.c_str(), mode);
	}

	FileSink::~FileSink()
	{
		if (logHandle)
		{
			fflush(logHandle); 
			fclose(logHandle);
			logHandle = nullptr; 
		}
	}

	void FileSink::Log(LogLevel level, Char const* entry, Char const* file, Uint32 line)
	{
		if (level < logLevel || !logHandle)
		{
			return;
		}

		std::string timeString = GetLogTime();
		std::string lineInfoString = LineInfoToString(file, line);
		std::string levelString = LevelToString(level);
		fprintf(logHandle, "%s%s%s%s\n",
			timeString.c_str(),
			lineInfoString.c_str(),
			levelString.c_str(),
			entry); 
	}

	void FileSink::Flush()
	{
		if (logHandle)
		{
			fflush(logHandle); 
		}
	}
} 