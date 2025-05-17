#include "Log.h"
#include <chrono>
#include <ctime>   
#include <vector>
#include <thread>
#include "Utilities/ConcurrentQueue.h"

namespace vista
{
	struct QueueEntry
	{
		LogLevel level;
		std::string str;
		std::string filename;
		Uint32 line;
	};

	class LogManagerImpl
	{
	public:

		LogManagerImpl() : logThread(&LogManagerImpl::ProcessLogs, this) {}
		~LogManagerImpl()
		{
			exit.store(true);
			logThread.join();
		}

		void RegisterLogger(ILogSink* logger)
		{
			logSinks.emplace_back(logger);
		}
		ILogSink* GetLastSink() const
		{
			return logSinks.back().get();
		}
		void Log(LogLevel level, Char const* str, Char const* filename, Uint32 line)
		{
			logQueue.Push(QueueEntry{ level, str, filename, line });
		}
		void LogSync(LogLevel level, Char const* str, Char const* filename, Uint32 line)
		{
			for (auto& log_sink : logSinks)
			{
				if (log_sink)
				{
					log_sink->Log(level, str, filename, line);
				}
			}
		}
		void Flush()
		{
			pause.store(true);
			for (auto& logger : logSinks) logger->Flush();
			pause.store(false);
		}

		std::vector<std::unique_ptr<ILogSink>> logSinks;
		ConcurrentQueue<QueueEntry> logQueue;
		std::thread logThread;
		std::atomic_bool exit = false;
		std::atomic_bool pause = false;

	private:
		void ProcessLogs()
		{
			QueueEntry entry{};
			while (true)
			{
				if (pause.load())
				{
					continue;
				}

				Bool success = logQueue.TryPop(entry);
				if (success)
				{
					for (auto& log_sink : logSinks)
					{
						if (log_sink)
						{
							log_sink->Log(entry.level, entry.str.c_str(), entry.filename.c_str(), entry.line);
						}
					}
				}
				if (exit.load() && logQueue.Empty())
				{
					break;
				}
			}
		}
	};

	std::string LevelToString(LogLevel type)
	{
		switch (type)
		{
		case LogLevel::LOG_DEBUG:
			return "[DEBUG]";
		case LogLevel::LOG_INFO:
			return "[INFO]";
		case LogLevel::LOG_WARNING:
			return "[WARNING]";
		case LogLevel::LOG_ERROR:
			return "[ERROR]";
		}
		return "";
	}
	std::string GetLogTime()
	{
		auto time = std::chrono::system_clock::now();
		time_t cTime = std::chrono::system_clock::to_time_t(time);
		std::string timeString = std::string(ctime(&cTime));
		timeString.pop_back();
		return "[" + timeString + "]";
	}
	std::string LineInfoToString(Char const* file, Uint32 line)
	{
		return "[File: " + std::string(file) + "  Line: " + std::to_string(line) + "]";
	}

	LogManager::LogManager() : pimpl(new LogManagerImpl) {}
	LogManager::~LogManager() = default;

	void LogManager::Register(ILogSink* logger)
	{
		pimpl->RegisterLogger(logger);
	}

	ILogSink* LogManager::GetLastSink()
	{
		return pimpl->GetLastSink();
	}

	void LogManager::Log(LogLevel level, Char const* str, Char const* filename, Uint32 line)
	{
		pimpl->Log(level, str, filename, line);
	}

	void LogManager::LogSync(LogLevel level, Char const* str, Char const* filename, Uint32 line)
	{
		pimpl->LogSync(level, str, filename, line);
	}

	void LogManager::Flush()
	{
		pimpl->Flush();
	}

}