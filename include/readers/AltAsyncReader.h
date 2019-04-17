#ifndef SIGMODBENCHMARKS_ALTASYNCREADER_H
#define SIGMODBENCHMARKS_ALTASYNCREADER_H

#include <cstdlib>
#include <aio.h>
#include "FileReader.h"

class AltAsyncReader : public FileReader {
private:
    unsigned char *async_buffer;
    struct aiocb   async_handle;

    unsigned char* blocking_next(uint64_t *sz, unsigned char* buffer);

public:
    AltAsyncReader(int fd, uint64_t from, uint64_t to, uint64_t chunk_size) :
            FileReader(fd, from, to, chunk_size)
    {
        // Setup async handle
        async_handle.aio_fildes   = fd;
        async_handle.aio_offset   = from + chunk_size; // First read is sync
        async_handle.aio_buf      = async_buffer;
        async_handle.aio_nbytes   = chunk_size;
        async_handle.aio_reqprio  = 0;
        async_handle.aio_sigevent.sigev_notify = SIGEV_NONE;

        // Allocate buffers
        buffer = static_cast<unsigned char*>(malloc(chunk_size * sizeof(unsigned char)));
        async_buffer = nullptr;
    }

    virtual unsigned char* next(uint64_t *sz);

    virtual ~AltAsyncReader();
};

#endif
