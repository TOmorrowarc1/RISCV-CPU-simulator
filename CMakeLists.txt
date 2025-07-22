cmake_minimum_required(VERSION 3.10)
project(TicketSystem)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

set(SOURCES 
    control.cpp 
    init.cpp
    naive.cpp
    memory.cpp
)
add_executable(code ${SOURCES})
