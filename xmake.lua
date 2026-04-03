set_project("ryn-engine")
set_version("0.0.1")

set_languages("cxx23")
set_encodings("utf-8")
set_policy("build.c++.modules", true)
set_policy("build.c++.modules.std", true)
set_policy("build.c++.modules.reuse", true)

add_rules("plugin.compile_commands.autoupdate", {outputdir = "./"})

-- Configurations
add_rules(
    "mode.debug",
    "mode.releasedbg",
    "mode.release"
)

if is_mode("debug") then
    set_fpmodels("precise")
    add_defines("RYN_CONFIG_DEBUG")
else
    if is_mode("releasedbg") then add_defines("RYN_CONFIG_RELEASE") end
    if is_mode("release") then add_defines("RYN_CONFIG_DIST") end

    set_fpmodels("fast")
    add_vectorexts("avx2")
end

-- Platform
if is_plat("windows") then
    set_toolchains("msvc")

    if is_mode("debug") then      set_runtimes("MDd") end
    if is_mode("releasedbg") then set_runtimes("MD") end
    if is_mode("release") then    set_runtimes("MD") end

    add_cxxflags("/Zc:preprocessor", "/Zc:__cplusplus")
    add_defines(
        "_CRT_SECURE_NO_WARNINGS",
        "RYN_PLATFORM_WIN"
    )
end

if is_plat("linux") then
    set_toolchains("clang")
    set_runtimes("c++_shared")

    add_defines("RYN_PLATFORM_LINUX")
end

-- Fetch dependencies
add_cxflags("-DFMT_USE_LOCALE=0")
add_requires("spdlog 1.17.0", {configs = {header_only = true}})

includes(
    "ryn-runtime", 
    "ryn-editor"
)