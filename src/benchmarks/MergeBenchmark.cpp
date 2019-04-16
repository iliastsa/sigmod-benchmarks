#include <Tuple.h>
#include <Timer.h>
#include <fcntl.h>
#include <ColumnStoreTask.h>
#include <iostream>
#include <boost/sort/block_indirect_sort/block_indirect_sort.hpp>
#include <ReadSortedRAMTask.h>
#include "MergeBenchmark.h"

using namespace std;

MergeBenchmark::MergeBenchmark(const char* input_file, const char* output_file, uint64_t file_size, uint64_t n_chunks, int n_threads)
    : input_file(input_file), output_file(output_file), file_size(file_size), n_chunks(n_chunks), n_threads(n_threads), thread_pool(n_threads)
{
    out_fds = static_cast<int*>(malloc(n_chunks * sizeof(int)));
}

int MergeBenchmark::single_run(const char *output_file, uint64_t start, Tuple *tuples, uint64_t chunk_size, unsigned char *buffer) {
    const uint64_t num_tuples = chunk_size / Constants::TUPLE_SIZE;

    Timer timer;

    timer.run();

    uint64_t segment_size = chunk_size / n_threads;

    for (int i = 0; i < n_threads; ++i) {
        int fd = open(input_file, O_RDONLY);
        uint64_t from = i * segment_size;
        uint64_t to   = (i + 1) * segment_size;

        thread_pool.add_task(new ColumnStoreTask(tuples, fd, from + start, to + start, Constants::CHUNK_SIZE, buffer + from));
    }

    cout << "All jobs in queue, waiting..." << endl;
    thread_pool.wait_all();

    timer.stop();

    cout << "Column store time: " << std::fixed << timer.elapsedMilliseconds() << " ms" << endl;


    timer.run();
    boost::sort::block_indirect_sort(tuples, tuples + num_tuples, Constants::N_THREADS);
    timer.stop();
    cout << "Sort time: " << std::fixed << timer.elapsedMilliseconds() << " ms" << endl;

    timer.run();
    int out_fd = open(output_file, O_CREAT | O_WRONLY, 0600);
    fallocate(out_fd, FALLOC_FL_ZERO_RANGE, 0, chunk_size);

    for (int i = 0; i < n_threads; ++i) {
        uint64_t from = i * segment_size;
        uint64_t thread_tuples = num_tuples / n_threads;
        uint64_t thread_offset = i * thread_tuples;

        thread_pool.add_task(new ReadSortedRAMTask(buffer, out_fd, from + start, tuples + thread_offset, thread_tuples));
    }

    thread_pool.wait_all();

    timer.stop();
    cout << "Write time: " << std::fixed << timer.elapsedMilliseconds() << " ms" << endl;

    return out_fd;
}

void MergeBenchmark::run() {
    const uint64_t chunk_size = file_size / n_chunks;
    const uint64_t chunk_tuples = chunk_size / Constants::TUPLE_SIZE;

    Tuple *tuples = static_cast<Tuple*>(malloc(chunk_tuples * sizeof(Tuple)));
    auto buffer = static_cast<unsigned char*> (malloc(chunk_size * sizeof(unsigned char)));

    // Sort chunks
    for (uint64_t i = 0; i < n_chunks; ++i) {
        uint64_t from = i * chunk_size;

        cout << "Chunk tuples: " << chunk_tuples << " Chunk size: " << chunk_size << " From: " << from << endl;

        out_fds[i] = single_run(to_string(i).c_str(), from, tuples, chunk_size, buffer);

        for (uint64_t j = 0; j < chunk_tuples - 1; ++j)
            tuples[j] = tuples[j + 1];

        for (uint64_t j = 0; j < chunk_size - 1; ++j)
            buffer[j] = buffer[j + 1];
    }
}


