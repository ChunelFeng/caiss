cmake_minimum_required(VERSION 3.16)
project(Caiss)

set(CMAKE_CXX_STANDARD 14)

aux_source_directory(. DIRSRCS)

# 以下是加入编译的时候，需要的宏
add_definitions(-D_CAISS_EXPORTS_)

# 设置在windows上可以正常运行
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

# 设置当前的boost路径(必填)
set (MY_BOOST_PATH C:/Users/Chunel/Documents/Chunel_File/Project_File/C++/boost_1_67_0/)

#添加头文件搜索路径
include_directories(${MY_BOOST_PATH})

#添加头文件搜索路径
include_directories(${MY_BOOST_PATH}/libs)

# 以下是编译dll对应的内容
add_library(Caiss SHARED manageCtrl/syncManage/SyncManageProc.cpp
        manageCtrl/asyncManage/AsyncManageProc.cpp
        caissLib/CaissLib.cpp
        algorithmCtrl/hnsw/hnswProc/HnswProc.cpp
        utilsCtrl/rapidJsonUtils/rapidJsonProc/RapidJsonProc.cpp
        threadCtrl/rwLock/RWLock.cpp
        threadCtrl/threadPool/ThreadPool.cpp
        manageCtrl/ManageProc.cpp
        caissDemo/caissSimpleDemo/CaissSimple.cpp
        caissDemo/caissMultiThreadDemo/CaissMutliThread.cpp
        caissDemo/CaissDemo.cpp)

# 添加对应依赖的内容
add_subdirectory(threadCtrl/rwLock)
add_subdirectory(threadCtrl/threadPool)
add_subdirectory(algorithmCtrl/hnsw/hnswProc)
add_subdirectory(caissDemo)