#ifndef SIGMODBENCHMARKS_FILEWRITER_H
#define SIGMODBENCHMARKS_FILEWRITER_H


#include <cstdint>
#include <unistd.h>
#include <Constants.h>

class FileWriter {
protected:
    int fd;
    bool owner;

public:
    FileWriter(int fd, bool owner = false) : fd(fd) {}

    virtual ~FileWriter() {
        if (owner)
            close(fd);
    }

    virtual void write(unsigned char *buf, uint64_t size) = 0;
};


#endif
