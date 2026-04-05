module;
#include "defines.h"

module Ryntime.Reflection;
import std;

namespace ryn {
    ITypeReflection::~ITypeReflection() = default;

    TypeRegistry::TypeRegistry()  = default;
    TypeRegistry::~TypeRegistry() = default;

    TypeRegistry& TypeRegistry::getInstance() {
        static TypeRegistry s_Instance;
        return s_Instance;
    }

    void TypeRegistry::rebuild() {
        TypeRegistry&    self = getInstance();
        std::scoped_lock lock(self.m_QueryMutex, self.m_RegisterMutex);

        if (!self.m_IsDirty.load(std::memory_order_acquire)) {
            RYN_TRACE("Rebuild ignored; registry is already synchronized (dirty flag is false).");
            return;
        }

        const size_t totalCount   = self.m_Types.size() + self.m_PendingTypes.size();
        const size_t pendingCount = self.m_PendingTypes.size();
        RYN_INFO("Rebuilding... Found {} pending types to reconcile.", pendingCount);

        // Gather pointers to all TypeInfos
        std::vector<const TypeInfo*> sortBuffer;
        sortBuffer.reserve(totalCount);

        for (const auto& t : self.m_Types) sortBuffer.push_back(&t);
        for (const auto& t : self.m_PendingTypes) sortBuffer.push_back(&t);

        // Sort the TypeInfo pointers by Category and Hash
        std::ranges::sort(sortBuffer, [](const TypeInfo* a, const TypeInfo* b) {
            if (a->Category.Hash != b->Category.Hash) return a->Category.Hash < b->Category.Hash;
            return a->Identity.Hash < b->Identity.Hash;
        });

        // Construct a new packed vector of the sorted TypeInfos
        std::vector<TypeInfo> newTypes;
        newTypes.reserve(totalCount);
        for (const TypeInfo* ptr : sortBuffer) newTypes.push_back(*ptr);

        // Swap the new buffer into the registry
        self.m_Types.swap(newTypes);
        self.m_PendingTypes.clear();

        // Rebuild m_ByIdentity and m_ByCategory
        self.m_ByIdentityHash.clear();
        self.m_ByCategoryHash.clear();

        size_t rangeStart    = 0;
        size_t categoryCount = 0;

        for (size_t i = 0; i < self.m_Types.size(); i++) {
            const auto& current = self.m_Types[i];

            // Map the Identity to the vector index
            self.m_ByIdentityHash[current.Identity.Hash] = i;

            // Determine if we've reached the end of a category block
            bool isLast          = (i == self.m_Types.size() - 1);
            bool nextIsDifferent = !isLast && (self.m_Types[i + 1].Category.Hash != current.Category.Hash);

            if (isLast || nextIsDifferent) {
                size_t count = (i - rangeStart) + 1;
                // Create a span pointing to the slice of the vector
                self.m_ByCategoryHash[current.Category.Hash] = ReadonlySpan(&self.m_Types[rangeStart], count);

                // Log the category slice just discovered
                RYN_TRACE("Registered category '{}' with {} types.", current.Category, count);

                rangeStart = i + 1;
                categoryCount++;
            }
        }

        self.m_IsDirty.store(false, std::memory_order_release);

        // TODO: add timer to know how long the rebuild took
        RYN_INFO("Rebuild complete. Total Types: {}, Total Categories: {}.", self.m_Types.size(), categoryCount);
    }

    void TypeRegistry::registerType(TypeInfo typeInfo) {
        TypeRegistry&    self = getInstance();
        std::scoped_lock lock(self.m_RegisterMutex);

        RYN_INFO("Registering pending type: {}", typeInfo);
        self.m_PendingTypes.push_back(std::move(typeInfo));
        self.m_IsDirty.store(true, std::memory_order_release);
    }

#define DIRTY_WARNING_MSG                                                                                                                  \
    "Querying registry while it's marked as dirty. This likely isn't what you intended! To reconcile state please call "                   \
    "TypeRegistry::rebuild() but be aware that this operation invalidates all past query results and should therefore be only "            \
    "invoked inbetween frames."

    const TypeInfo& TypeRegistry::queryType(const TypeIdentity& identity) {
        TypeRegistry& self = getInstance();
        if (self.m_IsDirty.load(std::memory_order_acquire)) RYN_WARN(DIRTY_WARNING_MSG);
        std::shared_lock lock(self.m_QueryMutex);

        auto it = self.m_ByIdentityHash.find(identity.Hash);
        if (it != self.m_ByIdentityHash.end()) return self.m_Types[it->second];

        RYN_ERROR("Type info for TypeIdentity: {} was not found!", identity);
        static TypeInfo s_InvalidInfo(TypeIdentity::Invalid, TypeCategory::None);
        return s_InvalidInfo;
    }

    ReadonlySpan<TypeInfo> TypeRegistry::queryTypeByCategory(const TypeCategory& category) {
        TypeRegistry& self = getInstance();
        if (self.m_IsDirty.load(std::memory_order_acquire)) RYN_WARN(DIRTY_WARNING_MSG);
        std::shared_lock lock(self.m_QueryMutex);

        auto it = self.m_ByCategoryHash.find(category.Hash);
        if (it == self.m_ByCategoryHash.end()) return ReadonlySpan<TypeInfo>();
        return it->second;
    }
}
