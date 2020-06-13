cmake_minimum_required(VERSION 3.16)
project(CaissDemo)

set(CMAKE_CXX_STANDARD 14)


set(SOURCE_FILES CaissLib.cpp CaissLibDemo.cpp
        ../threadCtrl/rwLock/RWLock.cpp
        ../manageCtrl/syncManage/SyncManageProc.cpp
        ../algorithmCtrl/hnsw/hnswProc/HnswProc.cpp
        ../utilsCtrl/rapidJsonUtils/rapidJsonProc/RapidJsonProc.cpp)

add_executable(CaissDemo ${SOURCE_FILES})