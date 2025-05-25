#include "ConsoleSink.h"
#include <cstdio>             

namespace vista
{

	ConsoleSink::ConsoleSink(Bool useCerr, LogLevel log_level)
		: useCerr{ useCerr }, logLevel{ logLevel }
	{
	}

	ConsoleSink::~ConsoleSink()
	{
		Flush(); 
	}

	void ConsoleSink::Log(LogLevel level, Char const* entry, Char const* file, Uint32 line)
	{
		if (level < logLevel)
		{
			return;
		}
		FILE* targetStream = useCerr ? stderr : stdout;
		std::string timeString = GetLogTime();
		std::string lineInfoString = LineInfoToString(file, line);
		std::string levelString = LevelToString(level);

		fprintf(targetStream, "%s%s%s%s\n",
			timeString.c_str(),
			lineInfoString.c_str(),
			levelString.c_str(),
			entry); 
	}

	void ConsoleSink::Flush()
	{
		FILE* targetStream = useCerr ? stderr : stdout;
		fflush(targetStream);
	}
} 