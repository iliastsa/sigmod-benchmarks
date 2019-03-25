#ifndef SIGMODBENCHMARKS_CONSTANTS_H
#define SIGMODBENCHMARKS_CONSTANTS_H

#include <cstdint>

namespace Constants {
    const int N_THREADS = 40;

    const uint16_t N_PARTITIONS = 1024;

    const uint16_t TUPLE_SIZE = 100;
    const uint64_t CHUNK_SIZE = 4000; // If CHUNK_SIZE is 0, the whole thread segment will be read
}

#endif
