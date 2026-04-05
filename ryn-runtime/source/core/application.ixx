module;
#include "defines.h"

export module Ryntime.Core:Application;
export import Ryntime.Container;

export namespace ryn {
    struct FrameTime {
        double   Total      = 0.0;
        double   Delta      = 0.0;
        uint64_t FrameCount = 0;
    };

    class RYN_API IApplication {
    public:
        static void bootstrap(int argc, char** argv, Box<IApplication> appBox);

        IApplication();
        virtual ~IApplication();

    protected:
        virtual void onInit()     = 0;
        virtual void onShutdown() = 0;

    private:
        bool m_Running = true;

        void main(int argc, char** argv);
    };
}