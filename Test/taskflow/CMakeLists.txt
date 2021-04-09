message(STATUS "Add entt Test")

set(CMAKE_CXX_FLAGS "${CAMKE_CXX_FLAGS} -pthread")

add_executable( task_flow_test_01
    test_01.cpp
)

target_link_libraries( task_flow_test_01
    pthread ${CMAKE_THREAD_LIBS_INIT}
)
