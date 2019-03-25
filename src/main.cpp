#include <Benchmark.h>
#include <SequentialReadHistoBenchmark.h>
#include <ParallelReadHistoBenchmark.h>
#include <Constants.h>

int main(int argc, char **argv) {
    Benchmark *parallel_bench = new ParallelReadHistoBenchmark(argv[1], Constants::N_THREADS);
    parallel_bench->run();
    delete parallel_bench;

//    Benchmark *sequential_bench = new SequentialReadHistoBenchmark(argv[1]);
//    sequential_bench->run();
//    delete sequential_bench;

    return 0;
}