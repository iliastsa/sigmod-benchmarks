#ifndef SIGMODBENCHMARKS_CONSTANTS_H
#define SIGMODBENCHMARKS_CONSTANTS_H

#include <cstdint>

namespace Constants {
    extern const int N_CORES;
    extern const int N_SOCKETS;
    extern const int N_THREADS;
    extern const uint32_t IO_THREADS;
    extern const uint32_t MEM_THREADS;

    extern const uint64_t MERGE_BUFFER_SIZE;
    extern const uint64_t MERGE_CHUNKS;

    extern const uint64_t MEM_ALLOC;
    extern const uint64_t MEM_LIMIT;

    extern const uint16_t PAGE_SIZE;
    extern const uint16_t PAGE_SHIFT_AMT;

    extern const uint16_t N_PARTITIONS;

    extern const uint64_t KEY_SIZE;
    extern const uint64_t TUPLE_SIZE;
    extern       uint32_t CHUNK_SIZE;
    extern       uint32_t WRITE_BUFFER_SIZE;

    enum Dataset {
        UNDEFINED,
        SMALL,
        MEDIUM,
        LARGE
    };

    extern Dataset dataset;
}

#endif
