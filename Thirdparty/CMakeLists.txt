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

add_library( Remotery
    Remotery/lib/Remotery.c
)
if(OpenGL)
    target_compile_options( Remotery PRIVATE
        -DRMT_USE_OPENGL=1
    )
endif()

add_subdirectory( FiberTaskingLib )
add_subdirectory( crossguid )