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