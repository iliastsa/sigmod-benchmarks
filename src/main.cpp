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
#include <MergeBenchmark.h>

using namespace std;

int main(int argc, char** argv) {
    Benchmark* bench;

    switch (Constants::dataset = determine_dataset(argv[1])) {
        case Constants::SMALL:
            bench = new FileInMemColumnStoreBenchmark(argv[1], argv[2], Constants::N_THREADS);
            break;
        case Constants::MEDIUM:
            Constants::WRITE_BUFFER_SIZE = 1024 * 1024;
            bench = new FileInMemColumnStoreBenchmark(argv[1], argv[2], Constants::N_THREADS);
            break;
        case Constants::LARGE:
            Constants::WRITE_BUFFER_SIZE = 1024 * 1024;
            Constants::CHUNK_SIZE = 1000 * 1000;
            bench = new MergeBenchmark(argv[1], argv[2], get_file_size(argv[1]), 3, Constants::N_THREADS);
            sleep(15);
            break;
        default:
            exit(EXIT_FAILURE);
    }

    bench->run();

    return 0;
}