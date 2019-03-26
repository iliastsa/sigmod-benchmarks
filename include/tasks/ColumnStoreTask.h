#ifndef SIGMODBENCHMARKS_COLUMNSTORETASK_H
#define SIGMODBENCHMARKS_COLUMNSTORETASK_H


#include <ThreadPool.h>
#include <ReadReader.h>
#include <Tuple.h>
#include <Constants.h>
#include <cstring>


class ColumnStoreTask : public ThreadPool::Task {
private:
    ReadReader reader;
    Tuple* tuples;
    uint32_t offset;

    void toColumnStore(const unsigned char* mem, uint64_t sz) {
        for (uint64_t i = 0; i < sz; i += Constants::TUPLE_SIZE) {
            memcpy(&(tuples[offset].key), &mem[i], Constants::KEY_SIZE);
            tuples[offset].rowID = offset;
            offset++;
        }
    }
public:
    ColumnStoreTask(Tuple* tuples, int fd, uint64_t from, uint64_t to, uint64_t chunk_size)
            : reader(fd, from, to, chunk_size), tuples(tuples), offset(from/Constants::TUPLE_SIZE) {}

    void run() final {
        uint64_t sz;
        unsigned char* mem;

        while ((mem = reader.next(&sz)) != nullptr)
            toColumnStore(mem, sz);
    }

    ~ColumnStoreTask() final = default;
};

#endif
