cmake_minimum_required (VERSION 3.2)
cmake_policy(SET CMP0054 NEW)

#### Main Project Config ############################
set(RENDER_API "Vulkan") # Render API Select: OpenGL Vulkan Metal
set(CMAKE_BUILD_TYPE "Debug") # Debug or Release
set(PROFILE 0) # This Option will generate runtime profile
set(PROFILE_FILE 0) # This Option will generate file log
#### Main Project Config ############################

#### Project Language Config ########################
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

#if (CMAKE_SYSTEM_NAME MATCHES "Linux")
#    set(CMAKE_TOOLCHAIN_FILE /home/developer/Program/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")
#elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
#    set(CMAKE_TOOLCHAIN_FILE C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")
#elseif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
#    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework Cocoa -framework IOKit -framework CoreVideo")
#    set(CMAKE_TOOLCHAIN_FILE /Users/developer/Program/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")
#endif ()

project( Rocket )
#### Project Language Config ########################

#### Project Definition #############################
if(PROFILE)
    add_definitions(-DRK_PROFILE)
endif()
if(PROFILE_FILE)
    add_definitions(-DRK_PROFILE_FILE)
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

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(WARNINGS "-Werror")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(WARNINGS "-Werror")
elseif(MSVC)
    set(WARNINGS "/WX" "/W4")
endif()
#### Project Definition #############################

#### Project Packages ###############################
find_package(Threads REQUIRED)
#if(openGL)
#    find_package(OpenGL REQUIRED)
#endif()
if(Vulkan)
    find_package(Vulkan REQUIRED)
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
    ${PROJECT_SOURCE_DIR}/External/${CMAKE_SYSTEM_NAME}/include
    ${PROJECT_SOURCE_DIR}/External/${CMAKE_SYSTEM_NAME}/include/eigen3
    ${PROJECT_SOURCE_DIR}/Thirdparty/stb
    ${PROJECT_SOURCE_DIR}/Thirdparty/imgui
    ${PROJECT_SOURCE_DIR}/Thirdparty/glad/include
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
link_directories(
    ${PROJECT_SOURCE_DIR}/External/${CMAKE_SYSTEM_NAME}/lib
)
#### Project Includes ###############################

#### Project Sources ################################
add_subdirectory( Thirdparty )
add_subdirectory( Rocket )

if(PROFILE)
    set( ENGINE_LIBRARY
        ${Threads_LIBS} glfw3 glad stb_image imgui sndfile tinyobjloader Remotery CACHE STRING ""
    )
else()
    set( ENGINE_LIBRARY
        ${Threads_LIBS} glfw3 glad stb_image imgui sndfile tinyobjloader CACHE STRING ""
    )
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(Apple)
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework Cocoa -framework IOKit -framework CoreVideo")
        set(ENGINE_PLATFORM_LIBRARY PlatformWindow spdlog crossguid openal yaml-cpp CACHE STRING "")
    elseif(Linux)
        set(ENGINE_PLATFORM_LIBRARY PlatformWindow spdlog crossguid openal yaml-cpp CACHE STRING "")
    elseif(Windows)
        set(ENGINE_PLATFORM_LIBRARY PlatformWindow spdlogd crossguid-dgb OpenAL32 libyaml-cppmdd CACHE STRING "")
    endif()
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(Apple)
        set(ENGINE_PLATFORM_LIBRARY PlatformWindow spdlog crossguid openal yaml-cpp CACHE STRING "")
    elseif(Linux)
        set(ENGINE_PLATFORM_LIBRARY PlatformWindow ${CMAKE_DL_LIBS} spdlog crossguid openal yaml-cpp CACHE STRING "")
    elseif(Windows)
        set(ENGINE_PLATFORM_LIBRARY PlatformWindow spdlog crossguid OpenAL32 libyaml-cppmd CACHE STRING "")
    endif()
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

#### Project Sources ################################