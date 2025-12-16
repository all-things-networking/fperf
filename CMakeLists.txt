cmake_minimum_required(VERSION 3.15)
project(FPerf CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g")

find_package(Z3 REQUIRED)
find_package(antlr4-runtime REQUIRED)

file(GLOB SRC_FILES src/*.cpp src/*/*.cpp)
file(GLOB HPP_FILES src/*.hpp src/*/*.hpp)


add_executable(search main.cpp ${SRC_FILES} ${HPP_FILES})
target_link_libraries(search PRIVATE z3::libz3 antlr4_static)
target_include_directories(search PRIVATE lib lib/metrics lib/qms lib/cps)

add_executable(checker checker.cpp ${SRC_FILES} ${HPP_FILES})
target_link_libraries(checker PRIVATE z3::libz3 antlr4_static)
target_include_directories(checker PRIVATE lib lib/metrics lib/qms lib/cps)
