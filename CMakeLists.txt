cmake_minimum_required(VERSION 3.17)
project(pascal_compiler_tutorial)

set(CMAKE_CXX_STANDARD 17)

include(DownloadProject.cmake)

download_project(PROJ expected
        GIT_REPOSITORY https://github.com/TartanLlama/expected.git
        GIT_TAG        v1.0.0
        )
add_subdirectory(${expected_SOURCE_DIR} ${expected_BINARY_DIR})

download_project(PROJ fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG        7.1.3
        )
add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR})

add_executable(pascal_compiler_tutorial main.cpp lexer.h lexer.cpp token.h result.h optional_formatter.h parser.h parser.cpp ast.h stringstream_formatter.h ast_dot_visualiser.h ast_visitor.h)
target_link_libraries(pascal_compiler_tutorial expected fmt)