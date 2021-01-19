message(STATUS "Add Thirdparty")

add_library( stb_image
    stb_image/stb_image.cpp
)

add_library( imgui
    imgui/imgui.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_demo.cpp
)

add_library( crossguid
    crossguid/src/guid.cpp
)

add_library( Remotery
    Remotery/lib/Remotery.c
)

add_subdirectory( FiberTaskingLib )