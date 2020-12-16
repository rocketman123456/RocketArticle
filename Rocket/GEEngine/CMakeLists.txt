message(STATUS "Add Module GEEngine")
message(STATUS "###################################")
add_library( GEEngine
    # GECore
    # GEModule
    GEModule/Application.cpp
    # GEWindow
    GEWindow/Window.cpp
)
