#include <Benchmark.h>
#include <SequentialReadHistoBenchmark.h>
#include <ParallelReadHistoBenchmark.h>
#include <Constants.h>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>
#include <sys/mman.h>
#include <iostream>
#include <SingleMMapHistoBenchmark.h>
#include <IOBenchmark.h>
#include <ColumnStoreBenchmark.h>
#include <Timer.h>
#include <climits>
#include <ColumnStoreBWBenchmark.h>
#include <borrowedsort.h>
#include <InMemoryBenchmark.h>
#include <FileInMemColumnStoreBenchmark.h>

using namespace std;

int main(int argc, char** argv) {
    if (!strcmp(argv[2], "/output-disk/small.out")) {
        Constants::dataset = Constants::Dataset::SMALL;
    }
    else if (!strcmp(argv[2], "/output-disk/medium.out")){
        Constants::WRITE_BUFFER_SIZE = 1024 * 1024;
        Constants::dataset = Constants::Dataset::MEDIUM;
    }
    else {
        Constants::WRITE_BUFFER_SIZE = 1024 * 1024;
        Constants::CHUNK_SIZE = 1000 * 1000;
        Constants::dataset = Constants::Dataset::LARGE;
    }

    RUN_WHEN(Constants::Dataset::SMALL,
    Benchmark* bench = new FileInMemColumnStoreBenchmark(argv[1], argv[2], Constants::N_THREADS);
    bench->run();)


    RUN_WHEN(Constants::Dataset::MEDIUM,
            Benchmark* bench = new FileInMemColumnStoreBenchmark(argv[1], argv[2], Constants::N_THREADS);
    bench->run();)

    RUN_WHEN(Constants::Dataset::LARGE,
             Benchmark* bench = new ColumnStoreBWBenchmark(argv[1], argv[2], Constants::N_THREADS);
                     bench->run(); sleep(15);)


//    Benchmark* column_store_bench = new ColumnStoreBWBenchmark(argv[1], argv[2], Constants::N_THREADS);
//    column_store_bench->run();

//    auto in_mem_bench = new InMemoryBenchmark(argv[1], argv[2], Constants::N_THREADS);
//    in_mem_bench->run();

//    uint64_t size = 100000000;
//    int32_t *nums = static_cast<int32_t*>(malloc(size * sizeof(int32_t)));
//
//    for (int32_t i = 0; i < size; ++i) {
//        nums[i] = static_cast<int32_t>(rand());
//    }
//
//    parasort(size, nums, 40);
//
//    for (int32_t i = 0; i < size - 1; ++i)
//        if (nums[i] > nums[i+1])
//            exit(EXIT_FAILURE);

    return 0;
}