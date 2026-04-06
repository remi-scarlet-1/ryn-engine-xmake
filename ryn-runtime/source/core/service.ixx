module;
#include "defines.h"

export module Ryntime.Core:Service;
export import Ryntime.Reflection;
export import Ryntime.Container.Box;
export import Ryntime.Container.Hash;
export import :Event;
import Ryntime.Container.Timer;
import std;

export namespace ryn {
    class IServiceLocator;

    struct FrameContext {
        struct {
            uint64_t FrameCount = 0;
            double   Total      = 0.0;
            double   Delta      = 0.0;
        } Time;
    };

    class RYN_API IService : public DynamicBase<TypeCategory::Service> {
    public:
        ~IService() override;

    protected:
        virtual inline void onAttach([[maybe_unused]] IServiceLocator& locator) {}
        virtual inline void onDetach() {}

        virtual inline void onBeginFrame([[maybe_unused]] const FrameContext& ctx) {}
        virtual inline void onUpdate([[maybe_unused]] const FrameContext& ctx) {}
        virtual inline void onEndFrame([[maybe_unused]] const FrameContext& ctx) {}

        virtual inline void onEvent([[maybe_unused]] EventListener& listener) {}

        inline void dispatchEvent(IEvent& event) const { m_EventDispatcher->dispatchEvent(event); }
        inline void dispatchEvent(IEvent&& event) const { m_EventDispatcher->dispatchEvent(event); }

    private:
        friend class ServiceStack;
        IEventDispatcher* m_EventDispatcher = nullptr;
    };

    template<typename T>
    concept IsService = std::derived_from<T, IService>;

    class RYN_API IServiceStack {
    public:
        virtual ~IServiceStack();

        template<typename T, typename... Args>
        requires IsService<T> && IsType<T>
        inline T* push(Args&&... args) {
            Box<IService> serviceBox = Box<T>::template make<T>(std::forward<Args>(args)...);
            IService*     servicePtr = serviceBox.get();
            pushService(T::TYPE_ID, std::move(serviceBox));
            return static_cast<T*>(servicePtr);
        }

        inline void pop() { internalPop(); }

    private:
        virtual void internalPush(TypeId typeId, Box<IService>&& serviceBox) = 0;
        virtual void internalPop()                                           = 0;
    };

    class RYN_API IServiceLocator {
    public:
        virtual ~IServiceLocator();

        template<typename T>
        requires IsService<T> && IsType<T>
        inline void require(T*& member) {
            member = static_cast<T*>(internalRequest(T::TYPE_ID, true));
        }

        template<typename T>
        requires IsService<T> && IsType<T>
        [[nodiscard]] inline T* require() {
            return static_cast<T*>(internalRequest(T::TYPE_ID, true));
        }

        template<typename T>
        requires IsService<T> && IsType<T>
        inline void optional(T*& member) {
            member = static_cast<T*>(internalRequest(T::TYPE_ID, false));
        }

        template<typename T>
        requires IsService<T> && IsType<T>
        [[nodiscard]] inline T* optional() {
            return static_cast<T*>(internalRequest(T::TYPE_ID, false));
        }

    protected:
        static inline IService* internalRequest(IServiceLocator& source, TypeId typeId, bool required) {
            return source.internalRequest(typeId, required);
        }

    private:
        virtual IService* internalRequest(TypeId typeId, bool required) = 0;
        virtual void      handleMissingService(TypeId typeId)           = 0;
    };

    class RYN_API ServiceDiscovery : public IServiceLocator {
    public:
        explicit ServiceDiscovery(IServiceLocator& locator);
        [[nodiscard]] inline bool wasSuccess() const noexcept { return m_Success; }

    private:
        IServiceLocator& m_Locator;
        bool             m_Success = true;

        IService* internalRequest(TypeId typeId, bool required) override;
        void      handleMissingService(TypeId typeId) override;
    };
}

namespace ryn {
    class RYN_API ServiceStack : public IServiceStack, public IServiceLocator, IEventDispatcher {
    public:
        using EventInterceptor = std::function<void(EventListener&)>;

        ServiceStack();
        ~ServiceStack() override;

        ServiceStack(const ServiceStack&)                = delete;
        ServiceStack& operator=(const ServiceStack&)     = delete;
        ServiceStack(ServiceStack&&) noexcept            = delete;
        ServiceStack& operator=(ServiceStack&&) noexcept = delete;

        void update();
        void dispatchEvent(IEvent& event) override;
        void setEventInterceptor(EventInterceptor interceptor);

    private:
        EventInterceptor           m_EventInterceptor;
        std::vector<Box<IService>> m_Services;
        HashMap<TypeId, IService*> m_ServiceLUT;
        FrameContext               m_FrameContext;
        Timer                      m_FrameTimer;

        void internalPush(TypeId typeId, Box<IService>&& serviceBox) override;
        void internalPop() override;

        IService* internalRequest(TypeId typeId, bool required) override;
        void      handleMissingService(TypeId typeId) override;
    };
}