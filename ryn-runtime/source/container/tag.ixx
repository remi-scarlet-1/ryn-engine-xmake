module;
#include "defines.h"

export module Ryntime.Container.Tag;
export import Ryntime.Container.Hash;
import std;

export namespace ryn {
    /**
     * @struct  Tag
     * @brief   An 8-byte tag carrying a unique hash derived at compile time.
     * @tparam  Discriminator The derived concrete tag type.
     * @warning Overwriting the `Hash` member manually is highly discouraged!
     */
    template<typename Discriminator>
    struct Tag {
        using ValueType                         = uint64_t;
        static constexpr ValueType INVALID_HASH = 0xDEADBEEFDEADBEEFull;
        ValueType                  Hash         = INVALID_HASH;

        consteval Tag() = default;

        template<size_t Length>
        consteval Tag(const char (&uniqueName)[Length]) {
            static_assert(std::is_same_v<ValueType, uint64_t>, "Tag hash algorithm requires 64-bit uint.");

            uint64_t hash = 0xcbf29ce484222325;
            for (size_t i = 0; i < Length - 1; i++) {
                hash ^= static_cast<uint64_t>(uniqueName[i]);
                hash *= 0x100000001b3;
            }

            if (hash == INVALID_HASH) throw "Hash collision with INVALID!";
            Hash = hash;
        }

        explicit constexpr    operator bool() const { return Hash != INVALID_HASH; }
        friend constexpr bool operator==(const Tag&, const Tag&)  = default;
        friend constexpr auto operator<=>(const Tag&, const Tag&) = default;
    };

    /**
     * @struct  NamedTag
     * @brief   A 16-byte tag carrying a unique name and hash derived at compile time.
     * @tparam  Discriminator The derived concrete tag type.
     * @warning Setting the `Hash` and `Name` members manually is highly discouraged!
     */
    template<typename Discriminator>
    struct NamedTag : Tag<Discriminator> {
        const char* Name = "<unknown>";

        consteval NamedTag() = default;

        template<size_t Length>
        consteval NamedTag(const char (&uniqueName)[Length]) : Tag<Discriminator>(uniqueName),
                                                               Name(uniqueName) {}
    };

    template<typename T>
    concept IsTag = std::derived_from<T, Tag<T>> && requires(const T& tag) {
        { tag.Hash } -> std::convertible_to<uint64_t>;
    };

    template<IsTag T>
    struct Hasher<T> {
        constexpr size_t operator()(const T& tag) const noexcept { return static_cast<size_t>(tag.Hash); }
    };
}