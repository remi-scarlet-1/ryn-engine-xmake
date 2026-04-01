#pragma once

/* Configuration Features */
#if RYN_CONFIG_DEBUG
    #define RYN_FEATURE_LOGGING 1
    #define RYN_FEATURE_VERIFY 1
    #define RYN_FEATURE_ASSERTS 1

#elif RYN_CONFIG_RELEASE
    #define RYN_FEATURE_LOGGING 1
    #define RYN_FEATURE_VERIFY 1
    
#elif RYN_CONFIG_DIST
    #define RYN_FEATURE_VERIFY 1

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

/* Feature: Logging */
#if RYN_FEATURE_LOGGING
    // FIXME: restore logging macros
    //  e.g.: RYN_LOG(Trace, Fmt __VA_OPT__(, ) __VA_ARGS__)
    #define RYN_TRACE(Fmt, ...)    (void)0
    #define RYN_INFO(Fmt, ...)     (void)0
    #define RYN_WARN(Fmt, ...)     (void)0
    #define RYN_ERROR(Fmt, ...)    (void)0
    #define RYN_CRITICAL(Fmt, ...) (void)0
#else
    #define RYN_TRACE(Fmt, ...)    (void)0
    #define RYN_INFO(Fmt, ...)     (void)0
    #define RYN_WARN(Fmt, ...)     (void)0
    #define RYN_ERROR(Fmt, ...)    (void)0
    #define RYN_CRITICAL(Fmt, ...) (void)0
#endif

/* Feature: Verify */
#if RYN_FEATURE_VERIFY
#define RYN_VERIFY(Check, ...)                                                                                                              \
        do {                                                                                                                                \
            if (!(Check)) {                                                                                                                 \
                RYN_CRITICAL("Verify Failed" __VA_OPT__(": ") __VA_ARGS__);                                                                 \
                RYN_ABORT();                                                                                                                \
            }                                                                                                                               \
        } while (0)
#else
    #define RYN_VERIFY(Check, ...) (void)(Check)
#endif

/* Feature: Asserts */
#if RYN_FEATURE_ASSERTS
    namespace ryn::detail {
        consteval const char* assertFilePath(const char* path) {
            const char* lastSlash = path;
            for (const char* p = path; *p; ++p)
                if (*p == '/' || *p == '\\') lastSlash = p + 1;
            return lastSlash;
        }
    }

    #define RYN_ASSERT(Check, ...)                                                                                                          \
        do {                                                                                                                                \
            if (!(Check)) {                                                                                                                 \
                RYN_CRITICAL("Assertion Failed" __VA_OPT__(": ") __VA_ARGS__);                                                              \
                RYN_CRITICAL("  at {0}:{1}", ryn::detail::assertFilePath(RYN_FILE), RYN_LINE);                                              \
                RYN_DEBUG_BREAK();                                                                                                          \
            }
        } while (0)
#else
    #define RYN_ASSERT(Check, ...)    
#endif