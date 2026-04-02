target("RynRuntime")
    set_kind("shared")
    add_extrafiles("**/xmake.lua", {group = "xmake"})
    
    add_defines("RYN_RUNTIME")

    add_includedirs("./source", {public = true})
    add_headerfiles("./source/**.h")
    add_files("./source/**.ixx", {public = true})
    add_files("./source/**.cpp")