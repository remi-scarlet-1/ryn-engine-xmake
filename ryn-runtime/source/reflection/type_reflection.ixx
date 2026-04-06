module;
#include "defines.h"

export module Ryntime.Reflection:TypeReflection;
export import Ryntime.Container.Span;
export import :TypeCommon;
import std;

namespace ryn {
    class RYN_API ITypeReflection {
    public:
        virtual ~ITypeReflection();
        [[nodiscard]] virtual TypeId       getTypeId() const       = 0;
        [[nodiscard]] virtual TypeCategory getTypeCategory() const = 0;
        [[nodiscard]] virtual const char*  getTypeName() const     = 0;
    };
}

export namespace ryn {
    /**
     * @class     TypeRegistry
     * @brief     Central database for type metadata.
     * @details   Collects registrations from static initializers and organizes them for optimized querying.
     * @attention Registrations are staged in a pending buffer. You must call rebuild() to synchronize the registry before new types
     *            become searchable.
     */
    class RYN_API TypeRegistry {
    public:
        static TypeRegistry& getInstance();

        static void rebuild();
        static void registerType(TypeInfo typeInfo);

        [[nodiscard]] static const TypeInfo&        queryType(const TypeId& id);
        [[nodiscard]] static ReadonlySpan<TypeInfo> queryTypeByCategory(const TypeCategory& category);

    private:
        TypeRegistry();
        ~TypeRegistry();

        std::atomic<bool> m_IsDirty{false};

        mutable std::shared_mutex m_QueryMutex;
        mutable std::mutex        m_RegisterMutex;

        std::vector<TypeInfo> m_Types;
        std::vector<TypeInfo> m_PendingTypes;

        HashMap<TypeId, size_t>                       m_ById;
        HashMap<TypeCategory, ReadonlySpan<TypeInfo>> m_ByCategory;
    };

    template<typename T>
    concept IsStaticType = requires { typename T::MarkStaticType; };

    template<typename T>
    concept IsDynamicBase = requires { typename T::MarkDynamicBase; };

    template<typename T>
    concept IsDynamicType = requires { typename T::MarkDynamicType; };

    template<typename T>
    concept IsType = IsStaticType<T> || IsDynamicType<T>;

    /**
     * @struct  StaticType
     * @brief   Mixin for non-virtual type reflection.
     * @details Uses CRTP to provide static constexpr type metadata without overhead or virtual tables.
     * @tparam  Category The types classification (e.g., Component, System, Event).
     * @tparam  Name The unique string identifier used to generate the type's id.
     */
    template<const TypeCategory& Category, TypeName Name>
    struct StaticType {
        using MarkStaticType = void;

        static constexpr auto        TYPE_ID       = TypeId(Name.value);
        static constexpr auto        TYPE_CATEGORY = Category;
        static constexpr const char* TYPE_NAME     = Name.data();

        static bool registerType() {
            TypeRegistry::registerType(TypeInfo(TYPE_ID, TYPE_CATEGORY));
            return true;
        }
    };

    /**
     * @class   DynamicBase
     * @brief   Base interface for types requiring polymorphic reflection.
     * @details Defines the shared category for an entire inheritance hierarchy.
     * @tparam  Category All derived types classification (e.g., Component, System, Event).
     */
    template<const TypeCategory& Category>
    class DynamicBase : public ITypeReflection {
    public:
        using MarkDynamicType = void;

        static constexpr auto TYPE_CATEGORY = Category;

        [[nodiscard]] TypeCategory getTypeCategory() const final { return TYPE_CATEGORY; }
    };

    /**
     * @class   DynamicType
     * @brief   Final implementation leaf for polymorphic reflected types.
     * @details Bridges DynamicBase and StaticType to implement the dynamic reflection contract.
     * @tparam  Base The DynamicBase providing the shared category metadata.
     * @tparam  Name The unique string identifier used to generate the type's id.
     */
    template<IsDynamicBase Base, TypeName Name>
    class DynamicType : public Base, public StaticType<Base::TYPE_CATEGORY, Name> {
    public:
        using MarkDynamicType = void;
        using Meta            = StaticType<Base::TYPE_CATEGORY, Name>;

        using Meta::registerType;
        using Meta::TYPE_CATEGORY;
        using Meta::TYPE_ID;
        using Meta::TYPE_NAME;

        [[nodiscard]] TypeId      getTypeId() const final { return TYPE_ID; }
        [[nodiscard]] const char* getTypeName() const final { return TYPE_NAME; }

    protected:
        constexpr DynamicType() noexcept { (void)s_Registered; }

    private:
        static inline bool s_Registered = registerType();
    };

    // TODO: don't forget that the RYN_KEEP macro is required somehow
    //  alternatively it would likely be better if I had a more concrete plan for cross-dll registration instead of relying on static init
    //  via registerType(). Perhaps a kind of TypeRegistry per binary which then send of a single register all package? Idk...
}