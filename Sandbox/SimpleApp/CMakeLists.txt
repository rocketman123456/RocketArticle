message(STATUS "Add SimpleApp")
if(Apple)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework Cocoa -framework IOKit -framework CoreVideo")
endif()
add_executable( SimpleApp
    SimpleApp.cpp
    RobotControl.cpp
    RobotUI.cpp
    RobotSerialPort.cpp
)
target_link_libraries( SimpleApp PRIVATE
    RocketEngine 
    ${ENGINE_LIBRARY} 
    ${ENGINE_PLATFORM_LIBRARY}
    ${ENGINE_RENDER_LIBRARY}
)