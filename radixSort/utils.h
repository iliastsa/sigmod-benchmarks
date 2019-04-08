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
    char key[10];
    char payload[90];
    uint32_t rowID;

    // bool operator<(const Tuple &rhs) const {
    //     return memcmp(key,rhs.key,10) < 0;
    // }
};

inline void swap(unsigned char *x, unsigned char* y, uint32_t rowIDx, uint32_t rowIDy){
    unsigned char swap_temp[Constants::KEY_SIZE];
    uint32_t tmp = rowIDx;
    rowIDx = rowIDy;
    rowIDy = tmp;
    memcpy(swap_temp, y, Constants::KEY_SIZE);
    memcpy(y, x, Constants::KEY_SIZE);
    memcpy(x, swap_temp, Constants::KEY_SIZE);


void readFile(uint32_t num_tuples, Tuple *tuples,  uint32_t fd, uint32_t file_size) ;
void radixsort(Tuple *tuples, uint32_t num_tuples);
void radixsort2(Tuple *tuples, uint32_t num_tuples);
void RadixQuicksort(Tuple *tuples, int32_t lo, int32_t hi, int32_t byte);

#endif
