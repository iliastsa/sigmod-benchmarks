cmake_minimum_required(VERSION 3.5)
project(SigmodBenchmarks)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-O3")

include_directories(include/partition include/readers include/writers include/tasks include/benchmarks include/utilities include/structures)

add_executable( SigmodBenchmarks include/tasks/RandomReadSortedTask.h include/writers/BufferringWriter.h
                src/main.cpp src/readers/MMapReader.cpp src/readers/ReadReader.cpp src/partition/Partition.cpp src/benchmarks/ParallelReadHistoBenchmark.cpp src/utilities/Utils.cpp src/utilities/ThreadPool.cpp src/readers/ASyncReader.cpp src/benchmarks/SequentialReadHistoBenchmark.cpp src/writers/FileWriter.cpp src/benchmarks/SingleMMapHistoBenchmark.cpp src/benchmarks/WriteBenchmark.cpp src/benchmarks/ColumnStoreBenchmark.cpp src/writers/BufferringWriter.cpp include/benchmarks/ColumnStoreBWBenchmark.cpp include/benchmarks/ColumnStoreBWBenchmark.h)

target_compile_definitions(SigmodBenchmarks PRIVATE DEBUG)
target_link_libraries(SigmodBenchmarks -lpthread -lrt)