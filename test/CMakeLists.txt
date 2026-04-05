cmake_minimum_required(VERSION 3.16)
project(STC8MorseCodeTest LANGUAGES C CXX)

add_executable(${PROJECT_NAME}
    test.cpp
)

target_include_directories(${PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src
)
