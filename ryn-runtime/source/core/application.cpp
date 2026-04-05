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

    IApplication::IApplication()  = default;
    IApplication::~IApplication() = default;

    void IApplication::main(int, char**) {
        {
            const Timer initTimer;
            RYN_INFO("Initialize...");
            onInit();
            RYN_INFO("Init completed in {:.3f} ms", initTimer.elapsedMilliSeconds());
        }

        RYN_INFO("Start...");
        // TODO: implement main loop
        // while (m_Running) {}

        RYN_INFO("Shutdown...");
        onShutdown();
    }
}