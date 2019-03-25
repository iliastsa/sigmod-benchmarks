#include <ThreadPool.h>
#include <Timer.h>
#include <HistogramTask.h>
#include <ReadReader.h>
#include <MMapReader.h>
#include "SequentialReadHistoBenchmark.h"

using namespace std;

SequentialReadHistoBenchmark::SequentialReadHistoBenchmark(char* filename) {
    f_init(filename, &fd, &file_size);
}

void SequentialReadHistoBenchmark::run() {

    auto histogram = static_cast<uint64_t*>(calloc(static_cast<size_t>(1024), sizeof(uint64_t)));

    Timer timer;

    timer.run();

    HistogramTask<MMapReader> task(histogram, fd, 0, file_size, file_size);
    task.run();

    timer.stop();

    cout << "Total time: " << std::fixed << timer.elapsedMilliseconds() << " ms" << endl;

    // Verify
    uint64_t sum = 0;
    for (uint64_t i = 0; i < 1024; ++i)
        sum += histogram[i];

    cout << "Tuples read: " << sum  << endl;

}

