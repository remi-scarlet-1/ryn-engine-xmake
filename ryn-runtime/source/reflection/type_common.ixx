module;
#include "defines.h"

export module Ryntime.Reflection:TypeCommon;
export import Ryntime.Container.Tag;
import std;

export namespace ryn {
    template<size_t N>
    struct TypeName {
        char value[N]{};
        consteval TypeName(const char (&str)[N]) {
            for (size_t i = 0; i < N; ++i)
                value[i] = str[i];
        }
        [[nodiscard]] constexpr const char* data() const { return value; }
    };

    struct TypeIdentity : NamedTag<TypeIdentity> {
        /* Invalid identity used as substitution in return statements on errors */
        static const TypeIdentity Invalid;
    };

    constexpr TypeIdentity TypeIdentity::Invalid{};

    struct TypeCategory : NamedTag<TypeCategory> {
        /* Well known engine type categories */
        static const TypeCategory None;
        static const TypeCategory Service;
        static const TypeCategory Event;
        static const TypeCategory System;
        static const TypeCategory Component;
        static const TypeCategory ComponentRequest;
    };

    constexpr TypeCategory TypeCategory::None{"Ryn.None"};
    constexpr TypeCategory TypeCategory::Service{"Ryn.Service"};
    constexpr TypeCategory TypeCategory::Event{"Ryn.Event"};
    constexpr TypeCategory TypeCategory::System{"Ryn.System"};
    constexpr TypeCategory TypeCategory::Component{"Ryn.Component"};
    constexpr TypeCategory TypeCategory::ComponentRequest{"Ryn.ComponentRequest"};

    struct TypeInfo {
        const TypeIdentity& Identity;
        const TypeCategory& Category;

        TypeInfo() = delete;
        TypeInfo(const TypeIdentity& identity, const TypeCategory& category) : Identity(identity), Category(category) {}
    };
}

template<>
struct std::formatter<ryn::TypeIdentity> {
    constexpr auto parse(const format_parse_context& ctx) const { return ctx.begin(); }
    inline auto    format(const ryn::TypeIdentity& identity, format_context& ctx) const {
        return format_to(ctx.out(), "({}, 0x{:X})", identity.Name, identity.Hash.Value);
    }
};

template<>
struct std::formatter<ryn::TypeCategory> {
    constexpr auto parse(const format_parse_context& ctx) const { return ctx.begin(); }
    inline auto    format(const ryn::TypeCategory& category, format_context& ctx) const {
        return format_to(ctx.out(), "({}, 0x{:X})", category.Name, category.Hash.Value);
    }
};

template<>
struct std::formatter<ryn::TypeInfo> {
    constexpr auto parse(const format_parse_context& ctx) const { return ctx.begin(); }
    inline auto    format(const ryn::TypeInfo& info, format_context& ctx) const {
        return format_to(ctx.out(), "[Identity: {}, Category: {}]", info.Identity, info.Category);
    }
};