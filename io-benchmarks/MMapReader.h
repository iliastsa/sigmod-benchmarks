#ifndef SIGMODBENCHMARKS_MMAPREADER_H
#define SIGMODBENCHMARKS_MMAPREADER_H


#include "FileReader.h"

class MMapReader : public FileReader {
private:
    uint64_t map_size;

public:
    MMapReader(int fd, uint64_t from, uint64_t to, uint64_t chunk_size) :
            FileReader(fd, from, to, chunk_size), map_size(0) {}

    virtual unsigned char* next(uint64_t *sz);

    virtual ~MMapReader();
};


#endif
