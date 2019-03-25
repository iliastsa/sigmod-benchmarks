#include <iostream>
#include <cstdlib>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <Benchmark.h>
#include <ParallelReadHistoBenchmark.h>
#include <aio.h>
#include <SequentialReadHistoBenchmark.h>

#include "FileReader.h"
#include "MMapReader.h"
#include "Timer.h"
#include "ReadReader.h"
#include "Partition.h"
#include "Utils.h"

using namespace std;

int main(int argc, char **argv) {

    Benchmark *sequential_bench = new SequentialReadHistoBenchmark(argv[1]);
    sequential_bench->run();

    Benchmark *parallel_bench = new ParallelReadHistoBenchmark(argv[1], 32);
    parallel_bench->run();

    return 0;
}