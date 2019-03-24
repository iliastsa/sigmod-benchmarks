#include <unistd.h>
#include <cstdlib>

#include "ReadReader.h"

ReadReader::~ReadReader() {
    free(buffer);
}

unsigned char* ReadReader::next(uint64_t *sz) {
    uint64_t bytes_read = 0;

    uint64_t read_amt = to - from < chunk_size ? to - from : chunk_size;

    while (bytes_read < read_amt) {
        bytes_read += pread(fd, buffer + bytes_read, read_amt, from + bytes_read);
    }
    *sz = read_amt;
    from += bytes_read;

    return read_amt == 0 ? nullptr : buffer;
}
