#ifndef RADIXQUICKSORT_H
#define RADIXQUICKSORT_H

#include "ThreadPool.h"
#include <cstdint>
#include "Constants.h"

class RadixQuickSortTask : public ThreadPool::Task {
private:
    Tuple *tuples;
    int64_t lo;
    int64_t hi;
    uint32_t digit;
    ThreadPool *thread_pool;

public:
    RadixQuickSortTask(Tuple *tuples, int64_t lo, int64_t hi, uint32_t digit, ThreadPool *thread_pool)
        : tuples(tuples), lo(lo), hi(hi), digit(digit), thread_pool(thread_pool){}


    void run() {
        if (lo >= hi)
            return;

        int64_t low = lo;
        int64_t high = hi;
        int32_t char_index = tuples[low].key[digit];
        int64_t index = low + 1;
        int32_t  test_char;

        while(index <= high){

            test_char = tuples[index].key[digit];
            // cout << "Test " << test_char << " Index " << index;
            if(test_char < char_index){
                // cout << endl;
                unsigned char tmp[11];
                // unsigned char tmp_payload[90];

                memcpy(tmp, tuples[low].key, Constants::KEY_SIZE);
                // memcpy(tmp_payload, tuples[low].payload, Constants::PAYLOAD);

                memcpy(tuples[low].key, tuples[index].key, Constants::KEY_SIZE);
                // memcpy(tuples[low].payload, tuples[index].payload, Constants::PAYLOAD);

                memcpy(tuples[index].key, tmp, Constants::KEY_SIZE);
                // memcpy(tuples[index].payload, tmp_payload, Constants::PAYLOAD);
                low++;
                index++;

            }
            else if(test_char > char_index){
                 // cout << " megalutero " << high << endl;
                unsigned char tmp[11];
                // unsigned char tmp_payload[90];
                memcpy(tmp, tuples[high].key, Constants::KEY_SIZE);
                // memcpy(tmp_payload, tuples[high].payload, Constants::PAYLOAD);

                memcpy(tuples[high].key, tuples[index].key, Constants::KEY_SIZE);
                // memcpy(tuples[high].payload, tuples[index].payload, Constants::PAYLOAD);

                memcpy(tuples[index].key, tmp, Constants::KEY_SIZE);
                // memcpy(tuples[index].payload, tmp_payload, Constants::PAYLOAD);

                high--;


            }
            else
                index++;
        }

        thread_pool->add_task(new RadixQuickSortTask(tuples, lo, low -1, digit, thread_pool));
        if(digit < Constants::KEY_SIZE)
            thread_pool->add_task( new RadixQuickSortTask(tuples, low, high, digit + 1, thread_pool));

        thread_pool->add_task(new RadixQuickSortTask(tuples, high + 1, hi, digit, thread_pool));
    }
};

#endif
