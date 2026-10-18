cmake_minimum_required(VERSION 3.16)

project(Search LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(BstTree
    BstTree/BstTree.c
)

target_include_directories(BstTree PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/BstTree
)