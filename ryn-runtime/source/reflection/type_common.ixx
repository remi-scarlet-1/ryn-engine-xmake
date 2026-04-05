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
            for (size_t i = 0; i < N; ++i) value[i] = str[i];
        }
        [[nodiscard]] constexpr const char* data() const { return value; }
    };

    struct TypeId : NamedTag<TypeId> {
        /* Invalid id used as substitution in return statements on errors */
        static const TypeId Invalid;
    };

    constexpr TypeId TypeId::Invalid{};

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
        const TypeId&       Id;
        const TypeCategory& Category;

        TypeInfo() = delete;
        TypeInfo(const TypeId& id, const TypeCategory& category) : Id(id), Category(category) {}
    };
}

template<>
struct std::formatter<ryn::TypeId> {
    constexpr auto parse(const format_parse_context& ctx) const { return ctx.begin(); }
    inline auto format(const ryn::TypeId& id, format_context& ctx) const { return format_to(ctx.out(), "({}, 0x{:X})", id.Name, id.Hash); }
};

template<>
struct std::formatter<ryn::TypeCategory> {
    constexpr auto parse(const format_parse_context& ctx) const { return ctx.begin(); }
    inline auto    format(const ryn::TypeCategory& category, format_context& ctx) const {
        return format_to(ctx.out(), "({}, 0x{:X})", category.Name, category.Hash);
    }
};

template<>
struct std::formatter<ryn::TypeInfo> {
    constexpr auto parse(const format_parse_context& ctx) const { return ctx.begin(); }
    inline auto    format(const ryn::TypeInfo& info, format_context& ctx) const {
        return format_to(ctx.out(), "[Id: {}, Category: {}]", info.Id, info.Category);
    }
};