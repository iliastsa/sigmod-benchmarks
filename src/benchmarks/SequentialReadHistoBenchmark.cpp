#include <ThreadPool.h>
#include <Timer.h>
#include <HistogramTask.h>
#include <ReadReader.h>
#include <MMapReader.h>
#include <ASyncReader.h>
#include <Constants.h>
#include "SequentialReadHistoBenchmark.h"

using namespace std;

SequentialReadHistoBenchmark::SequentialReadHistoBenchmark(char* filename) {
    f_init(filename, &fd, &file_size);
}

void SequentialReadHistoBenchmark::run() {

    auto histogram = static_cast<uint64_t*>(calloc(static_cast<size_t>(Constants::N_PARTITIONS), sizeof(uint64_t)));

    Timer timer;

    timer.run();

    HistogramTask<ASyncReader> task(histogram, fd, 0, file_size, Constants::CHUNK_SIZE);
    task.run();

    timer.stop();

    cout << "Total time: " << std::fixed << timer.elapsedMilliseconds() << " ms" << endl;

    // Verify
    uint64_t sum = 0;
    for (uint64_t i = 0; i < Constants::N_PARTITIONS; ++i)
        sum += histogram[i];

    cout << "Tuples read: " << sum  << endl;

}

