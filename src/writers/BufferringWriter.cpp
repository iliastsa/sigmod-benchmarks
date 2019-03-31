
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <Utils.h>
#include <cerrno>
#include "BufferringWriter.h"

BufferringWriter::BufferringWriter(int fd, uint64_t offset, uint64_t capacity)
        : FileWriter(fd), offset(offset), capacity(capacity), current_size(0)
{
    buffer = static_cast<unsigned char *>(malloc(capacity * sizeof(unsigned char)));
}

BufferringWriter::~BufferringWriter() {
    flush();
    free(buffer);
}

void BufferringWriter::flush() {
    uint64_t bytes_written = 0;

    while(bytes_written < current_size) {
        ssize_t bytes = pwrite(fd, buffer + bytes_written, current_size - bytes_written, offset + bytes_written);

        if (bytes < 0)
            P_ERR("Bad write!", errno);

       bytes_written += bytes;
    }

    offset += current_size;
    current_size = 0;
}

void BufferringWriter::write(unsigned char *buf, uint64_t size) {
    if (current_size + size > capacity)
        flush();

    memcpy(buffer + current_size, buf, size);
    current_size += size;
}
