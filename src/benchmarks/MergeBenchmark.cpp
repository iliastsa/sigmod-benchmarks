#include <Tuple.h>
#include <Timer.h>
#include <fcntl.h>
#include <iostream>
#include <queue>
#include <boost/sort/block_indirect_sort/block_indirect_sort.hpp>
#include <ColumnStoreTask.h>
#include <ReadSortedRAMTask.h>
#include <PrepMergeBufferTask.h>
#include <HybridReadReader.h>
#include <BigTuple.h>
#include "MergeBenchmark.h"

using namespace std;

MergeBenchmark::MergeBenchmark(const char* input_file, const char* output_file, uint64_t file_size, uint64_t n_chunks, int n_threads)
    : input_file(input_file), output_file(output_file), file_size(file_size), n_chunks(n_chunks), n_threads(n_threads), thread_pool(n_threads, 0)
{
    out_fds = static_cast<int*>(malloc(n_chunks * sizeof(int)));
    merge_buffer = static_cast<unsigned char*>(malloc(n_chunks * Constants::MERGE_BUFFER_SIZE));
}

int MergeBenchmark::single_run(const uint16_t run, const char *output_file, uint64_t start, Tuple *tuples, uint64_t chunk_size, unsigned char *buffer) {
    const uint64_t num_tuples = chunk_size / Constants::TUPLE_SIZE;

    Timer timer;

    timer.run();

    uint64_t segment_size = chunk_size / n_threads;

    // Column store
    for (int i = 0; i < n_threads; ++i) {
        int fd = open(input_file, O_RDONLY);
        uint64_t from = i * segment_size;
        uint64_t to   = (i + 1) * segment_size;

        thread_pool.add_task(new ColumnStoreTask(tuples, fd, from + start, to + start, 0, from/Constants::TUPLE_SIZE, buffer + from));
    }

    cout << "All jobs in queue, waiting..." << endl;
    thread_pool.wait_all();

    timer.stop();

    cout << "Column store time: " << std::fixed << timer.elapsedMilliseconds() << " ms" << endl;

    // Sort column store
    timer.run();
    boost::sort::block_indirect_sort(tuples, tuples + num_tuples, Constants::N_THREADS);
    timer.stop();
    cout << "Sort time: " << std::fixed << timer.elapsedMilliseconds() << " ms" << endl;


    timer.run();

    uint64_t buffer_residue_size = chunk_size - Constants::MERGE_BUFFER_SIZE;
    uint64_t local_buffer_residue_size = buffer_residue_size / Constants::IO_THREADS;

    int out_fd = open(output_file, O_CREAT | O_RDWR | O_TRUNC, 0600);
    fallocate(out_fd, FALLOC_FL_ZERO_RANGE, 0, buffer_residue_size);


    // Order and write buffer residue to temp file
    for (uint32_t i = 0; i < Constants::IO_THREADS; ++i) {
        uint64_t from = i * local_buffer_residue_size;
        uint64_t thread_tuples = local_buffer_residue_size / Constants::TUPLE_SIZE;
        uint64_t thread_offset = i * thread_tuples + Constants::MERGE_BUFFER_SIZE / Constants::TUPLE_SIZE;

        thread_pool.add_task(new ReadSortedRAMTask(buffer, out_fd, from, tuples + thread_offset, thread_tuples));
    }

    unsigned char *current_merge_buffer = merge_buffer + run * Constants::MERGE_BUFFER_SIZE;
    const uint64_t local_merge_buffer_size = Constants::MERGE_BUFFER_SIZE / Constants::MEM_THREADS;

    // Order and store buffer prefix in memory
    for (uint32_t i = 0; i < Constants::MEM_THREADS; ++i) {
        unsigned char *local_merge_buffer = current_merge_buffer + i * local_merge_buffer_size;
        uint64_t thread_tuples = local_merge_buffer_size / Constants::TUPLE_SIZE;
        uint64_t thread_offset = i * thread_tuples;

        thread_pool.add_task(new PrepMergeBufferTask(local_merge_buffer, buffer, tuples + thread_offset, thread_tuples));
    }

    thread_pool.wait_all();

    timer.stop();
    cout << "Prep + write time: " << std::fixed << timer.elapsedMilliseconds() << " ms" << endl;

    return out_fd;
}

void MergeBenchmark::print_key(ChunkInfo *chunk_info) {
    for (uint32_t j = 0; j < Constants::KEY_SIZE; ++j)
        cout << (chunk_info->buffer + (chunk_info->size - Constants::TUPLE_SIZE))[j];
}

bool MergeBenchmark::check_flushable(ChunkInfo *chunk_info, uint32_t i) {
    for (uint32_t j = 0; j < n_chunks; ++j) {
        if (chunk_info[j].buffer == nullptr || j == i)
            continue;

        if (memcmp(chunk_info[j].buffer, chunk_info[i].buffer + (chunk_info[i].size - Constants::TUPLE_SIZE), Constants::KEY_SIZE) < 0)
            return false;
    }

    cout << "Triggered" << endl;

    return true;
}

int MergeBenchmark::find_flushable(MergeBenchmark::ChunkInfo* chunk_info) {
    for (uint32_t i = 0; i < n_chunks; ++i) {
        if (chunk_info[i].buffer == nullptr)
            continue;

        bool flag = true;

        for (uint32_t j = 0; j < n_chunks; ++j) {
            if (chunk_info[j].buffer == nullptr)
                continue;

            if (memcmp(chunk_info[j].buffer, chunk_info[i].buffer + (chunk_info[i].size - Constants::TUPLE_SIZE), Constants::KEY_SIZE) < 0)
                flag = false;
        }

        if (flag) {
            return i;
        }
    }

    return -1;
}

void MergeBenchmark::run() {
    const uint64_t chunk_size = file_size / n_chunks;
    const uint64_t chunk_tuples = chunk_size / Constants::TUPLE_SIZE;

    Tuple *tuples = static_cast<Tuple*>(malloc(chunk_tuples * sizeof(Tuple)));
    auto buffer = static_cast<unsigned char*> (malloc(chunk_size * sizeof(unsigned char)));

    // Sort chunks
    for (uint16_t i = 0; i < n_chunks; ++i) {
        uint64_t from = i * chunk_size;

        out_fds[i] = single_run(i, (string(output_file) + to_string(i)).c_str(), from, tuples, chunk_size, buffer);
    }

    free(tuples);
    free(buffer);

    Timer timer;

    timer.run();

    // Merge
    FileReader **readers = static_cast<FileReader**>(malloc(n_chunks * sizeof(FileReader*)));


    for (uint64_t i = 0; i < n_chunks; ++i)
        readers[i] = new HybridReadReader(out_fds[i], 0, chunk_size - Constants::MERGE_BUFFER_SIZE, Constants::MERGE_BUFFER_SIZE, merge_buffer + i * Constants::MERGE_BUFFER_SIZE);

    int out_fd = open(output_file, O_CREAT | O_WRONLY | O_TRUNC, 0600);
    fallocate(out_fd, FALLOC_FL_ZERO_RANGE, 0, file_size);

    BufferringWriter writer(out_fd, 0, Constants::WRITE_BUFFER_SIZE);

    ChunkInfo *chunk_info = static_cast<ChunkInfo*>(malloc(n_chunks * sizeof(MergeBenchmark::ChunkInfo)));

    priority_queue<ChunkInfo*, vector<ChunkInfo*>, Compare> tuple_pqueue;

    // Initialize chunk info
    for (uint64_t i = 0; i < n_chunks; ++i) {
        chunk_info[i].buffer = readers[i]->next(&chunk_info[i].size);
        chunk_info[i].idx = i;
        tuple_pqueue.push(&chunk_info[i]);
    }

    //for (uint64_t i = 0; i < file_size; i += Constants::TUPLE_SIZE) {
//        uint32_t min_tuple = 0;

//        while (chunk_info[min_tuple].buffer == nullptr)
//            min_tuple++;
//
//        if (min_tuple > n_chunks) {
//            cout << "widepeepoHappy DETH" << endl;
//            break;
//        }
//
//        for (uint32_t j = min_tuple + 1; j < n_chunks; ++j) {
//            if (chunk_info[j].buffer == nullptr)
//                continue;
//
//            if (memcmp(chunk_info[min_tuple].buffer, chunk_info[j].buffer, Constants::KEY_SIZE) > 0)
//                min_tuple = j;
//        }

    while (!tuple_pqueue.empty()) {
        ChunkInfo *min_tuple = tuple_pqueue.top();
        tuple_pqueue.pop();
//        if (check_flushable(chunk_info, min_tuple)) {
//            i += chunk_info[min_tuple].size - Constants::TUPLE_SIZE;
//            writer.write(chunk_info[min_tuple].buffer, chunk_info[min_tuple].size);
//            chunk_info[min_tuple].buffer = readers[min_tuple]->next(&chunk_info[min_tuple].size);
//            continue;
//        }
//        else
        writer.write(min_tuple->buffer, Constants::TUPLE_SIZE);

        min_tuple->size -= Constants::TUPLE_SIZE;

        if (min_tuple->size == 0) {
            min_tuple->buffer = readers[min_tuple->idx]->next(&min_tuple->size);

            if (min_tuple->buffer == nullptr)
                continue;
        }
        else
            min_tuple->buffer += Constants::TUPLE_SIZE;

        tuple_pqueue.push(min_tuple);
    }
    timer.stop();

    cout << "Merge time: " << timer.elapsedMilliseconds() << " ms" << endl;

    //close(out_fd);
}


