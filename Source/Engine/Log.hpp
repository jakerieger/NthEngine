/// @author Jake Rieger
/// @created 11/27/25
///

#pragma once

#include "CommonPCH.hpp"

namespace Nth {
    class Log {
    public:
        static void Initialize();
        static void Shutdown();

        template<typename... Args>
        static void Trace(const std::string& subsystem, fmt::format_string<Args...> fmt, Args&&... args) {
            GetLogger()->trace("\033[1m{}\033[0m: {}", subsystem, fmt::format(fmt, std::forward<Args>(args)...));
        }

        template<typename... Args>
        static void Debug(const std::string& subsystem, fmt::format_string<Args...> fmt, Args&&... args) {
            GetLogger()->debug("\033[1m{}\033[0m: {}", subsystem, fmt::format(fmt, std::forward<Args>(args)...));
        }

        template<typename... Args>
        static void Info(const std::string& subsystem, fmt::format_string<Args...> fmt, Args&&... args) {
            GetLogger()->info("\033[1m{}\033[0m: {}", subsystem, fmt::format(fmt, std::forward<Args>(args)...));
        }

        template<typename... Args>
        static void Warn(const std::string& subsystem, fmt::format_string<Args...> fmt, Args&&... args) {
            GetLogger()->warn("\033[1m{}\033[0m: {}", subsystem, fmt::format(fmt, std::forward<Args>(args)...));
        }

        template<typename... Args>
        static void Error(const std::string& subsystem, fmt::format_string<Args...> fmt, Args&&... args) {
            GetLogger()->error("\033[1m{}\033[0m: {}", subsystem, fmt::format(fmt, std::forward<Args>(args)...));
        }

        template<typename... Args>
        static void Critical(const std::string& subsystem, fmt::format_string<Args...> fmt, Args&&... args) {
            GetLogger()->critical("\033[1m{}\033[0m: {}", subsystem, fmt::format(fmt, std::forward<Args>(args)...));
        }

    private:
        static shared_ptr<spdlog::logger> GetLogger();
        static shared_ptr<spdlog::logger> sLogger;
    };
}  // namespace Nth
