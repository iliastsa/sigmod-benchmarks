#include <cstdint>
#include <queue>
#include <vector>
#include <Timer.h>
#include <fcntl.h>
#include <BigTuple.h>
#include <FileReader.h>
#include <HybridReadReader.h>
#include <Utils.h>
#include <BigTupleChunkProcess.h>
#include <MergeChunkTask.h>
#include <CopyTask.h>
#include <BufferringWriter.h>
#include "BigTupleMergeBenchmark.h"

using namespace std;

BigTupleMergeBenchmark::BigTupleMergeBenchmark(const char* input_file, const char* output_file, uint64_t file_size, uint64_t n_chunks)
        : input_file(input_file), output_file(output_file), file_size(file_size), n_chunks(n_chunks)
{
    out_fds = static_cast<int*>(malloc(n_chunks * sizeof(int)));
}

void BigTupleMergeBenchmark::run() {

    uint64_t chunk_size = file_size / n_chunks;

    unsigned char *global_mem = static_cast<unsigned char*>(malloc(Constants::MEM_ALLOC));

    /* Determine chunk schedule */

    vector<uint16_t> chunk_schedule;

    uint16_t scheduled_chunks = 0;
    uint64_t free_mem = Constants::MEM_LIMIT;

    while (scheduled_chunks < n_chunks) {
        uint16_t next_chunk_num = static_cast<uint16_t>(free_mem / chunk_size);
        next_chunk_num = (uint16_t)(next_chunk_num == 0 ? 1 : next_chunk_num);
        chunk_schedule.push_back(next_chunk_num);
        free_mem -= next_chunk_num * Constants::MERGE_BUFFER_SIZE;
        scheduled_chunks += next_chunk_num;
    }

    /* Initialization */

    //TODO Tpool sizes
    ThreadPool sort_tpool(1, 0);
    ThreadPool io_tpool(40);

    uint64_t file_offset = 0;
    uint16_t chunk_counter = 0;

    FileReader **readers = static_cast<FileReader**>(malloc(n_chunks * sizeof(FileReader*)));
    FileReader **current_reader = readers;

    /* Run schedule */

    Timer t;

    const uint64_t buffer_residue_size = chunk_size - Constants::MERGE_BUFFER_SIZE;

    for (auto chunk_num : chunk_schedule) {

        t.run();
        unsigned char* current_base_mem = global_mem + chunk_counter * Constants::MERGE_BUFFER_SIZE;

        for (uint16_t i = 0; i < chunk_num; i++) {

            string temp_file = string(output_file) + to_string(chunk_counter);

            int in_fd  = open(input_file, O_RDONLY);
            int out_fd = open(temp_file.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0600);
            fallocate(out_fd, FALLOC_FL_ZERO_RANGE, 0, buffer_residue_size);

            /* Care with this */
            out_fds[2*i % chunk_num + (2*i >= chunk_num && chunk_num % 2 == 0)] = out_fd;

            io_tpool.add_task(new MergeChunkTask(in_fd, out_fd, current_base_mem, i, chunk_size, file_offset,
                                                    &sort_tpool, &io_tpool));

            /* Updates */
            current_base_mem += chunk_size;
            file_offset += chunk_size;
            chunk_counter++;
        }

        io_tpool.wait_all();
        sort_tpool.wait_all();
        io_tpool.wait_all();

        t.stop();
        cout << "Processed " << chunk_counter << " chunks in " <<
                       fixed << t.elapsedMilliseconds() << " ms" << endl;

        t.run();

        /* Cleanup for next stage */

        unsigned char* base_mem = global_mem + (chunk_counter - chunk_num) * Constants::MERGE_BUFFER_SIZE;

        /* Chunks that are copied */

        for (uint16_t i = (uint16_t)((chunk_num + 1) / 2), k = 0; i < chunk_num; i++, k++) {

            /* Copy */
            unsigned char* src  = base_mem + chunk_size * i;
            unsigned char* dest = base_mem + chunk_size * k + Constants::MERGE_BUFFER_SIZE;

            io_tpool.add_task(new CopyTask(src, dest, Constants::MERGE_BUFFER_SIZE));

            /* Init reader */

            int out_fd = out_fds[2*i % chunk_num + (chunk_num % 2 == 0)];

            *current_reader = new HybridReadReader(out_fd, 0, buffer_residue_size, Constants::MERGE_BUFFER_SIZE, dest);
            current_reader++;


        }

        /* Chunks that stay in place */

        for (uint16_t i = 0; i < (chunk_num+1)/2; i++) {

            int out_fd = out_fds[2*i];

            unsigned char* chunk_address = base_mem + i*chunk_size;
            /* Init reader */
            *current_reader = new HybridReadReader(out_fd, 0, buffer_residue_size, Constants::MERGE_BUFFER_SIZE, chunk_address);
            current_reader++;

        }

        t.stop();

        cout << "Cleanup time for " << chunk_counter << " chunks : " <<
             fixed << t.elapsedMilliseconds() << " ms" << endl;
    }


    Timer timer;

    timer.run();

    // Merge
    int out_fd = open(output_file, O_CREAT | O_WRONLY | O_TRUNC, 0600);
    fallocate(out_fd, FALLOC_FL_ZERO_RANGE, 0, file_size);

    BufferringWriter writer(out_fd, 0, Constants::WRITE_BUFFER_SIZE);

    ChunkInfo *chunk_info = static_cast<ChunkInfo*>(malloc(n_chunks * sizeof(BigTupleMergeBenchmark::ChunkInfo)));

    priority_queue<ChunkInfo*, vector<ChunkInfo*>, Compare> tuple_pqueue;

    // Initialize chunk info
    for (uint32_t i = 0; i < n_chunks; ++i) {
        chunk_info[i].buffer = readers[i]->next(&chunk_info[i].size);
        chunk_info[i].idx = i;
        tuple_pqueue.push(&chunk_info[i]);
    }



    while (!tuple_pqueue.empty()) {
        ChunkInfo *min_tuple = tuple_pqueue.top();
        tuple_pqueue.pop();

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

    close(out_fd);
}