#ifndef SIGMODBENCHMARKS_HYBRIDREADREADER_H
#define SIGMODBENCHMARKS_HYBRIDREADREADER_H

#include "FileReader.h"
#include "ReadReader.h"

class HybridReadReader : public ReadReader {
private:
    bool first_run;

public:
    HybridReadReader(int fd, uint64_t from, uint64_t to, uint64_t chunk_size, unsigned char* buffer);

    unsigned char* next(uint64_t* sz) override;
};


#endif
