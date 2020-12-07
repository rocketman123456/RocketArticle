# Rocket 引擎系列<br>
游戏引擎，如Unreal 4和Unity，可以进行游戏开发，或者是开发其他的软件应用，做场景展示等。我开展的这个系列，就是为了搭建一个框架，模仿游戏引擎，实现一定的对应功能，并针对这个框架进行应用/实例演示，作为个人的学习与笔记。
# 一、项目开始--RocketGE<br>
从本科起，就一直想进行引擎开发的相关工作，但是剧本是不会写的，只能做程序员了。经过学习整理，打算开始这段旅程。这里参考了[高品质游戏开发](https://www.zhihu.com/column/c_119702958)的内容，结合[Cherno Game Series](https://www.youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT)，又进行修改，得到的便是这个系列。这个可以算是个人总结，也希望能够对别人起到一定帮助。<br> 
本系列面向有一定C++基础的童鞋，有关的部分编程知识并不会在这里详细讲解。<br>
首先使用CMake开始第一个项目吧。<br>
搭建项目，首先确定一下文件夹结构。<br>
> - SourceDir<br>
>    - Assets<br>
>    - Rocket<br>
>    - Sandbox<br>
>    - Test<br>
>    - CMakeLists.txt<br>

其中，**Rocket**用于存放主体代码，**Sandbox**用于存放应用Rocket的例子，比如渲染和其他功能，**Test**用于存放学习以及测试的代码，**Assets**存放项目的资源文件。本项目存于[github](https://github.com/rocketman123456/RocketArticle)。<br>
## 1.建立github仓库<br>
请从web界面新建项目，选择.gitignore为C++类型，然后clone到电脑本地<br>
## 2.建立CMakeLists.txt<br>
[CMake](https://cmake.org/)是一套跨平台的项目配置系统，可以用于生成不同平台下的项目文件，本系列的代码都是面向Windows、MacOS和Linux，跨平台的，所以跨平台的项目配置必不可少。下面是第一个项目的配置内容：<br>
- CMakeLists.txt
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
set(CMAKE_BUILD_TYPE "Debug")
#### Main Project Config ############################

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

message(STATUS "###################################")

project( RocketGE )
message(STATUS "CMAKE_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "Project Name ${PROJECT_NAME}")

add_subdirectory( Test )
```
- Test/CMakeLists.txt
```
add_subdirectory( HelloWorld )
```

- Test/HelloWorld/CMakeLists.txt
```
add_executable( HelloWorld
    main.cpp
)
```

其中使用[vcpkg](https://github.com/microsoft/vcpkg)进行第三方库的管理，有关vcpkg的使用请自行百度。在Mac上面配置项目，命令`cmake -DCMAKE_TOOLCHAIN_FILE=/Users/developer/Program/vcpkg/scripts/buildsystems/vcpkg.cmake ..`，其中vcpkg的路径请根据个人项目自行修改。
## 3.第一个程序
- main.cpp
```
#include <iostream>

int main()
{
    std::cout << "Hello RocketGE !" << std::endl;
    return 0;
}
```

![article_01.1](https://github.com/rocketman123456/RocketArticle/Articles/Picture/article_01.1.png "article_01 Hello World")