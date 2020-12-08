# Rocket 引擎系列（三）<br>
本项目面对的是多平台，具体是指Mac，Windows和Linux，为了能够使代码顺利在不同平台下编译，我们需要对CMake进行配置，使其能够检测不同平台，并对其进行处理。
# 1.CMake 配置
- `CMakeLists.txt`
```
cmake_minimum_required (VERSION 2.8.12)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(VCPKG_ROOT "/usr/local/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE PATH "")
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(VCPKG_ROOT "D:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE PATH "")
elseif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(VCPKG_ROOT "/Users/developer/Program/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE PATH "")
endif ()
set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT})

#### Main Project Config ############################
set(RENDER_API "OpenGL")
set(CMAKE_BUILD_TYPE "Debug")
set(PROFILE 0)
#### Main Project Config ############################

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

message(STATUS "###################################")

project( RocketGE )
message(STATUS "CMAKE_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "Project Name ${PROJECT_NAME}")

configure_file (
    "${PROJECT_SOURCE_DIR}/RKConfig.h.in"
    "${PROJECT_SOURCE_DIR}/Rocket/GEEngine/RKConfig.h"
)

message(STATUS "###################################")

if(PROFILE)
    add_definitions(-DRK_PROFILE)
endif()
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DRK_DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DRK_RELEASE)
endif()
message(STATUS "Build System Set to ${CMAKE_BUILD_TYPE}")

message(STATUS "operation system is ${CMAKE_SYSTEM}")
if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    message(STATUS "current platform: Linux ")
    set(Linux 1)
    add_definitions(-DPLATFORM_LINUX)
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
    message(STATUS "current platform: Windows")
    set(Windows 1)
    add_definitions(-DPLATFORM_WINDOWS)
elseif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    message(STATUS "current platform: Apple Darwin")
    set(Apple 1)
    add_definitions(-DPLATFORM_APPLE)
else ()
    message(STATUS "other platform: ${CMAKE_SYSTEM_NAME}")
endif ()

message(STATUS "###################################")

message(STATUS "Render API Set to ${RENDER_API}")
if (RENDER_API MATCHES "OpenGL")
    set(OpenGL 1)
    add_definitions(-DRK_OPENGL)
    if(Windows)
        add_definitions(-DHIGH_OPENGL_VERSION)
    elseif(Linux)
        add_definitions(-DHIGH_OPENGL_VERSION)
    endif()
elseif (RENDER_API MATCHES "Vulkan")
    set(Vulkan 1)
    add_definitions(-DRK_VULKAN)
elseif (RENDER_API MATCHES "Metal")
    set(Metal 1)
    add_definitions(-DRK_METAL)
endif()

message(STATUS "###################################")

add_subdirectory( Test )
```
其中主要设定了各种defination，用于在代码中识别不同的编译对象，包括编译平台，渲染API选择等。其中，下面的代码用于生成对应的配置文件：
```
configure_file (
    "${PROJECT_SOURCE_DIR}/RKConfig.h.in"
    "${PROJECT_SOURCE_DIR}/Rocket/GEEngine/RKConfig.h"
)
```
- `RKConfig.h.in`
```
#define PROJECT_SOURCE_DIR "@PROJECT_SOURCE_DIR@"
#define RENDER_API "@RENDER_API@"
#define BUILD_TYPE "@CMAKE_BUILD_TYPE@"
```
在本项目中，渲染暂时只拥有OpenGL一个版本，未来首先会集成Vulkan，并将整个项目进行并行化处理。<br>
# 2.添加第一个Application<br>

# 3.添加第一个窗口<br>

