cmake_minimum_required(VERSION 3.5.1)
project(UtilsCtrlDemo)

set(CMAKE_CXX_STANDARD 14)


set(SOURCE_FILES
        UtilsCtrlDemo.cpp
        ../threadCtrl/rwLock/RWLock.cpp
        ../utilsCtrl/rapidJsonUtils/rapidJsonProc/RapidJsonProc.cpp
        ../utilsCtrl/trieProc/TrieProc.cpp
        memoryPool/MemoryPool.cpp)

add_executable(UtilsCtrlDemo ${SOURCE_FILES})