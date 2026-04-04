module;
#define RYN_LEAN_AND_MEAN // Required because this container is utilized in the Logger
#include "defines.h"

export module Ryntime.Container.HybridBuffer;
import std;

export namespace ryn {
    /**
     * @struct  HybridBuffer
     * @brief   A general-purpose Small Buffer Optimization (SBO) container.
     * @details Stores elements on the stack up to `StackCapacity`. Automatically
     *          transitions to heap allocation if the capacity is exceeded.
     * @tparam  T Type of the stored elements.
     * @tparam  StackCapacity Maximum number of elements to store inline on the stack.
     */
    template<typename T, size_t StackCapacity = 256>
    class [[nodiscard]] HybridBuffer {
        static_assert(StackCapacity > 0, "Stack capacity must be greater than zero.");

    public:
        using ValueType      = T;
        using SizeType       = size_t;
        using Pointer        = T*;
        using ConstPointer   = const T*;

        // STL compatibility aliases
        using value_type     = ValueType;
        using size_type      = SizeType;
        using iterator       = Pointer;
        using const_iterator = ConstPointer;

        constexpr HybridBuffer() noexcept : m_Data(reinterpret_cast<Pointer>(m_StackData.raw)), m_Size(0), m_Capacity(StackCapacity) {}
        constexpr ~HybridBuffer() noexcept {
            clear();
            if (isHeap()) { ::operator delete(m_Data); }
        }

        // Disable copy
        HybridBuffer(const HybridBuffer&)            = delete;
        HybridBuffer& operator=(const HybridBuffer&) = delete;

        // Move semantics
        HybridBuffer(HybridBuffer&& other) noexcept : m_Size(other.m_Size), m_Capacity(other.m_Capacity) {
            if (other.isHeap()) {
                m_Data           = std::exchange(other.m_Data, reinterpret_cast<Pointer>(other.m_StackData.raw));
                other.m_Capacity = StackCapacity;
            } else {
                m_Data = reinterpret_cast<Pointer>(m_StackData.raw);
                for (SizeType i = 0; i < m_Size; ++i) {
                    std::construct_at(m_Data + i, std::move_if_noexcept(other.m_Data[i]));
                    std::destroy_at(other.m_Data + i);
                }
            }
            other.m_Size = 0;
        }

        HybridBuffer& operator=(HybridBuffer&& other) noexcept {
            if (this != &other) {
                clear();
                if (isHeap()) { ::operator delete(m_Data); }

                m_Size     = other.m_Size;
                m_Capacity = other.m_Capacity;

                if (other.isHeap()) {
                    m_Data           = std::exchange(other.m_Data, reinterpret_cast<Pointer>(other.m_StackData.raw));
                    other.m_Capacity = StackCapacity;
                } else {
                    m_Data = reinterpret_cast<Pointer>(m_StackData.raw);
                    for (SizeType i = 0; i < m_Size; ++i) {
                        std::construct_at(m_Data + i, std::move_if_noexcept(other.m_Data[i]));
                        std::destroy_at(other.m_Data + i);
                    }
                }
                other.m_Size = 0;
            }
            return *this;
        }

        void reserve(SizeType newCapacity) {
            if (newCapacity <= m_Capacity) { return; }

            Pointer newData = static_cast<Pointer>(::operator new(newCapacity * sizeof(T)));
            for (SizeType i = 0; i < m_Size; ++i) {
                std::construct_at(newData + i, std::move_if_noexcept(m_Data[i]));
                std::destroy_at(m_Data + i);
            }

            if (isHeap()) { ::operator delete(m_Data); }

            m_Data     = newData;
            m_Capacity = newCapacity;
        }

        void clear() noexcept {
            for (SizeType i = 0; i < m_Size; ++i) {
                std::destroy_at(m_Data + i);
            }
            m_Size = 0;
        }

        template<typename... Args>
        T& emplaceBack(Args&&... args) {
            if (m_Size == m_Capacity) { reserve(m_Capacity * 2); }
            Pointer ptr = m_Data + m_Size;
            std::construct_at(ptr, std::forward<Args>(args)...);
            m_Size++;
            return *ptr;
        }

        void pushBack(const T& value) { emplaceBack(value); }
        void pushBack(T&& value) { emplaceBack(std::move(value)); }

        // STL compatibility functions
        void push_back(const T& value) { emplaceBack(value); }
        void push_back(T&& value) { emplaceBack(std::move(value)); }

        // STL iterators
        [[nodiscard]] constexpr Pointer      begin() noexcept { return m_Data; }
        [[nodiscard]] constexpr ConstPointer begin() const noexcept { return m_Data; }
        [[nodiscard]] constexpr Pointer      end() noexcept { return m_Data + m_Size; }
        [[nodiscard]] constexpr ConstPointer end() const noexcept { return m_Data + m_Size; }

        // Accessors
        [[nodiscard]] constexpr Pointer  getData() noexcept { return m_Data; }
        [[nodiscard]] constexpr SizeType getSize() const noexcept { return m_Size; }
        [[nodiscard]] constexpr SizeType getCapacity() const noexcept { return m_Capacity; }
        [[nodiscard]] constexpr bool     isEmpty() const noexcept { return m_Size == 0; }

    private:
        [[nodiscard]] constexpr bool isHeap() const noexcept { return m_Capacity > StackCapacity; }

        union Storage {
            alignas(T) std::byte raw[StackCapacity * sizeof(T)];

            // Do nothing. Prevents compiler from zero-initializing the `raw` stack-allocated array.
            inline Storage() noexcept {}
            inline ~Storage() noexcept {}
        };

        Pointer  m_Data;
        SizeType m_Size;
        SizeType m_Capacity;
        Storage  m_StackData;
    };
}