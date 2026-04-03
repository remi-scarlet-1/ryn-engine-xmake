target("ryn-editor")
    set_kind("binary")
    add_extrafiles("**/xmake.lua", {group = "xmake"})

    add_deps("ryn-runtime")

    add_includedirs("./source/")
    --add_headerfiles("./source/**.h")
    --add_files("./source/**.ixx")
    add_files("./source/**.cpp")