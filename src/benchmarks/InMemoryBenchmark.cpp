
#include <Utils.h>
#include <Timer.h>
#include <sys/mman.h>
#include <borrowedsort.h>
#include <Constants.h>
#include <unistd.h>
#include <fcntl.h>
#include <BigTuple.h>
#include "InMemoryBenchmark.h"


InMemoryBenchmark::InMemoryBenchmark(const char* filename, const char* out_filename)
    : out_filename(out_filename)
{
    f_init(filename, &fd, &file_size);
}


void InMemoryBenchmark::run() {
    Timer timer;

    timer.run();

    const uint64_t num_tuples = file_size / Constants::TUPLE_SIZE;

    BigTuple* tuples = static_cast<BigTuple*> (mmap(nullptr, file_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_POPULATE, fd, 0));

    parasort(num_tuples, tuples, Constants::N_THREADS);

    timer.stop();

    std::cout << "Mixed sort IO time : " << timer.elapsedMilliseconds() << " ms" << std::endl;

    timer.run();

    int out_fd = open(out_filename, O_CREAT | O_WRONLY, 0600);


    uint64_t bytes_written = 0;
    unsigned char* buf = reinterpret_cast<unsigned char*> (tuples);
    while(bytes_written < file_size) {
        bytes_written += pwrite(out_fd, buf + bytes_written, file_size - bytes_written, bytes_written);
    }

    close(out_fd);

    timer.stop();

    std::cout << "Write time : " << timer.elapsedMilliseconds() << " ms" << std::endl;
}


