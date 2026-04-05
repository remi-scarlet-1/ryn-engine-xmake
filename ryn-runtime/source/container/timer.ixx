module;
#include "defines.h"

export module Ryntime.Container.Timer;
import std;

export namespace ryn {
    class [[nodiscard]] Timer {
    public:
        inline Timer() noexcept : m_Start(std::chrono::steady_clock::now()) {}

        /// @brief Returns the total elapsed seconds since start or last reset.
        [[nodiscard]] inline double elapsedSeconds() const noexcept {
            return std::chrono::duration<double>(std::chrono::steady_clock::now() - m_Start).count();
        }

        /// @brief Returns the total elapsed milliseconds since start or last reset.
        [[nodiscard]] inline double elapsedMilliSeconds() const noexcept { return elapsedSeconds() * 1000.0; }

        /// @brief Resets the start point to the current time.
        inline void reset() noexcept { m_Start = std::chrono::steady_clock::now(); }

    private:
        std::chrono::steady_clock::time_point m_Start;
    };
}