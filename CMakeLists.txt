cmake_minimum_required(VERSION 3.5)
project(SigmodBenchmarks)

set(CMAKE_CXX_STANDARD 11)

add_executable(SigmodBenchmarks main.cpp io-benchmarks/FileReader.h io-benchmarks/MMapReader.cpp io-benchmarks/MMapReader.h Timer.h io-benchmarks/ReadReader.cpp io-benchmarks/ReadReader.h io-benchmarks/StreamReader.cpp io-benchmarks/StreamReader.h partition/Partition.cpp partition/Partition.h)