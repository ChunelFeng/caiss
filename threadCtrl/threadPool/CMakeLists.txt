cmake_minimum_required(VERSION 3.1)
project(ThreadPool)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES ThreadPool.cpp ThreadPoolDemo.cpp ThreadPoolDefine.h)
add_executable(ThreadPool ${SOURCE_FILES})
