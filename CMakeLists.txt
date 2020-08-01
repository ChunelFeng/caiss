cmake_minimum_required(VERSION 3.5.1)
project(Caiss)

set(CMAKE_CXX_STANDARD 14)

aux_source_directory(. DIRSRCS)

# 以下是加入编译的时候，需要的宏
add_definitions(-D_CAISS_EXPORTS_)

find_package(OpenMP)
find_package(Eigen3 REQUIRED)

if (EIGEN3_FOUND)
    add_definitions(-D_USE_EIGEN3_)
    message("eigen support")
endif()


# 设置当前的boost路径(MY_BOOST_PATH为必填)
IF (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")    # set on windows to make sure *.exe can work fine
    set(MY_BOOST_PATH C:/Users/Chunel/Documents/Chunel_File/Project_File/C++/boost_1_67_0/)
ELSEIF(APPLE)
    set(MY_BOOST_PATH /usr/local/Cellar/boost/1.72.0_3/include/)

    set(BLAS_INC_DIR /usr/local/opt/openblas/include/)
    set(BLAS_LINK_DIR /usr/local/opt/openblas/lib/)
    set(EIGEN_DIR /usr/local/Cellar/eigen/3.3.7/include/eigen3/)

    include_directories(${BLAS_INC_DIR} ${EIGEN_DIR})
    link_directories(${BLAS_LINK_DIR})

    link_libraries(libopenblas.a)    # import openblas static lib

    if (OPENMP_FOUND)
        message("openmp support")
        link_directories("/usr/local/lib" "/usr/local/opt/llvm/lib")    # import info with openmp if find openmp
        include_directories("/usr/local/include" "/usr/local/opt/llvm/include")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xpreprocessor -fopenmp -lomp -mavx2")
        add_definitions(-D_USE_OPENMP_)
    else()
        message("no openmp support")
    endif()
ELSEIF(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2 -pthread")    # set multi thread in Linux platform
    set(MY_BOOST_PATH /usr/local/Cellar/boost/1.72.0_3/include/)
ENDIF()

#添加头文件搜索路径
include_directories(${MY_BOOST_PATH})

#添加头文件搜索路径
include_directories(${MY_BOOST_PATH}/libs)

# 以下是编译动态库文件，对应的内容
add_library(Caiss SHARED
        manageCtrl/syncManage/SyncManageProc.cpp
        threadCtrl/threadPool/ThreadPool.cpp
        manageCtrl/asyncManage/AsyncManageProc.cpp
        caissLib/CaissLib.cpp
        algorithmCtrl/hnsw/hnswProc/HnswProc.cpp
        utilsCtrl/rapidJsonUtils/rapidJsonProc/RapidJsonProc.cpp
        threadCtrl/rwLock/RWLock.cpp
        manageCtrl/ManageProc.cpp
        utilsCtrl/trieProc/TrieProc.cpp
        utilsCtrl/memoryPool/MemoryPool.cpp)

# 添加对应依赖的内容
add_subdirectory(caissDemo)
add_subdirectory(utilsCtrl)
