
#ifndef SIGMODBENCHMARKS_BORROWEDSORT_H
#define SIGMODBENCHMARKS_BORROWEDSORT_H
//  Sort an array using in parallel using multiple threads
//  Copyright (C) 2016 Amir Baserinia (baserinia.github.io)

//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.

//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.

//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

//  To use this program, simply include it in your project and call
//  the function "parasort(size, list, np, sf)".
//      size: size of array
//      list: array pointer
//      np: number of threads
//      sf: sampling factor

#include <cstdlib>      // rand
#include <cstdint>
#include <cstring>      // memcpy
#include <vector>       // std::vector
#include <algorithm>    // std::sort
#include <thread>       // std::thread

//------------------------------------------------------------------------------
// Sort an array using std::sort
// sz: length of array
// list: array pointer
template <typename T> static void _sort (uint64_t sz, T* list)
{
    std::sort(list, list+sz);
}

//------------------------------------------------------------------------------
// Count the number of elements in each bucket
// sz: length of array
// list: array pointer
// np: number of processors (threads)
// range: range of buckets
// bucket: number of items in each bucket (return value)
template <typename T> static void _count (uint64_t sz, const T* list,
                                          uint64_t np, const T* range, uint64_t* bucket)
{
    for ( uint64_t n = 0; n < np; n++ ) bucket[n] = 0;
    for ( uint64_t i = 0; i < sz; i++ ) {
        uint64_t n;
        for ( n = 0; n < np-1; n++ ) {
            if ( list[i] < range[n] ) {
                bucket[n]++;
                break;
            }
        }
        if (n == np-1) bucket[np-1]++;
    }
}

//------------------------------------------------------------------------------
// Reorder array to separate buckets
// sz: length of array
// list: array pointer
// np: number of processors (threads)
// range: range of buckets
// map: reordering map
// out: reordered array (return value)
template <typename T> static void _reorder (uint64_t sz, const T* list,
                                            uint64_t np, const T* range, uint64_t* map, T* out)
{
    for ( uint64_t i = 0; i < sz; i++ ) {
        uint64_t n;
        for ( n = 0; n < np-1; n++ ) {
            if ( list[i] < range[n] ) {
                out[map[n]++] = list[i];
                break;
            }
        }
        if ( n == np-1 ) out[map[np-1]++] = list[i];
    }
}

//------------------------------------------------------------------------------
// Parallel sort
// sz: length of array
// list: array pointer
// np: number of processors (threads)
// sf: sampling factor (100 should be good enough for most cases)
template <typename T> void parasort(uint64_t sz, T* list, uint64_t np = 2, uint64_t sf = 100)
{
    uint64_t nSample = np * sf;
    uint64_t nMap = np * np;
    std::vector<T> range(np);
    std::vector<T> sample(nSample);
    std::vector<uint64_t> tmp(nMap, 0), map(nMap, 0), bucket(nMap, 0);
    std::vector<std::thread> threads(np);
// Step 1: Pick np*sf random elements from the input array.
// We use these elements to split the array into even partitions.
    for (uint64_t i = 0; i < nSample; i++) sample[i] = list[rand()%sz];
    _sort(nSample, &sample[0]);
    for (uint64_t i = 0; i < np-1; i++) range[i] = sample[(i+1)*sf];
// Step 2: Count how many elements go into each bucket.
// This is done in parallel
    uint64_t part = sz / np;
    for (uint64_t i = 0; i < np; i++) {
        uint64_t start = i * part;
        uint64_t length = ( i+1 == np ) ? sz - i * part : part;
        threads[i] = std::thread( _count<T>, length, &list[start], np, &range[0], &bucket[i*np] );
    }
    for (auto&& thread : threads) thread.join();
// Step 3: Create a mapping scheme to separate partitions.
// Since the number of operations is small, we do this step in serial.
    for (uint64_t i = 0; i < nMap; i++) tmp[i] = i ? tmp[i-1] + bucket[((i-1)%np)*np+(i-1)/np] : 0;
    for (uint64_t i = 0; i < nMap; i++) map[i] = tmp[(i%np)*np+i/np];
    for (uint64_t i = 0; i < nMap; i++) tmp[i] = map[i];
// Step 4: Map the input array into np separate partitions.
    std::vector<T> sorted(sz);
    for (uint64_t i = 0; i < np; i++) {
        uint64_t start = i * part;
        uint64_t length = (i+1 == np) ? sz - i * part : part;
        threads[i] = std::thread( _reorder<T>, length, &list[start], np, &range[0], &tmp[i*np], &sorted[0] );
    }
    for (auto&& thread : threads) thread.join();
// Step 5: Sort each partition in a separate thread.
    for (uint64_t i = 0; i < np; i++) {
        uint64_t start = map[i];
        uint64_t length = (i+1 == np) ? sz - map[i] : map[i+1] - map[i];
        threads[i] = std::thread( _sort<T>, length, &sorted[start] );
    }
    for (auto&& thread : threads) thread.join();
// Step 6: Copy the temporary container for the sorted array into the input array.
    std::memcpy(list, &sorted[0], sz * sizeof(T) );
}


#endif
