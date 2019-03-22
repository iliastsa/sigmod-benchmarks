#include <sys/mman.h>

#include "MMapReader.h"


unsigned char* MMapReader::read_file() {
    return static_cast<unsigned char*>(mem = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0));
}

MMapReader::~MMapReader() {
    munmap(mem, file_size);
}
