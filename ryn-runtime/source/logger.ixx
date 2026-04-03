module;
#define RYN_LEAN_AND_MEAN
#include "defines.h"

export module Ryntime.Logger;
export import Ryntime.Container.HybridBuffer;
import std;

export namespace ryn {
    enum class LogLevel : int32_t {
        Trace    = 0,
        Info     = 1,
        Warn     = 2,
        Error    = 3,
        Critical = 4
    };

    struct LogMeta {
        const char* File;
        const char* Function;
        int32_t     Line;
        LogLevel    Level;
    };

    class RYN_API Logger {
    public:
        static void init();
        static void shutdown();

        template<typename... Args>
        static void message(const LogMeta& meta, std::format_string<Args...> fmt, Args&&... args) {
            HybridBuffer<char, 1024> buffer;
            std::vformat_to(std::back_inserter(buffer), fmt.get(), std::make_format_args(args...));
            buffer.push_back('\0');
            dispatch(meta, buffer.data());
        }

    private:
        static void dispatch(const LogMeta& meta, const char* message);
    };
}