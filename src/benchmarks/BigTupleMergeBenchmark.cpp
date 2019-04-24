#include <cstdint>
#include <vector>
#include <Timer.h>
#include <fcntl.h>
#include <BigTuple.h>
#include <FileReader.h>
#include <HybridReadReader.h>
#include <Utils.h>
#include "BigTupleMergeBenchmark.h"

using namespace std;

BigTupleMergeBenchmark::BigTupleMergeBenchmark(const char* input_file, const char* output_file, uint64_t file_size, uint64_t n_chunks, int n_threads)
        : input_file(input_file), output_file(output_file), file_size(file_size), n_chunks(n_chunks), n_threads(n_threads), thread_pool(n_threads, 0)
{
    out_fds = static_cast<int*>(malloc(n_chunks * sizeof(int)));
    merge_buffer = static_cast<unsigned char*>(malloc(n_chunks * Constants::MERGE_BUFFER_SIZE));
}

void BigTupleMergeBenchmark::run() {
    //TODO CHANGE THIS LATER
    file_size = MB(60);
    uint64_t chunk_size = file_size / n_chunks;
    uint64_t chunk_tuples = chunk_size / Constants::TUPLE_SIZE;

    unsigned char *working_set = static_cast<unsigned char*>(malloc(Constants::MEM_LIMIT));

    // Sort chunks

    vector<ChunkScheduleInfo> chunk_schedule;

    uint16_t scheduled_chunks = 0;
    uint64_t free_mem = Constants::MEM_LIMIT;

    while (scheduled_chunks < n_chunks) {
        uint16_t next_chunk_num = static_cast<uint16_t>(free_mem / chunk_size);
        chunk_schedule.push_back(ChunkScheduleInfo(next_chunk_num, chunk_size));
        free_mem -= next_chunk_num * Constants::MERGE_BUFFER_SIZE;
        scheduled_chunks += next_chunk_num;
    }


//    for (auto chunk_num : chunk_schedule) {
//
//        for (uint16_t i = 0; i < chunk_num; i++) {
//            thread_pool.add_task(PROCESSONECHUNK);
//        }
//        thread_pool.wait_all();
//
//        CLEANUPFORNEXTRUN;
//    }





//    for (uint16_t i = 0; i < n_chunks; ++i) {
//        uint64_t from = i * chunk_size;
//
//        out_fds[i] = single_run(i, (string(output_file) + to_string(i)).c_str(), from, tuples, chunk_size, buffer);
//    }
//
//
//    Timer timer;
//
//    timer.run();
//
//    // Merge
//    FileReader **readers = static_cast<FileReader**>(malloc(n_chunks * sizeof(FileReader*)));
//
//
//    for (uint64_t i = 0; i < n_chunks; ++i)
//        readers[i] = new HybridReadReader(out_fds[i], 0, chunk_size - Constants::MERGE_BUFFER_SIZE, Constants::MERGE_BUFFER_SIZE, merge_buffer + i * Constants::MERGE_BUFFER_SIZE);
//
//    int out_fd = open(output_file, O_CREAT | O_WRONLY | O_TRUNC, 0600);
//    fallocate(out_fd, FALLOC_FL_ZERO_RANGE, 0, file_size);
//
//    BufferringWriter writer(out_fd, 0, Constants::WRITE_BUFFER_SIZE);
//
//    ChunkInfo *chunk_info = static_cast<ChunkInfo*>(malloc(n_chunks * sizeof(BigTupleMergeBenchmark::ChunkInfo)));
//
//    priority_queue<ChunkInfo*, vector<ChunkInfo*>, Compare> tuple_pqueue;
//
//    // Initialize chunk info
//    for (uint64_t i = 0; i < n_chunks; ++i) {
//        chunk_info[i].buffer = readers[i]->next(&chunk_info[i].size);
//        chunk_info[i].idx = i;
//        tuple_pqueue.push(&chunk_info[i]);
//    }
//
//    //for (uint64_t i = 0; i < file_size; i += Constants::TUPLE_SIZE) {
////        uint32_t min_tuple = 0;
//
////        while (chunk_info[min_tuple].buffer == nullptr)
////            min_tuple++;
////
////        if (min_tuple > n_chunks) {
////            cout << "widepeepoHappy DETH" << endl;
////            break;
////        }
////
////        for (uint32_t j = min_tuple + 1; j < n_chunks; ++j) {
////            if (chunk_info[j].buffer == nullptr)
////                continue;
////
////            if (memcmp(chunk_info[min_tuple].buffer, chunk_info[j].buffer, Constants::KEY_SIZE) > 0)
////                min_tuple = j;
////        }
//
//    while (!tuple_pqueue.empty()) {
//        ChunkInfo *min_tuple = tuple_pqueue.top();
//        tuple_pqueue.pop();
////        if (check_flushable(chunk_info, min_tuple)) {
////            i += chunk_info[min_tuple].size - Constants::TUPLE_SIZE;
////            writer.write(chunk_info[min_tuple].buffer, chunk_info[min_tuple].size);
////            chunk_info[min_tuple].buffer = readers[min_tuple]->next(&chunk_info[min_tuple].size);
////            continue;
////        }
////        else
//        writer.write(min_tuple->buffer, Constants::TUPLE_SIZE);
//
//        min_tuple->size -= Constants::TUPLE_SIZE;
//
//        if (min_tuple->size == 0) {
//            min_tuple->buffer = readers[min_tuple->idx]->next(&min_tuple->size);
//
//            if (min_tuple->buffer == nullptr)
//                continue;
//        }
//        else
//            min_tuple->buffer += Constants::TUPLE_SIZE;
//
//        tuple_pqueue.push(min_tuple);
//    }
//    timer.stop();
//
//    cout << "Merge time: " << timer.elapsedMilliseconds() << " ms" << endl;

    //close(out_fd);
}