target("simple_app")
    set_kind("binary")
    add_files("*.cpp")
    --add_packages("vcpkg::taskflow")
    add_deps(
        "RocketEngine", 
        "RocketPlatform", 
        "RocketRender", 
        "CSerialPort", 
        "crossguid"
    )
    add_packages(
        "vcpkg::spdlog",
        "vcpkg::fmt",
        "vcpkg::eigen3",
        "vcpkg::entt",
        "vcpkg::gli",
        "vcpkg::yaml-cpp",
        "vcpkg::glfw3",
        "vcpkg::shaderc",
        "vcpkg::openal-soft"
    )
    -- Add Platform Dependent Libs
    if is_plat("linux", "macosx") then
        add_links("pthread", "m", "dl")
        if is_plat("macosx") then
            add_frameworks("Cocoa", "IOKit", "CoreVideo", "CoreAudio")
        end
    elseif is_plat("windows") then
        add_links("user32", "gdi32", "shell32", "kernel32", "advapi32")
        add_ldflags("/subsystem:console")
    end