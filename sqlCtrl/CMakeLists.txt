cmake_minimum_required(VERSION 3.5.1)
project(SqlCtrlDemo)

set(CMAKE_CXX_STANDARD 14)


set (SOURCE_FILES
        sql/Expr.cpp
        sql/PrepareStatement.cpp
        sql/SQLStatement.cpp
        sql/statements.cpp
        SqlProc.cpp
        util/sqlhelper.cpp
        util/SQLParserResult.cpp
        util/SQLParser.cpp
        parser/bison_parser.cpp
        parser/flex_lexer.cpp
        SqlCtrlDemo.cpp)

add_executable(SqlCtrlDemo ${SOURCE_FILES})