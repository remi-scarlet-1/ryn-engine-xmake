module;
#include "Defines.h"

export module Ryntime.Core:Application;
export import Ryntime.Container;

export namespace ryn {
    class RYN_API IApplication {
    public:
        static void bootstrap(int argc, char** argv, Box<IApplication> appBox);

        IApplication();
        virtual ~IApplication();

    protected:
        virtual void onInit() = 0;
        virtual void onShutdown() = 0;

    private:
        bool m_Running = true;
        
        void run(int argc, char** argv);
    };
}