#ifndef RADIXSORTPASSTASK_H
#define RADIXSORTPASSTASK_H

#include "ThreadPool.h"
#include <cstdint>
#include "Constants.h"

class RadixSortPassTask : public ThreadPool::Task {
private:
    Tuple *tuples;
    uint64_t from;
    uint64_t to;
    uint32_t digit;
    atomic<uint64_t> *startOfBin;
    Tuple *outputArray;

public:
    RadixSortPassTask(Tuple *tuples, uint64_t from, uint64_t to, uint32_t digit, atomic<uint64_t> *startOfBin, Tuple *outputArray)
        : tuples(tuples), from(from), to(to), digit(digit), startOfBin(startOfBin), outputArray(outputArray){}


    void run() {

        for (uint32_t current = from; current < to; current++){

            uint32_t endOfBinIndex = tuples[current].key[digit];

            // cout << endOfBinIndex << endl;foo.load(std::memory_order_relaxed);
            uint32_t index = startOfBin[endOfBinIndex].load(memory_order_seq_cst);
            startOfBin[endOfBinIndex] += 1;
            // cout << index << " ";
            memcpy(outputArray[index].key, tuples[current].key, Constants::KEY_SIZE);
            // memcpy(outputArray[index].payload, tuples[current].payload, Constants::PAYLOAD);
            outputArray[index].rowID = tuples[current].rowID;

        }
        // cout << endl;
    }
};

#endif
