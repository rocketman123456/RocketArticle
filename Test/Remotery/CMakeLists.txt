message(STATUS "Add Remotery Test")

add_executable( remotery_sample
    sample.cpp
)
target_link_libraries( remotery_sample
    Remotery
)
target_include_directories( remotery_sample PRIVATE
    ${PROJECT_SOURCE_DIR}/Thirdparty/Remotery/lib
)