cmake_minimum_required(VERSION 3.5)

project(shgen)

find_package(benchmark REQUIRED)

add_executable(shgen
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cc
    ${CMAKE_CURRENT_LIST_DIR}/src/shgen.cc
    ${CMAKE_CURRENT_LIST_DIR}/src/shgen.h)

add_executable(shgen_benchmark
    ${CMAKE_CURRENT_LIST_DIR}/benchmark/benchmark.cc)

target_link_libraries(shgen_benchmark PUBLIC benchmark::benchmark)

target_include_directories(shgen PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dep/Clara/include")
target_compile_features(shgen PUBLIC cxx_std_17)
target_compile_features(shgen_benchmark PUBLIC cxx_std_17)
