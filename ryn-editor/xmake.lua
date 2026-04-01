target("RynEditor")
    set_kind("binary")
    add_extrafiles("**/xmake.lua", {group = "xmake"})

    add_deps("RynRuntime")

    add_includedirs("./Source/")
    add_headerfiles("./Source/**.h")

    --add_files("./Source/**.ixx")
    add_files("./Source/**.cpp")