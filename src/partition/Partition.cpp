#include <Constants.h>
#include "Partition.h"

Partition::Partition() : owner(true) {
    histogram = static_cast<uint64_t*>(calloc(Constants::N_PARTITIONS, sizeof(uint64_t)));
}

Partition::Partition(uint64_t* histogram) : owner(false), histogram(histogram) {}

uint64_t* Partition::get_histogram() const {
    return histogram;
}

void Partition::update_histogram(unsigned char* buf, uint64_t size) {
    for (uint64_t i = 0; i < size; i += Constants::TUPLE_SIZE) {
        uint64_t p = get_partition(&buf[i]);
        histogram[p]++;
    }
}

Partition::~Partition() {
    if (owner)
        free(histogram);
}