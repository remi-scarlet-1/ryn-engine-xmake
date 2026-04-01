module;
#include "Defines.h"

module Ryntime.Core;

namespace ryn {
    void IApplication::bootstrap(int argc, char** argv, Box<IApplication> appBox) {
        RYN_INFO("Bootstrap application...");
        RYN_VERIFY(appBox, "Provided app box is invalid!");

        appBox->run(argc, argv);
        appBox.reset();
    }

    IApplication::IApplication() = default;
    IApplication::~IApplication() = default;

    void IApplication::run(int, char**) {
        RYN_INFO("Initialize...");
        onInit();

        RYN_INFO("Start main loop...");
        while (m_Running) {}

        RYN_INFO("Shutdown...");
        onShutdown();
    }
}