#ifndef SIGMODBENCHMARKS_ASYNCREADER_H
#define SIGMODBENCHMARKS_ASYNCREADER_H

#include <cstdlib>


#include <cerrno>
#include <cstring>
#include "FileReader.h"

#include "Utils.h"
#include "AsyncUtils.h"

#include <linux/aio_abi.h>

class ASyncReader : public FileReader {
private:
    unsigned char *async_buffer;

    aio_context_t ctx;

    struct iocb     async_handle;
    struct iocb     *async_h_arr[1];

    struct io_event async_events[1];

    bool shared_buffer;

    unsigned char* blocking_next(uint64_t *sz, unsigned char* buffer);

public:
    ASyncReader(int fd, uint64_t from, uint64_t to, uint64_t chunk_size, unsigned char *buffer);
    ASyncReader(int fd, uint64_t from, uint64_t to, uint64_t chunk_size);

    virtual unsigned char* next(uint64_t *sz);

    virtual ~ASyncReader();
};
#endif
