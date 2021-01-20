message(STATUS "Add entt Test")

add_executable( event_test
    event_test.cpp
)
target_link_libraries( event_test
    EnTT::EnTT
)