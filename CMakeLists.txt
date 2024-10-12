cmake_minimum_required(VERSION 3.28)

enable_language(CXX)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_DEBUG_POSTFIX "-d")
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(cpplox)

add_executable(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_sources(${PROJECT_NAME} PRIVATE
    src/main.cpp
    src/chunk.cpp
    src/chunk.hpp
    src/debug.hpp
    src/debug.cpp
    src/value.hpp
    src/value.cpp
    src/vm.hpp
    src/vm.cpp
    src/compiler.hpp
    src/compiler.cpp
)
