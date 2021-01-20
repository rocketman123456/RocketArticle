message(STATUS "Add copp Test")

find_package(libcopp CONFIG REQUIRED)

add_executable( sample_01
    sample_01.cpp
)
target_link_libraries( sample_01 PRIVATE 
    libcopp::copp libcopp::cotask
)