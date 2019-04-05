#ifndef COUNTTASK_H
#define COUNTTASK_H

#include "ThreadPool.h"
#include <cstdint>
#include "Constants.h"

class CountTask : public ThreadPool::Task {
private:
    Tuple *tuples;
    uint64_t from;
    uint64_t to;
    uint64_t **count;

public:
    CountTask(Tuple *tuples, uint64_t from, uint64_t to, uint64_t **count)
        : tuples(tuples), from(from), to(to), count(count){}


    void run() {

        for (uint32_t j = from; j < to; j++)
            for (uint32_t i = 0; i < Constants::KEY_SIZE; i++)
                count[i][tuples[j].key[i]]++;
    }
};

#endif
