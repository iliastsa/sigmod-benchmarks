#include <iostream>
#include <stdlib.h>
#include <sys/mman.h>
#include "io-benchmarks/FileReader.h"
#include "io-benchmarks/MMapReader.h"
#include "Timer.h"
#include "io-benchmarks/ReadReader.h"
#include "partition/Partition.h"

using namespace std;

void f_init(const char *file_name, int *fd, uint64_t *file_size) {
    struct stat sb;

    if (stat(file_name, &sb) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    cout << "Preferred block size: " << sb.st_blksize << endl;

    // TODO: Check prefered IO block size: https://linux.die.net/man/2/fstat
    *fd = open(file_name, O_RDONLY);
    *file_size = static_cast<uint64_t>(sb.st_size);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        cout << "No file provided" << endl;
        exit(EXIT_FAILURE);
    }

    int fd;
    uint64_t file_size;

    f_init(argv[1], &fd, &file_size);

    // Note: Chunk size must be a multiple of 100z`
    //FileReader *fr = new MMapReader(fd, 0, file_size, file_size);
    FileReader *fr = new ReadReader(fd, 0 ,file_size, 4000);

    Partition partition;

    Timer timer;

    timer.run();

    uint64_t sz = 0;
    unsigned char* mem;

    while ((mem = fr->next(&sz)) != nullptr)
        partition.update_histogram(mem, sz);

    timer.stop();

    uint64_t *hist = partition.get_histogram();

    cout << "IO time: " << std::fixed << timer.elapsedMilliseconds() << " ms" << endl;

    uint64_t sum = 0;
    for (uint64_t i = 0; i < 1024; ++i)
        sum += hist[i];

    cout << "Tuples read: " << sum  << endl;

    return 0;
}