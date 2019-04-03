#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>


using namespace std;

struct Tuple {
    unsigned char *key;
    unsigned char *payload;
    uint32_t rowID;

    // bool operator<(const Tuple &rhs) const {
    //     return memcmp(key,rhs.key,10) < 0;
    // }
};

void readFile(uint32_t num_tuples, Tuple *tuples,  uint32_t fd, uint32_t file_size) ;
void radixsort(Tuple *tuples, uint32_t num_tuples);
void radixsort2(Tuple *tuples, uint32_t num_tuples);

#endif
