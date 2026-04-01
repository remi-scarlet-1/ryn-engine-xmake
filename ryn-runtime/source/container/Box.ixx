module;
#include "Defines.h"
#include <concepts>
#include <utility>

export module Ryntime.Container.Box;

export namespace ryn {
    /**
     * @struct  Box
     * @brief   A unique pointer with a type-erased deleter for safe cross-DLL destruction.
     * @details Captures the destruction context at the allocation site to prevent heap mismatches.
     * @tparam  T Type of the wrapped pointer.
     */
    template<typename T>
    struct [[nodiscard]] Box {
        using Pointer = T*;
        using Deleter = void (*)(void*);

        constexpr Box() noexcept = default;
        constexpr Box(std::nullptr_t) noexcept : Box() {}
        constexpr ~Box() noexcept { reset(); }

        // Disable copy
        Box(const Box&) = delete;
        Box& operator=(const Box&) = delete;

        // Implement move
        Box(Box&& other) noexcept : 
            m_Pointer(std::exchange(other.m_Pointer, nullptr)),
            m_Deleter(std::exchange(other.m_Deleter, nullptr)) 
        {}

        Box& operator=(Box&& other) noexcept {
            if (this != &other) {
                reset();
                m_Pointer = std::exchange(other.m_Pointer, nullptr);
                m_Deleter = std::exchange(other.m_Deleter, nullptr);
            }
            return *this;
        }

        // Implement converting move
        template<typename U> 
        requires std::derived_from<U, T>
        Box(Box<U>&& other) noexcept : 
            m_Pointer(std::exchange(other.m_Pointer, nullptr)),
            m_Deleter(std::exchange(other.m_Deleter, nullptr)) 
        {}

        template<typename U>
        requires std::derived_from<U, T>
        Box& operator=(Box<U>&& other) noexcept {
            reset();
            m_Pointer = std::exchange(other.m_Pointer, nullptr);
            m_Deleter = std::exchange(other.m_Deleter, nullptr);
            return *this;
        }

        void reset() {
            if (m_Pointer) {
                RYN_ASSERT(m_Deleter != nullptr, "Deleter must not be null if pointer exists!");
                m_Deleter(static_cast<void*>(m_Pointer));
            }
            m_Pointer = nullptr;
            m_Deleter = nullptr;
        }

        [[nodiscard]] Pointer release() noexcept {
            Pointer p = m_Pointer;
            m_Pointer = nullptr;
            m_Deleter = nullptr;
            return p;
        }

        // Factory
        template<typename Derived = T, typename... Args>
        requires std::derived_from<Derived, T>
        [[nodiscard]] static Box make(Args&&... args) {
            auto* pointer = new Derived(std::forward<Args>(args)...);
            // Lambda captures destructor on the owning CRT
            auto deleter = [](void* p) { delete static_cast<Derived*>(p); };
            return Box(static_cast<Pointer>(pointer), deleter);
        }

        // Accessors
        [[nodiscard]] constexpr T& operator*() const noexcept { return *m_Pointer; }
        [[nodiscard]] constexpr T* operator->() const noexcept { return m_Pointer; }
        [[nodiscard]] constexpr T* get() const noexcept { return m_Pointer; }
        [[nodiscard]] constexpr explicit operator bool() const noexcept { return m_Pointer != nullptr; }

    private:
        // Required for converting move constructors
        template<typename U>
        friend struct Box;

        // Internal constructor used by make
        constexpr Box(Pointer ptr, Deleter deleter) noexcept : m_Pointer(ptr), m_Deleter(deleter) {}

        Pointer m_Pointer = nullptr;
        Deleter m_Deleter = nullptr;
    };
}