set_project("Rocket")
add_rules("mode.debug", "mode.release")
set_languages("c99", "c++20")

add_requires(
    "vcpkg::spdlog",
    "vcpkg::fmt",
    "vcpkg::entt",
    "vcpkg::eigen3",
    "vcpkg::yaml-cpp",
    "vcpkg::glfw3",
    "vcpkg::shaderc",
    "vcpkg::glslang",
    "vcpkg::nlohmann-json",
    "vcpkg::gli",
    "vcpkg::openal-soft"
)

option("render_api")
    set_showmenu(true)
    set_description("The Render API config option")
option_end()
if is_config("render_api", "opengl") then
    add_defines("RK_OPENGL")
    add_requires(
        "vcpkg::glad"
    )
    printf("OpenGL Render API\n")
elseif is_config("render_api", "opengl_es") then
    add_defines("RK_OPENGL_ES")
    add_requires(
        "vcpkg::glad"
    )
    printf("OpenGL ES Render API\n")
elseif is_config("render_api", "vulkan") then
    add_defines("RK_VULKAN")
    add_requires(
        "vcpkg::volk",
        "vcpkg::vulkan-headers"
    )
    printf("Vulkan Render API\n")
elseif is_config("render_api", "soft_render") then 
    add_defines("RK_SOFT_RENDER")
    printf("Soft Render API\n")
elseif is_config("render_api", "metal") then
    add_defines("RK_METAL")
    add_requires(
        ""
    )
end

add_includedirs(
    "Rocket/Engine",
    "Rocket/Platform",
    "Rocket/RHI",
    "Thirdparty/CSerialPort/include",
    "Thirdparty/crossguid/include",
    "Thirdparty/imgui"
)

if is_plat("linux", "macosx", "windows") then
    add_defines("RK_DESKTOP", "GLFW_INCLUDE_NONE")
    if is_plat("linux") then
        add_defines("RK_LINUX")
    elseif is_plat("macosx") then
        add_defines("RK_MACOS")
    elseif is_plat("windows") then
        add_defines("RK_WINDOWS", "_CRT_SECURE_NO_WARNINGS")
    end
end

if is_mode("debug") then
    -- 添加DEBUG编译宏
    add_defines("RK_DEBUG")
    -- 启用调试符号
    set_symbols("debug")
    -- 禁用优化
    set_optimize("none")
-- 如果是release或者profile模式
elseif is_mode("release", "profile") then
    -- 如果是release模式
    if is_mode("release") then
        -- 添加DEBUG编译宏
        add_defines("RK_RELEASE")
        -- 隐藏符号
        set_symbols("hidden")
        -- strip所有符号
        set_strip("all")
        -- 忽略帧指针
        add_cxflags("-fomit-frame-pointer")
        add_mxflags("-fomit-frame-pointer")
        -- 优化
        set_optimize("fastest")
    -- 如果是profile模式
    else
        -- 添加DEBUG编译宏
        add_defines("RK_PROFILE")
        -- 启用调试符号
        set_symbols("debug")
        -- 优化
        set_optimize("fastest")
    end
    -- 添加扩展指令集
    add_vectorexts("sse2", "sse3", "ssse3", "mmx")
end

includes(
    "Rocket",
    "Sandbox",
    "Thirdparty"
)
