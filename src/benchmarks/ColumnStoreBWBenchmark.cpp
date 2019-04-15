#include <cstdint>
#include <fcntl.h>

#include <boost/sort/block_indirect_sort/block_indirect_sort.hpp>

#include <ThreadPool.h>
#include <Constants.h>
#include <Tuple.h>
#include <Timer.h>
#include <Utils.h>

#include <borrowedsort.h>

#include <ColumnStoreBWBenchmark.h>
#include <RandomReadSortedTask.h>
#include <ColumnStoreTask.h>
#include <unordered_set>

using namespace std;

ColumnStoreBWBenchmark::ColumnStoreBWBenchmark(const char *filename, const char *out_filename, int n_threads)
            : filename(filename), out_filename(out_filename), n_threads(n_threads)
{
    f_init(filename, &fd, &file_size);
}


void ColumnStoreBWBenchmark::run() {
    ThreadPool thread_pool(n_threads);

    cout << "File size: " << file_size << endl;

    const uint64_t num_tuples = file_size / Constants::TUPLE_SIZE;

    tuples = static_cast<Tuple*>(malloc(num_tuples * sizeof(Tuple)));

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

    timer.run();
//    parasort(num_tuples, tuples, Constants::N_THREADS);
    boost::sort::block_indirect_sort(tuples, tuples + num_tuples, Constants::N_THREADS);
    timer.stop();
    cout << "Sort time: " << std::fixed << timer.elapsedMilliseconds() << " ms" << endl;


//    Tuple& cur = tuples[0];
//    uint64_t counter = 1;
//    uint64_t frequent_keys = 0;
//    uint64_t limit = 50;
//    uint64_t max_occurences = 1;
//    for (uint64_t i = 1; i < num_tuples; i++){
//
//        if (memcmp(cur.key, tuples[i].key, Constants::KEY_SIZE) == 0){
//            counter++;
//        }
//        else{
//            max_occurences = counter > max_occurences ? counter : max_occurences;
//            counter = 1;
//            cur = tuples[i];
//        }
//
//        if (counter > limit){
//            frequent_keys++;
//            while(memcmp(cur.key, tuples[i++].key, Constants::KEY_SIZE) == 0) counter++;
//            max_occurences = counter > max_occurences ? counter : max_occurences;
//            cur = tuples[i];
//            counter = 1;
//        }
//    }
//    cout << "Keys more than " << limit << " are " << frequent_keys << endl;
//    cout << "Max occurrence is " << max_occurences << endl;
    timer.run();
    int out_fd = open(out_filename, O_CREAT | O_WRONLY, 0600);
    fallocate(out_fd, 0, 0, file_size);

    for (int i = 0; i < n_threads; ++i) {
        uint64_t from = i * segment_size;
        uint64_t thread_tuples = num_tuples / n_threads;
        uint64_t thread_offset = i * thread_tuples;

        thread_pool.add_task(new RandomReadSortedTask(fd, out_fd, from, tuples + thread_offset, thread_tuples));
    }

    thread_pool.wait_all();

    close(out_fd);

    timer.stop();
    cout << "Write time: " << std::fixed << timer.elapsedMilliseconds() << " ms" << endl;

    f_init(out_filename, &fd, &file_size);
    cout << "Out file size: " << file_size << endl;

}


