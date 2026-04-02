module;
#include "defines.h"

module Ryntime.Core;

namespace ryn {
    void IApplication::bootstrap(int argc, char** argv, Box<IApplication> appBox) {
#if RYN_FEATURE_LOGGING
        Logger::init();
#endif

        RYN_INFO("Bootstrap application...");
        RYN_VERIFY(appBox, "Provided app box is invalid!");
        RYN_WARN("Warning test!");

        appBox->run(argc, argv);
        appBox.reset();

#if RYN_FEATURE_LOGGING
        Logger::shutdown();
#endif
    }

    IApplication::IApplication()  = default;
    IApplication::~IApplication() = default;

    void IApplication::run(int, char**) {
        RYN_INFO("Initialize...");
        onInit();

        RYN_INFO("Start main loop...");
        // TODO: implement main loop
        // while (m_Running) {}

        RYN_INFO("Shutdown...");
        onShutdown();
    }
}