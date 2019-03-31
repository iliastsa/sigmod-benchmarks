
#include <Utils.h>
#include <unistd.h>
#include <ThreadPool.h>
#include <Timer.h>
#include <cinttypes>
#include <Constants.h>
#include <ColumnStoreTask.h>
#include <fcntl.h>
#include <borrowedsort.h>
#include <RandomWriteSortedTask.h>
#include "ColumnStoreBenchmark.h"


using namespace std;

ColumnStoreBenchmark::ColumnStoreBenchmark(const char *filename, const char *out_filename, int n_threads)
        : filename(filename), out_filename(out_filename), n_threads(n_threads)
{
    f_init(filename, &fd, &file_size);
}

ColumnStoreBenchmark::~ColumnStoreBenchmark() {
    close(fd);
}

void ColumnStoreBenchmark::run() {
    ThreadPool thread_pool(n_threads);

    const uint64_t num_tuples = file_size / Constants::TUPLE_SIZE;

    Tuple *tuples = static_cast<Tuple*>(malloc(num_tuples * sizeof(Tuple)));

    P_DEBUG("File size: %" PRIu64 "\n", file_size);

    Timer timer;

    timer.run();

    uint64_t segment_size = file_size / n_threads;

    for (int i = 0; i < n_threads; ++i) {
        int fd = open(filename, O_RDONLY);
        uint64_t from = i * segment_size;
        uint64_t to   = (i + 1) * segment_size;

        thread_pool.add_task(new ColumnStoreTask(tuples, fd, from, to, Constants::CHUNK_SIZE));
    }

    cout << "All jobs in queue, waiting..." << endl;

    thread_pool.wait_all();

    timer.stop();

    cout << "Column store time: " << std::fixed << timer.elapsedMilliseconds() << " ms" << endl;

//    // Verify
//    for (uint64_t i = 0; i < num_tuples; ++i){
//        if (tuples[i].rowID != i) {
//            P_DEBUG("Row id is wrong in verification");
//            exit(EXIT_FAILURE);
//        }
//    }

    timer.run();
    parasort(num_tuples, tuples, Constants::N_THREADS);
    timer.stop();
    cout << "Sort time: " << std::fixed << timer.elapsedMilliseconds() << " ms" << endl;

    timer.run();
    // Re arrange the tuples for easy access
    auto * tuples_by_row = static_cast<uint64_t*>(malloc(num_tuples * sizeof(uint64_t)));
    for (uint64_t i = 0; i < num_tuples; i++) {
        tuples_by_row[tuples[i].rowID] = i;
    }
    timer.stop();
    cout << "Rearrange time: " << std::fixed << timer.elapsedMilliseconds() << " ms" << endl;

    timer.run();
    //Read chunks and write in file.
    int out_fd = open(out_filename, O_CREAT | O_WRONLY, 0600);
    fallocate(out_fd, FALLOC_FL_ZERO_RANGE, 0, file_size);
    close(out_fd);

    for (int i = 0; i < n_threads; ++i) {
        int fd = open(filename, O_RDONLY, 0600);
        int out_fd = open(out_filename, O_WRONLY, 0600);

        uint64_t from = i * segment_size;
        uint64_t to   = (i + 1) * segment_size;


        thread_pool.add_task(new RandomWriteSortedTask(fd, out_fd, from, to, Constants::CHUNK_SIZE, tuples_by_row));
    }

    thread_pool.wait_all();
    timer.stop();


    cout << "Write to disk: " << std::fixed << timer.elapsedMilliseconds() << " ms" << endl;


//    // Verify
//    for (uint64_t i = 1; i < num_tuples ; ++i){
//        if (tuples[i] < tuples[i-1]) {
//            P_DEBUG("Bad para sort");
//            exit(EXIT_FAILURE);
//        }
//    }

    sleep(10);
}
