module;
#include "defines.h"

module Ryntime.Core;

namespace ryn {
    IService::~IService()               = default;
    IServiceStack::~IServiceStack()     = default;
    IServiceLocator::~IServiceLocator() = default;

    ServiceDiscovery::ServiceDiscovery(IServiceLocator& locator) : m_Locator(locator) {}

    IService* ServiceDiscovery::internalRequest(TypeId typeId, bool required) {
        auto* service = IServiceLocator::internalRequest(m_Locator, typeId, false);

        // Handle required missing services manually
        if (required && !service) {
            m_Success = false;
            this->handleMissingService(typeId);
        }

        return service;
    }

    void ServiceDiscovery::handleMissingService(TypeId typeId) {
        RYN_WARN("Service discovery failed. Couldn't find {}!", TypeRegistry::queryType(typeId));
    }

    ServiceStack::ServiceStack() = default;
    ServiceStack::~ServiceStack() {
        RYN_INFO("Destroy services...");
        while (!m_Services.empty()) ServiceStack::internalPop();
        m_Services.clear();
        m_ServiceLUT.clear();
    }

    void ServiceStack::update() {
        m_FrameContext.Time.FrameCount++;
        m_FrameContext.Time.Total += m_FrameTimer.elapsedSeconds();
        m_FrameContext.Time.Delta = m_FrameTimer.elapsedSeconds();
        m_FrameTimer.reset();

        for (const auto& serviceBox : m_Services) serviceBox->onBeginFrame(m_FrameContext);

        for (const auto& serviceBox : m_Services) serviceBox->onUpdate(m_FrameContext);

        // Reversed iteration for onEndFrame()
        for (auto it = m_Services.rbegin(); it != m_Services.rend(); ++it) (*it)->onEndFrame(m_FrameContext);
    }

    void ServiceStack::dispatchEvent(IEvent& event) {
        EventListener listener(event);

        // Allow event interception before propagation
        if (m_EventInterceptor) {
            m_EventInterceptor(listener);
            if (event.isConsumed()) return;
        }

        // Propagate event top to bottom through services
        for (auto it = m_Services.rbegin(); it != m_Services.rend(); ++it) {
            if (event.isConsumed()) break;
            (*it)->onEvent(listener);
        }

        // Be chatty about events that weren't consumed
        if (!event.isConsumed()) RYN_TRACE("Event '{}' was not consumed by any layer", event.getTypeId().Name);
    }

    void ServiceStack::setEventInterceptor(EventInterceptor interceptor) { m_EventInterceptor = std::move(interceptor); }

    void ServiceStack::internalPush(TypeId typeId, Box<IService>&& serviceBox) {
        IService* service = serviceBox.get();

        if (m_ServiceLUT.contains(typeId)) {
            RYN_ERROR("Failed to push service '{}' because an instance of it's type already exists!", service->getTypeId().Name);
            return;
        }

        RYN_INFO("Push service '{}'", service->getTypeId().Name);
        m_ServiceLUT[typeId] = service;
        m_Services.push_back(std::move(serviceBox));

        service->m_EventDispatcher = this;
        service->onAttach(*this);
    }

    void ServiceStack::internalPop() {
        if (m_Services.empty()) return;

        auto& box     = m_Services.back();
        auto* service = box.get();

        RYN_INFO("Pop service '{}'", service->getTypeId().Name);
        service->onDetach();

        m_ServiceLUT.erase(service->getTypeId());
        m_Services.pop_back();
    }

    IService* ServiceStack::internalRequest(TypeId typeId, bool required) {
        const auto it      = m_ServiceLUT.find(typeId);
        IService*  service = it != m_ServiceLUT.end() ? it->second : nullptr;

        if (!service && required) handleMissingService(typeId);

        return service;
    }

    void ServiceStack::handleMissingService(TypeId typeId) {
        // Fail on purpose
        RYN_VERIFY(false, "Required service {} couldn't be found!", TypeRegistry::queryType(typeId));
    }
}