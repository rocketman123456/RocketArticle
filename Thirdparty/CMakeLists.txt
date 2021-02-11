message(STATUS "Add Thirdparty")

add_library( stb_image stb_image.cpp)
add_library( glad glad/src/glad.c )
#add_library( tiny_gltf tiny_gltf.cpp )

add_library( imgui
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
)

add_library( ImGuizmo
    ImGuizmo/ImCurveEdit.cpp
    ImGuizmo/ImGradient.cpp
    ImGuizmo/ImGuizmo.cpp
    ImGuizmo/ImSequencer.cpp
)

if(PROFILE)
    add_library( Remotery Remotery/lib/Remotery.c )
    if(OpenGL)
        target_compile_options( Remotery PRIVATE -DRMT_USE_OPENGL=1 )
    elseif(Metal)
        target_compile_options( Remotery PRIVATE -DRMT_USE_METAL=1 )
    endif()
endif()
