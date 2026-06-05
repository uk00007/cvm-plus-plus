cmake_minimum_required(VERSION 3.10)

project(CVM_Plus_Plus)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Tell the compiler where to find all our .h files
include_directories(include)

# Tell CMake where the .cpp files live now
add_executable(cvm 
    src/main.cpp 
    src/lexer.cpp 
    src/parser.cpp 
    src/compiler.cpp 
    src/vm.cpp
)