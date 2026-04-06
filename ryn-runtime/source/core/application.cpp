module;
#include "defines.h"

module Ryntime.Core;
import Ryntime.Container.Timer;
import Ryntime.Reflection;

namespace ryn {
    void IApplication::bootstrap(int argc, char** argv, Box<IApplication> appBox) {
#if RYN_FEATURE_LOGGING
        Logger::init();
#endif

        RYN_INFO("Bootstrapping...");
        RYN_VERIFY(appBox, "Provided app box is invalid!");

        TypeRegistry::rebuild();
        appBox->main(argc, argv);
        appBox.reset();

#if RYN_FEATURE_LOGGING
        Logger::shutdown();
#endif
    }

    IApplication::IApplication() {
        m_ServiceStack.setEventInterceptor([this](auto& listener) { this->onEventIntercept(listener); });
    }

    IApplication::~IApplication() = default;

    IServiceStack&   IApplication::getServiceStack() { return m_ServiceStack; }
    IServiceLocator& IApplication::getServiceLocator() { return m_ServiceStack; }

    void IApplication::main(int, char**) {
        {
            const Timer initTimer;
            RYN_INFO("Initialize...");
            onInit();
            RYN_INFO("Init completed in {:.3f} ms", initTimer.elapsedMilliSeconds());
        }

        RYN_INFO("Start...");
        while (m_Running) m_ServiceStack.update();

        RYN_INFO("Shutdown...");
        onShutdown();
    }

    void IApplication::onEventIntercept(EventListener& listener) {
        // TODO: wire up events that should cause application shutdown
        // listener.match<event::WindowClose>([this](event::WindowClose& event) {
        //     m_Running = false;
        //     event.consume();
        // });
    }
}