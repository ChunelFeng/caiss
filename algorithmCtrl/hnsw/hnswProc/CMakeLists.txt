cmake_minimum_required(VERSION 3.16)
project(HnswProc)

set(CMAKE_CXX_STANDARD 14)


set(SOURCE_FILES ../../../threadCtrl/rwLock/RWLock.h
        ../../../threadCtrl/rwLock/RWLock.cpp
        ../../../utilsCtrl/rapidJsonUtils/rapidJsonProc/RapidJsonProc.h
        ../../../utilsCtrl/rapidJsonUtils/rapidJsonProc/RapidJsonProc.cpp
        HnswProc.h HnswProc.cpp HnswProcDemo.cpp)

add_executable(HnswProc ${SOURCE_FILES})