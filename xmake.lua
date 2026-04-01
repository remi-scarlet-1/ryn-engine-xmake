set_project("RynEngine")
set_version("0.0.1")

set_languages("cxx23")

set_policy("build.c++.modules", true)
set_policy("build.c++.modules.std", true)
set_policy("build.c++.modules.reuse", true)

add_rules(
    "mode.debug",
    "mode.release",
    "mode.releasedbg"
)

add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode"})

-- Definitions
if is_mode("debug") then add_defines("RYN_CONFIG_DEBUG") end
if is_mode("releasedbg") then add_defines("RYN_CONFIG_RELEASE") end
if is_mode("release") then add_defines("RYN_CONFIG_DIST") end

if is_plat("windows") then add_defines("RYN_PLATFORM_WIN") end
if is_plat("linux") then add_defines("RYN_PLATFORM_LINUX") end

-- Additional optimizations
if is_mode("debug") then
    set_fpmodels("precise")

elseif is_mode("releasedbg") or is_mode("release") then
    set_fpmodels("fast")
    add_vectorexts("avx2", "avx512")
end

includes(
    "ryn-runtime", 
    "ryn-editor"
)