cmake_minimum_required(VERSION 3.20)
project(qrk)

add_executable(qrk
    src/main.c
    src/lexer.c
    src/parser.c
    src/ast.c
)
