#ifndef SIGMODBENCHMARKS_READREADER_H
#define SIGMODBENCHMARKS_READREADER_H

#include "FileReader.h"

class ReadReader : public FileReader {
public:
    ReadReader(int fd, uint64_t from, uint64_t to, uint64_t chunk_size) :
            FileReader(fd, from, to, chunk_size)
    {
        buffer = static_cast<unsigned char*>(malloc(chunk_size * sizeof(unsigned char)));
    }

    virtual unsigned char* next(uint64_t *sz);

    virtual ~ReadReader();
};


#endif
