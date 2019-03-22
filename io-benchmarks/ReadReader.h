#ifndef SIGMODBENCHMARKS_READREADER_H
#define SIGMODBENCHMARKS_READREADER_H


#include "FileReader.h"

class ReadReader : public FileReader {
    typedef std::string string;

public:
    ReadReader(string filename, uint64_t chunk_size = 0) : FileReader(filename, chunk_size) {}

    virtual unsigned char* read_file();

    virtual ~ReadReader();
};


#endif
