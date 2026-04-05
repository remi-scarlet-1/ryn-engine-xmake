module;
#include "defines.h"

export module Ryntime.Container.Tag;
export import Ryntime.Container.Hash;
import std;

export namespace ryn {
    /**
     * @struct  TagHash
     * @brief   The underlying 64-bit numerical value of a Tag.
     */
    template<typename>
    struct TagHash {
        using ValueType                    = uint64_t;
        static constexpr ValueType INVALID = 0xDEADBEEFDEADBEEFull;

        ValueType Value = INVALID;

        explicit constexpr operator bool() const { return Value != INVALID; }
        constexpr bool     operator==(const TagHash&) const  = default;
        constexpr auto     operator<=>(const TagHash&) const = default;
    };

    template<typename T>
    struct Hasher<TagHash<T>> {
        constexpr size_t operator()(const TagHash<T>& tag) const noexcept { return std::hash<uint64_t>{}(tag.Value); }
    };

    /**
     * @struct  Tag
     * @brief   A 64-bit numerical tag derived from a string hash.
     * @tparam  Type The derived concrete tag type.
     * @warning Setting the `Hash` member manually is highly discouraged!
     */
    template<typename Type>
    struct Tag {
        using HashType = TagHash<Type>;
        HashType Hash{};

        consteval Tag() = default;

        template<size_t Length>
        consteval Tag(const char (&uniqueStr)[Length]) {
            static_assert(std::is_same_v<typename HashType::ValueType, uint64_t>);

            uint64_t hash = 0xcbf29ce484222325;
            for (size_t i = 0; i < Length - 1; i++) {
                hash ^= static_cast<uint64_t>(uniqueStr[i]);
                hash *= 0x100000001b3;
            }

            if (hash == HashType::INVALID) throw "Hash collision with HashType::INVALID!";
            Hash.Value = hash;
        }

        explicit constexpr operator bool() const { return static_cast<bool>(Hash); }
        constexpr bool     operator==(const Type& other) const { return Hash == other.Hash; }
        constexpr auto     operator<=>(const Type& other) const { return Hash <=> other.Hash; }
    };

    /**
     * @struct  NamedTag
     * @brief   A 64-bit numerical tag derived from a string hash that additionally stores it's name during runtime.
     * @tparam  Type The derived concrete tag type.
     * @tparam  NameBufferSize The size of the internal buffer used to store the name.
     * @warning Setting the `Hash` and `Name` members manually is highly discouraged!
     */
    template<typename Type, size_t NameBufferSize = 56>
    struct NamedTag : Tag<Type> {
        char Name[NameBufferSize]{};

        consteval NamedTag() = default;

        template<size_t Length>
        requires(Length <= NameBufferSize)
        consteval NamedTag(const char (&uniqueName)[Length]) : Tag<Type>(uniqueName) {
            for (size_t i = 0; i < Length && i < NameBufferSize; ++i) Name[i] = uniqueName[i];
            Name[NameBufferSize - 1] = '\0';
        }
    };
}