cmake_minimum_required(VERSION 3.6)
project(concurrent_huffman LANGUAGES CXX)

include(FetchContent)
find_package(Threads REQUIRED)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})
set(HUFFMAN_INSTALL_LIB_DIR ${PROJECT_SOURCE_DIR}/lib)
set(HUFFMAN_INSTALL_BIN_DIR ${CMAKE_INSTALL_BINDIR})

# ------------------------------------------------------------------------------
# Includes
# ------------------------------------------------------------------------------
include_directories(include/concurrent)
include_directories(include)
# ------------------------------------------------------------------------------
# Google Test
# ------------------------------------------------------------------------------
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ------------------------------------------------------------------------------
# Google Benchmark
# ------------------------------------------------------------------------------
FetchContent_Declare(
        googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG master
)
set(BENCHMARK_ENABLE_TESTING off)
FetchContent_MakeAvailable(googlebenchmark)

# ------------------------------------------------------------------------------
# Clang Tidy
# ------------------------------------------------------------------------------
if(ENABLE_CLANG_TIDY)

    find_program(CLANG_TIDY_BIN clang-tidy-4.0)
    find_program(RUN_CLANG_TIDY_BIN run-clang-tidy-4.0.py)

    if(CLANG_TIDY_BIN STREQUAL "CLANG_TIDY_BIN-NOTFOUND")
        message(FATAL_ERROR "unable to locate clang-tidy-4.0")
    endif()

    if(RUN_CLANG_TIDY_BIN STREQUAL "RUN_CLANG_TIDY_BIN-NOTFOUND")
        message(FATAL_ERROR "unable to locate run-clang-tidy-4.0.py")
    endif()

    list(APPEND RUN_CLANG_TIDY_BIN_ARGS
            -clang-tidy-binary ${CLANG_TIDY_BIN}
            -header-filter=.*
            -checks=clan*,cert*,misc*,perf*,cppc*,read*,mode*,-cert-err58-cpp,-misc-noexcept-move-constructor
            )

    add_custom_target(
            tidy
            COMMAND ${RUN_CLANG_TIDY_BIN} ${RUN_CLANG_TIDY_BIN_ARGS}
            COMMENT "running clang tidy"
   )

endif()

# ------------------------------------------------------------------------------
# Coverage
# ------------------------------------------------------------------------------
if(ENABLE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

# ------------------------------------------------------------------------------
# Google Sanitizers
# ------------------------------------------------------------------------------
if(ENABLE_ASAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=gold")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak")
endif()

if(ENABLE_USAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=gold")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
endif()

if(ENABLE_TSAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=gold")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
endif()

# ------------------------------------------------------------------------------
# Valgrind
# ------------------------------------------------------------------------------
set(MEMORYCHECK_COMMAND_OPTIONS "${MEMORYCHECK_COMMAND_OPTIONS} --leak-check=full")
set(MEMORYCHECK_COMMAND_OPTIONS "${MEMORYCHECK_COMMAND_OPTIONS} --track-fds=yes")
set(MEMORYCHECK_COMMAND_OPTIONS "${MEMORYCHECK_COMMAND_OPTIONS} --trace-children=yes")
set(MEMORYCHECK_COMMAND_OPTIONS "${MEMORYCHECK_COMMAND_OPTIONS} --error-exitcode=1")

# ------------------------------------------------------------------------------
# Concurrent Huffman Library
# ------------------------------------------------------------------------------
file(GLOB_RECURSE LIB_HEADER_FILES "include/*.h")
file(GLOB_RECURSE LIB_SOURCE_FILES "src/*.cpp")
add_library(concurrent_huffman_lib ${LIB_HEADER_FILES} ${LIB_SOURCE_FILES})
set_target_properties(concurrent_huffman_lib PROPERTIES LINKER_LANGUAGE CXX)

# ------------------------------------------------------------------------------
# Concurrent Huffman Example
# ------------------------------------------------------------------------------
#add_executable(concurrent_huffman_example example/main.cpp)
#target_link_libraries(concurrent_huffman_example concurrent_huffman_lib)
#install(TARGETS concurrent_huffman_example DESTINATION ${HUFFMAN_INSTALL_BIN_DIR}/example)

# ------------------------------------------------------------------------------
# Concurrent Huffman Benchmark
# ------------------------------------------------------------------------------
#add_executable(concurrent_huffman_benchmark benchmark/bench_huffman.cpp)
#target_link_libraries(concurrent_huffman_benchmark benchmark::benchmark concurrent_huffman)
#install(TARGETS concurrent_huffman_benchmark DESTINATION ${HUFFMAN_INSTALL_BIN_DIR}/benchmarks)

# ------------------------------------------------------------------------------
# Concurrent Huffman Tests
# ------------------------------------------------------------------------------
#include(CTest)
#include(GoogleTest)
#file(GLOB_RECURSE TEST_SOURCE_FILES "test/*.cpp")
#add_executable(concurrent_huffman_tests ${TEST_SOURCE_FILES})
#target_link_libraries(concurrent_huffman_tests concurrent_huffman)
#target_link_libraries(concurrent_huffman_tests gtest_main)
#gtest_discover_tests(concurrent_huffman_tests)
#install(TARGETS concurrent_huffman_tests DESTINATION ${HUFFMAN_INSTALL_BIN_DIR}/tests)

