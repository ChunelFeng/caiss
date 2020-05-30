cmake_minimum_required(VERSION 3.16)
project(ChunelAnn)

set(CMAKE_CXX_STANDARD 14)

# 以下是加入编译的时候，需要的宏
add_definitions(-D_CNL_ANN_EXPORTS)

# 以下是编译dll对应的内容
add_library(ChunelAnn SHARED annManageCtrl/AnnManageInclude.h annManageCtrl/syncManage/SyncManageProc.h
        annManageCtrl/syncManage/SyncManageProc.cpp chunelAnnLib/ChunelAnnLib.cpp
        chunelAnnLib/ChunelAnnLib.h chunelAnnLib/ChunelAnnLibDefine.h algorithmCtrl/AlgorithmProc.h
        algorithmCtrl/AlgorithmInclude.h algorithmCtrl/hnsw/hnswProc/HnswProc.cpp
        algorithmCtrl/hnsw/hnswProc/HnswProc.h utilsCtrl/UtilsInclude.h utilsCtrl/UtilsProc.h
        utilsCtrl/rapidJsonUtils/rapidJsonProc/RapidJsonProc.cpp utilsCtrl/rapidJsonUtils/rapidJsonProc/RapidJsonProc.h
        threadCtrl/ThreadInclude.h threadCtrl/rwLock/RWLock.cpp threadCtrl/rwLock/RWLock.h threadCtrl/rwLock/RWLockDemo.cpp)

# 添加对应依赖的内容
add_subdirectory(threadCtrl/rwLock)
add_subdirectory(algorithmCtrl/hnsw/hnswProc)
add_subdirectory(annManageCtrl/)
add_subdirectory(annManageCtrl/syncManage)