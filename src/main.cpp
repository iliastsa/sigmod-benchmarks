#include <iostream>
#include <cstdlib>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <Benchmark.h>
#include <ParallelReadHistoBenchmark.h>
#include <aio.h>

#include "FileReader.h"
#include "MMapReader.h"
#include "Timer.h"
#include "ReadReader.h"
#include "Partition.h"
#include "Utils.h"

using namespace std;

int main(int argc, char **argv) {
//    if (argc != 2) {
//        cout << "No file provided" << endl;
//        exit(EXIT_FAILURE);
//    }

//    int fd;
//    uint64_t file_size;
//
//    f_init(argv[1], &fd, &file_size);
//
//    // Note: Chunk size must be a multiple of 100z`
//    //FileReader *fr = new MMapReader(fd, 0, file_size, file_size);
//    FileReader *fr = new ReadReader(fd, 0 ,file_size, file_size);
//
//    Partition partition;
//
//    Timer timer;
//
//    timer.run();
//
//    uint64_t sz = 0;
//    unsigned char* mem;
//
//    while ((mem = fr->next(&sz)) != nullptr)
//        partition.update_histogram(mem, sz);
//
//    timer.stop();
//
//    uint64_t *hist = partition.get_histogram();
//
//    cout << "IO time: " << std::fixed << timer.elapsedMilliseconds() << " ms" << endl;
//
//    uint64_t sum = 0;
//    for (uint64_t i = 0; i < 1024; ++i)
//        sum += hist[i];
//
//    cout << "Tuples read: " << sum  << endl;

    Benchmark *benchmark = new ParallelReadHistoBenchmark(argv[1], 32);
    benchmark->run();

    return 0;
}