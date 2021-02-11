message(STATUS "Add SimpleApp")
if(Apple)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework Cocoa -framework IOKit -framework CoreVideo")
    #target_compile_options(SimpleApp PRIVATE "-framework Cocoa -framework IOKit -framework CoreVideo")
endif()
add_executable( SimpleApp
    SimpleApp.cpp
)
target_link_libraries( SimpleApp
    RocketEngine 
    ${ENGINE_LIBRARY} 
    ${ENGINE_PLATFORM_LIBRARY}
    ${ENGINE_RENDER_LIBRARY}
)