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
# 设置当前eigen路径（MY_EIGEN_PATH为选填）
IF (WIN32)
    if (MINGW)
        # 试了下MinGW编译的程序在Windows上运行，会出现一些问题，所以推荐使用MSVC编译
        set(CMAKE_CXX_FLAGS -m64) # 设置64位系统
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-result -Wno-deprecated-declarations -Wno-format-overflow")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O2 -pthread")    # set multi thread
    else ()
        # 1. Generate: > cmake -G "Visual Studio 17 2022" -S . -B cmake-build-release-vs64
        # 2. Build:    > cmake --build cmake-build-release-vs64 --config [Debug|Release|RelWithDebInfo|MinSizeRel]
        # MSVC 编译32位程序需要在 Generate 命令中加入 -A Win32，这样 cmake 才会生成32位的工程
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++14 /O2 /EHsc /MP")    # set multi thread
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4819")    # Suppresses the compiler warning C4819: 该文件包含不能在当前代码页(936)中表示的字符
    endif ()

    if(DEFINED ENV{MY_BOOST_PATH})
        set(MY_BOOST_PATH $ENV{MY_BOOST_PATH})
    else()
        set(MY_BOOST_PATH E:/04Code/Github/boost_1_80_0/)
    endif()
    if(DEFINED ENV{MY_EIGEN_PATH})
        set(MY_EIGEN_PATH $ENV{MY_EIGEN_PATH})
    else()
        set(MY_EIGEN_PATH E:/04Code/Github/eigen-3.4.0/)
    endif()
    message("[caiss] MY_BOOST_PATH: ${MY_BOOST_PATH}")
    message("[caiss] MY_EIGEN_PATH: ${MY_EIGEN_PATH}")
    include_directories(${MY_EIGEN_PATH})
    add_definitions(-D_USE_EIGEN3_)
    include_directories(./win32patch)
    if (OPENMP_FOUND)
        if (MINGW)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -mavx2")
        else ()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /openmp /arch:AVX2")
        endif ()
        add_definitions(-D_USE_OPENMP_)
        message("[caiss] windows openmp support.")
    else()
        message("[caiss] windows openmp no support, please install for speed up.")
    endif()
ELSEIF(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -Wno-unused-command-line-argument")    # 设置64位系统

    # set(BLAS_INC_DIR /usr/local/opt/openblas/include/)
    # set(BLAS_LINK_DIR /usr/local/opt/openblas/lib/)
    #link_libraries(libopenblas.a)    # import openblas static lib
    set(MY_BOOST_PATH /opt/homebrew/Cellar/boost/1.76.0/include/)
    set(MY_EIGEN_PATH /opt/homebrew/Cellar/eigen/3.4.0/include/eigen3/)

    include_directories(${BLAS_INC_DIR} ${MY_EIGEN_PATH})
    link_directories(${BLAS_LINK_DIR})

    if (EIGEN3_FOUND)
        add_definitions(-D_USE_EIGEN3_)
        message("[caiss] mac Eigen3 support.")
    else()
        message("[caiss] mac Eigen3 no support, please install for speed up.")
    endif()

    if (OPENMP_FOUND)
        link_directories("/usr/local/lib" "/opt/homebrew/Cellar/llvm/12.0.1/lib/")    # import info with openmp if find openmp
        include_directories("/usr/local/include"
                "/opt/homebrew/Cellar/llvm/12.0.1/include/"
                "/opt/homebrew/Cellar/libomp/12.0.1/include/")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xpreprocessor -fopenmp -lomp -mavx2")
        add_definitions(-D_USE_OPENMP_)
        message("[caiss] mac openmp support.")
    else()
        message("[caiss] mac openmp no support, please install for speed up.")
    endif()
ELSEIF(UNIX)
    set(CMAKE_CXX_FLAGS -m64)    # 设置64位系统
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O2 -pthread")    # set multi thread in Linux platform
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-result -Wno-deprecated-declarations -Wno-format-overflow")
    set(MY_BOOST_PATH /usr/local/Cellar/boost/1.72.0_3/include/)

    if (EIGEN3_FOUND)
        add_definitions(-D_USE_EIGEN3_)
        message("[caiss] linux Eigen3 support.")
    else()
        message("[caiss] linux Eigen3 no support, please install for speed up.")
    endif()

    if (OPENMP_FOUND)
        link_directories("/usr/local/lib" "/usr/local/opt/llvm/lib")    # import info with openmp if find openmp
        include_directories("/usr/local/include/" "/usr/local/opt/llvm/include/")
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
        algorithmCtrl/AlgorithmProc.cpp
        algorithmCtrl/hnsw/hnswProc/HnswProc.cpp
        algorithmCtrl/mrpt/mrptProc/MrptProc.cpp
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
