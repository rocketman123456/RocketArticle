message(STATUS "Add Module GECore")
message(STATUS "###################################")
add_library( GEEngine
    # GEModule
    GEModule/Application.cpp
    # GEWindow
    GEWindow/Window.cpp
)
target_link_libraries( GEEngine PRIVATE
    glfw
)
