#pragma once
#include <memory> 
#include <string>
#include <source_location>

namespace vista
{
	enum class LogLevel : Uint8
	{
		LOG_DEBUG,
		LOG_INFO,
		LOG_WARNING,
		LOG_ERROR
	};

	std::string LevelToString(LogLevel type);
	std::string GetLogTime();
	std::string LineInfoToString(Char const* file, Uint32 line);

	class ILogSink
	{
	public:
		virtual ~ILogSink() = default;
		virtual void Log(LogLevel level, Char const* entry, Char const* file, Uint32 line) = 0;
		virtual void Flush() {}
	};

	class LogManager
	{
	public:
		LogManager();
		VISTA_NONCOPYABLE_NONMOVABLE(LogManager)
		~LogManager();

		template<typename LogSinkT, typename... Args> requires std::is_base_of_v<ILogSink, LogSinkT>
		VISTA_MAYBE_UNUSED LogSinkT* Register(Args&&... args)
		{
			Register(new LogSinkT(std::forward<Args>(args)...));
			return static_cast<LogSinkT*>(GetLastSink());
		}
		void Log(LogLevel level, Char const* str, Char const* file, Uint32 line);
		void LogSync(LogLevel level, Char const* str, Char const* file, Uint32 line);
		void Flush();

	private:
		std::unique_ptr<class LogManagerImpl> pimpl;

	private:
		void Register(ILogSink* logger);
		ILogSink* GetLastSink();
	};
	inline LogManager g_Log{};

#define VISTA_LOG(level, ... ) [&]()  \
	{ \
		Uint64 const size = snprintf(nullptr, 0, __VA_ARGS__) + 1; \
		std::unique_ptr<Char[]> buf = std::make_unique<Char[]>(size); \
		snprintf(buf.get(), size, __VA_ARGS__); \
		g_Log.Log(LogLevel::LOG_##level, buf.get(), __FILE__, __LINE__);  \
	}()

#define VISTA_LOG_SYNC(level, ... ) [&]()  \
	{ \
		Uint64 const size = snprintf(nullptr, 0, __VA_ARGS__) + 1; \
		std::unique_ptr<Char[]> buf = std::make_unique<Char[]>(size); \
		snprintf(buf.get(), size, __VA_ARGS__); \
		g_Log.LogSync(LogLevel::LOG_##level, buf.get(), __FILE__, __LINE__);  \
	}()
#define VISTA_LOG_FLUSH()   (g_Log.Flush())
#define VISTA_SINK(SinkClass, ...) g_Log.Register<SinkClass>(__VA_ARGS__);
}