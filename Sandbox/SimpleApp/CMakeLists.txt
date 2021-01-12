message(STATUS "Add SimpleApp")
add_executable( SimpleApp
    SimpleApp.cpp
)
target_link_libraries( SimpleApp
    RocketEngine 
    ${ENGINE_LIBRARY} 
    ${ENGINE_PLATFORM_LIBRARY}
)