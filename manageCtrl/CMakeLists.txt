cmake_minimum_required(VERSION 3.16)
project(manageProc)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES ManageDemo.cpp ../utilsCtrl/rapidJsonUtils/rapidJsonProc/RapidJsonProc.h
        ../utilsCtrl/rapidJsonUtils/rapidJsonProc/RapidJsonProc.cpp ../algorithmCtrl/AlgorithmProc.h
        ../algorithmCtrl/hnsw/hnswProc/hnswProc.h ../algorithmCtrl/hnsw/hnswProc/hnswProc.cpp
        manageProc.h syncManage/SyncManageProc.h syncManage/SyncManageProc.cpp
        ../threadCtrl/rwLock/RWLock.cpp ../threadCtrl/rwLock/RWLock.h)
add_executable(manageProc ${SOURCE_FILES})