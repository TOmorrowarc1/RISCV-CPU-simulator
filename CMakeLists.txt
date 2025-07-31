cmake_minimum_required(VERSION 3.10)
project(TicketSystem)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

set(DIRECTORIES
    src/header
)

set(SOURCES
    main.cpp
    src/program/CDBselector.cpp
    src/program/Decoder.cpp
    src/program/Executor.cpp
    src/program/memory.cpp
    src/program/PC.cpp
    src/program/Predictor.cpp
    src/program/RegFile.cpp
    src/program/ROB.cpp
    src/program/RS.cpp
    src/program/run.cpp
    src/program/utils.cpp
)

add_executable(code ${SOURCES})

target_include_directories(code PUBLIC ${DIRECTORIES})
