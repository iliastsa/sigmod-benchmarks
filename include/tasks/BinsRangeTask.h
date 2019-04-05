#ifndef BINSRANGE_H
#define BINSRANGE_H

#include "ThreadPool.h"
#include <cstdint>
#include "Constants.h"

class BinsRangeTask : public ThreadPool::Task {
private:
    uint64_t *count;
    atomic<uint64_t> *startOfBin;

public:
    BinsRangeTask(uint64_t *count, uint32_t digit, atomic<uint64_t> *startOfBin)
        : count(count), startOfBin(startOfBin){}


    void run() {
        startOfBin[0] = 0;
        for (uint32_t j = 1; j < 128; j++)
            startOfBin[j] = startOfBin[j-1] + count[j-1];

    }
};

#endif
