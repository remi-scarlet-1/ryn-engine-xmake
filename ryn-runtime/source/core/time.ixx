module;
#include "defines.h"

export module Ryntime.Core:Time;
import std;

export namespace ryn {
    class [[nodiscard]] Timer {
    public:
        inline Timer() noexcept : m_Start(now()) {}

        [[nodiscard]] static inline double now() noexcept {
            const auto now = std::chrono::steady_clock::now().time_since_epoch();
            return std::chrono::duration<double>(now).count();
        }

        [[nodiscard]] inline double elapsed() const noexcept { return now() - m_Start; }

        inline void reset() noexcept { m_Start = now(); }

    private:
        double m_Start;
    };

    struct FrameTime {
        double   Total      = 0.0;
        double   Delta      = 0.0;
        uint64_t FrameCount = 0;
    };
}