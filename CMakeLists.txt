cmake_minimum_required(VERSION 3.16)
project(ChunelAnn)

set(CMAKE_CXX_STANDARD 14)

aux_source_directory(. DIRSRCS)

# 以下是加入编译的时候，需要的宏
add_definitions(-D_CNL_CAISS_EXPORTS)

# 设置在windows上可以正常运行
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

# 设置boost路径
set (MY_BOOST_PATH C:/Users/Chunel/Documents/Chunel_File/Project_File/C++/boost_1_67_0/)

#添加头文件搜索路径
include_directories(${MY_BOOST_PATH})

#添加头文件搜索路径
include_directories(${MY_BOOST_PATH}/libs)

# 以下是编译dll对应的内容
add_library(ChunelAnn SHARED manageCtrl/manageInclude.h manageCtrl/syncManage/SyncManageProc.h
        manageCtrl/syncManage/SyncManageProc.cpp caissLib/CaissLib.cpp
        caissLib/CaissLib.h caissLib/CaissLibDefine.h algorithmCtrl/AlgorithmProc.h
        algorithmCtrl/AlgorithmInclude.h algorithmCtrl/hnsw/hnswProc/HnswProc.cpp
        algorithmCtrl/hnsw/hnswProc/HnswProc.h utilsCtrl/UtilsInclude.h utilsCtrl/UtilsProc.h
        utilsCtrl/rapidJsonUtils/rapidJsonProc/RapidJsonProc.cpp utilsCtrl/rapidJsonUtils/rapidJsonProc/RapidJsonProc.h
        threadCtrl/ThreadInclude.h threadCtrl/rwLock/RWLock.cpp threadCtrl/rwLock/RWLock.h threadCtrl/rwLock/RWLockDemo.cpp algorithmCtrl/hnsw/hnswAlgo/space_edition.h)

# 添加对应依赖的内容
add_subdirectory(threadCtrl/rwLock)
add_subdirectory(algorithmCtrl/hnsw/hnswProc)
add_subdirectory(manageCtrl/)
add_subdirectory(manageCtrl/syncManage)