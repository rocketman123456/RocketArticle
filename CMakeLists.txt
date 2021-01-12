cmake_minimum_required (VERSION 2.8.12)

#### Main Project Config ############################
set(RENDER_API "OpenGL") # Render API Select
set(CMAKE_BUILD_TYPE "Debug") # Debug or Release
set(PROFILE 0) # This Option will generate runtime profile
#### Main Project Config ############################

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

project( RocketGE )

#### Project Definition #############################
if(PROFILE)
    add_definitions(-DRK_PROFILE)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DRK_DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DRK_RELEASE)
endif()

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(Linux 1)
    add_definitions(-DPLATFORM_LINUX)
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(Windows 1)
    add_definitions(-DPLATFORM_WINDOWS)
elseif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(Apple 1)
    add_definitions(-DPLATFORM_APPLE)
endif ()

if (RENDER_API MATCHES "OpenGL")
    set(OpenGL 1)
    add_definitions(-DRK_OPENGL)
    # TODO : use opengl 4.1 for all
    #if(Windows)
    #    add_definitions(-DHIGH_OPENGL_VERSION)
    #elseif(Linux)
    #    add_definitions(-DHIGH_OPENGL_VERSION)
    #endif()
elseif (RENDER_API MATCHES "Vulkan")
    set(Vulkan 1)
    add_definitions(-DRK_VULKAN)
elseif (RENDER_API MATCHES "Metal")
    set(Metal 1)
    add_definitions(-DRK_METAL)
endif()

configure_file (
    "${PROJECT_SOURCE_DIR}/RKConfig.h.in"
    "${PROJECT_SOURCE_DIR}/Rocket/Engine/RKConfig.h"
)
#### Project Definition #############################

#### Project Packages ###############################
find_package(Threads REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(Bullet CONFIG REQUIRED)
find_package(OpenAL CONFIG REQUIRED)
find_package(SndFile CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(tinyobjloader CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
#find_package(marl CONFIG REQUIRED)
#### Project Packages ###############################

#### Project Messages ###############################
message(STATUS "###################################")

message(STATUS "CMAKE_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "Project Name is ${PROJECT_NAME}")
message(STATUS "Render API Set to ${RENDER_API}")
message(STATUS "Build System Set to ${CMAKE_BUILD_TYPE}")
message(STATUS "Operation System is ${CMAKE_SYSTEM}")
message(STATUS "Current Platform: ${CMAKE_SYSTEM_NAME} ")

message(STATUS "###################################")
#### Project Messages ###############################

#### Project Includes ###############################
include_directories(
    #${PROJECT_SOURCE_DIR}/Thirdparty/glad/include
    ${PROJECT_SOURCE_DIR}/Thirdparty/stb_image
    ${PROJECT_SOURCE_DIR}/Thirdparty/crossguid/include
    ${PROJECT_SOURCE_DIR}/Thirdparty/imgui
    ${EIGEN3_INCLUDE_DIRS}
    ${BULLET_INCLUDE_DIRS}
    ${OPENAL_INCLUDE_DIRS}
    ${SNDFILE_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/Rocket/Engine
)

#if(OpenGL)
#    include_directories( ${PROJECT_SOURCE_DIR}/Rocket/RHI/OpenGL )
#elseif(Vulkan)
#    include_directories( ${PROJECT_SOURCE_DIR}/Rocket/RHI/Vulkan )
#elseif(Metal)
#    include_directories( ${PROJECT_SOURCE_DIR}/Rocket/RHI/Metal )
#endif()

#if(Apple)
#    include_directories( ${PROJECT_SOURCE_DIR}/Rocket/Platform/Apple )
#elseif(Linux)
#    include_directories( ${PROJECT_SOURCE_DIR}/Rocket/Platform/Linux )
#elseif(Windows)
#    include_directories( ${PROJECT_SOURCE_DIR}/Rocket/Platform/Windows )
#endif()
#### Project Includes ###############################

add_subdirectory( Thirdparty )
add_subdirectory( Rocket )

set( ENGINE_LIBRARY
    ${Threads_LIBS}
    glfw yaml-cpp glm 
    Eigen3::Eigen glad::glad EnTT::EnTT
    spdlog::spdlog spdlog::spdlog_header_only
    OpenAL::OpenAL SndFile::sndfile
    imgui stb_image crossguid
    CACHE STRING ""
)

#if(Apple)
#    set(ENGINE_PLATFORM_LIBRARY Platform_Apple CACHE STRING "")
#elseif(Linux)
#    set(ENGINE_PLATFORM_LIBRARY Platform_Linux CACHE STRING "")
#elseif(Windows)
#    set(ENGINE_PLATFORM_LIBRARY Platform_Windows CACHE STRING "")
#endif()

#if(OpenGL)
#    set(ENGINE_RENDER_LIBRARY Render_OpenGL CACHE STRING "")
#elseif(Vulkan)
#    set(ENGINE_RENDER_LIBRARY Render_Vulkan CACHE STRING "")
#elseif(Metal)
#    set(ENGINE_RENDER_LIBRARY Render_Metal CACHE STRING "")
#endif()

add_subdirectory( Test )
add_subdirectory( Editor )
add_subdirectory( Sandbox )

message(STATUS "###################################")