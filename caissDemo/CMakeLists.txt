cmake_minimum_required(VERSION 3.5.1)
project(CaissDemo)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        ../caissLib/CaissLib.cpp
        CaissDemo.cpp
        caissSqlDemo/CaissSql.cpp
        caissSimpleDemo/CaissSimple.cpp
        caissMultiThreadDemo/CaissMutliThread.cpp
        ../threadCtrl/rwLock/RWLock.cpp
        ../manageCtrl/asyncManage/AsyncManageProc.cpp
        ../manageCtrl/syncManage/SyncManageProc.cpp
        ../algorithmCtrl/AlgorithmProc.cpp
        ../algorithmCtrl/hnsw/hnswProc/HnswProc.cpp
        ../algorithmCtrl/mrpt/mrptProc/MrptProc.cpp
        ../utilsCtrl/rapidJsonUtils/rapidJsonProc/RapidJsonProc.cpp
        ../threadCtrl/threadPool/ThreadPool.cpp
        ../manageCtrl/ManageProc.cpp
        ../utilsCtrl/trieProc/TrieProc.cpp
        ../utilsCtrl/memoryPool/MemoryPool.cpp
        ../sqlCtrl/sql/Expr.cpp
        ../sqlCtrl/sql/PrepareStatement.cpp
        ../sqlCtrl/sql/SQLStatement.cpp
        ../sqlCtrl/sql/statements.cpp
        ../sqlCtrl/SqlProc.cpp
        ../sqlCtrl/util/sqlhelper.cpp
        ../sqlCtrl/util/SQLParserResult.cpp
        ../sqlCtrl/util/SQLParser.cpp
        ../sqlCtrl/parser/bison_parser.cpp
        ../sqlCtrl/parser/flex_lexer.cpp)

add_executable(CaissDemo ${SOURCE_FILES})
