//
// Created by Ilias on 22-Mar-19.
//

#include "Partition.h"

Partition::Partition(){
    histogram = static_cast<uint64_t*>(calloc(1024, sizeof(uint64_t)));
}

uint64_t* Partition::get_histogram() const {
    return histogram;
}

void Partition::update_histogram(unsigned char* buf, uint64_t size) {
    for (uint64_t i = 0; i < size; i += 100) {
        uint64_t p = get_partition(&buf[i]);
        histogram[p]++;
    }
}

Partition::~Partition() {
    free(histogram);
}
