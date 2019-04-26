#include <Benchmark.h>
#include <SequentialReadHistoBenchmark.h>
#include <ParallelReadHistoBenchmark.h>
#include <Constants.h>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>
#include <sys/mman.h>
#include <iostream>
#include <SingleMMapHistoBenchmark.h>
#include <IOBenchmark.h>
#include <ColumnStoreBenchmark.h>
#include <Timer.h>
#include <climits>
#include <ColumnStoreBWBenchmark.h>
#include <borrowedsort.h>
#include <InMemoryBenchmark.h>
#include <FileInMemColumnStoreBenchmark.h>
#include <MergeBenchmark.h>
#include <BigTuple.h>
#include "BigTupleMergeBenchmark.h"

using namespace std;


int main(int argc, char** argv) {
    Benchmark* bench;

#ifndef SUBMISSION

//    Constants::WRITE_BUFFER_SIZE = 6 * 1024 * 1024;
//    bench = new MergeBenchmark(argv[1], argv[2], get_file_size(argv[1]), Constants::MERGE_CHUNKS, Constants::N_THREADS);
    //Constants::WRITE_BUFFER_SIZE = 6 * 1024 * 1024;
    Constants::WRITE_BUFFER_SIZE = 1024;
    bench = new BigTupleMergeBenchmark(argv[1], argv[2], get_file_size(argv[1]), Constants::MERGE_CHUNKS);


#else

    switch (Constants::dataset = determine_dataset(argv[1])) {
        case Constants::SMALL:
            bench = new FileInMemColumnStoreBenchmark(argv[1], argv[2], Constants::N_THREADS / Constants::N_SOCKETS);
            break;
        case Constants::MEDIUM:
            Constants::WRITE_BUFFER_SIZE = 1024 * 1024;
            bench = new FileInMemColumnStoreBenchmark(argv[1], argv[2], Constants::N_THREADS / Constants::N_SOCKETS);
            break;
        case Constants::LARGE:
            Constants::WRITE_BUFFER_SIZE = 6 * 1024 * 1024;
            bench = new BigTupleMergeBenchmark(argv[1], argv[2], get_file_size(argv[1]), Constants::MERGE_CHUNKS);
            sleep(15);
            break;
        default:
            exit(EXIT_FAILURE);
    }

#endif

    bench->run();

//    ThreadPool t_pool0(20, 0);
//    ThreadPool t_pool1(20, 1);
//
//    unsigned char* mem = static_cast<unsigned char*>(malloc(GB(28)));
//
//
//    Timer t;
//    t.run();
//    for (int i = 1; i < 7; i+=2) {
//        t_pool0.add_task(new Copy1GBTask(mem + i * GB(2)));
//    }
//
//    for (int i = 7; i < 14; i+=2) {
//        t_pool1.add_task(new Copy1GBTask(mem + i * GB(2)));
//    }
//
//    t_pool0.wait_all();
//    t_pool1.wait_all();
//
//    t.stop();
//
//    cout << "Time to copy 7 GB : " << fixed << t.elapsedMilliseconds() << " ms" << endl;

//    unsigned char* mem = static_cast<unsigned char*>(malloc(GB(30)));
//
//    for (uint64_t i = 0; i < GB(30); i+= 100) {
//        mem[i] = 5;
//    }

    return 0;
}