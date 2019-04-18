cmake_minimum_required(VERSION 3.5)
project(SigmodBenchmarks)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Ofast -funroll-loops -Wall")
#set(CMAKE_CXX_FLAGS  "-fsanitize=address")

include_directories(include/libs
                    include/partition
                    include/readers
                    include/writers
                    include/tasks
                    include/benchmarks
                    include/utilities
                    include/structures)

set(SOURCES
    src/main.cpp
    src/readers/MMapReader.cpp
    src/readers/ReadReader.cpp
    src/partition/Partition.cpp
    src/benchmarks/ParallelReadHistoBenchmark.cpp
    src/utilities/Utils.cpp
    src/utilities/ThreadPool.cpp
    src/readers/ASyncReader.cpp
    src/benchmarks/SequentialReadHistoBenchmark.cpp
    src/writers/FileWriter.cpp
    src/benchmarks/SingleMMapHistoBenchmark.cpp
    src/benchmarks/IOBenchmark.cpp
    src/benchmarks/ColumnStoreBenchmark.cpp
    src/writers/BufferringWriter.cpp
    src/benchmarks/ColumnStoreBWBenchmark.cpp
    src/benchmarks/InMemoryBenchmark.cpp
    src/benchmarks/FileInMemColumnStoreBenchmark.cpp
    src/utilities/AsyncUtils.cpp
    src/utilities/Constants.cpp
    src/benchmarks/MergeBenchmark.cpp
    src/readers/HybridReadReader.cpp)

add_executable(SigmodBenchmarks ${SOURCES})
add_executable(local_sort ${SOURCES})

target_compile_definitions(SigmodBenchmarks PRIVATE DEBUG SUBMISSION)
target_compile_definitions(local_sort PRIVATE DEBUG)

target_link_libraries(SigmodBenchmarks -lpthread -lrt)
target_link_libraries(local_sort -lpthread -lrt)