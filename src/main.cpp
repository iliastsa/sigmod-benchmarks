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

using namespace std;

int main(int argc, char** argv) {
    Timer t;
    t.run();
    Benchmark* column_store_bench = new ColumnStoreBWBenchmark(argv[1], argv[2], Constants::N_THREADS);
    column_store_bench->run();
    t.stop();
    cout << "Total time: " << std::fixed << t.elapsedMilliseconds() << " ms" << endl;
    delete column_store_bench;

    return 0;
}