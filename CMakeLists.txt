cmake_minimum_required(VERSION 3.5)
project(SigmodBenchmarks)

set(CMAKE_CXX_STANDARD 11)

include_directories(include/partition include/utilities include/readers include/parallel include/benchmarks include/utilities)

add_executable(SigmodBenchmarks src/main.cpp include/utilities/ThreadPool.h src/parallel/HistogramTask.cpp include/parallel/HistogramTask.h include/readers/FileReader.h src/readers/MMapReader.cpp include/readers/MMapReader.h include/utilities/Timer.h src/readers/ReadReader.cpp include/readers/ReadReader.h src/readers/StreamReader.cpp include/readers/StreamReader.h src/partition/Partition.cpp include/partition/Partition.h include/benchmarks/Benchmark.h src/benchmarks/ParallelReadHistoBenchmark.cpp include/benchmarks/ParallelReadHistoBenchmark.h include/utilities/Utils.h src/utilities/Utils.cpp src/utilities/ThreadPool.cpp include/utilities/ThreadPool.h)
target_link_libraries(SigmodBenchmarks -lpthread)