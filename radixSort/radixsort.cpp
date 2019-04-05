
#include "utils.h"
#include "Timer.h"
#include "Constants.h"
#include "CountTask.h"
#include "BinsRangeTask.h"
#include "RadixSortPassTask.h"
#include "RadixQuicksort.h"

int main(int argc, char **argv){
    uint32_t fd;
    uint64_t file_size;
    struct stat sb = {0};
    fd = open(argv[1], O_RDONLY);

    if (stat(argv[1], &sb) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    file_size = static_cast<uint64_t>(sb.st_size);
    cout << "file size: " << file_size << endl;
    uint32_t num_tuples = file_size / 100;
    cout << "Tuples: " << num_tuples << endl;

    // uint64_t segment_size = file_size / Constants::N_THREADS;
    Tuple *tuples = static_cast<Tuple*>(malloc(num_tuples * sizeof(Tuple)));
    // Tuple *tmp = static_cast<Tuple*>(malloc(num_tuples * sizeof(Tuple)));
    // for (uint32_t i = 0; i < num_tuples; i++) {
    //     tuples[i].key = static_cast<unsigned char*>(malloc(Constants::KEY_SIZE * sizeof(char)));
    //     // tuples[i].payload = static_cast<unsigned char*>(malloc(Constants::PAYLOAD * sizeof(char)));
    // }


    ThreadPool thread_pool(Constants::N_THREADS);
    Timer timer;

    timer.run();

    readFile(num_tuples, tuples, fd, file_size);

    timer.stop();

    cout << "Column store time: " << std::fixed << timer.elapsedMilliseconds() << " ms" << endl;

    timer.run();

    //for ascii also numberOfBins = 128
    // uint32_t numberOfBins = 256;
    // uint64_t **count = static_cast<uint64_t**>(malloc(Constants::KEY_SIZE * sizeof(uint64_t *)));;
    // atomic<uint64_t> **startOfBin = static_cast< atomic <uint64_t>**>(malloc(Constants::KEY_SIZE * sizeof(uint64_t *)));;
    //
    // for (size_t i = 0; i < Constants::KEY_SIZE; i++) {
    //     count[i] = static_cast<uint64_t*>(malloc(numberOfBins * sizeof(uint64_t)));
    //     startOfBin[i] = static_cast<atomic_uint64_t*>(malloc(numberOfBins * sizeof(uint64_t)));
    //     for (size_t j = 0; j < numberOfBins; j++) {
    //         count[i][j] = 0;
    //         startOfBin[i][j] = 0;
    //     }
    // }
    //
    //
    // for (int i = 0; i < Constants::N_THREADS; ++i) {
    //
    //     uint64_t from = i * segment_size / Constants::TUPLE_SIZE;
    //     uint64_t to   = (i + 1) * segment_size / Constants::TUPLE_SIZE;
    //
    //     cout << "from " << from << " to " << to << endl;
    //
    //     thread_pool.add_task(new CountTask(tuples, from, to, count));
    // }
    //
    // thread_pool.wait_all();
    //
    // for (int i = 0; i < Constants::KEY_SIZE; ++i)
    //     thread_pool.add_task(new BinsRangeTask(count[i], i,startOfBin[i]));

    //

    // // int32_t byteMask = Constants::KEY_SIZE - 1;

    // while(byteMask >= 0){
    //     thread_pool.wait_all();
    //     for (int i = 0; i < Constants::N_THREADS; ++i) {
    //
    //         uint64_t from = i * segment_size / Constants::TUPLE_SIZE;
    //         uint64_t to   = (i + 1) * segment_size / Constants::TUPLE_SIZE;
    //
    //         // cout << "from " << from << " to " << to << endl;
    //
    //         thread_pool.add_task(new RadixSortPassTask(tuples, from, to, byteMask, startOfBin[byteMask], tmp));
    //     }
    //
    //     //wait all to finish theit pass and swap
    //     thread_pool.wait_all();
    //
    //     Tuple *swap = tuples;
    //     tuples = tmp;
    //     tmp = swap;
    //     byteMask--;
    // }
    //
    // thread_pool.wait_all();

    // radixsort(tuples, num_tuples);
    // radixsort2(tuples, num_tuples);
    // RadixQuicksort(tuples, 0, num_tuples - 1, 0);
    thread_pool.add_task(new RadixQuickSortTask(tuples, 0, num_tuples - 1, 0, &thread_pool));

    timer.stop();

    cout << "RadixSort time: " << std::fixed << timer.elapsedMilliseconds() << " ms" << endl;

    free(tuples);
    close(fd);
    return 0;
}
