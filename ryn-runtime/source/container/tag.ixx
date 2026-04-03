module;
#include "defines.h"

export module Ryntime.Container.Tag;

export namespace ryn {
    /**
     * @struct  Tag
     * @brief   A 64-bit numerical tag derived from a string hash.
     * @tparam  Type The derived concrete tag type.
     */
    template<typename Type>
    struct Tag {
        consteval Tag() = default;

        template<size_t Length>
        consteval Tag(const char (&uniqueStr)[Length]) {
            assignHash(uniqueStr, Length - 1);
        }

        constexpr uint64_t getHash() const { return m_Hash; }
        explicit constexpr operator bool() const { return m_Hash != INVALID_HASH; }

        constexpr bool operator==(const Type& other) const { return m_Hash == other.m_Hash; }
        constexpr auto operator<=>(const Type& other) const { return m_Hash <=> other.m_Hash; }

    private:
        static constexpr uint64_t INVALID_HASH = 0xDEADBEEFDEADBEEFull;

        uint64_t m_Hash = INVALID_HASH;

        consteval Tag(uint64_t rawHash) : m_Hash(rawHash) {}

        consteval void assignHash(const char* uniqueStr, size_t length) {
            uint64_t hash = 0xcbf29ce484222325;
            for (size_t i = 0; i < length; i++) {
                hash ^= static_cast<uint64_t>(uniqueStr[i]);
                hash *= 0x100000001b3;
            }
            if (hash == INVALID_HASH) throw "Hash collision with INVALID_HASH!";
            m_Hash = hash;
        }
    };

    /**
     * @struct  NamedTag
     * @brief   A 64-bit numerical tag derived from a string hash that additionally stores it's name during runtime.
     * @tparam  Type The derived concrete tag type.
     * @tparam  NameBufferSize The size of the internal buffer used to store the name.
     */
    template<typename Type, size_t NameBufferSize = 56>
    struct NamedTag : Tag<Type> {
        consteval NamedTag() = default;

        template<size_t Length>
        requires(Length <= NameBufferSize)
        consteval NamedTag(const char (&uniqueName)[Length]) : Tag<Type>(uniqueName) {
            assignName(uniqueName, Length);
        }

        constexpr const char* getName() const { return m_Name; }

    private:
        char m_Name[NameBufferSize] = {};

        consteval void assignName(const char* name, size_t length) {
            for (size_t i = 0; i < length && i < NameBufferSize; ++i)
                m_Name[i] = name[i];
            m_Name[NameBufferSize - 1] = '\0';
        }
    };
}