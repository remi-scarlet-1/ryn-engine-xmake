module;
#include "defines.h"

export module Ryntime.Container.Span;
import std;

export namespace ryn {
    /**
     * @struct  Span
     * @brief   A non-owning view over a contiguous sequence of objects.
     * @tparam  T The base type of the elements. Must not be a pointer, reference, or qualified type to ensure canonical representation.
     */
    template<typename T>
    struct [[nodiscard]] Span {
        static_assert(!std::is_pointer_v<T>, "Span: T cannot be a pointer.");
        static_assert(!std::is_reference_v<T>, "Span: T cannot be a reference.");

        using ElementPtr = T*;
        using ElementRef = T&;

        constexpr Span() noexcept = default;
        constexpr Span(T* data, size_t count) noexcept : m_Data(data), m_Count(count) {}
        constexpr Span(std::vector<T>& vector) noexcept : m_Data(vector.data()), m_Count(vector.size()) {}

        // STL iterators
        [[nodiscard]] constexpr ElementPtr begin() noexcept { return m_Data; }
        [[nodiscard]] constexpr ElementPtr end() noexcept { return m_Data + m_Count; }

        // Accessors
        [[nodiscard]] constexpr ElementPtr getData() const noexcept { return m_Data; }
        [[nodiscard]] constexpr size_t     getCount() const noexcept { return m_Count; }
        [[nodiscard]] constexpr bool       isEmpty() const noexcept { return m_Count == 0; }

        [[nodiscard]] constexpr ElementRef operator[](std::size_t index) const noexcept {
            RYN_ASSERT(index < m_Count, "Index out of range");
            return m_Data[index];
        }

        struct ReverseView {
            T*     m_Data;
            size_t m_Count;

            // clang-format off
            struct Iterator {
                T* ptr;
                constexpr T& operator*() const { return *(ptr - 1); } // Offset by 1
                constexpr Iterator& operator++() { --ptr; return *this; }
                constexpr bool operator!=(const Iterator& other) const { return ptr != other.ptr; }
            };
            // clang-format on

            constexpr Iterator begin() const { return {m_Data + m_Count}; }
            constexpr Iterator end() const { return {m_Data}; }
        };

        [[nodiscard]] constexpr ReverseView reversed() const noexcept { return {m_Data, m_Count}; }

    private:
        T*     m_Data  = nullptr;
        size_t m_Count = 0;
    };

    /**
     * @typedef ReadonlySpan
     * @brief   A non-owning read-only view over a contiguous sequence of objects.
     * @tparam  T The base type of the elements. Qualifiers are handled automatically.
     */
    template<typename T>
    using ReadonlySpan = Span<const T>;
}
