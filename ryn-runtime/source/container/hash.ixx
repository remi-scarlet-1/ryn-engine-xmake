module;
#include "defines.h"

export module Ryntime.Container.Hash;
import std;

export namespace ryn {
    template<typename T>
    struct Hasher {
        constexpr size_t operator()(const T& value) const noexcept { return std::hash<T>()(value); }
    };

    struct UniversalHasher {
        using is_transparent = void;

        template<typename T>
        constexpr size_t operator()(const T& value) const noexcept {
            return Hasher<T>{}(value);
        }
    };

    template<typename Key, typename Value, typename Hasher = UniversalHasher, typename KeyEq = std::equal_to<void>>
    using HashMap = std::unordered_map<Key, Value, Hasher, KeyEq>;

    template<typename Key, typename Hasher = UniversalHasher, typename KeyEq = std::equal_to<void>>
    using HashSet = std::unordered_set<Key, Hasher, KeyEq>;

    template<typename Key, typename Value, typename Hasher = UniversalHasher, typename KeyEq = std::equal_to<void>>
    using HashMultimap = std::unordered_multimap<Key, Value, Hasher, KeyEq>;

    template<typename Key, typename Hasher = UniversalHasher, typename KeyEq = std::equal_to<void>>
    using HashMultiset = std::unordered_multiset<Key, Hasher, KeyEq>;
}