cmake_minimum_required(VERSION 3.16)
project(CaissDemo)

set(CMAKE_CXX_STANDARD 14)


set(SOURCE_FILES ../caissLib/CaissLib.cpp CaissDemo.cpp
        ../threadCtrl/rwLock/RWLock.cpp
        ../manageCtrl/asyncManage/AsyncManageProc.cpp
        ../manageCtrl/syncManage/SyncManageProc.cpp
        ../algorithmCtrl/hnsw/hnswProc/HnswProc.cpp
        ../utilsCtrl/rapidJsonUtils/rapidJsonProc/RapidJsonProc.cpp
        ../threadCtrl/threadPool/ThreadPool.cpp
        caissSimpleDemo/CaissSimple.cpp
        ../manageCtrl/ManageProc.cpp
        caissMultiThreadDemo/CaissMutliThread.cpp
        ../utilsCtrl/trieProc/TrieProc.cpp
        ../utilsCtrl/memoryPool/MemoryPool.cpp)

add_executable(CaissDemo ${SOURCE_FILES})