message(STATUS "Add Thirdparty")

add_library( stb_image stb_image.cpp)
add_library( glad glad/src/glad.c )

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

#add_library( UDRefl
#    UDRefl/src/core/FieldPtr.cpp
#    UDRefl/src/core/IDRegistry.cpp
#    UDRefl/src/core/InvokeUtil.cpp
#    UDRefl/src/core/MethodPtr.cpp
#    UDRefl/src/core/Object.cpp
#    UDRefl/src/core/ReflMngr.cpp
#)

add_library( UGraphviz
    #UGraphviz/include/UGraphviz/Graph.h
    #UGraphviz/include/UGraphviz/Registry.h
    #UGraphviz/include/UGraphviz/Subgraph.h
    #UGraphviz/include/UGraphviz/UGraphviz.h
    UGraphviz/src/core/Graph.cpp
    UGraphviz/src/core/Registry.cpp
    UGraphviz/src/core/Subgraph.cpp
)

if(PROFILE)
    add_library( Remotery Remotery/lib/Remotery.c )
    if(OpenGL)
        target_compile_options( Remotery PRIVATE -DRMT_USE_OPENGL=1 )
    elseif(Metal)
        target_compile_options( Remotery PRIVATE -DRMT_USE_METAL=1 )
    endif()
endif()
