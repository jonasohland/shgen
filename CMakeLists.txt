cmake_minimum_required(VERSION 3.5)

if(NOT shgen_version)
    execute_process(COMMAND git describe --tags OUTPUT_VARIABLE shgen_version OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REPLACE "-" ";" shgen_ver_list ${shgen_version})
    list(GET shgen_ver_list 0 shgen_proj_ver)
else()
    set(shgen_proj_ver ${shgen_version})
endif()

message(STATUS "shgen version: ${shgen_proj_ver}")

project(shgen VERSION ${shgen_proj_ver})

find_package(benchmark)

add_executable(shgen
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cc
    ${CMAKE_CURRENT_LIST_DIR}/src/shgen.cc
    ${CMAKE_CURRENT_LIST_DIR}/src/extras.cc
    ${CMAKE_CURRENT_LIST_DIR}/src/shgen.h
    ${CMAKE_CURRENT_LIST_DIR}/src/extras.h)

target_compile_definitions(shgen PRIVATE "SHGEN_VERSION=${shgen_version}")

if(NOT MSVC)
    target_compile_options(shgen PUBLIC -Wall -Wextra -pedantic -Werror)
else()
    target_compile_definitions(shgen PUBLIC _CRT_SECURE_NO_WARNINGS)
endif()

if(benchmark_FOUND)
    add_executable(shgen_benchmark
        ${CMAKE_CURRENT_LIST_DIR}/benchmark/benchmark.cc)

    target_link_libraries(shgen_benchmark PUBLIC benchmark::benchmark)
    target_compile_features(shgen_benchmark PUBLIC cxx_std_17)
endif()


target_include_directories(shgen PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dep/Clara/include")
target_compile_features(shgen PUBLIC cxx_std_17)

install(TARGETS shgen DESTINATION bin)
