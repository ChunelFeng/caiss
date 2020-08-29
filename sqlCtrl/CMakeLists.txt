cmake_minimum_required(VERSION 3.5.1)
project(SqlCtrlDemo)

set(CMAKE_CXX_STANDARD 14)


aux_source_directory(sql SOURCE_LIST)
aux_source_directory(parser SOURCE_LIST)
aux_source_directory(util SOURCE_LIST)
aux_source_directory(../sqlCtrl SOURCE_LIST)

set (SOURCE_FILES
        SqlProc.cpp
        SqlCtrlDemo.cpp)


add_executable(SqlCtrlDemo ${SOURCE_LIST} ${SOURCE_FILES})