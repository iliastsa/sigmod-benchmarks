#include <cstdlib>
#include <ThreadPool.h>
#include <Constants.h>
#include <Timer.h>
#include <cinttypes>
#include <Utils.h>
#include <fcntl.h>
#include <MMapReader.h>
#include <WriteTask.h>
#include "WriteBenchmark.h"

using namespace std;

void WriteBenchmark::run() {
    auto *buffer = static_cast<unsigned char *>(malloc(filesize));

    for (uint64_t i = 0; i < filesize; ++i)
        buffer[i] = i % 256;

    ThreadPool thread_pool(n_threads);

    Timer timer;

    timer.run();

    P_DEBUG("File size: %" PRIu64 "\n", filesize);

    int fd = open(filename, O_CREAT | O_WRONLY, 0600);

    if (filesize % n_threads == 0) {
        uint64_t segment_size = filesize/n_threads;

        for (int i = 0; i < n_threads; ++i) {
            uint64_t from = i * segment_size;


            thread_pool.add_task(new WriteTask(fd, buffer + from, from, segment_size));
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

}

WriteBenchmark::~WriteBenchmark() {

}

WriteBenchmark::WriteBenchmark(char *filename, uint64_t filesize, int n_threads) : n_threads(n_threads),
                                                                                   filesize(filesize),
                                                                                   filename(filename) {}
