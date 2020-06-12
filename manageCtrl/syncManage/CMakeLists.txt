cmake_minimum_required(VERSION 3.16)
project(SyncManageProc)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES ../manageProc.h
        ../../utilsCtrl/rapidJsonUtils/rapidJsonProc/RapidJsonProc.h ../../utilsCtrl/rapidJsonUtils/rapidJsonProc/RapidJsonProc.cpp
        ../../algorithmCtrl/hnsw/hnswAlgo/hnswalg.h
        ../../algorithmCtrl/AlgorithmProc.h
        ../../algorithmCtrl/hnsw/hnswProc/HnswProc.h ../../algorithmCtrl/hnsw/hnswProc/HnswProc.cpp
        ../../threadCtrl/rwLock/RWLock.h ../../threadCtrl/rwLock/RWLock.cpp
        ./SyncManageProc.h ./SyncManageProc.cpp ./SyncManageProcDemo.cpp)

add_executable(SyncManageProc ${SOURCE_FILES})

