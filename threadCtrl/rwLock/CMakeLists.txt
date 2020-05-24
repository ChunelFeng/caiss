cmake_minimum_required(VERSION 3.16)
project(RWLock)

set(CMAKE_CXX_STANDARD 14)


# 以下是测试需要的内容，需要把RWlock.h/cpp也给加进来
set(SOURCE_FILES RWLock.h RWLock.cpp RWLockDemo.cpp)
add_executable(RWLock ${SOURCE_FILES})