message(STATUS "Add copp Test")

find_package(libcopp CONFIG REQUIRED)

add_executable( sample_01 sample_01.cpp )
target_link_libraries( sample_01 PRIVATE libcopp::copp libcopp::cotask )

add_executable( sample_02 sample_02.cpp )
target_link_libraries( sample_02 PRIVATE libcopp::copp libcopp::cotask )

add_executable( sample_03 sample_03.cpp )
target_link_libraries( sample_03 PRIVATE libcopp::copp libcopp::cotask )

add_executable( sample_04 sample_04.cpp )
target_link_libraries( sample_04 PRIVATE libcopp::copp libcopp::cotask )

add_executable( sample_05 sample_05.cpp )
target_link_libraries( sample_05 PRIVATE libcopp::copp libcopp::cotask )

add_executable( sample_06 sample_06.cpp )
target_link_libraries( sample_06 PRIVATE libcopp::copp libcopp::cotask )
