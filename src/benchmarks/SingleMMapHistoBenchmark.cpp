#include <cstdint>
#include <ReadReader.h>
#include <Partition.h>
#include <Timer.h>
#include <InterleavedHistogramTask.h>
#include <cinttypes>
#include <unistd.h>
#include <fcntl.h>
#include <MMapReader.h>
#include <ASyncReader.h>
#include <Constants.h>
#include <sys/mman.h>
#include <HistogramTask.h>

#include "ThreadPool.h"
#include "SingleMMapHistoBenchmark.h"
#include "Utils.h"

using namespace std;

SingleMMapHistoBenchmark::SingleMMapHistoBenchmark(char* filename, int n_threads) :
        n_threads(n_threads), filename(filename)
{
    f_init(filename, &fd, &file_size);
}

SingleMMapHistoBenchmark::~SingleMMapHistoBenchmark() {
    munmap(mem, file_size);
}

void SingleMMapHistoBenchmark::run() {
    ThreadPool thread_pool(n_threads);

    auto global_histogram = static_cast<uint64_t*>(calloc(static_cast<size_t>(n_threads * Constants::N_PARTITIONS), sizeof(uint64_t)));

    Timer timer;

    timer.run();

    P_DEBUG("File size: %" PRIu64 "\n", file_size);

    if (file_size % n_threads == 0) {
        uint64_t segment_size = file_size/n_threads;

        mem = mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, fd, 0);

        for (int i = 0; i < n_threads; ++i) {
            uint64_t* local_hist = global_histogram + i * Constants::N_PARTITIONS;

            uint64_t from = i * segment_size;
            unsigned char *local_mem = static_cast<unsigned char*>(mem) + from;

            thread_pool.add_task(new HistogramTask(local_hist, local_mem, segment_size));
        }
    }
    else {
        cout << "File size is not divisible by the number of threads" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "All jobs in queue, waiting..." << endl;

    thread_pool.wait_all();

    timer.stop();

    cout << "Total time: " << std::fixed << timer.elapsedMilliseconds() << " ms" << endl;

    // Verify
    uint64_t sum = 0;
    for (uint64_t i = 0; i < n_threads * Constants::N_PARTITIONS; ++i)
        sum += global_histogram[i];

    cout << "Tuples read: " << sum  << endl;
}