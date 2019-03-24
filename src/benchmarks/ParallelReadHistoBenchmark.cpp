#include <cstdint>
#include <ReadReader.h>
#include <Partition.h>
#include <Timer.h>
#include <HistogramTask.h>
#include <cinttypes>

#include "ThreadPool.h"
#include "ParallelReadHistoBenchmark.h"
#include "Utils.h"

using namespace std;

ParallelReadHistoBenchmark::ParallelReadHistoBenchmark(char* filename, int n_threads) : n_threads(n_threads) {
    f_init(filename, &fd, &file_size);
}

ParallelReadHistoBenchmark::~ParallelReadHistoBenchmark() = default;

void ParallelReadHistoBenchmark::run() {
    ThreadPool thread_pool(n_threads);

    auto global_histogram = static_cast<uint64_t*>(calloc(static_cast<size_t>(n_threads * 1024), sizeof(uint64_t)));

    Timer timer;

    timer.run();

    P_DEBUG("File size: %" PRIu64 "\n", file_size);

    if (file_size % n_threads == 0) {
        uint64_t segment_size = file_size/n_threads;

        for (int i = 0; i < n_threads; ++i) {
            uint64_t* local_hist = global_histogram + i * 1024;

            uint64_t from = i * segment_size;
            uint64_t to = (i + 1) * segment_size;

            thread_pool.add_task(new HistogramTask(local_hist, fd, from, to, 5000));
        }
    }
    else {
        cout << "File size is not divisible by the number of threads" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "All jobs in queue, waiting..." << endl;

    thread_pool.wait_all();

    timer.stop();

    cout << "IO time: " << std::fixed << timer.elapsedMilliseconds() << " ms" << endl;

    // Verify
    uint64_t sum = 0;
    for (uint64_t i = 0; i < n_threads * 1024; ++i)
        sum += global_histogram[i];

    cout << "Tuples read: " << sum  << endl;
}