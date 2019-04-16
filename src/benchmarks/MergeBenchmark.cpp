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

        thread_pool.add_task(new ColumnStoreTask(tuples, fd, from + start, to + start, Constants::CHUNK_SIZE, 0, buffer + from));
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
    int out_fd = open(output_file, O_CREAT | O_RDWR | O_TRUNC, 0600);
    fallocate(out_fd, FALLOC_FL_ZERO_RANGE, 0, chunk_size);

    for (int i = 0; i < n_threads; ++i) {
        uint64_t from = i * segment_size;
        uint64_t thread_tuples = num_tuples / n_threads;
        uint64_t thread_offset = i * thread_tuples;

        thread_pool.add_task(new ReadSortedRAMTask(buffer, out_fd, from, tuples + thread_offset, thread_tuples));
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

        out_fds[i] = single_run(to_string(i).c_str(), from, tuples, chunk_size, buffer);
    }

    Timer timer;

    timer.run();

    // Merge
    ReadReader **readers = static_cast<ReadReader**>(malloc(n_chunks * sizeof(ReadReader*)));

    for (uint64_t i = 0; i < n_chunks; ++i)
        readers[i] = new ReadReader(out_fds[i], 0, chunk_size, Constants::CHUNK_SIZE);

    int out_fd = open(output_file, O_CREAT | O_WRONLY | O_TRUNC, 0600);
    fallocate(out_fd, FALLOC_FL_ZERO_RANGE, 0, file_size);

    BufferringWriter writer(out_fd, 0, Constants::WRITE_BUFFER_SIZE);

    ChunkInfo *chunk_info = static_cast<ChunkInfo*>(malloc(n_chunks * sizeof(MergeBenchmark::ChunkInfo)));

    for (uint64_t i = 0; i < n_chunks; ++i)
        chunk_info[i].buffer  = readers[i]->next(&chunk_info[i].size);

    for (uint64_t i = 0; i < file_size; i += Constants::TUPLE_SIZE) {
        uint32_t min_tuple = 0;

        while (chunk_info[min_tuple].buffer == nullptr)
            min_tuple++;

        if (min_tuple > n_chunks) {
            cout << "widepeepoHappy DETH" << endl;
            break;
        }

        for (uint32_t j = min_tuple + 1; j < n_chunks; ++j) {
            if (chunk_info[j].buffer == nullptr)
                continue;

            if (memcmp(chunk_info[min_tuple].buffer, chunk_info[j].buffer, Constants::KEY_SIZE) > 0)
                min_tuple = j;
        }

        writer.write(chunk_info[min_tuple].buffer, Constants::TUPLE_SIZE);

        chunk_info[min_tuple].size -= Constants::TUPLE_SIZE;

        if (chunk_info[min_tuple].size == 0)
            chunk_info[min_tuple].buffer  = readers[min_tuple]->next(&chunk_info[min_tuple].size);
        else
            chunk_info[min_tuple].buffer += Constants::TUPLE_SIZE;
    }
    timer.stop();

    cout << "Merge time: " << timer.elapsedMilliseconds() << " ms" << endl;

    //close(out_fd);
}


