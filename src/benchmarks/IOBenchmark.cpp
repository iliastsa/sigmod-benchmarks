#include <cstdlib>
#include <ThreadPool.h>
#include <Constants.h>
#include <Timer.h>
#include <cinttypes>
#include <Utils.h>
#include <fcntl.h>
#include <MMapReader.h>
#include <WriteTask.h>
#include <ReadTask.h>
#include "IOBenchmark.h"

using namespace std;

void IOBenchmark::run() {
    ThreadPool thread_pool(n_threads);
    auto *buffer = static_cast<unsigned char *>(malloc(filesize));

    Timer timer;

    timer.run();

    uint64_t segment_size = filesize / n_threads;

    for (int i = 0; i < n_threads; ++i) {
        int fd = open(filename_in, O_RDONLY);
        uint64_t from = i * segment_size;
        uint64_t to   = (i + 1) * segment_size;

        thread_pool.add_task(new ReadTask(fd, from, to, Constants::CHUNK_SIZE, buffer + from));
    }

    thread_pool.wait_all();

    timer.stop();

    cout << "[" << n_threads << " threads] Read time: " << std::fixed << timer.elapsedMilliseconds() << " ms for file size: " << filesize/1000000000 << " GB" << endl;

    timer.run();

    int fd = open(filename_out, O_CREAT | O_WRONLY, 0600);

    fallocate(fd, FALLOC_FL_ZERO_RANGE, 0, filesize);

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

    thread_pool.wait_all();

    timer.stop();

    cout << "[" << n_threads << " threads] Write time: " << std::fixed << timer.elapsedMilliseconds() << " ms for file size: " << filesize/1000000000 << " GB" << endl;

}

IOBenchmark::~IOBenchmark() {

}

IOBenchmark::IOBenchmark(char *filename_in, char *filename_out, uint64_t filesize, int n_threads) : n_threads(n_threads),
                                                                                   filesize(filesize),
                                                                                   filename_in(filename_in),
                                                                                   filename_out(filename_out){}
