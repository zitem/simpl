cmake_minimum_required (VERSION 3.20)

set(project "simpl")
set(target_compiler "simplc")
set(CMAKE_CXX_STANDARD 23)

project(${project} CXX)

add_executable(${target_compiler} 
    builtinModules.cpp
    builtinModules.h
    cfg.cpp
    cfg.h
    lexer.cpp
    lexer.h
    main.cpp
    node.cpp
    node.h
    outs.cpp
    outs.h
    parser.cpp
    parser.h
    set.h
    utils.h
)

target_precompile_headers(${target_compiler} PRIVATE pch.h)