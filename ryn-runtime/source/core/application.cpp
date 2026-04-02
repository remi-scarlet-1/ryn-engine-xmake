module;
#include "defines.h"

module Ryntime.Core;

namespace ryn {
    void IApplication::bootstrap(int argc, char** argv, Box<IApplication> appBox) {
#if RYN_FEATURE_LOGGING
        Logger::init();
#endif

        RYN_INFO("Bootstrapping...");
        RYN_VERIFY(appBox, "Provided app box is invalid!");

        appBox->run(argc, argv);
        appBox.reset();

#if RYN_FEATURE_LOGGING
        Logger::shutdown();
#endif
    }

    IApplication::IApplication()  = default;
    IApplication::~IApplication() = default;

    void IApplication::run(int, char**) {
        RYN_INFO("Initializing...");
        onInit();

        RYN_INFO("Starting...");
        // TODO: implement main loop
        // while (m_Running) {}

        RYN_INFO("Shutdown...");
        onShutdown();
    }
}