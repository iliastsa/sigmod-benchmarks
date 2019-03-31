#include <unistd.h>
#include <cstdlib>
#include <fcntl.h>
#include <Utils.h>
#include <cstring>
#include <cerrno>
#include <iostream>

#include "ReadReader.h"

ReadReader::ReadReader(int fd, uint64_t from, uint64_t to, uint64_t chunk_size)
        : FileReader(fd, from, to, chunk_size), shared_buffer(false)
{
    buffer = static_cast<unsigned char*>(malloc(this->chunk_size * sizeof(unsigned char)));
}

ReadReader::ReadReader(int fd, uint64_t from, uint64_t to, uint64_t chunk_size, unsigned char* buffer)
        : FileReader(fd, from, to, chunk_size), shared_buffer(true)
{
    this->buffer = buffer;
}


ReadReader::~ReadReader() {
    if (!shared_buffer)
        free(buffer);
    close(fd);
}

unsigned char* ReadReader::next(uint64_t *sz) {
    uint64_t bytes_read = 0;

    uint64_t read_amt = to - from < chunk_size ? to - from : chunk_size;


    while (bytes_read < read_amt)
        bytes_read += pread(fd, buffer + bytes_read, read_amt - bytes_read, from + bytes_read);

    *sz = read_amt;
    from += bytes_read;

    unsigned char* ret_buf = buffer;
    if(shared_buffer)
        buffer += bytes_read;

    return read_amt == 0 ? nullptr : ret_buf;
}
