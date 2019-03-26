#include <sys/mman.h>
#include <unistd.h>
#include <Constants.h>

#include "MMapReader.h"

MMapReader::MMapReader(int fd, uint64_t from, uint64_t to, uint64_t chunk_size) :
    FileReader(fd, from, to, chunk_size) {}

uint64_t MMapReader::get_page_num(uint64_t num) const {
    return num >> Constants::PAGE_SHIFT_AMT;
}

unsigned char* MMapReader::next(uint64_t* sz){
    uint64_t read_amt = to - from < chunk_size ? to - from : chunk_size;

    munmap(buffer, map_size);

    uint64_t page_offset = get_page_num(from) * Constants::PAGE_SIZE;
    uint64_t adjust = from - page_offset;

    buffer = static_cast<unsigned char*>(mmap(buffer, read_amt + adjust, PROT_READ, MAP_PRIVATE, fd, page_offset));

    from += read_amt;
    map_size = read_amt + adjust;
    *sz = read_amt;

    return read_amt == 0 ? nullptr : buffer + adjust;
}

MMapReader::~MMapReader() {
    munmap(buffer, map_size);
    close(fd);
}
