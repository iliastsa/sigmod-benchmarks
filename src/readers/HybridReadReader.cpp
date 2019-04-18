#include "HybridReadReader.h"

HybridReadReader::HybridReadReader(int fd, uint64_t from, uint64_t to, uint64_t chunk_size, unsigned char* buffer)
        : ReadReader(fd, from, to, chunk_size, buffer, false), first_run(true) {}

unsigned char* HybridReadReader::next(uint64_t* sz) {
    if (first_run) {
        *sz = chunk_size;

        first_run = false;
        return buffer;
    }

    return ReadReader::next(sz);
}
