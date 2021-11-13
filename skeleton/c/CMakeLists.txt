cmake_minimum_required(VERSION 3.5)
project(spl C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)
add_compile_options(-Wall -Wextra)

include_directories(src)

FIND_PACKAGE(BISON REQUIRED)
FIND_PACKAGE(FLEX REQUIRED)

bison_target(PARSER
        ${CMAKE_CURRENT_SOURCE_DIR}/src/phases/_02_03_parser/parser.y
        ${CMAKE_CURRENT_SOURCE_DIR}/src/phases/_02_03_parser/parser.c
        VERBOSE
        )

flex_target(SCANNER
        ${CMAKE_CURRENT_SOURCE_DIR}/src/phases/_01_scanner/scanner.flex
        ${CMAKE_CURRENT_SOURCE_DIR}/src/phases/_01_scanner/scanner.c)

add_flex_bison_dependency(SCANNER PARSER)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(
        spl
        ${BISON_PARSER_OUTPUTS}
        ${FLEX_SCANNER_OUTPUTS}
        src/absyn/absyn.c
        src/phases/_04a_tablebuild/tablebuild.c
        src/phases/_04b_semant/procedurebodycheck.c
        src/phases/_05_varalloc/varalloc.c
        src/phases/_06_codegen/codegen.c
        src/main.c
        src/table/identifier.c
        src/table/table.c
        src/types/types.c
        src/util/errors.c
        src/util/errors.h
        src/util/memory.c
        src/util/memory.h
        src/phases/_06_codegen/codeprint.c
        src/phases/_06_codegen/codeprint.h)
