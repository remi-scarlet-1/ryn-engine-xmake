#pragma once

/* Shared Library Management */
#if RYN_PLATFORM_WIN
    #if RYN_RUNTIME
        #define RYN_API __declspec(dllexport)
    #else
        #define RYN_API __declspec(dllimport)
    #endif
#elif RYN_PLATFORM_LINUX
    #if RYN_RUNTIME
        #define RYN_API __attribute__((visibility("default")))
    #else
        #define RYN_API
    #endif
#else
    #error "Unknown platform. Shared library exports are undefined for this platform."
#endif

/* Features */
#if defined(RYN_LEAN_AND_MEAN)
    #define RYN_FEATURE_LOGGING 0
    #define RYN_FEATURE_VERIFY  0
    #define RYN_FEATURE_ASSERTS 0
#elif RYN_CONFIG_DEBUG
    #define RYN_FEATURE_LOGGING 1
    #define RYN_FEATURE_VERIFY  1
    #define RYN_FEATURE_ASSERTS 1
#elif RYN_CONFIG_RELEASE
    #define RYN_FEATURE_LOGGING 1
    #define RYN_FEATURE_VERIFY  1
#elif RYN_CONFIG_DIST
    #define RYN_FEATURE_LOGGING 1
    #define RYN_FEATURE_VERIFY  1
#else
    #error "Unknown configuration. Check CMake."
#endif

/* Common */
#if _MSC_VER >= 1926
    #define RYN_LINE     __builtin_LINE()
    #define RYN_FILE     __builtin_FILE()
    #define RYN_FUNCTION __builtin_FUNCTION()
#elif defined(__has_builtin) && __has_builtin(__builtin_LINE) && __has_builtin(__builtin_FILE) && __has_builtin(__builtin_FUNCTION)
    #define RYN_LINE     __builtin_LINE()
    #define RYN_FILE     __builtin_FILE()
    #define RYN_FUNCTION __builtin_FUNCTION()
#else
    #define RYN_LINE     __LINE__
    #define RYN_FILE     __FILE__
    #define RYN_FUNCTION __func__
#endif

#include <cstdlib>
#define RYN_ABORT() abort()

#if _MSC_VER
    #define RYN_DEBUG_BREAK() __debugbreak()
#elif defined(__has_builtin) && __has_builtin(__builtin_trap)
    #define RYN_DEBUG_BREAK() __builtin_trap()
#else
    #define RYN_DEBUG_BREAK() (*((volatile int*)0) = 0)
#endif

namespace ryn::detail {
    consteval const char* stripFilePath(const char* path) {
        const char* lastSlash = path;
        for (const char* p = path; *p; ++p)
            if (*p == '/' || *p == '\\') lastSlash = p + 1;
        return lastSlash;
    }
}

/* Feature: Logging */
#if RYN_FEATURE_LOGGING
import Ryntime.Logger;

    #define RYN_LOG(Level, Fmt, ...)                                                                                                       \
        do {                                                                                                                               \
            constexpr ryn::LogMeta EMBEDDED_LOG_META = ryn::LogMeta{RYN_FILE, RYN_FUNCTION, RYN_LINE, Level};                              \
            ryn::Logger::message(EMBEDDED_LOG_META, Fmt __VA_OPT__(, ) __VA_ARGS__);                                                       \
        } while (0)

    #define RYN_TRACE(Fmt, ...)    RYN_LOG(ryn::LogLevel::Trace, Fmt __VA_OPT__(, ) __VA_ARGS__)
    #define RYN_INFO(Fmt, ...)     RYN_LOG(ryn::LogLevel::Info, Fmt __VA_OPT__(, ) __VA_ARGS__)
    #define RYN_WARN(Fmt, ...)     RYN_LOG(ryn::LogLevel::Warn, Fmt __VA_OPT__(, ) __VA_ARGS__)
    #define RYN_ERROR(Fmt, ...)    RYN_LOG(ryn::LogLevel::Error, Fmt __VA_OPT__(, ) __VA_ARGS__)
    #define RYN_CRITICAL(Fmt, ...) RYN_LOG(ryn::LogLevel::Critical, Fmt __VA_OPT__(, ) __VA_ARGS__)
#else
    #define RYN_LOG(...)      (void)0
    #define RYN_TRACE(...)    (void)0
    #define RYN_INFO(...)     (void)0
    #define RYN_WARN(...)     (void)0
    #define RYN_ERROR(...)    (void)0
    #define RYN_CRITICAL(...) (void)0
#endif

/* Feature: Verify */
#if RYN_FEATURE_VERIFY
    #define RYN_VERIFY(Check, ...)                                                                                                         \
        do {                                                                                                                               \
            if (!(Check)) {                                                                                                                \
                RYN_CRITICAL("Verify Failed" __VA_OPT__(": ") __VA_ARGS__);                                                                \
                RYN_ABORT();                                                                                                               \
            }                                                                                                                              \
        } while (0)
#else
    #define RYN_VERIFY(Check, ...) (void)(Check)
#endif

/* Feature: Asserts */
#if RYN_FEATURE_ASSERTS
    #define RYN_ASSERT(Check, ...)                                                                                                         \
        do {                                                                                                                               \
            if (!(Check)) {                                                                                                                \
                RYN_CRITICAL("Assertion Failed" __VA_OPT__(": ") __VA_ARGS__);                                                             \
                RYN_CRITICAL("  at {0}:{1}", ::ryn::detail::stripFilePath(RYN_FILE), RYN_LINE);                                            \
                RYN_DEBUG_BREAK();                                                                                                         \
            }                                                                                                                              \
        } while (0)
#else
    #define RYN_ASSERT(Check, ...)
#endif