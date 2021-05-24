target("RocketRender")
    add_files(
        "Vulkan/*.cpp",
        "Vulkan/Render/*.cpp"
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
        "vcpkg::openal-soft",
        {public = true}
    )
