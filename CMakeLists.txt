cmake_minimum_required(VERSION 3.5)
project(SigmodBenchmarks)

set(CMAKE_CXX_STANDARD 11)

include_directories(include/partition include/utilities include/readers include/parallel include/benchmarks include/utilities)

add_executable(SigmodBenchmarks main.cpp include/utilities/thread_pool.h parallel/HistogramTask.cpp include/parallel/HistogramTask.h include/readers/FileReader.h readers/MMapReader.cpp include/readers/MMapReader.h include/utilities/Timer.h readers/ReadReader.cpp include/readers/ReadReader.h readers/StreamReader.cpp include/readers/StreamReader.h partition/Partition.cpp include/partition/Partition.h include/benchmarks/Benchmark.h benchmarks/ParallelReadHistoBenchmark.cpp include/benchmarks/ParallelReadHistoBenchmark.h include/utilities/Utils.h utilities/Utils.cpp)
target_link_libraries(SigmodBenchmarks -lpthread)