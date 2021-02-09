message(STATUS "Add Thirdparty")

add_library( stb_image stb_image.cpp)
add_library( tiny_gltf tiny_gltf.cpp )
add_library( tinyobjloader tinyobjloader/tiny_obj_loader.cc )
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

add_library( Remotery Remotery/lib/Remotery.c )
if(OpenGL)
    target_compile_options( Remotery PRIVATE -DRMT_USE_OPENGL=1 )
elseif(Metal)
    target_compile_options( Remotery PRIVATE -DRMT_USE_METAL=1 )
endif()

add_library( crossguid crossguid/src/guid.cpp )
if(WIN32)
    target_compile_definitions(crossguid PRIVATE GUID_WINDOWS)
elseif(APPLE)
    find_library(CFLIB CoreFoundation)
    target_link_libraries(crossguid ${CFLIB})
    target_compile_definitions(crossguid PRIVATE GUID_CFUUID)
elseif(ANDROID)
    # GUID_ANDROID is used in the headers, so make PUBLIC
    target_compile_definitions(crossguid PUBLIC GUID_ANDROID)
else()
    find_package(Libuuid REQUIRED)
    if (NOT LIBUUID_FOUND)
        message(FATAL_ERROR "You might need to run 'sudo apt-get install uuid-dev' or similar")
    endif()
    target_include_directories(crossguid PRIVATE ${LIBUUID_INCLUDE_DIR})
    target_link_libraries(crossguid ${LIBUUID_LIBRARY})
    target_compile_definitions(crossguid PRIVATE GUID_LIBUUID)
endif()

target_compile_options(crossguid PRIVATE ${WARNINGS})

add_subdirectory( taskflow )
#add_subdirectory( FiberTaskingLib )
