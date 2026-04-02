#pragma once

#if !RYN_RUNTIME
import Ryntime.Container.Box;
import Ryntime.Core;

ryn::Box<ryn::IApplication> createApplication();

int main(const int argc, char** argv) { ryn::IApplication::bootstrap(argc, argv, createApplication()); }
#endif