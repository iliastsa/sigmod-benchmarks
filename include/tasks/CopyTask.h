#ifndef SIGMODBENCHMARKS_COPYTASK_H
#define SIGMODBENCHMARKS_COPYTASK_H

#include <ThreadPool.h>
#include <cstdint>
#include <Utils.h>
#include <cstring>

class CopyTask : public ThreadPool::Task {

public:
    unsigned char* src;
    unsigned char* dest;
    uint64_t byte_amt;

    CopyTask(unsigned char* src, unsigned char* dest, uint64_t byte_amt) : src(src), dest(dest), byte_amt(byte_amt) {}

    void run() override {
        memcpy(dest, src, byte_amt);
    }
};

#endif
