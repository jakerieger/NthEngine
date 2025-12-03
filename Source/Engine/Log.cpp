/// @author Jake Rieger
/// @created 11/27/25
///
#include "Log.hpp"

namespace Nth {
    shared_ptr<spdlog::logger> Log::sLogger;

    void Log::Initialize() {
#ifdef _WIN32
        // Enable ANSI escape codes on Windows 10+
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE) {
            DWORD dwMode = 0;
            if (GetConsoleMode(hOut, &dwMode)) {
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                SetConsoleMode(hOut, dwMode);
            }
        }
#endif

        // Create a color console sink
        auto consoleSink = make_shared<spdlog::sinks::stdout_color_sink_mt>();

        // Set the pattern: [HH:MM:SS] [level] message
        consoleSink->set_pattern("[%T] [%^%l%$] %v");

        // Create logger with the sink
        sLogger = std::make_shared<spdlog::logger>("main", consoleSink);
#ifndef NDEBUG
        sLogger->set_level(spdlog::level::trace);
        sLogger->flush_on(spdlog::level::trace);
#else
        sLogger->set_level(spdlog::level::info);
        sLogger->flush_on(spdlog::level::info);
#endif

        spdlog::register_logger(sLogger);
        spdlog::set_default_logger(sLogger);

        Info("Log", "Logging system initialized");
    }

    void Log::Shutdown() {
        if (sLogger) {
            Info("Log", "Shutting down logging system");
            sLogger->flush();
            spdlog::drop_all();
            sLogger.reset();
        }
    }

    shared_ptr<spdlog::logger> Log::GetLogger() {
        if (!sLogger) { Initialize(); }
        return sLogger;
    }
}  // namespace Nth