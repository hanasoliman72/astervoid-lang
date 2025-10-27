cmake_minimum_required(VERSION 3.29)
project(CompilerProject)

set(CMAKE_CXX_STANDARD 14)

add_executable(CompilerProject
    Scanner/main.cpp
        Scanner/Scanner.h)
