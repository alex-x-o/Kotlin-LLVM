cmake_minimum_required(VERSION 3.13)
project(kotlin_llvm)

set(CMAKE_CXX_STANDARD 14)

find_package(BISON)
find_package(FLEX)
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader)

bison_target(MyParser src/parser.ypp ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cpp)
flex_target(MyLexer src/lexer.lex  ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
add_flex_bison_dependency(MyLexer MyParser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(kotlin-llvm
        ${BISON_MyParser_OUTPUTS}
        ${FLEX_MyLexer_OUTPUTS}
        src/ast.cpp
        src/ast.hpp)

# Link against LLVM libraries
target_link_libraries(kotlin-llvm ${llvm_libs})
