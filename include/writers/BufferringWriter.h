
#ifndef SIGMODBENCHMARKS_BUFFERRINGWRITER_H
#define SIGMODBENCHMARKS_BUFFERRINGWRITER_H


#include "FileWriter.h"

class BufferringWriter : public FileWriter {

private:
    unsigned char* buffer;

    uint64_t offset;

    uint64_t capacity;
    uint64_t current_size;

    void flush();
public:
    BufferringWriter(int fd, uint64_t offset, uint64_t capacity);

    ~BufferringWriter() final;

    // Size should always be <= capacity
    void write(unsigned char *buf, uint64_t size) override;

};




#endif
