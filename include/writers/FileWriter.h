#ifndef SIGMODBENCHMARKS_FILEWRITER_H
#define SIGMODBENCHMARKS_FILEWRITER_H


#include <cstdint>

class FileWriter {
protected:
    int fd;

    uint64_t from;
    uint64_t to;
    uint64_t chunk_size;

    unsigned char *buffer;
public:
    FileWriter(int fd, uint64_t from, uint64_t to, unsigned char *buffer, uint64_t chunk_size) :
            fd(fd), from(from), to(to), buffer(buffer)
    {
        if (chunk_size == 0)
            this->chunk_size = to - from;
        else
            this->chunk_size = chunk_size;
    }

    virtual ~FileWriter() = default;

    virtual void flush() = 0;
};


#endif
