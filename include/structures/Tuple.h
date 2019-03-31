
#ifndef SIGMODBENCHMARKS_TUPLE_H
#define SIGMODBENCHMARKS_TUPLE_H

#include <cstdint>
#include <cstring>

struct Tuple {
    unsigned char key[10];
    int32_t rowID;

    bool operator<(const Tuple &rhs) const {
        return memcmp(key,rhs.key,Constants::KEY_SIZE) < 0;
    }
};
#endif
