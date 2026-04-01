#include "Entrypoint.h"

import Ryntime;

class EditorApplication final : public ryn::IApplication {
protected:
    void onInit() override {}
    void onShutdown() override {}
};

ryn::Box<ryn::IApplication> createApplication() { return ryn::Box<EditorApplication>::make(); }