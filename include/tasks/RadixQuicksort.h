#ifndef RADIXQUICKSORT_H
#define RADIXQUICKSORT_H

#include "ThreadPool.h"
#include <cstdint>
#include "Constants.h"
#include "utils.h"

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

        int32_t low = lo;
        int32_t high = hi;
        uint32_t char_index = tuples[low].key[digit];
        int32_t index = low + 1;
        uint32_t  test_char;
        // cout << char_index << endl;
        while(index <= high){

            test_char = tuples[index].key[digit];
            if(test_char < char_index){
                swap(tuples[index].key, tuples[low].key, tuples[index].rowID, tuples[low].rowID);
                low++;
                index++;

            }
            else if(test_char > char_index){
                swap(tuples[index].key, tuples[high].key, tuples[index].rowID, tuples[high].rowID);
                high--;
            }
            else
                index++;
        }


//         thread_pool->add_task(new RadixQuickSortTask(tuples, lo, low -1, digit, thread_pool));
        if(digit < Constants::KEY_SIZE)
            thread_pool->add_task( new RadixQuickSortTask(tuples, low, high, digit + 1, thread_pool));

        thread_pool->add_task(new RadixQuickSortTask(tuples, high + 1, hi, digit, thread_pool));

        RadixQuicksort(tuples, lo, low -1, digit);

    }
};

#endif
