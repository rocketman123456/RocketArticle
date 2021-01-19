cmake_minimum_required (VERSION 3.2)
cmake_policy(SET CMP0054 NEW)

#### Main Project Config ############################
set(RENDER_API "Vulkan") # Render API Select: OpenGL Vulkan Metal
set(CMAKE_BUILD_TYPE "Debug") # Debug or Release
set(PROFILE 1) # This Option will generate runtime profile
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
if(Vulkan)
    find_package(Vulkan REQUIRED)
    find_package(glslang CONFIG REQUIRED)
endif()
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
    ${PROJECT_SOURCE_DIR}/Thirdparty/stb_image
    ${PROJECT_SOURCE_DIR}/Thirdparty/crossguid/include
    ${PROJECT_SOURCE_DIR}/Thirdparty/imgui
    ${EIGEN3_INCLUDE_DIRS}
    ${BULLET_INCLUDE_DIRS}
    ${OPENAL_INCLUDE_DIRS}
    ${SNDFILE_INCLUDE_DIRS}
)
include_directories(
    ${PROJECT_SOURCE_DIR}/Rocket/Engine
    ${PROJECT_SOURCE_DIR}/Rocket/RHI
    ${PROJECT_SOURCE_DIR}/Rocket/Platform
)
if(Vulkan)
    include_directories(
        ${Vulkan_INCLUDE_DIRS}
    )
endif()
if(PROFILE)
    include_directories(
        ${PROJECT_SOURCE_DIR}/Thirdparty/Remotery/lib
    )
endif()
#### Project Includes ###############################

#### Project Sources ################################
add_subdirectory( Thirdparty )
add_subdirectory( Rocket )

if(PROFILE)
    set( ENGINE_LIBRARY
        ${Threads_LIBS}
        glfw yaml-cpp glm 
        Eigen3::Eigen glad::glad EnTT::EnTT
        spdlog::spdlog spdlog::spdlog_header_only
        OpenAL::OpenAL SndFile::sndfile
        imgui stb_image crossguid Remotery
        CACHE STRING ""
    )
else()
    set( ENGINE_LIBRARY
        ${Threads_LIBS}
        glfw yaml-cpp glm 
        Eigen3::Eigen glad::glad EnTT::EnTT
        spdlog::spdlog spdlog::spdlog_header_only
        OpenAL::OpenAL SndFile::sndfile
        imgui stb_image crossguid
        CACHE STRING ""
    )
endif()

if(Apple)
    set(ENGINE_PLATFORM_LIBRARY PlatformWindow CACHE STRING "")
elseif(Linux)
    set(ENGINE_PLATFORM_LIBRARY PlatformWindow CACHE STRING "")
elseif(Windows)
    set(ENGINE_PLATFORM_LIBRARY PlatformWindow CACHE STRING "")
endif()

if(OpenGL)
    set(ENGINE_RENDER_LIBRARY RendererOpenGL CACHE STRING "")
elseif(Vulkan)
    set(ENGINE_RENDER_LIBRARY RendererVulkan ${Vulkan_LIBRARIES} CACHE STRING "")
elseif(Metal)
    set(ENGINE_RENDER_LIBRARY RendererMetal CACHE STRING "")
endif()

add_subdirectory( Test )
add_subdirectory( Editor )
add_subdirectory( Sandbox )

message(STATUS "###################################")
#### Project Sources ################################