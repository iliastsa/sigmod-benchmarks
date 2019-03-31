
#ifndef SIGMODBENCHMARKS_BIGTUPLE_H
#define SIGMODBENCHMARKS_BIGTUPLE_H

#include <cstdint>
#include <cstring>
#include <Constants.h>

struct BigTuple {
    unsigned char key[10];
    unsigned char payload[90];

    bool operator<(const BigTuple &rhs) const {
        return memcmp(key, rhs.key, Constants::KEY_SIZE) < 0;
    }
};

#endif