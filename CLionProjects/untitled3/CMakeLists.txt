cmake_minimum_required(VERSION 4.0)
project(untitled3)

set(CMAKE_CXX_STANDARD 20)

add_executable(untitled3 main.cpp
        astervoid-lang/implementation/parser/parser.h
        astervoid-lang/implementation/parser/parser.cpp
        astervoid-lang/implementation/scanner/scanner.cpp
        astervoid-lang/implementation/parseTree/genericParseTreeNode.cpp
        astervoid-lang/implementation/semantic/symbolTable.cpp
        astervoid-lang/implementation/scanner/TokenType.h
        astervoid-lang/implementation/semantic/SemanticAnalyzer.h
        astervoid-lang/implementation/semantic/SemanticAnalyzer.cpp

)

