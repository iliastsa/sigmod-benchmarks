#include <unistd.h>
#include <Utils.h>
#include "AltAsyncReader.h"

unsigned char* AltAsyncReader::next(uint64_t* sz) {
    if (from == to)
        return nullptr;

    if (async_buffer == nullptr) {
        async_buffer = static_cast<unsigned char*>(malloc(chunk_size * sizeof(unsigned char)));

        unsigned char* ret = blocking_next(sz, buffer);

        // async
        async_handle.aio_buf = async_buffer;
        aio_read(&async_handle);

        return ret;
    }

    // suspend
    struct aiocb* temp = &async_handle;
    aio_suspend(&temp, 1, nullptr);

    // swap
    unsigned char* temp_buffer = buffer;
    buffer = async_buffer;
    async_buffer = temp_buffer;

    from += chunk_size;

    if (from == to)
        return buffer;

    async_handle.aio_buf    = async_buffer;
    async_handle.aio_nbytes = chunk_size;
    async_handle.aio_offset = from;

    *sz = chunk_size;

    // begin next async if available
    aio_read(&async_handle);

    return buffer;
}

unsigned char* AltAsyncReader::blocking_next(uint64_t* sz, unsigned char *buffer) {
    uint64_t bytes_read = 0;

    uint64_t read_amt = to - from < chunk_size ? to - from : chunk_size;

    while (bytes_read < read_amt) {
        bytes_read += pread(fd, buffer + bytes_read, read_amt - bytes_read, from + bytes_read);
    }
    *sz = read_amt;
    from += bytes_read;

    return read_amt == 0 ? nullptr : buffer;
}

AltAsyncReader::~AltAsyncReader() {
    free(buffer);
    free(async_buffer);
}
