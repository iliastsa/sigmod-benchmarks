//
// Created by Ilias on 22-Mar-19.
//

#ifndef SIGMODBENCHMARKS_PARTITION_H
#define SIGMODBENCHMARKS_PARTITION_H


#include <stdint-gcc.h>
#include <cstdlib>

class Partition {
private:
    uint64_t *histogram;

    uint16_t get_partition(const unsigned char *key) {
        const uint16_t partition_bits = 10u;
        constexpr uint16_t shift_amt = partition_bits - sizeof(char) * 8;
        constexpr uint16_t low_mask = (1u << shift_amt) - 1u;
        constexpr uint16_t mask = (1u << partition_bits) - 1u;

        uint16_t pid = (*key) << shift_amt;
        pid += *(key + 1) & low_mask;
        pid &= mask;

        return pid;
    }

public:
    Partition();
    ~Partition();

    uint64_t* get_histogram() const;

    void update_histogram(unsigned char* buf, uint64_t size);
};


#endif
