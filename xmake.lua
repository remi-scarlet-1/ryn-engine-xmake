set_project("RynEngine")
set_version("0.0.1")

set_languages("cxx23")
set_encodings("utf-8")

set_policy("build.c++.modules", true)
set_policy("build.c++.modules.std", true)
set_policy("build.c++.modules.reuse", true)

add_rules("plugin.compile_commands.autoupdate", {outputdir = "./"})
add_rules(
    "mode.debug",
    "mode.release",
    "mode.releasedbg"
)

-- Definitions
if is_mode("debug") then add_defines("RYN_CONFIG_DEBUG") end
if is_mode("releasedbg") then add_defines("RYN_CONFIG_RELEASE") end
if is_mode("release") then add_defines("RYN_CONFIG_DIST") end

if is_plat("windows") then add_defines("RYN_PLATFORM_WIN") end
if is_plat("linux") then add_defines("RYN_PLATFORM_LINUX") end

-- Optimizations & Platform
if is_mode("debug") then
    -- Note: set_runtimes only affects Windows and Linux is dyn linked by default
    set_runtimes("MDd")
    set_fpmodels("precise")

elseif is_mode("releasedbg") or is_mode("release") then
    set_runtimes("MD")
    set_fpmodels("fast")
    add_vectorexts("avx2")
end

add_cxxflags("/Zc:preprocessor", "/Zc:__cplusplus", {tools = {"cl"}})
if is_plat("windows") then
    add_defines("_CRT_SECURE_NO_WARNINGS")
end

-- Dependencies
add_cxflags("-DFMT_USE_LOCALE=0") -- Disable use locale on spdlogs bundled fmt
add_requires("spdlog 1.17.0", {configs = {header_only = true}})

includes(
    "ryn-runtime", 
    "ryn-editor"
)