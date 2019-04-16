#include <cstdint>

#include "Constants.h"

namespace Constants {
    const int N_CORES = 20;
    const int N_THREADS = 40;
    const uint16_t PAGE_SIZE = 4096;
    const uint16_t PAGE_SHIFT_AMT = 12;

    const uint16_t N_PARTITIONS = 1024;

    const uint64_t KEY_SIZE   = 10;
    const uint64_t TUPLE_SIZE = 100;
          uint32_t CHUNK_SIZE = 0; // If CHUNK_SIZE is 0, the whole thread segment will be read
          uint32_t WRITE_BUFFER_SIZE = 1024 * 256;

    Dataset dataset = UNDEFINED;
}