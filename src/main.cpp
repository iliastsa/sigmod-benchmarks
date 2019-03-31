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
#include <WriteBenchmark.h>
#include <ColumnStoreBenchmark.h>
#include <Timer.h>
#include <climits>
#include <ColumnStoreBWBenchmark.h>
#include <borrowedsort.h>

using namespace std;

int main(int argc, char** argv) {

    Timer t;
    t.run();
    Benchmark* column_store_bench = new ColumnStoreBWBenchmark(argv[1], argv[2], Constants::N_THREADS);
    column_store_bench->run();
    t.stop();
    cout << "Total time: " << std::fixed << t.elapsedMilliseconds() << " ms" << endl;
    delete column_store_bench;

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