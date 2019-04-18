#ifndef SIGMODBENCHMARKS_READREADER_H
#define SIGMODBENCHMARKS_READREADER_H

#include <cstdlib>
#include "FileReader.h"

class ReadReader : public FileReader {
private:
    bool shared_buffer;
    bool append_buffer;

public:
    ReadReader(int fd, uint64_t from, uint64_t to, uint64_t chunk_size);
    ReadReader(int fd, uint64_t from, uint64_t to, uint64_t chunk_size, unsigned char* buffer, bool append_buffer = true);

    virtual unsigned char* next(uint64_t *sz);

    virtual ~ReadReader();
};


#endif
