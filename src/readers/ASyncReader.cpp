#include <unistd.h>
#include <Utils.h>
#include "ASyncReader.h"

ASyncReader::ASyncReader(int fd, uint64_t from, uint64_t to, uint64_t chunk_size, unsigned char *buffer)
        : FileReader(fd, from, to, chunk_size), ctx(0), shared_buffer(true)
{
    this->buffer = buffer;

    io_setup(128, &ctx);

    // Setup async handle
    memset(&async_handle, 0, sizeof(async_handle));
    async_handle.aio_fildes   = (uint32_t) fd;
    async_handle.aio_lio_opcode = IOCB_CMD_PREAD;

    async_handle.aio_buf      = (uint64_t) async_buffer;
    async_handle.aio_offset   = (int64_t) (from + chunk_size); // First read is sync
    async_handle.aio_nbytes   = chunk_size;

    async_h_arr[0] = &async_handle;

    // Allocate buffers
    async_buffer = nullptr;
}

ASyncReader::ASyncReader(int fd, uint64_t from, uint64_t to, uint64_t chunk_size)
        : FileReader(fd, from, to, chunk_size), ctx(0), shared_buffer(false)
{
    io_setup(128, &ctx);

    // Setup async handle
    memset(&async_handle, 0, sizeof(async_handle));
    async_handle.aio_fildes   = (uint32_t) fd;
    async_handle.aio_lio_opcode = IOCB_CMD_PREAD;

    async_handle.aio_buf      = (uint64_t) async_buffer;
    async_handle.aio_offset   = (int64_t) (from + chunk_size); // First read is sync
    async_handle.aio_nbytes   = chunk_size;

    async_h_arr[0] = &async_handle;

    // Allocate buffers
    buffer = static_cast<unsigned char*>(malloc(chunk_size * sizeof(unsigned char)));
    async_buffer = nullptr;
}

unsigned char* ASyncReader::next(uint64_t* sz) {
    if (from == to)
        return nullptr;

    if (async_buffer == nullptr) {
        async_buffer = shared_buffer ? buffer + chunk_size : static_cast<unsigned char*>(malloc(chunk_size * sizeof(unsigned char)));

        unsigned char* ret = blocking_next(sz, buffer);

        // async
        async_handle.aio_buf = (uint64_t) async_buffer;
        io_submit(ctx, 1, async_h_arr);

        return ret;
    }

    // suspend
    io_getevents(ctx, 1, 1, async_events, NULL);

    // swap
    if (shared_buffer){
      buffer += chunk_size;
      async_buffer += chunk_size;
    } else {
        unsigned char* temp_buffer = buffer;
        buffer = async_buffer;
        async_buffer = temp_buffer;
    }

    from += chunk_size;

    if (from == to)
        return buffer;

    async_handle.aio_buf    = (uint64_t) async_buffer;
    async_handle.aio_nbytes = chunk_size;
    async_handle.aio_offset = (int64_t) from;

    *sz = chunk_size;

    // begin next async if available
    io_submit(ctx, 1, async_h_arr);

    return buffer;
}

unsigned char* ASyncReader::blocking_next(uint64_t* sz, unsigned char *buffer) {
    uint64_t bytes_read = 0;

    uint64_t read_amt = to - from < chunk_size ? to - from : chunk_size;

    while (bytes_read < read_amt) {
        bytes_read += pread(fd, buffer + bytes_read, read_amt - bytes_read, from + bytes_read);
    }
    *sz = read_amt;
    from += bytes_read;

    return read_amt == 0 ? nullptr : buffer;
}

ASyncReader::~ASyncReader() {
    if (!shared_buffer) {
        free(buffer);
        free(async_buffer);
    }
}
