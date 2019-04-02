#include <FileInMemColumnStoreBenchmark.h>
#include <Utils.h>
#include <ThreadPool.h>
#include <boost/sort/block_indirect_sort/block_indirect_sort.hpp>
#include <iostream>
#include <Constants.h>
#include <Timer.h>
#include <fcntl.h>
#include <ColumnStoreTask.h>
#include <borrowedsort.h>
#include <RandomReadSortedTask.h>
#include <ReadSortedRAMTask.h>

using namespace std;

FileInMemColumnStoreBenchmark::FileInMemColumnStoreBenchmark(const char* filename, const char* out_filename, int n_threads)
        : filename(filename), out_filename(out_filename), n_threads(n_threads)
{
    f_init(filename, &fd, &file_size);
}


void FileInMemColumnStoreBenchmark::run() {
    ThreadPool thread_pool(n_threads);

    cout << "File size: " << file_size << endl;

    const uint64_t num_tuples = file_size / Constants::TUPLE_SIZE;

    tuples = static_cast<Tuple*>(malloc(num_tuples * sizeof(Tuple)));

    auto buffer = static_cast<unsigned char*> (malloc(file_size * sizeof(unsigned char)));

    Timer timer;

    timer.run();

    uint64_t segment_size = file_size / n_threads;

    for (int i = 0; i < n_threads; ++i) {
        int fd = open(filename, O_RDONLY);
        uint64_t from = i * segment_size;
        uint64_t to   = (i + 1) * segment_size;

        thread_pool.add_task(new ColumnStoreTask(tuples, fd, from, to, Constants::CHUNK_SIZE, buffer + from));
    }

    cout << "All jobs in queue, waiting..." << endl;
    thread_pool.wait_all();

    timer.stop();

    cout << "Column store time: " << std::fixed << timer.elapsedMilliseconds() << " ms" << endl;

    timer.run();
//    parasort(num_tuples, tuples, Constants::N_THREADS);
    boost::sort::block_indirect_sort(tuples, tuples + num_tuples, Constants::N_THREADS);
    timer.stop();
    cout << "Sort time: " << std::fixed << timer.elapsedMilliseconds() << " ms" << endl;

    timer.run();
    int out_fd = open(out_filename, O_CREAT | O_WRONLY, 0600);
    fallocate(out_fd, FALLOC_FL_ZERO_RANGE, 0, file_size);

    for (int i = 0; i < n_threads; ++i) {
        uint64_t from = i * segment_size;
        uint64_t thread_tuples = num_tuples / n_threads;
        uint64_t thread_offset = i * thread_tuples;

        thread_pool.add_task(new ReadSortedRAMTask(buffer, out_fd, from, tuples + thread_offset, thread_tuples));
    }

    thread_pool.wait_all();

    close(out_fd);

    timer.stop();
    cout << "Write time: " << std::fixed << timer.elapsedMilliseconds() << " ms" << endl;
}
