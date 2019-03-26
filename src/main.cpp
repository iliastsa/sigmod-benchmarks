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
#include <WriteBenchmark.h>

using namespace std;

int main(int argc, char **argv) {
//    Benchmark *parallel_bench = new ParallelReadHistoBenchmark(argv[1], Constants::N_THREADS);
//    parallel_bench->run();
//    delete parallel_bench;

    uint64_t fs = 10000000000;

    Benchmark *parallel_bench = new WriteBenchmark(argv[2], fs, Constants::N_THREADS);
    parallel_bench->run();
    delete parallel_bench;

//    Benchmark *parallel_bench = new SingleMMapHistoBenchmark(argv[1], Constants::N_THREADS);
//    parallel_bench->run();
//    delete parallel_bench;

//    int fd = open("test_file", O_RDWR | O_CREAT, (mode_t)0600);
//    const char *text = "hello";
//    size_t textsize = strlen(text) + 1;
//
//    lseek(fd, textsize-1, SEEK_SET);
//    write(fd, "", 1);
//
//    void *map = mmap(0, textsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
//
//    memcpy(map, text, strlen(text));
//    msync(map, textsize, MS_SYNC);
//
//    munmap(map, textsize);
//    close(fd);
//
//    return 0;

//    Benchmark *sequential_bench = new SequentialReadHistoBenchmark(argv[1]);
//    sequential_bench->run();
//    delete sequential_bench;

    return 0;
}