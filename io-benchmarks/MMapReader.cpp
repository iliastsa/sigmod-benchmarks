#include <sys/mman.h>

#include "MMapReader.h"


unsigned char* MMapReader::next(uint64_t* sz){
    uint64_t read_amt = to - from < chunk_size ? to - from : chunk_size;

    munmap(buffer, map_size);
    buffer = static_cast<unsigned char*>(mmap(buffer, read_amt, PROT_READ, MAP_PRIVATE, fd, 0));

    from += read_amt;
    map_size = read_amt;
    *sz = read_amt;

    return read_amt == 0 ? nullptr : buffer;
}

MMapReader::~MMapReader() {
    munmap(buffer, map_size);
}
