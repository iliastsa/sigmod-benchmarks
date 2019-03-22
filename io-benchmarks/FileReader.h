#ifndef SIGMODBENCHMARKS_FILEREADER_H
#define SIGMODBENCHMARKS_FILEREADER_H

#include <string>
#include <stdint-gcc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

class FileReader {
protected:
    int fd;

    uint64_t from;
    uint64_t to;
    uint64_t chunk_size;

    unsigned char *buffer;

public:
    FileReader(int fd, uint64_t from, uint64_t to, uint64_t chunk_size) :
            fd(fd), from(from), to(to), chunk_size(chunk_size), buffer(nullptr) {}

    virtual ~FileReader() {};

    virtual unsigned char *next(uint64_t *sz) {return nullptr;};
};

#endif
