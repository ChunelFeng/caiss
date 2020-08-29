cmake_minimum_required(VERSION 3.5.1)
project(SqlCtrlDemo)

set(CMAKE_CXX_STANDARD 14)


#set(SOURCE_FILES
#        UtilsCtrlDemo.cpp
#        ../threadCtrl/rwLock/RWLock.cpp
#        ../utilsCtrl/rapidJsonUtils/rapidJsonProc/RapidJsonProc.cpp
#        ../utilsCtrl/trieProc/TrieProc.cpp
#        memoryPool/MemoryPool.cpp)

aux_source_directory(sql SOURCE_LIST)
aux_source_directory(parser SOURCE_LIST)
aux_source_directory(util SOURCE_LIST)
aux_source_directory(../sqlCtrl SOURCE_LIST)

add_executable(SqlCtrlDemo ${SOURCE_LIST})