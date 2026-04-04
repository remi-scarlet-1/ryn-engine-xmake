module;
#include "defines.h"

export module Ryntime.Container.Span;
import std;

export namespace ryn {
    /**
     * @struct  Span
     * @brief   A non-owning view over a contiguous sequence of objects.
     * @tparam  T Type of the elements in the contiguous memory.
     */
    template<typename T>
    struct [[nodiscard]] Span {
        using ValueType = T;

        constexpr Span() noexcept = default;
        constexpr Span(T* data, std::size_t count) noexcept : m_Data(data), m_Count(count) {}

        // STL iterators
        [[nodiscard]] constexpr T*       begin() noexcept { return m_Data; }
        [[nodiscard]] constexpr const T* begin() const noexcept { return m_Data; }
        [[nodiscard]] constexpr T*       end() noexcept { return m_Data + m_Count; }
        [[nodiscard]] constexpr const T* end() const noexcept { return m_Data + m_Count; }

        // Accessors
        [[nodiscard]] constexpr T*          getData() const noexcept { return m_Data; }
        [[nodiscard]] constexpr std::size_t getCount() const noexcept { return m_Count; }
        [[nodiscard]] constexpr bool        isEmpty() const noexcept { return m_Count == 0; }

        [[nodiscard]] constexpr T& operator[](std::size_t index) const noexcept {
            RYN_ASSERT(index < m_Count, "Index out of range");
            return m_Data[index];
        }

    private:
        T*          m_Data  = nullptr;
        std::size_t m_Count = 0;
    };
}
