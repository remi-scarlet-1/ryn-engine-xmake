target("RynRuntime")
    set_kind("static")
    add_extrafiles("**/xmake.lua", {group = "xmake"})
    
    add_includedirs("./Source", {public = true})
    add_headerfiles("./Source/**.h")

    add_files("./Source/**.ixx", {public = true})
    add_files("./Source/**.cpp")