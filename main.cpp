#include <iostream>
#include <stdlib.h>
#include "io-benchmarks/FileReader.h"
#include "io-benchmarks/MMapReader.h"
#include "Timer.h"
#include "io-benchmarks/ReadReader.h"

#define TUPLE_SIZE 100

using namespace std;

// 2^10 partitions.
const std::uint16_t kNumPartitionBits = 10u;
constexpr std::uint16_t kShift = kNumPartitionBits - sizeof(char) * 8;
constexpr std::uint16_t kLowMask = (1u << kShift) - 1u;
constexpr std::uint16_t kMask = (1u << kNumPartitionBits) - 1u;

uint64_t get_partition(const unsigned char *key) {
    uint16_t pid = (*key) << kShift;
    pid += *(key + 1) & kLowMask;
    pid &= kMask;
    return pid;
}

void histogram(unsigned char *buf, uint64_t size) {
    uint64_t *hist = static_cast<uint64_t*>(calloc(1, 1024));

    for (uint64_t i = 0; i < size; i += TUPLE_SIZE) {
        uint64_t p = get_partition(&buf[i]);
        hist[p]++;
    }
}

int main() {
//    FileReader *fr = new MMapReader("test.txt");
    FileReader *fr = new ReadReader("test.txt");

    Timer timer;

    timer.run();

    unsigned char* mem = fr->read_file();
    uint64_t sz = fr->getFile_size();


    histogram(mem, sz);

    timer.stop();

    cout << "IO time: " << std::fixed << timer.elapsedMilliseconds() << " ms" << endl;

    return 0;
}