cmake_minimum_required(VERSION 3.5)

project(shgen)

add_executable(shgen
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cc
    ${CMAKE_CURRENT_LIST_DIR}/src/shgen.cc
    ${CMAKE_CURRENT_LIST_DIR}/src/shgen.h)

target_include_directories(shgen PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dep/Clara/include")
target_compile_features(shgen PUBLIC cxx_std_14)
