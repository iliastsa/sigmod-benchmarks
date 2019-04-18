#include <cstdint>

#include "Utils.h"
#include "Constants.h"

#ifdef SUBMISSION

namespace Constants {
    const int N_CORES = 20;
    const int N_THREADS = 40;
    const uint32_t IO_THREADS = 20;
    const uint32_t MEM_THREADS = 20;

    extern const uint64_t MERGE_BUFFER_SIZE = GB(3);

    const uint16_t PAGE_SIZE = 4096;
    const uint16_t PAGE_SHIFT_AMT = 12;

    const uint16_t N_PARTITIONS = 1024;

    const uint64_t KEY_SIZE   = 10;
    const uint64_t TUPLE_SIZE = 100;
    uint32_t CHUNK_SIZE = 0; // If CHUNK_SIZE is 0, the whole thread segment will be read
    uint32_t WRITE_BUFFER_SIZE = 1024 * 256;

    Dataset dataset = UNDEFINED;
}

#else

namespace Constants {
    const int N_CORES = 4;
    const int N_THREADS = 4;
    const uint32_t IO_THREADS = 2;
    const uint32_t MEM_THREADS = 2;

    extern const uint64_t MERGE_BUFFER_SIZE = MB(300);

    const uint16_t PAGE_SIZE = 4096;
    const uint16_t PAGE_SHIFT_AMT = 12;

    const uint16_t N_PARTITIONS = 1024;

    const uint64_t KEY_SIZE   = 10;
    const uint64_t TUPLE_SIZE = 100;
    uint32_t CHUNK_SIZE = 0; // If CHUNK_SIZE is 0, the whole thread segment will be read
    uint32_t WRITE_BUFFER_SIZE = 1024 * 256;

    Dataset dataset = UNDEFINED;
}

#endif