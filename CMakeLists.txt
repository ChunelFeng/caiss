cmake_minimum_required(VERSION 3.5.1)
project(Caiss)

set(CMAKE_CXX_STANDARD 14)

aux_source_directory(. DIRSRCS)

# 以下是加入编译的时候，需要的宏
add_definitions(-D_CAISS_EXPORTS_)

# 设置当前的boost路径(必填)
IF (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")    # set on windows to make sure .exe can work fine
    set(MY_BOOST_PATH C:/Users/Chunel/Documents/Chunel_File/Project_File/C++/boost_1_67_0/)
ELSEIF(APPLE)
    set(MY_BOOST_PATH /usr/local/Cellar/boost/1.72.0_3/include/)
ELSEIF(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2 -pthread")    # set multi thread in Linux platform
    set(MY_BOOST_PATH /usr/local/Cellar/boost/1.72.0_3/include/)
ENDIF()

#添加头文件搜索路径
include_directories(${MY_BOOST_PATH})

#添加头文件搜索路径
include_directories(${MY_BOOST_PATH}/libs)

# 以下是编译dll对应的内容
add_library(Caiss SHARED manageCtrl/syncManage/SyncManageProc.cpp
        threadCtrl/threadPool/ThreadPool.cpp
        manageCtrl/asyncManage/AsyncManageProc.cpp
        caissLib/CaissLib.cpp
        algorithmCtrl/hnsw/hnswProc/HnswProc.cpp
        utilsCtrl/rapidJsonUtils/rapidJsonProc/RapidJsonProc.cpp
        threadCtrl/rwLock/RWLock.cpp
        manageCtrl/ManageProc.cpp
        caissDemo/caissSimpleDemo/CaissSimple.cpp
        caissDemo/caissMultiThreadDemo/CaissMutliThread.cpp
        caissDemo/CaissDemo.cpp)

# 添加对应依赖的内容
add_subdirectory(threadCtrl/rwLock)
add_subdirectory(threadCtrl/threadPool)
add_subdirectory(caissDemo)
