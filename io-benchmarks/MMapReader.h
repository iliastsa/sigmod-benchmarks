#ifndef SIGMODBENCHMARKS_MMAPREADER_H
#define SIGMODBENCHMARKS_MMAPREADER_H


#include "FileReader.h"

class MMapReader : public FileReader {
    typedef std::string string;

    public:
    MMapReader(string filename, uint64_t chunk_size = 0) : FileReader(filename, chunk_size) {}

    virtual unsigned char* read_file();

    virtual ~MMapReader();

private:
    void *mem;
};


#endif
