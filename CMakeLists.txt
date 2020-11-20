#**************************************************************
# @author Chunel
# @Name CMakeList.txt
# @date 2020/11/15 1:25 上午
# @Desc Caiss对应CMakeList文件
#**************************************************************/

message("**************************************")
message("*    ______ ___     ____ _____ _____ *")
message("*   / ____//   |   /  _// ___// ___/ *")
message("*  / /    / /| |   / /  \\__ \\ \\__ \\  *")
message("* / /___ / ___ | _/ /  ___/ /___/ /  *")
message("* \\____//_/  |_|/___/ /____//____/   *")
message("*                                    *")
message("**************************************")


message("[caiss] welcome to the world of Caiss.")

cmake_minimum_required(VERSION 3.5.1)
project(Caiss)

set(CMAKE_CXX_STANDARD 14)

aux_source_directory(. DIRSRCS)

# 以下是加入编译的时候，需要的宏
add_definitions(-D_CAISS_EXPORTS_)

# 如果开启此宏定义，则Caiss执行过程中，不会在控制台打印任何信息
# add_definitions(-D_CAISS_SILENCE_)

find_package(OpenMP)
find_package(Eigen3)

# 设置当前的boost路径(MY_BOOST_PATH为必填)
IF (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")    # set on windows to make sure *.exe can work fine
    set(MY_BOOST_PATH C:/Users/Chunel/Documents/Chunel_File/Project_File/C++/boost_1_67_0/)
ELSEIF(APPLE)
    set(CMAKE_CXX_FLAGS -m64)    # 设置64位系统
    set(MY_BOOST_PATH /usr/local/Cellar/boost/1.72.0_3/include/)

    set(BLAS_INC_DIR /usr/local/opt/openblas/include/)
    set(BLAS_LINK_DIR /usr/local/opt/openblas/lib/)
    set(EIGEN_DIR /usr/local/Cellar/eigen/3.3.7/include/eigen3/)

    include_directories(${BLAS_INC_DIR} ${EIGEN_DIR})
    link_directories(${BLAS_LINK_DIR})

    #link_libraries(libopenblas.a)    # import openblas static lib

    if (EIGEN3_FOUND)
        add_definitions(-D_USE_EIGEN3_)
        message("[caiss] mac Eigen3 support.")
    else()
        message("[caiss] mac Eigen3 no support, please install for speed up.")
    endif()

    if (OPENMP_FOUND)
        link_directories("/usr/local/lib" "/usr/local/opt/llvm/lib")    # import info with openmp if find openmp
        include_directories("/usr/local/include" "/usr/local/opt/llvm/include")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xpreprocessor -fopenmp -lomp -mavx2")
        add_definitions(-D_USE_OPENMP_)
        message("[caiss] mac openmp support.")
    else()
        message("[caiss] mac openmp no support, please install for speed up.")
    endif()
ELSEIF(UNIX)
    set(CMAKE_CXX_FLAGS -m64)    # 设置64位系统
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2 -pthread")    # set multi thread in Linux platform
    set(MY_BOOST_PATH /usr/local/Cellar/boost/1.72.0_3/include/)

    if (EIGEN3_FOUND)
        add_definitions(-D_USE_EIGEN3_)
        message("[caiss] linux Eigen3 support.")
    else()
        message("[caiss] linux Eigen3 no support, please install for speed up.")
    endif()

    if (OPENMP_FOUND)
        link_directories("/usr/local/lib" "/usr/local/opt/llvm/lib")    # import info with openmp if find openmp
        include_directories("/usr/local/include" "/usr/local/opt/llvm/include")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -mavx2")
        add_definitions(-D_USE_OPENMP_)
        message("[caiss] linux openmp support.")
    else()
        message("[caiss] linux openmp no support, please install for speed up.")
    endif()
ENDIF()

#添加头文件搜索路径
include_directories(${MY_BOOST_PATH})

#添加头文件搜索路径
include_directories(${MY_BOOST_PATH}/libs)

aux_source_directory(sqlCtrl/sql SOURCE_LIST)

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
        utilsCtrl/memoryPool/MemoryPool.cpp
        sqlCtrl/sql/Expr.cpp
        sqlCtrl/sql/PrepareStatement.cpp
        sqlCtrl/sql/SQLStatement.cpp
        sqlCtrl/sql/statements.cpp
        sqlCtrl/SqlProc.cpp
        sqlCtrl/util/sqlhelper.cpp
        sqlCtrl/util/SQLParserResult.cpp
        sqlCtrl/util/SQLParser.cpp
        sqlCtrl/parser/bison_parser.cpp
        sqlCtrl/parser/flex_lexer.cpp
        )

# 添加对应依赖的内容
add_subdirectory(caissDemo)
