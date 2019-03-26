
#include <Utils.h>
#include <unistd.h>
#include <ThreadPool.h>
#include <Timer.h>
#include <cinttypes>
#include <Constants.h>
#include <ColumnStoreTask.h>
#include <fcntl.h>
#include <borrowedsort.h>
#include "ColumnStoreBenchmark.h"


using namespace std;

ColumnStoreBenchmark::ColumnStoreBenchmark(const char* filename, int n_threads)
        : filename(filename), n_threads(n_threads)
{
    f_init(filename, &fd, &file_size);
}

ColumnStoreBenchmark::~ColumnStoreBenchmark() {
    close(fd);
    free(tuples);
}

void ColumnStoreBenchmark::run() {
    ThreadPool thread_pool(n_threads);

    const uint64_t num_tuples = file_size / Constants::TUPLE_SIZE;

    tuples = static_cast<Tuple*>(malloc(num_tuples * sizeof(Tuple)));

    P_DEBUG("File size: %" PRIu64 "\n", file_size);

    Timer timer;

    timer.run();

    uint64_t segment_size = file_size / n_threads;

    for (int i = 0; i < n_threads; ++i) {
        int fd = open(filename, O_RDONLY);
        uint64_t from = i * segment_size;
        uint64_t to   = (i + 1) * segment_size;
        uint64_t tuple_offset = from / Constants::TUPLE_SIZE;

        thread_pool.add_task(new ColumnStoreTask(tuples, fd, from, to, Constants::CHUNK_SIZE));
    }

    cout << "All jobs in queue, waiting..." << endl;

    thread_pool.wait_all();

    timer.stop();

    cout << "Column store time: " << std::fixed << timer.elapsedMilliseconds() << " ms" << endl;

    // Verify
    for (uint64_t i = 0; i < num_tuples ; ++i){
        if (tuples[i].rowID != i) {
            P_DEBUG("Row id is wrong in verification");
            exit(EXIT_FAILURE);
        }
    }

    timer.run();
    parasort(num_tuples, tuples, Constants::N_THREADS);
    timer.stop();
    cout << "Sort time: " << std::fixed << timer.elapsedMilliseconds() << " ms" << endl;

    // Verify
    for (uint64_t i = 1; i < num_tuples ; ++i){
        if (tuples[i] < tuples[i-1]) {
            P_DEBUG("Bad para sort");
            exit(EXIT_FAILURE);
        }
    }


}
